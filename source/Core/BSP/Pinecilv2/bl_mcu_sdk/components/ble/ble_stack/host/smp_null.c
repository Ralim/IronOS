/**
 * @file smp_null.c
 * Security Manager Protocol stub
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <errno.h>
#include <atomic.h>
#include <misc/util.h>

#include <bluetooth.h>
#include <conn.h>
#include <../include/bluetooth/buf.h>

#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_HCI_CORE)
#define LOG_MODULE_NAME bt_smp
#include "log.h"

#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap_internal.h"
#include "smp.h"

static struct bt_l2cap_le_chan bt_smp_pool[CONFIG_BT_MAX_CONN];

int bt_smp_sign_verify(struct bt_conn *conn, struct net_buf *buf)
{
    return -ENOTSUP;
}

int bt_smp_sign(struct bt_conn *conn, struct net_buf *buf)
{
    return -ENOTSUP;
}

static int bt_smp_recv(struct bt_l2cap_chan *chan, struct net_buf *buf)
{
    struct bt_conn *conn = chan->conn;
    struct bt_smp_pairing_fail *rsp;
    struct bt_smp_hdr *hdr;

    /* If a device does not support pairing then it shall respond with
	 * a Pairing Failed command with the reason set to "Pairing Not
	 * Supported" when any command is received.
	 * Core Specification Vol. 3, Part H, 3.3
	 */

    buf = bt_l2cap_create_pdu(NULL, 0);
    /* NULL is not a possible return due to K_FOREVER */

    hdr = net_buf_add(buf, sizeof(*hdr));
    hdr->code = BT_SMP_CMD_PAIRING_FAIL;

    rsp = net_buf_add(buf, sizeof(*rsp));
    rsp->reason = BT_SMP_ERR_PAIRING_NOTSUPP;

    bt_l2cap_send(conn, BT_L2CAP_CID_SMP, buf);

    return 0;
}

static int bt_smp_accept(struct bt_conn *conn, struct bt_l2cap_chan **chan)
{
    int i;
    static struct bt_l2cap_chan_ops ops = {
        .recv = bt_smp_recv,
    };

    BT_DBG("conn %p handle %u", conn, conn->handle);

    for (i = 0; i < ARRAY_SIZE(bt_smp_pool); i++) {
        struct bt_l2cap_le_chan *smp = &bt_smp_pool[i];

        if (smp->chan.conn) {
            continue;
        }

        smp->chan.ops = &ops;

        *chan = &smp->chan;

        return 0;
    }

    BT_ERR("No available SMP context for conn %p", conn);

    return -ENOMEM;
}

BT_L2CAP_CHANNEL_DEFINE(smp_fixed_chan, BT_L2CAP_CID_SMP, bt_smp_accept);

int bt_smp_init(void)
{
    return 0;
}
