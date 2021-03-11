/*
 * BSP_PD.c
 *
 *  Created on: 21 Jul 2020
 *      Author: Ralim
 */

#include "BSP_PD.h"
#include "Model_Config.h"
#ifdef POW_PD
/*
 * An array of all of the desired voltages & minimum currents in preferred order
 */
const uint16_t USB_PD_Desired_Levels[] = {
    // mV desired input, mA minimum required current
    // Tip is ~ 7.5 ohms
    20000, 2666, // 20V, 2.6A
    18000, 2400, // 18V, 2.4A
    15000, 2000, // 15V 2A
    12000, 1600, // 12V @ 1.6A
    9000,  1200, // 9V @ 1.2A
    5000,  100,  // 5V @ whatever

};
const uint8_t USB_PD_Desired_Levels_Len = 6;
#endif
