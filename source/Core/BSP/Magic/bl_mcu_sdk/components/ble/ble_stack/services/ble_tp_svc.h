/****************************************************************************
FILE NAME
    ble_tp_svc.h

DESCRIPTION
NOTES
*/
/****************************************************************************/

#ifndef _BLE_TP_SVC_H_
#define _BLE_TP_SVC_H_

#include "config.h"

//07af27a5-9c22-11ea-9afe-02fcdc4e7412
#define BT_UUID_SVC_BLE_TP BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x07af27a5, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412))
//07af27a6-9c22-11ea-9afe-02fcdc4e7412
#define BT_UUID_CHAR_BLE_TP_RD BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x07af27a6, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412))
//07af27a7-9c22-11ea-9afe-02fcdc4e7412
#define BT_UUID_CHAR_BLE_TP_WR BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x07af27a7, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412))
//07af27a8-9c22-11ea-9afe-02fcdc4e7412
#define BT_UUID_CHAR_BLE_TP_IND BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x07af27a8, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412))
//07af27a9-9c22-11ea-9afe-02fcdc4e7412
#define BT_UUID_CHAR_BLE_TP_NOT BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0x07af27a9, 0x9c22, 0x11ea, 0x9afe, 0x02fcdc4e7412))

//read value handle offset 2
#define BT_CHAR_BLE_TP_RD_ATTR_VAL_INDEX (2)
//write value handle offset 4
#define BT_CHAR_BLE_TP_WR_ATTR_VAL_INDEX (4)
//indicate value handle offset 6
#define BT_CHAR_BLE_TP_IND_ATTR_VAL_INDEX (6)
//notity value handle offset 9
#define BT_CHAR_BLE_TP_NOT_ATTR_VAL_INDEX (9)

void ble_tp_init();
struct bt_gatt_attr *get_attr(u8_t index);

#endif
