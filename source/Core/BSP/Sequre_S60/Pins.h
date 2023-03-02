/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_PINS_H_
#define BSP_MINIWARE_PINS_H_
#include "configuration.h"

#ifdef MODEL_S60

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_7
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_7
#define TIP_TEMP_Pin          GPIO_PIN_0
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_8
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_8
#define VIN_Pin               GPIO_PIN_4
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_9
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_9
#define KEY_A_Pin             GPIO_PIN_0
#define KEY_A_GPIO_Port       GPIOB
#define PWM_Out_Pin           GPIO_PIN_8
#define PWM_Out_GPIO_Port     GPIOB
#define PWM_Out_CHANNEL       TIM_CHANNEL_1
#define SCL_Pin               GPIO_PIN_6
#define SCL_GPIO_Port         GPIOB
#define SDA_Pin               GPIO_PIN_7
#define SDA_GPIO_Port         GPIOB
#endif

#endif /* BSP_MINIWARE_PINS_H_ */
