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
void preRToSInit() {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick.
	 */
	HAL_Init();
	Setup_HAL();  // Setup all the HAL objects
	FRToSI2C::init();
	HAL_Delay(50);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(50);

}
