/*
 * Defines.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_DEFINES_H_
#define BSP_DEFINES_H_


enum Orientation {
	ORIENTATION_LEFT_HAND = 0, ORIENTATION_RIGHT_HAND = 1, ORIENTATION_FLAT = 3
};

//It is assumed that all hardware implements an 8Hz update period at this time
#define PID_TIM_HZ (8)

#endif /* BSP_DEFINES_H_ */
