/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_PINS_H_
#define BSP_MINIWARE_PINS_H_
#include "Model_Config.h"

// MHP30 pin map
#define KEY_B_Pin             GPIO_PIN_0
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_0 // UNKNOWN
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_0
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_0
#define TIP_TEMP_Pin          GPIO_PIN_2
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_2
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_2

#define VIN_Pin                   GPIO_PIN_1
#define VIN_GPIO_Port             GPIOA
#define VIN_ADC1_CHANNEL          ADC_CHANNEL_1
#define VIN_ADC2_CHANNEL          ADC_CHANNEL_1
#define OLED_RESET_Pin            GPIO_PIN_4
#define OLED_RESET_GPIO_Port      GPIOB
#define KEY_A_Pin                 GPIO_PIN_10
#define KEY_A_GPIO_Port           GPIOA
#define PWM_Out_Pin               GPIO_PIN_6
#define PWM_Out_GPIO_Port         GPIOA
#define PWM_Out_CHANNEL           TIM_CHANNEL_1
#define SCL_Pin                   GPIO_PIN_6
#define SCL_GPIO_Port             GPIOB
#define SDA_Pin                   GPIO_PIN_7
#define SDA_GPIO_Port             GPIOB
#define SCL2_Pin                  GPIO_PIN_3
#define SCL2_GPIO_Port            GPIOB
#define SDA2_Pin                  GPIO_PIN_15
#define SDA2_GPIO_Port            GPIOA
#define INT_PD_Pin                GPIO_PIN_5
#define INT_PD_GPIO_Port          GPIOB


#endif /* BSP_MINIWARE_PINS_H_ */
