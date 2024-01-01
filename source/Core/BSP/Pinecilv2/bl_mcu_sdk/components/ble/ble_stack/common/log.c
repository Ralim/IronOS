/* log.c - logging helpers */

/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Helper for printk parameters to convert from binary to hex.
 * We declare multiple buffers so the helper can be used multiple times
 * in a single printk call.
 */

#include <bluetooth.h>
#include <hci_host.h>
#include <misc/util.h>
#include <stddef.h>
#include <zephyr.h>
#include <zephyr/types.h>

const char *bt_hex_real(const void *buf, size_t len) {
  static const char hex[] = "0123456789abcdef";
#if defined(CONFIG_BT_DEBUG_MONITOR)
  static char str[255];
#else
  static char str[128];
#endif
  const u8_t *b = buf;
  int         i;

  len = MIN(len, (sizeof(str) - 1) / 2);

  for (i = 0; i < len; i++) {
    str[i * 2]     = hex[b[i] >> 4];
    str[i * 2 + 1] = hex[b[i] & 0xf];
  }

  str[i * 2] = '\0';

  return str;
}

const char *bt_addr_str_real(const bt_addr_t *addr) {
  static char str[BT_ADDR_STR_LEN];

  bt_addr_to_str(addr, str, sizeof(str));

  return str;
}

const char *bt_addr_le_str_real(const bt_addr_le_t *addr) {
  static char str[BT_ADDR_LE_STR_LEN];

  bt_addr_le_to_str(addr, str, sizeof(str));

  return str;
}
