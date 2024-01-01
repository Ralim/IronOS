/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_PINE64_PINS_H_
#define BSP_PINE64_PINS_H_
#include "bl702_adc.h"
#include "bl702_pwm.h"
#include "hal_gpio.h"

#define KEY_B_Pin            GPIO_PIN_28
#define TMP36_INPUT_Pin      GPIO_PIN_20
#define TMP36_ADC_CHANNEL    ADC_CHAN10
#define TIP_TEMP_Pin         GPIO_PIN_19
#define TIP_TEMP_ADC_CHANNEL ADC_CHAN9

#define TIP_RESISTANCE_SENSE GPIO_PIN_24
#define VIN_Pin              GPIO_PIN_18
#define VIN_ADC_CHANNEL      ADC_CHAN8
#define OLED_RESET_Pin       GPIO_PIN_3
#define KEY_A_Pin            GPIO_PIN_25
#define PWM_Out_Pin          GPIO_PIN_21
#define PWM_Channel          PWM_CH1
#define SCL_Pin              GPIO_PIN_11
#define SDA_Pin              GPIO_PIN_10

#define USB_DM_Pin    GPIO_PIN_8
#define QC_DP_LOW_Pin GPIO_PIN_5

// LOW = low resistance, HIGH = high resistance
#define QC_DM_LOW_Pin  GPIO_PIN_4
#define QC_DM_HIGH_Pin GPIO_PIN_6

#define FUSB302_IRQ_Pin     GPIO_PIN_16
#define FUSB302_IRQ_GLB_Pin GLB_GPIO_PIN_16

// uart
#define UART_TX_Pin GPIO_PIN_22
#define UART_RX_Pin GPIO_PIN_23

#endif /* BSP_PINE64_PINS_H_ */
