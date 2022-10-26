/****************************************************************************
FILE NAME
    ble_peripheral_tp_server.c

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
#include "ble_characteristics.h"
#include "bluetooth.h"
#include "conn.h"
#include "gatt.h"
#include "hci_core.h"
#include "uuid.h"
#include "ble_peripheral.h"
#include "log.h"
#include "hal_clock.h"

bool pds_start;

static void ble_device_connected(struct bt_conn *conn, u8_t err);
static void ble_device_disconnected(struct bt_conn *conn, u8_t reason);
static void ble_connection_param_changed(struct bt_conn *conn, u16_t interval, u16_t latency, u16_t timeout);

static struct bt_conn *ble_tp_conn;
static struct bt_gatt_exchange_params exchg_mtu;
static TaskHandle_t ble_tp_task_h;
static int tx_mtu_size = 20;
static u8_t created_tp_task = 0;

static struct bt_conn_cb ble_tp_conn_callbacks = {
    .connected = ble_device_connected,
    .disconnected = ble_device_disconnected,
    .le_param_updated = ble_connection_param_changed,
};

/*************************************************************************
NAME
    ble_tx_mtu_change_callback
*/
static void ble_tx_mtu_change_callback(struct bt_conn *conn, u8_t err,
                               struct bt_gatt_exchange_params *params)
{
    if (!err) {
        tx_mtu_size = bt_gatt_get_mtu(ble_tp_conn);
        BT_WARN("ble tp echange mtu size success, mtu size: %d", tx_mtu_size);
    } else {
        BT_WARN("ble tp echange mtu size failure, err: %d", err);
    }
}
/*************************************************************************
NAME
    ble_device_connected
*/
static void ble_device_connected(struct bt_conn *conn, u8_t err)
{
    int tx_octets = 0x00fb;
    int tx_time = 0x0848;
    int ret = -1;

    if (err) {
        return;
    }

    BT_INFO("BLE connected");
    ble_tp_conn = conn;
    pds_start = false;

    //set data length after connected.
    ret = bt_le_set_data_len(ble_tp_conn, tx_octets, tx_time);

    if (!ret) {
        BT_INFO("ble tp set data length success");
    } else {
        BT_WARN("ble tp set data length failure, err: %d", ret);
    }

    //exchange mtu size after connected.
    exchg_mtu.func = ble_tx_mtu_change_callback;
    ret = bt_gatt_exchange_mtu(ble_tp_conn, &exchg_mtu);

    if (!ret) {
        BT_INFO("ble tp exchange mtu size pending");
    } else {
        BT_WARN("ble tp exchange mtu size failure, err: %d", ret);
    }
}

/*************************************************************************
NAME
    ble_device_disconnected
*/
static void ble_device_disconnected(struct bt_conn *conn, u8_t reason)
{
    BT_WARN("Tp disconnected");

    if (created_tp_task) {
        BT_WARN("Delete throughput tx task");
        vTaskDelete(ble_tp_task_h);
        created_tp_task = 0;
    }

    ble_tp_conn = NULL;
    extern int ble_start_adv(void);
    ble_start_adv();
    pds_start = true;
}

/*************************************************************************
NAME
    ble_connection_param_changed
*/

static void ble_connection_param_changed(struct bt_conn *conn, u16_t interval,
                              u16_t latency, u16_t timeout)
{
    BT_INFO("LE conn param updated: int 0x%04x lat %d to %d \r\n", interval, latency, timeout);
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
    ble_tp_recv_wr(receive data from client)
*/
static int ble_tp_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *buf, u16_t len, u16_t offset, u8_t flags)
{
    BT_WARN("recv data len=%d, offset=%d, flag=%d", len, offset, flags);
    BT_WARN("recv data:%s", bt_hex(buf, len));

    if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
        //Don't use prepare write data, execute write will upload data again.
        BT_WARN("recv prepare write request");
        return 0;
    }

    if (flags & BT_GATT_WRITE_FLAG_CMD) {
        //Use write command data.
        BT_WARN("recv write command");
    } else {
        //Use write request / execute write data.
        BT_WARN("recv write request / exce write");
    }

    return len;
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
        BT_WARN("ble tp send indicate: %d", err);
    }
}


/*************************************************************************
*  DEFINE : attrs
*/
static struct bt_gatt_attr attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SVC_LIVE_DATA),

    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_SETPOINT_TEMP,
                           BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ,
                           ble_tp_recv_rd,
                           NULL,
                           NULL),




};

/*************************************************************************
NAME
    get_attr
*/
struct bt_gatt_attr *get_attr(u8_t index)
{
    return &attrs[index];
}

static struct bt_gatt_service ble_tp_server = BT_GATT_SERVICE(attrs);



const char* DEVICE_BLE_NAME="Pinecil";

//Start advertising with expected default values
int ble_start_adv(void)
{
    MSG("BLE Starting advertising\n");
    struct bt_le_adv_param adv_param = {
        //options:3, connectable undirected, adv one time
        .options = 3,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_3,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_3,
    };

    struct bt_data adv_data[2] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL)),
        BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_BLE_NAME, strlen(DEVICE_BLE_NAME)),
    };

    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), &adv_data[1], 1);
}



// Callback that the ble stack will call once it has been kicked off running
// We use this to register the handlers (as we know its now ready for them) + start advertising to the world
void bt_enable_cb(int err)
{
    bt_conn_cb_register(&ble_tp_conn_callbacks);
    bt_gatt_service_register(&ble_tp_server);
    
    ble_start_adv();
}