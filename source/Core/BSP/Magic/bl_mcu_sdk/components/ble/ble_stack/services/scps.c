/**
 ****************************************************************************************
 *
 * @file Scps.c
 *
 * @brief Bouffalo Lab GATT Scan Parameters Service implementation
 *
 * Copyright (C) Bouffalo Lab 2019
 *
 * History: 2019-08 crealted by llgong @ Shanghai
 *
 ****************************************************************************************
 */
#include "bluetooth.h"
#include "gatt.h"
#include "uuid.h"
#include "scps.h"
#include "byteorder.h"

struct scan_intvl_win {
    u16_t scan_intvl;
    u16_t scan_win;
} __packed;

static struct scan_intvl_win intvl_win = {
    .scan_intvl = BT_GAP_SCAN_FAST_INTERVAL,
    .scan_win = BT_GAP_SCAN_FAST_WINDOW,
};

static ssize_t scan_intvl_win_write(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr, const void *buf,
                                    u16_t len, u16_t offset, u8_t flags)
{
    const u8_t *data = buf;
    intvl_win.scan_intvl = sys_get_le16(data);
    data += 2;
    intvl_win.scan_win = sys_get_le16(data);

    return len;
}

static struct bt_gatt_attr attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SCPS),
    BT_GATT_CHARACTERISTIC(BT_UUID_SCPS_SCAN_INTVL_WIN,
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_NONE, NULL, NULL,
                           &intvl_win)
};

static struct bt_gatt_service scps = BT_GATT_SERVICE(attrs);

bool scps_init(u16_t scan_intvl, u16_t scan_win)
{
    int err;

    if (scan_intvl < 0x0004 || scan_intvl > 0x4000) {
        return false;
    }

    if (scan_win < 0x0004 || scan_win > 0x4000) {
        return false;
    }

    if (scan_win > scan_intvl) {
        return false;
    }

    intvl_win.scan_intvl = scan_intvl;
    intvl_win.scan_win = scan_win;

    err = bt_gatt_service_register(&scps);

    return err ? false : true;
}
