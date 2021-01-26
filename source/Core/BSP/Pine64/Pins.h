/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_PINE64_PINS_H_
#define BSP_PINE64_PINS_H_
#include "gd32vf103_gpio.h"

#define KEY_B_Pin             BIT(1)
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       BIT(4)
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC0_CHANNEL    ADC_CHANNEL_4
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_4
#define TIP_TEMP_Pin          BIT(1)
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC0_CHANNEL ADC_CHANNEL_1
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_1

#define VIN_Pin              BIT(0)
#define VIN_GPIO_Port        GPIOA
#define VIN_ADC0_CHANNEL     ADC_CHANNEL_0
#define VIN_ADC1_CHANNEL     ADC_CHANNEL_0
#define OLED_RESET_Pin       BIT(9)
#define OLED_RESET_GPIO_Port GPIOA
#define KEY_A_Pin            BIT(0)
#define KEY_A_GPIO_Port      GPIOB
#define PWM_Out_Pin          BIT(6)
#define PWM_Out_GPIO_Port    GPIOA
#define SCL_Pin              BIT(6)
#define SCL_GPIO_Port        GPIOB
#define SDA_Pin              BIT(7)
#define SDA_GPIO_Port        GPIOB

#define USB_DM_Pin           BIT(11)
#define USB_DM_LOW_GPIO_Port GPIOA

#define QC_DP_LOW_Pin       BIT(7)
#define QC_DP_LOW_GPIO_Port GPIOA

// LOW = low resistance, HIGH = high resistance
#define QC_DM_LOW_Pin        BIT(8)
#define QC_DM_LOW_GPIO_Port  GPIOA
#define QC_DM_HIGH_Pin       BIT(10)
#define QC_DM_HIGH_GPIO_Port GPIOA

#define FUSB302_IRQ_Pin       BIT(5)
#define FUSB302_IRQ_GPIO_Port GPIOB

// uart
#define UART_TX_Pin       BIT(2)
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin       BIT(3)
#define UART_RX_GPIO_Port GPIOA
#define UART_PERIF        USART1

#endif /* BSP_PINE64_PINS_H_ */
