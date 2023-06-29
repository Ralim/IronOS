/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_PINS_H_
#define BSP_MINIWARE_PINS_H_
#include "configuration.h"

#ifdef MODEL_TS100

#define KEY_B_Pin                 GPIO_PIN_6
#define KEY_B_GPIO_Port           GPIOA
#define TMP36_INPUT_Pin           GPIO_PIN_7
#define TMP36_INPUT_GPIO_Port     GPIOA
#define TMP36_ADC1_CHANNEL        ADC_CHANNEL_7
#define TMP36_ADC2_CHANNEL        ADC_CHANNEL_7
#define TIP_TEMP_Pin              GPIO_PIN_0
#define TIP_TEMP_GPIO_Port        GPIOB
#define TIP_TEMP_ADC1_CHANNEL     ADC_CHANNEL_8
#define TIP_TEMP_ADC2_CHANNEL     ADC_CHANNEL_8
#define VIN_Pin                   GPIO_PIN_1
#define VIN_GPIO_Port             GPIOB
#define VIN_ADC1_CHANNEL          ADC_CHANNEL_9
#define VIN_ADC2_CHANNEL          ADC_CHANNEL_9
#define OLED_RESET_Pin            GPIO_PIN_8
#define OLED_RESET_GPIO_Port      GPIOA
#define KEY_A_Pin                 GPIO_PIN_9
#define KEY_A_GPIO_Port           GPIOA
#define INT_Orientation_Pin       GPIO_PIN_3
#define INT_Orientation_GPIO_Port GPIOB
#define PWM_Out_Pin               GPIO_PIN_4
#define PWM_Out_GPIO_Port         GPIOB
#define PWM_Out_CHANNEL           TIM_CHANNEL_1
#define TIP_CONTROL_TIMER         TIM3
#define ADC_CONTROL_TIMER         TIM2
#define ADC_CONTROL_TIMER_IRQ     TIM2_IRQn
#define ADC_TRIGGER               ADC_EXTERNALTRIGINJECCONV_T2_TRGO
#define INT_Movement_Pin          GPIO_PIN_5
#define INT_Movement_GPIO_Port    GPIOB
#define SCL_Pin                   GPIO_PIN_6
#define SCL_GPIO_Port             GPIOB
#define SDA_Pin                   GPIO_PIN_7
#define SDA_GPIO_Port             GPIOB
#endif

#ifdef MODEL_TS101

#define KEY_B_Pin             GPIO_PIN_10
#define KEY_B_GPIO_Port       GPIOA
#define TMP36_INPUT_Pin       GPIO_PIN_4
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_4
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_4
#define TIP_TEMP_Pin          GPIO_PIN_3
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_3
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_3
#define VIN_Pin               GPIO_PIN_2
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_2
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_2
#define PD_VIN_Pin            GPIO_PIN_6
#define PD_VIN_GPIO_Port      GPIOA
#define PD_VIN_ADC1_CHANNEL   ADC_CHANNEL_6
#define PD_VIN_ADC2_CHANNEL   ADC_CHANNEL_6
#define OLED_RESET_Pin        GPIO_PIN_7
#define OLED_RESET_GPIO_Port  GPIOA
#define KEY_A_Pin             GPIO_PIN_8
#define KEY_A_GPIO_Port       GPIOA
#define PWM_Out_Pin           GPIO_PIN_0
#define PWM_Out_GPIO_Port     GPIOA
#define PWM_Out_CHANNEL       TIM_CHANNEL_1
#define TIP_CONTROL_TIMER     TIM2
#define ADC_CONTROL_TIMER     TIM4
#define ADC_CONTROL_TIMER_IRQ TIM4_IRQn
#define ADC_TRIGGER           ADC_EXTERNALTRIGINJECCONV_T4_TRGO
#define SCL_Pin               GPIO_PIN_0
#define SCL_GPIO_Port         GPIOB
#define SDA_Pin               GPIO_PIN_1
#define SDA_GPIO_Port         GPIOB
// PD controller
#define SCL2_Pin         GPIO_PIN_6
#define SCL2_GPIO_Port   GPIOB
#define SDA2_Pin         GPIO_PIN_5
#define SDA2_GPIO_Port   GPIOB
#define INT_PD_Pin       GPIO_PIN_7
#define INT_PD_GPIO_Port GPIOB
// Selecting the DC source to route to theg
#define DC_SELECT_Pin       GPIO_PIN_4
#define DC_SELECT_GPIO_Port GPIOB
#define PD_SELECT_Pin       GPIO_PIN_15
#define PD_SELECT_GPIO_Port GPIOA

#define TIP_RESISTANCE_SENSE_Pin       GPIO_PIN_1
#define TIP_RESISTANCE_SENSE_GPIO_Port GPIOA

#endif
#if defined(MODEL_TS80) + defined(MODEL_TS80P) > 0
// TS80 & TS80P pin map
#define KEY_B_Pin                 GPIO_PIN_0
#define KEY_B_GPIO_Port           GPIOB
#define TMP36_INPUT_Pin           GPIO_PIN_4
#define TMP36_INPUT_GPIO_Port     GPIOA
#define TMP36_ADC1_CHANNEL        ADC_CHANNEL_4
#define TMP36_ADC2_CHANNEL        ADC_CHANNEL_4
#define TIP_TEMP_Pin              GPIO_PIN_3
#define TIP_TEMP_GPIO_Port        GPIOA
#define TIP_TEMP_ADC1_CHANNEL     ADC_CHANNEL_3
#define TIP_TEMP_ADC2_CHANNEL     ADC_CHANNEL_3
#define VIN_Pin                   GPIO_PIN_2
#define VIN_GPIO_Port             GPIOA
#define VIN_ADC1_CHANNEL          ADC_CHANNEL_2
#define VIN_ADC2_CHANNEL          ADC_CHANNEL_2
#define OLED_RESET_Pin            GPIO_PIN_15
#define OLED_RESET_GPIO_Port      GPIOA
#define KEY_A_Pin                 GPIO_PIN_1
#define KEY_A_GPIO_Port           GPIOB
#define INT_Orientation_Pin       GPIO_PIN_4
#define INT_Orientation_GPIO_Port GPIOB
#define PWM_Out_Pin               GPIO_PIN_6
#define PWM_Out_GPIO_Port         GPIOA
#define PWM_Out_CHANNEL           TIM_CHANNEL_1
#define TIP_CONTROL_TIMER         TIM3
#define ADC_CONTROL_TIMER         TIM2
#define ADC_CONTROL_TIMER_IRQ     TIM2_IRQn
#define ADC_TRIGGER               ADC_EXTERNALTRIGINJECCONV_T2_TRGO
#define INT_Movement_Pin          GPIO_PIN_5
#define INT_Movement_GPIO_Port    GPIOB
#define SCL_Pin                   GPIO_PIN_6
#define SCL_GPIO_Port             GPIOB
#define SDA_Pin                   GPIO_PIN_7
#define SDA_GPIO_Port             GPIOB
#define SCL2_Pin                  GPIO_PIN_5
#define SCL2_GPIO_Port            GPIOA
#define SDA2_Pin                  GPIO_PIN_1
#define SDA2_GPIO_Port            GPIOA

#endif

#ifdef MODEL_TS80P
// TS80P pin map
#define INT_PD_Pin       GPIO_PIN_9
#define INT_PD_GPIO_Port GPIOA

#endif

#endif /* BSP_MINIWARE_PINS_H_ */
