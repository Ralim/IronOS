/****************************************************************************
FILE NAME
    ble_central_tp_client.c

DESCRIPTION
    test profile demo

NOTES
*/
/****************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>

#include "bluetooth.h"
#include "conn.h"
#include "gatt.h"
#include "hci_core.h"
#include "uuid.h"
#include "ble_central_tp_client.h"
#include "log.h"

static void ble_subscribe();
static void ble_discover(u8_t type, u16_t start_handle);
static void ble_tp_connected(struct bt_conn *conn, u8_t err);
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason);

static struct bt_conn *ble_tp_conn;
static TaskHandle_t ble_write_data_task_h;

static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;
static struct tp_char_hdl char_hdl;
static struct k_sem write_data_poll_sem;

static u8_t created_write_data_task = 0;
static u8_t isRegister = 0;

static struct bt_conn_cb ble_tp_conn_callbacks = {
    .connected = ble_tp_connected,
    .disconnected = ble_tp_disconnected,
};

/*************************************************************************
NAME
    notify_func(receive data from server)
*/
static u8_t notify_func(struct bt_conn *conn,
                        struct bt_gatt_subscribe_params *params,
                        const void *data, u16_t length)
{
    static u32_t time = 0;
    static int len = 0;
    BT_WARN("notify_func complete");

    if (!params->value) {
        BT_WARN("Unsubscribed");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }

    if (!time) {
        time = k_now_ms();
    }

    len += length;

    if (k_now_ms() - time >= 1000) {
        BT_WARN("data_len=[%d]", len);
        time = k_now_ms();
        len = 0;
    }

    BT_WARN("Notification: data length %u", length);
    if (length != 0) {
        BT_WARN("The first 10 recv data %s", bt_hex(data, 10));
    }

    if (length) {
        k_sem_give(&write_data_poll_sem);
    }

    return BT_GATT_ITER_CONTINUE;
}

/*************************************************************************
NAME
    ble_write_data_task(send data to server)
*/
static void ble_write_data_task(void *pvParameters)
{
    int error;
    u8_t buf[20] = { 0x1 };

    while (1) {
        k_sem_take(&write_data_poll_sem, K_FOREVER);
        BT_WARN("ble_write_data");
        // Send data to server
        error = bt_gatt_write_without_response(ble_tp_conn, char_hdl.tp_wr_hdl, buf, 20, 0);
        buf[0] = buf[0] + 1;
        BT_WARN("Write Complete (err %d)", error);
    }
}

/*************************************************************************
NAME
    ble_subscribe
*/
static void ble_subscribe()
{
    if (!ble_tp_conn) {
        BT_WARN("Not connected");
        return;
    }

    subscribe_params.ccc_handle = char_hdl.tp_ccc_hdl;
    subscribe_params.value_handle = char_hdl.tp_notify_hdl;
    subscribe_params.value = 1;
    subscribe_params.notify = notify_func;

    int err = bt_gatt_subscribe(ble_tp_conn, &subscribe_params);

    if (err) {
        BT_WARN("Subscribe failed (err %d)", err);
    } else {
        BT_WARN("Subscribed");
    }

    if (!created_write_data_task && (xTaskCreate(ble_write_data_task, (char *)"ble_write_data", 512, NULL, 15, &ble_write_data_task_h) == pdPASS)) {
        created_write_data_task = 1;
        BT_WARN("Create write data  task success");
    } else {
        created_write_data_task = 0;
        BT_WARN("Create write data taskfail");
    }
}

/*************************************************************************
NAME
    ble_discover_func
*/
static uint8_t ble_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
    struct bt_gatt_chrc *gatt_chrc;
    char str[37];

    if (!attr) {
        BT_WARN("Discover complete");

        if (params->type != BT_GATT_DISCOVER_DESCRIPTOR) {
            ble_discover(BT_GATT_DISCOVER_DESCRIPTOR, char_hdl.tp_notify_hdl);
            return BT_GATT_ITER_STOP;
        }
    }

    if (params == NULL) {
        BT_WARN("ble_discover_func_PARAMS");
    }

    switch (params->type) {
        case BT_GATT_DISCOVER_PRIMARY:
            break;

        case BT_GATT_DISCOVER_SECONDARY:
            break;

        case BT_GATT_DISCOVER_CHARACTERISTIC:
            gatt_chrc = attr->user_data;
            bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));

            if (!bt_uuid_cmp(gatt_chrc->uuid, BT_UUID_CHAR_BLE_TP_RD)) {
                char_hdl.tp_rd_hdl = gatt_chrc->value_handle;
            } else if (!bt_uuid_cmp(gatt_chrc->uuid, BT_UUID_CHAR_BLE_TP_WR)) {
                char_hdl.tp_wr_hdl = gatt_chrc->value_handle;
            } else if (!bt_uuid_cmp(gatt_chrc->uuid, BT_UUID_CHAR_BLE_TP_IND)) {
                char_hdl.tp_ind_hdl = gatt_chrc->value_handle;
            } else if (!bt_uuid_cmp(gatt_chrc->uuid, BT_UUID_CHAR_BLE_TP_NOT)) {
                char_hdl.tp_notify_hdl = gatt_chrc->value_handle;
            }

            break;

        case BT_GATT_DISCOVER_INCLUDE:
            break;

        case BT_GATT_DISCOVER_DESCRIPTOR:
            if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC)) {
                char_hdl.tp_ccc_hdl = attr->handle;
                (void)memset(params, 0, sizeof(*params));
                ble_subscribe();
                return BT_GATT_ITER_STOP;
            }

            break;

        default:
            break;
    }

    return BT_GATT_ITER_CONTINUE;
}

/*************************************************************************
NAME
    ble_discover
*/
static void ble_discover(u8_t type, u16_t start_handle)
{
    int err;

    if (!ble_tp_conn) {
        return;
    }

    discover_params.func = ble_discover_func;
    discover_params.start_handle = start_handle;
    discover_params.end_handle = 0xffff;
    discover_params.type = type;
    discover_params.uuid = NULL;

    err = bt_gatt_discover(ble_tp_conn, &discover_params);

    if (err) {
        BT_WARN("Discover failed (err %d)", err);
    } else {
        BT_WARN("Discover pending");
    }
}

/*************************************************************************
NAME
    ble_tp_connected
*/
static void ble_tp_connected(struct bt_conn *conn, u8_t err)
{
    int tx_octets = 0x00fb;
    int tx_time = 0x0848;
    int ret = -1;

    if (err) {
        return;
    }

    BT_WARN("TP connected");
    ble_tp_conn = conn;

    //set data length after connected.
    ret = bt_le_set_data_len(ble_tp_conn, tx_octets, tx_time);

    if (!ret) {
        BT_WARN("ble tp set data length success");
    } else {
        BT_WARN("ble tp set data length failure, err: %d", ret);
    }

    //discover after connected.
    ble_discover(BT_GATT_DISCOVER_CHARACTERISTIC, 0x0001);
}

/*************************************************************************
NAME
    ble_tp_disconnected
*/
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason)
{
    ble_tp_conn = NULL;
    BT_WARN("TP Disconnected");
    if (created_write_data_task) {
        BT_WARN("Delete write data  task");
        vTaskDelete(ble_write_data_task_h);
        created_write_data_task = 0;
    }
}

/*************************************************************************
NAME
    ble_tp_init
*/
void ble_tp_init()
{
    if (!isRegister) {
        isRegister = 1;
        bt_conn_cb_register(&ble_tp_conn_callbacks);
        k_sem_init(&write_data_poll_sem, 0, 1);
    }
}
