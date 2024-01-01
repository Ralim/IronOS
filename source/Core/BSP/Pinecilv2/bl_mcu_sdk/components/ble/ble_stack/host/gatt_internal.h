/** @file
 *  @brief Internal API for Generic Attribute Profile handling.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define BT_GATT_CENTRAL_ADDR_RES_NOT_SUPP 0
#define BT_GATT_CENTRAL_ADDR_RES_SUPP     1

#include <gatt.h>

#define BT_GATT_PERM_READ_MASK (BT_GATT_PERM_READ |         \
                                BT_GATT_PERM_READ_ENCRYPT | \
                                BT_GATT_PERM_READ_AUTHEN)
#define BT_GATT_PERM_WRITE_MASK (BT_GATT_PERM_WRITE |         \
                                 BT_GATT_PERM_WRITE_ENCRYPT | \
                                 BT_GATT_PERM_WRITE_AUTHEN)
#define BT_GATT_PERM_ENCRYPT_MASK (BT_GATT_PERM_READ_ENCRYPT | \
                                   BT_GATT_PERM_WRITE_ENCRYPT)
#define BT_GATT_PERM_AUTHEN_MASK (BT_GATT_PERM_READ_AUTHEN | \
                                  BT_GATT_PERM_WRITE_AUTHEN)

void bt_gatt_init(void);
#if defined(BFLB_BLE)
void bt_gatt_deinit(void);
#endif
void bt_gatt_connected(struct bt_conn *conn);
void bt_gatt_encrypt_change(struct bt_conn *conn);
void bt_gatt_disconnected(struct bt_conn *conn);

bool bt_gatt_change_aware(struct bt_conn *conn, bool req);

int bt_gatt_store_ccc(u8_t id, const bt_addr_le_t *addr);

int bt_gatt_clear(u8_t id, const bt_addr_le_t *addr);

#if defined(BFLB_BLE_MTU_CHANGE_CB)
void bt_gatt_mtu_changed(struct bt_conn *conn, u16_t mtu);
#endif

#if defined(CONFIG_BT_GATT_CLIENT)
void bt_gatt_notification(struct bt_conn *conn, u16_t handle,
                          const void *data, u16_t length);
#else
static inline void bt_gatt_notification(struct bt_conn *conn, u16_t handle,
                                        const void *data, u16_t length)
{
}
#endif /* CONFIG_BT_GATT_CLIENT */

struct bt_gatt_attr;

/* Check attribute permission */
u8_t bt_gatt_check_perm(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        u8_t mask);
