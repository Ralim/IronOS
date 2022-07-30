/*
 * preRTOS.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "Pins.h"
#include "Setup.h"
#include "bflb_platform.h"
#include "hal_gpio.h"
#include <I2C_Wrapper.hpp>

void preRToSInit() {
  // Normal system bringup -- GPIO etc
  bflb_platform_init(0);

  hardware_init();
  gpio_write(OLED_RESET_Pin, 0);
  delay_ms(10);
  gpio_write(OLED_RESET_Pin, 1);
  FRToSI2C::FRToSInit();
}
