/* hci.h - Bluetooth Host Control Interface definitions */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_HCI_HOST_H
#define __BT_HCI_HOST_H

#include <toolchain.h>
#include <zephyr/types.h>
#include <stdbool.h>
#include <string.h>
#include <misc/util.h>
#include <addr.h>
#include <hci_err.h>

#if defined(BFLB_BLE)
#include <buf.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Special own address types for LL privacy (used in adv & scan parameters) */
#define BT_HCI_OWN_ADDR_RPA_OR_PUBLIC 0x02
#define BT_HCI_OWN_ADDR_RPA_OR_RANDOM 0x03
#define BT_HCI_OWN_ADDR_RPA_MASK      0x02

#define BT_ENC_KEY_SIZE_MIN 0x07
#define BT_ENC_KEY_SIZE_MAX 0x10

struct bt_hci_evt_hdr {
    u8_t evt;
    u8_t len;
} __packed;
#define BT_HCI_EVT_HDR_SIZE 2

#define BT_ACL_START_NO_FLUSH 0x00
#define BT_ACL_CONT           0x01
#define BT_ACL_START          0x02

#define bt_acl_handle(h)         ((h)&0x0fff)
#define bt_acl_flags(h)          ((h) >> 12)
#define bt_acl_handle_pack(h, f) ((h) | ((f) << 12))

struct bt_hci_acl_hdr {
    u16_t handle;
    u16_t len;
} __packed;
#define BT_HCI_ACL_HDR_SIZE 4

#define BT_ISO_START  0x00
#define BT_ISO_CONT   0x01
#define BT_ISO_SINGLE 0x02
#define BT_ISO_END    0x03

#define bt_iso_handle(h)   ((h)&0x0fff)
#define bt_iso_flags(h)    ((h) >> 12)
#define bt_iso_flags_pb(f) ((f)&0x0003)
#define bt_iso_flags_ts(f) (((f) >> 2) & 0x0001)
#define bt_iso_pack_flags(pb, ts) \
    (((pb)&0x0003) | (((ts)&0x0001) << 2))
#define bt_iso_handle_pack(h, pb, ts) \
    ((h) | (bt_iso_pack_flags(pb, ts) << 12))

#define BT_ISO_DATA_VALID   0x00
#define BT_ISO_DATA_INVALID 0x01
#define BT_ISO_DATA_NOP     0x02

#define bt_iso_pkt_len(h)         ((h)&0x3fff)
#define bt_iso_pkt_flags(h)       ((h) >> 14)
#define bt_iso_pkt_len_pack(h, f) ((h) | ((f) << 14))

struct bt_hci_iso_data_hdr {
    uint16_t sn;
    uint16_t slen;
} __packed;
#define BT_HCI_ISO_DATA_HDR_SIZE 4

struct bt_hci_iso_ts_data_hdr {
    uint32_t ts;
    struct bt_hci_iso_data_hdr data;
} __packed;
#define BT_HCI_ISO_TS_DATA_HDR_SIZE 8

struct bt_hci_iso_hdr {
    uint16_t handle;
    uint16_t len;
} __packed;
#define BT_HCI_ISO_HDR_SIZE 4

struct bt_hci_cmd_hdr {
    u16_t opcode;
    u8_t param_len;
} __packed;
#define BT_HCI_CMD_HDR_SIZE 3

/* Supported Commands */
#define BT_CMD_TEST(cmd, octet, bit) (cmd[octet] & BIT(bit))
#define BT_CMD_LE_STATES(cmd)        BT_CMD_TEST(cmd, 28, 3)

#define BT_FEAT_TEST(feat, page, octet, bit) (feat[page][octet] & BIT(bit))

#define BT_FEAT_BREDR(feat)        !BT_FEAT_TEST(feat, 0, 4, 5)
#define BT_FEAT_LE(feat)           BT_FEAT_TEST(feat, 0, 4, 6)
#define BT_FEAT_EXT_FEATURES(feat) BT_FEAT_TEST(feat, 0, 7, 7)
#define BT_FEAT_HOST_SSP(feat)     BT_FEAT_TEST(feat, 1, 0, 0)
#define BT_FEAT_SC(feat)           BT_FEAT_TEST(feat, 2, 1, 0)

#define BT_FEAT_LMP_ESCO_CAPABLE(feat) BT_FEAT_TEST(feat, 0, 3, 7)
#define BT_FEAT_HV2_PKT(feat)          BT_FEAT_TEST(feat, 0, 1, 4)
#define BT_FEAT_HV3_PKT(feat)          BT_FEAT_TEST(feat, 0, 1, 5)
#define BT_FEAT_EV4_PKT(feat)          BT_FEAT_TEST(feat, 0, 4, 0)
#define BT_FEAT_EV5_PKT(feat)          BT_FEAT_TEST(feat, 0, 4, 1)
#define BT_FEAT_2EV3_PKT(feat)         BT_FEAT_TEST(feat, 0, 5, 5)
#define BT_FEAT_3EV3_PKT(feat)         BT_FEAT_TEST(feat, 0, 5, 6)
#define BT_FEAT_3SLOT_PKT(feat)        BT_FEAT_TEST(feat, 0, 5, 7)

/* LE features */
#define BT_LE_FEAT_BIT_ENC                     0
#define BT_LE_FEAT_BIT_CONN_PARAM_REQ          1
#define BT_LE_FEAT_BIT_EXT_REJ_IND             2
#define BT_LE_FEAT_BIT_SLAVE_FEAT_REQ          3
#define BT_LE_FEAT_BIT_PING                    4
#define BT_LE_FEAT_BIT_DLE                     5
#define BT_LE_FEAT_BIT_PRIVACY                 6
#define BT_LE_FEAT_BIT_EXT_SCAN                7
#define BT_LE_FEAT_BIT_PHY_2M                  8
#define BT_LE_FEAT_BIT_SMI_TX                  9
#define BT_LE_FEAT_BIT_SMI_RX                  10
#define BT_LE_FEAT_BIT_PHY_CODED               11
#define BT_LE_FEAT_BIT_ADV_EXT                 12
#define BT_LE_FEAT_BIT_ADV_PER                 13
#define BT_LE_FEAT_BIT_CHAN_SEL_ALGO_2         14
#define BT_LE_FEAT_BIT_PWR_CLASS_1             15
#define BT_LE_FEAT_BIT_MIN_USED_CHAN_PROC      16
#define BT_LE_FEAT_BIT_CONN_CTE_REQ            17
#define BT_LE_FEAT_BIT_CONN_CTE_RESP           18
#define BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_TX   19
#define BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_RX   20
#define BT_LE_FEAT_BIT_ANT_SWITCH_TX_AOD       21
#define BT_LE_FEAT_BIT_ANT_SWITCH_RX_AOA       22
#define BT_LE_FEAT_BIT_RX_CTE                  23
#define BT_LE_FEAT_BIT_PAST_SEND               24
#define BT_LE_FEAT_BIT_PAST_RECV               25
#define BT_LE_FEAT_BIT_SCA_UPDATE              26
#define BT_LE_FEAT_BIT_REMOTE_PUB_KEY_VALIDATE 27
#define BT_LE_FEAT_BIT_CIS_MASTER              28
#define BT_LE_FEAT_BIT_CIS_SLAVE               29
#define BT_LE_FEAT_BIT_ISO_BROADCASTER         30
#define BT_LE_FEAT_BIT_SYNC_RECEIVER           31
#define BT_LE_FEAT_BIT_ISO_CHANNELS            32
#define BT_LE_FEAT_BIT_PWR_CTRL_REQ            33
#define BT_LE_FEAT_BIT_PWR_CHG_IND             34
#define BT_LE_FEAT_BIT_PATH_LOSS_MONITOR       35

#define BT_LE_FEAT_TEST(feat, n) (feat[(n) >> 3] & \
                                  BIT((n)&7))

#define BT_FEAT_LE_ENCR(feat) BT_LE_FEAT_TEST(feat, \
                                              BT_LE_FEAT_BIT_ENC)
#define BT_FEAT_LE_CONN_PARAM_REQ_PROC(feat) BT_LE_FEAT_TEST(feat, \
                                                             BT_LE_FEAT_BIT_CONN_PARAM_REQ)
#define BT_FEAT_LE_SLAVE_FEATURE_XCHG(feat) BT_LE_FEAT_TEST(feat, \
                                                            BT_LE_FEAT_BIT_SLAVE_FEAT_REQ)
#define BT_FEAT_LE_DLE(feat) BT_LE_FEAT_TEST(feat, \
                                             BT_LE_FEAT_BIT_DLE)
#define BT_FEAT_LE_PHY_2M(feat) BT_LE_FEAT_TEST(feat, \
                                                BT_LE_FEAT_BIT_PHY_2M)
#define BT_FEAT_LE_PHY_CODED(feat) BT_LE_FEAT_TEST(feat, \
                                                   BT_LE_FEAT_BIT_PHY_CODED)
#define BT_FEAT_LE_PRIVACY(feat) BT_LE_FEAT_TEST(feat, \
                                                 BT_LE_FEAT_BIT_PRIVACY)
#define BT_FEAT_LE_EXT_ADV(feat) BT_LE_FEAT_TEST(feat, \
                                                 BT_LE_FEAT_BIT_EXT_ADV)
#define BT_FEAT_LE_EXT_PER_ADV(feat) BT_LE_FEAT_TEST(feat, \
                                                     BT_LE_FEAT_BIT_PER_ADV)
#define BT_FEAT_LE_CONNECTIONLESS_CTE_TX(feat) BT_LE_FEAT_TEST(feat, \
                                                               BT_LE_FEAT_BIT_CONNECTIONLESS_CTE_TX)
#define BT_FEAT_LE_ANT_SWITCH_TX_AOD(feat) BT_LE_FEAT_TEST(feat, \
                                                           BT_LE_FEAT_BIT_ANT_SWITCH_TX_AOD)
#define BT_FEAT_LE_PAST_SEND(feat) BT_LE_FEAT_TEST(feat, \
                                                   BT_LE_FEAT_BIT_PAST_SEND)
#define BT_FEAT_LE_PAST_RECV(feat) BT_LE_FEAT_TEST(feat, \
                                                   BT_LE_FEAT_BIT_PAST_RECV)
#define BT_FEAT_LE_CIS_MASTER(feat) BT_LE_FEAT_TEST(feat, \
                                                    BT_LE_FEAT_BIT_CIS_MASTER)
#define BT_FEAT_LE_CIS_SLAVE(feat) BT_LE_FEAT_TEST(feat, \
                                                   BT_LE_FEAT_BIT_CIS_SLAVE)
#define BT_FEAT_LE_ISO_BROADCASTER(feat) BT_LE_FEAT_TEST(feat, \
                                                         BT_LE_FEAT_BIT_ISO_BROADCASTER)
#define BT_FEAT_LE_SYNC_RECEIVER(feat) BT_LE_FEAT_TEST(feat, \
                                                       BT_LE_FEAT_BIT_SYNC_RECEIVER)
#define BT_FEAT_LE_ISO_CHANNELS(feat) BT_LE_FEAT_TEST(feat, \
                                                      BT_LE_FEAT_BIT_ISO_CHANNELS)

#define BT_FEAT_LE_CIS(feat) (BT_FEAT_LE_CIS_MASTER(feat) | \
                              BT_FEAT_LE_CIS_SLAVE(feat))
#define BT_FEAT_LE_BIS(feat) (BT_FEAT_LE_ISO_BROADCASTER(feat) | \
                              BT_FEAT_LE_SYNC_RECEIVER(feat))
#define BT_FEAT_LE_ISO(feat) (BT_FEAT_LE_CIS(feat) | \
                              BT_FEAT_LE_BIS(feat))

/* LE States */
#define BT_LE_STATES_SLAVE_CONN_ADV(states) (states & 0x0000004000000000)

/* Bonding/authentication types */
#define BT_HCI_NO_BONDING             0x00
#define BT_HCI_NO_BONDING_MITM        0x01
#define BT_HCI_DEDICATED_BONDING      0x02
#define BT_HCI_DEDICATED_BONDING_MITM 0x03
#define BT_HCI_GENERAL_BONDING        0x04
#define BT_HCI_GENERAL_BONDING_MITM   0x05

/*
 * MITM protection is enabled in SSP authentication requirements octet when
 * LSB bit is set.
 */
#define BT_MITM 0x01

/* I/O capabilities */
#define BT_IO_DISPLAY_ONLY    0x00
#define BT_IO_DISPLAY_YESNO   0x01
#define BT_IO_KEYBOARD_ONLY   0x02
#define BT_IO_NO_INPUT_OUTPUT 0x03

/* SCO packet types */
#define HCI_PKT_TYPE_HV1 0x0020
#define HCI_PKT_TYPE_HV2 0x0040
#define HCI_PKT_TYPE_HV3 0x0080

/* eSCO packet types */
#define HCI_PKT_TYPE_ESCO_HV1  0x0001
#define HCI_PKT_TYPE_ESCO_HV2  0x0002
#define HCI_PKT_TYPE_ESCO_HV3  0x0004
#define HCI_PKT_TYPE_ESCO_EV3  0x0008
#define HCI_PKT_TYPE_ESCO_EV4  0x0010
#define HCI_PKT_TYPE_ESCO_EV5  0x0020
#define HCI_PKT_TYPE_ESCO_2EV3 0x0040
#define HCI_PKT_TYPE_ESCO_3EV3 0x0080
#define HCI_PKT_TYPE_ESCO_2EV5 0x0100
#define HCI_PKT_TYPE_ESCO_3EV5 0x0200

#define ESCO_PKT_MASK (HCI_PKT_TYPE_ESCO_HV1 | \
                       HCI_PKT_TYPE_ESCO_HV2 | \
                       HCI_PKT_TYPE_ESCO_HV3)
#define SCO_PKT_MASK (HCI_PKT_TYPE_HV1 | \
                      HCI_PKT_TYPE_HV2 | \
                      HCI_PKT_TYPE_HV3)
#define EDR_ESCO_PKT_MASK (HCI_PKT_TYPE_ESCO_2EV3 | \
                           HCI_PKT_TYPE_ESCO_3EV3 | \
                           HCI_PKT_TYPE_ESCO_2EV5 | \
                           HCI_PKT_TYPE_ESCO_3EV5)

/* HCI BR/EDR link types */
#define BT_HCI_SCO  0x00
#define BT_HCI_ACL  0x01
#define BT_HCI_ESCO 0x02

/* OpCode Group Fields */
#define BT_OGF_LINK_CTRL 0x01
#define BT_OGF_BASEBAND  0x03
#define BT_OGF_INFO      0x04
#define BT_OGF_STATUS    0x05
#define BT_OGF_LE        0x08
#define BT_OGF_VS        0x3f

/* Construct OpCode from OGF and OCF */
#define BT_OP(ogf, ocf) ((ocf) | ((ogf) << 10))

/* Invalid opcode */
#define BT_OP_NOP 0x0000

/* Obtain OGF from OpCode */
#define BT_OGF(opcode) (((opcode) >> 10) & BIT_MASK(6))
/* Obtain OCF from OpCode */
#define BT_OCF(opcode) ((opcode)&BIT_MASK(10))

#define BT_HCI_OP_INQUIRY BT_OP(BT_OGF_LINK_CTRL, 0x0001)
struct bt_hci_op_inquiry {
    u8_t lap[3];
    u8_t length;
    u8_t num_rsp;
} __packed;

#define BT_HCI_OP_INQUIRY_CANCEL BT_OP(BT_OGF_LINK_CTRL, 0x0002)

#define BT_HCI_OP_CONNECT BT_OP(BT_OGF_LINK_CTRL, 0x0005)
struct bt_hci_cp_connect {
    bt_addr_t bdaddr;
    u16_t packet_type;
    u8_t pscan_rep_mode;
    u8_t reserved;
    u16_t clock_offset;
    u8_t allow_role_switch;
} __packed;

#define BT_HCI_OP_DISCONNECT BT_OP(BT_OGF_LINK_CTRL, 0x0006)
struct bt_hci_cp_disconnect {
    u16_t handle;
    u8_t reason;
} __packed;

#define BT_HCI_OP_CONNECT_CANCEL BT_OP(BT_OGF_LINK_CTRL, 0x0008)
struct bt_hci_cp_connect_cancel {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_connect_cancel {
    u8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_ACCEPT_CONN_REQ BT_OP(BT_OGF_LINK_CTRL, 0x0009)
struct bt_hci_cp_accept_conn_req {
    bt_addr_t bdaddr;
    u8_t role;
} __packed;

#define BT_HCI_OP_SETUP_SYNC_CONN BT_OP(BT_OGF_LINK_CTRL, 0x0028)
struct bt_hci_cp_setup_sync_conn {
    u16_t handle;
    u32_t tx_bandwidth;
    u32_t rx_bandwidth;
    u16_t max_latency;
    u16_t content_format;
    u8_t retrans_effort;
    u16_t pkt_type;
} __packed;

#define BT_HCI_OP_ACCEPT_SYNC_CONN_REQ BT_OP(BT_OGF_LINK_CTRL, 0x0029)
struct bt_hci_cp_accept_sync_conn_req {
    bt_addr_t bdaddr;
    u32_t tx_bandwidth;
    u32_t rx_bandwidth;
    u16_t max_latency;
    u16_t content_format;
    u8_t retrans_effort;
    u16_t pkt_type;
} __packed;

#define BT_HCI_OP_REJECT_CONN_REQ BT_OP(BT_OGF_LINK_CTRL, 0x000a)
struct bt_hci_cp_reject_conn_req {
    bt_addr_t bdaddr;
    u8_t reason;
} __packed;

#define BT_HCI_OP_LINK_KEY_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x000b)
struct bt_hci_cp_link_key_reply {
    bt_addr_t bdaddr;
    u8_t link_key[16];
} __packed;

#define BT_HCI_OP_LINK_KEY_NEG_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x000c)
struct bt_hci_cp_link_key_neg_reply {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_PIN_CODE_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x000d)
struct bt_hci_cp_pin_code_reply {
    bt_addr_t bdaddr;
    u8_t pin_len;
    u8_t pin_code[16];
} __packed;
struct bt_hci_rp_pin_code_reply {
    u8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_PIN_CODE_NEG_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x000e)
struct bt_hci_cp_pin_code_neg_reply {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_pin_code_neg_reply {
    u8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_AUTH_REQUESTED BT_OP(BT_OGF_LINK_CTRL, 0x0011)
struct bt_hci_cp_auth_requested {
    u16_t handle;
} __packed;

#define BT_HCI_OP_SET_CONN_ENCRYPT BT_OP(BT_OGF_LINK_CTRL, 0x0013)
struct bt_hci_cp_set_conn_encrypt {
    u16_t handle;
    u8_t encrypt;
} __packed;

#define BT_HCI_OP_REMOTE_NAME_REQUEST BT_OP(BT_OGF_LINK_CTRL, 0x0019)
struct bt_hci_cp_remote_name_request {
    bt_addr_t bdaddr;
    u8_t pscan_rep_mode;
    u8_t reserved;
    u16_t clock_offset;
} __packed;

#define BT_HCI_OP_REMOTE_NAME_CANCEL BT_OP(BT_OGF_LINK_CTRL, 0x001a)
struct bt_hci_cp_remote_name_cancel {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_remote_name_cancel {
    u8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_READ_REMOTE_FEATURES BT_OP(BT_OGF_LINK_CTRL, 0x001b)
struct bt_hci_cp_read_remote_features {
    u16_t handle;
} __packed;

#define BT_HCI_OP_READ_REMOTE_EXT_FEATURES BT_OP(BT_OGF_LINK_CTRL, 0x001c)
struct bt_hci_cp_read_remote_ext_features {
    u16_t handle;
    u8_t page;
} __packed;

#define BT_HCI_OP_READ_REMOTE_VERSION_INFO BT_OP(BT_OGF_LINK_CTRL, 0x001d)
struct bt_hci_cp_read_remote_version_info {
    u16_t handle;
} __packed;

#define BT_HCI_OP_IO_CAPABILITY_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x002b)
struct bt_hci_cp_io_capability_reply {
    bt_addr_t bdaddr;
    u8_t capability;
    u8_t oob_data;
    u8_t authentication;
} __packed;

#define BT_HCI_OP_USER_CONFIRM_REPLY     BT_OP(BT_OGF_LINK_CTRL, 0x002c)
#define BT_HCI_OP_USER_CONFIRM_NEG_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x002d)
struct bt_hci_cp_user_confirm_reply {
    bt_addr_t bdaddr;
} __packed;
struct bt_hci_rp_user_confirm_reply {
    u8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_USER_PASSKEY_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x002e)
struct bt_hci_cp_user_passkey_reply {
    bt_addr_t bdaddr;
    u32_t passkey;
} __packed;

#define BT_HCI_OP_USER_PASSKEY_NEG_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x002f)
struct bt_hci_cp_user_passkey_neg_reply {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_OP_IO_CAPABILITY_NEG_REPLY BT_OP(BT_OGF_LINK_CTRL, 0x0034)
struct bt_hci_cp_io_capability_neg_reply {
    bt_addr_t bdaddr;
    u8_t reason;
} __packed;

#define BT_HCI_OP_SET_EVENT_MASK BT_OP(BT_OGF_BASEBAND, 0x0001)
struct bt_hci_cp_set_event_mask {
    u8_t events[8];
} __packed;

#define BT_HCI_OP_RESET BT_OP(BT_OGF_BASEBAND, 0x0003)

#define BT_HCI_OP_WRITE_LOCAL_NAME BT_OP(BT_OGF_BASEBAND, 0x0013)
struct bt_hci_write_local_name {
    u8_t local_name[248];
} __packed;

#define BT_HCI_OP_WRITE_PAGE_TIMEOUT BT_OP(BT_OGF_BASEBAND, 0x0018)

#define BT_HCI_OP_WRITE_SCAN_ENABLE BT_OP(BT_OGF_BASEBAND, 0x001a)
#define BT_BREDR_SCAN_DISABLED      0x00
#define BT_BREDR_SCAN_INQUIRY       0x01
#define BT_BREDR_SCAN_PAGE          0x02

#define BT_HCI_OP_WRITE_INQUIRY_SCAN_ACTIVITY BT_OP(BT_OGF_BASEBAND, 0x001e)
struct bt_hci_cp_write_inquiry_scan_activity {
    u16_t interval;
    u16_t window;
} __packed;

#define BT_HCI_OP_WRITE_CLASS_OF_DEVICE BT_OP(BT_OGF_BASEBAND, 0x0024)
struct bt_hci_cp_write_class_of_device {
    u8_t cod[3];
} __packed;

#define BT_TX_POWER_LEVEL_CURRENT     0x00
#define BT_TX_POWER_LEVEL_MAX         0x01
#define BT_HCI_OP_READ_TX_POWER_LEVEL BT_OP(BT_OGF_BASEBAND, 0x002d)
struct bt_hci_cp_read_tx_power_level {
    u16_t handle;
    u8_t type;
} __packed;

struct bt_hci_rp_read_tx_power_level {
    u8_t status;
    u16_t handle;
    s8_t tx_power_level;
} __packed;

#define BT_HCI_CTL_TO_HOST_FLOW_DISABLE 0x00
#define BT_HCI_CTL_TO_HOST_FLOW_ENABLE  0x01
#define BT_HCI_OP_SET_CTL_TO_HOST_FLOW  BT_OP(BT_OGF_BASEBAND, 0x0031)
struct bt_hci_cp_set_ctl_to_host_flow {
    u8_t flow_enable;
} __packed;

#define BT_HCI_OP_HOST_BUFFER_SIZE BT_OP(BT_OGF_BASEBAND, 0x0033)
struct bt_hci_cp_host_buffer_size {
    u16_t acl_mtu;
    u8_t sco_mtu;
    u16_t acl_pkts;
    u16_t sco_pkts;
} __packed;

struct bt_hci_handle_count {
    u16_t handle;
    u16_t count;
} __packed;

#define BT_HCI_OP_HOST_NUM_COMPLETED_PACKETS BT_OP(BT_OGF_BASEBAND, 0x0035)
struct bt_hci_cp_host_num_completed_packets {
    u8_t num_handles;
    struct bt_hci_handle_count h[0];
} __packed;

#define BT_HCI_OP_WRITE_INQUIRY_SCAN_TYPE BT_OP(BT_OGF_BASEBAND, 0x0043)
struct bt_hci_cp_write_inquiry_scan_type {
    u8_t type;
} __packed;

#define BT_HCI_OP_WRITE_INQUIRY_MODE BT_OP(BT_OGF_BASEBAND, 0x0045)
struct bt_hci_cp_write_inquiry_mode {
    u8_t mode;
} __packed;

#define BT_HCI_OP_WRITE_PAGE_SCAN_TYPE BT_OP(BT_OGF_BASEBAND, 0x0047)
struct bt_hci_cp_write_page_scan_type {
    u8_t type;
} __packed;

#define BT_HCI_OP_WRITE_EXT_INQUIRY_RESP BT_OP(BT_OGF_BASEBAND, 0x0052)
struct bt_hci_cp_write_ext_inquiry_resp {
    u8_t rec;
    u8_t eir[240];
} __packed;

#define BT_HCI_OP_WRITE_SSP_MODE BT_OP(BT_OGF_BASEBAND, 0x0056)
struct bt_hci_cp_write_ssp_mode {
    u8_t mode;
} __packed;

#define BT_HCI_OP_SET_EVENT_MASK_PAGE_2 BT_OP(BT_OGF_BASEBAND, 0x0063)
struct bt_hci_cp_set_event_mask_page_2 {
    u8_t events_page_2[8];
} __packed;

#define BT_HCI_OP_LE_WRITE_LE_HOST_SUPP BT_OP(BT_OGF_BASEBAND, 0x006d)
struct bt_hci_cp_write_le_host_supp {
    u8_t le;
    u8_t simul;
} __packed;

#define BT_HCI_OP_WRITE_SC_HOST_SUPP BT_OP(BT_OGF_BASEBAND, 0x007a)
struct bt_hci_cp_write_sc_host_supp {
    u8_t sc_support;
} __packed;

#define BT_HCI_OP_READ_AUTH_PAYLOAD_TIMEOUT BT_OP(BT_OGF_BASEBAND, 0x007b)
struct bt_hci_cp_read_auth_payload_timeout {
    u16_t handle;
} __packed;

struct bt_hci_rp_read_auth_payload_timeout {
    u8_t status;
    u16_t handle;
    u16_t auth_payload_timeout;
} __packed;

#define BT_HCI_OP_WRITE_AUTH_PAYLOAD_TIMEOUT BT_OP(BT_OGF_BASEBAND, 0x007c)
struct bt_hci_cp_write_auth_payload_timeout {
    u16_t handle;
    u16_t auth_payload_timeout;
} __packed;

struct bt_hci_rp_write_auth_payload_timeout {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_OP_CONFIGURE_DATA_PATH BT_OP(BT_OGF_BASEBAND, 0x0083)
struct bt_hci_cp_configure_data_path {
    uint8_t data_path_dir;
    uint8_t data_path_id;
    uint8_t vs_config_len;
    uint8_t vs_config[0];
} __packed;

struct bt_hci_rp_configure_data_path {
    uint8_t status;
} __packed;

/* HCI version from Assigned Numbers */
#define BT_HCI_VERSION_1_0B 0
#define BT_HCI_VERSION_1_1  1
#define BT_HCI_VERSION_1_2  2
#define BT_HCI_VERSION_2_0  3
#define BT_HCI_VERSION_2_1  4
#define BT_HCI_VERSION_3_0  5
#define BT_HCI_VERSION_4_0  6
#define BT_HCI_VERSION_4_1  7
#define BT_HCI_VERSION_4_2  8
#define BT_HCI_VERSION_5_0  9
#define BT_HCI_VERSION_5_1  10
#define BT_HCI_VERSION_5_2  11

#define BT_HCI_OP_READ_LOCAL_VERSION_INFO BT_OP(BT_OGF_INFO, 0x0001)
struct bt_hci_rp_read_local_version_info {
    u8_t status;
    u8_t hci_version;
    u16_t hci_revision;
    u8_t lmp_version;
    u16_t manufacturer;
    u16_t lmp_subversion;
} __packed;

#define BT_HCI_OP_READ_SUPPORTED_COMMANDS BT_OP(BT_OGF_INFO, 0x0002)
struct bt_hci_rp_read_supported_commands {
    u8_t status;
    u8_t commands[64];
} __packed;

#define BT_HCI_OP_READ_LOCAL_EXT_FEATURES BT_OP(BT_OGF_INFO, 0x0004)
struct bt_hci_cp_read_local_ext_features {
    u8_t page;
};
struct bt_hci_rp_read_local_ext_features {
    u8_t status;
    u8_t page;
    u8_t max_page;
    u8_t ext_features[8];
} __packed;

#define BT_HCI_OP_READ_LOCAL_FEATURES BT_OP(BT_OGF_INFO, 0x0003)
struct bt_hci_rp_read_local_features {
    u8_t status;
    u8_t features[8];
} __packed;

#define BT_HCI_OP_READ_BUFFER_SIZE BT_OP(BT_OGF_INFO, 0x0005)
struct bt_hci_rp_read_buffer_size {
    u8_t status;
    u16_t acl_max_len;
    u8_t sco_max_len;
    u16_t acl_max_num;
    u16_t sco_max_num;
} __packed;

#define BT_HCI_OP_READ_BD_ADDR BT_OP(BT_OGF_INFO, 0x0009)
struct bt_hci_rp_read_bd_addr {
    u8_t status;
    bt_addr_t bdaddr;
} __packed;

/* logic transport type bits as returned when reading supported codecs */
#define BT_HCI_CODEC_TRANSPORT_MASK_BREDR_ACL BIT(0)
#define BT_HCI_CODEC_TRANSPORT_MASK_BREDR_SCO BIT(1)
#define BT_HCI_CODEC_TRANSPORT_MASK_LE_CIS    BIT(2)
#define BT_HCI_CODEC_TRANSPORT_MASK_LE_BIS    BIT(3)

/* logic transport types for reading codec capabilities and controller delays */
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_BREDR_ACL 0x00
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_BREDR_SCO 0x01
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_LE_CIS    0x02
#define BT_HCI_LOGICAL_TRANSPORT_TYPE_LE_BIS    0x03

/* audio datapath directions */
#define BT_HCI_DATAPATH_DIR_HOST_TO_CTLR 0x00
#define BT_HCI_DATAPATH_DIR_CTLR_TO_HOST 0x01

/* coding format assigned numbers, used for codec IDs */
#define BT_HCI_CODING_FORMAT_ULAW_LOG    0x00
#define BT_HCI_CODING_FORMAT_ALAW_LOG    0x01
#define BT_HCI_CODING_FORMAT_CVSD        0x02
#define BT_HCI_CODING_FORMAT_TRANSPARENT 0x03
#define BT_HCI_CODING_FORMAT_LINEAR_PCM  0x04
#define BT_HCI_CODING_FORMAT_MSBC        0x05
#define BT_HCI_CODING_FORMAT_VS          0xFF

#define BT_HCI_OP_READ_CODECS BT_OP(BT_OGF_INFO, 0x000b)
struct bt_hci_std_codec_info {
    uint8_t codec_id;
} __packed;
struct bt_hci_std_codecs {
    uint8_t num_codecs;
    struct bt_hci_std_codec_info codec_info[0];
} __packed;
struct bt_hci_vs_codec_info {
    uint16_t company_id;
    uint16_t codec_id;
} __packed;
struct bt_hci_vs_codecs {
    uint8_t num_codecs;
    struct bt_hci_vs_codec_info codec_info[0];
} __packed;
struct bt_hci_rp_read_codecs {
    uint8_t status;
    /* other fields filled in dynamically */
    uint8_t codecs[0];
} __packed;

#define BT_HCI_OP_READ_CODECS_V2 BT_OP(BT_OGF_INFO, 0x000d)
struct bt_hci_std_codec_info_v2 {
    uint8_t codec_id;
    uint8_t transports; /* bitmap */
} __packed;
struct bt_hci_std_codecs_v2 {
    uint8_t num_codecs;
    struct bt_hci_std_codec_info_v2 codec_info[0];
} __packed;
struct bt_hci_vs_codec_info_v2 {
    uint16_t company_id;
    uint16_t codec_id;
    uint8_t transports; /* bitmap */
} __packed;
struct bt_hci_vs_codecs_v2 {
    uint8_t num_codecs;
    struct bt_hci_vs_codec_info_v2 codec_info[0];
} __packed;
struct bt_hci_rp_read_codecs_v2 {
    uint8_t status;
    /* other fields filled in dynamically */
    uint8_t codecs[0];
} __packed;

struct bt_hci_cp_codec_id {
    uint8_t coding_format;
    uint16_t company_id;
    uint16_t vs_codec_id;
} __packed;

#define BT_HCI_OP_READ_CODEC_CAPABILITIES BT_OP(BT_OGF_INFO, 0x000e)
struct bt_hci_cp_read_codec_capabilities {
    struct bt_hci_cp_codec_id codec_id;
    uint8_t transport;
    uint8_t direction;
} __packed;
struct bt_hci_codec_capability_info {
    uint8_t length;
    uint8_t data[0];
} __packed;
struct bt_hci_rp_read_codec_capabilities {
    uint8_t status;
    uint8_t num_capabilities;
    /* other fields filled in dynamically */
    uint8_t capabilities[0];
} __packed;

#define BT_HCI_OP_READ_CTLR_DELAY BT_OP(BT_OGF_INFO, 0x000f)
struct bt_hci_cp_read_ctlr_delay {
    struct bt_hci_cp_codec_id codec_id;
    uint8_t transport;
    uint8_t direction;
    uint8_t codec_config_len;
    uint8_t codec_config[0];
} __packed;
struct bt_hci_rp_read_ctlr_delay {
    uint8_t status;
    uint8_t min_ctlr_delay[3];
    uint8_t max_ctlr_delay[3];
} __packed;

#define BT_HCI_OP_READ_RSSI BT_OP(BT_OGF_STATUS, 0x0005)
struct bt_hci_cp_read_rssi {
    u16_t handle;
} __packed;
struct bt_hci_rp_read_rssi {
    u8_t status;
    u16_t handle;
    s8_t rssi;
} __packed;

#define BT_HCI_ENCRYPTION_KEY_SIZE_MIN 7
#define BT_HCI_ENCRYPTION_KEY_SIZE_MAX 16

#define BT_HCI_OP_READ_ENCRYPTION_KEY_SIZE BT_OP(BT_OGF_STATUS, 0x0008)
struct bt_hci_cp_read_encryption_key_size {
    u16_t handle;
} __packed;
struct bt_hci_rp_read_encryption_key_size {
    u8_t status;
    u16_t handle;
    u8_t key_size;
} __packed;

/* BLE */

#define BT_HCI_OP_LE_SET_EVENT_MASK BT_OP(BT_OGF_LE, 0x0001)
struct bt_hci_cp_le_set_event_mask {
    u8_t events[8];
} __packed;

#define BT_HCI_OP_LE_READ_BUFFER_SIZE BT_OP(BT_OGF_LE, 0x0002)
struct bt_hci_rp_le_read_buffer_size {
    u8_t status;
    u16_t le_max_len;
    u8_t le_max_num;
} __packed;

#define BT_HCI_OP_LE_READ_LOCAL_FEATURES BT_OP(BT_OGF_LE, 0x0003)
struct bt_hci_rp_le_read_local_features {
    u8_t status;
    u8_t features[8];
} __packed;

#define BT_HCI_OP_LE_SET_RANDOM_ADDRESS BT_OP(BT_OGF_LE, 0x0005)
struct bt_hci_cp_le_set_random_address {
    bt_addr_t bdaddr;
} __packed;

/* Advertising types */
#define BT_LE_ADV_IND                 0x00
#define BT_LE_ADV_DIRECT_IND          0x01
#define BT_LE_ADV_SCAN_IND            0x02
#define BT_LE_ADV_NONCONN_IND         0x03
#define BT_LE_ADV_DIRECT_IND_LOW_DUTY 0x04
/* Needed in advertising reports when getting info about */
#define BT_LE_ADV_SCAN_RSP 0x04

#define BT_LE_ADV_FP_NO_WHITELIST       0x00
#define BT_LE_ADV_FP_WHITELIST_SCAN_REQ 0x01
#define BT_LE_ADV_FP_WHITELIST_CONN_IND 0x02
#define BT_LE_ADV_FP_WHITELIST_BOTH     0x03

#define BT_HCI_OP_LE_SET_ADV_PARAM BT_OP(BT_OGF_LE, 0x0006)
struct bt_hci_cp_le_set_adv_param {
    u16_t min_interval;
    u16_t max_interval;
    u8_t type;
    u8_t own_addr_type;
    bt_addr_le_t direct_addr;
    u8_t channel_map;
    u8_t filter_policy;
} __packed;

#define BT_HCI_OP_LE_READ_ADV_CHAN_TX_POWER BT_OP(BT_OGF_LE, 0x0007)
struct bt_hci_rp_le_read_chan_tx_power {
    u8_t status;
    s8_t tx_power_level;
} __packed;

#define BT_HCI_OP_LE_SET_ADV_DATA BT_OP(BT_OGF_LE, 0x0008)
struct bt_hci_cp_le_set_adv_data {
    u8_t len;
    u8_t data[31];
} __packed;

#define BT_HCI_OP_LE_SET_SCAN_RSP_DATA BT_OP(BT_OGF_LE, 0x0009)
struct bt_hci_cp_le_set_scan_rsp_data {
    u8_t len;
    u8_t data[31];
} __packed;

#define BT_HCI_LE_ADV_DISABLE 0x00
#define BT_HCI_LE_ADV_ENABLE  0x01

#define BT_HCI_OP_LE_SET_ADV_ENABLE BT_OP(BT_OGF_LE, 0x000a)
struct bt_hci_cp_le_set_adv_enable {
    u8_t enable;
} __packed;

/* Scan types */
#define BT_HCI_OP_LE_SET_SCAN_PARAM BT_OP(BT_OGF_LE, 0x000b)
#define BT_HCI_LE_SCAN_PASSIVE      0x00
#define BT_HCI_LE_SCAN_ACTIVE       0x01

#define BT_HCI_LE_SCAN_FP_NO_WHITELIST  0x00
#define BT_HCI_LE_SCAN_FP_USE_WHITELIST 0x01

struct bt_hci_cp_le_set_scan_param {
    u8_t scan_type;
    u16_t interval;
    u16_t window;
    u8_t addr_type;
    u8_t filter_policy;
} __packed;

#define BT_HCI_OP_LE_SET_SCAN_ENABLE BT_OP(BT_OGF_LE, 0x000c)

#define BT_HCI_LE_SCAN_DISABLE 0x00
#define BT_HCI_LE_SCAN_ENABLE  0x01

#define BT_HCI_LE_SCAN_FILTER_DUP_DISABLE 0x00
#define BT_HCI_LE_SCAN_FILTER_DUP_ENABLE  0x01

struct bt_hci_cp_le_set_scan_enable {
    u8_t enable;
    u8_t filter_dup;
} __packed;

#define BT_HCI_OP_LE_CREATE_CONN BT_OP(BT_OGF_LE, 0x000d)

#define BT_HCI_LE_CREATE_CONN_FP_DIRECT    0x00
#define BT_HCI_LE_CREATE_CONN_FP_WHITELIST 0x01

struct bt_hci_cp_le_create_conn {
    u16_t scan_interval;
    u16_t scan_window;
    u8_t filter_policy;
    bt_addr_le_t peer_addr;
    u8_t own_addr_type;
    u16_t conn_interval_min;
    u16_t conn_interval_max;
    u16_t conn_latency;
    u16_t supervision_timeout;
    u16_t min_ce_len;
    u16_t max_ce_len;
} __packed;

#define BT_HCI_OP_LE_CREATE_CONN_CANCEL BT_OP(BT_OGF_LE, 0x000e)

#define BT_HCI_OP_LE_READ_WL_SIZE BT_OP(BT_OGF_LE, 0x000f)
struct bt_hci_rp_le_read_wl_size {
    u8_t status;
    u8_t wl_size;
} __packed;

#define BT_HCI_OP_LE_CLEAR_WL BT_OP(BT_OGF_LE, 0x0010)

#define BT_HCI_OP_LE_ADD_DEV_TO_WL BT_OP(BT_OGF_LE, 0x0011)
struct bt_hci_cp_le_add_dev_to_wl {
    bt_addr_le_t addr;
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_WL BT_OP(BT_OGF_LE, 0x0012)
struct bt_hci_cp_le_rem_dev_from_wl {
    bt_addr_le_t addr;
} __packed;

#define BT_HCI_OP_LE_CONN_UPDATE BT_OP(BT_OGF_LE, 0x0013)
struct hci_cp_le_conn_update {
    u16_t handle;
    u16_t conn_interval_min;
    u16_t conn_interval_max;
    u16_t conn_latency;
    u16_t supervision_timeout;
    u16_t min_ce_len;
    u16_t max_ce_len;
} __packed;

#define BT_HCI_OP_LE_SET_HOST_CHAN_CLASSIF BT_OP(BT_OGF_LE, 0x0014)
struct bt_hci_cp_le_set_host_chan_classif {
    u8_t ch_map[5];
} __packed;

#define BT_HCI_OP_LE_READ_CHAN_MAP BT_OP(BT_OGF_LE, 0x0015)
struct bt_hci_cp_le_read_chan_map {
    u16_t handle;
} __packed;
struct bt_hci_rp_le_read_chan_map {
    u8_t status;
    u16_t handle;
    u8_t ch_map[5];
} __packed;

#define BT_HCI_OP_LE_READ_REMOTE_FEATURES BT_OP(BT_OGF_LE, 0x0016)
struct bt_hci_cp_le_read_remote_features {
    u16_t handle;
} __packed;

#define BT_HCI_OP_LE_ENCRYPT BT_OP(BT_OGF_LE, 0x0017)
struct bt_hci_cp_le_encrypt {
    u8_t key[16];
    u8_t plaintext[16];
} __packed;
struct bt_hci_rp_le_encrypt {
    u8_t status;
    u8_t enc_data[16];
} __packed;

#define BT_HCI_OP_LE_RAND BT_OP(BT_OGF_LE, 0x0018)
struct bt_hci_rp_le_rand {
    u8_t status;
    u8_t rand[8];
} __packed;

#define BT_HCI_OP_LE_START_ENCRYPTION BT_OP(BT_OGF_LE, 0x0019)
struct bt_hci_cp_le_start_encryption {
    u16_t handle;
    u64_t rand;
    u16_t ediv;
    u8_t ltk[16];
} __packed;

#define BT_HCI_OP_LE_LTK_REQ_REPLY BT_OP(BT_OGF_LE, 0x001a)
struct bt_hci_cp_le_ltk_req_reply {
    u16_t handle;
    u8_t ltk[16];
} __packed;
struct bt_hci_rp_le_ltk_req_reply {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_OP_LE_LTK_REQ_NEG_REPLY BT_OP(BT_OGF_LE, 0x001b)
struct bt_hci_cp_le_ltk_req_neg_reply {
    u16_t handle;
} __packed;
struct bt_hci_rp_le_ltk_req_neg_reply {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_OP_LE_READ_SUPP_STATES BT_OP(BT_OGF_LE, 0x001c)
struct bt_hci_rp_le_read_supp_states {
    u8_t status;
    u8_t le_states[8];
} __packed;

#define BT_HCI_OP_LE_RX_TEST BT_OP(BT_OGF_LE, 0x001d)
struct bt_hci_cp_le_rx_test {
    u8_t rx_ch;
} __packed;

#define BT_HCI_OP_LE_TX_TEST BT_OP(BT_OGF_LE, 0x001e)
struct bt_hci_cp_le_tx_test {
    u8_t tx_ch;
    u8_t test_data_len;
    u8_t pkt_payload;
} __packed;

#define BT_HCI_OP_LE_TEST_END BT_OP(BT_OGF_LE, 0x001f)
struct bt_hci_rp_le_test_end {
    u8_t status;
    u16_t rx_pkt_count;
} __packed;

#define BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY BT_OP(BT_OGF_LE, 0x0020)
struct bt_hci_cp_le_conn_param_req_reply {
    u16_t handle;
    u16_t interval_min;
    u16_t interval_max;
    u16_t latency;
    u16_t timeout;
    u16_t min_ce_len;
    u16_t max_ce_len;
} __packed;
struct bt_hci_rp_le_conn_param_req_reply {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY BT_OP(BT_OGF_LE, 0x0021)
struct bt_hci_cp_le_conn_param_req_neg_reply {
    u16_t handle;
    u8_t reason;
} __packed;
struct bt_hci_rp_le_conn_param_req_neg_reply {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_OP_LE_SET_DATA_LEN BT_OP(BT_OGF_LE, 0x0022)
struct bt_hci_cp_le_set_data_len {
    u16_t handle;
    u16_t tx_octets;
    u16_t tx_time;
} __packed;
struct bt_hci_rp_le_set_data_len {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_OP_LE_READ_DEFAULT_DATA_LEN BT_OP(BT_OGF_LE, 0x0023)
struct bt_hci_rp_le_read_default_data_len {
    u8_t status;
    u16_t max_tx_octets;
    u16_t max_tx_time;
} __packed;

#define BT_HCI_OP_LE_WRITE_DEFAULT_DATA_LEN BT_OP(BT_OGF_LE, 0x0024)
struct bt_hci_cp_le_write_default_data_len {
    u16_t max_tx_octets;
    u16_t max_tx_time;
} __packed;

#define BT_HCI_OP_LE_P256_PUBLIC_KEY BT_OP(BT_OGF_LE, 0x0025)

#define BT_HCI_OP_LE_GENERATE_DHKEY BT_OP(BT_OGF_LE, 0x0026)
struct bt_hci_cp_le_generate_dhkey {
    u8_t key[64];
} __packed;

#define BT_HCI_OP_LE_ADD_DEV_TO_RL BT_OP(BT_OGF_LE, 0x0027)
struct bt_hci_cp_le_add_dev_to_rl {
    bt_addr_le_t peer_id_addr;
    u8_t peer_irk[16];
    u8_t local_irk[16];
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_RL BT_OP(BT_OGF_LE, 0x0028)
struct bt_hci_cp_le_rem_dev_from_rl {
    bt_addr_le_t peer_id_addr;
} __packed;

#define BT_HCI_OP_LE_CLEAR_RL BT_OP(BT_OGF_LE, 0x0029)

#define BT_HCI_OP_LE_READ_RL_SIZE BT_OP(BT_OGF_LE, 0x002a)
struct bt_hci_rp_le_read_rl_size {
    u8_t status;
    u8_t rl_size;
} __packed;

#define BT_HCI_OP_LE_READ_PEER_RPA BT_OP(BT_OGF_LE, 0x002b)
struct bt_hci_cp_le_read_peer_rpa {
    bt_addr_le_t peer_id_addr;
} __packed;
struct bt_hci_rp_le_read_peer_rpa {
    u8_t status;
    bt_addr_t peer_rpa;
} __packed;

#define BT_HCI_OP_LE_READ_LOCAL_RPA BT_OP(BT_OGF_LE, 0x002c)
struct bt_hci_cp_le_read_local_rpa {
    bt_addr_le_t peer_id_addr;
} __packed;
struct bt_hci_rp_le_read_local_rpa {
    u8_t status;
    bt_addr_t local_rpa;
} __packed;

#define BT_HCI_ADDR_RES_DISABLE 0x00
#define BT_HCI_ADDR_RES_ENABLE  0x01

#define BT_HCI_OP_LE_SET_ADDR_RES_ENABLE BT_OP(BT_OGF_LE, 0x002d)
struct bt_hci_cp_le_set_addr_res_enable {
    u8_t enable;
} __packed;

#define BT_HCI_OP_LE_SET_RPA_TIMEOUT BT_OP(BT_OGF_LE, 0x002e)
struct bt_hci_cp_le_set_rpa_timeout {
    u16_t rpa_timeout;
} __packed;

#define BT_HCI_OP_LE_READ_MAX_DATA_LEN BT_OP(BT_OGF_LE, 0x002f)
struct bt_hci_rp_le_read_max_data_len {
    u8_t status;
    u16_t max_tx_octets;
    u16_t max_tx_time;
    u16_t max_rx_octets;
    u16_t max_rx_time;
} __packed;

#define BT_HCI_LE_PHY_1M    0x01
#define BT_HCI_LE_PHY_2M    0x02
#define BT_HCI_LE_PHY_CODED 0x03

#define BT_HCI_OP_LE_READ_PHY BT_OP(BT_OGF_LE, 0x0030)
struct bt_hci_cp_le_read_phy {
    u16_t handle;
} __packed;
struct bt_hci_rp_le_read_phy {
    u8_t status;
    u16_t handle;
    u8_t tx_phy;
    u8_t rx_phy;
} __packed;

#define BT_HCI_LE_PHY_TX_ANY BIT(0)
#define BT_HCI_LE_PHY_RX_ANY BIT(1)

#define BT_HCI_LE_PHY_PREFER_1M    BIT(0)
#define BT_HCI_LE_PHY_PREFER_2M    BIT(1)
#define BT_HCI_LE_PHY_PREFER_CODED BIT(2)

#define BT_HCI_OP_LE_SET_DEFAULT_PHY BT_OP(BT_OGF_LE, 0x0031)
struct bt_hci_cp_le_set_default_phy {
    u8_t all_phys;
    u8_t tx_phys;
    u8_t rx_phys;
} __packed;

#define BT_HCI_LE_PHY_CODED_ANY 0x00
#define BT_HCI_LE_PHY_CODED_S2  0x01
#define BT_HCI_LE_PHY_CODED_S8  0x02

#define BT_HCI_OP_LE_SET_PHY BT_OP(BT_OGF_LE, 0x0032)
struct bt_hci_cp_le_set_phy {
    u16_t handle;
    u8_t all_phys;
    u8_t tx_phys;
    u8_t rx_phys;
    u16_t phy_opts;
} __packed;

#define BT_HCI_LE_MOD_INDEX_STANDARD 0x00
#define BT_HCI_LE_MOD_INDEX_STABLE   0x01

#define BT_HCI_OP_LE_ENH_RX_TEST BT_OP(BT_OGF_LE, 0x0033)
struct bt_hci_cp_le_enh_rx_test {
    u8_t rx_ch;
    u8_t phy;
    u8_t mod_index;
} __packed;

/* Extends BT_HCI_LE_PHY */
#define BT_HCI_LE_TX_PHY_CODED_S8 0x03
#define BT_HCI_LE_TX_PHY_CODED_S2 0x04

#define BT_HCI_OP_LE_ENH_TX_TEST BT_OP(BT_OGF_LE, 0x0034)
struct bt_hci_cp_le_enh_tx_test {
    u8_t tx_ch;
    u8_t test_data_len;
    u8_t pkt_payload;
    u8_t phy;
} __packed;

#define BT_HCI_OP_LE_SET_ADV_SET_RANDOM_ADDR BT_OP(BT_OGF_LE, 0x0035)
struct bt_hci_cp_le_set_adv_set_random_addr {
    u8_t handle;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_LE_ADV_PROP_CONN       BIT(0)
#define BT_HCI_LE_ADV_PROP_SCAN       BIT(1)
#define BT_HCI_LE_ADV_PROP_DIRECT     BIT(2)
#define BT_HCI_LE_ADV_PROP_HI_DC_CONN BIT(3)
#define BT_HCI_LE_ADV_PROP_LEGACY     BIT(4)
#define BT_HCI_LE_ADV_PROP_ANON       BIT(5)
#define BT_HCI_LE_ADV_PROP_TX_POWER   BIT(6)

#define BT_HCI_LE_ADV_SCAN_REQ_ENABLE  1
#define BT_HCI_LE_ADV_SCAN_REQ_DISABLE 0

#define BT_HCI_LE_ADV_TX_POWER_NO_PREF 0x7F

#define BT_HCI_LE_ADV_HANDLE_MAX 0xEF

#define BT_HCI_OP_LE_SET_EXT_ADV_PARAM BT_OP(BT_OGF_LE, 0x0036)
struct bt_hci_cp_le_set_ext_adv_param {
    u8_t handle;
    u16_t props;
    u8_t prim_min_interval[3];
    u8_t prim_max_interval[3];
    u8_t prim_channel_map;
    u8_t own_addr_type;
    bt_addr_le_t peer_addr;
    u8_t filter_policy;
    s8_t tx_power;
    u8_t prim_adv_phy;
    u8_t sec_adv_max_skip;
    u8_t sec_adv_phy;
    u8_t sid;
    u8_t scan_req_notify_enable;
} __packed;
struct bt_hci_rp_le_set_ext_adv_param {
    u8_t status;
    s8_t tx_power;
} __packed;

#define BT_HCI_LE_EXT_ADV_OP_INTERM_FRAG    0x00
#define BT_HCI_LE_EXT_ADV_OP_FIRST_FRAG     0x01
#define BT_HCI_LE_EXT_ADV_OP_LAST_FRAG      0x02
#define BT_HCI_LE_EXT_ADV_OP_COMPLETE_DATA  0x03
#define BT_HCI_LE_EXT_ADV_OP_UNCHANGED_DATA 0x04

#define BT_HCI_LE_EXT_ADV_FRAG_ENABLED  0x00
#define BT_HCI_LE_EXT_ADV_FRAG_DISABLED 0x01

#define BT_HCI_LE_EXT_ADV_FRAG_MAX_LEN 251

#define BT_HCI_OP_LE_SET_EXT_ADV_DATA BT_OP(BT_OGF_LE, 0x0037)
struct bt_hci_cp_le_set_ext_adv_data {
    u8_t handle;
    u8_t op;
    u8_t frag_pref;
    u8_t len;
    u8_t data[251];
} __packed;

#define BT_HCI_OP_LE_SET_EXT_SCAN_RSP_DATA BT_OP(BT_OGF_LE, 0x0038)
struct bt_hci_cp_le_set_ext_scan_rsp_data {
    u8_t handle;
    u8_t op;
    u8_t frag_pref;
    u8_t len;
    u8_t data[251];
} __packed;

#define BT_HCI_OP_LE_SET_EXT_ADV_ENABLE BT_OP(BT_OGF_LE, 0x0039)
struct bt_hci_ext_adv_set {
    u8_t handle;
    u16_t duration;
    u8_t max_ext_adv_evts;
} __packed;

struct bt_hci_cp_le_set_ext_adv_enable {
    u8_t enable;
    u8_t set_num;
    struct bt_hci_ext_adv_set s[0];
} __packed;

#define BT_HCI_OP_LE_READ_MAX_ADV_DATA_LEN BT_OP(BT_OGF_LE, 0x003a)
struct bt_hci_rp_le_read_max_adv_data_len {
    u8_t status;
    u16_t max_adv_data_len;
} __packed;

#define BT_HCI_OP_LE_READ_NUM_ADV_SETS BT_OP(BT_OGF_LE, 0x003b)
struct bt_hci_rp_le_read_num_adv_sets {
    u8_t status;
    u8_t num_sets;
} __packed;

#define BT_HCI_OP_LE_REMOVE_ADV_SET BT_OP(BT_OGF_LE, 0x003c)
struct bt_hci_cp_le_remove_adv_set {
    u8_t handle;
} __packed;

#define BT_HCI_OP_CLEAR_ADV_SETS BT_OP(BT_OGF_LE, 0x003d)

#define BT_HCI_OP_LE_SET_PER_ADV_PARAM BT_OP(BT_OGF_LE, 0x003e)
struct bt_hci_cp_le_set_per_adv_param {
    u8_t handle;
    u16_t min_interval;
    u16_t max_interval;
    u16_t props;
} __packed;

#define BT_HCI_LE_PER_ADV_OP_INTERM_FRAG   0x00
#define BT_HCI_LE_PER_ADV_OP_FIRST_FRAG    0x01
#define BT_HCI_LE_PER_ADV_OP_LAST_FRAG     0x02
#define BT_HCI_LE_PER_ADV_OP_COMPLETE_DATA 0x03

#define BT_HCI_LE_PER_ADV_FRAG_MAX_LEN 252

#define BT_HCI_OP_LE_SET_PER_ADV_DATA BT_OP(BT_OGF_LE, 0x003f)
struct bt_hci_cp_le_set_per_adv_data {
    u8_t handle;
    u8_t op;
    u8_t len;
    u8_t data[251];
} __packed;

#define BT_HCI_OP_LE_SET_PER_ADV_ENABLE BT_OP(BT_OGF_LE, 0x0040)
struct bt_hci_cp_le_set_per_adv_enable {
    u8_t enable;
    u8_t handle;
} __packed;

#define BT_HCI_OP_LE_SET_EXT_SCAN_PARAM BT_OP(BT_OGF_LE, 0x0041)
struct bt_hci_ext_scan_phy {
    u8_t type;
    u16_t interval;
    u16_t window;
} __packed;

#define BT_HCI_LE_EXT_SCAN_PHY_1M    BIT(0)
#define BT_HCI_LE_EXT_SCAN_PHY_2M    BIT(1)
#define BT_HCI_LE_EXT_SCAN_PHY_CODED BIT(2)

struct bt_hci_cp_le_set_ext_scan_param {
    u8_t own_addr_type;
    u8_t filter_policy;
    u8_t phys;
    struct bt_hci_ext_scan_phy p[0];
} __packed;

/* Extends BT_HCI_LE_SCAN_FILTER_DUP */
#define BT_HCI_LE_EXT_SCAN_FILTER_DUP_ENABLE_RESET 0x02

#define BT_HCI_OP_LE_SET_EXT_SCAN_ENABLE BT_OP(BT_OGF_LE, 0x0042)
struct bt_hci_cp_le_set_ext_scan_enable {
    u8_t enable;
    u8_t filter_dup;
    u16_t duration;
    u16_t period;
} __packed;

#define BT_HCI_OP_LE_EXT_CREATE_CONN BT_OP(BT_OGF_LE, 0x0043)
struct bt_hci_ext_conn_phy {
    u16_t interval;
    u16_t window;
    u16_t conn_interval_min;
    u16_t conn_interval_max;
    u16_t conn_latency;
    u16_t supervision_timeout;
    u16_t min_ce_len;
    u16_t max_ce_len;
} __packed;

struct bt_hci_cp_le_ext_create_conn {
    u8_t filter_policy;
    u8_t own_addr_type;
    bt_addr_le_t peer_addr;
    u8_t phys;
    struct bt_hci_ext_conn_phy p[0];
} __packed;

#define BT_HCI_LE_PER_ADV_CREATE_SYNC_FP_USE_LIST         BIT(0)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_FP_REPORTS_DISABLED BIT(1)

#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_AOA     BIT(0)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_AOD_1US BIT(1)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_AOD_2US BIT(2)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_NO_CTE     BIT(3)
#define BT_HCI_LE_PER_ADV_CREATE_SYNC_CTE_TYPE_ONLY_CTE   BIT(4)

#define BT_HCI_OP_LE_PER_ADV_CREATE_SYNC BT_OP(BT_OGF_LE, 0x0044)
struct bt_hci_cp_le_per_adv_create_sync {
    u8_t filter_policy;
    u8_t sid;
    bt_addr_le_t addr;
    u16_t skip;
    u16_t sync_timeout;
    u8_t unused;
} __packed;

#define BT_HCI_OP_LE_PER_ADV_CREATE_SYNC_CANCEL BT_OP(BT_OGF_LE, 0x0045)

#define BT_HCI_OP_LE_PER_ADV_TERMINATE_SYNC BT_OP(BT_OGF_LE, 0x0046)
struct bt_hci_cp_le_per_adv_terminate_sync {
    u16_t handle;
} __packed;

#define BT_HCI_OP_LE_ADD_DEV_TO_PER_ADV_LIST BT_OP(BT_OGF_LE, 0x0047)
struct bt_hci_cp_le_add_dev_to_per_adv_list {
    bt_addr_le_t addr;
    u8_t sid;
} __packed;

#define BT_HCI_OP_LE_REM_DEV_FROM_PER_ADV_LIST BT_OP(BT_OGF_LE, 0x0048)
struct bt_hci_cp_le_rem_dev_from_per_adv_list {
    bt_addr_le_t addr;
    u8_t sid;
} __packed;

#define BT_HCI_OP_LE_CLEAR_PER_ADV_LIST BT_OP(BT_OGF_LE, 0x0049)

#define BT_HCI_OP_LE_READ_PER_ADV_LIST_SIZE BT_OP(BT_OGF_LE, 0x004a)
struct bt_hci_rp_le_read_per_adv_list_size {
    u8_t status;
    u8_t list_size;
} __packed;

#define BT_HCI_OP_LE_READ_TX_POWER BT_OP(BT_OGF_LE, 0x004b)
struct bt_hci_rp_le_read_tx_power {
    u8_t status;
    s8_t min_tx_power;
    s8_t max_tx_power;
} __packed;

#define BT_HCI_OP_LE_READ_RF_PATH_COMP BT_OP(BT_OGF_LE, 0x004c)
struct bt_hci_rp_le_read_rf_path_comp {
    u8_t status;
    s16_t tx_path_comp;
    s16_t rx_path_comp;
} __packed;

#define BT_HCI_OP_LE_WRITE_RF_PATH_COMP BT_OP(BT_OGF_LE, 0x004d)
struct bt_hci_cp_le_write_rf_path_comp {
    s16_t tx_path_comp;
    s16_t rx_path_comp;
} __packed;

#define BT_HCI_LE_PRIVACY_MODE_NETWORK 0x00
#define BT_HCI_LE_PRIVACY_MODE_DEVICE  0x01

#define BT_HCI_OP_LE_SET_PRIVACY_MODE BT_OP(BT_OGF_LE, 0x004e)
struct bt_hci_cp_le_set_privacy_mode {
    bt_addr_le_t id_addr;
    u8_t mode;
} __packed;

#define BT_HCI_OP_LE_SET_CL_CTE_TX_ENABLE BT_OP(BT_OGF_LE, 0x0052)
struct bt_hci_cp_le_set_cl_cte_tx_enable {
    uint8_t handle;
    uint8_t cte_enable;
} __packed;

/* Min and max Constant Tone Extension length in 8us units */
#define BT_HCI_LE_CTE_LEN_MIN 0x2
#define BT_HCI_LE_CTE_LEN_MAX 0x14

#define BT_HCI_LE_AOA_CTE     0x1
#define BT_HCI_LE_AOD_CTE_1US 0x2
#define BT_HCI_LE_AOD_CTE_2US 0x3

#define BT_HCI_LE_CTE_COUNT_MIN 0x1
#define BT_HCI_LE_CTE_COUNT_MAX 0x10

#define BT_HCI_OP_LE_SET_CL_CTE_TX_PARAMS BT_OP(BT_OGF_LE, 0x0051)
struct bt_hci_cp_le_set_cl_cte_tx_params {
    uint8_t handle;
    uint8_t cte_len;
    uint8_t cte_type;
    uint8_t cte_count;
    uint8_t switch_pattern_len;
    uint8_t ant_ids[0];
} __packed;

#define BT_HCI_LE_AOA_CTE_RSP     BIT(0)
#define BT_HCI_LE_AOD_CTE_RSP_1US BIT(1)
#define BT_HCI_LE_AOD_CTE_RSP_2US BIT(2)

#define BT_HCI_LE_SWITCH_PATTERN_LEN_MIN 0x2
#define BT_HCI_LE_SWITCH_PATTERN_LEN_MAX 0x4B

#define BT_HCI_OP_LE_SET_CONN_CTE_TX_PARAMS BT_OP(BT_OGF_LE, 0x0055)
struct bt_hci_cp_le_set_conn_cte_tx_params {
    uint16_t handle;
    uint8_t cte_types;
    uint8_t switch_pattern_len;
    uint8_t ant_id[0];
} __packed;

struct bt_hci_rp_le_set_conn_cte_tx_params {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_LE_1US_AOD_TX BIT(0)
#define BT_HCI_LE_1US_AOD_RX BIT(1)
#define BT_HCI_LE_1US_AOA_RX BIT(2)

#define BT_HCI_LE_NUM_ANT_MIN 0x1
#define BT_HCI_LE_NUM_ANT_MAX 0x4B

#define BT_HCI_LE_MAX_SWITCH_PATTERN_LEN_MIN 0x2
#define BT_HCI_LE_MAX_SWITCH_PATTERN_LEN_MAX 0x4B

#define BT_HCI_LE_MAX_CTE_LEN_MIN 0x2
#define BT_HCI_LE_MAX_CTE_LEN_MAX 0x14

#define BT_HCI_OP_LE_READ_ANT_INFO BT_OP(BT_OGF_LE, 0x0058)
struct bt_hci_rp_le_read_ant_info {
    uint8_t status;
    uint8_t switch_sample_rates;
    uint8_t num_ant;
    uint8_t max_switch_pattern_len;
    uint8_t max_cte_len;
};

#define BT_HCI_OP_LE_SET_PER_ADV_RECV_ENABLE BT_OP(BT_OGF_LE, 0x0059)
struct bt_hci_cp_le_set_per_adv_recv_enable {
    uint16_t handle;
    uint8_t enable;
} __packed;

#define BT_HCI_OP_LE_PER_ADV_SYNC_TRANSFER BT_OP(BT_OGF_LE, 0x005a)
struct bt_hci_cp_le_per_adv_sync_transfer {
    uint16_t conn_handle;
    uint16_t service_data;
    uint16_t sync_handle;
} __packed;

struct bt_hci_rp_le_per_adv_sync_transfer {
    uint8_t status;
    uint16_t conn_handle;
} __packed;

#define BT_HCI_OP_LE_PER_ADV_SET_INFO_TRANSFER BT_OP(BT_OGF_LE, 0x005b)
struct bt_hci_cp_le_per_adv_set_info_transfer {
    uint16_t conn_handle;
    uint16_t service_data;
    uint8_t adv_handle;
} __packed;

struct bt_hci_rp_le_per_adv_set_info_transfer {
    uint8_t status;
    uint16_t conn_handle;
} __packed;

#define BT_HCI_LE_PAST_MODE_NO_SYNC    0x00
#define BT_HCI_LE_PAST_MODE_NO_REPORTS 0x01
#define BT_HCI_LE_PAST_MODE_SYNC       0x02

#define BT_HCI_LE_PAST_CTE_TYPE_NO_AOA     BIT(0)
#define BT_HCI_LE_PAST_CTE_TYPE_NO_AOD_1US BIT(1)
#define BT_HCI_LE_PAST_CTE_TYPE_NO_AOD_2US BIT(2)
#define BT_HCI_LE_PAST_CTE_TYPE_NO_CTE     BIT(3)
#define BT_HCI_LE_PAST_CTE_TYPE_ONLY_CTE   BIT(4)

#define BT_HCI_OP_LE_PAST_PARAM BT_OP(BT_OGF_LE, 0x005c)
struct bt_hci_cp_le_past_param {
    uint16_t conn_handle;
    uint8_t mode;
    uint16_t skip;
    uint16_t timeout;
    uint8_t cte_type;
} __packed;

struct bt_hci_rp_le_past_param {
    uint8_t status;
    uint16_t conn_handle;
} __packed;

#define BT_HCI_OP_LE_DEFAULT_PAST_PARAM BT_OP(BT_OGF_LE, 0x005d)
struct bt_hci_cp_le_default_past_param {
    uint8_t mode;
    uint16_t skip;
    uint16_t timeout;
    uint8_t cte_type;
} __packed;

struct bt_hci_rp_le_default_past_param {
    uint8_t status;
} __packed;

#define BT_HCI_OP_LE_READ_BUFFER_SIZE_V2 BT_OP(BT_OGF_LE, 0x0060)
struct bt_hci_rp_le_read_buffer_size_v2 {
    uint8_t status;
    uint16_t acl_mtu;
    uint8_t acl_max_pkt;
    uint16_t iso_mtu;
    uint8_t iso_max_pkt;
} __packed;

#define BT_HCI_OP_LE_READ_ISO_TX_SYNC BT_OP(BT_OGF_LE, 0x0061)
struct bt_hci_cp_le_read_iso_tx_sync {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_read_iso_tx_sync {
    uint8_t status;
    uint16_t handle;
    uint16_t seq;
    uint32_t timestamp;
    uint8_t offset[3];
} __packed;

#define BT_HCI_OP_LE_SET_CIG_PARAMS BT_OP(BT_OGF_LE, 0x0062)
struct bt_hci_cis_params {
    uint8_t cis_id;
    uint16_t m_sdu;
    uint16_t s_sdu;
    uint8_t m_phy;
    uint8_t s_phy;
    uint8_t m_rtn;
    uint8_t s_rtn;
} __packed;

struct bt_hci_cp_le_set_cig_params {
    uint8_t cig_id;
    uint8_t m_interval[3];
    uint8_t s_interval[3];
    uint8_t sca;
    uint8_t packing;
    uint8_t framing;
    uint16_t m_latency;
    uint16_t s_latency;
    uint8_t num_cis;
    struct bt_hci_cis_params cis[0];
} __packed;

struct bt_hci_rp_le_set_cig_params {
    uint8_t status;
    uint8_t cig_id;
    uint8_t num_handles;
    uint16_t handle[0];
} __packed;

#define BT_HCI_OP_LE_SET_CIG_PARAMS_TEST BT_OP(BT_OGF_LE, 0x0063)
struct bt_hci_cis_params_test {
    uint8_t cis_id;
    uint8_t nse;
    uint16_t m_sdu;
    uint16_t s_sdu;
    uint16_t m_pdu;
    uint16_t s_pdu;
    uint8_t m_phy;
    uint8_t s_phy;
    uint8_t m_bn;
    uint8_t s_bn;
} __packed;

struct bt_hci_cp_le_set_cig_params_test {
    uint8_t cig_id;
    uint8_t m_interval[3];
    uint8_t s_interval[3];
    uint8_t m_ft;
    uint8_t s_ft;
    uint16_t iso_interval;
    uint8_t sca;
    uint8_t packing;
    uint8_t framing;
    uint8_t num_cis;
    struct bt_hci_cis_params_test cis[0];
} __packed;

struct bt_hci_rp_le_set_cig_params_test {
    uint8_t status;
    uint8_t cig_id;
    uint8_t num_handles;
    uint16_t handle[0];
} __packed;

#define BT_HCI_OP_LE_CREATE_CIS BT_OP(BT_OGF_LE, 0x0064)
struct bt_hci_cis {
    uint16_t cis_handle;
    uint16_t acl_handle;
} __packed;

struct bt_hci_cp_le_create_cis {
    uint8_t num_cis;
    struct bt_hci_cis cis[0];
} __packed;

#define BT_HCI_OP_LE_REMOVE_CIG BT_OP(BT_OGF_LE, 0x0065)
struct bt_hci_cp_le_remove_cig {
    uint8_t cig_id;
} __packed;

struct bt_hci_rp_le_remove_cig {
    uint8_t status;
    uint8_t cig_id;
} __packed;

#define BT_HCI_OP_LE_ACCEPT_CIS BT_OP(BT_OGF_LE, 0x0066)
struct bt_hci_cp_le_accept_cis {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_REJECT_CIS BT_OP(BT_OGF_LE, 0x0067)
struct bt_hci_cp_le_reject_cis {
    uint16_t handle;
    uint8_t reason;
} __packed;

struct bt_hci_rp_le_reject_cis {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_CREATE_BIG BT_OP(BT_OGF_LE, 0x0068)
struct bt_hci_cp_le_create_big {
    uint8_t big_handle;
    uint8_t adv_handle;
    uint8_t num_bis;
    uint8_t sdu_interval[3];
    uint16_t max_sdu;
    uint16_t max_latency;
    uint8_t rtn;
    uint8_t phy;
    uint8_t packing;
    uint8_t framing;
    uint8_t encryption;
    uint8_t bcode[16];
} __packed;

#define BT_HCI_OP_LE_CREATE_BIG_TEST BT_OP(BT_OGF_LE, 0x0069)
struct bt_hci_cp_le_create_big_test {
    uint8_t big_handle;
    uint8_t adv_handle;
    uint8_t num_bis;
    uint8_t sdu_interval[3];
    uint16_t iso_interval;
    uint8_t nse;
    uint16_t max_sdu;
    uint16_t max_pdu;
    uint8_t phy;
    uint8_t packing;
    uint8_t framing;
    uint8_t bn;
    uint8_t irc;
    uint8_t pto;
    uint8_t encryption;
    uint8_t bcode[16];
} __packed;

#define BT_HCI_OP_LE_TERMINATE_BIG BT_OP(BT_OGF_LE, 0x006a)
struct bt_hci_cp_le_terminate_big {
    uint8_t big_handle;
    uint8_t reason;
} __packed;

#define BT_HCI_OP_LE_BIG_CREATE_SYNC BT_OP(BT_OGF_LE, 0x006b)
struct bt_hci_cp_le_big_create_sync {
    uint8_t big_handle;
    uint16_t sync_handle;
    uint8_t encryption;
    uint8_t bcode[16];
    uint8_t mse;
    uint16_t sync_timeout;
    uint8_t num_bis;
    uint8_t bis[0];
} __packed;

#define BT_HCI_OP_LE_BIG_TERMINATE_SYNC BT_OP(BT_OGF_LE, 0x006c)
struct bt_hci_cp_le_big_terminate_sync {
    uint8_t big_handle;
} __packed;

struct bt_hci_rp_le_big_terminate_sync {
    uint8_t status;
    uint8_t big_handle;
} __packed;

#define BT_HCI_OP_LE_REQ_PEER_SC BT_OP(BT_OGF_LE, 0x006d)
struct bt_hci_cp_le_req_peer_sca {
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_SETUP_ISO_PATH BT_OP(BT_OGF_LE, 0x006e)
struct bt_hci_cp_le_setup_iso_path {
    uint16_t handle;
    uint8_t path_dir;
    uint8_t path_id;
    struct bt_hci_cp_codec_id codec_id;
    uint8_t controller_delay[3];
    uint8_t codec_config_len;
    uint8_t codec_config[0];
} __packed;

struct bt_hci_rp_le_setup_iso_path {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_REMOVE_ISO_PATH BT_OP(BT_OGF_LE, 0x006f)
struct bt_hci_cp_le_remove_iso_path {
    uint16_t handle;
    uint8_t path_dir;
} __packed;

struct bt_hci_rp_le_remove_iso_path {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ISO_TRANSMIT_TEST BT_OP(BT_OGF_LE, 0x0070)
struct bt_hci_cp_le_iso_transmit_test {
    uint16_t handle;
    uint8_t payload_type;
} __packed;

struct bt_hci_rp_le_iso_transmit_test {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ISO_RECEIVE_TEST BT_OP(BT_OGF_LE, 0x0071)
struct bt_hci_cp_le_iso_receive_test {
    uint16_t handle;
    uint8_t payload_type;
} __packed;

struct bt_hci_rp_le_iso_receive_test {
    uint8_t status;
    uint16_t handle;
} __packed;

#define BT_HCI_OP_LE_ISO_READ_TEST_COUNTERS BT_OP(BT_OGF_LE, 0x0072)
struct bt_hci_cp_le_read_test_counters {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_read_test_counters {
    uint8_t status;
    uint16_t handle;
    uint32_t received_cnt;
    uint32_t missed_cnt;
    uint32_t failed_cnt;
} __packed;

#define BT_HCI_OP_LE_ISO_TEST_END BT_OP(BT_OGF_LE, 0x0073)
struct bt_hci_cp_le_iso_test_end {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_iso_test_end {
    uint8_t status;
    uint16_t handle;
    uint32_t received_cnt;
    uint32_t missed_cnt;
    uint32_t failed_cnt;
} __packed;

#define BT_HCI_OP_LE_SET_HOST_FEATURE BT_OP(BT_OGF_LE, 0x0074)
struct bt_hci_cp_le_set_host_feature {
    uint8_t bit_number;
    uint8_t bit_value;
} __packed;

struct bt_hci_rp_le_set_host_feature {
    uint8_t status;
} __packed;

#define BT_HCI_OP_LE_READ_ISO_LINK_QUALITY BT_OP(BT_OGF_LE, 0x0075)
struct bt_hci_cp_le_read_iso_link_quality {
    uint16_t handle;
} __packed;

struct bt_hci_rp_le_read_iso_link_quality {
    uint8_t status;
    uint16_t handle;
    uint32_t tx_unacked_packets;
    uint32_t tx_flushed_packets;
    uint32_t tx_last_subevent_packets;
    uint32_t retransmitted_packets;
    uint32_t crc_error_packets;
    uint32_t rx_unreceived_packets;
    uint32_t duplicate_packets;
} __packed;

/* Event definitions */
#if defined(BFLB_BLE)
#define BT_HCI_EVT_CC_PARAM_OFFSET 0x05
#define BT_HCI_CCEVT_HDR_PARLEN    0x03
#define BT_HCI_CSEVT_LEN           0x06
#define BT_HCI_CSVT_PARLEN         0x04
#define BT_HCI_EVT_LE_PARAM_OFFSET 0x02
#endif

#define BT_HCI_EVT_UNKNOWN 0x00
#define BT_HCI_EVT_VENDOR  0xff

#define BT_HCI_EVT_INQUIRY_COMPLETE 0x01
struct bt_hci_evt_inquiry_complete {
    u8_t status;
} __packed;

#define BT_HCI_EVT_CONN_COMPLETE 0x03
struct bt_hci_evt_conn_complete {
    u8_t status;
    u16_t handle;
    bt_addr_t bdaddr;
    u8_t link_type;
    u8_t encr_enabled;
} __packed;

#define BT_HCI_EVT_CONN_REQUEST 0x04
struct bt_hci_evt_conn_request {
    bt_addr_t bdaddr;
    u8_t dev_class[3];
    u8_t link_type;
} __packed;

#define BT_HCI_EVT_DISCONN_COMPLETE 0x05
struct bt_hci_evt_disconn_complete {
    u8_t status;
    u16_t handle;
    u8_t reason;
} __packed;

#define BT_HCI_EVT_AUTH_COMPLETE 0x06
struct bt_hci_evt_auth_complete {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_EVT_REMOTE_NAME_REQ_COMPLETE 0x07
struct bt_hci_evt_remote_name_req_complete {
    u8_t status;
    bt_addr_t bdaddr;
    u8_t name[248];
} __packed;

#define BT_HCI_EVT_ENCRYPT_CHANGE 0x08
struct bt_hci_evt_encrypt_change {
    u8_t status;
    u16_t handle;
    u8_t encrypt;
} __packed;

#define BT_HCI_EVT_REMOTE_FEATURES 0x0b
struct bt_hci_evt_remote_features {
    u8_t status;
    u16_t handle;
    u8_t features[8];
} __packed;

#define BT_HCI_EVT_REMOTE_VERSION_INFO 0x0c
struct bt_hci_evt_remote_version_info {
    u8_t status;
    u16_t handle;
    u8_t version;
    u16_t manufacturer;
    u16_t subversion;
} __packed;

#define BT_HCI_EVT_CMD_COMPLETE 0x0e
struct bt_hci_evt_cmd_complete {
    u8_t ncmd;
    u16_t opcode;
} __packed;

struct bt_hci_evt_cc_status {
    u8_t status;
} __packed;

#define BT_HCI_EVT_CMD_STATUS 0x0f
struct bt_hci_evt_cmd_status {
    u8_t status;
    u8_t ncmd;
    u16_t opcode;
} __packed;

#define BT_HCI_EVT_HARDWARE_ERROR 0x10
struct bt_hci_evt_hardware_error {
    uint8_t hardware_code;
} __packed;

#define BT_HCI_EVT_ROLE_CHANGE 0x12
struct bt_hci_evt_role_change {
    u8_t status;
    bt_addr_t bdaddr;
    u8_t role;
} __packed;

#define BT_HCI_EVT_NUM_COMPLETED_PACKETS 0x13
struct bt_hci_evt_num_completed_packets {
    u8_t num_handles;
    struct bt_hci_handle_count h[0];
} __packed;

#define BT_HCI_EVT_PIN_CODE_REQ 0x16
struct bt_hci_evt_pin_code_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_LINK_KEY_REQ 0x17
struct bt_hci_evt_link_key_req {
    bt_addr_t bdaddr;
} __packed;

/* Link Key types */
#define BT_LK_COMBINATION             0x00
#define BT_LK_LOCAL_UNIT              0x01
#define BT_LK_REMOTE_UNIT             0x02
#define BT_LK_DEBUG_COMBINATION       0x03
#define BT_LK_UNAUTH_COMBINATION_P192 0x04
#define BT_LK_AUTH_COMBINATION_P192   0x05
#define BT_LK_CHANGED_COMBINATION     0x06
#define BT_LK_UNAUTH_COMBINATION_P256 0x07
#define BT_LK_AUTH_COMBINATION_P256   0x08

#define BT_HCI_EVT_LINK_KEY_NOTIFY 0x18
struct bt_hci_evt_link_key_notify {
    bt_addr_t bdaddr;
    u8_t link_key[16];
    u8_t key_type;
} __packed;

/* Overflow link types */
#define BT_OVERFLOW_LINK_SYNCH 0x00
#define BT_OVERFLOW_LINK_ACL   0x01

#define BT_HCI_EVT_DATA_BUF_OVERFLOW 0x1a
struct bt_hci_evt_data_buf_overflow {
    u8_t link_type;
} __packed;

#define BT_HCI_EVT_INQUIRY_RESULT_WITH_RSSI 0x22
struct bt_hci_evt_inquiry_result_with_rssi {
    bt_addr_t addr;
    u8_t pscan_rep_mode;
    u8_t reserved;
    u8_t cod[3];
    u16_t clock_offset;
    s8_t rssi;
} __packed;

#define BT_HCI_EVT_REMOTE_EXT_FEATURES 0x23
struct bt_hci_evt_remote_ext_features {
    u8_t status;
    u16_t handle;
    u8_t page;
    u8_t max_page;
    u8_t features[8];
} __packed;

#define BT_HCI_EVT_SYNC_CONN_COMPLETE 0x2c
struct bt_hci_evt_sync_conn_complete {
    u8_t status;
    u16_t handle;
    bt_addr_t bdaddr;
    u8_t link_type;
    u8_t tx_interval;
    u8_t retansmission_window;
    u16_t rx_pkt_length;
    u16_t tx_pkt_length;
    u8_t air_mode;
} __packed;

#define BT_HCI_EVT_EXTENDED_INQUIRY_RESULT 0x2f
struct bt_hci_evt_extended_inquiry_result {
    u8_t num_reports;
    bt_addr_t addr;
    u8_t pscan_rep_mode;
    u8_t reserved;
    u8_t cod[3];
    u16_t clock_offset;
    s8_t rssi;
    u8_t eir[240];
} __packed;

#define BT_HCI_EVT_ENCRYPT_KEY_REFRESH_COMPLETE 0x30
struct bt_hci_evt_encrypt_key_refresh_complete {
    u8_t status;
    u16_t handle;
} __packed;

#define BT_HCI_EVT_IO_CAPA_REQ 0x31
struct bt_hci_evt_io_capa_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_IO_CAPA_RESP 0x32
struct bt_hci_evt_io_capa_resp {
    bt_addr_t bdaddr;
    u8_t capability;
    u8_t oob_data;
    u8_t authentication;
} __packed;

#define BT_HCI_EVT_USER_CONFIRM_REQ 0x33
struct bt_hci_evt_user_confirm_req {
    bt_addr_t bdaddr;
    u32_t passkey;
} __packed;

#define BT_HCI_EVT_USER_PASSKEY_REQ 0x34
struct bt_hci_evt_user_passkey_req {
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_SSP_COMPLETE 0x36
struct bt_hci_evt_ssp_complete {
    u8_t status;
    bt_addr_t bdaddr;
} __packed;

#define BT_HCI_EVT_USER_PASSKEY_NOTIFY 0x3b
struct bt_hci_evt_user_passkey_notify {
    bt_addr_t bdaddr;
    u32_t passkey;
} __packed;

#define BT_HCI_EVT_LE_META_EVENT 0x3e
struct bt_hci_evt_le_meta_event {
    u8_t subevent;
} __packed;

#define BT_HCI_EVT_AUTH_PAYLOAD_TIMEOUT_EXP 0x57
struct bt_hci_evt_auth_payload_timeout_exp {
    u16_t handle;
} __packed;

#define BT_HCI_ROLE_MASTER 0x00
#define BT_HCI_ROLE_SLAVE  0x01

#define BT_HCI_EVT_LE_CONN_COMPLETE 0x01
struct bt_hci_evt_le_conn_complete {
    u8_t status;
    u16_t handle;
    u8_t role;
    bt_addr_le_t peer_addr;
    u16_t interval;
    u16_t latency;
    u16_t supv_timeout;
    u8_t clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_ADVERTISING_REPORT 0x02
struct bt_hci_evt_le_advertising_info {
    u8_t evt_type;
    bt_addr_le_t addr;
    u8_t length;
    u8_t data[0];
} __packed;
struct bt_hci_evt_le_advertising_report {
    u8_t num_reports;
    struct bt_hci_evt_le_advertising_info adv_info[0];
} __packed;

#define BT_HCI_EVT_LE_CONN_UPDATE_COMPLETE 0x03
struct bt_hci_evt_le_conn_update_complete {
    u8_t status;
    u16_t handle;
    u16_t interval;
    u16_t latency;
    u16_t supv_timeout;
} __packed;

#define BT_HCI_EV_LE_REMOTE_FEAT_COMPLETE 0x04
struct bt_hci_evt_le_remote_feat_complete {
    u8_t status;
    u16_t handle;
    u8_t features[8];
} __packed;

#define BT_HCI_EVT_LE_LTK_REQUEST 0x05
struct bt_hci_evt_le_ltk_request {
    u16_t handle;
    u64_t rand;
    u16_t ediv;
} __packed;

#define BT_HCI_EVT_LE_CONN_PARAM_REQ 0x06
struct bt_hci_evt_le_conn_param_req {
    u16_t handle;
    u16_t interval_min;
    u16_t interval_max;
    u16_t latency;
    u16_t timeout;
} __packed;

#define BT_HCI_EVT_LE_DATA_LEN_CHANGE 0x07
struct bt_hci_evt_le_data_len_change {
    u16_t handle;
    u16_t max_tx_octets;
    u16_t max_tx_time;
    u16_t max_rx_octets;
    u16_t max_rx_time;
} __packed;

#define BT_HCI_EVT_LE_P256_PUBLIC_KEY_COMPLETE 0x08
struct bt_hci_evt_le_p256_public_key_complete {
    u8_t status;
    u8_t key[64];
} __packed;

#define BT_HCI_EVT_LE_GENERATE_DHKEY_COMPLETE 0x09
struct bt_hci_evt_le_generate_dhkey_complete {
    u8_t status;
    u8_t dhkey[32];
} __packed;

#define BT_HCI_EVT_LE_ENH_CONN_COMPLETE 0x0a
struct bt_hci_evt_le_enh_conn_complete {
    u8_t status;
    u16_t handle;
    u8_t role;
    bt_addr_le_t peer_addr;
    bt_addr_t local_rpa;
    bt_addr_t peer_rpa;
    u16_t interval;
    u16_t latency;
    u16_t supv_timeout;
    u8_t clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_DIRECT_ADV_REPORT 0x0b
struct bt_hci_evt_le_direct_adv_info {
    u8_t evt_type;
    bt_addr_le_t addr;
    bt_addr_le_t dir_addr;
    s8_t rssi;
} __packed;
struct bt_hci_evt_le_direct_adv_report {
    u8_t num_reports;
    struct bt_hci_evt_le_direct_adv_info direct_adv_info[0];
} __packed;

#define BT_HCI_EVT_LE_PHY_UPDATE_COMPLETE 0x0c
struct bt_hci_evt_le_phy_update_complete {
    u8_t status;
    u16_t handle;
    u8_t tx_phy;
    u8_t rx_phy;
} __packed;

#define BT_HCI_EVT_LE_EXT_ADVERTISING_REPORT 0x0d

#define BT_HCI_LE_ADV_EVT_TYPE_CONN     BIT(0)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN     BIT(1)
#define BT_HCI_LE_ADV_EVT_TYPE_DIRECT   BIT(2)
#define BT_HCI_LE_ADV_EVT_TYPE_SCAN_RSP BIT(3)
#define BT_HCI_LE_ADV_EVT_TYPE_LEGACY   BIT(4)

#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS(ev_type)   (((ev_type) >> 5) & 0x03)
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_COMPLETE   0
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_PARTIAL    1
#define BT_HCI_LE_ADV_EVT_TYPE_DATA_STATUS_INCOMPLETE 2

struct bt_hci_evt_le_ext_advertising_info {
    u8_t evt_type;
    bt_addr_le_t addr;
    u8_t prim_phy;
    u8_t sec_phy;
    u8_t sid;
    s8_t tx_power;
    s8_t rssi;
    u16_t interval;
    bt_addr_le_t direct_addr;
    u8_t length;
    u8_t data[0];
} __packed;
struct bt_hci_evt_le_ext_advertising_report {
    u8_t num_reports;
    struct bt_hci_evt_le_ext_advertising_info adv_info[0];
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SYNC_ESTABLISHED 0x0e
struct bt_hci_evt_le_per_adv_sync_established {
    u8_t status;
    u16_t handle;
    u8_t sid;
    bt_addr_le_t adv_addr;
    u8_t phy;
    u16_t interval;
    u8_t clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_PER_ADVERTISING_REPORT 0x0f
struct bt_hci_evt_le_per_advertising_report {
    u16_t handle;
    s8_t tx_power;
    s8_t rssi;
    u8_t unused;
    u8_t data_status;
    u8_t length;
    u8_t data[0];
} __packed;

#define BT_HCI_EVT_LE_PER_ADV_SYNC_LOST 0x10
struct bt_hci_evt_le_per_adv_sync_lost {
    u16_t handle;
} __packed;

#define BT_HCI_EVT_LE_SCAN_TIMEOUT 0x11

#define BT_HCI_EVT_LE_ADV_SET_TERMINATED 0x12
struct bt_hci_evt_le_per_adv_set_terminated {
    u8_t status;
    u8_t adv_handle;
    u16_t conn_handle;
    u8_t num_completed_ext_adv_evts;
} __packed;

#define BT_HCI_EVT_LE_SCAN_REQ_RECEIVED 0x13
struct bt_hci_evt_le_scan_req_received {
    u8_t handle;
    bt_addr_le_t addr;
} __packed;

#define BT_HCI_LE_CHAN_SEL_ALGO_1 0x00
#define BT_HCI_LE_CHAN_SEL_ALGO_2 0x01

#define BT_HCI_EVT_LE_CHAN_SEL_ALGO 0x14
struct bt_hci_evt_le_chan_sel_algo {
    u16_t handle;
    u8_t chan_sel_algo;
} __packed;

#define BT_HCI_EVT_LE_PAST_RECEIVED 0x18
struct bt_hci_evt_le_past_received {
    uint8_t status;
    uint16_t conn_handle;
    uint16_t service_data;
    uint16_t sync_handle;
    uint8_t adv_sid;
    bt_addr_le_t addr;
    uint8_t phy;
    uint16_t interval;
    uint8_t clock_accuracy;
} __packed;

#define BT_HCI_EVT_LE_CIS_ESTABLISHED 0x19
struct bt_hci_evt_le_cis_established {
    uint8_t status;
    uint16_t conn_handle;
    uint8_t cig_sync_delay[3];
    uint8_t cis_sync_delay[3];
    uint8_t m_latency[3];
    uint8_t s_latency[3];
    uint8_t m_phy;
    uint8_t s_phy;
    uint8_t nse;
    uint8_t m_bn;
    uint8_t s_bn;
    uint8_t m_ft;
    uint8_t s_ft;
    uint16_t m_max_pdu;
    uint16_t s_max_pdu;
    uint16_t interval;
} __packed;

#define BT_HCI_EVT_LE_CIS_REQ 0x1a
struct bt_hci_evt_le_cis_req {
    uint16_t acl_handle;
    uint16_t cis_handle;
    uint8_t cig_id;
    uint8_t cis_id;
} __packed;

#define BT_HCI_EVT_LE_BIG_COMPLETE 0x1b
struct bt_hci_evt_le_big_complete {
    uint8_t status;
    uint8_t big_handle;
    uint8_t sync_delay[3];
    uint8_t latency[3];
    uint8_t phy;
    uint8_t nse;
    uint8_t bn;
    uint8_t pto;
    uint8_t irc;
    uint16_t max_pdu;
    uint8_t num_bis;
    uint16_t handle[0];
} __packed;

#define BT_HCI_EVT_LE_BIG_TERMINATE 0x1c
struct bt_hci_evt_le_big_terminate {
    uint8_t big_handle;
    uint8_t reason;
} __packed;

#define BT_HCI_EVT_LE_BIG_SYNC_ESTABLISHED 0x1d
struct bt_hci_evt_le_big_sync_established {
    uint8_t status;
    uint8_t big_handle;
    uint8_t latency[3];
    uint8_t nse;
    uint8_t bn;
    uint8_t pto;
    uint8_t irc;
    uint16_t max_pdu;
    uint8_t num_bis;
    uint16_t handle[0];
} __packed;

#define BT_HCI_EVT_LE_BIG_SYNC_LOST 0x1e
struct bt_hci_evt_le_big_sync_lost {
    uint8_t big_handle;
    uint8_t reason;
} __packed;

#define BT_HCI_EVT_LE_REQ_PEER_SCA_COMPLETE 0x1f
struct bt_hci_evt_le_req_peer_sca_complete {
    uint8_t status;
    uint16_t handle;
    uint8_t sca;
} __packed;

#define BT_HCI_EVT_LE_BIGINFO_ADV_REPORT 0x22
struct bt_hci_evt_le_biginfo_adv_report {
    uint16_t sync_handle;
    uint8_t num_bis;
    uint8_t nse;
    uint16_t iso_interval;
    uint8_t bn;
    uint8_t pto;
    uint8_t irc;
    uint16_t max_pdu;
    uint8_t sdu_interval[3];
    uint16_t max_sdu;
    uint8_t phy;
    uint8_t framing;
    uint8_t encryption;
} __packed;

/* Event mask bits */

#define BT_EVT_BIT(n) (1ULL << (n))

#define BT_EVT_MASK_INQUIRY_COMPLETE             BT_EVT_BIT(0)
#define BT_EVT_MASK_CONN_COMPLETE                BT_EVT_BIT(2)
#define BT_EVT_MASK_CONN_REQUEST                 BT_EVT_BIT(3)
#define BT_EVT_MASK_DISCONN_COMPLETE             BT_EVT_BIT(4)
#define BT_EVT_MASK_AUTH_COMPLETE                BT_EVT_BIT(5)
#define BT_EVT_MASK_REMOTE_NAME_REQ_COMPLETE     BT_EVT_BIT(6)
#define BT_EVT_MASK_ENCRYPT_CHANGE               BT_EVT_BIT(7)
#define BT_EVT_MASK_REMOTE_FEATURES              BT_EVT_BIT(10)
#define BT_EVT_MASK_REMOTE_VERSION_INFO          BT_EVT_BIT(11)
#define BT_EVT_MASK_HARDWARE_ERROR               BT_EVT_BIT(15)
#define BT_EVT_MASK_ROLE_CHANGE                  BT_EVT_BIT(17)
#define BT_EVT_MASK_PIN_CODE_REQ                 BT_EVT_BIT(21)
#define BT_EVT_MASK_LINK_KEY_REQ                 BT_EVT_BIT(22)
#define BT_EVT_MASK_LINK_KEY_NOTIFY              BT_EVT_BIT(23)
#define BT_EVT_MASK_DATA_BUFFER_OVERFLOW         BT_EVT_BIT(25)
#define BT_EVT_MASK_INQUIRY_RESULT_WITH_RSSI     BT_EVT_BIT(33)
#define BT_EVT_MASK_REMOTE_EXT_FEATURES          BT_EVT_BIT(34)
#define BT_EVT_MASK_SYNC_CONN_COMPLETE           BT_EVT_BIT(43)
#define BT_EVT_MASK_EXTENDED_INQUIRY_RESULT      BT_EVT_BIT(46)
#define BT_EVT_MASK_ENCRYPT_KEY_REFRESH_COMPLETE BT_EVT_BIT(47)
#define BT_EVT_MASK_IO_CAPA_REQ                  BT_EVT_BIT(48)
#define BT_EVT_MASK_IO_CAPA_RESP                 BT_EVT_BIT(49)
#define BT_EVT_MASK_USER_CONFIRM_REQ             BT_EVT_BIT(50)
#define BT_EVT_MASK_USER_PASSKEY_REQ             BT_EVT_BIT(51)
#define BT_EVT_MASK_SSP_COMPLETE                 BT_EVT_BIT(53)
#define BT_EVT_MASK_USER_PASSKEY_NOTIFY          BT_EVT_BIT(58)
#define BT_EVT_MASK_LE_META_EVENT                BT_EVT_BIT(61)

/* Page 2 */
#define BT_EVT_MASK_PHY_LINK_COMPLETE           BT_EVT_BIT(0)
#define BT_EVT_MASK_CH_SELECTED_COMPLETE        BT_EVT_BIT(1)
#define BT_EVT_MASK_DISCONN_PHY_LINK_COMPLETE   BT_EVT_BIT(2)
#define BT_EVT_MASK_PHY_LINK_LOSS_EARLY_WARN    BT_EVT_BIT(3)
#define BT_EVT_MASK_PHY_LINK_RECOVERY           BT_EVT_BIT(4)
#define BT_EVT_MASK_LOG_LINK_COMPLETE           BT_EVT_BIT(5)
#define BT_EVT_MASK_DISCONN_LOG_LINK_COMPLETE   BT_EVT_BIT(6)
#define BT_EVT_MASK_FLOW_SPEC_MODIFY_COMPLETE   BT_EVT_BIT(7)
#define BT_EVT_MASK_NUM_COMPLETE_DATA_BLOCKS    BT_EVT_BIT(8)
#define BT_EVT_MASK_AMP_START_TEST              BT_EVT_BIT(9)
#define BT_EVT_MASK_AMP_TEST_END                BT_EVT_BIT(10)
#define BT_EVT_MASK_AMP_RX_REPORT               BT_EVT_BIT(11)
#define BT_EVT_MASK_AMP_SR_MODE_CHANGE_COMPLETE BT_EVT_BIT(12)
#define BT_EVT_MASK_AMP_STATUS_CHANGE           BT_EVT_BIT(13)
#define BT_EVT_MASK_TRIGG_CLOCK_CAPTURE         BT_EVT_BIT(14)
#define BT_EVT_MASK_SYNCH_TRAIN_COMPLETE        BT_EVT_BIT(15)
#define BT_EVT_MASK_SYNCH_TRAIN_RX              BT_EVT_BIT(16)
#define BT_EVT_MASK_CL_SLAVE_BC_RX              BT_EVT_BIT(17)
#define BT_EVT_MASK_CL_SLAVE_BC_TIMEOUT         BT_EVT_BIT(18)
#define BT_EVT_MASK_TRUNC_PAGE_COMPLETE         BT_EVT_BIT(19)
#define BT_EVT_MASK_SLAVE_PAGE_RSP_TIMEOUT      BT_EVT_BIT(20)
#define BT_EVT_MASK_CL_SLAVE_BC_CH_MAP_CHANGE   BT_EVT_BIT(21)
#define BT_EVT_MASK_INQUIRY_RSP_NOT             BT_EVT_BIT(22)
#define BT_EVT_MASK_AUTH_PAYLOAD_TIMEOUT_EXP    BT_EVT_BIT(23)
#define BT_EVT_MASK_SAM_STATUS_CHANGE           BT_EVT_BIT(24)

#define BT_EVT_MASK_LE_CONN_COMPLETE            BT_EVT_BIT(0)
#define BT_EVT_MASK_LE_ADVERTISING_REPORT       BT_EVT_BIT(1)
#define BT_EVT_MASK_LE_CONN_UPDATE_COMPLETE     BT_EVT_BIT(2)
#define BT_EVT_MASK_LE_REMOTE_FEAT_COMPLETE     BT_EVT_BIT(3)
#define BT_EVT_MASK_LE_LTK_REQUEST              BT_EVT_BIT(4)
#define BT_EVT_MASK_LE_CONN_PARAM_REQ           BT_EVT_BIT(5)
#define BT_EVT_MASK_LE_DATA_LEN_CHANGE          BT_EVT_BIT(6)
#define BT_EVT_MASK_LE_P256_PUBLIC_KEY_COMPLETE BT_EVT_BIT(7)
#define BT_EVT_MASK_LE_GENERATE_DHKEY_COMPLETE  BT_EVT_BIT(8)
#define BT_EVT_MASK_LE_ENH_CONN_COMPLETE        BT_EVT_BIT(9)
#define BT_EVT_MASK_LE_DIRECT_ADV_REPORT        BT_EVT_BIT(10)
#define BT_EVT_MASK_LE_PHY_UPDATE_COMPLETE      BT_EVT_BIT(11)
#define BT_EVT_MASK_LE_EXT_ADVERTISING_REPORT   BT_EVT_BIT(12)
#define BT_EVT_MASK_LE_PER_ADV_SYNC_ESTABLISHED BT_EVT_BIT(13)
#define BT_EVT_MASK_LE_PER_ADVERTISING_REPORT   BT_EVT_BIT(14)
#define BT_EVT_MASK_LE_PER_ADV_SYNC_LOST        BT_EVT_BIT(15)
#define BT_EVT_MASK_LE_SCAN_TIMEOUT             BT_EVT_BIT(16)
#define BT_EVT_MASK_LE_ADV_SET_TERMINATED       BT_EVT_BIT(17)
#define BT_EVT_MASK_LE_SCAN_REQ_RECEIVED        BT_EVT_BIT(18)
#define BT_EVT_MASK_LE_CHAN_SEL_ALGO            BT_EVT_BIT(19)
#define BT_EVT_MASK_LE_PAST_RECEIVED            BT_EVT_BIT(23)
#define BT_EVT_MASK_LE_CIS_ESTABLISHED          BT_EVT_BIT(24)
#define BT_EVT_MASK_LE_CIS_REQ                  BT_EVT_BIT(25)
#define BT_EVT_MASK_LE_BIG_COMPLETE             BT_EVT_BIT(26)
#define BT_EVT_MASK_LE_BIG_TERMINATED           BT_EVT_BIT(27)
#define BT_EVT_MASK_LE_BIG_SYNC_ESTABLISHED     BT_EVT_BIT(28)
#define BT_EVT_MASK_LE_BIG_SYNC_LOST            BT_EVT_BIT(29)
#define BT_EVT_MASK_LE_REQ_PEER_SCA_COMPLETE    BT_EVT_BIT(30)
#define BT_EVT_MASK_LE_PATH_LOSS_THRESHOLD      BT_EVT_BIT(31)
#define BT_EVT_MASK_LE_TRANSMIT_POWER_REPORTING BT_EVT_BIT(32)
#define BT_EVT_MASK_LE_BIGINFO_ADV_REPORT       BT_EVT_BIT(33)

/** Allocate a HCI command buffer.
  *
  * This function allocates a new buffer for a HCI command. It is given
  * the OpCode (encoded e.g. using the BT_OP macro) and the total length
  * of the parameters. Upon successful return the buffer is ready to have
  * the parameters encoded into it.
  *
  * @param opcode     Command OpCode.
  * @param param_len  Length of command parameters.
  *
  * @return Newly allocated buffer.
  */
struct net_buf *bt_hci_cmd_create(u16_t opcode, u8_t param_len);

/** Send a HCI command asynchronously.
  *
  * This function is used for sending a HCI command asynchronously. It can
  * either be called for a buffer created using bt_hci_cmd_create(), or
  * if the command has no parameters a NULL can be passed instead. The
  * sending of the command will happen asynchronously, i.e. upon successful
  * return from this function the caller only knows that it was queued
  * successfully.
  *
  * If synchronous behavior, and retrieval of the Command Complete parameters
  * is desired, the bt_hci_cmd_send_sync() API should be used instead.
  *
  * @param opcode Command OpCode.
  * @param buf    Command buffer or NULL (if no parameters).
  *
  * @return 0 on success or negative error value on failure.
  */
int bt_hci_cmd_send(u16_t opcode, struct net_buf *buf);

/** Send a HCI command synchronously.
  *
  * This function is used for sending a HCI command synchronously. It can
  * either be called for a buffer created using bt_hci_cmd_create(), or
  * if the command has no parameters a NULL can be passed instead.
  *
  * The function will block until a Command Status or a Command Complete
  * event is returned. If either of these have a non-zero status the function
  * will return a negative error code and the response reference will not
  * be set. If the command completed successfully and a non-NULL rsp parameter
  * was given, this parameter will be set to point to a buffer containing
  * the response parameters.
  *
  * @param opcode Command OpCode.
  * @param buf    Command buffer or NULL (if no parameters).
  * @param rsp    Place to store a reference to the command response. May
  *               be NULL if the caller is not interested in the response
  *               parameters. If non-NULL is passed the caller is responsible
  *               for calling net_buf_unref() on the buffer when done parsing
  *               it.
  *
  * @return 0 on success or negative error value on failure.
  */
int bt_hci_cmd_send_sync(u16_t opcode, struct net_buf *buf,
                         struct net_buf **rsp);

//declare bt_hci_get_conn_handle in conn_internal.h to pass compile
#if !defined(BFLB_BLE)
/** @brief Get connection handle for a connection.
 *
 * @param conn Connection object.
 * @param conn_handle Place to store the Connection handle.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_hci_get_conn_handle(const struct bt_conn *conn, u16_t *conn_handle);
#endif

/** @typedef bt_hci_vnd_evt_cb_t
  * @brief Callback type for vendor handling of HCI Vendor-Specific Events.
  *
  * A function of this type is registered with bt_hci_register_vnd_evt_cb()
  * and will be called for any HCI Vendor-Specific Event.
  *
  * @param buf Buffer containing event parameters.
  *
  * @return true if the function handles the event or false to defer the
  *         handling of this event back to the stack.
  */
typedef bool bt_hci_vnd_evt_cb_t(struct net_buf_simple *buf);

/** Register user callback for HCI Vendor-Specific Events
  *
  * @param cb Callback to be called when the stack receives a
  *           HCI Vendor-Specific Event.
  *
  * @return 0 on success or negative error value on failure.
  */
int bt_hci_register_vnd_evt_cb(bt_hci_vnd_evt_cb_t cb);

#if (BFLB_BT_CO_THREAD)
struct k_thread *bt_get_co_thread(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BT_HCI_H */
