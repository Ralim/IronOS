/*
 * PD Buddy Firmware Library - USB Power Delivery for everyone
 * Copyright 2017-2018 Clayton G. Hobbs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PDB_PD_H
#define PDB_PD_H

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "pdb_conf.h"
#include "pdb_msg.h"
#include <stdint.h>
/*
 * Macros for working with USB Power Delivery messages.
 *
 * This file is mostly written from the PD Rev. 2.0 spec, but the header is
 * written from the Rev. 3.0 spec.
 */

/*
 * PD Header
 */
#define PD_HDR_MSGTYPE_SHIFT   0
#define PD_HDR_MSGTYPE         (0x1F << PD_HDR_MSGTYPE_SHIFT)
#define PD_HDR_DATAROLE_SHIFT  5
#define PD_HDR_DATAROLE        (0x1 << PD_HDR_DATAROLE_SHIFT)
#define PD_HDR_SPECREV_SHIFT   6
#define PD_HDR_SPECREV         (0x3 << PD_HDR_SPECREV_SHIFT)
#define PD_HDR_POWERROLE_SHIFT 8
#define PD_HDR_POWERROLE       (1 << PD_HDR_POWERROLE_SHIFT)
#define PD_HDR_MESSAGEID_SHIFT 9
#define PD_HDR_MESSAGEID       (0x7 << PD_HDR_MESSAGEID_SHIFT)
#define PD_HDR_NUMOBJ_SHIFT    12
#define PD_HDR_NUMOBJ          (0x7 << PD_HDR_NUMOBJ_SHIFT)
#define PD_HDR_EXT             (1 << 15)

/* Message types */
#define PD_MSGTYPE_GET(msg) (((msg)->hdr & PD_HDR_MSGTYPE) >> PD_HDR_MSGTYPE_SHIFT)
/* Control Message */
#define PD_MSGTYPE_GOODCRC                 0x01
#define PD_MSGTYPE_GOTOMIN                 0x02
#define PD_MSGTYPE_ACCEPT                  0x03
#define PD_MSGTYPE_REJECT                  0x04
#define PD_MSGTYPE_PING                    0x05
#define PD_MSGTYPE_PS_RDY                  0x06
#define PD_MSGTYPE_GET_SOURCE_CAP          0x07
#define PD_MSGTYPE_GET_SINK_CAP            0x08
#define PD_MSGTYPE_DR_SWAP                 0x09
#define PD_MSGTYPE_PR_SWAP                 0x0A
#define PD_MSGTYPE_VCONN_SWAP              0x0B
#define PD_MSGTYPE_WAIT                    0x0C
#define PD_MSGTYPE_SOFT_RESET              0x0D
#define PD_MSGTYPE_NOT_SUPPORTED           0x10
#define PD_MSGTYPE_GET_SOURCE_CAP_EXTENDED 0x11
#define PD_MSGTYPE_GET_STATUS              0x12
#define PD_MSGTYPE_FR_SWAP                 0x13
#define PD_MSGTYPE_GET_PPS_STATUS          0x14
#define PD_MSGTYPE_GET_COUNTRY_CODES       0x15
/* Data Message */
#define PD_MSGTYPE_SOURCE_CAPABILITIES 0x01
#define PD_MSGTYPE_REQUEST             0x02
#define PD_MSGTYPE_BIST                0x03
#define PD_MSGTYPE_SINK_CAPABILITIES   0x04
#define PD_MSGTYPE_BATTERY_STATUS      0x05
#define PD_MSGTYPE_ALERT               0x06
#define PD_MSGTYPE_GET_COUNTRY_INFO    0x07
#define PD_MSGTYPE_VENDOR_DEFINED      0x0F
/* Extended Message */
#define PD_MSGTYPE_SOURCE_CAPABILITIES_EXTENDED 0x01
#define PD_MSGTYPE_STATUS                       0x02
#define PD_MSGTYPE_GET_BATTERY_CAP              0x03
#define PD_MSGTYPE_GET_BATTERY_STATUS           0x04
#define PD_MSGTYPE_BATTERY_CAPABILITIES         0x05
#define PD_MSGTYPE_GET_MANUFACTURER_INFO        0x06
#define PD_MSGTYPE_MANUFACTURER_INFO            0x07
#define PD_MSGTYPE_SECURITY_REQUEST             0x08
#define PD_MSGTYPE_SECURITY_RESPONSE            0x09
#define PD_MSGTYPE_FIRMWARE_UPDATE_REQUEST      0x0A
#define PD_MSGTYPE_FIRMWARE_UPDATE_RESPONSE     0x0B
#define PD_MSGTYPE_PPS_STATUS                   0x0C
#define PD_MSGTYPE_COUNTRY_INFO                 0x0D
#define PD_MSGTYPE_COUNTRY_CODES                0x0E

/* Data roles */
#define PD_DATAROLE_UFP (0x0 << PD_HDR_DATAROLE_SHIFT)
#define PD_DATAROLE_DFP (0x1 << PD_HDR_DATAROLE_SHIFT)

/* Specification revisions */
#define PD_SPECREV_1_0 (0x0 << PD_HDR_SPECREV_SHIFT)
#define PD_SPECREV_2_0 (0x1 << PD_HDR_SPECREV_SHIFT)
#define PD_SPECREV_3_0 (0x2 << PD_HDR_SPECREV_SHIFT)

/* Port power roles */
#define PD_POWERROLE_SINK   (0x0 << PD_HDR_POWERROLE_SHIFT)
#define PD_POWERROLE_SOURCE (0x1 << PD_HDR_POWERROLE_SHIFT)

/* Message ID */
#define PD_MESSAGEID_GET(msg) (((msg)->hdr & PD_HDR_MESSAGEID) >> PD_HDR_MESSAGEID_SHIFT)

/* Number of data objects */
#define PD_NUMOBJ(n)       (((n) << PD_HDR_NUMOBJ_SHIFT) & PD_HDR_NUMOBJ)
#define PD_NUMOBJ_GET(msg) (((msg)->hdr & PD_HDR_NUMOBJ) >> PD_HDR_NUMOBJ_SHIFT)

/*
 * PD Extended Message Header
 */
#define PD_EXTHDR_DATA_SIZE_SHIFT     0
#define PD_EXTHDR_DATA_SIZE           (0x1FF << PD_EXTHDR_DATA_SIZE_SHIFT)
#define PD_EXTHDR_REQUEST_CHUNK_SHIFT 10
#define PD_EXTHDR_REQUEST_CHUNK       (1 << PD_EXTHDR_REQUEST_CHUNK_SHIFT)
#define PD_EXTHDR_CHUNK_NUMBER_SHIFT  11
#define PD_EXTHDR_CHUNK_NUMBER        (0xF << PD_EXTHDR_CHUNK_NUMBER_SHIFT)
#define PD_EXTHDR_CHUNKED_SHIFT       15
#define PD_EXTHDR_CHUNKED             (1 << PD_EXTHDR_CHUNKED_SHIFT)

/* Data size */
#define PD_DATA_SIZE(n)       (((n) << PD_EXTHDR_DATA_SIZE_SHIFT) & PD_EXTHDR_DATA_SIZE)
#define PD_DATA_SIZE_GET(msg) (((msg)->exthdr & PD_EXTHDR_DATA_SIZE) >> PD_EXTHDR_DATA_SIZE_SHIFT)

/* Chunk number */
#define PD_CHUNK_NUMBER(n)       (((n) << PD_EXTHDR_CHUNK_NUMBER_SHIFT) & PD_EXTHDR_CHUNK_NUMBER)
#define PD_CHUNK_NUMBER_GET(msg) (((msg)->exthdr & PD_EXTHDR_CHUNK_NUMBER) >> PD_EXTHDR_CHUNK_NUMBER_SHIFT)

/*
 * PD Power Data Object
 */
#define PD_PDO_TYPE_SHIFT 30
#define PD_PDO_TYPE       (0x3 << PD_PDO_TYPE_SHIFT)

/* PDO types */
#define PD_PDO_TYPE_FIXED     ((unsigned)(0x0 << PD_PDO_TYPE_SHIFT))
#define PD_PDO_TYPE_BATTERY   ((unsigned)(0x1 << PD_PDO_TYPE_SHIFT))
#define PD_PDO_TYPE_VARIABLE  ((unsigned)(0x2 << PD_PDO_TYPE_SHIFT))
#define PD_PDO_TYPE_AUGMENTED ((unsigned)(0x3 << PD_PDO_TYPE_SHIFT))

#define PD_APDO_TYPE_SHIFT 28
#define PD_APDO_TYPE       (0x3 << PD_APDO_TYPE_SHIFT)

/* APDO types */
#define PD_APDO_TYPE_PPS (0x0 << PD_APDO_TYPE_SHIFT)

/* PD Source Fixed PDO */
#define PD_PDO_SRC_FIXED_DUAL_ROLE_PWR_SHIFT     29
#define PD_PDO_SRC_FIXED_DUAL_ROLE_PWR           (1 << PD_PDO_SRC_FIXED_DUAL_ROLE_PWR_SHIFT)
#define PD_PDO_SRC_FIXED_USB_SUSPEND_SHIFT       28
#define PD_PDO_SRC_FIXED_USB_SUSPEND             (1 << PD_PDO_SRC_FIXED_USB_SUSPEND_SHIFT)
#define PD_PDO_SRC_FIXED_UNCONSTRAINED_SHIFT     27
#define PD_PDO_SRC_FIXED_UNCONSTRAINED           (1 << PD_PDO_SRC_FIXED_UNCONSTRAINED_SHIFT)
#define PD_PDO_SRC_FIXED_USB_COMMS_SHIFT         26
#define PD_PDO_SRC_FIXED_USB_COMMS               (1 << PD_PDO_SRC_FIXED_USB_COMMS_SHIFT)
#define PD_PDO_SRC_FIXED_DUAL_ROLE_DATA_SHIFT    25
#define PD_PDO_SRC_FIXED_DUAL_ROLE_DATA          (1 << PD_PDO_SRC_FIXED_DUAL_ROLE_DATA_SHIFT)
#define PD_PDO_SRC_FIXED_UNCHUNKED_EXT_MSG_SHIFT 24
#define PD_PDO_SRC_FIXED_UNCHUNKED_EXT_MSG       (1 << PD_PDO_SRC_FIXED_UNCHUNKED_EXT_MSG_SHIFT)
#define PD_PDO_SRC_FIXED_PEAK_CURRENT_SHIFT      20
#define PD_PDO_SRC_FIXED_PEAK_CURRENT            (0x3 << PD_PDO_SRC_FIXED_PEAK_CURRENT_SHIFT)
#define PD_PDO_SRC_FIXED_VOLTAGE_SHIFT           10
#define PD_PDO_SRC_FIXED_VOLTAGE                 (0x3FF << PD_PDO_SRC_FIXED_VOLTAGE_SHIFT)
#define PD_PDO_SRC_FIXED_CURRENT_SHIFT           0
#define PD_PDO_SRC_FIXED_CURRENT                 (0x3FF << PD_PDO_SRC_FIXED_CURRENT_SHIFT)

/* PD Source Fixed PDO current */
#define PD_PDO_SRC_FIXED_CURRENT_GET(pdo) (((pdo)&PD_PDO_SRC_FIXED_CURRENT) >> PD_PDO_SRC_FIXED_CURRENT_SHIFT)

/* PD Source Fixed PDO voltage */
#define PD_PDO_SRC_FIXED_VOLTAGE_GET(pdo) (((pdo)&PD_PDO_SRC_FIXED_VOLTAGE) >> PD_PDO_SRC_FIXED_VOLTAGE_SHIFT)

/* PD Programmable Power Supply APDO */
#define PD_APDO_PPS_MAX_VOLTAGE_SHIFT 17
#define PD_APDO_PPS_MAX_VOLTAGE       (0xFF << PD_APDO_PPS_MAX_VOLTAGE_SHIFT)
#define PD_APDO_PPS_MIN_VOLTAGE_SHIFT 8
#define PD_APDO_PPS_MIN_VOLTAGE       (0xFF << PD_APDO_PPS_MIN_VOLTAGE_SHIFT)
#define PD_APDO_PPS_CURRENT_SHIFT     0
#define PD_APDO_PPS_CURRENT           (0x7F << PD_APDO_PPS_CURRENT_SHIFT)

/* PD Programmable Power Supply APDO voltages */
#define PD_APDO_PPS_MAX_VOLTAGE_GET(pdo) (((pdo)&PD_APDO_PPS_MAX_VOLTAGE) >> PD_APDO_PPS_MAX_VOLTAGE_SHIFT)
#define PD_APDO_PPS_MIN_VOLTAGE_GET(pdo) (((pdo)&PD_APDO_PPS_MIN_VOLTAGE) >> PD_APDO_PPS_MIN_VOLTAGE_SHIFT)

#define PD_APDO_PPS_MAX_VOLTAGE_SET(v) (((v) << PD_APDO_PPS_MAX_VOLTAGE_SHIFT) & PD_APDO_PPS_MAX_VOLTAGE)
#define PD_APDO_PPS_MIN_VOLTAGE_SET(v) (((v) << PD_APDO_PPS_MIN_VOLTAGE_SHIFT) & PD_APDO_PPS_MIN_VOLTAGE)

/* PD Programmable Power Supply APDO current */
#define PD_APDO_PPS_CURRENT_GET(pdo) ((uint8_t)(((pdo)&PD_APDO_PPS_CURRENT) >> PD_APDO_PPS_CURRENT_SHIFT))

#define PD_APDO_PPS_CURRENT_SET(i) (((i) << PD_APDO_PPS_CURRENT_SHIFT) & PD_APDO_PPS_CURRENT)

/* PD Sink Fixed PDO */
#define PD_PDO_SNK_FIXED_DUAL_ROLE_PWR_SHIFT  29
#define PD_PDO_SNK_FIXED_DUAL_ROLE_PWR        (1 << PD_PDO_SNK_FIXED_DUAL_ROLE_PWR_SHIFT)
#define PD_PDO_SNK_FIXED_HIGHER_CAP_SHIFT     28
#define PD_PDO_SNK_FIXED_HIGHER_CAP           (1 << PD_PDO_SNK_FIXED_HIGHER_CAP_SHIFT)
#define PD_PDO_SNK_FIXED_UNCONSTRAINED_SHIFT  27
#define PD_PDO_SNK_FIXED_UNCONSTRAINED        (1 << PD_PDO_SNK_FIXED_UNCONSTRAINED_SHIFT)
#define PD_PDO_SNK_FIXED_USB_COMMS_SHIFT      26
#define PD_PDO_SNK_FIXED_USB_COMMS            (1 << PD_PDO_SNK_FIXED_USB_COMMS_SHIFT)
#define PD_PDO_SNK_FIXED_DUAL_ROLE_DATA_SHIFT 25
#define PD_PDO_SNK_FIXED_DUAL_ROLE_DATA       (1 << PD_PDO_SNK_FIXED_DUAL_ROLE_DATA_SHIFT)
#define PD_PDO_SNK_FIXED_VOLTAGE_SHIFT        10
#define PD_PDO_SNK_FIXED_VOLTAGE              (0x3FF << PD_PDO_SNK_FIXED_VOLTAGE_SHIFT)
#define PD_PDO_SNK_FIXED_CURRENT_SHIFT        0
#define PD_PDO_SNK_FIXED_CURRENT              (0x3FF << PD_PDO_SNK_FIXED_CURRENT_SHIFT)

/* PD Sink Fixed PDO current */
#define PD_PDO_SNK_FIXED_CURRENT_SET(i) (((i) << PD_PDO_SNK_FIXED_CURRENT_SHIFT) & PD_PDO_SNK_FIXED_CURRENT)

/* PD Sink Fixed PDO voltage */
#define PD_PDO_SNK_FIXED_VOLTAGE_SET(v) (((v) << PD_PDO_SNK_FIXED_VOLTAGE_SHIFT) & PD_PDO_SNK_FIXED_VOLTAGE)

/*
 * PD Request Data Object
 */
#define PD_RDO_OBJPOS_SHIFT            28
#define PD_RDO_OBJPOS                  (0x7 << PD_RDO_OBJPOS_SHIFT)
#define PD_RDO_GIVEBACK_SHIFT          27
#define PD_RDO_GIVEBACK                (1 << PD_RDO_GIVEBACK_SHIFT)
#define PD_RDO_CAP_MISMATCH_SHIFT      26
#define PD_RDO_CAP_MISMATCH            (1 << PD_RDO_CAP_MISMATCH_SHIFT)
#define PD_RDO_USB_COMMS_SHIFT         25
#define PD_RDO_USB_COMMS               (1 << PD_RDO_USB_COMMS_SHIFT)
#define PD_RDO_NO_USB_SUSPEND_SHIFT    24
#define PD_RDO_NO_USB_SUSPEND          (1 << PD_RDO_NO_USB_SUSPEND_SHIFT)
#define PD_RDO_UNCHUNKED_EXT_MSG_SHIFT 23
#define PD_RDO_UNCHUNKED_EXT_MSG       (1 << PD_RDO_UNCHUNKED_EXT_MSG_SHIFT)

#define PD_RDO_OBJPOS_SET(i)   (((i) << PD_RDO_OBJPOS_SHIFT) & PD_RDO_OBJPOS)
#define PD_RDO_OBJPOS_GET(msg) (((msg)->obj[0] & PD_RDO_OBJPOS) >> PD_RDO_OBJPOS_SHIFT)

/* Fixed and Variable RDO, no GiveBack support */
#define PD_RDO_FV_CURRENT_SHIFT     10
#define PD_RDO_FV_CURRENT           (0x3FF << PD_RDO_FV_CURRENT_SHIFT)
#define PD_RDO_FV_MAX_CURRENT_SHIFT 0
#define PD_RDO_FV_MAX_CURRENT       (0x3FF << PD_RDO_FV_MAX_CURRENT_SHIFT)

#define PD_RDO_FV_CURRENT_SET(i)     (((i) << PD_RDO_FV_CURRENT_SHIFT) & PD_RDO_FV_CURRENT)
#define PD_RDO_FV_MAX_CURRENT_SET(i) (((i) << PD_RDO_FV_MAX_CURRENT_SHIFT) & PD_RDO_FV_MAX_CURRENT)

/* Fixed and Variable RDO with GiveBack support */
#define PD_RDO_FV_MIN_CURRENT_SHIFT 0
#define PD_RDO_FV_MIN_CURRENT       (0x3FF << PD_RDO_FV_MIN_CURRENT_SHIFT)

#define PD_RDO_FV_MIN_CURRENT_SET(i) (((i) << PD_RDO_FV_MIN_CURRENT_SHIFT) & PD_RDO_FV_MIN_CURRENT)

/* TODO: Battery RDOs */

/* Programmable RDO */
#define PD_RDO_PROG_VOLTAGE_SHIFT 9
#define PD_RDO_PROG_VOLTAGE       (0x7FF << PD_RDO_PROG_VOLTAGE_SHIFT)
#define PD_RDO_PROG_CURRENT_SHIFT 0
#define PD_RDO_PROG_CURRENT       (0x7F << PD_RDO_PROG_CURRENT_SHIFT)

#define PD_RDO_PROG_VOLTAGE_SET(i) (((i) << PD_RDO_PROG_VOLTAGE_SHIFT) & PD_RDO_PROG_VOLTAGE)
#define PD_RDO_PROG_CURRENT_SET(i) (((i) << PD_RDO_PROG_CURRENT_SHIFT) & PD_RDO_PROG_CURRENT)

/*
 * Time values
 *
 * Where a range is specified, the middle of the range (rounded down to the
 * nearest millisecond) is used.
 */
#define PD_T_CHUNKING_NOT_SUPPORTED (TICKS_SECOND / 2)
#define PD_T_HARD_RESET_COMPLETE    (1 * TICKS_SECOND)
#define PD_T_PS_TRANSITION          (5 * TICKS_SECOND)
#define PD_T_SENDER_RESPONSE        (27 * TICKS_100MS)
#define PD_T_SINK_REQUEST           (1 * TICKS_SECOND)
#define PD_T_TYPEC_SINK_WAIT_CAP    (1 * TICKS_SECOND)
#define PD_T_PD_DEBOUNCE            (2 * TICKS_SECOND)

/*
 * Counter maximums
 */
#define PD_N_HARD_RESET_COUNT 2

/*
 * Value parameters
 */
#define PD_MAX_EXT_MSG_LEN        260
#define PD_MAX_EXT_MSG_CHUNK_LEN  26
#define PD_MAX_EXT_MSG_LEGACY_LEN 26

/*
 * Unit conversions
 *
 * V: volt
 * CV: centivolt
 * MV: millivolt
 * PRV: Programmable RDO voltage unit (20 mV)
 * PDV: Power Delivery voltage unit (50 mV)
 * PAV: PPS APDO voltage unit (100 mV)
 *
 * A: ampere
 * CA: centiampere
 * MA: milliampere
 * PDI: Power Delivery current unit (10 mA)
 * PAI: PPS APDO current unit (50 mA)
 *
 * W: watt
 * CW: centiwatt
 * MW: milliwatt
 *
 * O: ohm
 * CO: centiohm
 * MO: milliohm
 */
#define PD_MV2PRV(mv)  ((mv) / 20)
#define PD_MV2PDV(mv)  ((mv) / 50)
#define PD_MV2PAV(mv)  ((mv) / 100)
#define PD_PRV2MV(prv) ((prv)*20)
#define PD_PDV2MV(pdv) ((pdv)*50)
#define PD_PAV2MV(pav) ((pav)*100)

#define PD_MA2CA(ma)   (((ma) + 10 - 1) / 10)
#define PD_MA2PDI(ma)  (((ma) + 10 - 1) / 10)
#define PD_MA2PAI(ma)  (((ma) + 50 - 1) / 50)
#define PD_CA2PAI(ca)  (((ca) + 5 - 1) / 5)
#define PD_PDI2MA(pdi) ((pdi)*10)
#define PD_PAI2MA(pai) ((pai)*50)
#define PD_PAI2CA(pai) ((pai)*5)

#define PD_MW2CW(mw) ((mw) / 10)

#define PD_MO2CO(mo) ((mo) / 10)

/* Get portions of a voltage in more normal units */
#define PD_MV_V(mv)  ((mv) / 1000)
#define PD_MV_MV(mv) ((mv) % 1000)

#define PD_PDV_V(pdv)  ((pdv) / 20)
#define PD_PDV_CV(pdv) (5 * ((pdv) % 20))

#define PD_PAV_V(pav)  ((pav) / 10)
#define PD_PAV_CV(pav) (10 * ((pav) % 10))

/* Get portions of a PD current in more normal units */
#define PD_PDI_A(pdi)  ((pdi) / 100)
#define PD_PDI_CA(pdi) ((pdi) % 100)

#define PD_PAI_A(pai)  ((pai) / 20)
#define PD_PAI_CA(pai) (5 * ((pai) % 20))

/* Get portions of a power in more normal units */
#define PD_CW_W(cw)  ((cw) / 100)
#define PD_CW_CW(cw) ((cw) % 100)

/* Get portions of a resistance in more normal units */
#define PD_CO_O(co)  ((co) / 100)
#define PD_CO_CO(co) ((co) % 100)

/*
 * Unit constants
 */
#define PD_MV_MIN  0
#define PD_MV_MAX  21000
#define PD_PDV_MIN PD_MV2PDV(PD_MV_MIN)
#define PD_PDV_MAX PD_MV2PDV(PD_MV_MAX)

#define PD_MA_MIN  0
#define PD_MA_MAX  5000
#define PD_CA_MIN  PD_MA2CA(PD_MA_MIN)
#define PD_CA_MAX  PD_MA2CA(PD_MA_MAX)
#define PD_PDI_MIN PD_MA2PDI(PD_MA_MIN)
#define PD_PDI_MAX PD_MA2PDI(PD_MA_MAX)

#define PD_MW_MIN 0
#define PD_MW_MAX 100000

#define PD_MO_MIN 500
#define PD_MO_MAX 655350

/*
 * FUSB Type-C Current level enum
 */
enum fusb_typec_current { fusb_tcc_none = 0, fusb_tcc_default = 1, fusb_tcc_1_5 = 2, fusb_sink_tx_ng = 2, fusb_tcc_3_0 = 3, fusb_sink_tx_ok = 3 };

#endif /* PDB_PD_H */
