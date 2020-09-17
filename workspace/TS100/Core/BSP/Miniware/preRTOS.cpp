/*
 * preRTOS.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include <I2C_Wrapper.hpp>
#include "BSP.h"
#include "Setup.h"
#include "Pins.h"
#include "I2CBB.hpp"
#include "fusbpd.h"
#include "Model_Config.h"
void preRToSInit() {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick.
	 */
	HAL_Init();
	Setup_HAL();  // Setup all the HAL objects
#ifdef I2C_SOFT
	I2CBB::init();
#endif
	/* Init the IPC objects */
	FRToSI2C::FRToSInit();
}
