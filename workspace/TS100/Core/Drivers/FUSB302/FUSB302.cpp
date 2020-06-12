/*
 FUSB302.c - Library for interacting with the FUSB302B chip.
 Copyright 2015 The Chromium OS Authors
 Copyright 2017 Jason Cerundolo
 Released under an MIT license. See LICENSE file.
 */

#include "FUSB302.h"
#include "USBC_TCPM/usb_pd_tcpm.h"
#include "USBC_TCPM/tcpm.h"
#include "USBC_PD/usb_pd.h"
#include <string.h>
#include "cmsis_os.h"
#include "I2C_Wrapper.hpp"
#define PACKET_IS_GOOD_CRC(head) (PD_HEADER_TYPE(head) == PD_CTRL_GOOD_CRC && \
				 PD_HEADER_CNT(head) == 0)
const struct tcpc_config_t tcpc_config = {
fusb302_I2C_SLAVE_ADDR, &fusb302_tcpm_drv };
static struct fusb302_chip_state {
	int cc_polarity;
	int vconn_enabled;
	/* 1 = pulling up (DFP) 0 = pulling down (UFP) */
	int pulling_up;
	int rx_enable;
	uint8_t mdac_vnc;
	uint8_t mdac_rd;
} state;

/*
 * Bring the FUSB302 out of reset after Hard Reset signaling. This will
 * automatically flush both the Rx and Tx FIFOs.
 */
static void fusb302_pd_reset() {

	tcpc_write( TCPC_REG_RESET, TCPC_REG_RESET_PD_RESET);

}

/*
 * Flush our Rx FIFO. To prevent packet framing issues, this function should
 * only be called when Rx is disabled.
 */
static void fusb302_flush_rx_fifo() {
	/*
	 * other bits in the register _should_ be 0
	 * until the day we support other SOP* types...
	 * then we'll have to keep a shadow of what this register
	 * value should be so we don't clobber it here!
	 */

	tcpc_write( TCPC_REG_CONTROL1, TCPC_REG_CONTROL1_RX_FLUSH);

}

static void fusb302_flush_tx_fifo() {
	int reg;

	tcpc_read( TCPC_REG_CONTROL0, &reg);
	reg |= TCPC_REG_CONTROL0_TX_FLUSH;
	tcpc_write( TCPC_REG_CONTROL0, reg);

}

static void fusb302_auto_goodcrc_enable(int enable) {
	int reg;

	tcpc_read( TCPC_REG_SWITCHES1, &reg);

	if (enable)
		reg |= TCPC_REG_SWITCHES1_AUTO_GCRC;
	else
		reg &= ~TCPC_REG_SWITCHES1_AUTO_GCRC;

	tcpc_write( TCPC_REG_SWITCHES1, reg);

}

/* Convert BC LVL values (in FUSB302) to Type-C CC Voltage Status */
static int convert_bc_lvl(int bc_lvl) {
	/* assume OPEN unless one of the following conditions is true... */
	int ret = TYPEC_CC_VOLT_OPEN;

	if (state.pulling_up) {
		if (bc_lvl == 0x00)
			ret = TYPEC_CC_VOLT_RA;
		else if (bc_lvl < 0x3)
			ret = TYPEC_CC_VOLT_RD;
	} else {
		if (bc_lvl == 0x1)
			ret = TYPEC_CC_VOLT_SNK_DEF;
		else if (bc_lvl == 0x2)
			ret = TYPEC_CC_VOLT_SNK_1_5;
		else if (bc_lvl == 0x3)
			ret = TYPEC_CC_VOLT_SNK_3_0;
	}

	return ret;
}

static int measure_cc_pin_source(int cc_measure) {
	int switches0_reg;
	int reg;
	int cc_lvl;

	/* Read status register */
	tcpc_read( TCPC_REG_SWITCHES0, &reg);
	/* Save current value */
	switches0_reg = reg;
	/* Clear pull-up register settings and measure bits */
	reg &= ~(TCPC_REG_SWITCHES0_MEAS_CC1 | TCPC_REG_SWITCHES0_MEAS_CC2);
	/* Set desired pullup register bit */
	if (cc_measure == TCPC_REG_SWITCHES0_MEAS_CC1)
		reg |= TCPC_REG_SWITCHES0_CC1_PU_EN;
	else
		reg |= TCPC_REG_SWITCHES0_CC2_PU_EN;
	/* Set CC measure bit */
	reg |= cc_measure;

	/* Set measurement switch */
	tcpc_write( TCPC_REG_SWITCHES0, reg);

	/* Set MDAC for Open vs Rd/Ra comparison */
	tcpc_write( TCPC_REG_MEASURE, state.mdac_vnc);

	/* Wait on measurement */
	usleep(250);

	/* Read status register */
	tcpc_read( TCPC_REG_STATUS0, &reg);

	/* Assume open */
	cc_lvl = TYPEC_CC_VOLT_OPEN;

	/* CC level is below the 'no connect' threshold (vOpen) */
	if ((reg & TCPC_REG_STATUS0_COMP) == 0) {
		/* Set MDAC for Rd vs Ra comparison */
		tcpc_write( TCPC_REG_MEASURE, state.mdac_rd);

		/* Wait on measurement */
		usleep(250);

		/* Read status register */
		tcpc_read( TCPC_REG_STATUS0, &reg);

		cc_lvl =
				(reg & TCPC_REG_STATUS0_COMP) ?
						TYPEC_CC_VOLT_RD : TYPEC_CC_VOLT_RA;
	}

	/* Restore SWITCHES0 register to its value prior */
	tcpc_write( TCPC_REG_SWITCHES0, switches0_reg);

	return cc_lvl;
}

/* Determine cc pin state for source when in manual detect mode */
static void detect_cc_pin_source_manual(int *cc1_lvl, int *cc2_lvl) {
	int cc1_measure = TCPC_REG_SWITCHES0_MEAS_CC1;
	int cc2_measure = TCPC_REG_SWITCHES0_MEAS_CC2;

	if (state.vconn_enabled) {
		/* If VCONN enabled, measure cc_pin that matches polarity */
		if (state.cc_polarity)
			*cc2_lvl = measure_cc_pin_source(cc2_measure);
		else
			*cc1_lvl = measure_cc_pin_source(cc1_measure);
	} else {
		/* If VCONN not enabled, measure both cc1 and cc2 */
		*cc1_lvl = measure_cc_pin_source(cc1_measure);
		*cc2_lvl = measure_cc_pin_source(cc2_measure);
	}

}

/* Determine cc pin state for sink */
static void detect_cc_pin_sink(int *cc1, int *cc2) {
	int reg;
	int orig_meas_cc1;
	int orig_meas_cc2;
	int bc_lvl_cc1;
	int bc_lvl_cc2;

	/*
	 * Measure CC1 first.
	 */
	tcpc_read( TCPC_REG_SWITCHES0, &reg);

	/* save original state to be returned to later... */
	if (reg & TCPC_REG_SWITCHES0_MEAS_CC1)
		orig_meas_cc1 = 1;
	else
		orig_meas_cc1 = 0;

	if (reg & TCPC_REG_SWITCHES0_MEAS_CC2)
		orig_meas_cc2 = 1;
	else
		orig_meas_cc2 = 0;

	/* Disable CC2 measurement switch, enable CC1 measurement switch */
	reg &= ~TCPC_REG_SWITCHES0_MEAS_CC2;
	reg |= TCPC_REG_SWITCHES0_MEAS_CC1;

	tcpc_write( TCPC_REG_SWITCHES0, reg);

	/* CC1 is now being measured by FUSB302. */

	/* Wait on measurement */
	usleep(250);

	tcpc_read( TCPC_REG_STATUS0, &bc_lvl_cc1);

	/* mask away unwanted bits */
	bc_lvl_cc1 &= (TCPC_REG_STATUS0_BC_LVL0 | TCPC_REG_STATUS0_BC_LVL1);

	/*
	 * Measure CC2 next.
	 */

	tcpc_read( TCPC_REG_SWITCHES0, &reg);

	/* Disable CC1 measurement switch, enable CC2 measurement switch */
	reg &= ~TCPC_REG_SWITCHES0_MEAS_CC1;
	reg |= TCPC_REG_SWITCHES0_MEAS_CC2;

	tcpc_write( TCPC_REG_SWITCHES0, reg);

	/* CC2 is now being measured by FUSB302. */

	/* Wait on measurement */
	usleep(250);

	tcpc_read( TCPC_REG_STATUS0, &bc_lvl_cc2);

	/* mask away unwanted bits */
	bc_lvl_cc2 &= (TCPC_REG_STATUS0_BC_LVL0 | TCPC_REG_STATUS0_BC_LVL1);

	*cc1 = convert_bc_lvl(bc_lvl_cc1);
	*cc2 = convert_bc_lvl(bc_lvl_cc2);

	/* return MEAS_CC1/2 switches to original state */
	tcpc_read( TCPC_REG_SWITCHES0, &reg);
	if (orig_meas_cc1)
		reg |= TCPC_REG_SWITCHES0_MEAS_CC1;
	else
		reg &= ~TCPC_REG_SWITCHES0_MEAS_CC1;
	if (orig_meas_cc2)
		reg |= TCPC_REG_SWITCHES0_MEAS_CC2;
	else
		reg &= ~TCPC_REG_SWITCHES0_MEAS_CC2;

	tcpc_write( TCPC_REG_SWITCHES0, reg);

}

/* Parse header bytes for the size of packet */
static int get_num_bytes(uint16_t header) {
	int rv;

	/* Grab the Number of Data Objects field.*/
	rv = PD_HEADER_CNT(header);

	/* Multiply by four to go from 32-bit words -> bytes */
	rv *= 4;

	/* Plus 2 for header */
	rv += 2;

	return rv;
}

static int fusb302_send_message(uint16_t header, const uint32_t *data,
		uint8_t *buf, int buf_pos) {
	int rv;
	int reg;
	int len;

	len = get_num_bytes(header);

	/*
	 * packsym tells the TXFIFO that the next X bytes are payload,
	 * and should not be interpreted as special tokens.
	 * The 5 LSBs represent X, the number of bytes.
	 */
	reg = fusb302_TKN_PACKSYM;
	reg |= (len & 0x1F);

	buf[buf_pos++] = reg;

	/* write in the header */
	reg = header;
	buf[buf_pos++] = reg & 0xFF;

	reg >>= 8;
	buf[buf_pos++] = reg & 0xFF;

	/* header is done, subtract from length to make this for-loop simpler */
	len -= 2;

	/* write data objects, if present */
	memcpy(&buf[buf_pos], data, len);
	buf_pos += len;

	/* put in the CRC */
	buf[buf_pos++] = fusb302_TKN_JAMCRC;

	/* put in EOP */
	buf[buf_pos++] = fusb302_TKN_EOP;

	/* Turn transmitter off after sending message */
	buf[buf_pos++] = fusb302_TKN_TXOFF;

	/* Start transmission */
	reg = fusb302_TKN_TXON;
	buf[buf_pos++] = fusb302_TKN_TXON;

	/* burst write for speed! */

	rv = tcpc_xfer(buf, buf_pos, 0, 0, I2C_XFER_SINGLE);

	return rv;
}

static int fusb302_tcpm_select_rp_value(int rp) {
	int reg;
	int rv;
	uint8_t vnc, rd;

	rv = tcpc_read( TCPC_REG_CONTROL0, &reg);
	if (rv)
		return rv;

	/* Set the current source for Rp value */
	reg &= ~TCPC_REG_CONTROL0_HOST_CUR_MASK;
	switch (rp) {
	case TYPEC_RP_1A5:
		reg |= TCPC_REG_CONTROL0_HOST_CUR_1A5;
		vnc = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_1_5_VNC_MV);
		rd = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_1_5_RD_THRESH_MV);
		break;
	case TYPEC_RP_3A0:
		reg |= TCPC_REG_CONTROL0_HOST_CUR_3A0;
		vnc = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_3_0_VNC_MV);
		rd = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_3_0_RD_THRESH_MV);
		break;
	case TYPEC_RP_USB:
	default:
		reg |= TCPC_REG_CONTROL0_HOST_CUR_USB;
		vnc = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_DEF_VNC_MV);
		rd = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_DEF_RD_THRESH_MV);
	}
	state.mdac_vnc = vnc;
	state.mdac_rd = rd;
	rv = tcpc_write( TCPC_REG_CONTROL0, reg);

	return rv;
}

static int fusb302_tcpm_init() {
	int reg;

	/* set default */
	state.cc_polarity = -1;

	/* set the voltage threshold for no connect detection (vOpen) */
	state.mdac_vnc = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_DEF_VNC_MV);
	/* set the voltage threshold for Rd vs Ra detection */
	state.mdac_rd = TCPC_REG_MEASURE_MDAC_MV(PD_SRC_DEF_RD_THRESH_MV);

	/* all other variables assumed to default to 0 */

	/* Restore default settings */
	tcpc_write( TCPC_REG_RESET, TCPC_REG_RESET_SW_RESET);

	/* Turn on retries and set number of retries */
	tcpc_read( TCPC_REG_CONTROL3, &reg);
	reg |= TCPC_REG_CONTROL3_AUTO_RETRY;
	reg |= (PD_RETRY_COUNT & 0x3) <<
	TCPC_REG_CONTROL3_N_RETRIES_POS;
	tcpc_write( TCPC_REG_CONTROL3, reg);

	/* Create interrupt masks */
	reg = 0xFF;
	/* CC level changes */
	reg &= ~TCPC_REG_MASK_BC_LVL;
	/* collisions */
	reg &= ~TCPC_REG_MASK_COLLISION;
	/* misc alert */
	reg &= ~TCPC_REG_MASK_ALERT;
	/* packet received with correct CRC */
	reg &= ~TCPC_REG_MASK_CRC_CHK;
	tcpc_write( TCPC_REG_MASK, reg);

	reg = 0xFF;
	/* when all pd message retries fail... */
	reg &= ~TCPC_REG_MASKA_RETRYFAIL;
	/* when fusb302 send a hard reset. */
	reg &= ~TCPC_REG_MASKA_HARDSENT;
	/* when fusb302 receives GoodCRC ack for a pd message */
	reg &= ~TCPC_REG_MASKA_TX_SUCCESS;
	/* when fusb302 receives a hard reset */
	reg &= ~TCPC_REG_MASKA_HARDRESET;
	tcpc_write( TCPC_REG_MASKA, reg);

	reg = 0xFF;
	/* when fusb302 sends GoodCRC to ack a pd message */
	reg &= ~TCPC_REG_MASKB_GCRCSENT;
	tcpc_write( TCPC_REG_MASKB, reg);

	/* Interrupt Enable */
	tcpc_read( TCPC_REG_CONTROL0, &reg);
	reg &= ~TCPC_REG_CONTROL0_INT_MASK;
	tcpc_write( TCPC_REG_CONTROL0, reg);

	/* Set VCONN switch defaults */
	tcpm_set_polarity(0);
	tcpm_set_vconn(0);

	/* Turn on the power! */
	/* TODO: Reduce power consumption */
	tcpc_write( TCPC_REG_POWER, TCPC_REG_POWER_PWR_ALL);

	return 0;
}

static int fusb302_tcpm_release() {
	return EC_ERROR_UNIMPLEMENTED;
}

static int fusb302_tcpm_get_cc(int *cc1, int *cc2) {
	if (state.pulling_up) {
		/* Source mode? */
		detect_cc_pin_source_manual(cc1, cc2);
	} else {
		/* Sink mode? */
		detect_cc_pin_sink(cc1, cc2);
	}

	return 0;
}

static int fusb302_tcpm_set_cc(int pull) {
	int reg;

	/* NOTE: FUSB302 toggles a single pull-up between CC1 and CC2 */
	/* NOTE: FUSB302 Does not support Ra. */
	switch (pull) {
	case TYPEC_CC_RP:
		/* enable the pull-up we know to be necessary */
		tcpc_read( TCPC_REG_SWITCHES0, &reg);

		reg &= ~(TCPC_REG_SWITCHES0_CC2_PU_EN |
		TCPC_REG_SWITCHES0_CC1_PU_EN |
		TCPC_REG_SWITCHES0_CC1_PD_EN |
		TCPC_REG_SWITCHES0_CC2_PD_EN |
		TCPC_REG_SWITCHES0_VCONN_CC1 |
		TCPC_REG_SWITCHES0_VCONN_CC2);

		reg |= TCPC_REG_SWITCHES0_CC1_PU_EN |
		TCPC_REG_SWITCHES0_CC2_PU_EN;

		if (state.vconn_enabled)
			reg |= state.cc_polarity ?
			TCPC_REG_SWITCHES0_VCONN_CC1 :
										TCPC_REG_SWITCHES0_VCONN_CC2;

		tcpc_write( TCPC_REG_SWITCHES0, reg);

		state.pulling_up = 1;
		break;
	case TYPEC_CC_RD:
		/* Enable UFP Mode */

		/* turn off toggle */
		tcpc_read( TCPC_REG_CONTROL2, &reg);
		reg &= ~TCPC_REG_CONTROL2_TOGGLE;
		tcpc_write( TCPC_REG_CONTROL2, reg);

		/* enable pull-downs, disable pullups */
		tcpc_read( TCPC_REG_SWITCHES0, &reg);

		reg &= ~(TCPC_REG_SWITCHES0_CC2_PU_EN);
		reg &= ~(TCPC_REG_SWITCHES0_CC1_PU_EN);
		reg |= (TCPC_REG_SWITCHES0_CC1_PD_EN);
		reg |= (TCPC_REG_SWITCHES0_CC2_PD_EN);
		tcpc_write( TCPC_REG_SWITCHES0, reg);

		state.pulling_up = 0;
		break;
	case TYPEC_CC_OPEN:
		/* Disable toggling */
		tcpc_read( TCPC_REG_CONTROL2, &reg);
		reg &= ~TCPC_REG_CONTROL2_TOGGLE;
		tcpc_write( TCPC_REG_CONTROL2, reg);

		/* Ensure manual switches are opened */
		tcpc_read( TCPC_REG_SWITCHES0, &reg);
		reg &= ~TCPC_REG_SWITCHES0_CC1_PU_EN;
		reg &= ~TCPC_REG_SWITCHES0_CC2_PU_EN;
		reg &= ~TCPC_REG_SWITCHES0_CC1_PD_EN;
		reg &= ~TCPC_REG_SWITCHES0_CC2_PD_EN;
		tcpc_write( TCPC_REG_SWITCHES0, reg);

		state.pulling_up = 0;
		break;
	default:
		/* Unsupported... */
		return EC_ERROR_UNIMPLEMENTED;
	}

	return 0;
}

static int fusb302_tcpm_set_polarity(int polarity) {
	/* Port polarity : 0 => CC1 is CC line, 1 => CC2 is CC line */
	int reg;

	tcpc_read( TCPC_REG_SWITCHES0, &reg);

	/* clear VCONN switch bits */
	reg &= ~TCPC_REG_SWITCHES0_VCONN_CC1;
	reg &= ~TCPC_REG_SWITCHES0_VCONN_CC2;

	if (state.vconn_enabled) {
		/* set VCONN switch to be non-CC line */
		if (polarity)
			reg |= TCPC_REG_SWITCHES0_VCONN_CC1;
		else
			reg |= TCPC_REG_SWITCHES0_VCONN_CC2;
	}

	/* clear meas_cc bits (RX line select) */
	reg &= ~TCPC_REG_SWITCHES0_MEAS_CC1;
	reg &= ~TCPC_REG_SWITCHES0_MEAS_CC2;

	/* set rx polarity */
	if (polarity)
		reg |= TCPC_REG_SWITCHES0_MEAS_CC2;
	else
		reg |= TCPC_REG_SWITCHES0_MEAS_CC1;

	tcpc_write( TCPC_REG_SWITCHES0, reg);

	tcpc_read( TCPC_REG_SWITCHES1, &reg);

	/* clear tx_cc bits */
	reg &= ~TCPC_REG_SWITCHES1_TXCC1_EN;
	reg &= ~TCPC_REG_SWITCHES1_TXCC2_EN;

	/* set tx polarity */
	if (polarity)
		reg |= TCPC_REG_SWITCHES1_TXCC2_EN;
	else
		reg |= TCPC_REG_SWITCHES1_TXCC1_EN;

	tcpc_write( TCPC_REG_SWITCHES1, reg);

	/* Save the polarity for later */
	state.cc_polarity = polarity;

	return 0;
}

static int fusb302_tcpm_set_vconn(int enable) {
	/*
	 * FUSB302 does not have dedicated VCONN Enable switch.
	 * We'll get through this by disabling both of the
	 * VCONN - CC* switches to disable, and enabling the
	 * saved polarity when enabling.
	 * Therefore at startup, tcpm_set_polarity should be called first,
	 * or else live with the default put into tcpm_init.
	 */
	int reg;

	/* save enable state for later use */
	state.vconn_enabled = enable;

	if (enable) {
		/* set to saved polarity */
		tcpm_set_polarity(state.cc_polarity);
	} else {

		tcpc_read( TCPC_REG_SWITCHES0, &reg);

		/* clear VCONN switch bits */
		reg &= ~TCPC_REG_SWITCHES0_VCONN_CC1;
		reg &= ~TCPC_REG_SWITCHES0_VCONN_CC2;

		tcpc_write( TCPC_REG_SWITCHES0, reg);

	}

	return 0;
}

static int fusb302_tcpm_set_msg_header(int power_role, int data_role) {
	int reg;

	tcpc_read( TCPC_REG_SWITCHES1, &reg);

	reg &= ~TCPC_REG_SWITCHES1_POWERROLE;
	reg &= ~TCPC_REG_SWITCHES1_DATAROLE;

	if (power_role)
		reg |= TCPC_REG_SWITCHES1_POWERROLE;
	if (data_role)
		reg |= TCPC_REG_SWITCHES1_DATAROLE;

	tcpc_write( TCPC_REG_SWITCHES1, reg);

	return 0;
}

static int fusb302_tcpm_set_rx_enable(int enable) {
	int reg;

	state.rx_enable = enable;

	/* Get current switch state */
	tcpc_read( TCPC_REG_SWITCHES0, &reg);

	/* Clear CC1/CC2 measure bits */
	reg &= ~TCPC_REG_SWITCHES0_MEAS_CC1;
	reg &= ~TCPC_REG_SWITCHES0_MEAS_CC2;

	if (enable) {
		switch (state.cc_polarity) {
		/* if CC polarity hasnt been determined, can't enable */
		case -1:
			return EC_ERROR_UNKNOWN;
		case 0:
			reg |= TCPC_REG_SWITCHES0_MEAS_CC1;
			break;
		case 1:
			reg |= TCPC_REG_SWITCHES0_MEAS_CC2;
			break;
		default:
			/* "shouldn't get here" */
			return EC_ERROR_UNKNOWN;
		}
		tcpc_write( TCPC_REG_SWITCHES0, reg);

		/* Disable BC_LVL interrupt when enabling PD comm */
		if (!tcpc_read( TCPC_REG_MASK, &reg))
			tcpc_write( TCPC_REG_MASK, reg | TCPC_REG_MASK_BC_LVL);

		/* flush rx fifo in case messages have been coming our way */
		fusb302_flush_rx_fifo();

	} else {
		tcpc_write( TCPC_REG_SWITCHES0, reg);

		/* Enable BC_LVL interrupt when disabling PD comm */
		if (!tcpc_read( TCPC_REG_MASK, &reg))
			tcpc_write( TCPC_REG_MASK, reg & ~TCPC_REG_MASK_BC_LVL);
	}

	fusb302_auto_goodcrc_enable(enable);

	return 0;
}

/* Return true if our Rx FIFO is empty */
static int fusb302_rx_fifo_is_empty() {
	int reg, ret;

	ret = (!tcpc_read( TCPC_REG_STATUS1, &reg))
			&& (reg & TCPC_REG_STATUS1_RX_EMPTY);

	return ret;
}

static int fusb302_tcpm_get_message(uint32_t *payload, int *head) {
	/*
	 * This is the buffer that will get the burst-read data
	 * from the fusb302.
	 *
	 * It's re-used in a couple different spots, the worst of which
	 * is the PD packet (not header) and CRC.
	 * maximum size necessary = 28 + 4 = 32
	 */
	uint8_t buf[32];
	int rv, len;

	/* If our FIFO is empty then we have no packet */
	if (fusb302_rx_fifo_is_empty())
		return EC_ERROR_UNKNOWN;

	/* Read until we have a non-GoodCRC packet or an empty FIFO */
	do {
		buf[0] = TCPC_REG_FIFOS;

		/*
		 * PART 1 OF BURST READ: Write in register address.
		 * Issue a START, no STOP.
		 */
		rv = tcpc_xfer(buf, 1, 0, 0, I2C_XFER_START);

		/*
		 * PART 2 OF BURST READ: Read up to the header.
		 * Issue a repeated START, no STOP.
		 * only grab three bytes so we can get the header
		 * and determine how many more bytes we need to read.
		 * TODO: Check token to ensure valid packet.
		 */
		rv |= tcpc_xfer(0, 0, buf, 3, I2C_XFER_START);

		/* Grab the header */
		*head = (buf[1] & 0xFF);
		*head |= ((buf[2] << 8) & 0xFF00);

		/* figure out packet length, subtract header bytes */
		len = get_num_bytes(*head) - 2;

		/*
		 * PART 3 OF BURST READ: Read everything else.
		 * No START, but do issue a STOP at the end.
		 * add 4 to len to read CRC out
		 */
		rv |= tcpc_xfer(0, 0, buf, len + 4, I2C_XFER_STOP);

	} while (!rv && PACKET_IS_GOOD_CRC(*head) && !fusb302_rx_fifo_is_empty());

	if (!rv) {
		/* Discard GoodCRC packets */
		if (PACKET_IS_GOOD_CRC(*head))
			rv = EC_ERROR_UNKNOWN;
		else
			memcpy(payload, buf, len);
	}

	/*
	 * If our FIFO is non-empty then we may have a packet, we may get
	 * fewer interrupts than packets due to interrupt latency.
	 */
	//if (!fusb302_rx_fifo_is_empty(port))
	//	task_set_event(PD_PORT_TO_TASK_ID(port), PD_EVENT_RX, 0);
	return rv;
}

static int fusb302_tcpm_transmit(enum tcpm_transmit_type type, uint16_t header,
		const uint32_t *data) {
	/*
	 * this is the buffer that will be burst-written into the fusb302
	 * maximum size necessary =
	 * 1: FIFO register address
	 * 4: SOP* tokens
	 * 1: Token that signifies "next X bytes are not tokens"
	 * 30: 2 for header and up to 7*4 = 28 for rest of message
	 * 1: "Insert CRC" Token
	 * 1: EOP Token
	 * 1: "Turn transmitter off" token
	 * 1: "Star Transmission" Command
	 * -
	 * 40: 40 bytes worst-case
	 */
	uint8_t buf[40];
	int buf_pos = 0;

	int reg;

	/* Flush the TXFIFO */
	fusb302_flush_tx_fifo ();

	switch (type) {
	case TCPC_TX_SOP:

		/* put register address first for of burst tcpc write */
		buf[buf_pos++] = TCPC_REG_FIFOS;

		/* Write the SOP Ordered Set into TX FIFO */
		buf[buf_pos++] = fusb302_TKN_SYNC1;
		buf[buf_pos++] = fusb302_TKN_SYNC1;
		buf[buf_pos++] = fusb302_TKN_SYNC1;
		buf[buf_pos++] = fusb302_TKN_SYNC2;

		fusb302_send_message(header, data, buf, buf_pos);
// wait for the GoodCRC to come back before we let the rest
// of the code do stuff like change polarity and miss it
//		delay_us(600);
		osDelay(1);
		break;
	case TCPC_TX_HARD_RESET:

		/* Simply hit the SEND_HARD_RESET bit */
		tcpc_read( TCPC_REG_CONTROL3, &reg);
		reg |= TCPC_REG_CONTROL3_SEND_HARDRESET;
		tcpc_write( TCPC_REG_CONTROL3, reg);

		break;
	case TCPC_TX_BIST_MODE_2:

		/* Hit the BIST_MODE2 bit and start TX */
		tcpc_read( TCPC_REG_CONTROL1, &reg);
		reg |= TCPC_REG_CONTROL1_BIST_MODE2;
		tcpc_write( TCPC_REG_CONTROL1, reg);

		tcpc_read( TCPC_REG_CONTROL0, &reg);
		reg |= TCPC_REG_CONTROL0_TX_START;
		tcpc_write( TCPC_REG_CONTROL0, reg);

//task_wait_event(PD_T_BIST_TRANSMIT);

		/* Clear BIST mode bit, TX_START is self-clearing */
		tcpc_read( TCPC_REG_CONTROL1, &reg);
		reg &= ~TCPC_REG_CONTROL1_BIST_MODE2;
		tcpc_write( TCPC_REG_CONTROL1, reg);

		break;
	default:
		return EC_ERROR_UNIMPLEMENTED;
	}

	return 0;
}

#ifdef CONFIG_USB_PD_VBUS_DETECT_TCPC
static int fusb302_tcpm_get_vbus_level()
{
	int reg;

	/* Read status register */

	tcpc_read( TCPC_REG_STATUS0, &reg);

	return (reg & TCPC_REG_STATUS0_VBUSOK) ? 1 : 0;
}
#endif

void fusb302_tcpc_alert() {
	/* interrupt has been received */
	int interrupt;
	int interrupta;
	int interruptb;

	/* reading interrupt registers clears them */

	tcpc_read( TCPC_REG_INTERRUPT, &interrupt);
	tcpc_read( TCPC_REG_INTERRUPTA, &interrupta);
	tcpc_read( TCPC_REG_INTERRUPTB, &interruptb);

	/*
	 * Ignore BC_LVL changes when transmitting / receiving PD,
	 * since CC level will constantly change.
	 */
	if (state.rx_enable)
		interrupt &= ~TCPC_REG_INTERRUPT_BC_LVL;

	if (interrupt & TCPC_REG_INTERRUPT_BC_LVL) {
		/* CC Status change */
//task_set_event(PD_PORT_TO_TASK_ID(port), PD_EVENT_CC, 0);
	}

	if (interrupt & TCPC_REG_INTERRUPT_COLLISION) {
		/* packet sending collided */
		pd_transmit_complete(TCPC_TX_COMPLETE_FAILED);
	}

	/* GoodCRC was received, our FIFO is now non-empty */
	if (interrupta & TCPC_REG_INTERRUPTA_TX_SUCCESS) {
//task_set_event(PD_PORT_TO_TASK_ID(port),
//		PD_EVENT_RX, 0);

		pd_transmit_complete(TCPC_TX_COMPLETE_SUCCESS);
	}

	if (interrupta & TCPC_REG_INTERRUPTA_RETRYFAIL) {
		/* all retries have failed to get a GoodCRC */
		pd_transmit_complete(TCPC_TX_COMPLETE_FAILED);
	}

	if (interrupta & TCPC_REG_INTERRUPTA_HARDSENT) {
		/* hard reset has been sent */

		/* bring FUSB302 out of reset */
		fusb302_pd_reset();

		pd_transmit_complete(TCPC_TX_COMPLETE_SUCCESS);
	}

	if (interrupta & TCPC_REG_INTERRUPTA_HARDRESET) {
		/* hard reset has been received */

		/* bring FUSB302 out of reset */
		fusb302_pd_reset();

		pd_execute_hard_reset ();

//task_wake(PD_PORT_TO_TASK_ID(port));
	}

	if (interruptb & TCPC_REG_INTERRUPTB_GCRCSENT) {
		/* Packet received and GoodCRC sent */
		/* (this interrupt fires after the GoodCRC finishes) */
		if (state.rx_enable) {
			//task_set_event(PD_PORT_TO_TASK_ID(port),
			//		PD_EVENT_RX, 0);
		} else {
			/* flush rx fifo if rx isn't enabled */
			fusb302_flush_rx_fifo();
		}
	}
}

/* For BIST receiving */
void tcpm_set_bist_test_data() {
	int reg;

	/* Read control3 register */
	tcpc_read( TCPC_REG_CONTROL3, &reg);

	/* Set the BIST_TMODE bit (Clears on Hard Reset) */
	reg |= TCPC_REG_CONTROL3_BIST_TMODE;

	/* Write the updated value */
	tcpc_write( TCPC_REG_CONTROL3, reg);

}

const struct tcpm_drv fusb302_tcpm_drv = { &fusb302_tcpm_init, //init
		&fusb302_tcpm_release, //.release
		&fusb302_tcpm_get_cc, //get_cc
		NULL, //get_vbus_level
		&fusb302_tcpm_select_rp_value, //select_rp_value
		&fusb302_tcpm_set_cc, //set_cc
		&fusb302_tcpm_set_polarity, //set_polarity
		&fusb302_tcpm_set_vconn, //set_vconn
		&fusb302_tcpm_set_msg_header, //set_msg_header
		&fusb302_tcpm_set_rx_enable, //set_rx_enable
		&fusb302_tcpm_get_message, //get_message
		&fusb302_tcpm_transmit, //transmit
		&fusb302_tcpc_alert, //tcpc_alert
		NULL, //tcpc_discharge_vbus
		NULL, //get_chip_info
		};

