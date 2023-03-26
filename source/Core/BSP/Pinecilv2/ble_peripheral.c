/****************************************************************************
FILE NAME
    ble_peripheral_tp_server.c

DESCRIPTION
    test profile demo

NOTES
*/
/****************************************************************************/

#include <FreeRTOS.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <task.h>

#include "types.h"

#include "ble_peripheral.h"
#include "bluetooth.h"
#include "conn.h"
#include "gatt.h"
#include "hal_clock.h"
#include "hci_core.h"
#include "log.h"
#include "uuid.h"

#include "BSP.h"
#include "ble_characteristics.h"
#include "ble_handlers.h"
bool pds_start;

static void          ble_device_connected(struct bt_conn *conn, u8_t err);
static void          ble_device_disconnected(struct bt_conn *conn, u8_t reason);
static void          ble_connection_param_changed(struct bt_conn *conn, u16_t interval, u16_t latency, u16_t timeout);
struct bt_gatt_attr *get_attr(u8_t index);

static struct bt_conn                *ble_tp_conn;
static struct bt_gatt_exchange_params exchg_mtu;
static TaskHandle_t                   ble_tp_task_h;
static int                            tx_mtu_size     = 20;
static u8_t                           created_tp_task = 0;

static struct bt_conn_cb ble_tp_conn_callbacks = {
    .connected        = ble_device_connected,
    .disconnected     = ble_device_disconnected,
    .le_param_updated = ble_connection_param_changed,
};

/*************************************************************************
NAME
    ble_tx_mtu_change_callback
*/
static void ble_tx_mtu_change_callback(struct bt_conn *conn, u8_t err, struct bt_gatt_exchange_params *params) {
  if (!err) {
    tx_mtu_size = bt_gatt_get_mtu(ble_tp_conn);
    BT_WARN("ble tp echange mtu size success, mtu size: %d\n", tx_mtu_size);
  } else {
    BT_WARN("ble tp echange mtu size failure, err: %d\n", err);
  }
}
/*************************************************************************
NAME
    ble_device_connected
*/
static void ble_device_connected(struct bt_conn *conn, u8_t err) {
  int tx_octets = 0x00fb;
  int tx_time   = 0x0848;
  int ret       = -1;

  if (err) {
    return;
  }

  BT_INFO("BLE connected");
  ble_tp_conn = conn;
  pds_start   = false;

  // set data length after connected.
  ret = bt_le_set_data_len(ble_tp_conn, tx_octets, tx_time);

  if (!ret) {
    BT_INFO("ble tp set data length success\n");
  } else {
    BT_WARN("ble tp set data length failure, err: %d\n", ret);
  }

  // exchange mtu size after connected.
  exchg_mtu.func = ble_tx_mtu_change_callback;
  ret            = bt_gatt_exchange_mtu(ble_tp_conn, &exchg_mtu);

  if (!ret) {
    BT_INFO("ble tp exchange mtu size pending\n");
  } else {
    BT_WARN("ble tp exchange mtu size failure, err: %d\n", ret);
  }
}

/*************************************************************************
NAME
    ble_device_disconnected
*/
static void ble_device_disconnected(struct bt_conn *conn, u8_t reason) {
  BT_WARN("Tp disconnected");

  if (created_tp_task) {
    BT_WARN("Delete throughput tx task\n");
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

static void ble_connection_param_changed(struct bt_conn *conn, u16_t interval, u16_t latency, u16_t timeout) {
  BT_INFO("LE conn param updated: int 0x%04x lat %d to %d \r\n", interval, latency, timeout);
}

/*************************************************************************
NAME
    ble_tp_ind_ccc_changed
*/
static void ble_tp_ind_ccc_changed(const struct bt_gatt_attr *attr, u16_t value) {
  int  err     = -1;
  char data[9] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};

  if (value == BT_GATT_CCC_INDICATE) {
    err = bl_tp_send_indicate(ble_tp_conn, get_attr(BT_CHAR_BLE_TP_IND_ATTR_VAL_INDEX), data, 9);
    BT_WARN("ble tp send indicate: %d\n", err);
  }
}

/*************************************************************************
 *  DEFINE : attrs
 */
static struct bt_gatt_attr ble_attrs_declaration[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_SVC_LIVE_DATA),

    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_LIVE_TEMP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_SETPOINT_TEMP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_DC_INPUT, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_HANDLE_TEMP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_POWER_LEVEL, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_POWER_SRC, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_TIP_RES, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_UPTIME, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_MOVEMENT, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_MAX_TEMP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_RAW_TIP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_HALL_SENSOR, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_OP_MODE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_EST_WATTS, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_status_callback, NULL, NULL),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_SVC_BULK_DATA),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_BULK_LIVE_DATA, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_bulk_value_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_ACCEL_NAME, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_bulk_value_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_BUILD, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_bulk_value_callback, NULL, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_LIVE_DEV_ID, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, ble_char_read_bulk_value_callback, NULL, NULL),

    BT_GATT_PRIMARY_SERVICE(BT_UUID_SVC_SETTINGS_DATA),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_0, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_1, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_2, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_3, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_4, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_5, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_6, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_7, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_8, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_9, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_10, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_11, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_12, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_13, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_14, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_15, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_16, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_17, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_18, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_19, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_20, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_21, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_22, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_23, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_24, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_25, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_26, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_27, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_28, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_29, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_30, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_31, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_32, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_33, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_34, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_35, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_36, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_37, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_38, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_SAVE, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),

    BT_GATT_CHARACTERISTIC(BT_UUID_CHAR_BLE_SETTINGS_VALUE_RESET, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           ble_char_read_setting_value_callback, ble_char_write_setting_value_callback, NULL),

};

/*************************************************************************
NAME
    get_attr
*/
struct bt_gatt_attr *get_attr(u8_t index) { return &ble_attrs_declaration[index]; }

static struct bt_gatt_service ble_tp_server = BT_GATT_SERVICE(ble_attrs_declaration);

// Start advertising with expected default values
int ble_start_adv(void) {
  MSG("BLE Starting advertising\n");
  struct bt_le_adv_param adv_param = {
      // options:3, connectable undirected, adv one time
      .options      = 3,
      .interval_min = BT_GAP_ADV_FAST_INT_MIN_3,
      .interval_max = BT_GAP_ADV_FAST_INT_MAX_3,
  };
  char     nameBuffer[16];
  uint32_t scratch = getDeviceID() & 0xFFFFFFFF;
  scratch ^= (getDeviceID() >> 32) & 0xFFFFFFFF;
  int nameLen = snprintf(nameBuffer, 16, "Pinecil-%08X", (int)scratch);

  // scan and response data must each stay < 31 bytes
  struct bt_data adv_data[2] = {BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL)), BT_DATA(BT_DATA_NAME_COMPLETE, nameBuffer, nameLen)};

  struct bt_data scan_response_data[1] = {BT_DATA(BT_DATA_UUID128_SOME, ((struct bt_uuid_128 *)BT_UUID_SVC_BULK_DATA)->val, 16)};

  return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), scan_response_data, ARRAY_SIZE(scan_response_data));
}

// Callback that the ble stack will call once it has been kicked off running
// We use this to register the handlers (as we know its now ready for them) + start advertising to the world
void bt_enable_cb(int err) {
  bt_conn_cb_register(&ble_tp_conn_callbacks);
  bt_gatt_service_register(&ble_tp_server);

  ble_start_adv();
}
