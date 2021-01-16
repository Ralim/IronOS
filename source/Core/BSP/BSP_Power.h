#include "stdint.h"
/*
 * BSP_Power.h -- Board Support for Power control
 *
 * These functions are hooks used to allow for power control
 *
 */

#ifndef BSP_POWER_H_
#define BSP_POWER_H_
#ifdef __cplusplus
extern "C" {
#endif

// Called periodically in the movement handling thread
// Can be used to check any details for the power system
void power_check();

#ifdef __cplusplus
}
#endif

#endif
