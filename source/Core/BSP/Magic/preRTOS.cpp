/*
 * preRTOS.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "Pins.h"
#include "Setup.h"
#include "gd32vf103_libopt.h"
#include <I2C_Wrapper.hpp>
void preRToSInit() {
  // Normal system bringup -- GPIO etc

  hardware_init();
  gpio_bit_reset(OLED_RESET_GPIO_Port, OLED_RESET_Pin);
  delay_ms(5);
  gpio_bit_set(OLED_RESET_GPIO_Port, OLED_RESET_Pin);
  FRToSI2C::FRToSInit();
}
