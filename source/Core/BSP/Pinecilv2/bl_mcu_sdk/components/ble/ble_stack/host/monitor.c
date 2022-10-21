/** @file
 *  @brief Custom logging over UART
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if defined(CONFIG_BT_DEBUG_MONITOR)

#include <zephyr.h>
#include <buf.h>
#include "monitor.h"
#include "log.h"

void bt_monitor_send(uint16_t opcode, const void *data, size_t len)
{
    const uint8_t *buf = data;

    BT_WARN("[Hci]:pkt_type:[0x%x],pkt_data:[%s]\r\n", opcode, bt_hex(buf, len));
}

void bt_monitor_new_index(uint8_t type, uint8_t bus, bt_addr_t *addr,
                          const char *name)
{
}
#endif
