/****************************************************************************
FILE NAME
    ble_tp_svc.c

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
#include <blog.h>

#include "bluetooth.h"
#include "conn.h"
#include "gatt.h"
#include "hci_core.h"
#include "uuid.h"
#include "ble_tp_svc.h"
#include "log.h"

static void ble_tp_connected(struct bt_conn *conn, u8_t err);
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason);

struct bt_conn *ble_tp_conn;
struct bt_gatt_exchange_params exchg_mtu;
TaskHandle_t ble_tp_task_h;

int tx_mtu_size = 20;
u8_t tp_start = 0;
static u8_t created_tp_task = 0;
static u8_t isRegister = 0;

static struct bt_conn_cb ble_tp_conn_callbacks = {
    .connected = ble_tp_connected,
    .disconnected = ble_tp_disconnected,
};

/*************************************************************************
NAME
    ble_tp_tx_mtu_size
*/
static void ble_tp_tx_mtu_size(struct bt_conn *conn, u8_t err,
                               struct bt_gatt_exchange_params *params)
{
    if (!err) {
        tx_mtu_size = bt_gatt_get_mtu(ble_tp_conn);
        BT_WARN("ble tp echange mtu size success, mtu size: %d\n", tx_mtu_size);
    } else {
        BT_WARN("ble tp echange mtu size failure, err: %d\n", err);
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

    if (err)
        return;

    printf("%s\n", __func__);
    ble_tp_conn = conn;

    //set data length after connected.
    ret = bt_le_set_data_len(ble_tp_conn, tx_octets, tx_time);
    if (!ret) {
        BT_WARN("ble tp set data length success.\n");
    } else {
        BT_WARN("ble tp set data length failure, err: %d\n", ret);
    }

    //exchange mtu size after connected.
    exchg_mtu.func = ble_tp_tx_mtu_size;
    ret = bt_gatt_exchange_mtu(ble_tp_conn, &exchg_mtu);
    if (!ret) {
        BT_WARN("ble tp exchange mtu size pending.\n");
    } else {
        BT_WARN("ble tp exchange mtu size failure, err: %d\n", ret);
    }
}

/*************************************************************************
NAME    
    ble_tp_disconnected
*/
static void ble_tp_disconnected(struct bt_conn *conn, u8_t reason)
{
    BT_WARN("%s\n", __func__);

    ble_tp_conn = NULL;
}

/*************************************************************************
NAME
    ble_tp_recv_rd
*/
static int ble_tp_recv_rd(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, u16_t len, u16_t offset)
{
    int size = 9;
    char data[9] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

    memcpy(buf, data, size);

    return size;
}

/*************************************************************************
NAME
    ble_tp_recv_wr
*/
static int ble_tp_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *buf, u16_t len, u16_t offset, u8_t flags)
{
    BT_INFO("recv data len=%d, offset=%d, flag=%d\r\n", len, offset, flags);

    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        //Don't use prepare write data, execute write will upload data again.
        BT_WARN("rcv prepare write request\n");
        return 0;
    }

    if (flags & BT_GATT_WRITE_FLAG_CMD) {
        //Use write command data.
        BT_INFO("rcv write command\n");
    } else {
        //Use write request / execute write data.
        BT_INFO("rcv write request / exce write\n");
    }

    return len;
}

/*************************************************************************
NAME    
    indicate_rsp /bl_tp_send_indicate
*/
void indicate_rsp(struct bt_conn *conn, const struct bt_gatt_attr *attr, u8_t err)
{
    BT_WARN("receive confirm, err:%d\n", err);
}

static int bl_tp_send_indicate(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               const void *data, u16_t len)
{
    //indicate paramete must be allocated statically
    static struct bt_gatt_indicate_params ind_params;
    ind_params.attr = attr;
    ind_params.data = data;
    ind_params.len = len;
    ind_params.func = indicate_rsp;
    ind_params.uuid = NULL;

    return bt_gatt_indicate(conn, &ind_params);
}

/*************************************************************************
NAME
    ble_tp_ind_ccc_changed
*/
static void ble_tp_ind_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    int err = -1;
    char data[9] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

    if (value == BT_GATT_CCC_INDICATE) {
        err = bl_tp_send_indicate(ble_tp_conn, get_attr(BT_CHAR_BLE_TP_IND_ATTR_VAL_INDEX), data, 9);
        BT_WARN("ble tp send indatcate: %d\n", err);
    }
}

/*************************************************************************
NAME
    ble_tp_notify
*/
static void ble_tp_notify_task(void *pvParameters)
{
    int err = -1;
    char data[244] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };

    while (1) {
        err = bt_gatt_notify(ble_tp_conn, get_attr(BT_CHAR_BLE_TP_NOT_ATTR_VAL_INDEX), data, (tx_mtu_size - 3));
        BT_WARN("ble tp send notify : %d\n", err);
    }
}

/*************************************************************************
NAME    
    ble_tp_not_ccc_changed
*/
static void ble_tp_not_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    BT_WARN("ccc:value=[%d]\r\n", value);

    if (tp_start) {
        if (value == BT_GATT_CCC_NOTIFY) {
            if (xTaskCreate(ble_tp_notify_task, (char *)"bletp", 256, NULL, 15, &ble_tp_task_h) == pdPASS) {
                created_tp_task = 1;
                BT_WARN("Create throughput tx task success .\n");
            } else {
                created_tp_task = 0;
                BT_WARN("Create throughput tx taskfail .\n");
            }
        } else {
            if (created_tp_task) {
                BT_WARN("Delete throughput tx task .\n");
                vTaskDelete(ble_tp_task_h);
                created_tp_task = 0;
            }
        }
    } else if (tp_start == 0) {
        if (created_tp_task) {
            BT_WARN("Delete throughput tx task .\n");
            vTaskDelete(ble_tp_task_h);
            created_tp_task = 0;
        }
    }
}

/*************************************************************************
*  DEFINE : attrs 
*/
static struct bt_gatt_attr attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SVC_BLE_TP),

    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_RD,
                           BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ,
                           ble_tp_recv_rd,
                           NULL,
                           NULL),

    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_WR,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE | BT_GATT_PERM_PREPARE_WRITE,
                           NULL,
                           ble_tp_recv_wr,
                           NULL),

    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_IND,
                           BT_GATT_CHRC_INDICATE,
                           NULL,
                           NULL,
                           NULL,
                           NULL),

    BT_GATT_CCC(ble_tp_ind_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_TP_NOT,
                           BT_GATT_CHRC_NOTIFY,
                           NULL,
                           NULL,
                           NULL,
                           NULL),

    BT_GATT_CCC(ble_tp_not_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)

};

/*************************************************************************
NAME    
    get_attr
*/
struct bt_gatt_attr *get_attr(u8_t index)
{
    return &attrs[index];
}

struct bt_gatt_service ble_tp_server = BT_GATT_SERVICE(attrs);

/*************************************************************************
NAME    
    ble_tp_init
*/
void ble_tp_init()
{
    if (!isRegister) {
        isRegister = 1;
        bt_conn_cb_register(&ble_tp_conn_callbacks);
        bt_gatt_service_register(&ble_tp_server);
    }
}
