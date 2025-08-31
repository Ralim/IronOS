/** @file
 *  @brief Bluetooth L2CAP handling
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_L2CAP_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_L2CAP_H_

/**
 * @brief L2CAP
 * @defgroup bt_l2cap L2CAP
 * @ingroup bluetooth
 * @{
 */

#include <../bluetooth/buf.h>
#include <conn.h>
#include <hci_host.h>
#ifdef __cplusplus
extern "C" {
#endif

/* L2CAP header size, used for buffer size calculations */
#define BT_L2CAP_HDR_SIZE 4

/** @def BT_L2CAP_BUF_SIZE
 *
 *   Helper to calculate needed outgoing buffer size, useful e.g. for
 *   creating buffer pools.
 *
 *   @param mtu Needed L2CAP MTU.
 *
 *   @return Needed buffer size to match the requested L2CAP MTU.
 */
#define BT_L2CAP_BUF_SIZE(mtu) (BT_BUF_RESERVE +                          \
                                BT_HCI_ACL_HDR_SIZE + BT_L2CAP_HDR_SIZE + \
                                (mtu))

struct bt_l2cap_chan;

/** @typedef bt_l2cap_chan_destroy_t
 *  @brief Channel destroy callback
 *
 *  @param chan Channel object.
 */
typedef void (*bt_l2cap_chan_destroy_t)(struct bt_l2cap_chan *chan);

/** @brief Life-span states of L2CAP CoC channel. Used only by internal APIs
 *  dealing with setting channel to proper state depending on operational
 *  context.
 */
typedef enum bt_l2cap_chan_state {
    /** Channel disconnected */
    BT_L2CAP_DISCONNECTED,
    /** Channel in connecting state */
    BT_L2CAP_CONNECT,
    /** Channel in config state, BR/EDR specific */
    BT_L2CAP_CONFIG,
    /** Channel ready for upper layer traffic on it */
    BT_L2CAP_CONNECTED,
    /** Channel in disconnecting state */
    BT_L2CAP_DISCONNECT,

} __packed bt_l2cap_chan_state_t;

/** @brief Status of L2CAP channel. */
typedef enum bt_l2cap_chan_status {
    /** Channel output status */
    BT_L2CAP_STATUS_OUT,

    /* Total number of status - must be at the end of the enum */
    BT_L2CAP_NUM_STATUS,
} __packed bt_l2cap_chan_status_t;

/** @brief L2CAP Channel structure. */
struct bt_l2cap_chan {
    /** Channel connection reference */
    struct bt_conn *conn;
    /** Channel operations reference */
    struct bt_l2cap_chan_ops *ops;
    sys_snode_t node;
    bt_l2cap_chan_destroy_t destroy;
    /* Response Timeout eXpired (RTX) timer */
    struct k_delayed_work rtx_work;
    ATOMIC_DEFINE(status, BT_L2CAP_NUM_STATUS);

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    bt_l2cap_chan_state_t state;
    /** Remote PSM to be connected */
    u16_t psm;
    /** Helps match request context during CoC */
    u8_t ident;
    bt_security_t required_sec_level;
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */
};

/** @brief LE L2CAP Endpoint structure. */
struct bt_l2cap_le_endpoint {
    /** Endpoint CID */
    u16_t cid;
    /** Endpoint Maximum Transmission Unit */
    u16_t mtu;
    /** Endpoint Maximum PDU payload Size */
    u16_t mps;
    /** Endpoint initial credits */
    u16_t init_credits;
    /** Endpoint credits */
    struct k_sem credits;
};

/** @brief LE L2CAP Channel structure. */
struct bt_l2cap_le_chan {
    /** Common L2CAP channel reference object */
    struct bt_l2cap_chan chan;
    /** Channel Receiving Endpoint */
    struct bt_l2cap_le_endpoint rx;
    /** Channel Transmission Endpoint */
    struct bt_l2cap_le_endpoint tx;
    /** Channel Transmission queue */
    struct k_fifo tx_queue;
    /** Channel Pending Transmission buffer  */
    struct net_buf *tx_buf;
    /** Segment SDU packet from upper layer */
    struct net_buf *_sdu;
    u16_t _sdu_len;

    struct k_work rx_work;
    struct k_fifo rx_queue;
};

/** @def BT_L2CAP_LE_CHAN(_ch)
 *  @brief Helper macro getting container object of type bt_l2cap_le_chan
 *  address having the same container chan member address as object in question.
 *
 *  @param _ch Address of object of bt_l2cap_chan type
 *
 *  @return Address of in memory bt_l2cap_le_chan object type containing
 *  the address of in question object.
 */
#define BT_L2CAP_LE_CHAN(_ch) CONTAINER_OF(_ch, struct bt_l2cap_le_chan, chan)

/** @brief BREDR L2CAP Endpoint structure. */
struct bt_l2cap_br_endpoint {
    /** Endpoint CID */
    u16_t cid;
    /** Endpoint Maximum Transmission Unit */
    u16_t mtu;
};

/** @brief BREDR L2CAP Channel structure. */
struct bt_l2cap_br_chan {
    /** Common L2CAP channel reference object */
    struct bt_l2cap_chan chan;
    /** Channel Receiving Endpoint */
    struct bt_l2cap_br_endpoint rx;
    /** Channel Transmission Endpoint */
    struct bt_l2cap_br_endpoint tx;
    /* For internal use only */
    atomic_t flags[1];
};

/** @brief L2CAP Channel operations structure. */
struct bt_l2cap_chan_ops {
    /** Channel connected callback
	 *
	 *  If this callback is provided it will be called whenever the
	 *  connection completes.
	 *
	 *  @param chan The channel that has been connected
	 */
    void (*connected)(struct bt_l2cap_chan *chan);

    /** Channel disconnected callback
	 *
	 *  If this callback is provided it will be called whenever the
	 *  channel is disconnected, including when a connection gets
	 *  rejected.
	 *
	 *  @param chan The channel that has been Disconnected
	 */
    void (*disconnected)(struct bt_l2cap_chan *chan);

    /** Channel encrypt_change callback
	 *
	 *  If this callback is provided it will be called whenever the
	 *  security level changed (indirectly link encryption done) or
	 *  authentication procedure fails. In both cases security initiator
	 *  and responder got the final status (HCI status) passed by
	 *  related to encryption and authentication events from local host's
	 *  controller.
	 *
	 *  @param chan The channel which has made encryption status changed.
	 *  @param status HCI status of performed security procedure caused
	 *  by channel security requirements. The value is populated
	 *  by HCI layer and set to 0 when success and to non-zero (reference to
	 *  HCI Error Codes) when security/authentication failed.
	 */
    void (*encrypt_change)(struct bt_l2cap_chan *chan, u8_t hci_status);

    /** Channel alloc_buf callback
	 *
	 *  If this callback is provided the channel will use it to allocate
	 *  buffers to store incoming data.
	 *
	 *  @param chan The channel requesting a buffer.
	 *
	 *  @return Allocated buffer.
	 */
    struct net_buf *(*alloc_buf)(struct bt_l2cap_chan *chan);

    /** Channel recv callback
	 *
	 *  @param chan The channel receiving data.
	 *  @param buf Buffer containing incoming data.
	 *
	 *  @return 0 in case of success or negative value in case of error.
	 *  If -EINPROGRESS is returned user has to confirm once the data has
	 *  been processed by calling bt_l2cap_chan_recv_complete passing back
	 *  the buffer received with its original user_data which contains the
	 *  number of segments/credits used by the packet.
	 */
    int (*recv)(struct bt_l2cap_chan *chan, struct net_buf *buf);

    /*  Channel sent callback
	 *
	 *  If this callback is provided it will be called whenever a SDU has
	 *  been completely sent.
	 *
	 *  @param chan The channel which has sent data.
	 */
    void (*sent)(struct bt_l2cap_chan *chan);

    /*  Channel status callback
	 *
	 *  If this callback is provided it will be called whenever the
	 *  channel status changes.
	 *
	 *  @param chan The channel which status changed
	 *  @param status The channel status
	 */
    void (*status)(struct bt_l2cap_chan *chan, atomic_t *status);

#if defined(BFLB_BLE_MTU_CHANGE_CB)
    void (*mtu_changed)(struct bt_l2cap_chan *chan, u16_t mtu);
#endif
};

/** @def BT_L2CAP_CHAN_SEND_RESERVE
 *  @brief Headroom needed for outgoing buffers
 */
#define BT_L2CAP_CHAN_SEND_RESERVE (BT_BUF_RESERVE + 4 + 4)

/** @brief L2CAP Server structure. */
struct bt_l2cap_server {
    /** Server PSM. Possible values:
	 *
	 *  0               A dynamic value will be auto-allocated when
	 *                  bt_l2cap_server_register() is called.
	 *
	 *  0x0001-0x007f   Standard, Bluetooth SIG-assigned fixed values.
	 *
	 *  0x0080-0x00ff   Dynamically allocated. May be pre-set by the
	 *                  application before server registration (not
	 *                  recommended however), or auto-allocated by the
	 *                  stack if the app gave 0 as the value.
	 */
    u16_t psm;

    /** Required minimim security level */
    bt_security_t sec_level;

    /** Server accept callback
	 *
	 *  This callback is called whenever a new incoming connection requires
	 *  authorization.
	 *
	 *  @param conn The connection that is requesting authorization
	 *  @param chan Pointer to received the allocated channel
	 *
	 *  @return 0 in case of success or negative value in case of error.
	 *  Possible return values:
	 *  -ENOMEM if no available space for new channel.
	 *  -EACCES if application did not authorize the connection.
	 *  -EPERM if encryption key size is too short.
	 */
    int (*accept)(struct bt_conn *conn, struct bt_l2cap_chan **chan);

    sys_snode_t node;
};

/** @brief Register L2CAP server.
 *
 *  Register L2CAP server for a PSM, each new connection is authorized using
 *  the accept() callback which in case of success shall allocate the channel
 *  structure to be used by the new connection.
 *
 *  For fixed, SIG-assigned PSMs (in the range 0x0001-0x007f) the PSM should
 *  be assigned to server->psm before calling this API. For dynamic PSMs
 *  (in the range 0x0080-0x00ff) server->psm may be pre-set to a given value
 *  (this is however not recommended) or be left as 0, in which case upon
 *  return a newly allocated value will have been assigned to it. For
 *  dynamically allocated values the expectation is that it's exposed through
 *  a GATT service, and that's how L2CAP clients discover how to connect to
 *  the server.
 *
 *  @param server Server structure.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_l2cap_server_register(struct bt_l2cap_server *server);

/** @brief Register L2CAP server on BR/EDR oriented connection.
 *
 *  Register L2CAP server for a PSM, each new connection is authorized using
 *  the accept() callback which in case of success shall allocate the channel
 *  structure to be used by the new connection.
 *
 *  @param server Server structure.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_l2cap_br_server_register(struct bt_l2cap_server *server);

/** @brief Connect L2CAP channel
 *
 *  Connect L2CAP channel by PSM, once the connection is completed channel
 *  connected() callback will be called. If the connection is rejected
 *  disconnected() callback is called instead.
 *  Channel object passed (over an address of it) as second parameter shouldn't
 *  be instantiated in application as standalone. Instead of, application should
 *  create transport dedicated L2CAP objects, i.e. type of bt_l2cap_le_chan for
 *  LE and/or type of bt_l2cap_br_chan for BR/EDR. Then pass to this API
 *  the location (address) of bt_l2cap_chan type object which is a member
 *  of both transport dedicated objects.
 *
 *  @param conn Connection object.
 *  @param chan Channel object.
 *  @param psm Channel PSM to connect to.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_l2cap_chan_connect(struct bt_conn *conn, struct bt_l2cap_chan *chan,
                          u16_t psm);

/** @brief Disconnect L2CAP channel
 *
 *  Disconnect L2CAP channel, if the connection is pending it will be
 *  canceled and as a result the channel disconnected() callback is called.
 *  Regarding to input parameter, to get details see reference description
 *  to bt_l2cap_chan_connect() API above.
 *
 *  @param chan Channel object.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_l2cap_chan_disconnect(struct bt_l2cap_chan *chan);

/** @brief Send data to L2CAP channel
 *
 *  Send data from buffer to the channel. If credits are not available, buf will
 *  be queued and sent as and when credits are received from peer.
 *  Regarding to first input parameter, to get details see reference description
 *  to bt_l2cap_chan_connect() API above.
 *
 *  @return Bytes sent in case of success or negative value in case of error.
 */
int bt_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf);

/** @brief Complete receiving L2CAP channel data
 *
 * Complete the reception of incoming data. This shall only be called if the
 * channel recv callback has returned -EINPROGRESS to process some incoming
 * data. The buffer shall contain the original user_data as that is used for
 * storing the credits/segments used by the packet.
 *
 * @param chan Channel object.
 * @param buf Buffer containing the data.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_l2cap_chan_recv_complete(struct bt_l2cap_chan *chan,
                                struct net_buf *buf);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_L2CAP_H_ */
