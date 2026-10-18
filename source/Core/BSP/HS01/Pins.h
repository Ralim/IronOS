/*
 * Pins.h
 *
 * FNIRSI HS-01 (WCH CH571F), pin map recovered from the stock firmware
 */

#ifndef BSP_HS01_PINS_H_
#define BSP_HS01_PINS_H_
#include "CH57x_common.h"
#undef min // CH573SFR.h macros, clash with IronOS functions
#undef max
#include "configuration.h"
#include <stdbool.h>

// Port B
#define KEY_A_Pin      GPIO_Pin_15 // Left button, to GND
#define KEY_B_Pin      GPIO_Pin_14 // Right button, to GND
#define SCL_Pin        GPIO_Pin_12 // OLED, push-pull bit-bang like the stock firmware
#define SDA_Pin        GPIO_Pin_13 // OLED, open drain emulated through the direction register
#define OLED_RESET_Pin GPIO_Pin_4
#define TIP_PWM_Pin    GPIO_Pin_23 // TMR0 PWM output (remapped from PA9), high = heater on

// Port A
#define VIN_Pin         GPIO_Pin_5  // ADC channel 1
#define TIP_TEMP_Pin    GPIO_Pin_14 // ADC channel 4
#define MOVEMENT_Pin    GPIO_Pin_15 // Vibration switch, to GND
#define CH224K_CFG2_Pin GPIO_Pin_12
#define CH224K_CFG3_Pin GPIO_Pin_13

#define VIN_ADC_CHANNEL      1
#define TIP_TEMP_ADC_CHANNEL 4

// MOVThread shim: reports each latched switch edge once
#ifdef __cplusplus
extern "C" {
#endif
bool takeMovementLatch(void);
#ifdef __cplusplus
}
#endif
#define MOVEMENT_GPIO_Port          0
#define GPIO_PIN_SET                1
#define HAL_GPIO_ReadPin(port, pin) (takeMovementLatch() ? GPIO_PIN_SET : 0)

#endif /* BSP_HS01_PINS_H_ */
