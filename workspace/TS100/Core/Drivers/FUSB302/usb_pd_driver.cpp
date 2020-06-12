/*
 * usb_pd_driver.c
 *
 * Created: 11/11/2017 23:55:12
 *  Author: jason
 */

#include "usb_pd_driver.h"
#include "USBC_PD/usb_pd.h"
#include <string.h>
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(t) (sizeof(t) / sizeof(t[0]))
#endif

extern struct tc_module tc_instance;
extern uint32_t g_us_timestamp_upper_32bit;

uint32_t pd_task_set_event(uint32_t event, int wait_for_reply) {
	switch (event) {
	case PD_EVENT_TX:
		break;
	default:
		break;
	}
	return 0;
}

const uint32_t pd_src_pdo[] = { PDO_FIXED(5000, 1500, PDO_FIXED_FLAGS), };

const int pd_src_pdo_cnt = ARRAY_SIZE(pd_src_pdo);

const uint32_t pd_snk_pdo[] = { PDO_FIXED(5000, 1000, PDO_FIXED_FLAGS),
		PDO_FIXED(9000, 1500, PDO_FIXED_FLAGS), PDO_FIXED(12000, 3500,
				PDO_FIXED_FLAGS), };
const int pd_snk_pdo_cnt = ARRAY_SIZE(pd_snk_pdo);

void pd_set_input_current_limit(uint32_t max_ma, uint32_t supply_voltage) {

}

int pd_is_valid_input_voltage(int mv) {
	return 1;
}

int pd_snk_is_vbus_provided() {
	return 1;
}

timestamp_t get_time(void) {
	timestamp_t t;
	//TODO
//
//	system_interrupt_enter_critical_section();
//	t.le.lo = tc_get_count_value(&tc_instance);
//	t.le.hi = g_us_timestamp_upper_32bit;
//	system_interrupt_leave_critical_section();

	return t;
}

void pd_power_supply_reset() {
	return;
}

void pd_execute_data_swap(int data_role) {
	/* Do nothing */
}

int pd_check_data_swap(int data_role) {
	// Never allow data swap
	return 0;
}

int pd_check_power_swap() {
	/* Always refuse power swap */
	return 0;
}

int pd_board_checks(void) {
	return EC_SUCCESS;
}

int pd_set_power_supply_ready() {
//Tells other device if we can supply power
	return EC_SUCCESS; /* we are ready */
}

void pd_transition_voltage(int idx) {
	/* No-operation: we are always 5V */

#if 0
	timestamp_t deadline;
	uint32_t mv = src_pdo_charge[idx - 1].mv;

	/* Is this a transition to a new voltage? */
	if (charge_port_is_active() && vbus[CHG].mv != mv) {
		/*
		 * Alter voltage limit on charge port, this should cause
		 * the port to select the desired PDO.
		 */
		pd_set_external_voltage_limit(CHG, mv);

		/* Wait for CHG transition */
		deadline.val = get_time().val + PD_T_PS_TRANSITION;
		CPRINTS("Waiting for CHG port transition");
		while (charge_port_is_active() &&
		       vbus[CHG].mv != mv &&
		       get_time().val < deadline.val)
			msleep(10);

		if (vbus[CHG].mv != mv) {
			CPRINTS("Missed CHG transition, resetting DUT");
			pd_power_supply_reset(DUT);
			return;
		}

		CPRINTS("CHG transitioned");
	}

	vbus[DUT].mv = vbus[CHG].mv;
	vbus[DUT].ma = vbus[CHG].ma;
#endif // if 0

}

void pd_check_dr_role(int dr_role, int flags) {
#if 0
	/* If UFP, try to switch to DFP */
	if ((flags & PD_FLAGS_PARTNER_DR_DATA) && dr_role == PD_ROLE_UFP)
	pd_request_data_swap(port);
#endif
}

void pd_check_pr_role(int pr_role, int flags) {
#if 0
	/*
	 * If partner is dual-role power and dualrole toggling is on, consider
	 * if a power swap is necessary.
	 */
	if ((flags & PD_FLAGS_PARTNER_DR_POWER) &&
	    pd_get_dual_role() == PD_DRP_TOGGLE_ON) {
		/*
		 * If we are a sink and partner is not externally powered, then
		 * swap to become a source. If we are source and partner is
		 * externally powered, swap to become a sink.
		 */
		int partner_extpower = flags & PD_FLAGS_PARTNER_EXTPOWER;

		if ((!partner_extpower && pr_role == PD_ROLE_SINK) ||
		     (partner_extpower && pr_role == PD_ROLE_SOURCE))
			pd_request_power_swap(port);
	}
#endif // if 0
}

void pd_process_source_cap_callback(int cnt, uint32_t *src_caps) {
	char str[256];
	int i;
	uint32_t ma, mv, pdo;

	for (i = 0; i < cnt; i++) {
		pd_extract_pdo_power(src_caps[i], &ma, &mv);
		//Charger can supply power at mv  & mA
		//TODO we want to ask for the charger to select the closest to our ideal (12V)
		//And fall back to 9V
	}

//TODO Handle information on supported voltages?
}

/* ----------------- Vendor Defined Messages ------------------ */
/* Holds valid object position (opos) for entered mode */

const uint32_t vdo_idh = VDO_IDH(0, /* data caps as USB host */
1, /* data caps as USB device */
IDH_PTYPE_PERIPH, /* Alternate mode */
0, /* Does not support alt modes */
USB_VID_GOOGLE);

const uint32_t vdo_product = VDO_PRODUCT(CONFIG_USB_PID, CONFIG_USB_BCD_DEV);

const uint32_t vdo_ama = VDO_AMA(CONFIG_USB_PD_IDENTITY_HW_VERS,
		CONFIG_USB_PD_IDENTITY_SW_VERS, 0, 0, 0, 0, /* SS[TR][12] */
		0, /* Vconn power */
		0, /* Vconn power required */
		1, /* Vbus power required */
		AMA_USBSS_U2_ONLY /* USB 2.0 support */);

static int svdm_response_identity(uint32_t *payload) {
	payload[VDO_I(IDH)] = vdo_idh;
	payload[VDO_I(CSTAT)] = VDO_CSTAT(0);
	payload[VDO_I(PRODUCT)] = vdo_product;
	payload[VDO_I(AMA)] = vdo_ama;
	return VDO_I(AMA) + 1;
}
//No custom svdm
static int svdm_response_svids(uint32_t *payload) {
	payload[1] = 0;
	return 2;
}

#define OPOS_DP 1
#define OPOS_GFU 1

const uint32_t vdo_dp_modes[1] = { VDO_MODE_DP(0, /* UFP pin cfg supported : none */
MODE_DP_PIN_C, /* DFP pin cfg supported */
1, /* no usb2.0	signalling in AMode */
CABLE_PLUG, /* its a plug */
MODE_DP_V13, /* DPv1.3 Support, no Gen2 */
MODE_DP_SNK) /* Its a sink only */
};

static int svdm_response_modes(uint32_t *payload) {
	return 0; /* nak */
}

static int dp_status(uint32_t *payload) {
	int opos = PD_VDO_OPOS(payload[0]);
	int hpd = 0; // gpio_get_level(GPIO_DP_HPD);
	if (opos != OPOS_DP)
		return 0; /* nak */

	payload[1] = VDO_DP_STATUS(0, /* IRQ_HPD */
	(hpd == 1), /* HPD_HI|LOW */
	0, /* request exit DP */
	0, /* request exit USB */
	0, /* MF pref */
	0, //gpio_get_level(GPIO_PD_SBU_ENABLE),
			0, /* power low */
			0x2);
	return 2;
}

static int dp_config(uint32_t *payload) {
	return 1;
}

static int svdm_enter_mode(uint32_t *payload) {
	int rv = 0; /* will generate a NAK */

	return rv;
}

int pd_alt_mode(uint16_t svid) {

	return 0;
}

static int svdm_exit_mode(uint32_t *payload) {
	return 1; /* Must return ACK */
}

static struct amode_fx dp_fx = { .status = &dp_status, .config = &dp_config, };

const struct svdm_response svdm_rsp = { &svdm_response_identity,
		&svdm_response_svids, &svdm_response_modes, &svdm_enter_mode,
		&svdm_exit_mode, &dp_fx, };

