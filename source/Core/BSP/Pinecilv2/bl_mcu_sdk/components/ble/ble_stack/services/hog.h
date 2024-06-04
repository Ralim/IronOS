/** @file
 *  @brief HoG Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HOG_H_
#define _HOG_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#define HID_PAGE_KBD  0x07
#define HID_PAGE_CONS 0x0C

#define BT_CHAR_BLE_HID_INFO_ATTR_VAL_INDEX       (2)
#define BT_CHAR_BLE_HID_REPORT_MAP_ATTR_VAL_INDEX (4)
#define BT_CHAR_BLE_HID_REPORT_ATTR_VAL_INDEX     (6)
#define BT_CHAR_BLE_HID_CTRL_POINT_ATTR_VAL_INDEX (10)

enum hid_usage {
    Key_a_or_A2 = 0x0004,
    Key_b_or_B,
    Key_c_or_C
};

void hog_init(void);
int hog_notify(struct bt_conn *conn, uint16_t hid_usage, uint8_t press);

#ifdef __cplusplus
}
#endif
#endif
