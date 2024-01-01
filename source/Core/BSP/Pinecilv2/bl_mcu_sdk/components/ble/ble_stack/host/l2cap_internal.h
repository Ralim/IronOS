/** @file
 *  @brief Internal APIs for Bluetooth L2CAP handling.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <l2cap.h>

enum l2cap_conn_list_action {
    BT_L2CAP_CHAN_LOOKUP,
    BT_L2CAP_CHAN_DETACH,
};

#define BT_L2CAP_CID_BR_SIG 0x0001
#define BT_L2CAP_CID_ATT    0x0004
#define BT_L2CAP_CID_LE_SIG 0x0005
#define BT_L2CAP_CID_SMP    0x0006
#define BT_L2CAP_CID_BR_SMP 0x0007

#define BT_L2CAP_PSM_RFCOMM 0x0003

struct bt_l2cap_hdr {
    u16_t len;
    u16_t cid;
} __packed;

struct bt_l2cap_sig_hdr {
    u8_t code;
    u8_t ident;
    u16_t len;
} __packed;

#define BT_L2CAP_REJ_NOT_UNDERSTOOD 0x0000
#define BT_L2CAP_REJ_MTU_EXCEEDED   0x0001
#define BT_L2CAP_REJ_INVALID_CID    0x0002

#define BT_L2CAP_CMD_REJECT 0x01
struct bt_l2cap_cmd_reject {
    u16_t reason;
    u8_t data[0];
} __packed;

struct bt_l2cap_cmd_reject_cid_data {
    u16_t scid;
    u16_t dcid;
} __packed;

#define BT_L2CAP_CONN_REQ 0x02
struct bt_l2cap_conn_req {
    u16_t psm;
    u16_t scid;
} __packed;

/* command statuses in reposnse */
#define BT_L2CAP_CS_NO_INFO     0x0000
#define BT_L2CAP_CS_AUTHEN_PEND 0x0001

/* valid results in conn response on BR/EDR */
#define BT_L2CAP_BR_SUCCESS          0x0000
#define BT_L2CAP_BR_PENDING          0x0001
#define BT_L2CAP_BR_ERR_PSM_NOT_SUPP 0x0002
#define BT_L2CAP_BR_ERR_SEC_BLOCK    0x0003
#define BT_L2CAP_BR_ERR_NO_RESOURCES 0x0004
#define BT_L2CAP_BR_ERR_INVALID_SCID 0x0006
#define BT_L2CAP_BR_ERR_SCID_IN_USE  0x0007

#define BT_L2CAP_CONN_RSP 0x03
struct bt_l2cap_conn_rsp {
    u16_t dcid;
    u16_t scid;
    u16_t result;
    u16_t status;
} __packed;

#define BT_L2CAP_CONF_SUCCESS  0x0000
#define BT_L2CAP_CONF_UNACCEPT 0x0001
#define BT_L2CAP_CONF_REJECT   0x0002

#define BT_L2CAP_CONF_REQ 0x04
struct bt_l2cap_conf_req {
    u16_t dcid;
    u16_t flags;
    u8_t data[0];
} __packed;

#define BT_L2CAP_CONF_RSP 0x05
struct bt_l2cap_conf_rsp {
    u16_t scid;
    u16_t flags;
    u16_t result;
    u8_t data[0];
} __packed;

/* Option type used by MTU config request data */
#define BT_L2CAP_CONF_OPT_MTU 0x01
/* Options bits selecting most significant bit (hint) in type field */
#define BT_L2CAP_CONF_HINT 0x80
#define BT_L2CAP_CONF_MASK 0x7f

struct bt_l2cap_conf_opt {
    u8_t type;
    u8_t len;
    u8_t data[0];
} __packed;

#define BT_L2CAP_DISCONN_REQ 0x06
struct bt_l2cap_disconn_req {
    u16_t dcid;
    u16_t scid;
} __packed;

#define BT_L2CAP_DISCONN_RSP 0x07
struct bt_l2cap_disconn_rsp {
    u16_t dcid;
    u16_t scid;
} __packed;

#define BT_L2CAP_INFO_FEAT_MASK  0x0002
#define BT_L2CAP_INFO_FIXED_CHAN 0x0003

#define BT_L2CAP_INFO_REQ 0x0a
struct bt_l2cap_info_req {
    u16_t type;
} __packed;

/* info result */
#define BT_L2CAP_INFO_SUCCESS 0x0000
#define BT_L2CAP_INFO_NOTSUPP 0x0001

#define BT_L2CAP_INFO_RSP 0x0b
struct bt_l2cap_info_rsp {
    u16_t type;
    u16_t result;
    u8_t data[0];
} __packed;

#define BT_L2CAP_CONN_PARAM_REQ 0x12
struct bt_l2cap_conn_param_req {
    u16_t min_interval;
    u16_t max_interval;
    u16_t latency;
    u16_t timeout;
} __packed;

#define BT_L2CAP_CONN_PARAM_ACCEPTED 0x0000
#define BT_L2CAP_CONN_PARAM_REJECTED 0x0001

#define BT_L2CAP_CONN_PARAM_RSP 0x13
struct bt_l2cap_conn_param_rsp {
    u16_t result;
} __packed;

#define BT_L2CAP_LE_CONN_REQ 0x14
struct bt_l2cap_le_conn_req {
    u16_t psm;
    u16_t scid;
    u16_t mtu;
    u16_t mps;
    u16_t credits;
} __packed;

/* valid results in conn response on LE */
#define BT_L2CAP_LE_SUCCESS             0x0000
#define BT_L2CAP_LE_ERR_PSM_NOT_SUPP    0x0002
#define BT_L2CAP_LE_ERR_NO_RESOURCES    0x0004
#define BT_L2CAP_LE_ERR_AUTHENTICATION  0x0005
#define BT_L2CAP_LE_ERR_AUTHORIZATION   0x0006
#define BT_L2CAP_LE_ERR_KEY_SIZE        0x0007
#define BT_L2CAP_LE_ERR_ENCRYPTION      0x0008
#define BT_L2CAP_LE_ERR_INVALID_SCID    0x0009
#define BT_L2CAP_LE_ERR_SCID_IN_USE     0x000A
#define BT_L2CAP_LE_ERR_UNACCEPT_PARAMS 0x000B

#define BT_L2CAP_LE_CONN_RSP 0x15
struct bt_l2cap_le_conn_rsp {
    u16_t dcid;
    u16_t mtu;
    u16_t mps;
    u16_t credits;
    u16_t result;
};

#define BT_L2CAP_LE_CREDITS 0x16
struct bt_l2cap_le_credits {
    u16_t cid;
    u16_t credits;
} __packed;

#define BT_L2CAP_SDU_HDR_LEN 2

#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
#define BT_L2CAP_RX_MTU CONFIG_BT_L2CAP_RX_MTU
#else
#define BT_L2CAP_RX_MTU (CONFIG_BT_RX_BUF_LEN - \
                         BT_HCI_ACL_HDR_SIZE - BT_L2CAP_HDR_SIZE)
#endif

struct bt_l2cap_fixed_chan {
    u16_t cid;
    int (*accept)(struct bt_conn *conn, struct bt_l2cap_chan **chan);
    sys_snode_t node;
};

#define BT_L2CAP_CHANNEL_DEFINE(_name, _cid, _accept)               \
    const Z_STRUCT_SECTION_ITERABLE(bt_l2cap_fixed_chan, _name) = { \
        .cid = _cid,                                                \
        .accept = _accept,                                          \
    }

/* Need a name different than bt_l2cap_fixed_chan for a different section */
struct bt_l2cap_br_fixed_chan {
    u16_t cid;
    int (*accept)(struct bt_conn *conn, struct bt_l2cap_chan **chan);
};

#define BT_L2CAP_BR_CHANNEL_DEFINE(_name, _cid, _accept)               \
    const Z_STRUCT_SECTION_ITERABLE(bt_l2cap_br_fixed_chan, _name) = { \
        .cid = _cid,                                                   \
        .accept = _accept,                                             \
    }

void l2cap_chan_sdu_sent(struct bt_conn *conn, void *user_data);
/* Register a fixed L2CAP channel for L2CAP */
void bt_l2cap_le_fixed_chan_register(struct bt_l2cap_fixed_chan *chan);

/* Notify L2CAP channels of a new connection */
void bt_l2cap_connected(struct bt_conn *conn);

/* Notify L2CAP channels of a disconnect event */
void bt_l2cap_disconnected(struct bt_conn *conn);

/* Add channel to the connection */
void bt_l2cap_chan_add(struct bt_conn *conn, struct bt_l2cap_chan *chan,
                       bt_l2cap_chan_destroy_t destroy);

/* Remove channel from the connection */
void bt_l2cap_chan_remove(struct bt_conn *conn, struct bt_l2cap_chan *chan);

/* Delete channel */
void bt_l2cap_chan_del(struct bt_l2cap_chan *chan);

const char *bt_l2cap_chan_state_str(bt_l2cap_chan_state_t state);

#if defined(CONFIG_BT_DEBUG_L2CAP)
void bt_l2cap_chan_set_state_debug(struct bt_l2cap_chan *chan,
                                   bt_l2cap_chan_state_t state,
                                   const char *func, int line);
#define bt_l2cap_chan_set_state(_chan, _state) \
    bt_l2cap_chan_set_state_debug(_chan, _state, __func__, __LINE__)
#else
void bt_l2cap_chan_set_state(struct bt_l2cap_chan *chan,
                             bt_l2cap_chan_state_t state);
#endif /* CONFIG_BT_DEBUG_L2CAP */

/*
 * Notify L2CAP channels of a change in encryption state passing additionally
 * HCI status of performed security procedure.
 */
void bt_l2cap_encrypt_change(struct bt_conn *conn, u8_t hci_status);

/* Prepare an L2CAP PDU to be sent over a connection */
struct net_buf *bt_l2cap_create_pdu_timeout(struct net_buf_pool *pool,
                                            size_t reserve, s32_t timeout);

#define bt_l2cap_create_pdu(_pool, _reserve) \
    bt_l2cap_create_pdu_timeout(_pool, _reserve, K_FOREVER)

/* Prepare a L2CAP Response PDU to be sent over a connection */
struct net_buf *bt_l2cap_create_rsp(struct net_buf *buf, size_t reserve);

/* Send L2CAP PDU over a connection
 *
 * Buffer ownership is transferred to stack so either in case of success
 * or error the buffer will be unref internally.
 *
 * Calling this from RX thread is assumed to never fail so the return can be
 * ignored.
 */
int bt_l2cap_send_cb(struct bt_conn *conn, u16_t cid, struct net_buf *buf,
                     bt_conn_tx_cb_t cb, void *user_data);

static inline void bt_l2cap_send(struct bt_conn *conn, u16_t cid,
                                 struct net_buf *buf)
{
    bt_l2cap_send_cb(conn, cid, buf, NULL, NULL);
}

/* Receive a new L2CAP PDU from a connection */
void bt_l2cap_recv(struct bt_conn *conn, struct net_buf *buf);

/* Perform connection parameter update request */
int bt_l2cap_update_conn_param(struct bt_conn *conn,
                               const struct bt_le_conn_param *param);

/* Initialize L2CAP and supported channels */
void bt_l2cap_init(void);

/* Lookup channel by Transmission CID */
struct bt_l2cap_chan *bt_l2cap_le_lookup_tx_cid(struct bt_conn *conn,
                                                u16_t cid);

/* Lookup channel by Receiver CID */
struct bt_l2cap_chan *bt_l2cap_le_lookup_rx_cid(struct bt_conn *conn,
                                                u16_t cid);

/* Initialize BR/EDR L2CAP signal layer */
void bt_l2cap_br_init(void);

/* Register fixed channel */
void bt_l2cap_br_fixed_chan_register(struct bt_l2cap_fixed_chan *chan);

/* Notify BR/EDR L2CAP channels about established new ACL connection */
void bt_l2cap_br_connected(struct bt_conn *conn);

/* Lookup BR/EDR L2CAP channel by Receiver CID */
struct bt_l2cap_chan *bt_l2cap_br_lookup_rx_cid(struct bt_conn *conn,
                                                u16_t cid);

/* Disconnects dynamic channel */
int bt_l2cap_br_chan_disconnect(struct bt_l2cap_chan *chan);

/* Make connection to peer psm server */
int bt_l2cap_br_chan_connect(struct bt_conn *conn, struct bt_l2cap_chan *chan,
                             u16_t psm);

/* Send packet data to connected peer */
int bt_l2cap_br_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf);

/*
 * Handle security level changed on link passing HCI status of performed
 * security procedure.
 */
void l2cap_br_encrypt_change(struct bt_conn *conn, u8_t hci_status);

/* Handle received data */
void bt_l2cap_br_recv(struct bt_conn *conn, struct net_buf *buf);
