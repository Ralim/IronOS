/*
 * Defines.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_DEFINES_H_
#define BSP_DEFINES_H_
#include "FreeRTOSConfig.h"
enum Orientation { ORIENTATION_LEFT_HAND = 0, ORIENTATION_RIGHT_HAND = 1, ORIENTATION_FLAT = 3 };

// It is assumed that all hardware implements an 8Hz update period at this time
#define PID_TIM_HZ   (8)
#define TICKS_SECOND configTICK_RATE_HZ
#define TICKS_MIN    (60 * TICKS_SECOND)
#define TICKS_100MS  (TICKS_SECOND / 10)
#define TICKS_10MS   (TICKS_100MS / 10)
#endif /* BSP_DEFINES_H_ */
