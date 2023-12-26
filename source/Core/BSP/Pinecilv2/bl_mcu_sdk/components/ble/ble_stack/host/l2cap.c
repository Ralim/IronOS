/* l2cap.c - L2CAP handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <atomic.h>
#include <errno.h>
#include <misc/byteorder.h>
#include <misc/util.h>
#include <string.h>
#include <zephyr.h>

#include <bluetooth.h>
#include <conn.h>
#include <hci_driver.h>
#include <hci_host.h>

#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_L2CAP)
#define LOG_MODULE_NAME bt_l2cap
#include "log.h"

#include "conn_internal.h"
#include "hci_core.h"
#include "l2cap_internal.h"

#include "ble_config.h"

#define LE_CHAN_RTX(_w) CONTAINER_OF(_w, struct bt_l2cap_le_chan, chan.rtx_work)
#define CHAN_RX(_w)     CONTAINER_OF(_w, struct bt_l2cap_le_chan, rx_work)

#define L2CAP_LE_MIN_MTU 23

#if defined(CONFIG_BT_HCI_ACL_FLOW_CONTROL)
#define L2CAP_LE_MAX_CREDITS (CONFIG_BT_ACL_RX_COUNT - 1)
#else
#define L2CAP_LE_MAX_CREDITS (CONFIG_BT_RX_BUF_COUNT - 1)
#endif

#define L2CAP_LE_CID_DYN_START    0x0040
#define L2CAP_LE_CID_DYN_END      0x007f
#define L2CAP_LE_CID_IS_DYN(_cid) (_cid >= L2CAP_LE_CID_DYN_START && _cid <= L2CAP_LE_CID_DYN_END)

#define L2CAP_LE_PSM_FIXED_START 0x0001
#define L2CAP_LE_PSM_FIXED_END   0x007f
#define L2CAP_LE_PSM_DYN_START   0x0080
#define L2CAP_LE_PSM_DYN_END     0x00ff

#define L2CAP_CONN_TIMEOUT K_SECONDS(40)
#define L2CAP_DISC_TIMEOUT K_SECONDS(2)

#if defined(BFLB_BLE_DISABLE_STATIC_CHANNEL)
static sys_slist_t le_channels;
#endif

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
/* Size of MTU is based on the maximum amount of data the buffer can hold
 * excluding ACL and driver headers.
 */
#define L2CAP_MAX_LE_MPS BT_L2CAP_RX_MTU
/* For now use MPS - SDU length to disable segmentation */
#define L2CAP_MAX_LE_MTU (L2CAP_MAX_LE_MPS - 2)

#define l2cap_lookup_ident(conn, ident) __l2cap_lookup_ident(conn, ident, false)
#define l2cap_remove_ident(conn, ident) __l2cap_lookup_ident(conn, ident, true)

static sys_slist_t servers;

#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

/* L2CAP signalling channel specific context */
struct bt_l2cap {
  /* The channel this context is associated with */
  struct bt_l2cap_le_chan chan;
};

static struct bt_l2cap bt_l2cap_pool[CONFIG_BT_MAX_CONN];

static u8_t get_ident(void) {
  static u8_t ident;

  ident++;
  /* handle integer overflow (0 is not valid) */
  if (!ident) {
    ident++;
  }

  return ident;
}

#if defined(BFLB_BLE_DISABLE_STATIC_CHANNEL)
void bt_l2cap_le_fixed_chan_register(struct bt_l2cap_fixed_chan *chan) {
  BT_DBG("CID 0x%04x", chan->cid);

  sys_slist_append(&le_channels, &chan->node);
}
#endif

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
static struct bt_l2cap_le_chan *l2cap_chan_alloc_cid(struct bt_conn *conn, struct bt_l2cap_chan *chan) {
  struct bt_l2cap_le_chan *ch = BT_L2CAP_LE_CHAN(chan);
  u16_t                    cid;

  /*
   * No action needed if there's already a CID allocated, e.g. in
   * the case of a fixed channel.
   */
  if (ch && ch->rx.cid > 0) {
    return ch;
  }

  for (cid = L2CAP_LE_CID_DYN_START; cid <= L2CAP_LE_CID_DYN_END; cid++) {
    if (ch && !bt_l2cap_le_lookup_rx_cid(conn, cid)) {
      ch->rx.cid = cid;
      return ch;
    }
  }

  return NULL;
}

static struct bt_l2cap_le_chan *__l2cap_lookup_ident(struct bt_conn *conn, u16_t ident, bool remove) {
  struct bt_l2cap_chan *chan;
  sys_snode_t          *prev = NULL;

  SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
    if (chan->ident == ident) {
      if (remove) {
        sys_slist_remove(&conn->channels, prev, &chan->node);
      }
      return BT_L2CAP_LE_CHAN(chan);
    }

    prev = &chan->node;
  }

  return NULL;
}
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

void bt_l2cap_chan_remove(struct bt_conn *conn, struct bt_l2cap_chan *ch) {
  struct bt_l2cap_chan *chan;
  sys_snode_t          *prev = NULL;

  SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
    if (chan == ch) {
      sys_slist_remove(&conn->channels, prev, &chan->node);
      return;
    }

    prev = &chan->node;
  }
}

const char *bt_l2cap_chan_state_str(bt_l2cap_chan_state_t state) {
  switch (state) {
  case BT_L2CAP_DISCONNECTED:
    return "disconnected";
  case BT_L2CAP_CONNECT:
    return "connect";
  case BT_L2CAP_CONFIG:
    return "config";
  case BT_L2CAP_CONNECTED:
    return "connected";
  case BT_L2CAP_DISCONNECT:
    return "disconnect";
  default:
    return "unknown";
  }
}

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
#if defined(CONFIG_BT_DEBUG_L2CAP)
void bt_l2cap_chan_set_state_debug(struct bt_l2cap_chan *chan, bt_l2cap_chan_state_t state, const char *func, int line) {
  BT_DBG("chan %p psm 0x%04x %s -> %s", chan, chan->psm, bt_l2cap_chan_state_str(chan->state), bt_l2cap_chan_state_str(state));

  /* check transitions validness */
  switch (state) {
  case BT_L2CAP_DISCONNECTED:
    /* regardless of old state always allows this state */
    break;
  case BT_L2CAP_CONNECT:
    if (chan->state != BT_L2CAP_DISCONNECTED) {
      BT_WARN("%s()%d: invalid transition", func, line);
    }
    break;
  case BT_L2CAP_CONFIG:
    if (chan->state != BT_L2CAP_CONNECT) {
      BT_WARN("%s()%d: invalid transition", func, line);
    }
    break;
  case BT_L2CAP_CONNECTED:
    if (chan->state != BT_L2CAP_CONFIG && chan->state != BT_L2CAP_CONNECT) {
      BT_WARN("%s()%d: invalid transition", func, line);
    }
    break;
  case BT_L2CAP_DISCONNECT:
    if (chan->state != BT_L2CAP_CONFIG && chan->state != BT_L2CAP_CONNECTED) {
      BT_WARN("%s()%d: invalid transition", func, line);
    }
    break;
  default:
    BT_ERR("%s()%d: unknown (%u) state was set", func, line, state);
    return;
  }

  chan->state = state;
}
#else
void bt_l2cap_chan_set_state(struct bt_l2cap_chan *chan, bt_l2cap_chan_state_t state) { chan->state = state; }
#endif /* CONFIG_BT_DEBUG_L2CAP */
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

void bt_l2cap_chan_del(struct bt_l2cap_chan *chan) {
  BT_DBG("conn %p chan %p", chan->conn, chan);

  if (!chan->conn) {
    goto destroy;
  }

  if (chan->ops->disconnected) {
    chan->ops->disconnected(chan);
  }

  chan->conn = NULL;

destroy:
#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
  /* Reset internal members of common channel */
  bt_l2cap_chan_set_state(chan, BT_L2CAP_DISCONNECTED);
  chan->psm = 0U;
#endif

  if (chan->destroy) {
    chan->destroy(chan);
  }

#ifdef BFLB_BLE_PATCH_FREE_ALLOCATED_BUFFER_IN_OS
  if (chan->rtx_work.timer.timer.hdl) {
    k_delayed_work_del_timer(&chan->rtx_work);
  }
#endif
}

static void l2cap_rtx_timeout(struct k_work *work) {
  struct bt_l2cap_le_chan *chan = LE_CHAN_RTX(work);

  BT_ERR("chan %p timeout", chan);

  bt_l2cap_chan_remove(chan->chan.conn, &chan->chan);
  bt_l2cap_chan_del(&chan->chan);
}

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
static void l2cap_chan_le_recv(struct bt_l2cap_le_chan *chan, struct net_buf *buf);

static void l2cap_rx_process(struct k_work *work) {
  struct bt_l2cap_le_chan *ch = CHAN_RX(work);
  struct net_buf          *buf;

  while ((buf = net_buf_get(&ch->rx_queue, K_NO_WAIT))) {
    BT_DBG("ch %p buf %p", ch, buf);
    l2cap_chan_le_recv(ch, buf);
    net_buf_unref(buf);
  }
}
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

void bt_l2cap_chan_add(struct bt_conn *conn, struct bt_l2cap_chan *chan, bt_l2cap_chan_destroy_t destroy) {
  /* Attach channel to the connection */
  sys_slist_append(&conn->channels, &chan->node);
  chan->conn    = conn;
  chan->destroy = destroy;

  BT_DBG("conn %p chan %p", conn, chan);
}

static bool l2cap_chan_add(struct bt_conn *conn, struct bt_l2cap_chan *chan, bt_l2cap_chan_destroy_t destroy) {
  struct bt_l2cap_le_chan *ch;

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
  ch = l2cap_chan_alloc_cid(conn, chan);
#else
  ch = BT_L2CAP_LE_CHAN(chan);
#endif

  if (!ch) {
    BT_ERR("Unable to allocate L2CAP CID");
    return false;
  }

  k_delayed_work_init(&chan->rtx_work, l2cap_rtx_timeout);

  bt_l2cap_chan_add(conn, chan, destroy);

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
  if (L2CAP_LE_CID_IS_DYN(ch->rx.cid)) {
    k_work_init(&ch->rx_work, l2cap_rx_process);
    k_fifo_init(&ch->rx_queue, 20);
    bt_l2cap_chan_set_state(chan, BT_L2CAP_CONNECT);
  }
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

  return true;
}

void bt_l2cap_connected(struct bt_conn *conn) {
#if defined(BFLB_BLE_DISABLE_STATIC_CHANNEL)
  struct bt_l2cap_fixed_chan *fchan;
#endif
  struct bt_l2cap_chan *chan;

  if (IS_ENABLED(CONFIG_BT_BREDR) && conn->type == BT_CONN_TYPE_BR) {
    bt_l2cap_br_connected(conn);
    return;
  }

#if defined(BFLB_BLE_DISABLE_STATIC_CHANNEL)
  SYS_SLIST_FOR_EACH_CONTAINER(&le_channels, fchan, node) {
#else
  Z_STRUCT_SECTION_FOREACH(bt_l2cap_fixed_chan, fchan) {
#endif
    struct bt_l2cap_le_chan *ch;

    if (fchan->accept(conn, &chan) < 0) {
      continue;
    }

    ch = BT_L2CAP_LE_CHAN(chan);

    /* Fill up remaining fixed channel context attached in
     * fchan->accept()
     */
    ch->rx.cid = fchan->cid;
    ch->tx.cid = fchan->cid;

    if (!l2cap_chan_add(conn, chan, NULL)) {
      return;
    }

    if (chan->ops->connected) {
      chan->ops->connected(chan);
    }

    /* Always set output status to fixed channels */
    atomic_set_bit(chan->status, BT_L2CAP_STATUS_OUT);

    if (chan->ops->status) {
      chan->ops->status(chan, chan->status);
    }
  }
}

void bt_l2cap_disconnected(struct bt_conn *conn) {
  struct bt_l2cap_chan *chan, *next;

  SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&conn->channels, chan, next, node) { bt_l2cap_chan_del(chan); }
}

static struct net_buf *l2cap_create_le_sig_pdu(struct net_buf *buf, u8_t code, u8_t ident, u16_t len) {
  struct bt_l2cap_sig_hdr *hdr;

  /* Don't wait more than the minimum RTX timeout of 2 seconds */
  buf = bt_l2cap_create_pdu_timeout(NULL, 0, K_SECONDS(2));
  if (!buf) {
    /* If it was not possible to allocate a buffer within the
     * timeout return NULL.
     */
    BT_ERR("Unable to allocate buffer for op 0x%02x", code);
    return NULL;
  }

  hdr        = net_buf_add(buf, sizeof(*hdr));
  hdr->code  = code;
  hdr->ident = ident;
  hdr->len   = sys_cpu_to_le16(len);

  return buf;
}

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
static void l2cap_chan_send_req(struct bt_l2cap_le_chan *chan, struct net_buf *buf, s32_t timeout) {
  /* BLUETOOTH SPECIFICATION Version 4.2 [Vol 3, Part A] page 126:
   *
   * The value of this timer is implementation-dependent but the minimum
   * initial value is 1 second and the maximum initial value is 60
   * seconds. One RTX timer shall exist for each outstanding signaling
   * request, including each Echo Request. The timer disappears on the
   * final expiration, when the response is received, or the physical
   * link is lost.
   */
  if (timeout) {
    k_delayed_work_submit(&chan->chan.rtx_work, timeout);
  } else {
    k_delayed_work_cancel(&chan->chan.rtx_work);
  }

  bt_l2cap_send(chan->chan.conn, BT_L2CAP_CID_LE_SIG, buf);
}

static int l2cap_le_conn_req(struct bt_l2cap_le_chan *ch) {
  struct net_buf              *buf;
  struct bt_l2cap_le_conn_req *req;

  ch->chan.ident = get_ident();

  buf = l2cap_create_le_sig_pdu(NULL, BT_L2CAP_LE_CONN_REQ, ch->chan.ident, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req          = net_buf_add(buf, sizeof(*req));
  req->psm     = sys_cpu_to_le16(ch->chan.psm);
  req->scid    = sys_cpu_to_le16(ch->rx.cid);
  req->mtu     = sys_cpu_to_le16(ch->rx.mtu);
  req->mps     = sys_cpu_to_le16(ch->rx.mps);
  req->credits = sys_cpu_to_le16(ch->rx.init_credits);

  l2cap_chan_send_req(ch, buf, L2CAP_CONN_TIMEOUT);

  return 0;
}

static void l2cap_le_encrypt_change(struct bt_l2cap_chan *chan, u8_t status) {
  /* Skip channels already connected or with a pending request */
  if (chan->state != BT_L2CAP_CONNECT || chan->ident) {
    return;
  }

  if (status) {
    bt_l2cap_chan_remove(chan->conn, chan);
    bt_l2cap_chan_del(chan);
    return;
  }

  /* Retry to connect */
  l2cap_le_conn_req(BT_L2CAP_LE_CHAN(chan));
}
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

void bt_l2cap_encrypt_change(struct bt_conn *conn, u8_t hci_status) {
  struct bt_l2cap_chan *chan;

  if (IS_ENABLED(CONFIG_BT_BREDR) && conn->type == BT_CONN_TYPE_BR) {
    l2cap_br_encrypt_change(conn, hci_status);
    return;
  }

  SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
    l2cap_le_encrypt_change(chan, hci_status);
#endif

    if (chan->ops->encrypt_change) {
      chan->ops->encrypt_change(chan, hci_status);
    }
  }
}

struct net_buf *bt_l2cap_create_pdu_timeout(struct net_buf_pool *pool, size_t reserve, s32_t timeout) { return bt_conn_create_pdu_timeout(pool, sizeof(struct bt_l2cap_hdr) + reserve, timeout); }

int bt_l2cap_send_cb(struct bt_conn *conn, u16_t cid, struct net_buf *buf, bt_conn_tx_cb_t cb, void *user_data) {
  struct bt_l2cap_hdr *hdr;

  BT_DBG("conn %p cid %u len %zu", conn, cid, net_buf_frags_len(buf));

  hdr      = net_buf_push(buf, sizeof(*hdr));
  hdr->len = sys_cpu_to_le16(buf->len - sizeof(*hdr));
  hdr->cid = sys_cpu_to_le16(cid);

  return bt_conn_send_cb(conn, buf, cb, user_data);
}

static void l2cap_send_reject(struct bt_conn *conn, u8_t ident, u16_t reason, void *data, u8_t data_len) {
  struct bt_l2cap_cmd_reject *rej;
  struct net_buf             *buf;

  buf = l2cap_create_le_sig_pdu(NULL, BT_L2CAP_CMD_REJECT, ident, sizeof(*rej) + data_len);
  if (!buf) {
    return;
  }

  rej         = net_buf_add(buf, sizeof(*rej));
  rej->reason = sys_cpu_to_le16(reason);

  if (data) {
    net_buf_add_mem(buf, data, data_len);
  }

  bt_l2cap_send(conn, BT_L2CAP_CID_LE_SIG, buf);
}

static void le_conn_param_rsp(struct bt_l2cap *l2cap, struct net_buf *buf) {
  struct bt_l2cap_conn_param_rsp *rsp = (void *)buf->data;

  if (buf->len < sizeof(*rsp)) {
    BT_ERR("Too small LE conn param rsp");
    return;
  }

  BT_DBG("LE conn param rsp result %u", sys_le16_to_cpu(rsp->result));
}

static void le_conn_param_update_req(struct bt_l2cap *l2cap, u8_t ident, struct net_buf *buf) {
  struct bt_conn                 *conn = l2cap->chan.chan.conn;
  struct bt_le_conn_param         param;
  struct bt_l2cap_conn_param_rsp *rsp;
  struct bt_l2cap_conn_param_req *req = (void *)buf->data;
  bool                            accepted;

  if (buf->len < sizeof(*req)) {
    BT_ERR("Too small LE conn update param req");
    return;
  }

  if (conn->role != BT_HCI_ROLE_MASTER) {
    l2cap_send_reject(conn, ident, BT_L2CAP_REJ_NOT_UNDERSTOOD, NULL, 0);
    return;
  }

  param.interval_min = sys_le16_to_cpu(req->min_interval);
  param.interval_max = sys_le16_to_cpu(req->max_interval);
  param.latency      = sys_le16_to_cpu(req->latency);
  param.timeout      = sys_le16_to_cpu(req->timeout);

  BT_DBG("min 0x%04x max 0x%04x latency: 0x%04x timeout: 0x%04x", param.interval_min, param.interval_max, param.latency, param.timeout);

  buf = l2cap_create_le_sig_pdu(buf, BT_L2CAP_CONN_PARAM_RSP, ident, sizeof(*rsp));
  if (!buf) {
    return;
  }

  accepted = le_param_req(conn, &param);

  rsp = net_buf_add(buf, sizeof(*rsp));
  if (accepted) {
    rsp->result = sys_cpu_to_le16(BT_L2CAP_CONN_PARAM_ACCEPTED);
  } else {
    rsp->result = sys_cpu_to_le16(BT_L2CAP_CONN_PARAM_REJECTED);
  }

  bt_l2cap_send(conn, BT_L2CAP_CID_LE_SIG, buf);

  if (accepted) {
    bt_conn_le_conn_update(conn, &param);
  }
}

struct bt_l2cap_chan *bt_l2cap_le_lookup_tx_cid(struct bt_conn *conn, u16_t cid) {
  struct bt_l2cap_chan *chan;

  SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
    if (BT_L2CAP_LE_CHAN(chan)->tx.cid == cid) {
      return chan;
    }
  }

  return NULL;
}

struct bt_l2cap_chan *bt_l2cap_le_lookup_rx_cid(struct bt_conn *conn, u16_t cid) {
  struct bt_l2cap_chan *chan;

  SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
    if (BT_L2CAP_LE_CHAN(chan)->rx.cid == cid) {
      return chan;
    }
  }

  return NULL;
}

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
static struct bt_l2cap_server *l2cap_server_lookup_psm(u16_t psm) {
  struct bt_l2cap_server *server;

  SYS_SLIST_FOR_EACH_CONTAINER(&servers, server, node) {
    if (server->psm == psm) {
      return server;
    }
  }

  return NULL;
}

int bt_l2cap_server_register(struct bt_l2cap_server *server) {
  if (!server->accept) {
    return -EINVAL;
  }

  if (server->psm) {
    if (server->psm < L2CAP_LE_PSM_FIXED_START || server->psm > L2CAP_LE_PSM_DYN_END) {
      return -EINVAL;
    }

    /* Check if given PSM is already in use */
    if (l2cap_server_lookup_psm(server->psm)) {
      BT_DBG("PSM already registered");
      return -EADDRINUSE;
    }
  } else {
    u16_t psm;

    for (psm = L2CAP_LE_PSM_DYN_START; psm <= L2CAP_LE_PSM_DYN_END; psm++) {
      if (!l2cap_server_lookup_psm(psm)) {
        break;
      }
    }

    if (psm > L2CAP_LE_PSM_DYN_END) {
      BT_WARN("No free dynamic PSMs available");
      return -EADDRNOTAVAIL;
    }

    BT_DBG("Allocated PSM 0x%04x for new server", psm);
    server->psm = psm;
  }

  if (server->sec_level > BT_SECURITY_L4) {
    return -EINVAL;
  } else if (server->sec_level < BT_SECURITY_L1) {
    /* Level 0 is only applicable for BR/EDR */
    server->sec_level = BT_SECURITY_L1;
  }

  BT_DBG("PSM 0x%04x", server->psm);

  sys_slist_append(&servers, &server->node);

  return 0;
}

static void l2cap_chan_rx_init(struct bt_l2cap_le_chan *chan) {
  BT_DBG("chan %p", chan);

  /* Use existing MTU if defined */
  if (!chan->rx.mtu) {
    chan->rx.mtu = L2CAP_MAX_LE_MTU;
  }

  /* Use existing credits if defined */
  if (!chan->rx.init_credits) {
    if (chan->chan.ops->alloc_buf) {
      /* Auto tune credits to receive a full packet */
      chan->rx.init_credits = (chan->rx.mtu + (L2CAP_MAX_LE_MPS - 1)) / L2CAP_MAX_LE_MPS;
    } else {
      chan->rx.init_credits = L2CAP_LE_MAX_CREDITS;
    }
  }

  /* MPS shall not be bigger than MTU + 2 as the remaining bytes cannot
   * be used.
   */
  chan->rx.mps = MIN(chan->rx.mtu + 2, L2CAP_MAX_LE_MPS);
  k_sem_init(&chan->rx.credits, 0, BT_UINT_MAX);

  if (BT_DBG_ENABLED && chan->rx.init_credits * chan->rx.mps < chan->rx.mtu + 2) {
    BT_WARN("Not enough credits for a full packet");
  }
}

static void l2cap_chan_tx_init(struct bt_l2cap_le_chan *chan) {
  BT_DBG("chan %p", chan);

  (void)memset(&chan->tx, 0, sizeof(chan->tx));
  k_sem_init(&chan->tx.credits, 0, BT_UINT_MAX);
  k_fifo_init(&chan->tx_queue, 20);
}

static void l2cap_chan_tx_give_credits(struct bt_l2cap_le_chan *chan, u16_t credits) {
  BT_DBG("chan %p credits %u", chan, credits);

  while (credits--) {
    k_sem_give(&chan->tx.credits);
  }

  if (atomic_test_and_set_bit(chan->chan.status, BT_L2CAP_STATUS_OUT) && chan->chan.ops->status) {
    chan->chan.ops->status(&chan->chan, chan->chan.status);
  }
}

static void l2cap_chan_rx_give_credits(struct bt_l2cap_le_chan *chan, u16_t credits) {
  BT_DBG("chan %p credits %u", chan, credits);

  while (credits--) {
    k_sem_give(&chan->rx.credits);
  }
}

static void l2cap_chan_destroy(struct bt_l2cap_chan *chan) {
  struct bt_l2cap_le_chan *ch = BT_L2CAP_LE_CHAN(chan);
  struct net_buf          *buf;

  BT_DBG("chan %p cid 0x%04x", ch, ch->rx.cid);

  /* Cancel ongoing work */
  k_delayed_work_cancel(&chan->rtx_work);

  if (ch->tx_buf) {
    net_buf_unref(ch->tx_buf);
    ch->tx_buf = NULL;
  }

  /* Remove buffers on the TX queue */
  while ((buf = net_buf_get(&ch->tx_queue, K_NO_WAIT))) {
    net_buf_unref(buf);
  }

  /* Remove buffers on the RX queue */
  while ((buf = net_buf_get(&ch->rx_queue, K_NO_WAIT))) {
    net_buf_unref(buf);
  }

  /* Destroy segmented SDU if it exists */
  if (ch->_sdu) {
    net_buf_unref(ch->_sdu);
    ch->_sdu     = NULL;
    ch->_sdu_len = 0U;
  }
}

static u16_t le_err_to_result(int err) {
  switch (err) {
  case -ENOMEM:
    return BT_L2CAP_LE_ERR_NO_RESOURCES;
  case -EACCES:
    return BT_L2CAP_LE_ERR_AUTHORIZATION;
  case -EPERM:
    return BT_L2CAP_LE_ERR_KEY_SIZE;
  case -ENOTSUP:
    /* This handle the cases where a fixed channel is registered but
     * for some reason (e.g. controller not suporting a feature)
     * cannot be used.
     */
    return BT_L2CAP_LE_ERR_PSM_NOT_SUPP;
  default:
    return BT_L2CAP_LE_ERR_UNACCEPT_PARAMS;
  }
}

static void le_conn_req(struct bt_l2cap *l2cap, u8_t ident, struct net_buf *buf) {
  struct bt_conn              *conn = l2cap->chan.chan.conn;
  struct bt_l2cap_chan        *chan;
  struct bt_l2cap_server      *server;
  struct bt_l2cap_le_conn_req *req = (void *)buf->data;
  struct bt_l2cap_le_conn_rsp *rsp;
  u16_t                        psm, scid, mtu, mps, credits;
  int                          err;

  if (buf->len < sizeof(*req)) {
    BT_ERR("Too small LE conn req packet size");
    return;
  }

  psm     = sys_le16_to_cpu(req->psm);
  scid    = sys_le16_to_cpu(req->scid);
  mtu     = sys_le16_to_cpu(req->mtu);
  mps     = sys_le16_to_cpu(req->mps);
  credits = sys_le16_to_cpu(req->credits);

  BT_DBG("psm 0x%02x scid 0x%04x mtu %u mps %u credits %u", psm, scid, mtu, mps, credits);

  if (mtu < L2CAP_LE_MIN_MTU || mps < L2CAP_LE_MIN_MTU) {
    BT_ERR("Invalid LE-Conn Req params");
    return;
  }

  buf = l2cap_create_le_sig_pdu(buf, BT_L2CAP_LE_CONN_RSP, ident, sizeof(*rsp));
  if (!buf) {
    return;
  }

  rsp = net_buf_add(buf, sizeof(*rsp));
  (void)memset(rsp, 0, sizeof(*rsp));

  /* Check if there is a server registered */
  server = l2cap_server_lookup_psm(psm);
  if (!server) {
    rsp->result = sys_cpu_to_le16(BT_L2CAP_LE_ERR_PSM_NOT_SUPP);
    goto rsp;
  }

/* Check if connection has minimum required security level */
#if defined(CONFIG_BT_SMP)
  if (conn->sec_level < server->sec_level) {
    rsp->result = sys_cpu_to_le16(BT_L2CAP_LE_ERR_AUTHENTICATION);
    goto rsp;
  }
#endif

  if (!L2CAP_LE_CID_IS_DYN(scid)) {
    rsp->result = sys_cpu_to_le16(BT_L2CAP_LE_ERR_INVALID_SCID);
    goto rsp;
  }

  chan = bt_l2cap_le_lookup_tx_cid(conn, scid);
  if (chan) {
    rsp->result = sys_cpu_to_le16(BT_L2CAP_LE_ERR_SCID_IN_USE);
    goto rsp;
  }

  /* Request server to accept the new connection and allocate the
   * channel.
   */
  err = server->accept(conn, &chan);
  if (err < 0) {
    rsp->result = sys_cpu_to_le16(le_err_to_result(err));
    goto rsp;
  }

  chan->required_sec_level = server->sec_level;

  if (l2cap_chan_add(conn, chan, l2cap_chan_destroy)) {
    struct bt_l2cap_le_chan *ch = BT_L2CAP_LE_CHAN(chan);

    /* Init TX parameters */
    l2cap_chan_tx_init(ch);
    ch->tx.cid          = scid;
    ch->tx.mps          = mps;
    ch->tx.mtu          = mtu;
    ch->tx.init_credits = credits;
    l2cap_chan_tx_give_credits(ch, credits);

    /* Init RX parameters */
    l2cap_chan_rx_init(ch);
    l2cap_chan_rx_give_credits(ch, ch->rx.init_credits);

    /* Set channel PSM */
    chan->psm = server->psm;

    /* Update state */
    bt_l2cap_chan_set_state(chan, BT_L2CAP_CONNECTED);

    if (chan->ops->connected) {
      chan->ops->connected(chan);
    }

    /* Prepare response protocol data */
    rsp->dcid    = sys_cpu_to_le16(ch->rx.cid);
    rsp->mps     = sys_cpu_to_le16(ch->rx.mps);
    rsp->mtu     = sys_cpu_to_le16(ch->rx.mtu);
    rsp->credits = sys_cpu_to_le16(ch->rx.init_credits);
    rsp->result  = BT_L2CAP_LE_SUCCESS;
  } else {
    rsp->result = sys_cpu_to_le16(BT_L2CAP_LE_ERR_NO_RESOURCES);
  }
rsp:
  bt_l2cap_send(conn, BT_L2CAP_CID_LE_SIG, buf);
}

static struct bt_l2cap_le_chan *l2cap_remove_rx_cid(struct bt_conn *conn, u16_t cid) {
  struct bt_l2cap_chan *chan;
  sys_snode_t          *prev = NULL;

  /* Protect fixed channels against accidental removal */
  if (!L2CAP_LE_CID_IS_DYN(cid)) {
    return NULL;
  }

  SYS_SLIST_FOR_EACH_CONTAINER(&conn->channels, chan, node) {
    if (BT_L2CAP_LE_CHAN(chan)->rx.cid == cid) {
      sys_slist_remove(&conn->channels, prev, &chan->node);
      return BT_L2CAP_LE_CHAN(chan);
    }

    prev = &chan->node;
  }

  return NULL;
}

static void le_disconn_req(struct bt_l2cap *l2cap, u8_t ident, struct net_buf *buf) {
  struct bt_conn              *conn = l2cap->chan.chan.conn;
  struct bt_l2cap_le_chan     *chan;
  struct bt_l2cap_disconn_req *req = (void *)buf->data;
  struct bt_l2cap_disconn_rsp *rsp;
  u16_t                        dcid;

  if (buf->len < sizeof(*req)) {
    BT_ERR("Too small LE conn req packet size");
    return;
  }

  dcid = sys_le16_to_cpu(req->dcid);

  BT_DBG("dcid 0x%04x scid 0x%04x", dcid, sys_le16_to_cpu(req->scid));

  chan = l2cap_remove_rx_cid(conn, dcid);
  if (!chan) {
    struct bt_l2cap_cmd_reject_cid_data data;

    data.scid = req->scid;
    data.dcid = req->dcid;

    l2cap_send_reject(conn, ident, BT_L2CAP_REJ_INVALID_CID, &data, sizeof(data));
    return;
  }

  buf = l2cap_create_le_sig_pdu(buf, BT_L2CAP_DISCONN_RSP, ident, sizeof(*rsp));
  if (!buf) {
    return;
  }

  rsp       = net_buf_add(buf, sizeof(*rsp));
  rsp->dcid = sys_cpu_to_le16(chan->rx.cid);
  rsp->scid = sys_cpu_to_le16(chan->tx.cid);

  bt_l2cap_chan_del(&chan->chan);

  bt_l2cap_send(conn, BT_L2CAP_CID_LE_SIG, buf);
}

#if defined(CONFIG_BT_SMP)
static int l2cap_change_security(struct bt_l2cap_le_chan *chan, u16_t err) {
  switch (err) {
  case BT_L2CAP_LE_ERR_ENCRYPTION:
    if (chan->chan.required_sec_level >= BT_SECURITY_L2) {
      return -EALREADY;
    }
    chan->chan.required_sec_level = BT_SECURITY_L2;
    break;
  case BT_L2CAP_LE_ERR_AUTHENTICATION:
    if (chan->chan.required_sec_level < BT_SECURITY_L2) {
      chan->chan.required_sec_level = BT_SECURITY_L2;
    } else if (chan->chan.required_sec_level < BT_SECURITY_L3) {
      chan->chan.required_sec_level = BT_SECURITY_L3;
    } else if (chan->chan.required_sec_level < BT_SECURITY_L4) {
      chan->chan.required_sec_level = BT_SECURITY_L4;
    } else {
      return -EALREADY;
    }
    break;
  default:
    return -EINVAL;
  }

  return bt_conn_set_security(chan->chan.conn, chan->chan.required_sec_level);
}
#endif // CONFIG_BT_SMP

static void le_conn_rsp(struct bt_l2cap *l2cap, u8_t ident, struct net_buf *buf) {
  struct bt_conn              *conn = l2cap->chan.chan.conn;
  struct bt_l2cap_le_chan     *chan;
  struct bt_l2cap_le_conn_rsp *rsp = (void *)buf->data;
  u16_t                        dcid, mtu, mps, credits, result;

  if (buf->len < sizeof(*rsp)) {
    BT_ERR("Too small LE conn rsp packet size");
    return;
  }

  dcid    = sys_le16_to_cpu(rsp->dcid);
  mtu     = sys_le16_to_cpu(rsp->mtu);
  mps     = sys_le16_to_cpu(rsp->mps);
  credits = sys_le16_to_cpu(rsp->credits);
  result  = sys_le16_to_cpu(rsp->result);

  BT_DBG("dcid 0x%04x mtu %u mps %u credits %u result 0x%04x", dcid, mtu, mps, credits, result);

  /* Keep the channel in case of security errors */
  if (result == BT_L2CAP_LE_SUCCESS || result == BT_L2CAP_LE_ERR_AUTHENTICATION || result == BT_L2CAP_LE_ERR_ENCRYPTION) {
    chan = l2cap_lookup_ident(conn, ident);
  } else {
    chan = l2cap_remove_ident(conn, ident);
  }

  if (!chan) {
    BT_ERR("Cannot find channel for ident %u", ident);
    return;
  }

  /* Cancel RTX work */
  k_delayed_work_cancel(&chan->chan.rtx_work);

  /* Reset ident since it got a response */
  chan->chan.ident = 0U;

  switch (result) {
  case BT_L2CAP_LE_SUCCESS:
    chan->tx.cid = dcid;
    chan->tx.mtu = mtu;
    chan->tx.mps = mps;

    /* Update state */
    bt_l2cap_chan_set_state(&chan->chan, BT_L2CAP_CONNECTED);

    if (chan->chan.ops->connected) {
      chan->chan.ops->connected(&chan->chan);
    }

    /* Give credits */
    l2cap_chan_tx_give_credits(chan, credits);
    l2cap_chan_rx_give_credits(chan, chan->rx.init_credits);

    break;
  case BT_L2CAP_LE_ERR_AUTHENTICATION:
  case BT_L2CAP_LE_ERR_ENCRYPTION:
#if defined(CONFIG_BT_SMP)
    /* If security needs changing wait it to be completed */
    if (l2cap_change_security(chan, result) == 0) {
      return;
    }
#endif
    bt_l2cap_chan_remove(conn, &chan->chan);
    __attribute__((fallthrough));
  default:
    bt_l2cap_chan_del(&chan->chan);
  }
}

static void le_disconn_rsp(struct bt_l2cap *l2cap, u8_t ident, struct net_buf *buf) {
  struct bt_conn              *conn = l2cap->chan.chan.conn;
  struct bt_l2cap_le_chan     *chan;
  struct bt_l2cap_disconn_rsp *rsp = (void *)buf->data;
  u16_t                        scid;

  if (buf->len < sizeof(*rsp)) {
    BT_ERR("Too small LE disconn rsp packet size");
    return;
  }

  scid = sys_le16_to_cpu(rsp->scid);

  BT_DBG("dcid 0x%04x scid 0x%04x", sys_le16_to_cpu(rsp->dcid), scid);

  chan = l2cap_remove_rx_cid(conn, scid);
  if (!chan) {
    return;
  }

  bt_l2cap_chan_del(&chan->chan);
}

static inline struct net_buf *l2cap_alloc_seg(struct net_buf *buf) {
  struct net_buf_pool *pool = net_buf_pool_get(buf->pool_id);
  struct net_buf      *seg;

  /* Try to use original pool if possible */
  seg = net_buf_alloc(pool, K_NO_WAIT);
  if (seg) {
    net_buf_reserve(seg, BT_L2CAP_CHAN_SEND_RESERVE);
    return seg;
  }

  /* Fallback to using global connection tx pool */
  return bt_l2cap_create_pdu(NULL, 0);
}

static struct net_buf *l2cap_chan_create_seg(struct bt_l2cap_le_chan *ch, struct net_buf *buf, size_t sdu_hdr_len) {
  struct net_buf *seg;
  u16_t           headroom;
  u16_t           len;

  /* Segment if data (+ data headroom) is bigger than MPS */
  if (buf->len + sdu_hdr_len > ch->tx.mps) {
    goto segment;
  }

  headroom = BT_L2CAP_CHAN_SEND_RESERVE + sdu_hdr_len;

  /* Check if original buffer has enough headroom and don't have any
   * fragments.
   */
  if (net_buf_headroom(buf) >= headroom && !buf->frags) {
    if (sdu_hdr_len) {
      /* Push SDU length if set */
      net_buf_push_le16(buf, net_buf_frags_len(buf));
    }
    return net_buf_ref(buf);
  }

segment:
  seg = l2cap_alloc_seg(buf);
  if (!seg) {
    return NULL;
  }

  if (sdu_hdr_len) {
    net_buf_add_le16(seg, net_buf_frags_len(buf));
  }

  /* Don't send more that TX MPS including SDU length */
  len = MIN(net_buf_tailroom(seg), ch->tx.mps - sdu_hdr_len);
  /* Limit if original buffer is smaller than the segment */
  len = MIN(buf->len, len);
  net_buf_add_mem(seg, buf->data, len);
  net_buf_pull(buf, len);

  BT_DBG("ch %p seg %p len %u", ch, seg, seg->len);

  return seg;
}

void l2cap_chan_sdu_sent(struct bt_conn *conn, void *user_data) {
  struct bt_l2cap_chan *chan = user_data;

  BT_DBG("conn %p chan %p", conn, chan);

  if (chan->ops->sent) {
    chan->ops->sent(chan);
  }
}

static int l2cap_chan_le_send(struct bt_l2cap_le_chan *ch, struct net_buf *buf, u16_t sdu_hdr_len) {
  struct net_buf *seg;
  int             len;

  /* Wait for credits */
  if (k_sem_take(&ch->tx.credits, K_NO_WAIT)) {
    BT_DBG("No credits to transmit packet");
    return -EAGAIN;
  }

  seg = l2cap_chan_create_seg(ch, buf, sdu_hdr_len);
  if (!seg) {
    return -ENOMEM;
  }

  /* Channel may have been disconnected while waiting for a buffer */
  if (!ch->chan.conn) {
    net_buf_unref(buf);
    return -ECONNRESET;
  }

  BT_DBG("ch %p cid 0x%04x len %u credits %u", ch, ch->tx.cid, seg->len, k_sem_count_get(&ch->tx.credits));

  len = seg->len - sdu_hdr_len;

  /* Set a callback if there is no data left in the buffer and sent
   * callback has been set.
   */
  if ((buf == seg || !buf->len) && ch->chan.ops->sent) {
    bt_l2cap_send_cb(ch->chan.conn, ch->tx.cid, seg, l2cap_chan_sdu_sent, &ch->chan);
  } else {
    bt_l2cap_send(ch->chan.conn, ch->tx.cid, seg);
  }

  /* Check if there is no credits left clear output status and notify its
   * change.
   */
  if (!k_sem_count_get(&ch->tx.credits)) {
    atomic_clear_bit(ch->chan.status, BT_L2CAP_STATUS_OUT);
    if (ch->chan.ops->status) {
      ch->chan.ops->status(&ch->chan, ch->chan.status);
    }
  }

  return len;
}

static int l2cap_chan_le_send_sdu(struct bt_l2cap_le_chan *ch, struct net_buf **buf, u16_t sent) {
  int             ret, total_len;
  struct net_buf *frag;

  total_len = net_buf_frags_len(*buf) + sent;

  if (total_len > ch->tx.mtu) {
    return -EMSGSIZE;
  }

  frag = *buf;
  if (!frag->len && frag->frags) {
    frag = frag->frags;
  }

  if (!sent) {
    /* Add SDU length for the first segment */
    ret = l2cap_chan_le_send(ch, frag, BT_L2CAP_SDU_HDR_LEN);
    if (ret < 0) {
      if (ret == -EAGAIN) {
        /* Store sent data into user_data */
        memcpy(net_buf_user_data(frag), &sent, sizeof(sent));
      }
      *buf = frag;
      return ret;
    }
    sent = ret;
  }

  /* Send remaining segments */
  for (ret = 0; sent < total_len; sent += ret) {
    /* Proceed to next fragment */
    if (!frag->len) {
      frag = net_buf_frag_del(NULL, frag);
    }

    ret = l2cap_chan_le_send(ch, frag, 0);
    if (ret < 0) {
      if (ret == -EAGAIN) {
        /* Store sent data into user_data */
        memcpy(net_buf_user_data(frag), &sent, sizeof(sent));
      }
      *buf = frag;
      return ret;
    }
  }

  BT_DBG("ch %p cid 0x%04x sent %u total_len %u", ch, ch->tx.cid, sent, total_len);

  net_buf_unref(frag);

  return ret;
}

static struct net_buf *l2cap_chan_le_get_tx_buf(struct bt_l2cap_le_chan *ch) {
  struct net_buf *buf;

  /* Return current buffer */
  if (ch->tx_buf) {
    buf        = ch->tx_buf;
    ch->tx_buf = NULL;
    return buf;
  }

  return net_buf_get(&ch->tx_queue, K_NO_WAIT);
}

static void l2cap_chan_le_send_resume(struct bt_l2cap_le_chan *ch) {
  struct net_buf *buf;

  /* Resume tx in case there are buffers in the queue */
  while ((buf = l2cap_chan_le_get_tx_buf(ch))) {
#if defined(BFLB_BLE)
    int sent = *((int *)net_buf_user_data(buf));
#else
    u16_t sent = *((u16_t *)net_buf_user_data(buf));
#endif

    BT_DBG("buf %p sent %u", buf, sent);

    sent = l2cap_chan_le_send_sdu(ch, &buf, sent);
    if (sent < 0) {
      if (sent == -EAGAIN) {
        ch->tx_buf = buf;
      }
      break;
    }
  }
}

static void le_credits(struct bt_l2cap *l2cap, u8_t ident, struct net_buf *buf) {
  struct bt_conn             *conn = l2cap->chan.chan.conn;
  struct bt_l2cap_chan       *chan;
  struct bt_l2cap_le_credits *ev = (void *)buf->data;
  struct bt_l2cap_le_chan    *ch;
  u16_t                       credits, cid;

  if (buf->len < sizeof(*ev)) {
    BT_ERR("Too small LE Credits packet size");
    return;
  }

  cid     = sys_le16_to_cpu(ev->cid);
  credits = sys_le16_to_cpu(ev->credits);

  BT_DBG("cid 0x%04x credits %u", cid, credits);

  chan = bt_l2cap_le_lookup_tx_cid(conn, cid);
  if (!chan) {
    BT_ERR("Unable to find channel of LE Credits packet");
    return;
  }

  ch = BT_L2CAP_LE_CHAN(chan);

  if (k_sem_count_get(&ch->tx.credits) + credits > UINT16_MAX) {
    BT_ERR("Credits overflow");
    bt_l2cap_chan_disconnect(chan);
    return;
  }

  l2cap_chan_tx_give_credits(ch, credits);

  BT_DBG("chan %p total credits %u", ch, k_sem_count_get(&ch->tx.credits));

  l2cap_chan_le_send_resume(ch);
}

static void reject_cmd(struct bt_l2cap *l2cap, u8_t ident, struct net_buf *buf) {
  struct bt_conn          *conn = l2cap->chan.chan.conn;
  struct bt_l2cap_le_chan *chan;

  /* Check if there is a outstanding channel */
  chan = l2cap_remove_ident(conn, ident);
  if (!chan) {
    return;
  }

  bt_l2cap_chan_del(&chan->chan);
}
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

static int l2cap_recv(struct bt_l2cap_chan *chan, struct net_buf *buf) {
  struct bt_l2cap         *l2cap = CONTAINER_OF(chan, struct bt_l2cap, chan);
  struct bt_l2cap_sig_hdr *hdr;
  u16_t                    len;

  if (buf->len < sizeof(*hdr)) {
    BT_ERR("Too small L2CAP signaling PDU");
    return 0;
  }

  hdr = net_buf_pull_mem(buf, sizeof(*hdr));
  len = sys_le16_to_cpu(hdr->len);

  BT_DBG("Signaling code 0x%02x ident %u len %u", hdr->code, hdr->ident, len);

  if (buf->len != len) {
    BT_ERR("L2CAP length mismatch (%u != %u)", buf->len, len);
    return 0;
  }

  if (!hdr->ident) {
    BT_ERR("Invalid ident value in L2CAP PDU");
    return 0;
  }

  switch (hdr->code) {
  case BT_L2CAP_CONN_PARAM_RSP:
    le_conn_param_rsp(l2cap, buf);
    break;
#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
  case BT_L2CAP_LE_CONN_REQ:
    le_conn_req(l2cap, hdr->ident, buf);
    break;
  case BT_L2CAP_LE_CONN_RSP:
    le_conn_rsp(l2cap, hdr->ident, buf);
    break;
  case BT_L2CAP_DISCONN_REQ:
    le_disconn_req(l2cap, hdr->ident, buf);
    break;
  case BT_L2CAP_DISCONN_RSP:
    le_disconn_rsp(l2cap, hdr->ident, buf);
    break;
  case BT_L2CAP_LE_CREDITS:
    le_credits(l2cap, hdr->ident, buf);
    break;
  case BT_L2CAP_CMD_REJECT:
    reject_cmd(l2cap, hdr->ident, buf);
    break;
#else
  case BT_L2CAP_CMD_REJECT:
    /* Ignored */
    break;
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */
  case BT_L2CAP_CONN_PARAM_REQ:
    if (IS_ENABLED(CONFIG_BT_CENTRAL)) {
      le_conn_param_update_req(l2cap, hdr->ident, buf);
      break;
    }
#if defined(BFLB_BLE)
    __attribute__((fallthrough));
#endif

  /* Fall-through */
  default:
    BT_WARN("Unknown L2CAP PDU code 0x%02x", hdr->code);
    l2cap_send_reject(chan->conn, hdr->ident, BT_L2CAP_REJ_NOT_UNDERSTOOD, NULL, 0);
    break;
  }

  return 0;
}

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
static void l2cap_chan_send_credits(struct bt_l2cap_le_chan *chan, struct net_buf *buf, u16_t credits) {
  struct bt_l2cap_le_credits *ev;

  /* Cap the number of credits given */
  if (credits > chan->rx.init_credits) {
    credits = chan->rx.init_credits;
  }

  l2cap_chan_rx_give_credits(chan, credits);

  buf = l2cap_create_le_sig_pdu(buf, BT_L2CAP_LE_CREDITS, get_ident(), sizeof(*ev));
  if (!buf) {
    return;
  }

  ev          = net_buf_add(buf, sizeof(*ev));
  ev->cid     = sys_cpu_to_le16(chan->rx.cid);
  ev->credits = sys_cpu_to_le16(credits);

  bt_l2cap_send(chan->chan.conn, BT_L2CAP_CID_LE_SIG, buf);

  BT_DBG("chan %p credits %u", chan, k_sem_count_get(&chan->rx.credits));
}

static void l2cap_chan_update_credits(struct bt_l2cap_le_chan *chan, struct net_buf *buf) {
  s16_t credits;

  /* Restore enough credits to complete the sdu */
  credits = ((chan->_sdu_len - net_buf_frags_len(buf)) + (chan->rx.mps - 1)) / chan->rx.mps;
  credits -= k_sem_count_get(&chan->rx.credits);
  if (credits <= 0) {
    return;
  }

  l2cap_chan_send_credits(chan, buf, credits);
}

int bt_l2cap_chan_recv_complete(struct bt_l2cap_chan *chan, struct net_buf *buf) {
  struct bt_l2cap_le_chan *ch   = BT_L2CAP_LE_CHAN(chan);
  struct bt_conn          *conn = chan->conn;
  u16_t                    credits;

  __ASSERT_NO_MSG(chan);
  __ASSERT_NO_MSG(buf);

  if (!conn) {
    return -ENOTCONN;
  }

  if (conn->type != BT_CONN_TYPE_LE) {
    return -ENOTSUP;
  }

  BT_DBG("chan %p buf %p", chan, buf);

  /* Restore credits used by packet */
  memcpy(&credits, net_buf_user_data(buf), sizeof(credits));

  l2cap_chan_send_credits(ch, buf, credits);

  net_buf_unref(buf);

  return 0;
}

static struct net_buf *l2cap_alloc_frag(s32_t timeout, void *user_data) {
  struct bt_l2cap_le_chan *chan = user_data;
  struct net_buf          *frag = NULL;

  frag = chan->chan.ops->alloc_buf(&chan->chan);
  if (!frag) {
    return NULL;
  }

  BT_DBG("frag %p tailroom %zu", frag, net_buf_tailroom(frag));

  return frag;
}

static void l2cap_chan_le_recv_sdu(struct bt_l2cap_le_chan *chan, struct net_buf *buf, u16_t seg) {
  int err;

  BT_DBG("chan %p len %zu", chan, net_buf_frags_len(buf));

  /* Receiving complete SDU, notify channel and reset SDU buf */
  err = chan->chan.ops->recv(&chan->chan, buf);
  if (err < 0) {
    if (err != -EINPROGRESS) {
      BT_ERR("err %d", err);
      bt_l2cap_chan_disconnect(&chan->chan);
      net_buf_unref(buf);
    }
    return;
  }

  l2cap_chan_send_credits(chan, buf, seg);
  net_buf_unref(buf);
}

static void l2cap_chan_le_recv_seg(struct bt_l2cap_le_chan *chan, struct net_buf *buf) {
  u16_t len;
  u16_t seg = 0U;

  len = net_buf_frags_len(chan->_sdu);
  if (len) {
    memcpy(&seg, net_buf_user_data(chan->_sdu), sizeof(seg));
  }

  if (len + buf->len > chan->_sdu_len) {
    BT_ERR("SDU length mismatch");
    bt_l2cap_chan_disconnect(&chan->chan);
    return;
  }

  seg++;
  /* Store received segments in user_data */
  memcpy(net_buf_user_data(chan->_sdu), &seg, sizeof(seg));

  BT_DBG("chan %p seg %d len %zu", chan, seg, net_buf_frags_len(buf));

  /* Append received segment to SDU */
  len = net_buf_append_bytes(chan->_sdu, buf->len, buf->data, K_NO_WAIT, l2cap_alloc_frag, chan);
  if (len != buf->len) {
    BT_ERR("Unable to store SDU");
    bt_l2cap_chan_disconnect(&chan->chan);
    return;
  }

  if (net_buf_frags_len(chan->_sdu) < chan->_sdu_len) {
    /* Give more credits if remote has run out of them, this
     * should only happen if the remote cannot fully utilize the
     * MPS for some reason.
     */
    if (!k_sem_count_get(&chan->rx.credits) && seg == chan->rx.init_credits) {
      l2cap_chan_update_credits(chan, buf);
    }
    return;
  }

  buf            = chan->_sdu;
  chan->_sdu     = NULL;
  chan->_sdu_len = 0U;

  l2cap_chan_le_recv_sdu(chan, buf, seg);
}

static void l2cap_chan_le_recv(struct bt_l2cap_le_chan *chan, struct net_buf *buf) {
  u16_t sdu_len;
  int   err;

  if (k_sem_take(&chan->rx.credits, K_NO_WAIT)) {
    BT_ERR("No credits to receive packet");
    bt_l2cap_chan_disconnect(&chan->chan);
    return;
  }

  /* Check if segments already exist */
  if (chan->_sdu) {
    l2cap_chan_le_recv_seg(chan, buf);
    return;
  }

  sdu_len = net_buf_pull_le16(buf);

  BT_DBG("chan %p len %u sdu_len %u", chan, buf->len, sdu_len);

  if (sdu_len > chan->rx.mtu) {
    BT_ERR("Invalid SDU length");
    bt_l2cap_chan_disconnect(&chan->chan);
    return;
  }

  /* Always allocate buffer from the channel if supported. */
  if (chan->chan.ops->alloc_buf) {
    chan->_sdu = chan->chan.ops->alloc_buf(&chan->chan);
    if (!chan->_sdu) {
      BT_ERR("Unable to allocate buffer for SDU");
      bt_l2cap_chan_disconnect(&chan->chan);
      return;
    }
    chan->_sdu_len = sdu_len;
    l2cap_chan_le_recv_seg(chan, buf);
    return;
  }

  err = chan->chan.ops->recv(&chan->chan, buf);
  if (err) {
    if (err != -EINPROGRESS) {
      BT_ERR("err %d", err);
      bt_l2cap_chan_disconnect(&chan->chan);
    }
    return;
  }

  l2cap_chan_send_credits(chan, buf, 1);
}
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

static void l2cap_chan_recv(struct bt_l2cap_chan *chan, struct net_buf *buf) {
#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
  struct bt_l2cap_le_chan *ch = BT_L2CAP_LE_CHAN(chan);

  if (L2CAP_LE_CID_IS_DYN(ch->rx.cid)) {
    net_buf_put(&ch->rx_queue, net_buf_ref(buf));
    k_work_submit(&ch->rx_work);
    return;
  }
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */

  BT_DBG("chan %p len %u", chan, buf->len);

  chan->ops->recv(chan, buf);
  net_buf_unref(buf);
}

void bt_l2cap_recv(struct bt_conn *conn, struct net_buf *buf) {
  struct bt_l2cap_hdr  *hdr;
  struct bt_l2cap_chan *chan;
  u16_t                 cid;

  if (IS_ENABLED(CONFIG_BT_BREDR) && conn->type == BT_CONN_TYPE_BR) {
    bt_l2cap_br_recv(conn, buf);
    return;
  }

  if (buf->len < sizeof(*hdr)) {
    BT_ERR("Too small L2CAP PDU received");
    net_buf_unref(buf);
    return;
  }

  hdr = net_buf_pull_mem(buf, sizeof(*hdr));
  cid = sys_le16_to_cpu(hdr->cid);

  BT_DBG("Packet for CID %u len %u", cid, buf->len);

  chan = bt_l2cap_le_lookup_rx_cid(conn, cid);
  if (!chan) {
    BT_WARN("Ignoring data for unknown CID 0x%04x", cid);
    net_buf_unref(buf);
    return;
  }

  l2cap_chan_recv(chan, buf);
}

int bt_l2cap_update_conn_param(struct bt_conn *conn, const struct bt_le_conn_param *param) {
  struct bt_l2cap_conn_param_req *req;
  struct net_buf                 *buf;

  buf = l2cap_create_le_sig_pdu(NULL, BT_L2CAP_CONN_PARAM_REQ, get_ident(), sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req               = net_buf_add(buf, sizeof(*req));
  req->min_interval = sys_cpu_to_le16(param->interval_min);
  req->max_interval = sys_cpu_to_le16(param->interval_max);
  req->latency      = sys_cpu_to_le16(param->latency);
  req->timeout      = sys_cpu_to_le16(param->timeout);

  bt_l2cap_send(conn, BT_L2CAP_CID_LE_SIG, buf);

  return 0;
}

static void l2cap_connected(struct bt_l2cap_chan *chan) { BT_DBG("ch %p cid 0x%04x", BT_L2CAP_LE_CHAN(chan), BT_L2CAP_LE_CHAN(chan)->rx.cid); }

static void l2cap_disconnected(struct bt_l2cap_chan *chan) { BT_DBG("ch %p cid 0x%04x", BT_L2CAP_LE_CHAN(chan), BT_L2CAP_LE_CHAN(chan)->rx.cid); }

static int l2cap_accept(struct bt_conn *conn, struct bt_l2cap_chan **chan) {
  int                             i;
  static struct bt_l2cap_chan_ops ops = {
      .connected    = l2cap_connected,
      .disconnected = l2cap_disconnected,
      .recv         = l2cap_recv,
  };

  BT_DBG("conn %p handle %u", conn, conn->handle);

  for (i = 0; i < ARRAY_SIZE(bt_l2cap_pool); i++) {
    struct bt_l2cap *l2cap = &bt_l2cap_pool[i];

    if (l2cap->chan.chan.conn) {
      continue;
    }

    l2cap->chan.chan.ops = &ops;
    *chan                = &l2cap->chan.chan;

    return 0;
  }

  BT_ERR("No available L2CAP context for conn %p", conn);

  return -ENOMEM;
}

BT_L2CAP_CHANNEL_DEFINE(le_fixed_chan, BT_L2CAP_CID_LE_SIG, l2cap_accept);

void bt_l2cap_init(void) {
#if defined(BFLB_BLE_DISABLE_STATIC_CHANNEL)
  static struct bt_l2cap_fixed_chan chan = {
      .cid    = BT_L2CAP_CID_LE_SIG,
      .accept = l2cap_accept,
  };

  bt_l2cap_le_fixed_chan_register(&chan);
#endif
  if (IS_ENABLED(CONFIG_BT_BREDR)) {
    bt_l2cap_br_init();
  }
}

#if defined(CONFIG_BT_L2CAP_DYNAMIC_CHANNEL)
static int l2cap_le_connect(struct bt_conn *conn, struct bt_l2cap_le_chan *ch, u16_t psm) {
  if (psm < L2CAP_LE_PSM_FIXED_START || psm > L2CAP_LE_PSM_DYN_END) {
    return -EINVAL;
  }

  l2cap_chan_tx_init(ch);
  l2cap_chan_rx_init(ch);

  if (!l2cap_chan_add(conn, &ch->chan, l2cap_chan_destroy)) {
    return -ENOMEM;
  }

  ch->chan.psm = psm;

  return l2cap_le_conn_req(ch);
}

int bt_l2cap_chan_connect(struct bt_conn *conn, struct bt_l2cap_chan *chan, u16_t psm) {
  BT_DBG("conn %p chan %p psm 0x%04x", conn, chan, psm);

  if (!conn || conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  if (!chan) {
    return -EINVAL;
  }

  if (IS_ENABLED(CONFIG_BT_BREDR) && conn->type == BT_CONN_TYPE_BR) {
    return bt_l2cap_br_chan_connect(conn, chan, psm);
  }

  if (chan->required_sec_level > BT_SECURITY_L4) {
    return -EINVAL;
  } else if (chan->required_sec_level == BT_SECURITY_L0) {
    chan->required_sec_level = BT_SECURITY_L1;
  }

  return l2cap_le_connect(conn, BT_L2CAP_LE_CHAN(chan), psm);
}

int bt_l2cap_chan_disconnect(struct bt_l2cap_chan *chan) {
  struct bt_conn              *conn = chan->conn;
  struct net_buf              *buf;
  struct bt_l2cap_disconn_req *req;
  struct bt_l2cap_le_chan     *ch;

  if (!conn) {
    return -ENOTCONN;
  }

  if (IS_ENABLED(CONFIG_BT_BREDR) && conn->type == BT_CONN_TYPE_BR) {
    return bt_l2cap_br_chan_disconnect(chan);
  }

  ch = BT_L2CAP_LE_CHAN(chan);

  BT_DBG("chan %p scid 0x%04x dcid 0x%04x", chan, ch->rx.cid, ch->tx.cid);

  ch->chan.ident = get_ident();

  buf = l2cap_create_le_sig_pdu(NULL, BT_L2CAP_DISCONN_REQ, ch->chan.ident, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req       = net_buf_add(buf, sizeof(*req));
  req->dcid = sys_cpu_to_le16(ch->rx.cid);
  req->scid = sys_cpu_to_le16(ch->tx.cid);

  l2cap_chan_send_req(ch, buf, L2CAP_DISC_TIMEOUT);
  bt_l2cap_chan_set_state(chan, BT_L2CAP_DISCONNECT);

  return 0;
}

int bt_l2cap_chan_send(struct bt_l2cap_chan *chan, struct net_buf *buf) {
  int err;

  if (!buf) {
    return -EINVAL;
  }

  BT_DBG("chan %p buf %p len %zu", chan, buf, net_buf_frags_len(buf));

  if (!chan->conn || chan->conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  if (IS_ENABLED(CONFIG_BT_BREDR) && chan->conn->type == BT_CONN_TYPE_BR) {
    return bt_l2cap_br_chan_send(chan, buf);
  }

  err = l2cap_chan_le_send_sdu(BT_L2CAP_LE_CHAN(chan), &buf, 0);
  if (err < 0) {
    if (err == -EAGAIN) {
      /* Queue buffer to be sent later */
      net_buf_put(&(BT_L2CAP_LE_CHAN(chan))->tx_queue, buf);
      return *((u16_t *)net_buf_user_data(buf));
    }
    BT_ERR("failed to send message %d", err);
  }

  return err;
}
#endif /* CONFIG_BT_L2CAP_DYNAMIC_CHANNEL */
