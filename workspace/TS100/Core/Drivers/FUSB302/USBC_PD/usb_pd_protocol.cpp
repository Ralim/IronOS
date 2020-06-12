/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "usb_pd.h"
#include "USBC_TCPM/usb_pd_tcpm.h"
#include "USBC_TCPM/tcpm.h"
#include "usb_pd_driver.h"
#include <stddef.h>
#include <string.h>
#ifdef CONFIG_COMMON_RUNTIME
#define CPRINTF(format, args...) cprintf(CC_USBPD, format, ## args)
#define CPRINTS(format, args...) cprints(CC_USBPD, format, ## args)

BUILD_ASSERT(CONFIG_USB_PD_PORT_COUNT <= EC_USB_PD_MAX_PORTS);

/*
 * Debug log level - higher number == more log
 *   Level 0: Log state transitions
 *   Level 1: Level 0, plus state name
 *   Level 2: Level 1, plus packet info
 *   Level 3: Level 2, plus ping packet and packet dump on error
 *
 * Note that higher log level causes timing changes and thus may affect
 * performance.
 *
 * Can be limited to constant debug_level by CONFIG_USB_PD_DEBUG_LEVEL
 */
#ifdef CONFIG_USB_PD_DEBUG_LEVEL
static const int debug_level = CONFIG_USB_PD_DEBUG_LEVEL;
#else
static int debug_level;
#endif

/*
 * PD communication enabled flag. When false, PD state machine still
 * detects source/sink connection and disconnection, and will still
 * provide VBUS, but never sends any PD communication.
 */
static uint8_t pd_comm_enabled[CONFIG_USB_PD_PORT_COUNT];
#else /* CONFIG_COMMON_RUNTIME */
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
static const int debug_level = 0;
#endif

#ifdef CONFIG_USB_PD_DUAL_ROLE
#define DUAL_ROLE_IF_ELSE( sink_clause, src_clause) \
	(pd.power_role == PD_ROLE_SINK ? (sink_clause) : (src_clause))
#else
#define DUAL_ROLE_IF_ELSE( sink_clause, src_clause) (src_clause)
#endif

#define READY_RETURN_STATE() DUAL_ROLE_IF_ELSE( PD_STATE_SNK_READY, \
							 PD_STATE_SRC_READY)

/* Type C supply voltage (mV) */
#define TYPE_C_VOLTAGE	5000 /* mV */

/* PD counter definitions */
#define PD_MESSAGE_ID_COUNT 7
#define PD_HARD_RESET_COUNT 2
#define PD_CAPS_COUNT 50
#define PD_SNK_CAP_RETRIES 3

enum vdm_states {
	VDM_STATE_ERR_BUSY = -3,
	VDM_STATE_ERR_SEND = -2,
	VDM_STATE_ERR_TMOUT = -1,
	VDM_STATE_DONE = 0,
	/* Anything >0 represents an active state */
	VDM_STATE_READY = 1,
	VDM_STATE_BUSY = 2,
	VDM_STATE_WAIT_RSP_BUSY = 3,
};

#ifdef CONFIG_USB_PD_DUAL_ROLE
/* Port dual-role state */
enum pd_dual_role_states drp_state = CONFIG_USB_PD_INITIAL_DRP_STATE;

/* Enable variable for Try.SRC states */
static uint8_t pd_try_src_enable;
#endif

#ifdef CONFIG_USB_PD_REV30
/*
 * The spec. revision is used to index into this array.
 *  Rev 0 (PD 1.0) - return PD_CTRL_REJECT
 *  Rev 1 (PD 2.0) - return PD_CTRL_REJECT
 *  Rev 2 (PD 3.0) - return PD_CTRL_NOT_SUPPORTED
 */
static const uint8_t refuse[] = {
	PD_CTRL_REJECT, PD_CTRL_REJECT, PD_CTRL_NOT_SUPPORTED};
#define REFUSE(r) refuse[r]
#else
#define REFUSE(r) PD_CTRL_REJECT
#endif

#ifdef CONFIG_USB_PD_REV30
/*
 * The spec. revision is used to index into this array.
 *  Rev 0 (VDO 1.0) - return VDM_VER10
 *  Rev 1 (VDO 1.0) - return VDM_VER10
 *  Rev 2 (VDO 2.0) - return VDM_VER20
 */
static const uint8_t vdo_ver[] = {
	VDM_VER10, VDM_VER10, VDM_VER20};
#define VDO_VER(v) vdo_ver[v]
#else
#define VDO_VER(v) VDM_VER10
#endif

// variables that used to be pd_task, but had to be promoted 
// so both pd_init and pd_run_state_machine can see them
static int head;
static int port = TASK_ID_TO_PD_PORT(task_get_current());
static uint32_t payload[7];
static int timeout = 10 * MSEC;
static int cc1, cc2;
static int res, incoming_packet = 0;
static int hard_reset_count = 0;
#ifdef CONFIG_USB_PD_DUAL_ROLE
static uint64_t next_role_swap = PD_T_DRP_SNK;
#ifndef CONFIG_USB_PD_VBUS_DETECT_NONE
static int snk_hard_reset_vbus_off = 0;
#endif
#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
static const int auto_toggle_supported = tcpm_auto_toggle_supported();
#endif
#if defined(CONFIG_CHARGE_MANAGER)
static typec_current_t typec_curr = 0, typec_curr_change = 0;
#endif /* CONFIG_CHARGE_MANAGER */
#endif /* CONFIG_USB_PD_DUAL_ROLE */
static enum pd_states this_state;
static enum pd_cc_states new_cc_state;
static timestamp_t now;
static int caps_count = 0, hard_reset_sent = 0;
static int snk_cap_count = 0;
static int evt;

static struct pd_protocol {
	/* current port power role (SOURCE or SINK) */
	uint8_t power_role;
	/* current port data role (DFP or UFP) */
	uint8_t data_role;
	/* 3-bit rolling message ID counter */
	uint8_t msg_id;
	/* Port polarity : 0 => CC1 is CC line, 1 => CC2 is CC line */
	uint8_t polarity;
	/* PD state for port */
	enum pd_states task_state;
	/* PD state when we run state handler the last time */
	enum pd_states last_state;
	/* bool: request state change to SUSPENDED */
	uint8_t req_suspend_state;
	/* The state to go to after timeout */
	enum pd_states timeout_state;
	/* port flags, see PD_FLAGS_* */
	uint32_t flags;
	/* Timeout for the current state. Set to 0 for no timeout. */
	uint64_t timeout;
	/* Time for source recovery after hard reset */
	uint64_t src_recover;
	/* Time for CC debounce end */
	uint64_t cc_debounce;
	/* The cc state */
	enum pd_cc_states cc_state;
	/* status of last transmit */
	uint8_t tx_status;

	/* last requested voltage PDO index */
	int requested_idx;
#ifdef CONFIG_USB_PD_DUAL_ROLE
	/* Current limit / voltage based on the last request message */
	uint32_t curr_limit;
	uint32_t supply_voltage;
	/* Signal charging update that affects the port */
	int new_power_request;
	/* Store previously requested voltage request */
	int prev_request_mv;
	/* Time for Try.SRC states */
	uint64_t try_src_marker;
#endif

	/* PD state for Vendor Defined Messages */
	enum vdm_states vdm_state;
	/* Timeout for the current vdm state.  Set to 0 for no timeout. */
	timestamp_t vdm_timeout;
	/* next Vendor Defined Message to send */
	uint32_t vdo_data[VDO_MAX_SIZE];
	uint8_t vdo_count;
	/* VDO to retry if UFP responder replied busy. */
	uint32_t vdo_retry;

#ifdef CONFIG_USB_PD_CHROMEOS
	/* Attached ChromeOS device id, RW hash, and current RO / RW image */
	uint16_t dev_id;
	uint32_t dev_rw_hash[PD_RW_HASH_SIZE/4];
	enum ec_current_image current_image;
#endif
#ifdef CONFIG_USB_PD_REV30
	/* PD Collision avoidance buffer */
	uint16_t ca_buffered;
	uint16_t ca_header;
	uint32_t ca_buffer[PDO_MAX_OBJECTS];
	enum tcpm_transmit_type ca_type;
	/* protocol revision */
	uint8_t rev;
#endif
} pd;

#ifdef CONFIG_COMMON_RUNTIME
static const char * const pd_state_names[] = {
	"DISABLED", "SUSPENDED",
#ifdef CONFIG_USB_PD_DUAL_ROLE
	"SNK_DISCONNECTED", "SNK_DISCONNECTED_DEBOUNCE",
	"SNK_HARD_RESET_RECOVER",
	"SNK_DISCOVERY", "SNK_REQUESTED", "SNK_TRANSITION", "SNK_READY",
	"SNK_SWAP_INIT", "SNK_SWAP_SNK_DISABLE",
	"SNK_SWAP_SRC_DISABLE", "SNK_SWAP_STANDBY", "SNK_SWAP_COMPLETE",
#endif /* CONFIG_USB_PD_DUAL_ROLE */
	"SRC_DISCONNECTED", "SRC_DISCONNECTED_DEBOUNCE",
	"SRC_HARD_RESET_RECOVER", "SRC_STARTUP",
	"SRC_DISCOVERY", "SRC_NEGOCIATE", "SRC_ACCEPTED", "SRC_POWERED",
	"SRC_TRANSITION", "SRC_READY", "SRC_GET_SNK_CAP", "DR_SWAP",
#ifdef CONFIG_USB_PD_DUAL_ROLE
	"SRC_SWAP_INIT", "SRC_SWAP_SNK_DISABLE", "SRC_SWAP_SRC_DISABLE",
	"SRC_SWAP_STANDBY",
#ifdef CONFIG_USBC_VCONN_SWAP
	"VCONN_SWAP_SEND", "VCONN_SWAP_INIT", "VCONN_SWAP_READY",
#endif /* CONFIG_USBC_VCONN_SWAP */
#endif /* CONFIG_USB_PD_DUAL_ROLE */
	"SOFT_RESET", "HARD_RESET_SEND", "HARD_RESET_EXECUTE", "BIST_RX",
	"BIST_TX",
#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
	"DRP_AUTO_TOGGLE",
#endif
};
BUILD_ASSERT(ARRAY_SIZE(pd_state_names) == PD_STATE_COUNT);
#endif

/*
 * 4 entry rw_hash table of type-C devices that AP has firmware updates for.
 */
#ifdef CONFIG_COMMON_RUNTIME
#define RW_HASH_ENTRIES 4
static struct ec_params_usb_pd_rw_hash_entry rw_hash_table[RW_HASH_ENTRIES];
#endif

int pd_comm_is_enabled() {
#ifdef CONFIG_COMMON_RUNTIME
	return pd_comm_enabled[port];
#else
	return 1;
#endif
}

static inline void set_state_timeout(uint64_t timeout,
		enum pd_states timeout_state) {
	pd.timeout = timeout;
	pd.timeout_state = timeout_state;
}

#ifdef CONFIG_USB_PD_REV30
int pd_get_rev()
{
	return pd.rev;
}

int pd_get_vdo_ver()
{
	return vdo_ver[pd.rev];
}
#endif

/* Return flag for pd state is connected */
int pd_is_connected() {
	if (pd.task_state == PD_STATE_DISABLED)
		return 0;

#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
	if (pd.task_state == PD_STATE_DRP_AUTO_TOGGLE)
		return 0;
#endif

	return DUAL_ROLE_IF_ELSE(
			/* sink */
			pd.task_state != PD_STATE_SNK_DISCONNECTED && pd.task_state != PD_STATE_SNK_DISCONNECTED_DEBOUNCE,
			/* source */
			pd.task_state != PD_STATE_SRC_DISCONNECTED && pd.task_state != PD_STATE_SRC_DISCONNECTED_DEBOUNCE);
}

/*
 * Return true if partner port is a DTS or TS capable of entering debug
 * mode (eg. is presenting Rp/Rp or Rd/Rd).
 */
int pd_ts_dts_plugged() {
	return pd.flags & PD_FLAGS_TS_DTS_PARTNER;
}

#ifdef CONFIG_USB_PD_DUAL_ROLE
void pd_vbus_low() {
	pd.flags &= ~PD_FLAGS_VBUS_NEVER_LOW;
}

static inline int pd_is_vbus_present() {
#ifdef CONFIG_USB_PD_VBUS_DETECT_TCPC
	return tcpm_get_vbus_level();
#else
	return pd_snk_is_vbus_provided();
#endif
}
#endif

static inline void set_state(enum pd_states next_state) {
	enum pd_states last_state = pd.task_state;
#ifdef CONFIG_LOW_POWER_IDLE
	int i;
#endif

	set_state_timeout(0, PD_STATE_DISABLED);
	pd.task_state = next_state;

	if (last_state == next_state)
		return;

#ifdef CONFIG_USB_PD_DUAL_ROLE
#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
	/* Clear flag to allow DRP auto toggle when possible */
	if (last_state != PD_STATE_DRP_AUTO_TOGGLE)
		pd.flags &= ~PD_FLAGS_TCPC_DRP_TOGGLE;
#endif

	/* Ignore dual-role toggling between sink and source */
	if ((last_state == PD_STATE_SNK_DISCONNECTED
			&& next_state == PD_STATE_SRC_DISCONNECTED)
			|| (last_state == PD_STATE_SRC_DISCONNECTED
					&& next_state == PD_STATE_SNK_DISCONNECTED))
		return;

	if (next_state == PD_STATE_SRC_DISCONNECTED
			|| next_state == PD_STATE_SNK_DISCONNECTED) {
		/* Clear the input current limit */
		pd_set_input_current_limit(0, 0);
#ifdef CONFIG_CHARGE_MANAGER
		//typec_set_input_current_limit( 0, 0);
		//charge_manager_set_ceil(
		//			CEIL_REQUESTOR_PD,
		//			CHARGE_CEIL_NONE);
#endif
#ifdef CONFIG_USBC_VCONN
		tcpm_set_vconn( 0);
#endif
#else /* CONFIG_USB_PD_DUAL_ROLE */
	if (next_state == PD_STATE_SRC_DISCONNECTED) {
#endif

#ifdef CONFIG_USB_PD_REV30
		/* Adjust rev to highest level*/
		pd.rev = PD_REV30;
#endif
#ifdef CONFIG_USB_PD_CHROMEOS
		pd.dev_id = 0;
		pd.flags &= ~PD_FLAGS_RESET_ON_DISCONNECT_MASK;
#endif
#ifdef CONFIG_CHARGE_MANAGER
		//charge_manager_update_dualrole( CAP_UNKNOWN);
#endif
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
		pd_dfp_exit_mode( 0, 0);
#endif
#ifdef CONFIG_USBC_SS_MUX
		usb_mux_set( TYPEC_MUX_NONE, USB_SWITCH_DISCONNECT,
			    pd.polarity);
#endif
		/* Disable TCPC RX */
		tcpm_set_rx_enable(0);
	}

#ifdef CONFIG_LOW_POWER_IDLE
	/* If a PD device is attached then disable deep sleep */
	for (i = 0; i < CONFIG_USB_PD_PORT_COUNT; i++) {
		if (pd[i].flags & PD_FLAGS_PREVIOUS_PD_CONN)
			break;
	}
	if (i == CONFIG_USB_PD_PORT_COUNT)
		enable_sleep(SLEEP_MASK_USB_PD);
	else
		disable_sleep(SLEEP_MASK_USB_PD);
#endif

	if (debug_level >= 1)
		CPRINTF("C%d st%d %s\n", port, next_state,
				pd_state_names[next_state]);
	else
		CPRINTF("C%d st%d\n", port, next_state);
}

/* increment message ID counter */
static void inc_id() {
	pd.msg_id = (pd.msg_id + 1) & PD_MESSAGE_ID_COUNT;
}

#ifdef CONFIG_USB_PD_REV30
static void sink_can_xmit( int rp)
{
	tcpm_select_rp_value( rp);
	tcpm_set_cc( TYPEC_CC_RP);
}

static inline void pd_ca_reset()
{
	pd.ca_buffered = 0;
}
#endif

void pd_transmit_complete(int status) {
	if (status == TCPC_TX_COMPLETE_SUCCESS)
		inc_id();

	pd.tx_status = status;
	//task_set_event(PD_PORT_TO_TASK_ID(), PD_EVENT_TX, 0);
	pd_task_set_event(PD_EVENT_TX, 0);
}

static int pd_transmit(enum tcpm_transmit_type type, uint16_t header,
		const uint32_t *data) {
	int evt;

	/* If comms are disabled, do not transmit, return error */
	if (!pd_comm_is_enabled())
		return -1;
#ifdef CONFIG_USB_PD_REV30
	/* Source-coordinated collision avoidance */
	/*
	 * In order to avoid message collisions due to asynchronous Messaging
	 * sent from the Sink, the Source sets Rp to SinkTxOk to indicate to
	 * the Sink that it is ok to initiate an AMS. When the Source wishes
	 * to initiate an AMS it sets Rp to SinkTxNG. When the Sink detects
	 * that Rp is set to SinkTxOk it May initiate an AMS. When the Sink
	 * detects that Rp is set to SinkTxNG it Shall Not initiate an AMS
	 * and Shall only send Messages that are part of an AMS the Source has
	 * initiated. Note that this restriction applies to SOP* AMS’s i.e.
	 * for both Port to Port and Port to Cable Plug communications.
	 *
	 * This starts after an Explicit Contract is in place
	 * PD R3 V1.1 Section 2.5.2.
	 *
	 * Note: a Sink can still send Hard Reset signaling at any time.
	 */
	if ((pd.rev == PD_REV30) &&
		(pd.flags & PD_FLAGS_EXPLICIT_CONTRACT)) {
		if (pd.power_role == PD_ROLE_SOURCE) {
			/*
			 * Inform Sink that it can't transmit. If a sink
			 * transmition is in progress and a collsion occurs,
			 * a reset is generated. This should be rare because
			 * all extended messages are chunked. This effectively
			 * defaults to PD REV 2.0 collision avoidance.
			 */
			sink_can_xmit( SINK_TX_NG);
		} else if (type != TCPC_TX_HARD_RESET) {
			int cc1;
			int cc2;

			tcpm_get_cc( &cc1, &cc2);
			if (cc1 == TYPEC_CC_VOLT_SNK_1_5 ||
				cc2 == TYPEC_CC_VOLT_SNK_1_5) {
				/* Sink can't transmit now. */
				/* Check if message is already buffered. */
				if (pd.ca_buffered)
					return -1;

				/* Buffer message and send later. */
				pd.ca_type = type;
				pd.ca_header = header;
				memcpy(pd.ca_buffer,
					data, sizeof(uint32_t) *
					PD_HEADER_CNT(header));
				pd.ca_buffered = 1;
				return 1;
			}
		}
	}
#endif
	tcpm_transmit(type, header, data);

	/* Wait until TX is complete */
	// Would wait, except that we're making tcpm_transmit blocking
	//evt = task_wait_event_mask(PD_EVENT_TX, PD_T_TCPC_TX_TIMEOUT);
#ifdef CONFIG_USB_PD_REV30
	/*
	 * If the source just completed a transmit, tell
	 * the sink it can transmit if it wants to.
	 */
	if ((pd.rev == PD_REV30) &&
			(pd.power_role == PD_ROLE_SOURCE) &&
			(pd.flags & PD_FLAGS_EXPLICIT_CONTRACT)) {
		sink_can_xmit( SINK_TX_OK);
	}
#endif

	// removing task-based stuff from the library
	//if (evt & TASK_EVENT_TIMER)
	//		return -1;

	/* TODO: give different error condition for failed vs discarded */
	return pd.tx_status == TCPC_TX_COMPLETE_SUCCESS ? 1 : -1;
}

#ifdef CONFIG_USB_PD_REV30
static void pd_ca_send_pending()
{
	int cc1;
	int cc2;

	/* Check if a message has been buffered. */
	if (!pd.ca_buffered)
		return;

	tcpm_get_cc( &cc1, &cc2);
	if ((cc1 != TYPEC_CC_VOLT_SNK_1_5) &&
			(cc2 != TYPEC_CC_VOLT_SNK_1_5))
		if (pd_transmit( pd.ca_type,
				pd.ca_header,
				pd.ca_buffer) < 0)
			return;

	/* Message was sent, so free up the buffer. */
	pd.ca_buffered = 0;
}
#endif

static void pd_update_roles() {
	/* Notify TCPC of role update */
	tcpm_set_msg_header(pd.power_role, pd.data_role);
}

static int send_control(int type) {
	int bit_len;
	uint16_t header = PD_HEADER(type, pd.power_role, pd.data_role, pd.msg_id, 0,
			pd_get_rev(), 0);

	bit_len = pd_transmit(TCPC_TX_SOP, header, NULL);
	if (debug_level >= 2)
		CPRINTF("CTRL[%d]>%d\n", type, bit_len);

	return bit_len;
}

static int send_source_cap() {
	int bit_len;
#if defined(CONFIG_USB_PD_DYNAMIC_SRC_CAP) || \
		defined(CONFIG_USB_PD_MAX_SINGLE_SOURCE_CURRENT)
	const uint32_t *src_pdo;
	const int src_pdo_cnt = charge_manager_get_source_pdo(&src_pdo, port);
#else
	const uint32_t *src_pdo = pd_src_pdo;
	const int src_pdo_cnt = pd_src_pdo_cnt;
#endif
	uint16_t header;

	if (src_pdo_cnt == 0)
		/* No source capabilities defined, sink only */
		header = PD_HEADER(PD_CTRL_REJECT, pd.power_role, pd.data_role,
				pd.msg_id, 0, pd_get_rev(), 0);
	else
		header = PD_HEADER(PD_DATA_SOURCE_CAP, pd.power_role, pd.data_role,
				pd.msg_id, src_pdo_cnt, pd_get_rev(), 0);

	bit_len = pd_transmit(TCPC_TX_SOP, header, src_pdo);
	if (debug_level >= 2)
		CPRINTF("srcCAP>%d\n", bit_len);

	return bit_len;
}

#ifdef CONFIG_USB_PD_REV30
static int send_battery_cap( uint32_t *payload)
{
	int bit_len;
	uint16_t msg[6] = {0, 0, 0, 0, 0, 0};
	uint16_t header = PD_HEADER(PD_EXT_BATTERY_CAP,
				    pd.power_role,
				    pd.data_role,
				    pd.msg_id,
				    3, /* Number of Data Objects */
				    pd.rev,
				    1  /* This is an exteded message */
				   );

	/* Set extended header */
	msg[0] = PD_EXT_HEADER(0, /* Chunk Number */
			       0, /* Request Chunk */
			       9  /* Data Size in bytes */
			      );
	/* Set VID */
	msg[1] = USB_VID_GOOGLE;

	/* Set PID */
	msg[2] = CONFIG_USB_PID;

	if (battery_is_present()) {
		/*
		 * We only have one fixed battery,
		 * so make sure batt cap ref is 0.
		 */
		if (BATT_CAP_REF(payload[0]) != 0) {
			/* Invalid battery reference */
			msg[5] = 1;
		} else {
			uint32_t v;
			uint32_t c;

			/*
			 * The Battery Design Capacity field shall return the
			 * Battery’s design capacity in tenths of Wh. If the
			 * Battery is Hot Swappable and is not present, the
			 * Battery Design Capacity field shall be set to 0. If
			 * the Battery is unable to report its Design Capacity,
			 * it shall return 0xFFFF
			 */
			msg[3] = 0xffff;

			/*
			 * The Battery Last Full Charge Capacity field shall
			 * return the Battery’s last full charge capacity in
			 * tenths of Wh. If the Battery is Hot Swappable and
			 * is not present, the Battery Last Full Charge Capacity
			 * field shall be set to 0. If the Battery is unable to
			 * report its Design Capacity, the Battery Last Full
			 * Charge Capacity field shall be set to 0xFFFF.
			 */
			msg[4] = 0xffff;

			if (battery_design_voltage(&v) == 0) {
				if (battery_design_capacity(&c) == 0) {
					/*
					 * Wh = (c * v) / 1000000
					 * 10th of a Wh = Wh * 10
					 */
					msg[3] = DIV_ROUND_NEAREST((c * v),
								100000);
				}

				if (battery_full_charge_capacity(&c) == 0) {
					/*
					 * Wh = (c * v) / 1000000
					 * 10th of a Wh = Wh * 10
					 */
					msg[4] = DIV_ROUND_NEAREST((c * v),
								100000);
				}
			}
		}
	}

	bit_len = pd_transmit( TCPC_TX_SOP, header, (uint32_t *)msg);
	if (debug_level >= 2)
		CPRINTF("batCap>%d\n", bit_len);
	return bit_len;
}

static int send_battery_status(  uint32_t *payload)
{
	int bit_len;
	uint32_t msg = 0;
	uint16_t header = PD_HEADER(PD_DATA_BATTERY_STATUS,
				    pd.power_role,
				    pd.data_role,
				    pd.msg_id,
				    1, /* Number of Data Objects */
				    pd.rev,
				    0 /* This is NOT an extended message */
				  );

	if (battery_is_present()) {
		/*
		 * We only have one fixed battery,
		 * so make sure batt cap ref is 0.
		 */
		if (BATT_CAP_REF(payload[0]) != 0) {
			/* Invalid battery reference */
			msg |= BSDO_INVALID;
		} else {
			uint32_t v;
			uint32_t c;

			if (battery_design_voltage(&v) != 0 ||
					battery_remaining_capacity(&c) != 0) {
				msg |= BSDO_CAP(BSDO_CAP_UNKNOWN);
			} else {
				/*
				 * Wh = (c * v) / 1000000
				 * 10th of a Wh = Wh * 10
				 */
				msg |= BSDO_CAP(DIV_ROUND_NEAREST((c * v),
								100000));
			}

			/* Battery is present */
			msg |= BSDO_PRESENT;

			/*
			 * For drivers that are not smart battery compliant,
			 * battery_status() returns EC_ERROR_UNIMPLEMENTED and
			 * the battery is assumed to be idle.
			 */
			if (battery_status(&c) != 0) {
				msg |= BSDO_IDLE; /* assume idle */
			} else {
				if (c & STATUS_FULLY_CHARGED)
					/* Fully charged */
					msg |= BSDO_IDLE;
				else if (c & STATUS_DISCHARGING)
					/* Discharging */
					msg |= BSDO_DISCHARGING;
				/* else battery is charging.*/
			}
		}
	} else {
		msg = BSDO_CAP(BSDO_CAP_UNKNOWN);
	}

	bit_len = pd_transmit( TCPC_TX_SOP, header, &msg);
	if (debug_level >= 2)
		CPRINTF("batStat>%d\n", bit_len);

	return bit_len;
}
#endif

#ifdef CONFIG_USB_PD_DUAL_ROLE
static void send_sink_cap() {
	int bit_len;
	uint16_t header = PD_HEADER(PD_DATA_SINK_CAP, pd.power_role,
			pd.data_role, pd.msg_id, pd_snk_pdo_cnt, pd_get_rev(),
			0);

	bit_len = pd_transmit(TCPC_TX_SOP, header, pd_snk_pdo);
	if (debug_level >= 2)
		CPRINTF("snkCAP>%d\n", bit_len);
}

static int send_request(uint32_t rdo) {
	int bit_len;
	uint16_t header = PD_HEADER(PD_DATA_REQUEST, pd.power_role,
			pd.data_role, pd.msg_id, 1, pd_get_rev(), 0);

	bit_len = pd_transmit(TCPC_TX_SOP, header, &rdo);
	if (debug_level >= 2)
		CPRINTF("REQ%d>\n", bit_len);

	return bit_len;
}
#ifdef CONFIG_BBRAM
static int pd_get_saved_active()
{
	uint8_t val;

	if (system_get_bbram(port ? SYSTEM_BBRAM_IDX_PD1 :
				    SYSTEM_BBRAM_IDX_PD0, &val)) {
		CPRINTS("PD NVRAM FAIL");
		return 0;
	}
	return !!val;
}

static void pd_set_saved_active( int val)
{
	if (system_set_bbram(port ? SYSTEM_BBRAM_IDX_PD1 :
				    SYSTEM_BBRAM_IDX_PD0, val))
		CPRINTS("PD NVRAM FAIL");
}
#endif // CONFIG_BBRAM
#endif /* CONFIG_USB_PD_DUAL_ROLE */

#ifdef CONFIG_COMMON_RUNTIME
static int send_bist_cmd()
{
	/* currently only support sending bist carrier 2 */
	uint32_t bdo = BDO(BDO_MODE_CARRIER2, 0);
	int bit_len;
	uint16_t header = PD_HEADER(PD_DATA_BIST, pd.power_role,
			pd.data_role, pd.msg_id, 1,
			pd_get_rev(), 0);

	bit_len = pd_transmit( TCPC_TX_SOP, header, &bdo);
	CPRINTF("BIST>%d\n", bit_len);

	return bit_len;
}
#endif

static void queue_vdm(uint32_t *header, const uint32_t *data, int data_cnt) {
	pd.vdo_count = data_cnt + 1;
	pd.vdo_data[0] = header[0];
	memcpy(&pd.vdo_data[1], data, sizeof(uint32_t) * data_cnt);
	/* Set ready, pd task will actually send */
	pd.vdm_state = VDM_STATE_READY;
}

static void handle_vdm_request(int cnt, uint32_t *payload) {
	int rlen = 0;
	uint32_t *rdata;

	if (pd.vdm_state == VDM_STATE_BUSY) {
		/* If UFP responded busy retry after timeout */
		if (PD_VDO_CMDT(payload[0]) == CMDT_RSP_BUSY) {
			pd.vdm_timeout.val = get_time().val +
			PD_T_VDM_BUSY;
			pd.vdm_state = VDM_STATE_WAIT_RSP_BUSY;
			pd.vdo_retry = (payload[0] & ~VDO_CMDT_MASK) |
			CMDT_INIT;
			return;
		} else {
			pd.vdm_state = VDM_STATE_DONE;
		}
	}

	if (PD_VDO_SVDM(payload[0]))
		rlen = pd_svdm(cnt, payload, &rdata);

	if (rlen > 0) {
		queue_vdm(rdata, &rdata[1], rlen - 1);
		return;
	}
	if (debug_level >= 2)
		CPRINTF("Unhandled VDM VID %04x CMD %04x\n",
				PD_VDO_VID(payload[0]), payload[0] & 0xFFFF);
}

void pd_execute_hard_reset() {
	if (pd.last_state == PD_STATE_HARD_RESET_SEND)
		CPRINTF("C%d HARD RST TX\n", port);
	else
		CPRINTF("C%d HARD RST RX\n", port);

	pd.msg_id = 0;
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
	pd_dfp_exit_mode( 0, 0);
#endif

#ifdef CONFIG_USB_PD_REV30
	pd.rev = PD_REV30;
	pd_ca_reset();
#endif
	/*
	 * Fake set last state to hard reset to make sure that the next
	 * state to run knows that we just did a hard reset.
	 */
	pd.last_state = PD_STATE_HARD_RESET_EXECUTE;

#ifdef CONFIG_USB_PD_DUAL_ROLE
	/*
	 * If we are swapping to a source and have changed to Rp, restore back
	 * to Rd and turn off vbus to match our power_role.
	 */
	if (pd.task_state == PD_STATE_SNK_SWAP_STANDBY
			|| pd.task_state == PD_STATE_SNK_SWAP_COMPLETE) {
		tcpm_set_cc(TYPEC_CC_RD);
		pd_power_supply_reset();
	}

	if (pd.power_role == PD_ROLE_SINK) {
		/* Clear the input current limit */
		pd_set_input_current_limit(0, 0);
#ifdef CONFIG_CHARGE_MANAGER
		//charge_manager_set_ceil(
		//			CEIL_REQUESTOR_PD,
		//			CHARGE_CEIL_NONE);
#endif /* CONFIG_CHARGE_MANAGER */

		set_state (PD_STATE_SNK_HARD_RESET_RECOVER);
		return;
	}
#endif /* CONFIG_USB_PD_DUAL_ROLE */

	/* We are a source, cut power */
	pd_power_supply_reset();
	pd.src_recover = get_time().val + PD_T_SRC_RECOVER;
	set_state(PD_STATE_SRC_HARD_RESET_RECOVER);
}

static void execute_soft_reset() {
	pd.msg_id = 0;
	set_state(
			DUAL_ROLE_IF_ELSE(PD_STATE_SNK_DISCOVERY, PD_STATE_SRC_DISCOVERY));
	CPRINTF("C%d Soft Rst\n", port);
}

void pd_soft_reset(void) {
	int i;

	if (pd_is_connected()) {
		set_state(PD_STATE_SOFT_RESET);
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID(i));
	}
}

#ifdef CONFIG_USB_PD_DUAL_ROLE
/*
 * Request desired charge voltage from source.
 * Returns EC_SUCCESS on success or non-zero on failure.
 */
static int pd_send_request_msg(int always_send_request) {
	uint32_t rdo, curr_limit, supply_voltage;
	int res;

#ifdef CONFIG_CHARGE_MANAGER
	//int charging = (charge_manager_get_active_charge_port() == port);
	const int charging = 1;
#else
	const int charging = 1;
#endif

#ifdef CONFIG_USB_PD_CHECK_MAX_REQUEST_ALLOWED
	int max_request_allowed = pd_is_max_request_allowed();
#else
	const int max_request_allowed = 1;
#endif

	/* Clear new power request */
	pd.new_power_request = 0;

	/* Build and send request RDO */
	/*
	 * If this port is not actively charging or we are not allowed to
	 * request the max voltage, then select vSafe5V
	 */
	res = pd_build_request(&rdo, &curr_limit, &supply_voltage,
			charging && max_request_allowed ?
					PD_REQUEST_MAX : PD_REQUEST_VSAFE5V);

	if (res != EC_SUCCESS)
		/*
		 * If fail to choose voltage, do nothing, let source re-send
		 * source cap
		 */
		return -1;

	if (!always_send_request) {
		/* Don't re-request the same voltage */
		if (pd.prev_request_mv == supply_voltage)
			return EC_SUCCESS;
#ifdef CONFIG_CHARGE_MANAGER
		/* Limit current to PD_MIN_MA during transition */
		//else
		//	charge_manager_force_ceil( PD_MIN_MA);
#endif
	}

	CPRINTF("Req C%d [%d] %dmV %dmA", port, RDO_POS(rdo),
			supply_voltage, curr_limit);
	if (rdo & RDO_CAP_MISMATCH)
		CPRINTF(" Mismatch");CPRINTF("\n");

	pd.curr_limit = curr_limit;
	pd.supply_voltage = supply_voltage;
	pd.prev_request_mv = supply_voltage;
	res = send_request(rdo);
	if (res < 0)
		return res;
	set_state (PD_STATE_SNK_REQUESTED);
	return EC_SUCCESS;
}
#endif

static void pd_update_pdo_flags(uint32_t pdo) {
#ifdef CONFIG_CHARGE_MANAGER
#ifdef CONFIG_USB_PD_ALT_MODE_DFP
	int charge_whitelisted =
		(pd.power_role == PD_ROLE_SINK &&
		 pd_charge_from_device(pd_get_identity_vid(),
				       pd_get_identity_pid()));
#else
	const int charge_whitelisted = 0;
#endif
#endif

	/* can only parse PDO flags if type is fixed */
	if ((pdo & PDO_TYPE_MASK) != PDO_TYPE_FIXED)
		return;

#ifdef CONFIG_USB_PD_DUAL_ROLE
	if (pdo & PDO_FIXED_DUAL_ROLE)
		pd.flags |= PD_FLAGS_PARTNER_DR_POWER;
	else
		pd.flags &= ~PD_FLAGS_PARTNER_DR_POWER;

	if (pdo & PDO_FIXED_EXTERNAL)
		pd.flags |= PD_FLAGS_PARTNER_EXTPOWER;
	else
		pd.flags &= ~PD_FLAGS_PARTNER_EXTPOWER;

	if (pdo & PDO_FIXED_COMM_CAP)
		pd.flags |= PD_FLAGS_PARTNER_USB_COMM;
	else
		pd.flags &= ~PD_FLAGS_PARTNER_USB_COMM;
#endif

	if (pdo & PDO_FIXED_DATA_SWAP)
		pd.flags |= PD_FLAGS_PARTNER_DR_DATA;
	else
		pd.flags &= ~PD_FLAGS_PARTNER_DR_DATA;

#ifdef CONFIG_CHARGE_MANAGER
	/*
	 * Treat device as a dedicated charger (meaning we should charge
	 * from it) if it does not support power swap, or if it is externally
	 * powered, or if we are a sink and the device identity matches a
	 * charging white-list.
	 */
	/*
	 if (!(pd.flags & PD_FLAGS_PARTNER_DR_POWER) ||
	 (pd.flags & PD_FLAGS_PARTNER_EXTPOWER) ||
	 charge_whitelisted)
	 charge_manager_update_dualrole( CAP_DEDICATED);
	 else
	 charge_manager_update_dualrole( CAP_DUALROLE);
	 */
#endif
}

static void handle_data_request(uint16_t head, uint32_t *payload) {
	int type = PD_HEADER_TYPE(head);
	int cnt = PD_HEADER_CNT(head);

	switch (type) {
#ifdef CONFIG_USB_PD_DUAL_ROLE
	case PD_DATA_SOURCE_CAP:
		if ((pd.task_state == PD_STATE_SNK_DISCOVERY)
				|| (pd.task_state == PD_STATE_SNK_TRANSITION)
#ifdef CONFIG_USB_PD_VBUS_DETECT_NONE
			|| (pd.task_state ==
			    PD_STATE_SNK_HARD_RESET_RECOVER)
#endif
				|| (pd.task_state == PD_STATE_SNK_READY)) {
#ifdef CONFIG_USB_PD_REV30
			/*
			 * Only adjust sink rev if source rev is higher.
			 */
			if (PD_HEADER_REV(head) < pd.rev)
				pd.rev = PD_HEADER_REV(head);
#endif
			/* Port partner is now known to be PD capable */
			pd.flags |= PD_FLAGS_PREVIOUS_PD_CONN;

			/* src cap 0 should be fixed PDO */
			pd_update_pdo_flags(payload[0]);

			pd_process_source_cap(cnt, payload);

			/* Source will resend source cap on failure */
			pd_send_request_msg(1);

			// We call the callback after we send the request
			// because the timing on Request seems to be sensitive
			// User code can take the time until PS_RDY to do stuff
			pd_process_source_cap_callback(cnt, payload);
		}
		break;
#endif /* CONFIG_USB_PD_DUAL_ROLE */
	case PD_DATA_REQUEST:

		/* the message was incorrect or cannot be satisfied */
		send_control(PD_CTRL_REJECT);
		/* keep last contract in place (whether implicit or explicit) */
		set_state(PD_STATE_SRC_READY);
		break;
	case PD_DATA_BIST:
		/* If not in READY state, then don't start BIST */
		if (DUAL_ROLE_IF_ELSE(pd.task_state == PD_STATE_SNK_READY,
				pd.task_state == PD_STATE_SRC_READY)) {
			/* currently only support sending bist carrier mode 2 */
			if ((payload[0] >> 28) == 5) {
				/* bist data object mode is 2 */
				pd_transmit(TCPC_TX_BIST_MODE_2, 0,
				NULL);
				/* Set to appropriate port disconnected state */
				set_state(
						DUAL_ROLE_IF_ELSE(PD_STATE_SNK_DISCONNECTED,
								PD_STATE_SRC_DISCONNECTED));
			}
		}
		break;
	case PD_DATA_SINK_CAP:
		pd.flags |= PD_FLAGS_SNK_CAP_RECVD;
		/* snk cap 0 should be fixed PDO */
		pd_update_pdo_flags(payload[0]);
		if (pd.task_state == PD_STATE_SRC_GET_SINK_CAP)
			set_state(PD_STATE_SRC_READY);
		break;
#ifdef CONFIG_USB_PD_REV30
	case PD_DATA_BATTERY_STATUS:
		break;
#endif
	case PD_DATA_VENDOR_DEF:
		handle_vdm_request(cnt, payload);
		break;
	default:
		CPRINTF("Unhandled data message type %d\n", type);
	}
}

#ifdef CONFIG_USB_PD_DUAL_ROLE
void pd_request_power_swap() {
	if (pd.task_state == PD_STATE_SRC_READY)
		set_state (PD_STATE_SRC_SWAP_INIT);
	else if (pd.task_state == PD_STATE_SNK_READY)
		set_state (PD_STATE_SNK_SWAP_INIT);
	// getting rid of task stuff
	//task_wake(PD_PORT_TO_TASK_ID());
}

#ifdef CONFIG_USBC_VCONN_SWAP
static void pd_request_vconn_swap()
{
	if (pd.task_state == PD_STATE_SRC_READY ||
	    pd.task_state == PD_STATE_SNK_READY)
		set_state( PD_STATE_VCONN_SWAP_SEND);
	// getting rid of task stuff
	//task_wake(PD_PORT_TO_TASK_ID());
}

void pd_try_vconn_src()
{
	/*
	 * If we don't currently provide vconn, and we can supply it, send
	 * a vconn swap request.
	 */
	if (!(pd.flags & PD_FLAGS_VCONN_ON)) {
		if (pd_check_vconn_swap())
			pd_request_vconn_swap();
	}
}
#endif
#endif /* CONFIG_USB_PD_DUAL_ROLE */

void pd_request_data_swap() {
	if (DUAL_ROLE_IF_ELSE(pd.task_state == PD_STATE_SNK_READY,
			pd.task_state == PD_STATE_SRC_READY))
		set_state(PD_STATE_DR_SWAP);
	// getting rid of task stuff
	//task_wake(PD_PORT_TO_TASK_ID());
}

static void pd_set_data_role(int role) {
	pd.data_role = role;
	pd_execute_data_swap(role);

#ifdef CONFIG_USBC_SS_MUX
#ifdef CONFIG_USBC_SS_MUX_DFP_ONLY
	/*
	 * Need to connect SS mux for if new data role is DFP.
	 * If new data role is UFP, then disconnect the SS mux.
	 */
	if (role == PD_ROLE_DFP)
		usb_mux_set( TYPEC_MUX_USB, USB_SWITCH_CONNECT,
			    pd.polarity);
	else
		usb_mux_set( TYPEC_MUX_NONE, USB_SWITCH_DISCONNECT,
			    pd.polarity);
#else
	usb_mux_set( TYPEC_MUX_USB, USB_SWITCH_CONNECT,
		    pd.polarity);
#endif
#endif
	pd_update_roles();
}

static void pd_dr_swap() {
	pd_set_data_role(!pd.data_role);
	pd.flags |= PD_FLAGS_CHECK_IDENTITY;
}

static void handle_ctrl_request(uint16_t head, uint32_t *payload) {
	int type = PD_HEADER_TYPE(head);
	int res;

	switch (type) {
	case PD_CTRL_GOOD_CRC:
		/* should not get it */
		break;
	case PD_CTRL_PING:
		/* Nothing else to do */
		break;
	case PD_CTRL_GET_SOURCE_CAP:
		res = send_source_cap();
		if ((res >= 0) && (pd.task_state == PD_STATE_SRC_DISCOVERY))
			set_state(PD_STATE_SRC_NEGOCIATE);
		break;
	case PD_CTRL_GET_SINK_CAP:
#ifdef CONFIG_USB_PD_DUAL_ROLE
		send_sink_cap();
#else
		send_control(REFUSE(pd.rev));
#endif
		break;
#ifdef CONFIG_USB_PD_DUAL_ROLE
	case PD_CTRL_GOTO_MIN:
#ifdef CONFIG_USB_PD_GIVE_BACK
		if (pd.task_state == PD_STATE_SNK_READY) {
			/*
			 * Reduce power consumption now!
			 *
			 * The source will restore power to this sink
			 * by sending a new source cap message at a
			 * later time.
			 */
			pd_snk_give_back( &pd.curr_limit,
				&pd.supply_voltage);
			set_state( PD_STATE_SNK_TRANSITION);
		}
#endif

		break;
	case PD_CTRL_PS_RDY:
		if (pd.task_state == PD_STATE_SNK_SWAP_SRC_DISABLE) {
			set_state (PD_STATE_SNK_SWAP_STANDBY);
		} else if (pd.task_state == PD_STATE_SRC_SWAP_STANDBY) {
			/* reset message ID and swap roles */
			pd.msg_id = 0;
			pd.power_role = PD_ROLE_SINK;
			pd_update_roles();
			set_state (PD_STATE_SNK_DISCOVERY);
#ifdef CONFIG_USBC_VCONN_SWAP
		} else if (pd.task_state == PD_STATE_VCONN_SWAP_INIT) {
			/*
			 * If VCONN is on, then this PS_RDY tells us it's
			 * ok to turn VCONN off
			 */
			if (pd.flags & PD_FLAGS_VCONN_ON)
				set_state( PD_STATE_VCONN_SWAP_READY);
#endif
		} else if (pd.task_state == PD_STATE_SNK_DISCOVERY) {
			/* Don't know what power source is ready. Reset. */
			set_state(PD_STATE_HARD_RESET_SEND);
		} else if (pd.task_state == PD_STATE_SNK_SWAP_STANDBY) {
			/* Do nothing, assume this is a redundant PD_RDY */
		} else if (pd.power_role == PD_ROLE_SINK) {
			set_state (PD_STATE_SNK_READY);
			pd_set_input_current_limit(pd.curr_limit,
					pd.supply_voltage);
#ifdef CONFIG_CHARGE_MANAGER
			/* Set ceiling based on what's negotiated */
			//charge_manager_set_ceil(
			//			CEIL_REQUESTOR_PD,
			//			pd.curr_limit);
#endif
		}
		break;
#endif
	case PD_CTRL_REJECT:
	case PD_CTRL_WAIT:
		if (pd.task_state == PD_STATE_DR_SWAP)
			set_state(READY_RETURN_STATE());
#ifdef CONFIG_USBC_VCONN_SWAP
		else if (pd.task_state == PD_STATE_VCONN_SWAP_SEND)
			set_state( READY_RETURN_STATE());
#endif
#ifdef CONFIG_USB_PD_DUAL_ROLE
		else if (pd.task_state == PD_STATE_SRC_SWAP_INIT)
			set_state(PD_STATE_SRC_READY);
		else if (pd.task_state == PD_STATE_SNK_SWAP_INIT)
			set_state (PD_STATE_SNK_READY);
		else if (pd.task_state == PD_STATE_SNK_REQUESTED) {
			/*
			 * Explicit Contract in place
			 *
			 *  On reception of a WAIT message, transition to
			 *  PD_STATE_SNK_READY after PD_T_SINK_REQUEST ms to
			 *  send another reqest.
			 *
			 *  On reception of a REJECT messag, transition to
			 *  PD_STATE_SNK_READY but don't resend the request.
			 *
			 * NO Explicit Contract in place
			 *
			 *  On reception of a WAIT or REJECT message,
			 *  transition to PD_STATE_SNK_DISCOVERY
			 */
			if (pd.flags & PD_FLAGS_EXPLICIT_CONTRACT) {
				/* We have an explicit contract */
				if (type == PD_CTRL_WAIT) {
					/*
					 * Trigger a new power request when
					 * we enter PD_STATE_SNK_READY
					 */
					pd.new_power_request = 1;

					/*
					 * After the request is triggered,
					 * make sure the request is sent.
					 */
					pd.prev_request_mv = 0;

					/*
					 * Transition to PD_STATE_SNK_READY
					 * after PD_T_SINK_REQUEST ms.
					 */
					set_state_timeout(get_time().val +
					PD_T_SINK_REQUEST, PD_STATE_SNK_READY);
				} else {
					/* The request was rejected */
					set_state(PD_STATE_SNK_READY);
				}
			} else {
				/* No explicit contract */
				set_state (PD_STATE_SNK_DISCOVERY);
			}
		}
#endif
		break;
	case PD_CTRL_ACCEPT:
		if (pd.task_state == PD_STATE_SOFT_RESET) {
			/*
			 * For the case that we sent soft reset in SNK_DISCOVERY
			 * on startup due to VBUS never low, clear the flag.
			 */
			pd.flags &= ~PD_FLAGS_VBUS_NEVER_LOW;
			execute_soft_reset();
		} else if (pd.task_state == PD_STATE_DR_SWAP) {
			/* switch data role */
			pd_dr_swap();
			set_state(READY_RETURN_STATE());
#ifdef CONFIG_USB_PD_DUAL_ROLE
#ifdef CONFIG_USBC_VCONN_SWAP
		} else if (pd.task_state == PD_STATE_VCONN_SWAP_SEND) {
			/* switch vconn */
			set_state( PD_STATE_VCONN_SWAP_INIT);
#endif
		} else if (pd.task_state == PD_STATE_SRC_SWAP_INIT) {
			/* explicit contract goes away for power swap */
			pd.flags &= ~PD_FLAGS_EXPLICIT_CONTRACT;
			set_state (PD_STATE_SRC_SWAP_SNK_DISABLE);
		} else if (pd.task_state == PD_STATE_SNK_SWAP_INIT) {
			/* explicit contract goes away for power swap */
			pd.flags &= ~PD_FLAGS_EXPLICIT_CONTRACT;
			set_state (PD_STATE_SNK_SWAP_SNK_DISABLE);
		} else if (pd.task_state == PD_STATE_SNK_REQUESTED) {
			/* explicit contract is now in place */
			pd.flags |= PD_FLAGS_EXPLICIT_CONTRACT;
#ifdef CONFIG_BBRAM
			pd_set_saved_active( 1);
#endif
			set_state (PD_STATE_SNK_TRANSITION);
#endif
		}
		break;
	case PD_CTRL_SOFT_RESET:
		execute_soft_reset();
		/* We are done, acknowledge with an Accept packet */
		send_control(PD_CTRL_ACCEPT);
		break;
	case PD_CTRL_PR_SWAP:
#ifdef CONFIG_USB_PD_DUAL_ROLE
		if (pd_check_power_swap()) {
			send_control(PD_CTRL_ACCEPT);
			/*
			 * Clear flag for checking power role to avoid
			 * immediately requesting another swap.
			 */
			pd.flags &= ~PD_FLAGS_CHECK_PR_ROLE;
			set_state(
					DUAL_ROLE_IF_ELSE(PD_STATE_SNK_SWAP_SNK_DISABLE,
							PD_STATE_SRC_SWAP_SNK_DISABLE));
		} else {
			send_control(REFUSE(pd.rev));
		}
#else
		send_control(REFUSE(pd.rev));
#endif
		break;
	case PD_CTRL_DR_SWAP:
		if (pd_check_data_swap(pd.data_role)) {
			/*
			 * Accept switch and perform data swap. Clear
			 * flag for checking data role to avoid
			 * immediately requesting another swap.
			 */
			pd.flags &= ~PD_FLAGS_CHECK_DR_ROLE;
			if (send_control(PD_CTRL_ACCEPT) >= 0)
				pd_dr_swap();
		} else {
			send_control(REFUSE(pd.rev));

		}
		break;
	case PD_CTRL_VCONN_SWAP:
#ifdef CONFIG_USBC_VCONN_SWAP
		if (pd.task_state == PD_STATE_SRC_READY ||
		    pd.task_state == PD_STATE_SNK_READY) {
			if (pd_check_vconn_swap()) {
				if (send_control( PD_CTRL_ACCEPT) > 0)
					set_state(
						  PD_STATE_VCONN_SWAP_INIT);
			} else {
				send_control( REFUSE(pd.rev));
			}
		}
#else
		send_control(REFUSE(pd.rev));
#endif
		break;
	default:
#ifdef CONFIG_USB_PD_REV30
		send_control( PD_CTRL_NOT_SUPPORTED);
#endif
		CPRINTF("Unhandled ctrl message type %d\n", type);
	}
}

#ifdef CONFIG_USB_PD_REV30
static void handle_ext_request( uint16_t head, uint32_t *payload)
{
	int type = PD_HEADER_TYPE(head);

	switch (type) {
	case PD_EXT_GET_BATTERY_CAP:
		send_battery_cap( payload);
		break;
	case PD_EXT_GET_BATTERY_STATUS:
		send_battery_status( payload);
		break;
	case PD_EXT_BATTERY_CAP:
		break;
	default:
		send_control( PD_CTRL_NOT_SUPPORTED);
	}
}
#endif

static void handle_request(uint16_t head, uint32_t *payload) {
	int cnt = PD_HEADER_CNT(head);
	int p;

	/* dump received packet content (only dump ping at debug level 3) */
	if ((debug_level == 2 && PD_HEADER_TYPE(head) != PD_CTRL_PING)
			|| debug_level >= 3) {
		CPRINTF("RECV %04x/%d ", head, cnt);
		for (p = 0; p < cnt; p++)
			CPRINTF("[%d]%08x ", p, payload[p]);CPRINTF("\n");
	}

	/*
	 * If we are in disconnected state, we shouldn't get a request. 
	 * Ignore it if we get one.
	 */
	if (!pd_is_connected())
		return;

#ifdef CONFIG_USB_PD_REV30
	/* Check if this is an extended chunked data message. */
	if (pd.rev == PD_REV30 && PD_HEADER_EXT(head)) {
		handle_ext_request( head, payload);
		return;
	}
#endif
	if (cnt)
		handle_data_request(head, payload);
	else
		handle_ctrl_request(head, payload);
}

void pd_send_vdm(uint32_t vid, int cmd, const uint32_t *data, int count) {
	if (count > VDO_MAX_SIZE - 1) {
		CPRINTF("VDM over max size\n");
		return;
	}

	/* set VDM header with VID & CMD */
	pd.vdo_data[0] =
			VDO(vid,
					((vid & USB_SID_PD) == USB_SID_PD) ? 1 : (PD_VDO_CMD(cmd) <= CMD_ATTENTION),
					cmd);
#ifdef CONFIG_USB_PD_REV30
	pd.vdo_data[0] |= VDO_SVDM_VERS(vdo_ver[pd.rev]);
#endif
	queue_vdm(pd.vdo_data, data, count);

	// getting rid of task stuff
	//task_wake(PD_PORT_TO_TASK_ID());
}

static inline int pdo_busy() {
	/*
	 * Note, main PDO state machine (pd_task) uses READY state exclusively
	 * to denote port partners have successfully negociated a contract.  All
	 * other protocol actions force state transitions.
	 */
	int rv = (pd.task_state != PD_STATE_SRC_READY);
#ifdef CONFIG_USB_PD_DUAL_ROLE
	rv &= (pd.task_state != PD_STATE_SNK_READY);
#endif
	return rv;
}

static uint64_t vdm_get_ready_timeout(uint32_t vdm_hdr) {
	uint64_t timeout;
	int cmd = PD_VDO_CMD(vdm_hdr);

	/* its not a structured VDM command */
	if (!PD_VDO_SVDM(vdm_hdr))
		return 500 * MSEC;

	switch (PD_VDO_CMDT(vdm_hdr)) {
	case CMDT_INIT:
		if ((cmd == CMD_ENTER_MODE) || (cmd == CMD_EXIT_MODE))
			timeout = PD_T_VDM_WAIT_MODE_E;
		else
			timeout = PD_T_VDM_SNDR_RSP;
		break;
	default:
		if ((cmd == CMD_ENTER_MODE) || (cmd == CMD_EXIT_MODE))
			timeout = PD_T_VDM_E_MODE;
		else
			timeout = PD_T_VDM_RCVR_RSP;
		break;
	}
	return timeout;
}

static void pd_vdm_send_state_machine() {
	int res;
	uint16_t header;

	switch (pd.vdm_state) {
	case VDM_STATE_READY:
		/* Only transmit VDM if connected. */
		if (!pd_is_connected()) {
			pd.vdm_state = VDM_STATE_ERR_BUSY;
			break;
		}

		/*
		 * if there's traffic or we're not in PDO ready state don't send
		 * a VDM.
		 */
		if (pdo_busy())
			break;

		/* Prepare and send VDM */
		header = PD_HEADER(PD_DATA_VENDOR_DEF, pd.power_role, pd.data_role,
				pd.msg_id, (int )pd.vdo_count, pd_get_rev(), 0);
		res = pd_transmit(TCPC_TX_SOP, header, pd.vdo_data);
		if (res < 0) {
			pd.vdm_state = VDM_STATE_ERR_SEND;
		} else {
			pd.vdm_state = VDM_STATE_BUSY;
			pd.vdm_timeout.val = get_time().val
					+ vdm_get_ready_timeout(pd.vdo_data[0]);
		}
		break;
	case VDM_STATE_WAIT_RSP_BUSY:
		/* wait and then initiate request again */
		if (get_time().val > pd.vdm_timeout.val) {
			pd.vdo_data[0] = pd.vdo_retry;
			pd.vdo_count = 1;
			pd.vdm_state = VDM_STATE_READY;
		}
		break;
	case VDM_STATE_BUSY:
		/* Wait for VDM response or timeout */
		if (pd.vdm_timeout.val && (get_time().val > pd.vdm_timeout.val)) {
			pd.vdm_state = VDM_STATE_ERR_TMOUT;
		}
		break;
	default:
		break;
	}
}

#ifdef CONFIG_CMD_PD_DEV_DUMP_INFO
static inline void pd_dev_dump_info(uint16_t dev_id, uint8_t *hash)
{
	int j;
	ccprintf("DevId:%d.%d Hash:", HW_DEV_ID_MAJ(dev_id),
		 HW_DEV_ID_MIN(dev_id));
	for (j = 0; j < PD_RW_HASH_SIZE; j += 4) {
		ccprintf(" 0x%02x%02x%02x%02x", hash[j + 3], hash[j + 2],
			 hash[j + 1], hash[j]);
	}
	ccprintf("\n");
}
#endif /* CONFIG_CMD_PD_DEV_DUMP_INFO */

int pd_dev_store_rw_hash(uint16_t dev_id, uint32_t *rw_hash,
		uint32_t current_image) {
#ifdef CONFIG_COMMON_RUNTIME
	int i;
#endif

#ifdef CONFIG_USB_PD_CHROMEOS
	pd.dev_id = dev_id;
	memcpy(pd.dev_rw_hash, rw_hash, PD_RW_HASH_SIZE);
#endif
#ifdef CONFIG_CMD_PD_DEV_DUMP_INFO
	if (debug_level >= 2)
		pd_dev_dump_info(dev_id, (uint8_t *)rw_hash);
#endif
#ifdef CONFIG_USB_PD_CHROMEOS
	pd.current_image = current_image;
#endif

#ifdef CONFIG_COMMON_RUNTIME
	/* Search table for matching device / hash */
	for (i = 0; i < RW_HASH_ENTRIES; i++)
		if (dev_id == rw_hash_table[i].dev_id)
			return !memcmp(rw_hash,
				       rw_hash_table[i].dev_rw_hash,
				       PD_RW_HASH_SIZE);
#endif
	return 0;
}

#ifdef CONFIG_USB_PD_DUAL_ROLE
enum pd_dual_role_states pd_get_dual_role(void) {
	return drp_state;
}

#ifdef CONFIG_USB_PD_TRY_SRC
static void pd_update_try_source(void)
{
	int i;

#ifndef CONFIG_CHARGER
	int batt_soc = board_get_battery_soc();
#else
	int batt_soc = charge_get_percent();
#endif

	/*
	 * Enable try source when dual-role toggling AND battery is present
	 * and at some minimum percentage.
	 */
	pd_try_src_enable = drp_state == PD_DRP_TOGGLE_ON &&
			    batt_soc >= CONFIG_USB_PD_TRY_SRC_MIN_BATT_SOC;
#if defined(CONFIG_BATTERY_PRESENT_CUSTOM) || \
	defined(CONFIG_BATTERY_PRESENT_GPIO)
	/*
	 * When battery is cutoff in ship mode it may not be reliable to
	 * check if battery is present with its state of charge.
	 * Also check if battery is initialized and ready to provide power.
	 */
	pd_try_src_enable &= (battery_is_present() == BP_YES);
#endif

	/*
	 * Clear this flag to cover case where a TrySrc
	 * mode went from enabled to disabled and trying_source
	 * was active at that time.
	 */
	for (i = 0; i < CONFIG_USB_PD_PORT_COUNT; i++)
		pd[i].flags &= ~PD_FLAGS_TRY_SRC;

}
DECLARE_HOOK(HOOK_BATTERY_SOC_CHANGE, pd_update_try_source, HOOK_PRIO_DEFAULT);
#endif

void pd_set_dual_role(enum pd_dual_role_states state) {
	int i;
	drp_state = state;

#ifdef CONFIG_USB_PD_TRY_SRC
	pd_update_try_source();
#endif

	/* Inform PD tasks of dual role change. */
	for (i = 0; i < CONFIG_USB_PD_PORT_COUNT; i++)
		// getting rid of task stuff
		//task_set_event(PD_PORT_TO_TASK_ID(i),
		//	       PD_EVENT_UPDATE_DUAL_ROLE, 0);
		;
}

void pd_update_dual_role_config() {
	/*
	 * Change to sink if port is currently a source AND (new DRP
	 * state is force sink OR new DRP state is either toggle off
	 * or debug accessory toggle only and we are in the source
	 * disconnected state).
	 */
	if (pd.power_role == PD_ROLE_SOURCE
			&& ((drp_state == PD_DRP_FORCE_SINK && !pd_ts_dts_plugged())
					|| (drp_state == PD_DRP_TOGGLE_OFF
							&& pd.task_state == PD_STATE_SRC_DISCONNECTED))) {
		pd.power_role = PD_ROLE_SINK;
		set_state (PD_STATE_SNK_DISCONNECTED);
		tcpm_set_cc(TYPEC_CC_RD);
		/* Make sure we're not sourcing VBUS. */
		pd_power_supply_reset();
	}

	/*
	 * Change to source if port is currently a sink and the
	 * new DRP state is force source.
	 */
	if (pd.power_role == PD_ROLE_SINK
			&& drp_state == PD_DRP_FORCE_SOURCE) {
		pd.power_role = PD_ROLE_SOURCE;
		set_state(PD_STATE_SRC_DISCONNECTED);
		tcpm_set_cc(TYPEC_CC_RP);
	}

#if defined(CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE) && \
	defined(CONFIG_USB_PD_TCPC_LOW_POWER)
	/* When switching drp mode, make sure tcpc is out of standby mode */
	tcpm_set_drp_toggle( 0);
#endif
}

int pd_get_role() {
	return pd.power_role;
}

static int pd_is_power_swapping() {
	/* return true if in the act of swapping power roles */
	return pd.task_state == PD_STATE_SNK_SWAP_SNK_DISABLE
			|| pd.task_state == PD_STATE_SNK_SWAP_SRC_DISABLE
			|| pd.task_state == PD_STATE_SNK_SWAP_STANDBY
			|| pd.task_state == PD_STATE_SNK_SWAP_COMPLETE
			|| pd.task_state == PD_STATE_SRC_SWAP_SNK_DISABLE
			|| pd.task_state == PD_STATE_SRC_SWAP_SRC_DISABLE
			|| pd.task_state == PD_STATE_SRC_SWAP_STANDBY;
}

/*
 * Provide Rp to ensure the partner port is in a known state (eg. not
 * PD negotiated, not sourcing 20V).
 */
static void pd_partner_port_reset() {
	uint64_t timeout;

#ifdef CONFIG_BBRAM
	/*
	 * Check our battery-backed previous port state. If PD comms were
	 * active, and we didn't just lose power, make sure we
	 * don't boot into RO with a pre-existing power contract.
	 */
	if (!pd_get_saved_active() ||
	   system_get_image_copy() != SYSTEM_IMAGE_RO ||
	   system_get_reset_flags() &
	   (RESET_FLAG_BROWNOUT | RESET_FLAG_POWER_ON))
		return;
#endif // CONFIG_BBRAM
	/* Provide Rp for 100 msec. or until we no longer have VBUS. */
	tcpm_set_cc(TYPEC_CC_RP);
	timeout = get_time().val + 100 * MSEC;

	while (get_time().val < timeout && pd_is_vbus_present())
		msleep(10);
#ifdef CONFIG_BBRAM
	pd_set_saved_active( 0);
#endif
}
#endif /* CONFIG_USB_PD_DUAL_ROLE */

int pd_get_polarity() {
	return pd.polarity;
}

int pd_get_partner_data_swap_capable() {
	/* return data swap capable status of port partner */
	return pd.flags & PD_FLAGS_PARTNER_DR_DATA;
}

#ifdef CONFIG_COMMON_RUNTIME
void pd_comm_enable( int enable)
{
	/* We don't check port >= CONFIG_USB_PD_PORT_COUNT deliberately */
	pd_comm_enabled[port] = enable;

	/* If type-C connection, then update the TCPC RX enable */
	if (pd_is_connected())
		tcpm_set_rx_enable( enable);

#ifdef CONFIG_USB_PD_DUAL_ROLE
	/*
	 * If communications are enabled, start hard reset timer for
	 * any port in PD_SNK_DISCOVERY.
	 */
	if (enable && pd.task_state == PD_STATE_SNK_DISCOVERY)
		set_state_timeout(
				  get_time().val + PD_T_SINK_WAIT_CAP,
				  PD_STATE_HARD_RESET_SEND);
#endif
}
#endif

void pd_ping_enable(int enable) {
	if (enable)
		pd.flags |= PD_FLAGS_PING_ENABLED;
	else
		pd.flags &= ~PD_FLAGS_PING_ENABLED;
}

/**
 * Returns whether the sink has detected a Rp resistor on the other side.
 */
static inline int cc_is_rp(int cc) {
	return (cc == TYPEC_CC_VOLT_SNK_DEF) || (cc == TYPEC_CC_VOLT_SNK_1_5)
			|| (cc == TYPEC_CC_VOLT_SNK_3_0);
}

/*
 * CC values for regular sources and Debug sources (aka DTS)
 *
 * Source type  Mode of Operation   CC1    CC2
 * ---------------------------------------------
 * Regular      Default USB Power   RpUSB  Open
 * Regular      USB-C @ 1.5 A       Rp1A5  Open
 * Regular      USB-C @ 3 A         Rp3A0  Open
 * DTS          Default USB Power   Rp3A0  Rp1A5
 * DTS          USB-C @ 1.5 A       Rp1A5  RpUSB
 * DTS          USB-C @ 3 A         Rp3A0  RpUSB
 */

/**
 * Returns the polarity of a Sink.
 */
static inline int get_snk_polarity(int cc1, int cc2) {
	/* the following assumes:
	 * TYPEC_CC_VOLT_SNK_3_0 > TYPEC_CC_VOLT_SNK_1_5
	 * TYPEC_CC_VOLT_SNK_1_5 > TYPEC_CC_VOLT_SNK_DEF
	 * TYPEC_CC_VOLT_SNK_DEF > TYPEC_CC_VOLT_OPEN
	 */
	return (cc2 > cc1);
}

#if defined(CONFIG_CHARGE_MANAGER)
/**
 * Returns type C current limit (mA) based upon cc_voltage (mV).
 */
static typec_current_t get_typec_current_limit(int polarity, int cc1, int cc2) {
	typec_current_t charge;
	int cc = polarity ? cc2 : cc1;
	int cc_alt = polarity ? cc1 : cc2;

	if (cc == TYPEC_CC_VOLT_SNK_3_0 && cc_alt != TYPEC_CC_VOLT_SNK_1_5)
		charge = 3000;
	else if (cc == TYPEC_CC_VOLT_SNK_1_5)
		charge = 1500;
	else
		charge = 0;

	if (cc_is_rp(cc_alt))
		charge |= TYPEC_CURRENT_DTS_MASK;

	return charge;
}

/**
 * Signal power request to indicate a charger update that affects the port.
 */
void pd_set_new_power_request() {
//	pd.new_power_request = 1;
	// getting rid of task stuff
	//task_wake(PD_PORT_TO_TASK_ID());
}
#endif /* CONFIG_CHARGE_MANAGER */

#if defined(CONFIG_USBC_BACKWARDS_COMPATIBLE_DFP) && defined(CONFIG_USBC_SS_MUX)
/*
 * Backwards compatible DFP does not support USB SS because it applies VBUS
 * before debouncing CC and setting USB SS muxes, but SS detection will fail
 * before we are done debouncing CC.
 */
#error "Backwards compatible DFP does not support USB"
#endif

#ifdef CONFIG_COMMON_RUNTIME

/* Initialize globals based on system state. */
static void pd_init_tasks(void)
{
	static int initialized;
	int enable = 1;
	int i;

	/* Initialize globals once, for all PD tasks.  */
	if (initialized)
		return;

#if defined(HAS_TASK_CHIPSET) && defined(CONFIG_USB_PD_DUAL_ROLE)
	/* Set dual-role state based on chipset power state */
	if (chipset_in_state(CHIPSET_STATE_ANY_OFF))
		drp_state = PD_DRP_FORCE_SINK;
	else if (chipset_in_state(CHIPSET_STATE_SUSPEND))
		drp_state = PD_DRP_TOGGLE_OFF;
	else /* CHIPSET_STATE_ON */
		drp_state = PD_DRP_TOGGLE_ON;
#endif

#if defined(CONFIG_USB_PD_COMM_DISABLED)
	enable = 0;
#elif defined(CONFIG_USB_PD_COMM_LOCKED)
	/* Disable PD communication at init if we're in RO and locked. */
	if (!system_is_in_rw() && system_is_locked())
		enable = 0;
#endif
	for (i = 0; i < CONFIG_USB_PD_PORT_COUNT; i++)
		pd_comm_enabled[i] = enable;
	CPRINTS("PD comm %sabled", enable ? "en" : "dis");

	initialized = 1;
}
#endif /* CONFIG_COMMON_RUNTIME */

#ifndef CONFIG_USB_PD_TCPC
static int pd_restart_tcpc() {
	if (board_set_tcpc_power_mode) {
		/* force chip reset */
		board_set_tcpc_power_mode(0);
	}
	return tcpm_init();
}
#endif

void pd_init() {
#ifdef CONFIG_COMMON_RUNTIME
	pd_init_tasks();
#endif

	/* Ensure the power supply is in the default state */
	pd_power_supply_reset();

#ifdef CONFIG_USB_PD_TCPC_BOARD_INIT
	/* Board specific TCPC init */
	board_tcpc_init();
#endif

	/* Initialize TCPM driver and wait for TCPC to be ready */
	res = tcpm_init();

#ifdef CONFIG_USB_PD_DUAL_ROLE
	//pd_partner_port_reset();
#endif

	CPRINTS("TCPC p%d init %s", port, res ? "failed" : "ready");
	this_state = res ? PD_STATE_SUSPENDED : PD_DEFAULT_STATE();
#ifndef CONFIG_USB_PD_TCPC
	if (!res) {
		struct ec_response_pd_chip_info *info;
		tcpm_get_chip_info(0, &info);
		CPRINTS("TCPC p%d VID:0x%x PID:0x%x DID:0x%x FWV:0x%lx",
				port, info->vendor_id, info->product_id,
				info->device_id, info->fw_version_number);
	}
#endif

#ifdef CONFIG_USB_PD_REV30
	/* Set Revision to highest */
	pd.rev = PD_REV30;
	pd_ca_reset();
#endif

#ifdef CONFIG_USB_PD_DUAL_ROLE
	/*
	 * If VBUS is high, then initialize flag for VBUS has always been
	 * present. This flag is used to maintain a PD connection after a
	 * reset by sending a soft reset.
	 */
	//pd.flags = pd_is_vbus_present() ? PD_FLAGS_VBUS_NEVER_LOW : 0;
	pd.flags = 0;
#endif

	/* Disable TCPC RX until connection is established */
	tcpm_set_rx_enable(0);

#ifdef CONFIG_USBC_SS_MUX
	/* Initialize USB mux to its default state */
	usb_mux_init();
#endif

	/* Initialize PD protocol state variables for each port. */
	pd.power_role = PD_ROLE_DEFAULT();
	pd.vdm_state = VDM_STATE_DONE;
	set_state(this_state);
#ifdef CONFIG_USB_PD_MAX_SINGLE_SOURCE_CURRENT
	ASSERT(PD_ROLE_DEFAULT() == PD_ROLE_SINK);
	tcpm_select_rp_value( CONFIG_USB_PD_MAX_SINGLE_SOURCE_CURRENT);
#else
	tcpm_select_rp_value( CONFIG_USB_PD_PULLUP);
#endif
	tcpm_set_cc(
	PD_ROLE_DEFAULT() == PD_ROLE_SOURCE ? TYPEC_CC_RP : TYPEC_CC_RD);

#ifdef CONFIG_USB_PD_ALT_MODE_DFP
	/* Initialize PD Policy engine */
	pd_dfp_pe_init();
#endif

#ifdef CONFIG_CHARGE_MANAGER
	/* Initialize PD and type-C supplier current limits to 0 */
	pd_set_input_current_limit(0, 0);
	//typec_set_input_current_limit( 0, 0);
	//charge_manager_update_dualrole( CAP_UNKNOWN);
#endif
}

void pd_run_state_machine() {
#ifdef CONFIG_USB_PD_REV30
	/* send any pending messages */
	pd_ca_send_pending();
#endif
	/* process VDM messages last */
	pd_vdm_send_state_machine();

	/* Verify board specific health status : current, voltages... */
	res = pd_board_checks();
	if (res != EC_SUCCESS) {
		/* cut the power */
		pd_execute_hard_reset();
		/* notify the other side of the issue */
		pd_transmit(TCPC_TX_HARD_RESET, 0, NULL);
	}

	/* wait for next event/packet or timeout expiration */
	// getting rid of task stuff
	//evt = task_wait_event(timeout);
#ifdef CONFIG_USB_PD_DUAL_ROLE
	if (evt & PD_EVENT_UPDATE_DUAL_ROLE)
		pd_update_dual_role_config();
#endif

#ifdef CONFIG_USB_PD_TCPC
	/*
		* run port controller task to check CC and/or read incoming
		* messages
		*/
	tcpc_run( evt);
#else
	/* if TCPC has reset, then need to initialize it again */
	if (evt & PD_EVENT_TCPC_RESET) {
		CPRINTS("TCPC p%d reset!", port);
		if (tcpm_init() != EC_SUCCESS)
			CPRINTS("TCPC p%d init failed", port);
#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
	}

	if ((evt & PD_EVENT_TCPC_RESET) &&
		(pd.task_state != PD_STATE_DRP_AUTO_TOGGLE)) {
#endif
		/* Ensure CC termination is default */
		tcpm_set_cc( PD_ROLE_DEFAULT() ==
		PD_ROLE_SOURCE ? TYPEC_CC_RP : TYPEC_CC_RD);

		/*
		 * If we have a stable contract in the default role,
		 * then simply update TCPC with some missing info
		 * so that we can continue without resetting PD comms.
		 * Otherwise, go to the default disconnected state
		 * and force renegotiation.
		 */
		if (pd.vdm_state == VDM_STATE_DONE
				&& (
#ifdef CONFIG_USB_PD_DUAL_ROLE
				(PD_ROLE_DEFAULT() == PD_ROLE_SINK
						&& pd.task_state == PD_STATE_SNK_READY)
						||
#endif
				(PD_ROLE_DEFAULT() == PD_ROLE_SOURCE
						&& pd.task_state == PD_STATE_SRC_READY))) {
			tcpm_set_polarity(pd.polarity);
			tcpm_set_msg_header(pd.power_role, pd.data_role);
			tcpm_set_rx_enable(1);
		} else {
			/* Ensure state variables are at default */
			pd.power_role = PD_ROLE_DEFAULT();
			pd.vdm_state = VDM_STATE_DONE;
			set_state(PD_DEFAULT_STATE());
		}
	}
#endif

	/* process any potential incoming message */
	incoming_packet = evt & PD_EVENT_RX;
	//if (incoming_packet) {
	if (!tcpm_get_message(payload, &head))
		handle_request(head, payload);
	//}

	if (pd.req_suspend_state)
		set_state(PD_STATE_SUSPENDED);

	/* if nothing to do, verify the state of the world in 500ms */
	this_state = pd.task_state;
	timeout = 500 * MSEC;
	switch (this_state) {
	case PD_STATE_DISABLED:
		/* Nothing to do */
		break;
	case PD_STATE_SRC_DISCONNECTED:
		timeout = 10 * MSEC;
		tcpm_get_cc(&cc1, &cc2);
#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
		/*
			* Attempt TCPC auto DRP toggle if it is
			* not already auto toggling and not try.src
			*/
		if (auto_toggle_supported &&
			!(pd.flags & PD_FLAGS_TCPC_DRP_TOGGLE) &&
			!(pd.flags & PD_FLAGS_TRY_SRC) &&
			(cc1 == TYPEC_CC_VOLT_OPEN &&
			    cc2 == TYPEC_CC_VOLT_OPEN)) {
			set_state( PD_STATE_DRP_AUTO_TOGGLE);
			timeout = 2*MSEC;
			break;
		}
#endif

		/* Vnc monitoring */
		if ((cc1 == TYPEC_CC_VOLT_RD || cc2 == TYPEC_CC_VOLT_RD)
				|| (cc1 == TYPEC_CC_VOLT_RA && cc2 == TYPEC_CC_VOLT_RA)) {
#ifdef CONFIG_USBC_BACKWARDS_COMPATIBLE_DFP
			/* Enable VBUS */
			if (pd_set_power_supply_ready())
				break;
#endif
			pd.cc_state = PD_CC_NONE;
			set_state(PD_STATE_SRC_DISCONNECTED_DEBOUNCE);
		}
#if defined(CONFIG_USB_PD_DUAL_ROLE)
		/*
		 * Try.SRC state is embedded here. Wait for SNK
		 * detect, or if timer expires, transition to
		 * SNK_DISCONNETED.
		 *
		 * If Try.SRC state is not active, then this block
		 * handles the normal DRP toggle from SRC->SNK
		 */
		else if ((pd.flags & PD_FLAGS_TRY_SRC
				&& get_time().val >= pd.try_src_marker)
				|| (!(pd.flags & PD_FLAGS_TRY_SRC)
						&& drp_state != PD_DRP_FORCE_SOURCE
						&& drp_state != PD_DRP_FREEZE
						&& get_time().val >= next_role_swap)) {
			pd.power_role = PD_ROLE_SINK;
			set_state (PD_STATE_SNK_DISCONNECTED);
			tcpm_set_cc(TYPEC_CC_RD);
			next_role_swap = get_time().val + PD_T_DRP_SNK;
			pd.try_src_marker = get_time().val + PD_T_TRY_WAIT;

			/* Swap states quickly */
			timeout = 2 * MSEC;
		}
#endif
		break;
	case PD_STATE_SRC_DISCONNECTED_DEBOUNCE:
		timeout = 20 * MSEC;
		tcpm_get_cc(&cc1, &cc2);

		if (cc1 == TYPEC_CC_VOLT_RD && cc2 == TYPEC_CC_VOLT_RD) {
			/* Debug accessory */
			new_cc_state = PD_CC_DEBUG_ACC;
		} else if (cc1 == TYPEC_CC_VOLT_RD || cc2 == TYPEC_CC_VOLT_RD) {
			/* UFP attached */
			new_cc_state = PD_CC_UFP_ATTACHED;
		} else if (cc1 == TYPEC_CC_VOLT_RA && cc2 == TYPEC_CC_VOLT_RA) {
			/* Audio accessory */
			new_cc_state = PD_CC_AUDIO_ACC;
		} else {
			/* No UFP */
			set_state(PD_STATE_SRC_DISCONNECTED);
			timeout = 5 * MSEC;
			break;
		}
		/* If in Try.SRC state, then don't need to debounce */
		if (!(pd.flags & PD_FLAGS_TRY_SRC)) {
			/* Debounce the cc state */
			if (new_cc_state != pd.cc_state) {
				pd.cc_debounce = get_time().val +
				PD_T_CC_DEBOUNCE;
				pd.cc_state = new_cc_state;
				break;
			} else if (get_time().val < pd.cc_debounce) {
				break;
			}
		}

		/* Debounce complete */
		/* UFP is attached */
		if (new_cc_state == PD_CC_UFP_ATTACHED
				|| new_cc_state == PD_CC_DEBUG_ACC) {
			pd.polarity = (cc1 != TYPEC_CC_VOLT_RD);
			tcpm_set_polarity(pd.polarity);

			/* initial data role for source is DFP */
			pd_set_data_role( PD_ROLE_DFP);

			if (new_cc_state == PD_CC_DEBUG_ACC)
				pd.flags |=
				PD_FLAGS_TS_DTS_PARTNER;

#ifndef CONFIG_USBC_BACKWARDS_COMPATIBLE_DFP
			/* Enable VBUS */
			if (pd_set_power_supply_ready()) {
#ifdef CONFIG_USBC_SS_MUX
				usb_mux_set( TYPEC_MUX_NONE,
						USB_SWITCH_DISCONNECT,
						pd.polarity);
#endif
				break;
			}
#endif
			/* If PD comm is enabled, enable TCPC RX */
			if (pd_comm_is_enabled())
				tcpm_set_rx_enable(1);

#ifdef CONFIG_USBC_VCONN
			tcpm_set_vconn( 1);
			pd.flags |= PD_FLAGS_VCONN_ON;
#endif

			pd.flags |= PD_FLAGS_CHECK_PR_ROLE |
			PD_FLAGS_CHECK_DR_ROLE;
			hard_reset_count = 0;
			timeout = 5 * MSEC;
			set_state(PD_STATE_SRC_STARTUP);
		}
		/*
		 * AUDIO_ACC will remain in this state indefinitely
		 * until disconnect.
		 */
		break;
	case PD_STATE_SRC_HARD_RESET_RECOVER:
		/* Do not continue until hard reset recovery time */
		if (get_time().val < pd.src_recover) {
			timeout = 50 * MSEC;
			break;
		}

		/* Enable VBUS */
		timeout = 10 * MSEC;
		if (pd_set_power_supply_ready()) {
			set_state(PD_STATE_SRC_DISCONNECTED);
			break;
		}
#ifdef CONFIG_USB_PD_TCPM_TCPCI
		/*
			* After transmitting hard reset, TCPM writes
			* to RECEIVE_DETECT register to enable
			* PD message passing.
			*/
		if (pd_comm_is_enabled())
			tcpm_set_rx_enable( 1);
#endif /* CONFIG_USB_PD_TCPM_TCPCI */

		set_state(PD_STATE_SRC_STARTUP);
		break;
	case PD_STATE_SRC_STARTUP:
		/* Wait for power source to enable */
		if (pd.last_state != pd.task_state) {
			pd.flags |= PD_FLAGS_CHECK_IDENTITY;
			/* reset various counters */
			caps_count = 0;
			pd.msg_id = 0;
			snk_cap_count = 0;
			set_state_timeout(
#ifdef CONFIG_USBC_BACKWARDS_COMPATIBLE_DFP
				/*
					* delay for power supply to start up.
					* subtract out debounce time if coming
					* from debounce state since vbus is
					* on during debounce.
					*/
				get_time().val +
				PD_POWER_SUPPLY_TURN_ON_DELAY -
					(pd.last_state ==
					PD_STATE_SRC_DISCONNECTED_DEBOUNCE
					? PD_T_CC_DEBOUNCE : 0),
#else
					get_time().val +
					PD_POWER_SUPPLY_TURN_ON_DELAY,
#endif
					PD_STATE_SRC_DISCOVERY);
		}
		break;
	case PD_STATE_SRC_DISCOVERY:
		if (pd.last_state != pd.task_state) {
			/*
			 * If we have had PD connection with this port
			 * partner, then start NoResponseTimer.
			 */
			if (pd.flags & PD_FLAGS_PREVIOUS_PD_CONN)
				set_state_timeout(get_time().val +
				PD_T_NO_RESPONSE,
						hard_reset_count <
						PD_HARD_RESET_COUNT ?
								PD_STATE_HARD_RESET_SEND :
								PD_STATE_SRC_DISCONNECTED);
		}

		/* Send source cap some minimum number of times */
		if (caps_count < PD_CAPS_COUNT) {
			/* Query capabilities of the other side */
			res = send_source_cap();
			/* packet was acked => PD capable device) */
			if (res >= 0) {
				set_state(PD_STATE_SRC_NEGOCIATE);
				timeout = 10 * MSEC;
				hard_reset_count = 0;
				caps_count = 0;
				/* Port partner is PD capable */
				pd.flags |=
				PD_FLAGS_PREVIOUS_PD_CONN;
			} else { /* failed, retry later */
				timeout = PD_T_SEND_SOURCE_CAP;
				caps_count++;
			}
		}
		break;
	case PD_STATE_SRC_NEGOCIATE:
		/* wait for a "Request" message */
		if (pd.last_state != pd.task_state)
			set_state_timeout(get_time().val +
			PD_T_SENDER_RESPONSE, PD_STATE_HARD_RESET_SEND);
		break;
	case PD_STATE_SRC_ACCEPTED:
		/* Accept sent, wait for enabling the new voltage */
		if (pd.last_state != pd.task_state)
			set_state_timeout(get_time().val +
			PD_T_SINK_TRANSITION, PD_STATE_SRC_POWERED);
		break;
	case PD_STATE_SRC_POWERED:
		/* Switch to the new requested voltage */
		if (pd.last_state != pd.task_state) {
			pd_transition_voltage(pd.requested_idx);
			set_state_timeout(get_time().val +
			PD_POWER_SUPPLY_TURN_ON_DELAY, PD_STATE_SRC_TRANSITION);
		}
		break;
	case PD_STATE_SRC_TRANSITION:
		/* the voltage output is good, notify the source */
		res = send_control(PD_CTRL_PS_RDY);
		if (res >= 0) {
			timeout = 10 * MSEC;
			/* it'a time to ping regularly the sink */
			set_state(PD_STATE_SRC_READY);
		} else {
			/* The sink did not ack, cut the power... */
			set_state(PD_STATE_SRC_DISCONNECTED);
		}
		break;
	case PD_STATE_SRC_READY:
		timeout = PD_T_SOURCE_ACTIVITY;

		/*
		 * Don't send any PD traffic if we woke up due to
		 * incoming packet or if VDO response pending to avoid
		 * collisions.
		 */
		if (incoming_packet || (pd.vdm_state == VDM_STATE_BUSY))
			break;

		/* Send updated source capabilities to our partner */
		if (pd.flags & PD_FLAGS_UPDATE_SRC_CAPS) {
			res = send_source_cap();
			if (res >= 0) {
				set_state(PD_STATE_SRC_NEGOCIATE);
				pd.flags &= ~PD_FLAGS_UPDATE_SRC_CAPS;
			}
			break;
		}

		/* Send get sink cap if haven't received it yet */
		if (!(pd.flags & PD_FLAGS_SNK_CAP_RECVD)) {
			if (++snk_cap_count <= PD_SNK_CAP_RETRIES) {
				/* Get sink cap to know if dual-role device */
				send_control(PD_CTRL_GET_SINK_CAP);
				set_state(PD_STATE_SRC_GET_SINK_CAP);
				break;
			} else if (debug_level >= 2
					&& snk_cap_count == PD_SNK_CAP_RETRIES + 1) {
				CPRINTF("ERR SNK_CAP\n");
			}
		}

		/* Check power role policy, which may trigger a swap */
		if (pd.flags & PD_FLAGS_CHECK_PR_ROLE) {
			pd_check_pr_role( PD_ROLE_SOURCE, pd.flags);
			pd.flags &= ~PD_FLAGS_CHECK_PR_ROLE;
			break;
		}

		/* Check data role policy, which may trigger a swap */
		if (pd.flags & PD_FLAGS_CHECK_DR_ROLE) {
			pd_check_dr_role(pd.data_role, pd.flags);
			pd.flags &= ~PD_FLAGS_CHECK_DR_ROLE;
			break;
		}

		/* Send discovery SVDMs last */
		if (pd.data_role == PD_ROLE_DFP
				&& (pd.flags & PD_FLAGS_CHECK_IDENTITY)) {
#ifndef CONFIG_USB_PD_SIMPLE_DFP
			pd_send_vdm( USB_SID_PD,
			CMD_DISCOVER_IDENT, NULL, 0);
#endif
			pd.flags &= ~PD_FLAGS_CHECK_IDENTITY;
			break;
		}

		if (!(pd.flags & PD_FLAGS_PING_ENABLED))
			break;

		/* Verify that the sink is alive */
		res = send_control(PD_CTRL_PING);
		if (res >= 0)
			break;

		/* Ping dropped. Try soft reset. */
		set_state(PD_STATE_SOFT_RESET);
		timeout = 10 * MSEC;
		break;
	case PD_STATE_SRC_GET_SINK_CAP:
		if (pd.last_state != pd.task_state)
			set_state_timeout(get_time().val +
			PD_T_SENDER_RESPONSE, PD_STATE_SRC_READY);
		break;
	case PD_STATE_DR_SWAP:
		if (pd.last_state != pd.task_state) {
			res = send_control(PD_CTRL_DR_SWAP);
			if (res < 0) {
				timeout = 10 * MSEC;
				/*
				 * If failed to get goodCRC, send
				 * soft reset, otherwise ignore
				 * failure.
				 */
				set_state(
						res == -1 ? PD_STATE_SOFT_RESET : READY_RETURN_STATE());
				break;
			}
			/* Wait for accept or reject */
			set_state_timeout(get_time().val +
			PD_T_SENDER_RESPONSE, READY_RETURN_STATE());
		}
		break;
#ifdef CONFIG_USB_PD_DUAL_ROLE
	case PD_STATE_SRC_SWAP_INIT:
		if (pd.last_state != pd.task_state) {
			res = send_control(PD_CTRL_PR_SWAP);
			if (res < 0) {
				timeout = 10 * MSEC;
				/*
				 * If failed to get goodCRC, send
				 * soft reset, otherwise ignore
				 * failure.
				 */
				set_state(res == -1 ? PD_STATE_SOFT_RESET : PD_STATE_SRC_READY);
				break;
			}
			/* Wait for accept or reject */
			set_state_timeout(get_time().val +
			PD_T_SENDER_RESPONSE, PD_STATE_SRC_READY);
		}
		break;
	case PD_STATE_SRC_SWAP_SNK_DISABLE:
		/* Give time for sink to stop drawing current */
		if (pd.last_state != pd.task_state)
			set_state_timeout(get_time().val +
			PD_T_SINK_TRANSITION, PD_STATE_SRC_SWAP_SRC_DISABLE);
		break;
	case PD_STATE_SRC_SWAP_SRC_DISABLE:
		/* Turn power off */
		if (pd.last_state != pd.task_state) {
			pd_power_supply_reset();
			set_state_timeout(get_time().val +
			PD_POWER_SUPPLY_TURN_OFF_DELAY, PD_STATE_SRC_SWAP_STANDBY);
		}
		break;
	case PD_STATE_SRC_SWAP_STANDBY:
		/* Send PS_RDY to let sink know our power is off */
		if (pd.last_state != pd.task_state) {
			/* Send PS_RDY */
			res = send_control(PD_CTRL_PS_RDY);
			if (res < 0) {
				timeout = 10 * MSEC;
				set_state(PD_STATE_SRC_DISCONNECTED);
				break;
			}
			/* Switch to Rd and swap roles to sink */
			tcpm_set_cc(TYPEC_CC_RD);
			pd.power_role = PD_ROLE_SINK;
			/* Wait for PS_RDY from new source */
			set_state_timeout(get_time().val +
			PD_T_PS_SOURCE_ON, PD_STATE_SNK_DISCONNECTED);
		}
		break;
	case PD_STATE_SUSPENDED: {
#ifndef CONFIG_USB_PD_TCPC
		int rstatus;
#endif
		CPRINTS("TCPC p%d suspended!", port);
		pd.req_suspend_state = 0;
#ifdef CONFIG_USB_PD_TCPC
		pd_rx_disable_monitoring();
		pd_hw_release();
		pd_power_supply_reset();
#else
		rstatus = tcpm_release();
		if (rstatus != 0 && rstatus != EC_ERROR_UNIMPLEMENTED)
			CPRINTS("TCPC p%d release failed!", port);
#endif
		/* Wait for resume */
		// getting rid of task stuff
		//while (pd.task_state == PD_STATE_SUSPENDED)
		//	task_wait_event(-1);
#ifdef CONFIG_USB_PD_TCPC
		pd_hw_init( PD_ROLE_DEFAULT());
		CPRINTS("TCPC p%d resumed!", port);
#else
		if (rstatus != EC_ERROR_UNIMPLEMENTED && pd_restart_tcpc() != 0) {
			/* stay in PD_STATE_SUSPENDED */
			CPRINTS("TCPC p%d restart failed!", port);
			break;
		}
		set_state(PD_DEFAULT_STATE());
		CPRINTS("TCPC p%d resumed!", port);
#endif
		break;
	}
	case PD_STATE_SNK_DISCONNECTED:
#ifdef CONFIG_USB_PD_LOW_POWER
		timeout = drp_state != PD_DRP_TOGGLE_ON ? SECOND
							: 10*MSEC;
#else
		timeout = 10 * MSEC;
#endif
		tcpm_get_cc(&cc1, &cc2);

#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
		/*
			* Attempt TCPC auto DRP toggle if it is
			* not already auto toggling and not try.src
			*/
		if (auto_toggle_supported &&
			!(pd.flags & PD_FLAGS_TCPC_DRP_TOGGLE) &&
			!(pd.flags & PD_FLAGS_TRY_SRC) &&
			(cc1 == TYPEC_CC_VOLT_OPEN &&
			    cc2 == TYPEC_CC_VOLT_OPEN)) {
			set_state( PD_STATE_DRP_AUTO_TOGGLE);
			timeout = 2*MSEC;
			break;
		}
#endif

		/* Source connection monitoring */
		if (cc1 != TYPEC_CC_VOLT_OPEN || cc2 != TYPEC_CC_VOLT_OPEN) {
			pd.cc_state = PD_CC_NONE;
			hard_reset_count = 0;
			new_cc_state = PD_CC_NONE;
			pd.cc_debounce = get_time().val +
			PD_T_CC_DEBOUNCE;
			set_state (PD_STATE_SNK_DISCONNECTED_DEBOUNCE);
			timeout = 10 * MSEC;
			break;
		}

		/*
		 * If Try.SRC is active and failed to detect a SNK,
		 * then it transitions to TryWait.SNK. Need to prevent
		 * normal dual role toggle until tDRPTryWait timer
		 * expires.
		 */
		if (pd.flags & PD_FLAGS_TRY_SRC) {
			if (get_time().val > pd.try_src_marker)
				pd.flags &= ~PD_FLAGS_TRY_SRC;
			break;
		}

		/* If no source detected, check for role toggle. */
		if (drp_state == PD_DRP_TOGGLE_ON && get_time().val >= next_role_swap) {
			/* Swap roles to source */
			pd.power_role = PD_ROLE_SOURCE;
			set_state(PD_STATE_SRC_DISCONNECTED);
			tcpm_set_cc(TYPEC_CC_RP);
			next_role_swap = get_time().val + PD_T_DRP_SRC;

			/* Swap states quickly */
			timeout = 2 * MSEC;
		}
		break;
	case PD_STATE_SNK_DISCONNECTED_DEBOUNCE:
		tcpm_get_cc(&cc1, &cc2);

		if (cc_is_rp(cc1) && cc_is_rp(cc2)) {
			/* Debug accessory */
			new_cc_state = PD_CC_DEBUG_ACC;
		} else if (cc_is_rp(cc1) || cc_is_rp(cc2)) {
			new_cc_state = PD_CC_DFP_ATTACHED;
		} else {
			/* No connection any more */
			set_state (PD_STATE_SNK_DISCONNECTED);
			timeout = 5 * MSEC;
			break;
		}

		timeout = 20 * MSEC;

		/* Debounce the cc state */
		if (new_cc_state != pd.cc_state) {
			pd.cc_debounce = get_time().val +
			PD_T_CC_DEBOUNCE;
			pd.cc_state = new_cc_state;
			break;
		}
		/* Wait for CC debounce and VBUS present */
		if (get_time().val < pd.cc_debounce || !pd_is_vbus_present())
			break;

		if (pd_try_src_enable && !(pd.flags & PD_FLAGS_TRY_SRC)) {
			/*
			 * If TRY_SRC is enabled, but not active,
			 * then force attempt to connect as source.
			 */
			pd.try_src_marker = get_time().val + PD_T_TRY_SRC;
			/* Swap roles to source */
			pd.power_role = PD_ROLE_SOURCE;
			tcpm_set_cc(TYPEC_CC_RP);
			timeout = 2 * MSEC;
			set_state(PD_STATE_SRC_DISCONNECTED);
			/* Set flag after the state change */
			pd.flags |= PD_FLAGS_TRY_SRC;
			break;
		}

		/* We are attached */
		pd.polarity = get_snk_polarity(cc1, cc2);
		tcpm_set_polarity(pd.polarity);
		/* reset message ID  on connection */
		pd.msg_id = 0;
		/* initial data role for sink is UFP */
		pd_set_data_role( PD_ROLE_UFP);
#if defined(CONFIG_CHARGE_MANAGER)
		typec_curr = get_typec_current_limit(pd.polarity, cc1, cc2);
		//typec_set_input_current_limit(
		//	port, typec_curr, TYPE_C_VOLTAGE);
#endif
		/* If PD comm is enabled, enable TCPC RX */
		if (pd_comm_is_enabled())
			tcpm_set_rx_enable(1);

		/* DFP is attached */
		if (new_cc_state == PD_CC_DFP_ATTACHED
				|| new_cc_state == PD_CC_DEBUG_ACC) {
			pd.flags |= PD_FLAGS_CHECK_PR_ROLE |
			PD_FLAGS_CHECK_DR_ROLE |
			PD_FLAGS_CHECK_IDENTITY;
			if (new_cc_state == PD_CC_DEBUG_ACC)
				pd.flags |=
				PD_FLAGS_TS_DTS_PARTNER;
			send_control(PD_CTRL_GET_SOURCE_CAP);
			set_state (PD_STATE_SNK_DISCOVERY);
			timeout = 10 * MSEC;
			//hook_call_deferred(
			//	&pd_usb_billboard_deferred_data,
			//	PD_T_AME);
		}
		break;
	case PD_STATE_SNK_HARD_RESET_RECOVER:
		if (pd.last_state != pd.task_state)
			pd.flags |= PD_FLAGS_CHECK_IDENTITY;
#ifdef CONFIG_USB_PD_VBUS_DETECT_NONE
		/*
			* Can't measure vbus state so this is the maximum
			* recovery time for the source.
			*/
		if (pd.last_state != pd.task_state)
			set_state_timeout( get_time().val +
						PD_T_SAFE_0V +
						PD_T_SRC_RECOVER_MAX +
						PD_T_SRC_TURN_ON,
						PD_STATE_SNK_DISCONNECTED);
#else
		/* Wait for VBUS to go low and then high*/
		if (pd.last_state != pd.task_state) {
			snk_hard_reset_vbus_off = 0;
			set_state_timeout(get_time().val +
			PD_T_SAFE_0V,
					hard_reset_count <
					PD_HARD_RESET_COUNT ?
							PD_STATE_HARD_RESET_SEND : PD_STATE_SNK_DISCOVERY);
		}

		if (!pd_is_vbus_present() && !snk_hard_reset_vbus_off) {
			/* VBUS has gone low, reset timeout */
			snk_hard_reset_vbus_off = 1;
			set_state_timeout(get_time().val +
			PD_T_SRC_RECOVER_MAX +
			PD_T_SRC_TURN_ON, PD_STATE_SNK_DISCONNECTED);
		}
		if (pd_is_vbus_present() && snk_hard_reset_vbus_off) {
#ifdef CONFIG_USB_PD_TCPM_TCPCI
			/*
				* After transmitting hard reset, TCPM writes
				* to RECEIVE_MESSAGE register to enable
				* PD message passing.
				*/
			if (pd_comm_is_enabled())
				tcpm_set_rx_enable( 1);
#endif /* CONFIG_USB_PD_TCPM_TCPCI */

			/* VBUS went high again */
			set_state (PD_STATE_SNK_DISCOVERY);
			timeout = 10 * MSEC;
		}

		/*
		 * Don't need to set timeout because VBUS changing
		 * will trigger an interrupt and wake us up.
		 */
#endif
		break;
	case PD_STATE_SNK_DISCOVERY:
		/* Wait for source cap expired only if we are enabled */
		if ((pd.last_state != pd.task_state)
				&& pd_comm_is_enabled()) {
			/*
			 * If VBUS has never been low, and we timeout
			 * waiting for source cap, try a soft reset
			 * first, in case we were already in a stable
			 * contract before this boot.
			 */
			if (pd.flags & PD_FLAGS_VBUS_NEVER_LOW)
				set_state_timeout(get_time().val +
				PD_T_SINK_WAIT_CAP, PD_STATE_SOFT_RESET);
			/*
			 * If we haven't passed hard reset counter,
			 * start SinkWaitCapTimer, otherwise start
			 * NoResponseTimer.
			 */
			else if (hard_reset_count < PD_HARD_RESET_COUNT)
				set_state_timeout(get_time().val +
				PD_T_SINK_WAIT_CAP, PD_STATE_HARD_RESET_SEND);
			else if (pd.flags &
			PD_FLAGS_PREVIOUS_PD_CONN)
				/* ErrorRecovery */
				set_state_timeout(get_time().val +
				PD_T_NO_RESPONSE, PD_STATE_SNK_DISCONNECTED);
#if defined(CONFIG_CHARGE_MANAGER)
			/*
			 * If we didn't come from disconnected, must
			 * have come from some path that did not set
			 * typec current limit. So, set to 0 so that
			 * we guarantee this is revised below.
			 */
			if (pd.last_state != PD_STATE_SNK_DISCONNECTED_DEBOUNCE)
				typec_curr = 0;
#endif
		}

#if defined(CONFIG_CHARGE_MANAGER)
		timeout = PD_T_SINK_ADJ - PD_T_DEBOUNCE;

		/* Check if CC pull-up has changed */
		tcpm_get_cc(&cc1, &cc2);
		if (typec_curr
				!= get_typec_current_limit(pd.polarity, cc1, cc2)) {
			/* debounce signal by requiring two reads */
			if (typec_curr_change) {
				/* set new input current limit */
				typec_curr = get_typec_current_limit(pd.polarity, cc1,
						cc2);
				//typec_set_input_current_limit(
				//	port, typec_curr, TYPE_C_VOLTAGE);
			} else {
				/* delay for debounce */
				timeout = PD_T_DEBOUNCE;
			}
			typec_curr_change = !typec_curr_change;
		} else {
			typec_curr_change = 0;
		}
#endif
		break;
	case PD_STATE_SNK_REQUESTED:
		/* Wait for ACCEPT or REJECT */
		if (pd.last_state != pd.task_state) {
			hard_reset_count = 0;
			set_state_timeout(get_time().val +
			PD_T_SENDER_RESPONSE, PD_STATE_HARD_RESET_SEND);
		}
		break;
	case PD_STATE_SNK_TRANSITION:
		/* Wait for PS_RDY */
		if (pd.last_state != pd.task_state)
			set_state_timeout(get_time().val +
			PD_T_PS_TRANSITION, PD_STATE_HARD_RESET_SEND);
		break;
	case PD_STATE_SNK_READY:
		timeout = 20 * MSEC;

		/*
		 * Don't send any PD traffic if we woke up due to
		 * incoming packet or if VDO response pending to avoid
		 * collisions.
		 */
		if (incoming_packet || (pd.vdm_state == VDM_STATE_BUSY))
			break;

		/* Check for new power to request */
		if (pd.new_power_request) {
			if (pd_send_request_msg(0) != EC_SUCCESS)
				set_state(PD_STATE_SOFT_RESET);
			break;
		}

		/* Check power role policy, which may trigger a swap */
		if (pd.flags & PD_FLAGS_CHECK_PR_ROLE) {
			pd_check_pr_role( PD_ROLE_SINK, pd.flags);
			pd.flags &= ~PD_FLAGS_CHECK_PR_ROLE;
			break;
		}

		/* Check data role policy, which may trigger a swap */
		if (pd.flags & PD_FLAGS_CHECK_DR_ROLE) {
			pd_check_dr_role(pd.data_role, pd.flags);
			pd.flags &= ~PD_FLAGS_CHECK_DR_ROLE;
			break;
		}

		/* If DFP, send discovery SVDMs */
		if (pd.data_role == PD_ROLE_DFP
				&& (pd.flags & PD_FLAGS_CHECK_IDENTITY)) {
			pd_send_vdm( USB_SID_PD,
			CMD_DISCOVER_IDENT, NULL, 0);
			pd.flags &= ~PD_FLAGS_CHECK_IDENTITY;
			break;
		}

		/* Sent all messages, don't need to wake very often */
		timeout = 200 * MSEC;
		break;
	case PD_STATE_SNK_SWAP_INIT:
		if (pd.last_state != pd.task_state) {
			res = send_control(PD_CTRL_PR_SWAP);
			if (res < 0) {
				timeout = 10 * MSEC;
				/*
				 * If failed to get goodCRC, send
				 * soft reset, otherwise ignore
				 * failure.
				 */
				set_state(res == -1 ? PD_STATE_SOFT_RESET : PD_STATE_SNK_READY);
				break;
			}
			/* Wait for accept or reject */
			set_state_timeout(get_time().val +
			PD_T_SENDER_RESPONSE, PD_STATE_SNK_READY);
		}
		break;
	case PD_STATE_SNK_SWAP_SNK_DISABLE:
		/* Stop drawing power */
		pd_set_input_current_limit(0, 0);
#ifdef CONFIG_CHARGE_MANAGER
		//typec_set_input_current_limit( 0, 0);
		//charge_manager_set_ceil(
		//			CEIL_REQUESTOR_PD,
		//			CHARGE_CEIL_NONE);
#endif
		set_state (PD_STATE_SNK_SWAP_SRC_DISABLE);
		timeout = 10 * MSEC;
		break;
	case PD_STATE_SNK_SWAP_SRC_DISABLE:
		/* Wait for PS_RDY */
		if (pd.last_state != pd.task_state)
			set_state_timeout(get_time().val +
			PD_T_PS_SOURCE_OFF, PD_STATE_HARD_RESET_SEND);
		break;
	case PD_STATE_SNK_SWAP_STANDBY:
		if (pd.last_state != pd.task_state) {
			/* Switch to Rp and enable power supply */
			tcpm_set_cc(TYPEC_CC_RP);
			if (pd_set_power_supply_ready()) {
				/* Restore Rd */
				tcpm_set_cc(TYPEC_CC_RD);
				timeout = 10 * MSEC;
				set_state (PD_STATE_SNK_DISCONNECTED);
				break;
			}
			/* Wait for power supply to turn on */
			set_state_timeout(get_time().val +
			PD_POWER_SUPPLY_TURN_ON_DELAY, PD_STATE_SNK_SWAP_COMPLETE);
		}
		break;
	case PD_STATE_SNK_SWAP_COMPLETE:
		/* Send PS_RDY and change to source role */
		res = send_control(PD_CTRL_PS_RDY);
		if (res < 0) {
			/* Restore Rd */
			tcpm_set_cc(TYPEC_CC_RD);
			pd_power_supply_reset();
			timeout = 10 * MSEC;
			set_state (PD_STATE_SNK_DISCONNECTED);
			break;
		}

		/* Don't send GET_SINK_CAP on swap */
		snk_cap_count = PD_SNK_CAP_RETRIES + 1;
		caps_count = 0;
		pd.msg_id = 0;
		pd.power_role = PD_ROLE_SOURCE;
		pd_update_roles();
		set_state(PD_STATE_SRC_DISCOVERY);
		timeout = 10 * MSEC;
		break;
#ifdef CONFIG_USBC_VCONN_SWAP
	case PD_STATE_VCONN_SWAP_SEND:
		if (pd.last_state != pd.task_state) {
			res = send_control( PD_CTRL_VCONN_SWAP);
			if (res < 0) {
				timeout = 10*MSEC;
				/*
					* If failed to get goodCRC, send
					* soft reset, otherwise ignore
					* failure.
					*/
				set_state( res == -1 ?
						PD_STATE_SOFT_RESET :
						READY_RETURN_STATE());
				break;
			}
			/* Wait for accept or reject */
			set_state_timeout(
						get_time().val +
						PD_T_SENDER_RESPONSE,
						READY_RETURN_STATE());
		}
		break;
	case PD_STATE_VCONN_SWAP_INIT:
		if (pd.last_state != pd.task_state) {
			if (!(pd.flags & PD_FLAGS_VCONN_ON)) {
				/* Turn VCONN on and wait for it */
				tcpm_set_vconn( 1);
				set_state_timeout(
					get_time().val + PD_VCONN_SWAP_DELAY,
					PD_STATE_VCONN_SWAP_READY);
			} else {
				set_state_timeout(
					get_time().val + PD_T_VCONN_SOURCE_ON,
					READY_RETURN_STATE());
			}
		}
		break;
	case PD_STATE_VCONN_SWAP_READY:
		if (pd.last_state != pd.task_state) {
			if (!(pd.flags & PD_FLAGS_VCONN_ON)) {
				/* VCONN is now on, send PS_RDY */
				pd.flags |= PD_FLAGS_VCONN_ON;
				res = send_control(
							PD_CTRL_PS_RDY);
				if (res == -1) {
					timeout = 10*MSEC;
					/*
						* If failed to get goodCRC,
						* send soft reset
						*/
					set_state(
							PD_STATE_SOFT_RESET);
					break;
				}
				set_state(
						READY_RETURN_STATE());
			} else {
				/* Turn VCONN off and wait for it */
				tcpm_set_vconn( 0);
				pd.flags &= ~PD_FLAGS_VCONN_ON;
				set_state_timeout(
					get_time().val + PD_VCONN_SWAP_DELAY,
					READY_RETURN_STATE());
			}
		}
		break;
#endif /* CONFIG_USBC_VCONN_SWAP */
#endif /* CONFIG_USB_PD_DUAL_ROLE */
	case PD_STATE_SOFT_RESET:
		if (pd.last_state != pd.task_state) {
			/* Message ID of soft reset is always 0 */
			pd.msg_id = 0;
			res = send_control(PD_CTRL_SOFT_RESET);

			/* if soft reset failed, try hard reset. */
			if (res < 0) {
				set_state(PD_STATE_HARD_RESET_SEND);
				timeout = 5 * MSEC;
				break;
			}

			set_state_timeout(get_time().val + PD_T_SENDER_RESPONSE,
					PD_STATE_HARD_RESET_SEND);
		}
		break;
	case PD_STATE_HARD_RESET_SEND:
		hard_reset_count++;
		if (pd.last_state != pd.task_state)
			hard_reset_sent = 0;
#ifdef CONFIG_CHARGE_MANAGER
//		if (pd.last_state == PD_STATE_SNK_DISCOVERY
//				|| (pd.last_state == PD_STATE_SOFT_RESET
//						&& (pd.flags & PD_FLAGS_VBUS_NEVER_LOW))) {
//			pd.flags &= ~PD_FLAGS_VBUS_NEVER_LOW;
//			/*
//			 * If discovery timed out, assume that we
//			 * have a dedicated charger attached. This
//			 * may not be a correct assumption according
//			 * to the specification, but it generally
//			 * works in practice and the harmful
//			 * effects of a wrong assumption here
//			 * are minimal.
//			 */
//			//charge_manager_update_dualrole(
//			//				    CAP_DEDICATED);
//		}
#endif

		/* try sending hard reset until it succeeds */
		if (!hard_reset_sent) {
			if (pd_transmit(TCPC_TX_HARD_RESET, 0, NULL) < 0) {
				timeout = 10 * MSEC;
				break;
			}

			/* successfully sent hard reset */
			hard_reset_sent = 1;
			/*
			 * If we are source, delay before cutting power
			 * to allow sink time to get hard reset.
			 */

			set_state(PD_STATE_HARD_RESET_EXECUTE);
			timeout = 10 * MSEC;

		}
		break;
	case PD_STATE_HARD_RESET_EXECUTE:
#ifdef CONFIG_USB_PD_DUAL_ROLE
		/*
		 * If hard reset while in the last stages of power
		 * swap, then we need to restore our CC resistor.
		 */
		if (pd.last_state == PD_STATE_SNK_SWAP_STANDBY)
			tcpm_set_cc(TYPEC_CC_RD);
#endif

		/* reset our own state machine */
		pd_execute_hard_reset();
		timeout = 10 * MSEC;
		break;
#ifdef CONFIG_COMMON_RUNTIME
	case PD_STATE_BIST_RX:
		send_bist_cmd();
		/* Delay at least enough for partner to finish BIST */
		timeout = PD_T_BIST_RECEIVE + 20*MSEC;
		/* Set to appropriate port disconnected state */
		set_state( DUAL_ROLE_IF_ELSE(
					PD_STATE_SNK_DISCONNECTED,
					PD_STATE_SRC_DISCONNECTED));
		break;
	case PD_STATE_BIST_TX:
		pd_transmit( TCPC_TX_BIST_MODE_2, 0, NULL);
		/* Delay at least enough to finish sending BIST */
		timeout = PD_T_BIST_TRANSMIT + 20*MSEC;
		/* Set to appropriate port disconnected state */
		set_state( DUAL_ROLE_IF_ELSE(
					PD_STATE_SNK_DISCONNECTED,
					PD_STATE_SRC_DISCONNECTED));
		break;
#endif
#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
	case PD_STATE_DRP_AUTO_TOGGLE:
	{
		enum pd_states next_state;

		assert(auto_toggle_supported);

		/* Check for connection */
		tcpm_get_cc( &cc1, &cc2);

		/* Set to appropriate port state */
		if (cc1 == TYPEC_CC_VOLT_OPEN &&
			cc2 == TYPEC_CC_VOLT_OPEN)
			/* nothing connected, keep toggling*/
			next_state = PD_STATE_DRP_AUTO_TOGGLE;
		else if ((cc_is_rp(cc1) || cc_is_rp(cc2)) &&
				drp_state != PD_DRP_FORCE_SOURCE)
			/* SNK allowed unless ForceSRC */
			next_state = PD_STATE_SNK_DISCONNECTED;
		else if (((cc1 == TYPEC_CC_VOLT_RD ||
				cc2 == TYPEC_CC_VOLT_RD) ||
				(cc1 == TYPEC_CC_VOLT_RA &&
				cc2 == TYPEC_CC_VOLT_RA)) &&
				(drp_state != PD_DRP_TOGGLE_OFF &&
				drp_state != PD_DRP_FORCE_SINK))
			/* SRC allowed unless ForceSNK or Toggle Off */
			next_state = PD_STATE_SRC_DISCONNECTED;
		else
			/* Anything else, keep toggling */
			next_state = PD_STATE_DRP_AUTO_TOGGLE;

		if (next_state != PD_STATE_DRP_AUTO_TOGGLE) {
			tcpm_set_drp_toggle( 0);
#ifdef CONFIG_USB_PD_TCPC_LOW_POWER
			CPRINTS("TCPC p%d Exit Low Power Mode", port);
#endif
		}

		if (next_state == PD_STATE_SNK_DISCONNECTED) {
			tcpm_set_cc( TYPEC_CC_RD);
			pd.power_role = PD_ROLE_SINK;
			timeout = 2*MSEC;
		} else if (next_state == PD_STATE_SRC_DISCONNECTED) {
			tcpm_set_cc( TYPEC_CC_RP);
			pd.power_role = PD_ROLE_SOURCE;
			timeout = 2*MSEC;
		} else {
			tcpm_set_drp_toggle( 1);
			pd.flags |= PD_FLAGS_TCPC_DRP_TOGGLE;
			timeout = -1;
#ifdef CONFIG_USB_PD_TCPC_LOW_POWER
			CPRINTS("TCPC p%d Low Power Mode", port);
#endif
		}
		set_state( next_state);

		break;
	}
#endif
	default:
		break;
	}

	pd.last_state = this_state;

	/*
	 * Check for state timeout, and if not check if need to adjust
	 * timeout value to wake up on the next state timeout.
	 */
	now = get_time();
	if (pd.timeout) {
		if (now.val >= pd.timeout) {
			set_state(pd.timeout_state);
			/* On a state timeout, run next state soon */
			timeout = timeout < 10 * MSEC ? timeout : 10 * MSEC;
		} else if (pd.timeout - now.val < timeout) {
			timeout = pd.timeout - now.val;
		}
	}

	/* Check for disconnection if we're connected */
	if (!pd_is_connected())
		return;
#ifdef CONFIG_USB_PD_DUAL_ROLE
	if (pd_is_power_swapping())
		return;
#endif

#ifdef CONFIG_USB_PD_DUAL_ROLE
	/*
	 * Sink disconnect if VBUS is low and we are not recovering
	 * a hard reset.
	 */
	if (pd.power_role == PD_ROLE_SINK && !pd_is_vbus_present()
			&& pd.task_state != PD_STATE_SNK_HARD_RESET_RECOVER
			&& pd.task_state != PD_STATE_HARD_RESET_EXECUTE) {
		/* Sink: detect disconnect by monitoring VBUS */
		set_state (PD_STATE_SNK_DISCONNECTED);
		/* set timeout small to reconnect fast */
		timeout = 5 * MSEC;
	}
#endif /* CONFIG_USB_PD_DUAL_ROLE */
}

#ifdef CONFIG_USB_PD_DUAL_ROLE
//extern "C" {
//static void dual_role_on(void) {
//	int i;
//
//	for (i = 0; i < CONFIG_USB_PD_PORT_COUNT; i++) {
//#ifdef CONFIG_CHARGE_MANAGER
//		//if (charge_manager_get_active_charge_port() != i)
//#endif
//		pd[i].flags |= PD_FLAGS_CHECK_PR_ROLE |
//		PD_FLAGS_CHECK_DR_ROLE;
//
//		pd[i].flags |= PD_FLAGS_CHECK_IDENTITY;
//	}
//
//	pd_set_dual_role(PD_DRP_TOGGLE_ON);
//	CPRINTS("chipset -> S0");
//}
//}
//DECLARE_HOOK(HOOK_CHIPSET_RESUME, dual_role_on, HOOK_PRIO_DEFAULT);
//
//static void dual_role_off(void) {
//	pd_set_dual_role(PD_DRP_TOGGLE_OFF);
//	CPRINTS("chipset -> S3");
//}
//DECLARE_HOOK(HOOK_CHIPSET_SUSPEND, dual_role_off, HOOK_PRIO_DEFAULT);
//DECLARE_HOOK(HOOK_CHIPSET_STARTUP, dual_role_off, HOOK_PRIO_DEFAULT);
//
//static void dual_role_force_sink(void) {
//	pd_set_dual_role(PD_DRP_FORCE_SINK);
//	CPRINTS("chipset -> S5");
//}
//DECLARE_HOOK(HOOK_CHIPSET_SHUTDOWN, dual_role_force_sink, HOOK_PRIO_DEFAULT);

#endif /* CONFIG_USB_PD_DUAL_ROLE */

#ifdef CONFIG_COMMON_RUNTIME

/*
 * (enable=1) request pd_task transition to the suspended state.  hang
 * around for a while until we observe the state change.  this can
 * take a while (like 300ms) on startup when pd_task is sleeping in
 * tcpci_tcpm_init.
 *
 * (enable=0) force pd_task out of the suspended state and into the
 * port's default state.
 */

void pd_set_suspend( int enable)
{
	int tries = 300;

	if (enable) {
		pd.req_suspend_state = 1;
		do {
			// getting rid of task stuff
			//task_wake(PD_PORT_TO_TASK_ID());
			if (pd.task_state == PD_STATE_SUSPENDED)
				break;
			msleep(1);
		} while (--tries != 0);
		if (!tries)
			CPRINTS("TCPC p%d set_suspend failed!", port);
	} else {
		if (pd.task_state != PD_STATE_SUSPENDED)
			CPRINTS("TCPC p%d suspend disable request "
				"while not suspended!", port);
		set_state( PD_DEFAULT_STATE());
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	}
}

int pd_is_port_enabled()
{
	switch (pd.task_state) {
	case PD_STATE_DISABLED:
	case PD_STATE_SUSPENDED:
		return 0;
	default:
		return 1;
	}
}

#if defined(CONFIG_CMD_PD) && defined(CONFIG_CMD_PD_FLASH)
static int hex8tou32(char *str, uint32_t *val)
{
	char *ptr = str;
	uint32_t tmp = 0;

	while (*ptr) {
		char c = *ptr++;
		if (c >= '0' && c <= '9')
			tmp = (tmp << 4) + (c - '0');
		else if (c >= 'A' && c <= 'F')
			tmp = (tmp << 4) + (c - 'A' + 10);
		else if (c >= 'a' && c <= 'f')
			tmp = (tmp << 4) + (c - 'a' + 10);
		else
			return EC_ERROR_INVAL;
	}
	if (ptr != str + 8)
		return EC_ERROR_INVAL;
	*val = tmp;
	return EC_SUCCESS;
}

static int remote_flashing(int argc, char **argv)
{
	int port, cnt, cmd;
	uint32_t data[VDO_MAX_SIZE-1];
	char *e;
	static int flash_offset[CONFIG_USB_PD_PORT_COUNT];

	if (argc < 4 || argc > (VDO_MAX_SIZE + 4 - 1))
		return EC_ERROR_PARAM_COUNT;

	port = strtoi(argv[1], &e, 10);
	if (*e || port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_ERROR_PARAM2;

	cnt = 0;
	if (!strcasecmp(argv[3], "erase")) {
		cmd = VDO_CMD_FLASH_ERASE;
		flash_offset[port] = 0;
		ccprintf("ERASE ...");
	} else if (!strcasecmp(argv[3], "reboot")) {
		cmd = VDO_CMD_REBOOT;
		ccprintf("REBOOT ...");
	} else if (!strcasecmp(argv[3], "signature")) {
		cmd = VDO_CMD_ERASE_SIG;
		ccprintf("ERASE SIG ...");
	} else if (!strcasecmp(argv[3], "info")) {
		cmd = VDO_CMD_READ_INFO;
		ccprintf("INFO...");
	} else if (!strcasecmp(argv[3], "version")) {
		cmd = VDO_CMD_VERSION;
		ccprintf("VERSION...");
	} else {
		int i;
		argc -= 3;
		for (i = 0; i < argc; i++)
			if (hex8tou32(argv[i+3], data + i))
				return EC_ERROR_INVAL;
		cmd = VDO_CMD_FLASH_WRITE;
		cnt = argc;
		ccprintf("WRITE %d @%04x ...", argc * 4,
			 flash_offset[port]);
		flash_offset[port] += argc * 4;
	}

	pd_send_vdm( USB_VID_GOOGLE, cmd, data, cnt);

	/* Wait until VDM is done */
	while (pd.vdm_state > 0)
		task_wait_event(100*MSEC);

	ccprintf("DONE %d\n", pd.vdm_state);
	return EC_SUCCESS;
}
#endif /* defined(CONFIG_CMD_PD) && defined(CONFIG_CMD_PD_FLASH) */

#if defined(CONFIG_USB_PD_ALT_MODE) && !defined(CONFIG_USB_PD_ALT_MODE_DFP)
void pd_send_hpd( enum hpd_event hpd)
{
	uint32_t data[1];
	int opos = pd_alt_mode( USB_SID_DISPLAYPORT);
	if (!opos)
		return;

	data[0] = VDO_DP_STATUS((hpd == hpd_irq),  /* IRQ_HPD */
				(hpd != hpd_low),  /* HPD_HI|LOW */
				0,		      /* request exit DP */
				0,		      /* request exit USB */
				0,		      /* MF pref */
				1,                    /* enabled */
				0,		      /* power low */
				0x2);
	pd_send_vdm( USB_SID_DISPLAYPORT,
		    VDO_OPOS(opos) | CMD_ATTENTION, data, 1);
	/* Wait until VDM is done. */
	while (pd[0].vdm_state > 0)
		task_wait_event(USB_PD_RX_TMOUT_US * (PD_RETRY_COUNT + 1));
}
#endif

int pd_fetch_acc_log_entry()
{
	timestamp_t timeout;

	/* Cannot send a VDM now, the host should retry */
	if (pd.vdm_state > 0)
		return pd.vdm_state == VDM_STATE_BUSY ?
				EC_RES_BUSY : EC_RES_UNAVAILABLE;

	pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_GET_LOG, NULL, 0);
	timeout.val = get_time().val + 75*MSEC;

	/* Wait until VDM is done */
	while ((pd.vdm_state > 0) &&
	       (get_time().val < timeout.val))
		task_wait_event(10*MSEC);

	if (pd.vdm_state > 0)
		return EC_RES_TIMEOUT;
	else if (pd.vdm_state < 0)
		return EC_RES_ERROR;

	return EC_RES_SUCCESS;
}

#ifdef CONFIG_USB_PD_DUAL_ROLE
void pd_request_source_voltage( int mv)
{
	pd_set_max_voltage(mv);

	if (pd.task_state == PD_STATE_SNK_READY ||
	    pd.task_state == PD_STATE_SNK_TRANSITION) {
		/* Set flag to send new power request in pd_task */
		pd.new_power_request = 1;
	} else {
		pd.power_role = PD_ROLE_SINK;
		tcpm_set_cc( TYPEC_CC_RD);
		set_state( PD_STATE_SNK_DISCONNECTED);
	}

	// getting rid of task stuff
	//task_wake(PD_PORT_TO_TASK_ID());
}

void pd_set_external_voltage_limit( int mv)
{
	pd_set_max_voltage(mv);

	if (pd.task_state == PD_STATE_SNK_READY ||
	    pd.task_state == PD_STATE_SNK_TRANSITION) {
		/* Set flag to send new power request in pd_task */
		pd.new_power_request = 1;
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	}
}

void pd_update_contract()
{
	if ((pd.task_state >= PD_STATE_SRC_NEGOCIATE) &&
	    (pd.task_state <= PD_STATE_SRC_GET_SINK_CAP)) {
		pd.flags |= PD_FLAGS_UPDATE_SRC_CAPS;
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	}
}

#endif /* CONFIG_USB_PD_DUAL_ROLE */

static int command_pd(int argc, char **argv)
{
	int port;
	char *e;

	if (argc < 2)
		return EC_ERROR_PARAM_COUNT;

#if defined(CONFIG_CMD_PD) && defined(CONFIG_USB_PD_DUAL_ROLE)
	/* command: pd <subcmd> <args> */
	if (!strcasecmp(argv[1], "dualrole")) {
		if (argc < 3) {
			ccprintf("dual-role toggling: ");
			switch (drp_state) {
			case PD_DRP_TOGGLE_ON:
				ccprintf("on\n");
				break;
			case PD_DRP_TOGGLE_OFF:
				ccprintf("off\n");
				break;
			case PD_DRP_FREEZE:
				ccprintf("freeze\n");
				break;
			case PD_DRP_FORCE_SINK:
				ccprintf("force sink\n");
				break;
			case PD_DRP_FORCE_SOURCE:
				ccprintf("force source\n");
				break;
			}
		} else {
			if (!strcasecmp(argv[2], "on"))
				pd_set_dual_role(PD_DRP_TOGGLE_ON);
			else if (!strcasecmp(argv[2], "off"))
				pd_set_dual_role(PD_DRP_TOGGLE_OFF);
			else if (!strcasecmp(argv[2], "freeze"))
				pd_set_dual_role(PD_DRP_FREEZE);
			else if (!strcasecmp(argv[2], "sink"))
				pd_set_dual_role(PD_DRP_FORCE_SINK);
			else if (!strcasecmp(argv[2], "source"))
				pd_set_dual_role(PD_DRP_FORCE_SOURCE);
			else
				return EC_ERROR_PARAM3;
		}
		return EC_SUCCESS;
	} else
#endif
	if (!strcasecmp(argv[1], "dump")) {
#ifndef CONFIG_USB_PD_DEBUG_LEVEL
		int level;

		if (argc >= 3) {
			level = strtoi(argv[2], &e, 10);
			if (*e)
				return EC_ERROR_PARAM2;
			debug_level = level;
		} else
#endif
			ccprintf("dump level: %d\n", debug_level);

		return EC_SUCCESS;
	}
#ifdef CONFIG_CMD_PD
#ifdef CONFIG_CMD_PD_DEV_DUMP_INFO
	else if (!strncasecmp(argv[1], "rwhashtable", 3)) {
		int i;
		struct ec_params_usb_pd_rw_hash_entry *p;
		for (i = 0; i < RW_HASH_ENTRIES; i++) {
			p = &rw_hash_table[i];
			pd_dev_dump_info(p->dev_id, p->dev_rw_hash);
		}
		return EC_SUCCESS;
	}
#endif /* CONFIG_CMD_PD_DEV_DUMP_INFO */
#ifdef CONFIG_USB_PD_TRY_SRC
	else if (!strncasecmp(argv[1], "trysrc", 6)) {
		int enable;

		if (argc < 2) {
			return EC_ERROR_PARAM_COUNT;
		} else if (argc >= 3) {
			enable = strtoi(argv[2], &e, 10);
			if (*e)
				return EC_ERROR_PARAM3;
			pd_try_src_enable = enable ? 1 : 0;
		}

		ccprintf("Try.SRC %s\n", pd_try_src_enable ? "on" : "off");
		return EC_SUCCESS;
	}
#endif
#endif
	/* command: pd <port> <subcmd> [args] */
	port = strtoi(argv[1], &e, 10);
	if (argc < 3)
		return EC_ERROR_PARAM_COUNT;
	if (*e || port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_ERROR_PARAM2;
#if defined(CONFIG_CMD_PD) && defined(CONFIG_USB_PD_DUAL_ROLE)

	if (!strcasecmp(argv[2], "tx")) {
		set_state( PD_STATE_SNK_DISCOVERY);
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	} else if (!strcasecmp(argv[2], "bist_rx")) {
		set_state( PD_STATE_BIST_RX);
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	} else if (!strcasecmp(argv[2], "bist_tx")) {
		if (*e)
			return EC_ERROR_PARAM3;
		set_state( PD_STATE_BIST_TX);
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	} else if (!strcasecmp(argv[2], "charger")) {
		pd.power_role = PD_ROLE_SOURCE;
		tcpm_set_cc( TYPEC_CC_RP);
		set_state( PD_STATE_SRC_DISCONNECTED);
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	} else if (!strncasecmp(argv[2], "dev", 3)) {
		int max_volt;
		if (argc >= 4)
			max_volt = strtoi(argv[3], &e, 10) * 1000;
		else
			max_volt = pd_get_max_voltage();

		pd_request_source_voltage( max_volt);
		ccprintf("max req: %dmV\n", max_volt);
	} else if (!strcasecmp(argv[2], "disable")) {
		pd_comm_enable( 0);
		ccprintf("Port C%d disable\n", port);
		return EC_SUCCESS;
	} else if (!strcasecmp(argv[2], "enable")) {
		pd_comm_enable( 1);
		ccprintf("Port C%d enabled\n", port);
		return EC_SUCCESS;
	} else if (!strncasecmp(argv[2], "hard", 4)) {
		set_state( PD_STATE_HARD_RESET_SEND);
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	} else if (!strncasecmp(argv[2], "info", 4)) {
		int i;
		ccprintf("Hash ");
		for (i = 0; i < PD_RW_HASH_SIZE / 4; i++)
			ccprintf("%08x ", pd.dev_rw_hash[i]);
		ccprintf("\nImage %s\n", system_image_copy_t_to_string(
						pd.current_image));
	} else if (!strncasecmp(argv[2], "soft", 4)) {
		set_state( PD_STATE_SOFT_RESET);
		// getting rid of task stuff
		//task_wake(PD_PORT_TO_TASK_ID());
	} else if (!strncasecmp(argv[2], "swap", 4)) {
		if (argc < 4)
			return EC_ERROR_PARAM_COUNT;

		if (!strncasecmp(argv[3], "power", 5))
			pd_request_power_swap();
		else if (!strncasecmp(argv[3], "data", 4))
			pd_request_data_swap();
#ifdef CONFIG_USBC_VCONN_SWAP
		else if (!strncasecmp(argv[3], "vconn", 5))
			pd_request_vconn_swap();
#endif
		else
			return EC_ERROR_PARAM3;
	} else if (!strncasecmp(argv[2], "ping", 4)) {
		int enable;

		if (argc > 3) {
			enable = strtoi(argv[3], &e, 10);
			if (*e)
				return EC_ERROR_PARAM3;
			pd_ping_enable( enable);
		}

		ccprintf("Pings %s\n",
			 (pd.flags & PD_FLAGS_PING_ENABLED) ?
			 "on" : "off");
	} else if (!strncasecmp(argv[2], "vdm", 3)) {
		if (argc < 4)
			return EC_ERROR_PARAM_COUNT;

		if (!strncasecmp(argv[3], "ping", 4)) {
			uint32_t enable;
			if (argc < 5)
				return EC_ERROR_PARAM_COUNT;
			enable = strtoi(argv[4], &e, 10);
			if (*e)
				return EC_ERROR_PARAM4;
			pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_PING_ENABLE,
				    &enable, 1);
		} else if (!strncasecmp(argv[3], "curr", 4)) {
			pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_CURRENT,
				    NULL, 0);
		} else if (!strncasecmp(argv[3], "vers", 4)) {
			pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_VERSION,
				    NULL, 0);
		} else {
			return EC_ERROR_PARAM_COUNT;
		}
#if defined(CONFIG_CMD_PD) && defined(CONFIG_CMD_PD_FLASH)
	} else if (!strncasecmp(argv[2], "flash", 4)) {
		return remote_flashing(argc, argv);
#endif
	} else
#endif
	if (!strncasecmp(argv[2], "state", 5)) {
		ccprintf("Port C%d CC%d, %s - Role: %s-%s%s "
			 "State: %s, Flags: 0x%04x\n",
			port, pd.polarity + 1,
			pd_comm_is_enabled() ? "Ena" : "Dis",
			pd.power_role == PD_ROLE_SOURCE ? "SRC" : "SNK",
			pd.data_role == PD_ROLE_DFP ? "DFP" : "UFP",
			(pd.flags & PD_FLAGS_VCONN_ON) ? "-VC" : "",
			pd_state_names[pd.task_state],
			pd.flags);
	} else {
		return EC_ERROR_PARAM1;
	}

	return EC_SUCCESS;
}
DECLARE_CONSOLE_COMMAND(pd, command_pd,
			"dualrole|dump|rwhashtable"
			"|trysrc [0|1]\n\t<port> "
			"[tx|bist_rx|bist_tx|charger|clock|dev|disable|enable"
			"|soft|hash|hard|ping|state|swap [power|data]|"
			"vdm [ping | curr | vers]]",
			"USB PD");

#ifdef HAS_TASK_HOSTCMD

static int hc_pd_ports(struct host_cmd_handler_args *args)
{
	struct ec_response_usb_pd_ports *r = args->response;
	r->num_ports = CONFIG_USB_PD_PORT_COUNT;

	args->response_size = sizeof(*r);
	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_USB_PD_PORTS,
		     hc_pd_ports,
		     EC_VER_MASK(0));

static const enum pd_dual_role_states dual_role_map[USB_PD_CTRL_ROLE_COUNT] = {
	[USB_PD_CTRL_ROLE_TOGGLE_ON]    = PD_DRP_TOGGLE_ON,
	[USB_PD_CTRL_ROLE_TOGGLE_OFF]   = PD_DRP_TOGGLE_OFF,
	[USB_PD_CTRL_ROLE_FORCE_SINK]   = PD_DRP_FORCE_SINK,
	[USB_PD_CTRL_ROLE_FORCE_SOURCE] = PD_DRP_FORCE_SOURCE,
	[USB_PD_CTRL_ROLE_FREEZE]       = PD_DRP_FREEZE,
};

#ifdef CONFIG_USBC_SS_MUX
static const enum typec_mux typec_mux_map[USB_PD_CTRL_MUX_COUNT] = {
	[USB_PD_CTRL_MUX_NONE] = TYPEC_MUX_NONE,
	[USB_PD_CTRL_MUX_USB]  = TYPEC_MUX_USB,
	[USB_PD_CTRL_MUX_AUTO] = TYPEC_MUX_DP,
	[USB_PD_CTRL_MUX_DP]   = TYPEC_MUX_DP,
	[USB_PD_CTRL_MUX_DOCK] = TYPEC_MUX_DOCK,
};
#endif

static int hc_usb_pd_control(struct host_cmd_handler_args *args)
{
	const struct ec_params_usb_pd_control *p = args->params;
	struct ec_response_usb_pd_control_v1 *r_v1 = args->response;
	struct ec_response_usb_pd_control *r = args->response;

	if (p->port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_RES_INVALID_PARAM;

	if (p->role >= USB_PD_CTRL_ROLE_COUNT ||
	    p->mux >= USB_PD_CTRL_MUX_COUNT)
		return EC_RES_INVALID_PARAM;

	if (p->role != USB_PD_CTRL_ROLE_NO_CHANGE)
		pd_set_dual_role(dual_role_map[p->role]);

#ifdef CONFIG_USBC_SS_MUX
	if (p->mux != USB_PD_CTRL_MUX_NO_CHANGE)
		usb_mux_set(p->port, typec_mux_map[p->mux],
			    typec_mux_map[p->mux] == TYPEC_MUX_NONE ?
			    USB_SWITCH_DISCONNECT :
			    USB_SWITCH_CONNECT,
			    pd_get_polarity(p->port));
#endif /* CONFIG_USBC_SS_MUX */

	if (p->swap == USB_PD_CTRL_SWAP_DATA)
		pd_request_data_swap(p->port);
#ifdef CONFIG_USB_PD_DUAL_ROLE
	else if (p->swap == USB_PD_CTRL_SWAP_POWER)
		pd_request_power_swap(p->port);
#ifdef CONFIG_USBC_VCONN_SWAP
	else if (p->swap == USB_PD_CTRL_SWAP_VCONN)
		pd_request_vconn_swap(p->port);
#endif
#endif

	if (args->version == 0) {
		r->enabled = pd_comm_is_enabled(p->port);
		r->role = pd[p->port].power_role;
		r->polarity = pd[p->port].polarity;
		r->state = pd[p->port].task_state;
		args->response_size = sizeof(*r);
	} else {
		r_v1->enabled =
			(pd_comm_is_enabled(p->port) ?
				PD_CTRL_RESP_ENABLED_COMMS : 0) |
			(pd_is_connected(p->port) ?
				PD_CTRL_RESP_ENABLED_CONNECTED : 0) |
			((pd[p->port].flags & PD_FLAGS_PREVIOUS_PD_CONN) ?
				PD_CTRL_RESP_ENABLED_PD_CAPABLE : 0);
		r_v1->role =
			(pd[p->port].power_role ? PD_CTRL_RESP_ROLE_POWER : 0) |
			(pd[p->port].data_role ? PD_CTRL_RESP_ROLE_DATA : 0) |
			((pd[p->port].flags & PD_FLAGS_VCONN_ON) ?
				PD_CTRL_RESP_ROLE_VCONN : 0) |
			((pd[p->port].flags & PD_FLAGS_PARTNER_DR_POWER) ?
				PD_CTRL_RESP_ROLE_DR_POWER : 0) |
			((pd[p->port].flags & PD_FLAGS_PARTNER_DR_DATA) ?
				PD_CTRL_RESP_ROLE_DR_DATA : 0) |
			((pd[p->port].flags & PD_FLAGS_PARTNER_USB_COMM) ?
				PD_CTRL_RESP_ROLE_USB_COMM : 0) |
			((pd[p->port].flags & PD_FLAGS_PARTNER_EXTPOWER) ?
				PD_CTRL_RESP_ROLE_EXT_POWERED : 0);
		r_v1->polarity = pd[p->port].polarity;
		strzcpy(r_v1->state,
			pd_state_names[pd[p->port].task_state],
			sizeof(r_v1->state));
		args->response_size = sizeof(*r_v1);
	}
	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_USB_PD_CONTROL,
		     hc_usb_pd_control,
		     EC_VER_MASK(0) | EC_VER_MASK(1));

static int hc_remote_flash(struct host_cmd_handler_args *args)
{
	const struct ec_params_usb_pd_fw_update *p = args->params;
	int port = p->port;
	const uint32_t *data = &(p->size) + 1;
	int i, size, rv = EC_RES_SUCCESS;
	timestamp_t timeout;

	if (port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_RES_INVALID_PARAM;

	if (p->size + sizeof(*p) > args->params_size)
		return EC_RES_INVALID_PARAM;

#if defined(CONFIG_BATTERY_PRESENT_CUSTOM) ||	\
	defined(CONFIG_BATTERY_PRESENT_GPIO)
	/*
	 * Do not allow PD firmware update if no battery and this port
	 * is sinking power, because we will lose power.
	 */
	if (battery_is_present() != BP_YES &&
	    charge_manager_get_active_charge_port() == port)
		return EC_RES_UNAVAILABLE;
#endif

	/*
	 * Busy still with a VDM that host likely generated.  1 deep VDM queue
	 * so just return for retry logic on host side to deal with.
	 */
	if (pd.vdm_state > 0)
		return EC_RES_BUSY;

	switch (p->cmd) {
	case USB_PD_FW_REBOOT:
		pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_REBOOT, NULL, 0);

		/*
		 * Return immediately to free pending i2c bus.	Host needs to
		 * manage this delay.
		 */
		return EC_RES_SUCCESS;

	case USB_PD_FW_FLASH_ERASE:
		pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_FLASH_ERASE, NULL, 0);

		/*
		 * Return immediately.	Host needs to manage delays here which
		 * can be as long as 1.2 seconds on 64KB RW flash.
		 */
		return EC_RES_SUCCESS;

	case USB_PD_FW_ERASE_SIG:
		pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_ERASE_SIG, NULL, 0);
		timeout.val = get_time().val + 500*MSEC;
		break;

	case USB_PD_FW_FLASH_WRITE:
		/* Data size must be a multiple of 4 */
		if (!p->size || p->size % 4)
			return EC_RES_INVALID_PARAM;

		size = p->size / 4;
		for (i = 0; i < size; i += VDO_MAX_SIZE - 1) {
			pd_send_vdm( USB_VID_GOOGLE, VDO_CMD_FLASH_WRITE,
				    data + i, MIN(size - i, VDO_MAX_SIZE - 1));
			timeout.val = get_time().val + 500*MSEC;

			/* Wait until VDM is done */
			while ((pd.vdm_state > 0) &&
			       (get_time().val < timeout.val))
				task_wait_event(10*MSEC);

			if (pd.vdm_state > 0)
				return EC_RES_TIMEOUT;
		}
		return EC_RES_SUCCESS;

	default:
		return EC_RES_INVALID_PARAM;
		break;
	}

	/* Wait until VDM is done or timeout */
	while ((pd.vdm_state > 0) && (get_time().val < timeout.val))
		task_wait_event(50*MSEC);

	if ((pd.vdm_state > 0) ||
	    (pd.vdm_state == VDM_STATE_ERR_TMOUT))
		rv = EC_RES_TIMEOUT;
	else if (pd.vdm_state < 0)
		rv = EC_RES_ERROR;

	return rv;
}
DECLARE_HOST_COMMAND(EC_CMD_USB_PD_FW_UPDATE,
		     hc_remote_flash,
		     EC_VER_MASK(0));

static int hc_remote_rw_hash_entry(struct host_cmd_handler_args *args)
{
	int i, idx = 0, found = 0;
	const struct ec_params_usb_pd_rw_hash_entry *p = args->params;
	static int rw_hash_next_idx;

	if (!p->dev_id)
		return EC_RES_INVALID_PARAM;

	for (i = 0; i < RW_HASH_ENTRIES; i++) {
		if (p->dev_id == rw_hash_table[i].dev_id) {
			idx = i;
			found = 1;
			break;
		}
	}
	if (!found) {
		idx = rw_hash_next_idx;
		rw_hash_next_idx = rw_hash_next_idx + 1;
		if (rw_hash_next_idx == RW_HASH_ENTRIES)
			rw_hash_next_idx = 0;
	}
	memcpy(&rw_hash_table[idx], p, sizeof(*p));

	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_USB_PD_RW_HASH_ENTRY,
		     hc_remote_rw_hash_entry,
		     EC_VER_MASK(0));

static int hc_remote_pd_dev_info(struct host_cmd_handler_args *args)
{
	const uint8_t *port = args->params;
	struct ec_params_usb_pd_rw_hash_entry *r = args->response;

	if (*port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_RES_INVALID_PARAM;

	r->dev_id = pd[*port].dev_id;

	if (r->dev_id) {
		memcpy(r->dev_rw_hash, pd[*port].dev_rw_hash,
		       PD_RW_HASH_SIZE);
	}

	r->current_image = pd[*port].current_image;

	args->response_size = sizeof(*r);
	return EC_RES_SUCCESS;
}

DECLARE_HOST_COMMAND(EC_CMD_USB_PD_DEV_INFO,
		     hc_remote_pd_dev_info,
		     EC_VER_MASK(0));

#ifndef CONFIG_USB_PD_TCPC
#ifdef CONFIG_EC_CMD_PD_CHIP_INFO
static int hc_remote_pd_chip_info(struct host_cmd_handler_args *args)
{
	const struct ec_params_pd_chip_info *p = args->params;
	struct ec_response_pd_chip_info *r = args->response, *info;

	if (p->port >= CONFIG_USB_PD_PORT_COUNT)
		return EC_RES_INVALID_PARAM;

	if (tcpm_get_chip_info(p->port, p->renew, &info))
		return EC_RES_ERROR;

	memcpy(r, info, sizeof(*r));
	args->response_size = sizeof(*r);

	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_PD_CHIP_INFO,
		     hc_remote_pd_chip_info,
		     EC_VER_MASK(0));
#endif
#endif

#ifdef CONFIG_USB_PD_ALT_MODE_DFP
static int hc_remote_pd_set_amode(struct host_cmd_handler_args *args)
{
	const struct ec_params_usb_pd_set_mode_request *p = args->params;

	if ((p->port >= CONFIG_USB_PD_PORT_COUNT) || (!p->svid) || (!p->opos))
		return EC_RES_INVALID_PARAM;

	switch (p->cmd) {
	case PD_EXIT_MODE:
		if (pd_dfp_exit_mode(p->port, p->svid, p->opos))
			pd_send_vdm(p->port, p->svid,
				    CMD_EXIT_MODE | VDO_OPOS(p->opos), NULL, 0);
		else {
			CPRINTF("Failed exit mode\n");
			return EC_RES_ERROR;
		}
		break;
	case PD_ENTER_MODE:
		if (pd_dfp_enter_mode(p->port, p->svid, p->opos))
			pd_send_vdm(p->port, p->svid, CMD_ENTER_MODE |
				    VDO_OPOS(p->opos), NULL, 0);
		break;
	default:
		return EC_RES_INVALID_PARAM;
	}
	return EC_RES_SUCCESS;
}
DECLARE_HOST_COMMAND(EC_CMD_USB_PD_SET_AMODE,
		     hc_remote_pd_set_amode,
		     EC_VER_MASK(0));
#endif /* CONFIG_USB_PD_ALT_MODE_DFP */

#endif /* HAS_TASK_HOSTCMD */

#ifdef CONFIG_CMD_PD_CONTROL

static int pd_control(struct host_cmd_handler_args *args)
{
	static int pd_control_disabled[CONFIG_USB_PD_PORT_COUNT];
	const struct ec_params_pd_control *cmd = args->params;
	int enable = 0;

	if (cmd->chip >= CONFIG_USB_PD_PORT_COUNT)
		return EC_RES_INVALID_PARAM;

	/* Always allow disable command */
	if (cmd->subcmd == PD_CONTROL_DISABLE) {
		pd_control_disabled[cmd->chip] = 1;
		return EC_RES_SUCCESS;
	}

	if (pd_control_disabled[cmd->chip])
		return EC_RES_ACCESS_DENIED;

	if (cmd->subcmd == PD_SUSPEND) {
		enable = 0;
	} else if (cmd->subcmd == PD_RESUME) {
		enable = 1;
	} else if (cmd->subcmd == PD_RESET) {
#ifdef HAS_TASK_PDCMD
		board_reset_pd_mcu();
#else
		return EC_RES_INVALID_COMMAND;
#endif
	} else if (cmd->subcmd == PD_CHIP_ON && board_set_tcpc_power_mode) {
		board_set_tcpc_power_mode(cmd->chip, 1);
		return EC_RES_SUCCESS;
	} else {
		return EC_RES_INVALID_COMMAND;
	}

	pd_comm_enable(cmd->chip, enable);
	pd_set_suspend(cmd->chip, !enable);

	return EC_RES_SUCCESS;
}

DECLARE_HOST_COMMAND(EC_CMD_PD_CONTROL, pd_control, EC_VER_MASK(0));
#endif /* CONFIG_CMD_PD_CONTROL */

#endif /* CONFIG_COMMON_RUNTIME */
