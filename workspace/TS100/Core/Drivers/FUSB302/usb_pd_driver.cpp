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

const uint32_t pd_snk_pdo[] = { PDO_FIXED(5000, 500, PDO_FIXED_FLAGS),
		PDO_FIXED(9000, 500, PDO_FIXED_FLAGS), PDO_FIXED(20000, 500,
				PDO_FIXED_FLAGS), };
const int pd_snk_pdo_cnt = ARRAY_SIZE(pd_snk_pdo);

void pd_set_input_current_limit(int port, uint32_t max_ma,
		uint32_t supply_voltage) {

}

int pd_is_valid_input_voltage(int mv) {
	return 1;
}

int pd_snk_is_vbus_provided(int port) {
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

void pd_power_supply_reset(int port) {
	return;
}

void pd_execute_data_swap(int port, int data_role) {
	/* Do nothing */
}

int pd_check_data_swap(int port, int data_role) {
	// Never allow data swap
	return 0;
}

int pd_check_power_swap(int port) {
	/* Always refuse power swap */
	return 0;
}

int pd_board_checks(void) {
	return EC_SUCCESS;
}

int pd_set_power_supply_ready(int port) {
#if 0
	/* Disable charging */
	gpio_set_level(GPIO_USB_C0_CHARGE_L, 1);

	/* Enable VBUS source */
	gpio_set_level(GPIO_USB_C0_5V_EN, 1);

	/* notify host of power info change */
	pd_send_host_event(PD_EVENT_POWER_CHANGE);
#endif // if 0
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

void pd_check_dr_role(int port, int dr_role, int flags) {
#if 0
	/* If UFP, try to switch to DFP */
	if ((flags & PD_FLAGS_PARTNER_DR_DATA) && dr_role == PD_ROLE_UFP)
	pd_request_data_swap(port);
#endif
}

void pd_check_pr_role(int port, int pr_role, int flags) {
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

void pd_process_source_cap_callback(int port, int cnt, uint32_t *src_caps) {
//	char str[256];
//	int i;
//	uint32_t ma, mv, pdo;
//	uint8_t old_display;
//
//	old_display = display_screen;
//	display_screen = SCREEN_POWER;
//	memset(display_buffer[SCREEN_POWER], 0x00, DISP_MEM_SIZE);
//
//	sprintf(str, "Has Power Delivery");
//	UG_PutString(0, 8, str);
//
//	for (i = 0; i < cnt; i++)
//	{
//		pd_extract_pdo_power(src_caps[i], &ma, &mv);
//		sprintf(str, "%5.2f V, %5.2f A", (float)mv/1000, (float)ma/1000);
//		UG_PutString(0, 8*(i+2), str);
//	}
//
//	display_screen = old_display;
//	display_needs_update = 1;

//TODO Handle information on supported voltages?
}

/* ----------------- Vendor Defined Messages ------------------ */
/* Holds valid object position (opos) for entered mode */
static int alt_mode[PD_AMODE_COUNT];

const uint32_t vdo_idh = VDO_IDH(0, /* data caps as USB host */
1, /* data caps as USB device */
IDH_PTYPE_AMA, /* Alternate mode */
1, /* supports alt modes */
USB_VID_GOOGLE);

const uint32_t vdo_product = VDO_PRODUCT(CONFIG_USB_PID, CONFIG_USB_BCD_DEV);

const uint32_t vdo_ama = VDO_AMA(CONFIG_USB_PD_IDENTITY_HW_VERS,
		CONFIG_USB_PD_IDENTITY_SW_VERS, 0, 0, 0, 0, /* SS[TR][12] */
		0, /* Vconn power */
		0, /* Vconn power required */
		1, /* Vbus power required */
		AMA_USBSS_BBONLY /* USB SS support */);

static int svdm_response_identity(int port, uint32_t *payload) {
	payload[VDO_I(IDH)] = vdo_idh;
	/* TODO(tbroch): Do we plan to obtain TID (test ID) */
	payload[VDO_I(CSTAT)] = VDO_CSTAT(0);
	payload[VDO_I(PRODUCT)] = vdo_product;
	payload[VDO_I(AMA)] = vdo_ama;
	return VDO_I(AMA) + 1;
}

static int svdm_response_svids(int port, uint32_t *payload) {
	payload[1] = VDO_SVID(USB_SID_DISPLAYPORT, USB_VID_GOOGLE);
	payload[2] = 0;
	return 3;
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

const uint32_t vdo_goog_modes[1] = { VDO_MODE_GOOGLE(MODE_GOOGLE_FU) };

static int svdm_response_modes(int port, uint32_t *payload) {
	if (PD_VDO_VID(payload[0]) == USB_SID_DISPLAYPORT) {
		memcpy(payload + 1, vdo_dp_modes, sizeof(vdo_dp_modes));
		return ARRAY_SIZE(vdo_dp_modes) + 1;
	} else if (PD_VDO_VID(payload[0]) == USB_VID_GOOGLE) {
		memcpy(payload + 1, vdo_goog_modes, sizeof(vdo_goog_modes));
		return ARRAY_SIZE(vdo_goog_modes) + 1;
	} else {
		return 0; /* nak */
	}
}

static int dp_status(int port, uint32_t *payload) {
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

static int dp_config(int port, uint32_t *payload) {
	if (PD_DP_CFG_DPON(payload[1]))
		0; //gpio_set_level(GPIO_PD_SBU_ENABLE, 1);
	return 1;
}

static int svdm_enter_mode(int port, uint32_t *payload) {
	int rv = 0; /* will generate a NAK */
	char str[256];
	uint8_t old_display;

	/* SID & mode request is valid */
	if ((PD_VDO_VID(payload[0]) == USB_SID_DISPLAYPORT)
			&& (PD_VDO_OPOS(payload[0]) == OPOS_DP)) {
		alt_mode[PD_AMODE_DISPLAYPORT] = OPOS_DP;
		rv = 1;
		//pd_log_event(PD_EVENT_VIDEO_DP_MODE, 0, 1, NULL);
	} else if ((PD_VDO_VID(payload[0]) == USB_VID_GOOGLE)
			&& (PD_VDO_OPOS(payload[0]) == OPOS_GFU)) {
		alt_mode[PD_AMODE_GOOGLE] = OPOS_GFU;
		rv = 1;
	}

//	if (rv)
	/*
	 * If we failed initial mode entry we'll have enumerated the USB
	 * Billboard class.  If so we should disconnect.
	 */
	//usb_disconnect();
//	old_display = display_screen;
//	display_screen = SCREEN_ALTMODE;
//	memset(display_buffer[SCREEN_ALTMODE], 0x00, DISP_MEM_SIZE);
//
//	sprintf(str, "Requested Alt Mode");
//	UG_PutString(0, 8, str);
//
//	display_screen = old_display;
//	display_needs_update = 1;
//TODO handle alt mode ?
	return rv;
}

int pd_alt_mode(int port, uint16_t svid) {
	if (svid == USB_SID_DISPLAYPORT)
		return alt_mode[PD_AMODE_DISPLAYPORT];
	else if (svid == USB_VID_GOOGLE)
		return alt_mode[PD_AMODE_GOOGLE];
	return 0;
}

static int svdm_exit_mode(int port, uint32_t *payload) {
	return 1; /* Must return ACK */
}

static struct amode_fx dp_fx = { .status = &dp_status, .config = &dp_config, };

const struct svdm_response svdm_rsp = { &svdm_response_identity,
		&svdm_response_svids, &svdm_response_modes, &svdm_enter_mode,
		&svdm_exit_mode, &dp_fx, };

int pd_custom_vdm(int port, int cnt, uint32_t *payload, uint32_t **rpayload) {
	int rsize;

	if (PD_VDO_VID(payload[0]) != USB_VID_GOOGLE || !alt_mode[PD_AMODE_GOOGLE])
		return 0;

	*rpayload = payload;

	rsize = pd_custom_flash_vdm(port, cnt, payload);
	if (!rsize) {
		int cmd = PD_VDO_CMD(payload[0]);
		switch (cmd) {
		case VDO_CMD_GET_LOG:
			rsize = pd_vdm_get_log_entry(payload);
			break;
		default:
			/* Unknown : do not answer */
			return 0;
		}
	}

	/* respond (positively) to the request */
	payload[0] |= VDO_SRC_RESPONDER;

	return rsize;
}

int pd_custom_flash_vdm(int port, int cnt, uint32_t *payload) {
	static int flash_offset;
	int rsize = 1; /* default is just VDM header returned */

	switch (PD_VDO_CMD(payload[0])) {
#if 0
		case VDO_CMD_VERSION:
			memcpy(payload + 1, &current_image_data.version, 24);
			rsize = 7;
			break;
		case VDO_CMD_REBOOT:
			/* ensure the power supply is in a safe state */
			pd_power_supply_reset(0);
			system_reset(0);
			break;
		case VDO_CMD_READ_INFO:
			/* copy info into response */
			pd_get_info(payload + 1);
			rsize = 7;
			break;
		case VDO_CMD_FLASH_ERASE:
			/* do not kill the code under our feet */
			if (system_get_image_copy() != SYSTEM_IMAGE_RO)
				break;
			pd_log_event(PD_EVENT_ACC_RW_ERASE, 0, 0, NULL);
			flash_offset = CONFIG_EC_WRITABLE_STORAGE_OFF +
			CONFIG_RW_STORAGE_OFF;
			flash_physical_erase(CONFIG_EC_WRITABLE_STORAGE_OFF +
			CONFIG_RW_STORAGE_OFF, CONFIG_RW_SIZE);
			rw_flash_changed = 1;
			break;
		case VDO_CMD_FLASH_WRITE:
			/* do not kill the code under our feet */
			if ((system_get_image_copy() != SYSTEM_IMAGE_RO) ||
			(flash_offset < CONFIG_EC_WRITABLE_STORAGE_OFF +
			CONFIG_RW_STORAGE_OFF))
				break;
			flash_physical_write(flash_offset, 4*(cnt - 1),
			(const char *)(payload+1));
			flash_offset += 4*(cnt - 1);
			rw_flash_changed = 1;
			break;
		case VDO_CMD_ERASE_SIG:
			/* this is not touching the code area */
			{
				uint32_t zero = 0;
				int offset;
				/* zeroes the area containing the RSA signature */
				for (offset = FW_RW_END - RSANUMBYTES;
				offset < FW_RW_END; offset += 4)
				flash_physical_write(offset, 4,
				(const char *)&zero);
			}
			break;
#endif // 0
	default:
		/* Unknown : do not answer */
		return 0;
	}
	return rsize;
}
