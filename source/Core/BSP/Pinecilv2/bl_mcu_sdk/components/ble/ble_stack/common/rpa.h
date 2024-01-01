/* rpa.h - Bluetooth Resolvable Private Addresses (RPA) generation and
 * resolution
 */

/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bluetooth.h"
#include "hci_host.h"

bool bt_rpa_irk_matches(const u8_t irk[16], const bt_addr_t *addr);
int bt_rpa_create(const u8_t irk[16], bt_addr_t *rpa);
