/* Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* USB Power delivery module */

#ifndef __USB_PD_H
#define __USB_PD_H

#include "tcpm_driver.h"
#include "usb_pd_driver.h"

/* Time units in microseconds */
#define MSEC         (1000)
#define SECOND    (1000000)
#define MINUTE   (60000000)
#define HOUR   (3600000000ull)  /* Too big to fit in a signed int */

/* PD Host command timeout */
#define PD_HOST_COMMAND_TIMEOUT_US SECOND

#ifdef CONFIG_USB_PD_PORT_COUNT
/*
 * Define PD_PORT_TO_TASK_ID() and TASK_ID_TO_PD_PORT() macros to
 * go between PD port number and task ID. Assume that TASK_ID_PD_C0 is the
 * lowest task ID and IDs are on a continuous range.
 */
#ifdef HAS_TASK_PD_C0
#define PD_PORT_TO_TASK_ID(port) (TASK_ID_PD_C0 + (port))
#define TASK_ID_TO_PD_PORT(id) ((id) - TASK_ID_PD_C0)
#else
#define PD_PORT_TO_TASK_ID(port) -1 /* dummy task ID */
#define TASK_ID_TO_PD_PORT(id) 0
#endif /* CONFIG_COMMON_RUNTIME */
#endif /* CONFIG_USB_PD_PORT_COUNT */

enum pd_rx_errors {
	PD_RX_ERR_INVAL = -1,           /* Invalid packet */
	PD_RX_ERR_HARD_RESET = -2,      /* Got a Hard-Reset packet */
	PD_RX_ERR_CRC = -3,             /* CRC mismatch */
	PD_RX_ERR_ID = -4,              /* Invalid ID number */
	PD_RX_ERR_UNSUPPORTED_SOP = -5, /* Unsupported SOP */
	PD_RX_ERR_CABLE_RESET = -6      /* Got a Cable-Reset packet */
};

/* Events for USB PD task */
#define PD_EVENT_RX               (1<<2) /* Incoming packet event */
#define PD_EVENT_TX               (1<<3) /* Outgoing packet event */
#define PD_EVENT_CC               (1<<4) /* CC line change event */
#define PD_EVENT_TCPC_RESET       (1<<5) /* TCPC has reset */
#define PD_EVENT_UPDATE_DUAL_ROLE (1<<6) /* DRP state has changed */

/* --- PD data message helpers --- */
#define PDO_MAX_OBJECTS   7
#define PDO_MODES (PDO_MAX_OBJECTS - 1)

/* PDO : Power Data Object */
/*
 * 1. The vSafe5V Fixed Supply Object shall always be the first object.
 * 2. The remaining Fixed Supply Objects,
 *    if present, shall be sent in voltage order; lowest to highest.
 * 3. The Battery Supply Objects,
 *    if present shall be sent in Minimum Voltage order; lowest to highest.
 * 4. The Variable Supply (non battery) Objects,
 *    if present, shall be sent in Minimum Voltage order; lowest to highest.
 * 5. (PD3.0) The Augmented PDO is defined to allow extension beyond the 4 PDOs
 *     above by examining bits <29:28> to determine the additional PDO function.
 */
#define PDO_TYPE_FIXED     (0 << 30)
#define PDO_TYPE_BATTERY   (1 << 30)
#define PDO_TYPE_VARIABLE  (2 << 30)
#define PDO_TYPE_AUGMENTED (3 << 30)
#define PDO_TYPE_MASK      (3 << 30)

#define PDO_FIXED_DUAL_ROLE (1 << 29) /* Dual role device */
#define PDO_FIXED_SUSPEND   (1 << 28) /* USB Suspend supported */
#define PDO_FIXED_EXTERNAL  (1 << 27) /* Externally powered */
#define PDO_FIXED_COMM_CAP  (1 << 26) /* USB Communications Capable */
#define PDO_FIXED_DATA_SWAP (1 << 25) /* Data role swap command supported */
#define PDO_FIXED_PEAK_CURR () /* [21..20] Peak current */
#define PDO_FIXED_VOLT(mv)  (((mv)/50) << 10) /* Voltage in 50mV units */
#define PDO_FIXED_CURR(ma)  (((ma)/10) << 0)  /* Max current in 10mA units */

#define PDO_FIXED(mv, ma, flags) (PDO_FIXED_VOLT(mv) |\
				  PDO_FIXED_CURR(ma) | (flags))

#define PDO_VAR_MAX_VOLT(mv) ((((mv) / 50) & 0x3FF) << 20)
#define PDO_VAR_MIN_VOLT(mv) ((((mv) / 50) & 0x3FF) << 10)
#define PDO_VAR_OP_CURR(ma)  ((((ma) / 10) & 0x3FF) << 0)

#define PDO_VAR(min_mv, max_mv, op_ma) \
				(PDO_VAR_MIN_VOLT(min_mv) | \
				 PDO_VAR_MAX_VOLT(max_mv) | \
				 PDO_VAR_OP_CURR(op_ma)   | \
				 PDO_TYPE_VARIABLE)

#define PDO_BATT_MAX_VOLT(mv) ((((mv) / 50) & 0x3FF) << 20)
#define PDO_BATT_MIN_VOLT(mv) ((((mv) / 50) & 0x3FF) << 10)
#define PDO_BATT_OP_POWER(mw) ((((mw) / 250) & 0x3FF) << 0)

#define PDO_BATT(min_mv, max_mv, op_mw) \
				(PDO_BATT_MIN_VOLT(min_mv) | \
				 PDO_BATT_MAX_VOLT(max_mv) | \
				 PDO_BATT_OP_POWER(op_mw) | \
				 PDO_TYPE_BATTERY)

/* RDO : Request Data Object */
#define RDO_OBJ_POS(n)             (((n) & 0x7) << 28)
#define RDO_POS(rdo)               (((rdo) >> 28) & 0x7)
#define RDO_GIVE_BACK              (1 << 27)
#define RDO_CAP_MISMATCH           (1 << 26)
#define RDO_COMM_CAP               (1 << 25)
#define RDO_NO_SUSPEND             (1 << 24)
#define RDO_FIXED_VAR_OP_CURR(ma)  ((((ma) / 10) & 0x3FF) << 10)
#define RDO_FIXED_VAR_MAX_CURR(ma) ((((ma) / 10) & 0x3FF) << 0)

#define RDO_BATT_OP_POWER(mw)      ((((mw) / 250) & 0x3FF) << 10)
#define RDO_BATT_MAX_POWER(mw)     ((((mw) / 250) & 0x3FF) << 10)

#define RDO_FIXED(n, op_ma, max_ma, flags) \
				(RDO_OBJ_POS(n) | (flags) | \
				RDO_FIXED_VAR_OP_CURR(op_ma) | \
				RDO_FIXED_VAR_MAX_CURR(max_ma))


#define RDO_BATT(n, op_mw, max_mw, flags) \
				(RDO_OBJ_POS(n) | (flags) | \
				RDO_BATT_OP_POWER(op_mw) | \
				RDO_BATT_MAX_POWER(max_mw))

/* BDO : BIST Data Object */
#define BDO_MODE_RECV       (0 << 28)
#define BDO_MODE_TRANSMIT   (1 << 28)
#define BDO_MODE_COUNTERS   (2 << 28)
#define BDO_MODE_CARRIER0   (3 << 28)
#define BDO_MODE_CARRIER1   (4 << 28)
#define BDO_MODE_CARRIER2   (5 << 28)
#define BDO_MODE_CARRIER3   (6 << 28)
#define BDO_MODE_EYE        (7 << 28)

#define BDO(mode, cnt)      ((mode) | ((cnt) & 0xFFFF))

#define SVID_DISCOVERY_MAX 16

/* Timers */
#define PD_T_SINK_TX          (18*MSEC) /* between 16ms and 20 */
#define PD_T_CHUNK_SENDER_RSP (24*MSEC) /* between 24ms and 30ms */
#define PD_T_CHUNK_SENDER_REQ (24*MSEC) /* between 24ms and 30ms */
#define PD_T_SEND_SOURCE_CAP  (100*MSEC) /* between 100ms and 200ms */
#define PD_T_SINK_WAIT_CAP    (600*MSEC) /* between 310ms and 620ms */
#define PD_T_SINK_TRANSITION   (35*MSEC) /* between 20ms and 35ms */
#define PD_T_SOURCE_ACTIVITY   (45*MSEC) /* between 40ms and 50ms */
//#define PD_T_SENDER_RESPONSE   (30*MSEC) /* between 24ms and 30ms */
#define PD_T_SENDER_RESPONSE   (100*MSEC) /* between 24ms and 30ms */
#define PD_T_PS_TRANSITION    (500*MSEC) /* between 450ms and 550ms */
#define PD_T_PS_SOURCE_ON     (480*MSEC) /* between 390ms and 480ms */
#define PD_T_PS_SOURCE_OFF    (920*MSEC) /* between 750ms and 920ms */
#define PD_T_PS_HARD_RESET     (25*MSEC) /* between 25ms and 35ms */
#define PD_T_ERROR_RECOVERY    (25*MSEC) /* 25ms */
#define PD_T_CC_DEBOUNCE       (100*MSEC) /* between 100ms and 200ms */
/* DRP_SNK + DRP_SRC must be between 50ms and 100ms with 30%-70% duty cycle */
#define PD_T_DRP_SNK           (40*MSEC) /* toggle time for sink DRP */
#define PD_T_DRP_SRC           (30*MSEC) /* toggle time for source DRP */
#define PD_T_DEBOUNCE          (15*MSEC) /* between 10ms and 20ms */
#define PD_T_SINK_ADJ          (55*MSEC) /* between PD_T_DEBOUNCE and 60ms */
#define PD_T_SRC_RECOVER      (760*MSEC) /* between 660ms and 1000ms */
#define PD_T_SRC_RECOVER_MAX (1000*MSEC) /* 1000ms */
#define PD_T_SRC_TURN_ON      (275*MSEC) /* 275ms */
#define PD_T_SAFE_0V          (650*MSEC) /* 650ms */
#define PD_T_NO_RESPONSE     (5500*MSEC) /* between 4.5s and 5.5s */
#define PD_T_BIST_TRANSMIT     (50*MSEC) /* 50ms (used for task_wait arg) */
#define PD_T_BIST_RECEIVE      (60*MSEC) /* 60ms (max time to process bist) */
#define PD_T_VCONN_SOURCE_ON  (100*MSEC) /* 100ms */
#define PD_T_TRY_SRC          (125*MSEC) /* Max time for Try.SRC state */
#define PD_T_TRY_WAIT         (600*MSEC) /* Max time for TryWait.SNK state */
#define PD_T_SINK_REQUEST     (100*MSEC) /* Wait 100ms before next request */

/* number of edges and time window to detect CC line is not idle */
#define PD_RX_TRANSITION_COUNT  3
#define PD_RX_TRANSITION_WINDOW 20 /* between 12us and 20us */

/* from USB Type-C Specification Table 5-1 */
#define PD_T_AME (1*SECOND) /* timeout from UFP attach to Alt Mode Entry */

/* VDM Timers ( USB PD Spec Rev2.0 Table 6-30 )*/
#define PD_T_VDM_BUSY         (100*MSEC) /* at least 100ms */
#define PD_T_VDM_E_MODE        (25*MSEC) /* enter/exit the same max */
#define PD_T_VDM_RCVR_RSP      (15*MSEC) /* max of 15ms */
#define PD_T_VDM_SNDR_RSP      (30*MSEC) /* max of 30ms */
#define PD_T_VDM_WAIT_MODE_E  (100*MSEC) /* enter/exit the same max */

/* function table for entered mode */
struct amode_fx {
	int (*status)(int port, uint32_t *payload);
	int (*config)(int port, uint32_t *payload);
};

/* function table for alternate mode capable responders */
struct svdm_response {
	int (*identity)(int port, uint32_t *payload);
	int (*svids)(int port, uint32_t *payload);
	int (*modes)(int port, uint32_t *payload);
	int (*enter_mode)(int port, uint32_t *payload);
	int (*exit_mode)(int port, uint32_t *payload);
	struct amode_fx *amode;
};

struct svdm_svid_data {
	uint16_t svid;
	int mode_cnt;
	uint32_t mode_vdo[PDO_MODES];
};

struct svdm_amode_fx {
	uint16_t svid;
	int (*enter)(int port, uint32_t mode_caps);
	int (*status)(int port, uint32_t *payload);
	int (*config)(int port, uint32_t *payload);
	void (*post_config)(int port);
	int (*attention)(int port, uint32_t *payload);
	void (*exit)(int port);
};

/* defined in <board>/usb_pd_policy.c */
/* All UFP_U should have */
extern const struct svdm_response svdm_rsp;
/* All DFP_U should have */
extern const struct svdm_amode_fx supported_modes[];
extern const int supported_modes_cnt;

/* DFP data needed to support alternate mode entry and exit */
struct svdm_amode_data {
	const struct svdm_amode_fx *fx;
	/* VDM object position */
	int opos;
	/* mode capabilities specific to SVID amode. */
	struct svdm_svid_data *data;
};

enum hpd_event {
	hpd_none,
	hpd_low,
	hpd_high,
	hpd_irq,
};

/* DisplayPort flags */
#define DP_FLAGS_DP_ON              (1 << 0) /* Display port mode is on */
#define DP_FLAGS_HPD_HI_PENDING     (1 << 1) /* Pending HPD_HI */

/* supported alternate modes */
enum pd_alternate_modes {
	PD_AMODE_GOOGLE,
	PD_AMODE_DISPLAYPORT,
	/* not a real mode */
	PD_AMODE_COUNT,
};

/* Policy structure for driving alternate mode */
struct pd_policy {
	/* index of svid currently being operated on */
	int svid_idx;
	/* count of svids discovered */
	int svid_cnt;
	/* SVDM identity info (Id, Cert Stat, 0-4 Typec specific) */
	uint32_t identity[PDO_MAX_OBJECTS - 1];
	/* supported svids & corresponding vdo mode data */
	struct svdm_svid_data svids[SVID_DISCOVERY_MAX];
	/*  active modes */
	struct svdm_amode_data amodes[PD_AMODE_COUNT];
	/* Next index to insert DFP alternate mode into amodes */
	int amode_idx;
};

/*
 * VDO : Vendor Defined Message Object
 * VDM object is minimum of VDM header + 6 additional data objects.
 */

#define VDO_MAX_SIZE 7

#define VDM_VER10 0
#define VDM_VER20 1

/*
 * VDM header
 * ----------
 * <31:16>  :: SVID
 * <15>     :: VDM type ( 1b == structured, 0b == unstructured )
 * <14:13>  :: Structured VDM version (00b == Rev 2.0, 01b == Rev 3.0 )
 * <12:11>  :: reserved
 * <10:8>   :: object position (1-7 valid ... used for enter/exit mode only)
 * <7:6>    :: command type (SVDM only?)
 * <5>      :: reserved (SVDM), command type (UVDM)
 * <4:0>    :: command
 */
#define VDO(vid, type, custom) \
	(((vid) << 16) |       \
	((type) << 15) |       \
	((custom) & 0x7FFF))

#define VDO_SVDM_TYPE     (1 << 15)
#define VDO_SVDM_VERS(x)  (x << 13)
#define VDO_OPOS(x)       (x << 8)
#define VDO_CMDT(x)       (x << 6)
#define VDO_OPOS_MASK     VDO_OPOS(0x7)
#define VDO_CMDT_MASK     VDO_CMDT(0x3)

#define CMDT_INIT     0
#define CMDT_RSP_ACK  1
#define CMDT_RSP_NAK  2
#define CMDT_RSP_BUSY 3


/* reserved for SVDM ... for Google UVDM */
#define VDO_SRC_INITIATOR (0 << 5)
#define VDO_SRC_RESPONDER (1 << 5)

#define CMD_DISCOVER_IDENT  1
#define CMD_DISCOVER_SVID   2
#define CMD_DISCOVER_MODES  3
#define CMD_ENTER_MODE      4
#define CMD_EXIT_MODE       5
#define CMD_ATTENTION       6
#define CMD_DP_STATUS      16
#define CMD_DP_CONFIG      17

#define VDO_CMD_VENDOR(x)    (((10 + (x)) & 0x1f))

/* ChromeOS specific commands */
#define VDO_CMD_VERSION      VDO_CMD_VENDOR(0)
#define VDO_CMD_SEND_INFO    VDO_CMD_VENDOR(1)
#define VDO_CMD_READ_INFO    VDO_CMD_VENDOR(2)
#define VDO_CMD_REBOOT       VDO_CMD_VENDOR(5)
#define VDO_CMD_FLASH_ERASE  VDO_CMD_VENDOR(6)
#define VDO_CMD_FLASH_WRITE  VDO_CMD_VENDOR(7)
#define VDO_CMD_ERASE_SIG    VDO_CMD_VENDOR(8)
#define VDO_CMD_PING_ENABLE  VDO_CMD_VENDOR(10)
#define VDO_CMD_CURRENT      VDO_CMD_VENDOR(11)
#define VDO_CMD_FLIP         VDO_CMD_VENDOR(12)
#define VDO_CMD_GET_LOG      VDO_CMD_VENDOR(13)
#define VDO_CMD_CCD_EN       VDO_CMD_VENDOR(14)

#define PD_VDO_VID(vdo)  ((vdo) >> 16)
#define PD_VDO_SVDM(vdo) (((vdo) >> 15) & 1)
#define PD_VDO_OPOS(vdo) (((vdo) >> 8) & 0x7)
#define PD_VDO_CMD(vdo)  ((vdo) & 0x1f)
#define PD_VDO_CMDT(vdo) (((vdo) >> 6) & 0x3)

/*
 * SVDM Identity request -> response
 *
 * Request is simply properly formatted SVDM header
 *
 * Response is 4 data objects:
 * [0] :: SVDM header
 * [1] :: Identitiy header
 * [2] :: Cert Stat VDO
 * [3] :: (Product | Cable) VDO
 * [4] :: AMA VDO
 *
 */
#define VDO_INDEX_HDR     0
#define VDO_INDEX_IDH     1
#define VDO_INDEX_CSTAT   2
#define VDO_INDEX_CABLE   3
#define VDO_INDEX_PRODUCT 3
#define VDO_INDEX_AMA     4
#define VDO_I(name) VDO_INDEX_##name

/*
 * SVDM Identity Header
 * --------------------
 * <31>     :: data capable as a USB host
 * <30>     :: data capable as a USB device
 * <29:27>  :: product type
 * <26>     :: modal operation supported (1b == yes)
 * <25:16>  :: SBZ
 * <15:0>   :: USB-IF assigned VID for this cable vendor
 */
#define IDH_PTYPE_UNDEF  0
#define IDH_PTYPE_HUB    1
#define IDH_PTYPE_PERIPH 2
#define IDH_PTYPE_PCABLE 3
#define IDH_PTYPE_ACABLE 4
#define IDH_PTYPE_AMA    5

#define VDO_IDH(usbh, usbd, ptype, is_modal, vid)		\
	((usbh) << 31 | (usbd) << 30 | ((ptype) & 0x7) << 27	\
	 | (is_modal) << 26 | ((vid) & 0xffff))

#define PD_IDH_PTYPE(vdo) (((vdo) >> 27) & 0x7)
#define PD_IDH_VID(vdo)   ((vdo) & 0xffff)

/*
 * Cert Stat VDO
 * -------------
 * <31:20> : SBZ
 * <19:0>  : USB-IF assigned TID for this cable
 */
#define VDO_CSTAT(tid)    ((tid) & 0xfffff)
#define PD_CSTAT_TID(vdo) ((vdo) & 0xfffff)

/*
 * Product VDO
 * -----------
 * <31:16> : USB Product ID
 * <15:0>  : USB bcdDevice
 */
#define VDO_PRODUCT(pid, bcd) (((pid) & 0xffff) << 16 | ((bcd) & 0xffff))
#define PD_PRODUCT_PID(vdo) (((vdo) >> 16) & 0xffff)

/*
 * Cable VDO
 * ---------
 * <31:28> :: Cable HW version
 * <27:24> :: Cable FW version
 * <23:20> :: SBZ
 * <19:18> :: type-C to Type-A/B/C (00b == A, 01 == B, 10 == C)
 * <17>    :: Type-C to Plug/Receptacle (0b == plug, 1b == receptacle)
 * <16:13> :: cable latency (0001 == <10ns(~1m length))
 * <12:11> :: cable termination type (11b == both ends active VCONN req)
 * <10>    :: SSTX1 Directionality support (0b == fixed, 1b == cfgable)
 * <9>     :: SSTX2 Directionality support
 * <8>     :: SSRX1 Directionality support
 * <7>     :: SSRX2 Directionality support
 * <6:5>   :: Vbus current handling capability
 * <4>     :: Vbus through cable (0b == no, 1b == yes)
 * <3>     :: SOP" controller present? (0b == no, 1b == yes)
 * <2:0>   :: USB SS Signaling support
 */
#define CABLE_ATYPE 0
#define CABLE_BTYPE 1
#define CABLE_CTYPE 2
#define CABLE_PLUG       0
#define CABLE_RECEPTACLE 1
#define CABLE_CURR_1A5   0
#define CABLE_CURR_3A    1
#define CABLE_CURR_5A    2
#define CABLE_USBSS_U2_ONLY  0
#define CABLE_USBSS_U31_GEN1 1
#define CABLE_USBSS_U31_GEN2 2
#define VDO_CABLE(hw, fw, cbl, gdr, lat, term, tx1d, tx2d, rx1d, rx2d, cur, vps, sopp, usbss) \
	(((hw) & 0x7) << 28 | ((fw) & 0x7) << 24 | ((cbl) & 0x3) << 18	\
	 | (gdr) << 17 | ((lat) & 0x7) << 13 | ((term) & 0x3) << 11	\
	 | (tx1d) << 10 | (tx2d) << 9 | (rx1d) << 8 | (rx2d) << 7	\
	 | ((cur) & 0x3) << 5 | (vps) << 4 | (sopp) << 3		\
	 | ((usbss) & 0x7))

/*
 * AMA VDO
 * ---------
 * <31:28> :: Cable HW version
 * <27:24> :: Cable FW version
 * <23:12> :: SBZ
 * <11>    :: SSTX1 Directionality support (0b == fixed, 1b == cfgable)
 * <10>    :: SSTX2 Directionality support
 * <9>     :: SSRX1 Directionality support
 * <8>     :: SSRX2 Directionality support
 * <7:5>   :: Vconn power
 * <4>     :: Vconn power required
 * <3>     :: Vbus power required
 * <2:0>   :: USB SS Signaling support
 */
#define VDO_AMA(hw, fw, tx1d, tx2d, rx1d, rx2d, vcpwr, vcr, vbr, usbss) \
	(((hw) & 0x7) << 28 | ((fw) & 0x7) << 24			\
	 | (tx1d) << 11 | (tx2d) << 10 | (rx1d) << 9 | (rx2d) << 8	\
	 | ((vcpwr) & 0x3) << 5 | (vcr) << 4 | (vbr) << 3		\
	 | ((usbss) & 0x7))

#define PD_VDO_AMA_VCONN_REQ(vdo) (((vdo) >> 4) & 1)
#define PD_VDO_AMA_VBUS_REQ(vdo)  (((vdo) >> 3) & 1)

#define AMA_VCONN_PWR_1W   0
#define AMA_VCONN_PWR_1W5  1
#define AMA_VCONN_PWR_2W   2
#define AMA_VCONN_PWR_3W   3
#define AMA_VCONN_PWR_4W   4
#define AMA_VCONN_PWR_5W   5
#define AMA_VCONN_PWR_6W   6
#define AMA_USBSS_U2_ONLY  0
#define AMA_USBSS_U31_GEN1 1
#define AMA_USBSS_U31_GEN2 2
#define AMA_USBSS_BBONLY   3

/*
 * SVDM Discover SVIDs request -> response
 *
 * Request is properly formatted VDM Header with discover SVIDs command.
 * Response is a set of SVIDs of all all supported SVIDs with all zero's to
 * mark the end of SVIDs.  If more than 12 SVIDs are supported command SHOULD be
 * repeated.
 */
#define VDO_SVID(svid0, svid1) (((svid0) & 0xffff) << 16 | ((svid1) & 0xffff))
#define PD_VDO_SVID_SVID0(vdo) ((vdo) >> 16)
#define PD_VDO_SVID_SVID1(vdo) ((vdo) & 0xffff)

/*
 * Google modes capabilities
 * <31:8> : reserved
 * <7:0>  : mode
 */
#define VDO_MODE_GOOGLE(mode) (mode & 0xff)

#define MODE_GOOGLE_FU 1 /* Firmware Update mode */

/*
 * Mode Capabilities
 *
 * Number of VDOs supplied is SID dependent (but <= 6 VDOS?)
 */
#define VDO_MODE_CNT_DISPLAYPORT 1

/*
 * DisplayPort modes capabilities
 * -------------------------------
 * <31:24> : SBZ
 * <23:16> : UFP_D pin assignment supported
 * <15:8>  : DFP_D pin assignment supported
 * <7>     : USB 2.0 signaling (0b=yes, 1b=no)
 * <6>     : Plug | Receptacle (0b == plug, 1b == receptacle)
 * <5:2>   : xxx1: Supports DPv1.3, xx1x Supports USB Gen 2 signaling
 *           Other bits are reserved.
 * <1:0>   : signal direction ( 00b=rsv, 01b=sink, 10b=src 11b=both )
 */
#define VDO_MODE_DP(snkp, srcp, usb, gdr, sign, sdir)			\
	(((snkp) & 0xff) << 16 | ((srcp) & 0xff) << 8			\
	 | ((usb) & 1) << 7 | ((gdr) & 1) << 6 | ((sign) & 0xF) << 2	\
	 | ((sdir) & 0x3))
#define PD_DP_PIN_CAPS(x) ((((x) >> 6) & 0x1) ? (((x) >> 16) & 0x3f)	\
			   : (((x) >> 8) & 0x3f))

#define MODE_DP_PIN_A 0x01
#define MODE_DP_PIN_B 0x02
#define MODE_DP_PIN_C 0x04
#define MODE_DP_PIN_D 0x08
#define MODE_DP_PIN_E 0x10
#define MODE_DP_PIN_F 0x20

/* Pin configs B/D/F support multi-function */
#define MODE_DP_PIN_MF_MASK 0x2a
/* Pin configs A/B support BR2 signaling levels */
#define MODE_DP_PIN_BR2_MASK 0x3
/* Pin configs C/D/E/F support DP signaling levels */
#define MODE_DP_PIN_DP_MASK 0x3c

#define MODE_DP_V13  0x1
#define MODE_DP_GEN2 0x2

#define MODE_DP_SNK  0x1
#define MODE_DP_SRC  0x2
#define MODE_DP_BOTH 0x3

/*
 * DisplayPort Status VDO
 * ----------------------
 * <31:9> : SBZ
 * <8>    : IRQ_HPD : 1 == irq arrived since last message otherwise 0.
 * <7>    : HPD state : 0 = HPD_LOW, 1 == HPD_HIGH
 * <6>    : Exit DP Alt mode: 0 == maintain, 1 == exit
 * <5>    : USB config : 0 == maintain current, 1 == switch to USB from DP
 * <4>    : Multi-function preference : 0 == no pref, 1 == MF preferred.
 * <3>    : enabled : is DPout on/off.
 * <2>    : power low : 0 == normal or LPM disabled, 1 == DP disabled for LPM
 * <1:0>  : connect status : 00b ==  no (DFP|UFP)_D is connected or disabled.
 *          01b == DFP_D connected, 10b == UFP_D connected, 11b == both.
 */
#define VDO_DP_STATUS(irq, lvl, amode, usbc, mf, en, lp, conn)		\
	(((irq) & 1) << 8 | ((lvl) & 1) << 7 | ((amode) & 1) << 6	\
	 | ((usbc) & 1) << 5 | ((mf) & 1) << 4 | ((en) & 1) << 3	\
	 | ((lp) & 1) << 2 | ((conn & 0x3) << 0))

#define PD_VDO_DPSTS_HPD_IRQ(x) (((x) >> 8) & 1)
#define PD_VDO_DPSTS_HPD_LVL(x) (((x) >> 7) & 1)
#define PD_VDO_DPSTS_MF_PREF(x) (((x) >> 4) & 1)

/* Per DisplayPort Spec v1.3 Section 3.3 */
#define HPD_USTREAM_DEBOUNCE_LVL (2*MSEC)
#define HPD_USTREAM_DEBOUNCE_IRQ (250)
#define HPD_DSTREAM_DEBOUNCE_IRQ (500)  /* between 500-1000us */

/*
 * DisplayPort Configure VDO
 * -------------------------
 * <31:24> : SBZ
 * <23:16> : SBZ
 * <15:8>  : Pin assignment requested.  Choose one from mode caps.
 * <7:6>   : SBZ
 * <5:2>   : signalling : 1h == DP v1.3, 2h == Gen 2
 *           Oh is only for USB, remaining values are reserved
 * <1:0>   : cfg : 00 == USB, 01 == DFP_D, 10 == UFP_D, 11 == reserved
 */
#define VDO_DP_CFG(pin, sig, cfg) \
	(((pin) & 0xff) << 8 | ((sig) & 0xf) << 2 | ((cfg) & 0x3))

#define PD_DP_CFG_DPON(x) (((x & 0x3) == 1) || ((x & 0x3) == 2))
/*
 * Get the pin assignment mask
 * for backward compatibility, if it is null,
 * get the former sink pin assignment we used to be in <23:16>.
 */
#define PD_DP_CFG_PIN(x) ((((x) >> 8) & 0xff) ? (((x) >> 8) & 0xff) \
					      : (((x) >> 16) & 0xff))
/*
 * ChromeOS specific PD device Hardware IDs. Used to identify unique
 * products and used in VDO_INFO. Note this field is 10 bits.
 */
#define USB_PD_HW_DEV_ID_RESERVED    0
#define USB_PD_HW_DEV_ID_ZINGER      1
#define USB_PD_HW_DEV_ID_MINIMUFFIN  2
#define USB_PD_HW_DEV_ID_DINGDONG    3
#define USB_PD_HW_DEV_ID_HOHO        4
#define USB_PD_HW_DEV_ID_HONEYBUNS   5

/*
 * ChromeOS specific VDO_CMD_READ_INFO responds with device info including:
 * RW Hash: First 20 bytes of SHA-256 of RW (20 bytes)
 * HW Device ID: unique descriptor for each ChromeOS model (2 bytes)
 *               top 6 bits are minor revision, bottom 10 bits are major
 * SW Debug Version: Software version useful for debugging (15 bits)
 * IS RW: True if currently in RW, False otherwise (1 bit)
 */
#define VDO_INFO(id, id_minor, ver, is_rw) ((id_minor) << 26 \
				  | ((id) & 0x3ff) << 16 \
				  | ((ver) & 0x7fff) << 1 \
				  | ((is_rw) & 1))
#define VDO_INFO_HW_DEV_ID(x)    ((x) >> 16)
#define VDO_INFO_SW_DBG_VER(x)   (((x) >> 1) & 0x7fff)
#define VDO_INFO_IS_RW(x)        ((x) & 1)

#define HW_DEV_ID_MAJ(x) (x & 0x3ff)
#define HW_DEV_ID_MIN(x) ((x) >> 10)

/* USB-IF SIDs */
#define USB_SID_PD          0xff00 /* power delivery */
#define USB_SID_DISPLAYPORT 0xff01

#define USB_GOOGLE_TYPEC_URL "http://www.google.com/chrome/devices/typec"
/* USB Vendor ID assigned to Google Inc. */
#define USB_VID_GOOGLE 0x18d1

/* Other Vendor IDs */
#define USB_VID_APPLE  0x05ac

/* Timeout for message receive in microseconds */
#define USB_PD_RX_TMOUT_US 1800

/* --- Protocol layer functions --- */

enum pd_states {
	PD_STATE_DISABLED,
	PD_STATE_SUSPENDED,
#ifdef CONFIG_USB_PD_DUAL_ROLE
	PD_STATE_SNK_DISCONNECTED,
	PD_STATE_SNK_DISCONNECTED_DEBOUNCE,
	PD_STATE_SNK_HARD_RESET_RECOVER,
	PD_STATE_SNK_DISCOVERY,
	PD_STATE_SNK_REQUESTED,
	PD_STATE_SNK_TRANSITION,
	PD_STATE_SNK_READY,

	PD_STATE_SNK_SWAP_INIT,
	PD_STATE_SNK_SWAP_SNK_DISABLE,
	PD_STATE_SNK_SWAP_SRC_DISABLE,
	PD_STATE_SNK_SWAP_STANDBY,
	PD_STATE_SNK_SWAP_COMPLETE,
#endif /* CONFIG_USB_PD_DUAL_ROLE */

	PD_STATE_SRC_DISCONNECTED,
	PD_STATE_SRC_DISCONNECTED_DEBOUNCE,
	PD_STATE_SRC_HARD_RESET_RECOVER,
	PD_STATE_SRC_STARTUP,
	PD_STATE_SRC_DISCOVERY,
	PD_STATE_SRC_NEGOCIATE,
	PD_STATE_SRC_ACCEPTED,
	PD_STATE_SRC_POWERED,
	PD_STATE_SRC_TRANSITION,
	PD_STATE_SRC_READY,
	PD_STATE_SRC_GET_SINK_CAP,
	PD_STATE_DR_SWAP,

#ifdef CONFIG_USB_PD_DUAL_ROLE
	PD_STATE_SRC_SWAP_INIT,
	PD_STATE_SRC_SWAP_SNK_DISABLE,
	PD_STATE_SRC_SWAP_SRC_DISABLE,
	PD_STATE_SRC_SWAP_STANDBY,

#ifdef CONFIG_USBC_VCONN_SWAP
	PD_STATE_VCONN_SWAP_SEND,
	PD_STATE_VCONN_SWAP_INIT,
	PD_STATE_VCONN_SWAP_READY,
#endif /* CONFIG_USBC_VCONN_SWAP */
#endif /* CONFIG_USB_PD_DUAL_ROLE */

	PD_STATE_SOFT_RESET,
	PD_STATE_HARD_RESET_SEND,
	PD_STATE_HARD_RESET_EXECUTE,
#ifdef CONFIG_COMMON_RUNTIME
	PD_STATE_BIST_RX,
	PD_STATE_BIST_TX,
#endif

#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
	PD_STATE_DRP_AUTO_TOGGLE,
#endif
	/* Number of states. Not an actual state. */
	PD_STATE_COUNT,
};

#define PD_FLAGS_PING_ENABLED      (1 << 0) /* SRC_READY pings enabled */
#define PD_FLAGS_PARTNER_DR_POWER  (1 << 1) /* port partner is dualrole power */
#define PD_FLAGS_PARTNER_DR_DATA   (1 << 2) /* port partner is dualrole data */
#define PD_FLAGS_CHECK_IDENTITY    (1 << 3) /* discover identity in READY */
#define PD_FLAGS_SNK_CAP_RECVD     (1 << 4) /* sink capabilities received */
#define PD_FLAGS_TCPC_DRP_TOGGLE   (1 << 5) /* TCPC-controlled DRP toggling */
#define PD_FLAGS_EXPLICIT_CONTRACT (1 << 6) /* explicit pwr contract in place */
#define PD_FLAGS_VBUS_NEVER_LOW    (1 << 7) /* VBUS input has never been low */
#define PD_FLAGS_PREVIOUS_PD_CONN  (1 << 8) /* previously PD connected */
#define PD_FLAGS_CHECK_PR_ROLE     (1 << 9) /* check power role in READY */
#define PD_FLAGS_CHECK_DR_ROLE     (1 << 10)/* check data role in READY */
#define PD_FLAGS_PARTNER_EXTPOWER  (1 << 11)/* port partner has external pwr */
#define PD_FLAGS_VCONN_ON          (1 << 12)/* vconn is being sourced */
#define PD_FLAGS_TRY_SRC           (1 << 13)/* Try.SRC states are active */
#define PD_FLAGS_PARTNER_USB_COMM  (1 << 14)/* port partner is USB comms */
#define PD_FLAGS_UPDATE_SRC_CAPS   (1 << 15)/* send new source capabilities */
#define PD_FLAGS_TS_DTS_PARTNER    (1 << 16)/* partner has rp/rp or rd/rd */
/* Flags to clear on a disconnect */
#define PD_FLAGS_RESET_ON_DISCONNECT_MASK (PD_FLAGS_PARTNER_DR_POWER | \
					   PD_FLAGS_PARTNER_DR_DATA | \
					   PD_FLAGS_CHECK_IDENTITY | \
					   PD_FLAGS_SNK_CAP_RECVD | \
					   PD_FLAGS_TCPC_DRP_TOGGLE | \
					   PD_FLAGS_EXPLICIT_CONTRACT | \
					   PD_FLAGS_PREVIOUS_PD_CONN | \
					   PD_FLAGS_CHECK_PR_ROLE | \
					   PD_FLAGS_CHECK_DR_ROLE | \
					   PD_FLAGS_PARTNER_EXTPOWER | \
					   PD_FLAGS_VCONN_ON | \
					   PD_FLAGS_TRY_SRC | \
					   PD_FLAGS_PARTNER_USB_COMM | \
					   PD_FLAGS_UPDATE_SRC_CAPS | \
					   PD_FLAGS_TS_DTS_PARTNER)

enum pd_cc_states {
	PD_CC_NONE,

	/* From DFP perspective */
	PD_CC_NO_UFP,
	PD_CC_AUDIO_ACC,
	PD_CC_DEBUG_ACC,
	PD_CC_UFP_ATTACHED,

	/* From UFP perspective */
	PD_CC_DFP_ATTACHED
};

#ifdef CONFIG_USB_PD_DUAL_ROLE
enum pd_dual_role_states {
	/* While disconnected, toggle between src and sink */
	PD_DRP_TOGGLE_ON,
	/* Stay in src until disconnect, then stay in sink forever */
	PD_DRP_TOGGLE_OFF,
	/* Stay in current power role, don't switch. No auto-toggle support */
	PD_DRP_FREEZE,
	/* Switch to sink */
	PD_DRP_FORCE_SINK,
	/* Switch to source */
	PD_DRP_FORCE_SOURCE,
};
/**
 * Get dual role state
 *
 * @return Current dual-role state, from enum pd_dual_role_states
 */
enum pd_dual_role_states pd_get_dual_role(void);
/**
 * Set dual role state, from among enum pd_dual_role_states
 *
 * @param state New state of dual-role port, selected from
 *              enum pd_dual_role_states
 */
void pd_set_dual_role(enum pd_dual_role_states state);

/**
 * Get role, from among PD_ROLE_SINK and PD_ROLE_SOURCE
 *
 * @param port Port number from which to get role
 */
int pd_get_role(int port);

#endif

/* Control Message type */
enum pd_ctrl_msg_type {
	/* 0 Reserved */
	PD_CTRL_GOOD_CRC = 1,
	PD_CTRL_GOTO_MIN = 2,
	PD_CTRL_ACCEPT = 3,
	PD_CTRL_REJECT = 4,
	PD_CTRL_PING = 5,
	PD_CTRL_PS_RDY = 6,
	PD_CTRL_GET_SOURCE_CAP = 7,
	PD_CTRL_GET_SINK_CAP = 8,
	PD_CTRL_DR_SWAP = 9,
	PD_CTRL_PR_SWAP = 10,
	PD_CTRL_VCONN_SWAP = 11,
	PD_CTRL_WAIT = 12,
	PD_CTRL_SOFT_RESET = 13,
	/* 14-15 Reserved */

	/* Used for REV 3.0 */
	PD_CTRL_NOT_SUPPORTED = 16,
	PD_CTRL_GET_SOURCE_CAP_EXT = 17,
	PD_CTRL_GET_STATUS = 18,
	PD_CTRL_FR_SWAP = 19,
	PD_CTRL_GET_PPS_STATUS = 20,
	PD_CTRL_GET_COUNTRY_CODES = 21,
	/* 22-31 Reserved */
};

/* Battery Status Data Object fields for REV 3.0 */
#define BSDO_CAP_UNKNOWN 0xffff
#define BSDO_CAP(n)      (((n) & 0xffff) << 16)
#define BSDO_INVALID     (1 << 8)
#define BSDO_PRESENT     (1 << 9)
#define BSDO_DISCHARGING (1 << 10)
#define BSDO_IDLE        (1 << 11)

/* Get Battery Cap Message fields for REV 3.0 */
#define BATT_CAP_REF(n)  (((n) >> 16) & 0xff)

/* Extended message type for REV 3.0 */
enum pd_ext_msg_type {
	/* 0 Reserved */
	PD_EXT_SOURCE_CAP = 1,
	PD_EXT_STATUS = 2,
	PD_EXT_GET_BATTERY_CAP = 3,
	PD_EXT_GET_BATTERY_STATUS = 4,
	PD_EXT_BATTERY_CAP = 5,
	PD_EXT_GET_MANUFACTURER_INFO = 6,
	PD_EXT_MANUFACTURER_INFO = 7,
	PD_EXT_SECURITY_REQUEST = 8,
	PD_EXT_SECURITY_RESPONSE = 9,
	PD_EXT_FIRMWARE_UPDATE_REQUEST = 10,
	PD_EXT_FIRMWARE_UPDATE_RESPONSE = 11,
	PD_EXT_PPS_STATUS = 12,
	PD_EXT_COUNTRY_INFO = 13,
	PD_EXT_COUNTRY_CODES = 14,
	/* 15-31 Reserved */
};

/* Data message type */
enum pd_data_msg_type {
	/* 0 Reserved */
	PD_DATA_SOURCE_CAP = 1,
	PD_DATA_REQUEST = 2,
	PD_DATA_BIST = 3,
	PD_DATA_SINK_CAP = 4,
	/* 5-14 Reserved for REV 2.0 */
	PD_DATA_BATTERY_STATUS = 5,
	PD_DATA_ALERT = 6,
	PD_DATA_GET_COUNTRY_INFO = 7,
	/* 8-14 Reserved for REV 3.0 */
	PD_DATA_VENDOR_DEF = 15,
};

/* Protocol revision */
#define PD_REV10 0
#define PD_REV20 1
#define PD_REV30 2

/* Power role */
#define PD_ROLE_SINK   0
#define PD_ROLE_SOURCE 1
/* Data role */
#define PD_ROLE_UFP    0
#define PD_ROLE_DFP    1
/* Vconn role */
#define PD_ROLE_VCONN_OFF 0
#define PD_ROLE_VCONN_ON  1

/* chunk is a request or response in REV 3.0 */
#define CHUNK_RESPONSE 0
#define CHUNK_REQUEST  1

/* collision avoidance Rp values in REV 3.0 */
#define SINK_TX_OK TYPEC_RP_3A0
#define SINK_TX_NG TYPEC_RP_1A5

/* Port role at startup */
#ifndef PD_ROLE_DEFAULT
#ifdef CONFIG_USB_PD_DUAL_ROLE
#define PD_ROLE_DEFAULT(port) PD_ROLE_SINK
#else
#define PD_ROLE_DEFAULT(port) PD_ROLE_SOURCE
#endif
#endif

/* Port default state at startup */
#ifdef CONFIG_USB_PD_DUAL_ROLE
#define PD_DEFAULT_STATE(port) ((PD_ROLE_DEFAULT(port) == PD_ROLE_SOURCE) ? \
				PD_STATE_SRC_DISCONNECTED :	      \
				PD_STATE_SNK_DISCONNECTED)
#else
#define PD_DEFAULT_STATE(port) PD_STATE_SRC_DISCONNECTED
#endif

/* build extended message header */
/* All extended messages are chunked, so set bit 15 */
#define PD_EXT_HEADER(cnum, rchk, dsize) \
	 ((1 << 15) | ((cnum) << 11) | \
	 ((rchk) << 10) | (dsize))

/* build message header */
#define PD_HEADER(type, prole, drole, id, cnt, rev, ext) \
	((type) | ((rev) << 6) | \
	((drole) << 5) | ((prole) << 8) | \
	((id) << 9) | ((cnt) << 12) | ((ext) << 15))

/* Used for processing pd header */
#define PD_HEADER_EXT(header)  (((header) >> 15) & 1)
#define PD_HEADER_CNT(header)  (((header) >> 12) & 7)
#define PD_HEADER_TYPE(header) ((header) & 0xF)
#define PD_HEADER_ID(header)   (((header) >> 9) & 7)
#define PD_HEADER_REV(header)  (((header) >> 6) & 3)

/* Used for processing pd extended header */
#define PD_EXT_HEADER_CHUNKED(header)   (((header) >> 15) & 1)
#define PD_EXT_HEADER_CHUNK_NUM(header) (((header) >> 11) & 0xf)
#define PD_EXT_HEADER_REQ_CHUNK(header) (((header) >> 10) & 1)
#define PD_EXT_HEADER_DATA_SIZE(header) ((header) & 0x1ff)

/* K-codes for special symbols */
#define PD_SYNC1 0x18
#define PD_SYNC2 0x11
#define PD_SYNC3 0x06
#define PD_RST1  0x07
#define PD_RST2  0x19
#define PD_EOP   0x0D

/* Minimum PD supply current  (mA) */
#define PD_MIN_MA	500

/* Minimum PD voltage (mV) */
#define PD_MIN_MV	5000

/* No connect voltage threshold for sources based on Rp */
#define PD_SRC_DEF_VNC_MV        1600
#define PD_SRC_1_5_VNC_MV        1600
#define PD_SRC_3_0_VNC_MV        2600

/* Rd voltage threshold for sources based on Rp */
#define PD_SRC_DEF_RD_THRESH_MV  200
#define PD_SRC_1_5_RD_THRESH_MV  400
#define PD_SRC_3_0_RD_THRESH_MV  800

/* Voltage threshold to detect connection when presenting Rd */
#define PD_SNK_VA_MV             250

/* --- Policy layer functions --- */

/* Request types for pd_build_request() */
enum pd_request_type {
	PD_REQUEST_VSAFE5V,
	PD_REQUEST_MAX,
};

#ifdef CONFIG_USB_PD_REV30
/**
 * Get current PD Revision
 *
 * @param port USB-C port number
 * @return 0 for PD_REV1.0, 1 for PD_REV2.0, 2 for PD_REV3.0
 */
int pd_get_rev(int port);

/**
 * Get current PD VDO Version
 *
 * @param port USB-C port number
 * @return 0 for PD_REV1.0, 1 for PD_REV2.0
 */
int pd_get_vdo_ver(int port);
#else
#define pd_get_rev(n)     PD_REV20
#define pd_get_vdo_ver(n) VDM_VER10
#endif
/**
 * Decide which PDO to choose from the source capabilities.
 *
 * @param port USB-C port number
 * @param rdo  requested Request Data Object.
 * @param ma  selected current limit (stored on success)
 * @param mv  selected supply voltage (stored on success)
 * @param req_type request type
 * @return <0 if invalid, else EC_SUCCESS
 */
int pd_build_request(int port, uint32_t *rdo, uint32_t *ma, uint32_t *mv,
		     enum pd_request_type req_type);

/**
 * Check if max voltage request is allowed (only used if
 * CONFIG_USB_PD_CHECK_MAX_REQUEST_ALLOWED is defined).
 *
 * @return True if max voltage request allowed, False otherwise
 */
int pd_is_max_request_allowed(void);

/**
 * Callback with source capabilities packet
 *
 * @param port USB-C port number
 * @param cnt  the number of Power Data Objects.
 * @param src_caps Power Data Objects representing the source capabilities.
 */
void pd_process_source_cap_callback(int port, int cnt, uint32_t *src_caps);

/**
 * Process source capabilities packet
 *
 * @param port USB-C port number
 * @param cnt  the number of Power Data Objects.
 * @param src_caps Power Data Objects representing the source capabilities.
 */
void pd_process_source_cap(int port, int cnt, uint32_t *src_caps);

/**
 * Find PDO index that offers the most amount of power and stays within
 * max_mv voltage.
 *
 * @param port USB-C port number
 * @param max_mv maximum voltage (or -1 if no limit)
 * @param pdo raw pdo corresponding to index, or index 0 on error (output)
 * @return index of PDO within source cap packet
 */
int pd_find_pdo_index(int port, int max_mv, uint32_t *pdo);

/**
 * Extract power information out of a Power Data Object (PDO)
 *
 * @param pdo raw pdo to extract
 * @param ma current of the PDO (output)
 * @param mv voltage of the PDO (output)
 */
void pd_extract_pdo_power(uint32_t pdo, uint32_t *ma, uint32_t *mv);

/**
 * Reduce the sink power consumption to a minimum value.
 *
 * @param port USB-C port number
 * @param ma reduce current to minimum value.
 * @param mv reduce voltage to minimum value.
 */
void pd_snk_give_back(int port, uint32_t * const ma, uint32_t * const mv);

/**
 * Put a cap on the max voltage requested as a sink.
 * @param mv maximum voltage in millivolts.
 */
void pd_set_max_voltage(unsigned mv);

/**
 * Get the max voltage that can be requested as set by pd_set_max_voltage().
 * @return max voltage
 */
unsigned pd_get_max_voltage(void);

/**
 * Check if this board supports the given input voltage.
 *
 * @mv input voltage
 * @return 1 if voltage supported, 0 if not
 */
int pd_is_valid_input_voltage(int mv);

/**
 * Request a new operating voltage.
 *
 * @param rdo  Request Data Object with the selected operating point.
 * @param port The port which the request came in on.
 * @return EC_SUCCESS if we can get the requested voltage/OP, <0 else.
 */
int pd_check_requested_voltage(uint32_t rdo, const int port);

/**
 * Run board specific checks on request message
 *
 * @param rdo the request data object word sent by the sink.
 * @param pdo_cnt the total number of source PDOs.
 * @return EC_SUCCESS if request is ok , <0 else.
 */
int pd_board_check_request(uint32_t rdo, int pdo_cnt);

/**
 * Select a new output voltage.
 *
 * param idx index of the new voltage in the source PDO table.
 */
void pd_transition_voltage(int idx);

/**
 * Go back to the default/safe state of the power supply
 *
 * @param port USB-C port number
 */
void pd_power_supply_reset(int port);

/**
 * Enable or disable VBUS discharge for a given port.
 *
 * @param port USB-C port number
 * @enable 1 if enabling discharge, 0 if disabling
 */
void pd_set_vbus_discharge(int port, int enable);

/**
 * Enable the power supply output after the ready delay.
 *
 * @param port USB-C port number
 * @return EC_SUCCESS if the power supply is ready, <0 else.
 */
int pd_set_power_supply_ready(int port);

/**
 * Ask the specified voltage from the PD source.
 *
 * It triggers a new negotiation sequence with the source.
 * @param port USB-C port number
 * @param mv request voltage in millivolts.
 */
void pd_request_source_voltage(int port, int mv);

/**
 * Set a voltage limit from the PD source.
 *
 * If the source is currently active, it triggers a new negotiation.
 * @param port USB-C port number
 * @param mv limit voltage in millivolts.
 */
void pd_set_external_voltage_limit(int port, int mv);

/**
 * Set the PD input current limit.
 *
 * @param port USB-C port number
 * @param max_ma Maximum current limit
 * @param supply_voltage Voltage at which current limit is applied
 */
void pd_set_input_current_limit(int port, uint32_t max_ma,
				uint32_t supply_voltage);


/**
 * Update the power contract if it exists.
 *
 * @param port USB-C port number.
 */
void pd_update_contract(int port);

/* Encode DTS status of port partner in current limit parameter */
typedef uint32_t typec_current_t;
#define TYPEC_CURRENT_DTS_MASK (1 << 31)
#define TYPEC_CURRENT_ILIM_MASK (~TYPEC_CURRENT_DTS_MASK)

/**
 * Set the type-C input current limit.
 *
 * @param port USB-C port number
 * @param max_ma Maximum current limit
 * @param supply_voltage Voltage at which current limit is applied
 */
void typec_set_input_current_limit(int port, typec_current_t max_ma,
				   uint32_t supply_voltage);

/**
 * Set the type-C current limit when sourcing current..
 *
 * @param port USB-C port number
 * @param rp One of enum tcpc_rp_value (eg TYPEC_RP_3A0) defining the limit.
 */
void typec_set_source_current_limit(int port, int rp);

/**
 * Verify board specific health status : current, voltages...
 *
 * @return EC_SUCCESS if the board is good, <0 else.
 */
int pd_board_checks(void);

/**
 * Return if VBUS is detected on type-C port
 *
 * @param port USB-C port number
 * @return VBUS is detected
 */
int pd_snk_is_vbus_provided(int port);

/**
 * Notify PD protocol that VBUS has gone low
 *
 * @param port USB-C port number
 */
void pd_vbus_low(int port);

/**
 * Check if power swap is allowed.
 *
 * @param port USB-C port number
 * @return True if power swap is allowed, False otherwise
 */
int pd_check_power_swap(int port);

/**
 * Check if data swap is allowed.
 *
 * @param port USB-C port number
 * @param data_role current data role
 * @return True if data swap is allowed, False otherwise
 */
int pd_check_data_swap(int port, int data_role);

/**
 * Check if vconn swap is allowed.
 *
 * @param port USB-C port number
 * @return True if vconn swap is allowed, False otherwise
 */

int pd_check_vconn_swap(int port);

/**
 * Check current power role for potential power swap
 *
 * @param port USB-C port number
 * @param pr_role Our power role
 * @param flags PD flags
 */
void pd_check_pr_role(int port, int pr_role, int flags);

/**
 * Check current data role for potential data swap
 *
 * @param port USB-C port number
 * @param dr_role Our data role
 * @param flags PD flags
 */
void pd_check_dr_role(int port, int dr_role, int flags);

/**
 * Check if we should charge from this device. This is
 * basically a white-list for chargers that are dual-role,
 * don't set the externally powered bit, but we should charge
 * from by default.
 *
 * @param vid Port partner Vendor ID
 * @param pid Port partner Product ID
 */
int pd_charge_from_device(uint16_t vid, uint16_t pid);

/**
 * Execute data swap.
 *
 * @param port USB-C port number
 * @param data_role new data role
 */
void pd_execute_data_swap(int port, int data_role);

/**
 * Get PD device info used for VDO_CMD_SEND_INFO / VDO_CMD_READ_INFO
 *
 * @param info_data pointer to info data array
 */
void pd_get_info(uint32_t *info_data);

/**
 * Handle Vendor Defined Messages
 *
 * @param port     USB-C port number
 * @param cnt      number of data objects in the payload.
 * @param payload  payload data.
 * @param rpayload pointer to the data to send back.
 * @return if >0, number of VDOs to send back.
 */
int pd_custom_vdm(int port, int cnt, uint32_t *payload, uint32_t **rpayload);

/**
 * Handle Structured Vendor Defined Messages
 *
 * @param port     USB-C port number
 * @param cnt      number of data objects in the payload.
 * @param payload  payload data.
 * @param rpayload pointer to the data to send back.
 * @return if >0, number of VDOs to send back.
 */
int pd_svdm(int port, int cnt, uint32_t *payload, uint32_t **rpayload);

/**
 * Handle Custom VDMs for flashing.
 *
 * @param port     USB-C port number
 * @param cnt      number of data objects in the payload.
 * @param payload  payload data.
 * @return if >0, number of VDOs to send back.
 */
int pd_custom_flash_vdm(int port, int cnt, uint32_t *payload);

/**
 * Enter alternate mode on DFP
 *
 * @param port     USB-C port number
 * @param svid USB standard or vendor id to exit or zero for DFP amode reset.
 * @param opos object position of mode to exit.
 * @return vdm for UFP to be sent to enter mode or zero if not.
 */
uint32_t pd_dfp_enter_mode(int port, uint16_t svid, int opos);

/**
 *  Get DisplayPort pin mode for DFP to request from UFP's capabilities.
 *
 * @param port     USB-C port number.
 * @param status   DisplayPort Status VDO.
 * @return one-hot PIN config to request.
 */
int pd_dfp_dp_get_pin_mode(int port, uint32_t status);

/**
 * Exit alternate mode on DFP
 *
 * @param port USB-C port number
 * @param svid USB standard or vendor id to exit or zero for DFP amode reset.
 * @param opos object position of mode to exit.
 * @return 1 if UFP should be sent exit mode VDM.
 */
int pd_dfp_exit_mode(int port, uint16_t svid, int opos);

/**
 * Initialize policy engine for DFP
 *
 * @param port     USB-C port number
 */
void pd_dfp_pe_init(int port);

/**
 * Return the VID of the USB PD accessory connected to a specified port
 *
 * @param port  USB-C port number
 * @return      the USB Vendor Identifier or 0 if it doesn't exist
 */
uint16_t pd_get_identity_vid(int port);

/**
 * Return the PID of the USB PD accessory connected to a specified port
 *
 * @param port  USB-C port number
 * @return      the USB Product Identifier or 0 if it doesn't exist
 */
uint16_t pd_get_identity_pid(int port);

/**
 * Store Device ID & RW hash of device
 *
 * @param port			USB-C port number
 * @param dev_id		device identifier
 * @param rw_hash		pointer to rw_hash
 * @param current_image		current image: RW or RO
 * @return			true if the dev / hash match an existing hash
 *				in our table, false otherwise
 */
int pd_dev_store_rw_hash(int port, uint16_t dev_id, uint32_t *rw_hash,
			 uint32_t ec_current_image);

/**
 * Try to fetch one PD log entry from accessory
 *
 * @param port	USB-C accessory port number
 * @return	EC_RES_SUCCESS if the VDM was sent properly else error code
 */
int pd_fetch_acc_log_entry(int port);

/**
 * Analyze the log entry received as the VDO_CMD_GET_LOG payload.
 *
 * @param port		USB-C accessory port number
 * @param cnt		number of data objects in payload
 * @param payload	payload data
 */
void pd_log_recv_vdm(int port, int cnt, uint32_t *payload);

/**
 * Send Vendor Defined Message
 *
 * @param port     USB-C port number
 * @param vid      Vendor ID
 * @param cmd      VDO command number
 * @param data     Pointer to payload to send
 * @param count    number of data objects in payload
 */
void pd_send_vdm(int port, uint32_t vid, int cmd, const uint32_t *data,
		 int count);

/* Power Data Objects for the source and the sink */
extern const uint32_t pd_src_pdo[];
extern const int pd_src_pdo_cnt;
extern const uint32_t pd_src_pdo_max[];
extern const int pd_src_pdo_max_cnt;
extern const uint32_t pd_snk_pdo[];
extern const int pd_snk_pdo_cnt;

/**
 * Request that a host event be sent to notify the AP of a PD power event.
 *
 * @param mask host event mask.
 */
#if defined(HAS_TASK_HOSTCMD) && !defined(TEST_BUILD)
void pd_send_host_event(int mask);
#else
static inline void pd_send_host_event(int mask) { }
#endif

/**
 * Determine if in alternate mode or not.
 *
 * @param port port number.
 * @param svid USB standard or vendor id
 * @return object position of mode chosen in alternate mode otherwise zero.
 */
int pd_alt_mode(int port, uint16_t svid);

/**
 * Send hpd over USB PD.
 *
 * @param port port number.
 * @param hpd hotplug detect type.
 */
void pd_send_hpd(int port, enum hpd_event hpd);

/**
 * Enable USB Billboard Device.
 */
extern const struct deferred_data pd_usb_billboard_deferred_data;
/* --- Physical layer functions : chip specific --- */

/* Packet preparation/retrieval */

/**
 * Prepare packet reading state machine.
 *
 * @param port USB-C port number
 */
void pd_init_dequeue(int port);

/**
 * Prepare packet reading state machine.
 *
 * @param port USB-C port number
 * @param off  current position in the packet buffer.
 * @param len  minimum size to read in bits.
 * @param val  the read bits.
 * @return new position in the packet buffer.
 */
int pd_dequeue_bits(int port, int off, int len, uint32_t *val);

/**
 * Advance until the end of the preamble.
 *
 * @param port USB-C port number
 * @return new position in the packet buffer.
 */
int pd_find_preamble(int port);

/**
 * Write the preamble in the TX buffer.
 *
 * @param port USB-C port number
 * @return new position in the packet buffer.
 */
int pd_write_preamble(int port);

/**
 * Write one 10-period symbol in the TX packet.
 * corresponding to a quartet with 4b5b encoding
 * and Biphase Mark Coding.
 *
 * @param port USB-C port number
 * @param bit_off current position in the packet buffer.
 * @param val10    the 10-bit integer.
 * @return new position in the packet buffer.
 */
int pd_write_sym(int port, int bit_off, uint32_t val10);


/**
 * Ensure that we have an edge after EOP and we end up at level 0,
 * also fill the last byte.
 *
 * @param port USB-C port number
 * @param bit_off current position in the packet buffer.
 * @return new position in the packet buffer.
 */
int pd_write_last_edge(int port, int bit_off);

/**
 * Do 4B5B encoding on a 32-bit word.
 *
 * @param port USB-C port number
 * @param off current offset in bits inside the message
 * @param val32 32-bit word value to encode
 * @return new offset in the message in bits.
 */
int encode_word(int port, int off, uint32_t val32);

/**
 * Ensure that we have an edge after EOP and we end up at level 0,
 * also fill the last byte.
 *
 * @param port USB-C port number
 * @param header PD packet header
 * @param cnt number of payload words
 * @param data payload content
 * @return length of the message in bits.
 */
int prepare_message(int port, uint16_t header, uint8_t cnt,
		    const uint32_t *data);

/**
 * Dump the current PD packet on the console for debug.
 *
 * @param port USB-C port number
 * @param msg  context string.
 */
void pd_dump_packet(int port, const char *msg);

/**
 * Change the TX data clock frequency.
 *
 * @param port USB-C port number
 * @param freq frequency in hertz.
 */
void pd_set_clock(int port, int freq);

/* TX/RX callbacks */

/**
 * Start sending over the wire the prepared packet.
 *
 * @param port USB-C port number
 * @param polarity plug polarity (0=CC1, 1=CC2).
 * @param bit_len size of the packet in bits.
 * @return length transmitted or negative if error
 */
int pd_start_tx(int port, int polarity, int bit_len);

/**
 * Set PD TX DMA to use circular mode. Call this before pd_start_tx() to
 * continually loop over the transmit buffer given in pd_start_tx().
 *
 * @param port USB-C port number
 */
void pd_tx_set_circular_mode(int port);

/**
 * Stop PD TX DMA circular mode transaction already in progress.
 *
 * @param port USB-C port number
 */
void pd_tx_clear_circular_mode(int port);

/**
 * Call when we are done sending a packet.
 *
 * @param port USB-C port number
 * @param polarity plug polarity (0=CC1, 1=CC2).
 */
void pd_tx_done(int port, int polarity);

/**
 * Check whether the PD reception is started.
 *
 * @param port USB-C port number
 * @return true if the reception is on-going.
 */
int pd_rx_started(int port);

/**
 * Suspend the PD task.
 * @param port USB-C port number
 * @param enable pass 0 to resume, anything else to suspend
 */
void pd_set_suspend(int port, int enable);

/**
 * Check if the port has been initialized and PD task has not been
 * suspended.
 *
 * @param port USB-C port number
 * @return true if the PD task is not suspended.
 */
int pd_is_port_enabled(int port);

/* Callback when the hardware has detected an incoming packet */
void pd_rx_event(int port);
/* Start sampling the CC line for reception */
void pd_rx_start(int port);
/* Call when we are done reading a packet */
void pd_rx_complete(int port);

/* restart listening to the CC wire */
void pd_rx_enable_monitoring(int port);
/* stop listening to the CC wire during transmissions */
void pd_rx_disable_monitoring(int port);

/* get time since last RX edge interrupt */
uint64_t get_time_since_last_edge(int port);

/**
 * Deinitialize the hardware used for PD.
 *
 * @param port USB-C port number
 */
void pd_hw_release(int port);

/**
 * Initialize the hardware used for PD RX/TX.
 *
 * @param port USB-C port number
 * @param role Role to initialize pins in
 */
void pd_hw_init(int port, int role);

/**
 * Initialize the reception side of hardware used for PD.
 *
 * This is a subset of pd_hw_init() including only :
 * the comparators + the RX edge delay timer + the RX DMA.
 *
 * @param port USB-C port number
 */
void pd_hw_init_rx(int port);

/**
 * Initialize the Power Delivery state machine
 */
void pd_init(int port);

/**
 * Run the state machine. This function must be called regularly
 * to iterate through the state machine. It uses get_time() to 
 * determine what actions to take each call. 
 */
void pd_run_state_machine(int port);

/* --- Protocol layer functions --- */

/**
 * Decode a raw packet in the RX buffer.
 *
 * @param port USB-C port number
 * @param payload buffer to store the packet payload (must be 7x 32-bit)
 * @return the packet header or <0 in case of error
 */
int pd_analyze_rx(int port, uint32_t *payload);

/**
 * Check if PD communication is enabled
 *
 * @return true if it's enabled or false otherwise
 */
int pd_comm_is_enabled(int port);

/**
 * Get connected state
 *
 * @param port USB-C port number
 * @return True if port is in connected state
 */
int pd_is_connected(int port);

/**
 * Execute a hard reset
 *
 * @param port USB-C port number
 */
void pd_execute_hard_reset(int port);

/**
 * Signal to protocol layer that PD transmit is complete
 *
 * @param port USB-C port number
 * @param status status of the transmission
 */
void pd_transmit_complete(int port, int status);

/**
 * Get port polarity.
 *
 * @param port USB-C port number
 */
int pd_get_polarity(int port);

/**
 * Get port partner data swap capable status
 *
 * @param port USB-C port number
 */
int pd_get_partner_data_swap_capable(int port);

/**
 * Request power swap command to be issued
 *
 * @param port USB-C port number
 */
void pd_request_power_swap(int port);

/**
 * Try to become the VCONN source, if we are not already the source and the
 * other side is willing to accept a VCONN swap.
 *
 * @param port USB-C port number
 */
void pd_try_vconn_src(int port);

/**
 * Request data swap command to be issued
 *
 * @param port USB-C port number
 */
void pd_request_data_swap(int port);

/**
 * Set the PD communication enabled flag. When communication is disabled,
 * the port can still detect connection and source power but will not
 * send or respond to any PD communication.
 *
 * @param port USB-C port number
 * @param enable Enable flag to set
 */
void pd_comm_enable(int port, int enable);

/**
 * Set the PD pings enabled flag. When source has negotiated power over
 * PD successfully, it can optionally send pings periodically based on
 * this enable flag.
 *
 * @param port USB-C port number
 * @param enable Enable flag to set
 */
void pd_ping_enable(int port, int enable);

/* Issue PD soft reset */
void pd_soft_reset(void);

/* Prepare PD communication for reset */
void pd_prepare_reset(void);

/**
 * Signal power request to indicate a charger update that affects the port.
 *
 * @param port USB-C port number
 */
void pd_set_new_power_request(int port);

/**
 * Return true if partner port is a DTS or TS capable of entering debug
 * mode (eg. is presenting Rp/Rp or Rd/Rd).
 *
 * @param port USB-C port number
 */
int pd_ts_dts_plugged(int port);

/* ----- Logging ----- */
#ifdef CONFIG_USB_PD_LOGGING
/**
 * Record one event in the PD logging FIFO.
 *
 * @param type event type as defined by PD_EVENT_xx in ec_commands.h
 * @param size_port payload size and port num (defined by PD_LOG_PORT_SIZE)
 * @param data type-defined information
 * @param payload pointer to the optional payload (0..16 bytes)
 */
void pd_log_event(uint8_t type, uint8_t size_port,
		  uint16_t data, void *payload);

/**
 * Retrieve one logged event and prepare a VDM with it.
 *
 * Used to answer the VDO_CMD_GET_LOG unstructured VDM.
 *
 * @param payload pointer to the payload data buffer (must be 7 words)
 * @return number of 32-bit words in the VDM payload.
 */
int pd_vdm_get_log_entry(uint32_t *payload);
#else  /* CONFIG_USB_PD_LOGGING */
static inline void pd_log_event(uint8_t type, uint8_t size_port,
				uint16_t data, void *payload) {}
static inline int pd_vdm_get_log_entry(uint32_t *payload) { return 0; }
#endif /* CONFIG_USB_PD_LOGGING */

#endif  /* __CROS_EC_USB_PD_H */
