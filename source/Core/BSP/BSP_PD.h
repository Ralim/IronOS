/*
 * BSP_PD.h
 *
 *  Created on: 21 Jul 2020
 *      Author: Ralim
 */

#ifndef USER_BSP_PD_H_
#define USER_BSP_PD_H_
#include "BSP.h"
/*
 * An array of all of the desired voltages & minimum currents in preferred order
 */
extern const uint16_t USB_PD_Desired_Levels[];
extern const uint8_t  USB_PD_Desired_Levels_Len;
#endif /* USER_BSP_PD_H_ */
