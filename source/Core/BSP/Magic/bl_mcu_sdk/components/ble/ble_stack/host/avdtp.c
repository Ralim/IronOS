/*
 * Audio Video Distribution Protocol
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <zephyr.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <atomic.h>
#include <byteorder.h>
#include <util.h>

#include <hci_host.h>
#include <bluetooth.h>
#include <l2cap.h>
#include <a2dp.h>
#include <avdtp.h>

#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_AVDTP)
#define LOG_MODULE_NAME bt_avdtp
#include "log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap_internal.h"
#include "avdtp_internal.h"
#include "a2dp-codec.h"

#define AVDTP_MSG_POISTION 0x00
#define AVDTP_PKT_POSITION 0x02
#define AVDTP_TID_POSITION 0x04
#define AVDTP_SIGID_MASK   0x3f

#define AVDTP_GET_TR_ID(hdr)    ((hdr & 0xf0) >> AVDTP_TID_POSITION)
#define AVDTP_GET_MSG_TYPE(hdr) (hdr & 0x03)
#define AVDTP_GET_PKT_TYPE(hdr) ((hdr & 0x0c) >> AVDTP_PKT_POSITION)
#define AVDTP_GET_SIG_ID(s)     (s & AVDTP_SIGID_MASK)

static struct bt_avdtp_event_cb *event_cb;

static struct bt_avdtp_seid_lsep *lseps;

static uint8_t tid;

extern struct bt_a2dp_codec_sbc_params sbc_info;

#define AVDTP_CHAN(_ch) CONTAINER_OF(_ch, struct bt_avdtp, br_chan.chan)

#define AVDTP_KWORK(_work) CONTAINER_OF(_work, struct bt_avdtp_req, \
                                        timeout_work)

#define AVDTP_TIMEOUT K_SECONDS(6)

static void handle_avdtp_discover_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_get_cap_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_set_conf_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_get_conf_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_reconf_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_open_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_start_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_close_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_suspend_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_abort_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_sec_ctrl_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_get_all_cap_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);
static void handle_avdtp_dly_rpt_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type);

static const struct {
    uint8_t sig_id;
    void (*func)(struct bt_avdtp *session, struct net_buf *buf,
                 uint8_t msg_type);
} handler[] = {
    { BT_AVDTP_DISCOVER, handle_avdtp_discover_cmd },
    { BT_AVDTP_GET_CAPABILITIES, handle_avdtp_get_cap_cmd },
    { BT_AVDTP_SET_CONFIGURATION, handle_avdtp_set_conf_cmd },
    { BT_AVDTP_GET_CONFIGURATION, handle_avdtp_get_conf_cmd },
    { BT_AVDTP_RECONFIGURE, handle_avdtp_reconf_cmd },
    { BT_AVDTP_OPEN, handle_avdtp_open_cmd },
    { BT_AVDTP_START, handle_avdtp_start_cmd },
    { BT_AVDTP_CLOSE, handle_avdtp_close_cmd },
    { BT_AVDTP_SUSPEND, handle_avdtp_suspend_cmd },
    { BT_AVDTP_ABORT, handle_avdtp_abort_cmd },
    { BT_AVDTP_SECURITY_CONTROL, handle_avdtp_sec_ctrl_cmd },
    { BT_AVDTP_GET_ALL_CAPABILITIES, handle_avdtp_get_all_cap_cmd },
    { BT_AVDTP_DELAYREPORT, handle_avdtp_dly_rpt_cmd },
};

static int avdtp_send(struct bt_avdtp *session,
                      struct net_buf *buf, struct bt_avdtp_req *req)
{
    int result;
    struct bt_avdtp_single_sig_hdr *hdr;

    hdr = (struct bt_avdtp_single_sig_hdr *)buf->data;

    result = bt_l2cap_chan_send(&session->br_chan.chan, buf);
    if (result < 0) {
        BT_ERR("Error:L2CAP send fail - result = %d", result);
        return result;
    }

    /*Save the sent request*/
    req->sig = AVDTP_GET_SIG_ID(hdr->signal_id);
    req->tid = AVDTP_GET_TR_ID(hdr->hdr);
    BT_DBG("sig 0x%02X, tid 0x%02X", req->sig, req->tid);

    session->req = req;
    /* Start timeout work */
    k_delayed_work_submit(&session->req->timeout_work, AVDTP_TIMEOUT);
    return result;
}

static struct net_buf *avdtp_create_pdu(uint8_t msg_type,
                                        uint8_t pkt_type,
                                        uint8_t sig_id)
{
    struct net_buf *buf;
    struct bt_avdtp_single_sig_hdr *hdr;

    BT_DBG("tid = %d", tid);

    buf = bt_l2cap_create_pdu(NULL, 0);

    hdr = net_buf_add(buf, sizeof(*hdr));

    hdr->hdr = (msg_type | pkt_type << AVDTP_PKT_POSITION |
                tid++ << AVDTP_TID_POSITION);
    tid %= 16; /* Loop for 16*/
    hdr->signal_id = sig_id & AVDTP_SIGID_MASK;

    BT_DBG("hdr = 0x%02X, Signal_ID = 0x%02X", hdr->hdr, hdr->signal_id);
    return buf;
}

/* Timeout handler */
static void avdtp_timeout(struct k_work *work)
{
    BT_DBG("Failed Signal_id = %d", (AVDTP_KWORK(work))->sig);

    /* Gracefully Disconnect the Signalling and streaming L2cap chann*/
}

/**
* @brief  avdtp_parsing_capability : parsing avdtp capability content
*/
static int avdtp_parsing_capability(struct net_buf *buf)
{
    BT_DBG(" ");
    while (buf->len) {
        uint8_t svc_cat = net_buf_pull_u8(buf);
        uint8_t cat_len = net_buf_pull_u8(buf);
        ;
        switch (svc_cat) {
            case BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT:

                break;

            case BT_AVDTP_SERVICE_CAT_REPORTING:

                break;

            case BT_AVDTP_SERVICE_CAT_RECOVERY:

                break;

            case BT_AVDTP_SERVICE_CAT_CONTENT_PROTECTION:

                break;

            case BT_AVDTP_SERVICE_CAT_HDR_COMPRESSION:

                break;

            case BT_AVDTP_SERVICE_CAT_MULTIPLEXING:

                break;

            case BT_AVDTP_SERVICE_CAT_MEDIA_CODEC:

                break;

            case BT_AVDTP_SERVICE_CAT_DELAYREPORTING:

                break;

            default:
                break;
        }
    }

    return 0;
}

static void handle_avdtp_discover_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_DISCOVER);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    struct bt_avdtp_seid_lsep *disc_sep = lseps;
    uint8_t acp_endpoint[2];
    while (disc_sep) {
        acp_endpoint[0] = disc_sep->sep.id << 2 | disc_sep->sep.inuse << 1 | disc_sep->sep.rfa0;
        acp_endpoint[1] = disc_sep->sep.media_type << 4 | disc_sep->sep.tsep << 3 | disc_sep->sep.rfa1;
        memcpy(rsp_buf->data + rsp_buf->len, acp_endpoint, 2);
        rsp_buf->len += 2;
        disc_sep = disc_sep->next;
    }

#if 0
    BT_DBG("rsp_buf len: %d \n", rsp_buf->len);
    for(int i = 0; i < rsp_buf->len; i++)
    {
         BT_WARN("0x%02x, ", rsp_buf->data[i]);
    }
     BT_WARN("\n");
#endif

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_get_cap_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_GET_CAPABILITIES);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    uint8_t svc_cat_1[2];
    svc_cat_1[0] = BT_AVDTP_SERVICE_CAT_MEDIA_TRANSPORT;
    svc_cat_1[1] = 0;
    memcpy(rsp_buf->data + rsp_buf->len, svc_cat_1, 2);
    rsp_buf->len += 2;

    uint8_t svc_cat_2[8];
    svc_cat_2[0] = BT_AVDTP_SERVICE_CAT_MEDIA_CODEC;
    svc_cat_2[1] = 6;
    svc_cat_2[2] = BT_A2DP_AUDIO;
    svc_cat_2[3] = BT_A2DP_CODEC_TYPE_SBC;
    svc_cat_2[4] = sbc_info.config[0];
    svc_cat_2[5] = sbc_info.config[1];
    svc_cat_2[6] = sbc_info.min_bitpool;
    svc_cat_2[7] = sbc_info.max_bitpool;
    memcpy(rsp_buf->data + rsp_buf->len, svc_cat_2, 8);
    rsp_buf->len += 8;

    uint8_t svc_cat_3[4];
    svc_cat_3[0] = BT_AVDTP_SERVICE_CAT_CONTENT_PROTECTION;
    svc_cat_3[1] = 2;
    svc_cat_3[2] = BT_AVDTP_CONTENT_PROTECTION_LSB_SCMS_T;
    svc_cat_3[3] = BT_AVDTP_CONTENT_PROTECTION_MSB;
    memcpy(rsp_buf->data + rsp_buf->len, svc_cat_3, 4);
    rsp_buf->len += 4;

#if 0
    BT_DBG("rsp_buf len: %d \n", rsp_buf->len);
    for(int i = 0; i < rsp_buf->len; i++)
    {
         BT_WARN("0x%02x, ", rsp_buf->data[i]);
    }
     BT_WARN("\n");
#endif

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_set_conf_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    uint8_t acp_seid = net_buf_pull_u8(buf) >> 2;
    uint8_t int_seid = net_buf_pull_u8(buf) >> 2;

    int res_pars = avdtp_parsing_capability(buf);

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_SET_CONFIGURATION);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_get_conf_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
}

static void handle_avdtp_reconf_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
}

static void handle_avdtp_open_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_OPEN);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_start_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_START);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_close_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_CLOSE);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_suspend_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_SUSPEND);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_abort_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
    if (!session) {
        BT_DBG("Error: Session not valid");
        return;
    }

    struct net_buf *rsp_buf;
    rsp_buf = avdtp_create_pdu(BT_AVDTP_ACCEPT, BT_AVDTP_PACKET_TYPE_SINGLE, BT_AVDTP_ABORT);
    if (!rsp_buf) {
        BT_ERR("Error: No Buff available");
        return;
    }

    int result = bt_l2cap_chan_send(&session->br_chan.chan, rsp_buf);
    if (result < 0) {
        BT_ERR("Error: BT L2CAP send fail - result = %d", result);
        return;
    }
}

static void handle_avdtp_sec_ctrl_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
}

static void handle_avdtp_get_all_cap_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
}

static void handle_avdtp_dly_rpt_cmd(struct bt_avdtp *session, struct net_buf *buf, uint8_t msg_type)
{
    BT_DBG(" ");
}

/* L2CAP Interface callbacks */
void bt_avdtp_l2cap_connected(struct bt_l2cap_chan *chan)
{
    struct bt_avdtp *session;

    if (!chan) {
        BT_ERR("Invalid AVDTP chan");
        return;
    }

    session = AVDTP_CHAN(chan);
    BT_DBG("chan %p session %p", chan, session);
    /* Init the timer */
    k_delayed_work_init(&session->req->timeout_work, avdtp_timeout);
}

void bt_avdtp_l2cap_disconnected(struct bt_l2cap_chan *chan)
{
    struct bt_avdtp *session = AVDTP_CHAN(chan);

    BT_DBG("chan %p session %p", chan, session);
    session->br_chan.chan.conn = NULL;
    /* Clear the Pending req if set*/
}

void bt_avdtp_l2cap_encrypt_changed(struct bt_l2cap_chan *chan, uint8_t status)
{
    BT_DBG("");
}

int bt_avdtp_l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct bt_avdtp_single_sig_hdr *hdr;
    struct bt_avdtp *session = AVDTP_CHAN(chan);
    uint8_t i, msgtype, sigid, tid;

    if (buf->len < sizeof(*hdr)) {
        BT_ERR("Recvd Wrong AVDTP Header");
        return 0;
    }

    hdr = net_buf_pull_mem(buf, sizeof(*hdr));
    msgtype = AVDTP_GET_MSG_TYPE(hdr->hdr);
    sigid = AVDTP_GET_SIG_ID(hdr->signal_id);
    tid = AVDTP_GET_TR_ID(hdr->hdr);

    BT_DBG("msg_type[0x%02x] sig_id[0x%02x] tid[0x%02x]",
           msgtype, sigid, tid);

#if 0
	BT_DBG("avdtp payload len: %d \n", buf->len);
	for(int i = 0; i < buf->len; i++)
	{
		 BT_WARN("0x%02x, ", buf->data[i]);
	}
	 BT_WARN("\n");
#endif

    /* validate if there is an outstanding resp expected*/
    if (msgtype != BT_AVDTP_CMD) {
        if (session->req == NULL) {
            BT_DBG("Unexpected peer response");
            return 0;
        }

        if (session->req->sig != sigid ||
            session->req->tid != tid) {
            BT_DBG("Peer mismatch resp, expected sig[0x%02x]"
                   "tid[0x%02x]",
                   session->req->sig,
                   session->req->tid);
            return 0;
        }
    }

    for (i = 0U; i < ARRAY_SIZE(handler); i++) {
        if (sigid == handler[i].sig_id) {
            handler[i].func(session, buf, msgtype);
            return 0;
        }
    }

    return 0;
}

int bt_avdtp_l2cap_media_stream_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
#if 0
	BT_DBG("avdtp payload len: %d \n", buf->len);
	for(int i = 0; i < buf->len; i++)
	{
		 BT_WARN("0x%02x, ", buf->data[i]);
	}
	 BT_WARN("\n");
#endif

    int res = a2dp_sbc_decode_process(buf->data, buf->len);
    if (res) {
        BT_DBG("decode fail, error: %d \n", res);
    }

    return 0;
}

/*A2DP Layer interface */
int bt_avdtp_connect(struct bt_conn *conn, struct bt_avdtp *session)
{
    static const struct bt_l2cap_chan_ops ops = {
        .connected = bt_avdtp_l2cap_connected,
        .disconnected = bt_avdtp_l2cap_disconnected,
        .encrypt_change = bt_avdtp_l2cap_encrypt_changed,
        .recv = bt_avdtp_l2cap_recv
    };

    if (!session) {
        return -EINVAL;
    }

    session->br_chan.chan.ops = &ops;
    session->br_chan.chan.required_sec_level = BT_SECURITY_L2;

    return bt_l2cap_chan_connect(conn, &session->br_chan.chan,
                                 BT_L2CAP_PSM_AVDTP);
}

int bt_avdtp_disconnect(struct bt_avdtp *session)
{
    if (!session) {
        return -EINVAL;
    }

    BT_DBG("session %p", session);

    return bt_l2cap_chan_disconnect(&session->br_chan.chan);
}

int bt_avdtp_l2cap_accept(struct bt_conn *conn, struct bt_l2cap_chan **chan)
{
    struct bt_avdtp *session = NULL;
    int result;
    static const struct bt_l2cap_chan_ops ops = {
        .connected = bt_avdtp_l2cap_connected,
        .disconnected = bt_avdtp_l2cap_disconnected,
        .recv = bt_avdtp_l2cap_recv,
    };

    static const struct bt_l2cap_chan_ops media_ops = {
        .connected = bt_avdtp_l2cap_connected,
        .disconnected = bt_avdtp_l2cap_disconnected,
        .recv = bt_avdtp_l2cap_media_stream_recv,
    };

    BT_DBG("conn %p", conn);
    /* Get the AVDTP session from upper layer */
    result = event_cb->accept(conn, &session);
    if (result < 0) {
        return result;
    }

    if (!session->br_chan.chan.conn) {
        BT_DBG("create l2cap_br signal stream, session %p", session);
        session->br_chan.chan.ops = &ops;
        session->br_chan.rx.mtu = BT_AVDTP_MAX_MTU;
        *chan = &session->br_chan.chan;
        tid = 0;
    } else {
        BT_DBG("create l2cap_br AV stream, session %p", session);
        session->streams->chan.chan.ops = &media_ops;
        session->streams->chan.rx.mtu = BT_AVDTP_MAX_MTU;
        *chan = &session->streams->chan.chan;
    }

    return 0;
}

/* Application will register its callback */
int bt_avdtp_register(struct bt_avdtp_event_cb *cb)
{
    BT_DBG("");

    if (event_cb) {
        return -EALREADY;
    }

    event_cb = cb;

    return 0;
}

int bt_avdtp_register_sep(uint8_t media_type, uint8_t role,
                          struct bt_avdtp_seid_lsep *lsep)
{
    BT_DBG("");

    static uint8_t bt_avdtp_seid = BT_AVDTP_MIN_SEID;

    if (!lsep) {
        return -EIO;
    }

    if (bt_avdtp_seid == BT_AVDTP_MAX_SEID) {
        return -EIO;
    }

    lsep->sep.id = bt_avdtp_seid++;
    lsep->sep.inuse = 0U;
    lsep->sep.media_type = media_type;
    lsep->sep.tsep = role;

    lsep->next = lseps;
    lseps = lsep;

    return 0;
}

/* init function */
int bt_avdtp_init(void)
{
    int err;
    static struct bt_l2cap_server avdtp_l2cap = {
        .psm = BT_L2CAP_PSM_AVDTP,
        .sec_level = BT_SECURITY_L2,
        .accept = bt_avdtp_l2cap_accept,
    };

    BT_DBG("");

    /* Register AVDTP PSM with L2CAP */
    err = bt_l2cap_br_server_register(&avdtp_l2cap);
    if (err < 0) {
        BT_ERR("AVDTP L2CAP Registration failed %d", err);
    }

    return err;
}

/* AVDTP Discover Request */
int bt_avdtp_discover(struct bt_avdtp *session,
                      struct bt_avdtp_discover_params *param)
{
    struct net_buf *buf;

    BT_DBG("");
    if (!param || !session) {
        BT_DBG("Error: Callback/Session not valid");
        return -EINVAL;
    }

    buf = avdtp_create_pdu(BT_AVDTP_CMD,
                           BT_AVDTP_PACKET_TYPE_SINGLE,
                           BT_AVDTP_DISCOVER);
    if (!buf) {
        BT_ERR("Error: No Buff available");
        return -ENOMEM;
    }

    /* Body of the message */

    return avdtp_send(session, buf, &param->req);
}
