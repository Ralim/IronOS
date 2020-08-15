/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_PINS_H_
#define BSP_MINIWARE_PINS_H_
#include "Vendor/Lib/gd32vf103_gpio.h"
//TODO
#define KEY_B_Pin BIT(6)
#define KEY_B_GPIO_Port GPIOA
#define TMP36_INPUT_Pin BIT(7)
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL ADC_CHANNEL_7
#define TMP36_ADC2_CHANNEL ADC_CHANNEL_7
#define TIP_TEMP_Pin BIT(0)
#define TIP_TEMP_GPIO_Port GPIOB
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_8
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_8

#define VIN_Pin BIT(1)
#define VIN_GPIO_Port GPIOB
#define VIN_ADC1_CHANNEL ADC_CHANNEL_9
#define VIN_ADC2_CHANNEL ADC_CHANNEL_9
#define OLED_RESET_Pin BIT(8)
#define OLED_RESET_GPIO_Port GPIOA
#define KEY_A_Pin BIT(9)
#define KEY_A_GPIO_Port GPIOA
#define PWM_Out_Pin BIT(4)
#define PWM_Out_GPIO_Port GPIOB
#define SCL_Pin BIT(6)
#define SCL_GPIO_Port GPIOB
#define SDA_Pin BIT(7)
#define SDA_GPIO_Port GPIOB

#endif /* BSP_MINIWARE_PINS_H_ */
