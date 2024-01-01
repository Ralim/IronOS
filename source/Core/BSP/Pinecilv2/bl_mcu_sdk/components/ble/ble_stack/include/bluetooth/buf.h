/** @file
 *  @brief Bluetooth data buffer API
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_BUF_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_BUF_H_

/**
 * @brief Data buffers
 * @defgroup bt_buf Data buffers
 * @ingroup bluetooth
 * @{
 */

#include <zephyr/types.h>
#include <net/buf.h>
#include <hci_host.h>

/** Possible types of buffers passed around the Bluetooth stack */
enum bt_buf_type {
    /** HCI command */
    BT_BUF_CMD,
    /** HCI event */
    BT_BUF_EVT,
    /** Outgoing ACL data */
    BT_BUF_ACL_OUT,
    /** Incoming ACL data */
    BT_BUF_ACL_IN,
    /** Outgoing ISO data */
    BT_BUF_ISO_OUT,
    /** Incoming ISO data */
    BT_BUF_ISO_IN,
};

/** Minimum amount of user data size for buffers passed to the stack. */
#define BT_BUF_USER_DATA_MIN 4

#if defined(CONFIG_BT_HCI_RAW)
#define BT_BUF_RESERVE MAX(CONFIG_BT_HCI_RESERVE, CONFIG_BT_HCI_RAW_RESERVE)
#else
#define BT_BUF_RESERVE CONFIG_BT_HCI_RESERVE
#endif

/** Data size neeed for HCI RX buffers */
#define BT_BUF_RX_SIZE (BT_BUF_RESERVE + CONFIG_BT_RX_BUF_LEN)

int bt_buf_get_rx_avail_cnt(void);

/** Allocate a buffer for incoming data
 *
 *  This will set the buffer type so bt_buf_set_type() does not need to
 *  be explicitly called before bt_recv_prio().
 *
 *  @param type    Type of buffer. Only BT_BUF_EVT and BT_BUF_ACL_IN are
 *                 allowed.
 *  @param timeout Timeout in milliseconds, or one of the special values
 *                 K_NO_WAIT and K_FOREVER.
 *  @return A new buffer.
 */
struct net_buf *bt_buf_get_rx(enum bt_buf_type type, s32_t timeout);

/** Allocate a buffer for an HCI Command Complete/Status Event
 *
 *  This will set the buffer type so bt_buf_set_type() does not need to
 *  be explicitly called before bt_recv_prio().
 *
 *  @param timeout Timeout in milliseconds, or one of the special values
 *                 K_NO_WAIT and K_FOREVER.
 *  @return A new buffer.
 */
struct net_buf *bt_buf_get_cmd_complete(s32_t timeout);

/** Allocate a buffer for an HCI Event
 *
 *  This will set the buffer type so bt_buf_set_type() does not need to
 *  be explicitly called before bt_recv_prio() or bt_recv().
 *
 *  @param evt          HCI event code
 *  @param discardable  Whether the driver considers the event discardable.
 *  @param timeout      Timeout in milliseconds, or one of the special values
 *                      K_NO_WAIT and K_FOREVER.
 *  @return A new buffer.
 */
struct net_buf *bt_buf_get_evt(u8_t evt, bool discardable, s32_t timeout);

/** Set the buffer type
 *
 *  @param buf   Bluetooth buffer
 *  @param type  The BT_* type to set the buffer to
 */
static inline void bt_buf_set_type(struct net_buf *buf, enum bt_buf_type type)
{
    *(u8_t *)net_buf_user_data(buf) = type;
}

#if defined(BFLB_BLE)
static inline void bt_buf_set_rx_adv(struct net_buf *buf, bool is_adv)
{
    u8_t *usr_data = (u8_t *)net_buf_user_data(buf);
    usr_data++;
    *usr_data = is_adv;
}

static inline u8_t bt_buf_check_rx_adv(struct net_buf *buf)
{
    u8_t *usr_data = (u8_t *)net_buf_user_data(buf);
    usr_data++;
    return (*usr_data);
}
#endif

/** Get the buffer type
 *
 *  @param buf   Bluetooth buffer
 *
 *  @return The BT_* type to of the buffer
 */
static inline enum bt_buf_type bt_buf_get_type(struct net_buf *buf)
{
    /* De-referencing the pointer from net_buf_user_data(buf) as a
	 * pointer to an enum causes issues on qemu_x86 because the true
	 * size is 8-bit, but the enum is 32-bit on qemu_x86. So we put in
	 * a temporary cast to 8-bit to ensure only 8 bits are read from
	 * the pointer.
	 */
    return (enum bt_buf_type)(*(u8_t *)net_buf_user_data(buf));
}

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_BUF_H_ */
