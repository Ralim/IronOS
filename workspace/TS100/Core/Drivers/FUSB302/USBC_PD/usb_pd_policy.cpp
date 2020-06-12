/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "USBC_TCPM/tcpm.h"
#include "usb_pd.h"
#include <stddef.h>
#ifdef CONFIG_COMMON_RUNTIME
#define CPRINTS(format, args...) cprints(CC_USBPD, format, ## args)
#define CPRINTF(format, args...) cprintf(CC_USBPD, format, ## args)
#else
#define CPRINTS(format, args...)
#define CPRINTF(format, args...)
#endif

static int rw_flash_changed = 1;

int pd_check_requested_voltage(uint32_t rdo, const int port) {
	//No source
	return EC_ERROR_INVAL;
}

#ifdef CONFIG_USB_PD_DUAL_ROLE
/* Last received source cap */
static uint32_t pd_src_caps[PDO_MAX_OBJECTS];
static uint8_t pd_src_cap_cnt;

/* Cap on the max voltage requested as a sink (in millivolts) */
static unsigned max_request_mv = PD_MAX_VOLTAGE_MV; /* no cap */

int pd_find_pdo_index(int max_mv, uint32_t *selected_pdo) {
	int i, uw, mv, ma;
	int ret = 0;
	int __attribute__((unused)) cur_mv = 0;
	int cur_uw = 0;
	int prefer_cur;
	const uint32_t *src_caps = pd_src_caps;

	/* max voltage is always limited by this boards max request */
	max_mv = MIN(max_mv, PD_MAX_VOLTAGE_MV);

	/* Get max power that is under our max voltage input */
	for (i = 0; i < pd_src_cap_cnt; i++) {
		/* its an unsupported Augmented PDO (PD3.0) */
		if ((src_caps[i] & PDO_TYPE_MASK) == PDO_TYPE_AUGMENTED)
			continue;

		mv = ((src_caps[i] >> 10) & 0x3FF) * 50;
		/* Skip invalid voltage */
		if (!mv)
			continue;
		/* Skip any voltage not supported by this board */
		if (!pd_is_valid_input_voltage(mv))
			continue;

		if ((src_caps[i] & PDO_TYPE_MASK) == PDO_TYPE_BATTERY) {
			uw = 250000 * (src_caps[i] & 0x3FF);
		} else {
			ma = (src_caps[i] & 0x3FF) * 10;
			ma = MIN(ma, PD_MAX_CURRENT_MA);
			uw = ma * mv;
		}

		if (mv > max_mv)
			continue;
		uw = MIN(uw, PD_MAX_POWER_MW * 1000);
		prefer_cur = 0;

		/* Apply special rules in case of 'tie' */
#ifdef PD_PREFER_LOW_VOLTAGE
		if (uw == cur_uw && mv < cur_mv)
			prefer_cur = 1;
#elif defined(PD_PREFER_HIGH_VOLTAGE)
		if (uw == cur_uw && mv > cur_mv)
			prefer_cur = 1;
#endif
		/* Prefer higher power, except for tiebreaker */
		if (uw > cur_uw || prefer_cur) {
			ret = i;
			cur_uw = uw;
			cur_mv = mv;
		}
	}

	if (selected_pdo)
		*selected_pdo = src_caps[ret];

	return ret;
}

void pd_extract_pdo_power(uint32_t pdo, uint32_t *ma, uint32_t *mv) {
	int max_ma, uw;

	*mv = ((pdo >> 10) & 0x3FF) * 50;

	if (*mv == 0) {
		CPRINTF("ERR:PDO mv=0\n");
		*ma = 0;
		return;
	}

	if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_BATTERY) {
		uw = 250000 * (pdo & 0x3FF);
		max_ma = 1000 * MIN(1000 * uw, PD_MAX_POWER_MW) / *mv;
	} else {
		max_ma = 10 * (pdo & 0x3FF);
		max_ma = MIN(max_ma, PD_MAX_POWER_MW * 1000 / *mv);
	}

	*ma = MIN(max_ma, PD_MAX_CURRENT_MA);
}

int pd_build_request(uint32_t *rdo, uint32_t *ma, uint32_t *mv,
		enum pd_request_type req_type) {
	uint32_t pdo;
	int pdo_index, flags = 0;
	int uw;
	int max_or_min_ma;
	int max_or_min_mw;

	if (req_type == PD_REQUEST_VSAFE5V) {
		/* src cap 0 should be vSafe5V */
		pdo_index = 0;
		pdo = pd_src_caps[0];
	} else {
		/* find pdo index for max voltage we can request */
		pdo_index = pd_find_pdo_index(max_request_mv, &pdo);
	}

	pd_extract_pdo_power(pdo, ma, mv);
	uw = *ma * *mv;
	/* Mismatch bit set if less power offered than the operating power */
	if (uw < (1000 * PD_OPERATING_POWER_MW))
		flags |= RDO_CAP_MISMATCH;

#ifdef CONFIG_USB_PD_GIVE_BACK
	/* Tell source we are give back capable. */
	flags |= RDO_GIVE_BACK;

	/*
	 * BATTERY PDO: Inform the source that the sink will reduce
	 * power to this minimum level on receipt of a GotoMin Request.
	 */
	max_or_min_mw = PD_MIN_POWER_MW;

	/*
	 * FIXED or VARIABLE PDO: Inform the source that the sink will reduce
	 * current to this minimum level on receipt of a GotoMin Request.
	 */
	max_or_min_ma = PD_MIN_CURRENT_MA;
#else
	/*
	 * Can't give back, so set maximum current and power to operating
	 * level.
	 */
	max_or_min_ma = *ma;
	max_or_min_mw = uw / 1000;
#endif

	if ((pdo & PDO_TYPE_MASK) == PDO_TYPE_BATTERY) {
		int mw = uw / 1000;
		*rdo = RDO_BATT(pdo_index + 1, mw, max_or_min_mw, flags);
	} else {
		*rdo = RDO_FIXED(pdo_index + 1, *ma, max_or_min_ma, flags);
	}
	return EC_SUCCESS;
}

void pd_process_source_cap(int cnt, uint32_t *src_caps) {
#ifdef CONFIG_CHARGE_MANAGER
	uint32_t ma, mv, pdo;
#endif
	int i;

	pd_src_cap_cnt = cnt;
	for (i = 0; i < cnt; i++)
		pd_src_caps[i] = *src_caps++;

#ifdef CONFIG_CHARGE_MANAGER
	/* Get max power info that we could request */
	pd_find_pdo_index( PD_MAX_VOLTAGE_MV, &pdo);
	pd_extract_pdo_power(pdo, &ma, &mv);

	/* Set max. limit, but apply 500mA ceiling */
	//charge_manager_set_ceil( CEIL_REQUESTOR_PD, PD_MIN_MA);
	pd_set_input_current_limit(ma, mv);
#endif
}

#pragma weak pd_process_source_cap_callback
void pd_process_source_cap_callback(int cnt, uint32_t *src_caps) {
}

void pd_set_max_voltage(unsigned mv) {
	max_request_mv = mv;
}

unsigned pd_get_max_voltage(void) {
	return max_request_mv;
}

int pd_charge_from_device(uint16_t vid, uint16_t pid) {
	/* TODO: rewrite into table if we get more of these */
	/*
	 * White-list Apple charge-through accessory since it doesn't set
	 * externally powered bit, but we still need to charge from it when
	 * we are a sink.
	 */
	return (vid == USB_VID_APPLE && (pid == 0x1012 || pid == 0x1013));
}
#endif /* CONFIG_USB_PD_DUAL_ROLE */

#ifdef CONFIG_USB_PD_ALT_MODE

#ifdef CONFIG_USB_PD_ALT_MODE_DFP

static struct pd_policy pe[CONFIG_USB_PD_PORT_COUNT];

void pd_dfp_pe_init(int port)
{
	memset(&pe, 0, sizeof(struct pd_policy));
}

static void dfp_consume_identity( int cnt, uint32_t *payload)
{
	int ptype = PD_IDH_PTYPE(payload[VDO_I(IDH)]);
	size_t identity_size = MIN(sizeof(pe.identity),
				   (cnt - 1) * sizeof(uint32_t));
	pd_dfp_pe_init(port);
	memcpy(&pe.identity, payload + 1, identity_size);
	switch (ptype) {
	case IDH_PTYPE_AMA:
		/* TODO(tbroch) do I disable VBUS here if power contract
		 * requested it
		 */
		if (!PD_VDO_AMA_VBUS_REQ(payload[VDO_I(AMA)]))
			pd_power_supply_reset(port);

#if defined(CONFIG_USB_PD_DUAL_ROLE) && defined(CONFIG_USBC_VCONN_SWAP)
		/* Adapter is requesting vconn, try to supply it */
		if (PD_VDO_AMA_VCONN_REQ(payload[VDO_I(AMA)]))
			pd_try_vconn_src(port);
#endif
		break;
	default:
		break;
	}
}

static int dfp_discover_svids( uint32_t *payload)
{
	payload[0] = VDO(USB_SID_PD, 1, CMD_DISCOVER_SVID);
	return 1;
}

static void dfp_consume_svids( uint32_t *payload)
{
	int i;
	uint32_t *ptr = payload + 1;
	uint16_t svid0, svid1;

	for (i = pe.svid_cnt; i < pe.svid_cnt + 12; i += 2) {
		if (i == SVID_DISCOVERY_MAX) {
			CPRINTF("ERR:SVIDCNT\n");
			break;
		}

		svid0 = PD_VDO_SVID_SVID0(*ptr);
		if (!svid0)
			break;
		pe.svids[i].svid = svid0;
		pe.svid_cnt++;

		svid1 = PD_VDO_SVID_SVID1(*ptr);
		if (!svid1)
			break;
		pe.svids[i + 1].svid = svid1;
		pe.svid_cnt++;
		ptr++;
	}
	/* TODO(tbroch) need to re-issue discover svids if > 12 */
	if (i && ((i % 12) == 0))
		CPRINTF("ERR:SVID+12\n");
}

static int dfp_discover_modes( uint32_t *payload)
{
	uint16_t svid = pe.svids[pe.svid_idx].svid;
	if (pe.svid_idx >= pe.svid_cnt)
		return 0;
	payload[0] = VDO(svid, 1, CMD_DISCOVER_MODES);
	return 1;
}

static void dfp_consume_modes( int cnt, uint32_t *payload)
{
	int idx = pe.svid_idx;
	pe.svids[idx].mode_cnt = cnt - 1;
	if (pe.svids[idx].mode_cnt < 0) {
		CPRINTF("ERR:NOMODE\n");
	} else {
		memcpy(pe.svids[pe.svid_idx].mode_vdo, &payload[1],
		       sizeof(uint32_t) * pe.svids[idx].mode_cnt);
	}

	pe.svid_idx++;
}

static int get_mode_idx( uint16_t svid)
{
	int i;

	for (i = 0; i < PD_AMODE_COUNT; i++) {
		if (pe.amodes[i].fx->svid == svid)
			return i;
	}
	return -1;
}

static struct svdm_amode_data *get_modep( uint16_t svid)
{
	int idx = get_mode_idx( svid);

	return (idx == -1) ? NULL : &pe.amodes[idx];
}

int pd_alt_mode( uint16_t svid)
{
	struct svdm_amode_data *modep = get_modep( svid);

	return (modep) ? modep->opos : -1;
}

int allocate_mode( uint16_t svid)
{
	int i, j;
	struct svdm_amode_data *modep;
	int mode_idx = get_mode_idx( svid);

	if (mode_idx != -1)
		return mode_idx;

	/* There's no space to enter another mode */
	if (pe.amode_idx == PD_AMODE_COUNT) {
		CPRINTF("ERR:NO AMODE SPACE\n");
		return -1;
	}

	/* Allocate ...  if SVID == 0 enter default supported policy */
	for (i = 0; i < supported_modes_cnt; i++) {
		if (!&supported_modes[i])
			continue;

		for (j = 0; j < pe.svid_cnt; j++) {
			struct svdm_svid_data *svidp = &pe.svids[j];
			if ((svidp->svid != supported_modes[i].svid) ||
			    (svid && (svidp->svid != svid)))
				continue;

			modep = &pe.amodes[pe.amode_idx];
			modep->fx = &supported_modes[i];
			modep->data = &pe.svids[j];
			pe.amode_idx++;
			return pe.amode_idx - 1;
		}
	}
	return -1;
}

/*
 * Enter default mode ( payload[0] == 0 ) or attempt to enter mode via svid &
 * opos
*/
uint32_t pd_dfp_enter_mode( uint16_t svid, int opos)
{
	int mode_idx = allocate_mode( svid);
	struct svdm_amode_data *modep;
	uint32_t mode_caps;

	if (mode_idx == -1)
		return 0;
	modep = &pe.amodes[mode_idx];

	if (!opos) {
		/* choose the lowest as default */
		modep->opos = 1;
	} else if (opos <= modep->data->mode_cnt) {
		modep->opos = opos;
	} else {
		CPRINTF("opos error\n");
		return 0;
	}

	mode_caps = modep->data->mode_vdo[modep->opos - 1];
	if (modep->fx->enter( mode_caps) == -1)
		return 0;

	/* SVDM to send to UFP for mode entry */
	return VDO(modep->fx->svid, 1, CMD_ENTER_MODE | VDO_OPOS(modep->opos));
}

static int validate_mode_request(struct svdm_amode_data *modep,
				 uint16_t svid, int opos)
{
	if (!modep->fx)
		return 0;

	if (svid != modep->fx->svid) {
		CPRINTF("ERR:svid r:0x%04x != c:0x%04x\n",
			svid, modep->fx->svid);
		return 0;
	}

	if (opos != modep->opos) {
		CPRINTF("ERR:opos r:%d != c:%d\n",
			opos, modep->opos);
		return 0;
	}

	return 1;
}

static void dfp_consume_attention( uint32_t *payload)
{
	uint16_t svid = PD_VDO_VID(payload[0]);
	int opos = PD_VDO_OPOS(payload[0]);
	struct svdm_amode_data *modep = get_modep( svid);

	if (!modep || !validate_mode_request(modep, svid, opos))
		return;

	if (modep->fx->attention)
		modep->fx->attention( payload);
}

/*
 * This algorithm defaults to choosing higher pin config over lower ones in
 * order to prefer multi-function if desired.
 *
 *  NAME | SIGNALING | OUTPUT TYPE | MULTI-FUNCTION | PIN CONFIG
 * -------------------------------------------------------------
 *  A    |  USB G2   |  ?          | no             | 00_0001
 *  B    |  USB G2   |  ?          | yes            | 00_0010
 *  C    |  DP       |  CONVERTED  | no             | 00_0100
 *  D    |  PD       |  CONVERTED  | yes            | 00_1000
 *  E    |  DP       |  DP         | no             | 01_0000
 *  F    |  PD       |  DP         | yes            | 10_0000
 *
 * if UFP has NOT asserted multi-function preferred code masks away B/D/F
 * leaving only A/C/E.  For single-output dongles that should leave only one
 * possible pin config depending on whether its a converter DP->(VGA|HDMI) or DP
 * output.  If UFP is a USB-C receptacle it may assert C/D/E/F.  The DFP USB-C
 * receptacle must always choose C/D in those cases.
 */
int pd_dfp_dp_get_pin_mode( uint32_t status)
{
	struct svdm_amode_data *modep = get_modep( USB_SID_DISPLAYPORT);
	uint32_t mode_caps;
	uint32_t pin_caps;
	if (!modep)
		return 0;

	mode_caps = modep->data->mode_vdo[modep->opos - 1];

	/* TODO(crosbug.com/p/39656) revisit with DFP that can be a sink */
	pin_caps = PD_DP_PIN_CAPS(mode_caps);

	/* if don't want multi-function then ignore those pin configs */
	if (!PD_VDO_DPSTS_MF_PREF(status))
		pin_caps &= ~MODE_DP_PIN_MF_MASK;

	/* TODO(crosbug.com/p/39656) revisit if DFP drives USB Gen 2 signals */
	pin_caps &= ~MODE_DP_PIN_BR2_MASK;

	/* if C/D present they have precedence over E/F for USB-C->USB-C */
	if (pin_caps & (MODE_DP_PIN_C | MODE_DP_PIN_D))
		pin_caps &= ~(MODE_DP_PIN_E | MODE_DP_PIN_F);

	/* get_next_bit returns undefined for zero */
	if (!pin_caps)
		return 0;

	return 1 << get_next_bit(&pin_caps);
}

int pd_dfp_exit_mode( uint16_t svid, int opos)
{
	struct svdm_amode_data *modep;
	int idx;

	/*
	 * Empty svid signals we should reset DFP VDM state by exiting all
	 * entered modes then clearing state.  This occurs when we've
	 * disconnected or for hard reset.
	 */
	if (!svid) {
		for (idx = 0; idx < PD_AMODE_COUNT; idx++)
			if (pe.amodes[idx].fx)
				pe.amodes[idx].fx->exit(port);

		pd_dfp_pe_init(port);
		return 0;
	}

	/*
	 * TODO(crosbug.com/p/33946) : below needs revisited to allow multiple
	 * mode exit.  Additionally it should honor OPOS == 7 as DFP's request
	 * to exit all modes.  We currently don't have any UFPs that support
	 * multiple modes on one SVID.
	 */
	modep = get_modep( svid);
	if (!modep || !validate_mode_request(modep, svid, opos))
		return 0;

	/* call DFPs exit function */
	modep->fx->exit(port);
	/* exit the mode */
	modep->opos = 0;
	return 1;
}

uint16_t pd_get_identity_vid(int port)
{
	return PD_IDH_VID(pe.identity[0]);
}

uint16_t pd_get_identity_pid(int port)
{
	return PD_PRODUCT_PID(pe.identity[2]);
}

#ifdef CONFIG_CMD_USB_PD_PE
static void dump_pe(int port)
{
	const char * const idh_ptype_names[]  = {
		"UNDEF", "Hub", "Periph", "PCable", "ACable", "AMA",
		"RSV6", "RSV7"};

	int i, j, idh_ptype;
	struct svdm_amode_data *modep;
	uint32_t mode_caps;

	if (pe.identity[0] == 0) {
		ccprintf("No identity discovered yet.\n");
		return;
	}
	idh_ptype = PD_IDH_PTYPE(pe.identity[0]);
	ccprintf("IDENT:\n");
	ccprintf("\t[ID Header] %08x :: %s, VID:%04x\n", pe.identity[0],
		 idh_ptype_names[idh_ptype], pd_get_identity_vid(port));
	ccprintf("\t[Cert Stat] %08x\n", pe.identity[1]);
	for (i = 2; i < ARRAY_SIZE(pe.identity); i++) {
		ccprintf("\t");
		if (pe.identity[i])
			ccprintf("[%d] %08x ", i, pe.identity[i]);
	}
	ccprintf("\n");

	if (pe.svid_cnt < 1) {
		ccprintf("No SVIDS discovered yet.\n");
		return;
	}

	for (i = 0; i < pe.svid_cnt; i++) {
		ccprintf("SVID[%d]: %04x MODES:", i, pe.svids[i].svid);
		for (j = 0; j < pe.svids[j].mode_cnt; j++)
			ccprintf(" [%d] %08x", j + 1,
				 pe.svids[i].mode_vdo[j]);
		ccprintf("\n");
		modep = get_modep( pe.svids[i].svid);
		if (modep) {
			mode_caps = modep->data->mode_vdo[modep->opos - 1];
			ccprintf("MODE[%d]: svid:%04x caps:%08x\n", modep->opos,
				 modep->fx->svid, mode_caps);
		}
	}
}

static int command_pe(int argc, char **argv)
{
	int port;
	char *e;
	if (argc < 3)
		return EC_ERROR_PARAM_COUNT;
	/* command: pe <port> <subcmd> <args> */
	port = strtoi(argv[1], &e, 10);
	if (*e || port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_ERROR_PARAM2;
	if (!strncasecmp(argv[2], "dump", 4))
		dump_pe(port);

	return EC_SUCCESS;
}

DECLARE_CONSOLE_COMMAND(pe, command_pe,
			"<port> dump",
			"USB PE");
#endif /* CONFIG_CMD_USB_PD_PE */

#endif /* CONFIG_USB_PD_ALT_MODE_DFP */

int pd_svdm(int cnt, uint32_t *payload, uint32_t **rpayload) {
	int cmd = PD_VDO_CMD(payload[0]);
	int cmd_type = PD_VDO_CMDT(payload[0]);
	int (*func)(uint32_t *payload) = NULL;

	int rsize = 1; /* VDM header at a minimum */

	payload[0] &= ~VDO_CMDT_MASK;
	*rpayload = payload;

	if (cmd_type == CMDT_INIT) {
		switch (cmd) {
		case CMD_DISCOVER_IDENT:
			func = svdm_rsp.identity;
			break;
		case CMD_DISCOVER_SVID:
			func = svdm_rsp.svids;
			break;
		case CMD_DISCOVER_MODES:
			func = svdm_rsp.modes;
			break;
		case CMD_ENTER_MODE:
			func = svdm_rsp.enter_mode;
			break;
		case CMD_DP_STATUS:
			func = svdm_rsp.amode->status;
			break;
		case CMD_DP_CONFIG:
			func = svdm_rsp.amode->config;
			break;
		case CMD_EXIT_MODE:
			func = svdm_rsp.exit_mode;
			break;
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
		case CMD_ATTENTION:
			/*
			 * attention is only SVDM with no response
			 * (just goodCRC) return zero here.
			 */
			dfp_consume_attention( payload);
			return 0;
#endif
		default:
			CPRINTF("ERR:CMD:%d\n", cmd);
			rsize = 0;
		}
		if (func)
			rsize = func(payload);
		else
			/* not supported : NACK it */
			rsize = 0;
		if (rsize >= 1)
			payload[0] |= VDO_CMDT(CMDT_RSP_ACK);
		else if (!rsize) {
			payload[0] |= VDO_CMDT(CMDT_RSP_NAK);
			rsize = 1;
		} else {
			payload[0] |= VDO_CMDT(CMDT_RSP_BUSY);
			rsize = 1;
		}
		payload[0] |= VDO_SVDM_VERS(pd_get_vdo_ver(port));
	} else if (cmd_type == CMDT_RSP_ACK) {
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
		struct svdm_amode_data *modep;

		modep = get_modep( PD_VDO_VID(payload[0]));
#endif
		switch (cmd) {
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
		case CMD_DISCOVER_IDENT:
			dfp_consume_identity( cnt, payload);
			rsize = dfp_discover_svids( payload);
#ifdef CONFIG_CHARGE_MANAGER
			if (pd_charge_from_device(pd_get_identity_vid(port),
						  pd_get_identity_pid(port)))
				charge_manager_update_dualrole(
							       CAP_DEDICATED);
#endif
			break;
		case CMD_DISCOVER_SVID:
			dfp_consume_svids( payload);
			rsize = dfp_discover_modes( payload);
			break;
		case CMD_DISCOVER_MODES:
			dfp_consume_modes( cnt, payload);
			rsize = dfp_discover_modes( payload);
			/* enter the default mode for DFP */
			if (!rsize) {
				payload[0] = pd_dfp_enter_mode( 0, 0);
				if (payload[0])
					rsize = 1;
			}
			break;
		case CMD_ENTER_MODE:
			if (!modep) {
				rsize = 0;
			} else {
				if (!modep->opos)
					pd_dfp_enter_mode( 0, 0);

				if (modep->opos) {
					rsize = modep->fx->status(
								  payload);
					payload[0] |= PD_VDO_OPOS(modep->opos);
				}
			}
			break;
		case CMD_DP_STATUS:
			/* DP status response & UFP's DP attention have same
			   payload */
			dfp_consume_attention( payload);
			if (modep && modep->opos)
				rsize = modep->fx->config( payload);
			else
				rsize = 0;
			break;
		case CMD_DP_CONFIG:
			if (modep && modep->opos && modep->fx->post_config)
				modep->fx->post_config(port);
			/* no response after DFPs ack */
			rsize = 0;
			break;
		case CMD_EXIT_MODE:
			/* no response after DFPs ack */
			rsize = 0;
			break;
#endif
		case CMD_ATTENTION:
			/* no response after DFPs ack */
			rsize = 0;
			break;
		default:
			CPRINTF("ERR:CMD:%d\n", cmd);
			rsize = 0;
		}

		payload[0] |= VDO_CMDT(CMDT_INIT);
		payload[0] |= VDO_SVDM_VERS(pd_get_vdo_ver(port));
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
	} else if (cmd_type == CMDT_RSP_BUSY) {
		switch (cmd) {
		case CMD_DISCOVER_IDENT:
		case CMD_DISCOVER_SVID:
		case CMD_DISCOVER_MODES:
			/* resend if its discovery */
			rsize = 1;
			break;
		case CMD_ENTER_MODE:
			/* Error */
			CPRINTF("ERR:ENTBUSY\n");
			rsize = 0;
			break;
		case CMD_EXIT_MODE:
			rsize = 0;
			break;
		default:
			rsize = 0;
		}
	} else if (cmd_type == CMDT_RSP_NAK) {
		/* nothing to do */
		rsize = 0;
#endif /* CONFIG_USB_PD_ALT_MODE_DFP */
	} else {
		CPRINTF("ERR:CMDT:%d\n", cmd);
		/* do not answer */
		rsize = 0;
	}
	return rsize;
}

#else

int pd_svdm( int cnt, uint32_t *payload, uint32_t **rpayload)
{
	return 0;
}

#endif /* CONFIG_USB_PD_ALT_MODE */

#ifndef CONFIG_USB_PD_CUSTOM_VDM
int pd_vdm(int cnt, uint32_t *payload, uint32_t **rpayload) {
	return 0;
}
#endif /* !CONFIG_USB_PD_CUSTOM_VDM */

static void pd_usb_billboard_deferred(void) {
#if defined(CONFIG_USB_PD_ALT_MODE) && !defined(CONFIG_USB_PD_ALT_MODE_DFP) \
	&& !defined(CONFIG_USB_PD_SIMPLE_DFP) && defined(CONFIG_USB_BOS)

	/*
	 * TODO(tbroch)
	 * 1. Will we have multiple type-C port UFPs
	 * 2. Will there be other modes applicable to DFPs besides DP
	 */
	if (!pd_alt_mode(0, USB_SID_DISPLAYPORT))
		usb_connect();

#endif
}

#ifdef CONFIG_USB_PD_ALT_MODE_DFP
static int hc_remote_pd_discovery(struct host_cmd_handler_args *args)
{
	const uint8_t *port = args->params;
	struct ec_params_usb_pd_discovery_entry *r = args->response;

	if (*port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_RES_INVALID_PARAM;

	r->vid = pd_get_identity_vid(*port);
	r->ptype = PD_IDH_PTYPE(pe[*port].identity[0]);
	/* pid only included if vid is assigned */
	if (r->vid)
		r->pid = PD_PRODUCT_PID(pe[*port].identity[2]);

	args->response_size = sizeof(*r);
	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_USB_PD_DISCOVERY,
		     hc_remote_pd_discovery,
		     EC_VER_MASK(0));

static int hc_remote_pd_get_amode(struct host_cmd_handler_args *args)
{
	struct svdm_amode_data *modep;
	const struct ec_params_usb_pd_get_mode_request *p = args->params;
	struct ec_params_usb_pd_get_mode_response *r = args->response;

	if (p->port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_RES_INVALID_PARAM;

	/* no more to send */
	if (p->svid_idx >= pe[p->port].svid_cnt) {
		r->svid = 0;
		args->response_size = sizeof(r->svid);
		return EC_RES_SUCCESS;
	}

	r->svid = pe[p->port].svids[p->svid_idx].svid;
	r->opos = 0;
	memcpy(r->vdo, pe[p->port].svids[p->svid_idx].mode_vdo, 24);
	modep = get_modep(p->port, r->svid);

	if (modep)
		r->opos = pd_alt_mode(p->port, r->svid);

	args->response_size = sizeof(*r);
	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_USB_PD_GET_AMODE,
		     hc_remote_pd_get_amode,
		     EC_VER_MASK(0));

#endif

#define FW_RW_END (CONFIG_EC_WRITABLE_STORAGE_OFF + \
		   CONFIG_RW_STORAGE_OFF + CONFIG_RW_SIZE)

#ifdef CONFIG_USB_PD_DISCHARGE
void pd_set_vbus_discharge( int enable)
{
	static struct mutex discharge_lock[CONFIG_USB_PD_PORT_COUNT];

	mutex_lock(&discharge_lock);
	enable &= !board_vbus_source_enabled(port);
#ifdef CONFIG_USB_PD_DISCHARGE_GPIO
	if (!port)
		gpio_set_level(GPIO_USB_C0_DISCHARGE, enable);
#if CONFIG_USB_PD_PORT_COUNT > 1
	else
		gpio_set_level(GPIO_USB_C1_DISCHARGE, enable);
#endif /* CONFIG_USB_PD_PORT_COUNT */
#elif defined(CONFIG_USB_PD_DISCHARGE_TCPC)
	tcpc_discharge_vbus( enable);
#else
#error "PD discharge implementation not defined"
#endif
	mutex_unlock(&discharge_lock);
}
#endif /* CONFIG_USB_PD_DISCHARGE */
