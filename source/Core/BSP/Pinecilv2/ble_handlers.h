#ifndef BLE_HANDLERS_H_
#define BLE_HANDLERS_H_

#include "conn_internal.h"
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

int ble_char_read_status_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset);
int ble_char_read_setting_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset);
int ble_char_read_bulk_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset);

int ble_char_write_setting_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags);

uint32_t getPowerSrc();

#ifdef __cplusplus
};
#endif

#endif