/****************************************************************************
FILE NAME
    ble_peripheral_tp_server.h

DESCRIPTION
NOTES
*/
/****************************************************************************/

#ifndef _BLE_TP_SVC_H_
#define _BLE_TP_SVC_H_

#include "ble_config.h"

// read value handle offset 2
#define BT_CHAR_BLE_TP_RD_ATTR_VAL_INDEX (2)
// write value handle offset 4
#define BT_CHAR_BLE_TP_WR_ATTR_VAL_INDEX (4)
// indicate value handle offset 6
#define BT_CHAR_BLE_TP_IND_ATTR_VAL_INDEX (6)
// notity value handle offset 9
#define BT_CHAR_BLE_TP_NOT_ATTR_VAL_INDEX (9)
#ifdef __cplusplus
extern "C" {
#endif

void                 ble_tp_init();
void                 bt_enable_cb(int err);
struct bt_gatt_attr *get_attr(u8_t index);

#ifdef __cplusplus
};
#endif

#endif
