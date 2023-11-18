/*
 * preRTOS.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "I2CBB1.hpp"
#include "I2CBB2.hpp"
#include "Pins.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>

void preRToSInit() {
  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();
  Setup_HAL(); // Setup all the HAL objects
  BSPInit();
  I2CBB2::init();
  I2CBB1::init();
}
