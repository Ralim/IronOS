#ifndef INCLUDE_BLUETOOTH_SERVICES_SCPS_H_
#define INCLUDE_BLUETOOTH_SERVICES_SCPS_H_

/**
 * @brief Scan Parameters Service (SCPS)
 * @defgroup bt_gatt_scps Scan Parameters Service (SCPS)
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

bool scps_init(u16_t scan_itvl, u16_t scan_win);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif
