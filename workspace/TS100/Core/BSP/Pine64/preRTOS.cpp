/*
 * preRTOS.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "Pins.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>
void preRToSInit() {
	//Normal system bringup -- GPIO etc
	eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL4_PRIO0);
	eclic_global_interrupt_enable();
	hardware_init();
	FRToSI2C::FRToSInit();
	gpio_bit_reset(OLED_RESET_GPIO_Port, OLED_RESET_Pin);
	delay_ms(50);
	gpio_bit_set(OLED_RESET_GPIO_Port, OLED_RESET_Pin);
}
