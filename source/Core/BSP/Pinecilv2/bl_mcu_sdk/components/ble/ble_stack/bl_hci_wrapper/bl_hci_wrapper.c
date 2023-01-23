/*****************************************************************************************
*
* @file bl_hci_wrapper.c
*
* @brief Bouffalo Lab hci wrapper functions
*
* Copyright (C) Bouffalo Lab 2018
*
* History: 2018-08 crealted by llgong @ Shanghai
*
*****************************************************************************************/

#include <string.h>
#include <log.h>
#include "hci_host.h"
#include "bl_hci_wrapper.h"
#include "hci_driver.h"
#include "../common/include/errno.h"
#include "byteorder.h"
#include "hci_onchip.h"

#define DATA_MSG_CNT 10

struct rx_msg_struct data_msg[DATA_MSG_CNT];
struct k_queue msg_queue;
#if defined(BFLB_BLE_NOTIFY_ADV_DISCARDED)
extern void ble_controller_notify_adv_discarded(uint8_t *adv_bd_addr, uint8_t adv_type);
#endif

struct rx_msg_struct *bl_find_valid_data_msg()
{
    struct rx_msg_struct empty_msg;
    memset(&empty_msg, 0, sizeof(struct rx_msg_struct));

    for (int i = 0; i < DATA_MSG_CNT; i++) {
        if (!memcmp(&data_msg[i], &empty_msg, sizeof(struct rx_msg_struct))) {
            return (data_msg + i);
        }
    }

    return NULL;
}

int bl_onchiphci_send_2_controller(struct net_buf *buf)
{
    uint16_t opcode;
    uint16_t dest_id = 0x00;
    uint8_t buf_type;
    uint8_t pkt_type;
    hci_pkt_struct pkt;

    buf_type = bt_buf_get_type(buf);
    switch (buf_type) {
        case BT_BUF_CMD: {
            struct bt_hci_cmd_hdr *chdr;

            if (buf->len < sizeof(struct bt_hci_cmd_hdr)) {
                return -EINVAL;
            }

            chdr = (void *)buf->data;

            if (buf->len < chdr->param_len) {
                return -EINVAL;
            }

            pkt_type = BT_HCI_CMD;
            opcode = sys_le16_to_cpu(chdr->opcode);
            //move buf to the payload
            net_buf_pull(buf, sizeof(struct bt_hci_cmd_hdr));
            switch (opcode) {
                //ble refer to hci_cmd_desc_tab_le, for the ones of which dest_ll is BLE_CTRL
                case BT_HCI_OP_LE_CONN_UPDATE:
                case BT_HCI_OP_LE_READ_CHAN_MAP:
                case BT_HCI_OP_LE_READ_REMOTE_FEATURES:
                case BT_HCI_OP_LE_START_ENCRYPTION:
                case BT_HCI_OP_LE_LTK_REQ_REPLY:
                case BT_HCI_OP_LE_LTK_REQ_NEG_REPLY:
                case BT_HCI_OP_LE_CONN_PARAM_REQ_REPLY:
                case BT_HCI_OP_LE_CONN_PARAM_REQ_NEG_REPLY:
                case BT_HCI_OP_LE_SET_DATA_LEN:
                case BT_HCI_OP_LE_READ_PHY:
                case BT_HCI_OP_LE_SET_PHY:
                //bredr identify link id, according to dest_id
                case BT_HCI_OP_READ_REMOTE_FEATURES:
                case BT_HCI_OP_READ_REMOTE_EXT_FEATURES:
                case BT_HCI_OP_READ_ENCRYPTION_KEY_SIZE: {
                    //dest_id is connectin handle
                    dest_id = buf->data[0];
                }
                default:
                    break;
            }
            pkt.p.hci_cmd.opcode = opcode;
            pkt.p.hci_cmd.param_len = chdr->param_len;
            pkt.p.hci_cmd.params = buf->data;

            break;
        }

        case BT_BUF_ACL_OUT: {
            struct bt_hci_acl_hdr *acl;
            //connhandle +l2cap field
            uint16_t connhdl_l2cf, tlt_len;

            if (buf->len < sizeof(struct bt_hci_acl_hdr)) {
                return -EINVAL;
            }

            pkt_type = BT_HCI_ACL_DATA;
            acl = (void *)buf->data;
            tlt_len = sys_le16_to_cpu(acl->len);
            connhdl_l2cf = sys_le16_to_cpu(acl->handle);
            //move buf to the payload
            net_buf_pull(buf, sizeof(struct bt_hci_acl_hdr));

            if (buf->len < tlt_len) {
                return -EINVAL;
            }

            //get connection_handle
            dest_id = bt_acl_handle(connhdl_l2cf);
            pkt.p.acl_data.conhdl = dest_id;
            pkt.p.acl_data.pb_bc_flag = bt_acl_flags(connhdl_l2cf);
            pkt.p.acl_data.len = tlt_len;
            pkt.p.acl_data.buffer = (uint8_t *)buf->data;

            break;
        }

        default:
            return -EINVAL;
    }

    return bt_onchiphci_send(pkt_type, dest_id, &pkt);
}

void bl_packet_to_host(uint8_t pkt_type, uint16_t src_id, uint8_t *param, uint8_t param_len, struct net_buf *buf)
{
    uint16_t tlt_len;
    bool prio = true;
    uint8_t nb_h2c_cmd_pkts = 0x01;

    uint8_t *buf_data = net_buf_tail(buf);
    bt_buf_set_rx_adv(buf, false);

    switch (pkt_type) {
        case BT_HCI_CMD_CMP_EVT: {
            tlt_len = BT_HCI_EVT_CC_PARAM_OFFSET + param_len;
            *buf_data++ = BT_HCI_EVT_CMD_COMPLETE;
            *buf_data++ = BT_HCI_CCEVT_HDR_PARLEN + param_len;
            *buf_data++ = nb_h2c_cmd_pkts;
            sys_put_le16(src_id, buf_data);
            buf_data += 2;
            memcpy(buf_data, param, param_len);
            break;
        }
        case BT_HCI_CMD_STAT_EVT: {
            tlt_len = BT_HCI_CSEVT_LEN;
            *buf_data++ = BT_HCI_EVT_CMD_STATUS;
            *buf_data++ = BT_HCI_CSVT_PARLEN;
            *buf_data++ = *(uint8_t *)param;
            *buf_data++ = nb_h2c_cmd_pkts;
            sys_put_le16(src_id, buf_data);
            break;
        }
        case BT_HCI_LE_EVT: {
            prio = false;
            bt_buf_set_type(buf, BT_BUF_EVT);
            if (param[0] == BT_HCI_EVT_LE_ADVERTISING_REPORT) {
                bt_buf_set_rx_adv(buf, true);
            }
            tlt_len = BT_HCI_EVT_LE_PARAM_OFFSET + param_len;
            *buf_data++ = BT_HCI_EVT_LE_META_EVENT;
            *buf_data++ = param_len;
            memcpy(buf_data, param, param_len);
            break;
        }
        case BT_HCI_EVT: {
            if (src_id != BT_HCI_EVT_NUM_COMPLETED_PACKETS) {
                prio = false;
            }
            bt_buf_set_type(buf, BT_BUF_EVT);
            tlt_len = BT_HCI_EVT_LE_PARAM_OFFSET + param_len;
            *buf_data++ = src_id;
            *buf_data++ = param_len;
            memcpy(buf_data, param, param_len);
            break;
        }
        case BT_HCI_ACL_DATA: {
            prio = false;
            bt_buf_set_type(buf, BT_BUF_ACL_IN);
            tlt_len = bt_onchiphci_hanlde_rx_acl(param, buf_data);
            break;
        }
        default: {
            net_buf_unref(buf);
            return;
        }
    }

    net_buf_add(buf, tlt_len);

    if (prio) {
        bt_recv_prio(buf);
    } else {
        hci_driver_enque_recvq(buf);
    }
}

void bl_trigger_queued_msg()
{
    struct net_buf *buf = NULL;
    struct rx_msg_struct *msg = NULL;

    do {
        unsigned int lock = irq_lock();

        if (k_queue_is_empty(&msg_queue)) {
            break;
        }

        if (bt_buf_get_rx_avail_cnt() <= CONFIG_BT_RX_BUF_RSV_COUNT)
            break;

        buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_NO_WAIT);
        if (!buf) {
            break;
        }

        msg = k_fifo_get(&msg_queue, K_NO_WAIT);

        BT_ASSERT(msg);

        bl_packet_to_host(msg->pkt_type, msg->src_id, msg->param, msg->param_len, buf);

        irq_unlock(lock);

        if (msg->param) {
            k_free(msg->param);
        }
        memset(msg, 0, sizeof(struct rx_msg_struct));

    } while (buf);
}

static void bl_onchiphci_rx_packet_handler(uint8_t pkt_type, uint16_t src_id, uint8_t *param, uint8_t param_len)
{
    struct net_buf *buf = NULL;
    struct rx_msg_struct *rx_msg = NULL;

    if (pkt_type == BT_HCI_CMD_CMP_EVT || pkt_type == BT_HCI_CMD_STAT_EVT) {
        buf = bt_buf_get_cmd_complete(K_FOREVER);
        bl_packet_to_host(pkt_type, src_id, param, param_len, buf);
        return;
    } else if (pkt_type == BT_HCI_LE_EVT && param[0] == BT_HCI_EVT_LE_ADVERTISING_REPORT) {
        if (bt_buf_get_rx_avail_cnt() <= CONFIG_BT_RX_BUF_RSV_COUNT) {
            BT_INFO("Discard adv report.");
#if defined(BFLB_BLE_NOTIFY_ADV_DISCARDED)
            ble_controller_notify_adv_discarded(&param[4], param[2]);
#endif
            return;
        }
        buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_NO_WAIT);
        if (buf)
            bl_packet_to_host(pkt_type, src_id, param, param_len, buf);
        return;
    } else {
        if (pkt_type != BT_HCI_ACL_DATA) {
            /* Using the reserved buf (CONFIG_BT_RX_BUF_RSV_COUNT) firstly. */
            buf = bt_buf_get_rx(BT_BUF_ACL_IN, K_NO_WAIT);
            if (buf) {
                bl_packet_to_host(pkt_type, src_id, param, param_len, buf);
                return;
            }
        }

        rx_msg = bl_find_valid_data_msg();
    }

    BT_ASSERT(rx_msg);

    rx_msg->pkt_type = pkt_type;
    rx_msg->src_id = src_id;
    rx_msg->param_len = param_len;
    if (param_len) {
        rx_msg->param = k_malloc(param_len);
        memcpy(rx_msg->param, param, param_len);
    }

    k_fifo_put(&msg_queue, rx_msg);

    bl_trigger_queued_msg();
}

uint8_t bl_onchiphci_interface_init(void)
{
    for (int i = 0; i < DATA_MSG_CNT; i++) {
        memset(data_msg + i, 0, sizeof(struct rx_msg_struct));
    }

    k_queue_init(&msg_queue, DATA_MSG_CNT);

    return bt_onchiphci_interface_init(bl_onchiphci_rx_packet_handler);
}

void bl_onchiphci_interface_deinit(void)
{
    struct rx_msg_struct *msg;

    do {
        msg = k_fifo_get(&msg_queue, K_NO_WAIT);
        if (msg) {
            if (msg->param) {
                k_free(msg->param);
            }
        } else {
            break;
        }
    } while (1);

    k_queue_free(&msg_queue);
}
