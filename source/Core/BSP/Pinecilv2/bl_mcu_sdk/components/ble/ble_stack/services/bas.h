/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_BAS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_BAS_H_

/**
 * @brief Battery Service (BAS)
 * @defgroup bt_gatt_bas Battery Service (BAS)
 * @ingroup bluetooth
 * @{
 *
 * [Experimental] Users should note that the APIs can change
 * as a part of ongoing development.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>

void bas_init(void);

/** @brief Read battery level value.
 *
 * Read the characteristic value of the battery level
 *
 *  @return The battery level in percent.
 */
u8_t bt_gatt_bas_get_battery_level(void);

/** @brief Update battery level value.
 *
 * Update the characteristic value of the battery level
 * This will send a GATT notification to all current subscribers.
 *
 *  @param level The battery level in percent.
 *
 *  @return Zero in case of success and error code in case of error.
 */
int bt_gatt_bas_set_battery_level(u8_t level);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_BAS_H_ */
