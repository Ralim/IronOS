/*
 * Pins.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef BSP_SEQURE_PINS_H_
#define BSP_SEQURE_PINS_H_
#include "configuration.h"

#ifdef MODEL_S60

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_5
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_5
#define TIP_TEMP_Pin          GPIO_PIN_0
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_0
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_0
#define VIN_Pin               GPIO_PIN_4
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_4
#define KEY_A_Pin             GPIO_PIN_0
#define KEY_A_GPIO_Port       GPIOB
#define PWM_Out_Pin           GPIO_PIN_8
#define PWM_Out_GPIO_Port     GPIOB
#define PWM_Out_CHANNEL       TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin              GPIO_PIN_6
#define SCL2_GPIO_Port        GPIOB
#define SDA2_Pin              GPIO_PIN_7
#define SDA2_GPIO_Port        GPIOB
// Pin gets pulled high on movement
#define MOVEMENT_Pin       GPIO_PIN_3
#define MOVEMENT_GPIO_Port GPIOA

#endif

#ifdef MODEL_S60P

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_5
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_5
#define TIP_TEMP_Pin          GPIO_PIN_0
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_0
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_0
#define VIN_Pin               GPIO_PIN_4
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_4
#define KEY_A_Pin             GPIO_PIN_0
#define KEY_A_GPIO_Port       GPIOB
#define PWM_Out_Pin           GPIO_PIN_8
#define PWM_Out_GPIO_Port     GPIOB
#define PWM_Out_CHANNEL       TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin              GPIO_PIN_6
#define SCL2_GPIO_Port        GPIOB
#define SDA2_Pin              GPIO_PIN_7
#define SDA2_GPIO_Port        GPIOB
#define SCL_Pin               GPIO_PIN_11
#define SCL_GPIO_Port         GPIOB
#define SDA_Pin               GPIO_PIN_10
#define SDA_GPIO_Port         GPIOB
// Pin gets pulled high on movement
#define MOVEMENT_Pin       GPIO_PIN_3
#define MOVEMENT_GPIO_Port GPIOA

#endif

#ifdef MODEL_S99

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_5
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_5
#define TIP_TEMP_Pin          GPIO_PIN_0
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_0
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_0
#define VIN_Pin               GPIO_PIN_4
#define VIN_GPIO_Port         GPIOA
#define VIN_ADC1_CHANNEL      ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL      ADC_CHANNEL_4
#define KEY_A_Pin             GPIO_PIN_0
#define KEY_A_GPIO_Port       GPIOB
#define PWM_Out_Pin           GPIO_PIN_8
#define PWM_Out_GPIO_Port     GPIOB
#define PWM_Out_CHANNEL       TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin              GPIO_PIN_6
#define SCL2_GPIO_Port        GPIOB
#define SDA2_Pin              GPIO_PIN_7
#define SDA2_GPIO_Port        GPIOB
#define SCL_Pin               GPIO_PIN_11
#define SCL_GPIO_Port         GPIOB
#define SDA_Pin               GPIO_PIN_10
#define SDA_GPIO_Port         GPIOB
// Pin gets pulled high on movement
#define MOVEMENT_Pin       GPIO_PIN_3
#define MOVEMENT_GPIO_Port GPIOA
// Passive piezo buzzer, driven with a software square wave from TIM3 (as the stock firmware does)
#define BUZZER_Pin       GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOB
// The stock firmware drives this low at boot and never touches it again; it looks like the return / enable
// side of the buzzer circuit, so it is kept low as well.
#define BUZZER_RETURN_Pin       GPIO_PIN_8
#define BUZZER_RETURN_GPIO_Port GPIOA

#endif // MODEL_S99

#ifdef MODEL_Si012ProMax
// Same core layout as the S60 / S99 (buttons, NTC, VIN divider, OLED bus, motion switch, buzzer), plus a
// second heater channel for TS100 / SI style tips and ten "mood" LEDs. Pin map from Ralim/IronOS#2124.

#define KEY_B_Pin             GPIO_PIN_1
#define KEY_B_GPIO_Port       GPIOB
#define TMP36_INPUT_Pin       GPIO_PIN_5
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL    ADC_CHANNEL_5
#define TMP36_ADC2_CHANNEL    ADC_CHANNEL_5
// Socket 0: T12 tips (contacts deep in the handle, by the USB-C / DC jack). Ralim/IronOS#2124 has these the
// other way round ("T12" on PB8 / PA0); on hardware PB9 / PA6 is the T12 socket and PB8 / PA0 the TS / SI one.
#define TIP_TEMP_Pin          GPIO_PIN_6
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_6
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_6
// Socket 1: TS100 / SI tips (contacts near the opening), own op-amp and MOSFET
#define TIP2_TEMP_Pin          GPIO_PIN_0
#define TIP2_TEMP_GPIO_Port    GPIOA
#define TIP2_TEMP_ADC1_CHANNEL ADC_CHANNEL_0
#define VIN_Pin                GPIO_PIN_4
#define VIN_GPIO_Port          GPIOA
#define VIN_ADC1_CHANNEL       ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL       ADC_CHANNEL_4
// TC1047 on the board: absolute temperature, used for the output stage over-temperature cut-out
#define BOARD_TEMP_Pin          GPIO_PIN_2
#define BOARD_TEMP_GPIO_Port    GPIOA
#define BOARD_TEMP_ADC1_CHANNEL ADC_CHANNEL_2
#define KEY_A_Pin               GPIO_PIN_0
#define KEY_A_GPIO_Port         GPIOB
#define PWM_Out_Pin             GPIO_PIN_9 // T12 socket
#define PWM_Out_GPIO_Port       GPIOB
#define PWM_Out_CHANNEL         TIM_CHANNEL_4 // Timer 4; channel 4
#define PWM_Out_CCR             CCR4
#define PWM2_Out_Pin            GPIO_PIN_8 // TS / SI socket
#define PWM2_Out_GPIO_Port      GPIOB
#define PWM2_Out_CHANNEL        TIM_CHANNEL_3 // Timer 4; channel 3
#define PWM2_Out_CCR            CCR3
#define SCL2_Pin                GPIO_PIN_6
#define SCL2_GPIO_Port          GPIOB
#define SDA2_Pin                GPIO_PIN_7
#define SDA2_GPIO_Port          GPIOB
// Pin gets pulled high on movement
#define MOVEMENT_Pin       GPIO_PIN_3
#define MOVEMENT_GPIO_Port GPIOA
// Passive piezo buzzer, driven with a software square wave from TIM3
#define BUZZER_Pin       GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOB
// Gates a MOSFET on USB D+ (PA11 / PA12 are the MCU's own USB pins, used by the stock bootloader). Held low so
// nothing is presented on D+ to the charger while IronOS runs.
#define USB_DP_GATE_Pin       GPIO_PIN_8
#define USB_DP_GATE_GPIO_Port GPIOA
// Status LEDs, numbered as on the board. PB3 / PA15 are JTAG pins, freed by the SWJ_NOJTAG remap (SWD stays)
#define LED1_Pin         GPIO_PIN_13
#define LED1_GPIO_Port   GPIOC
#define LED2_Pin         GPIO_PIN_14
#define LED2_GPIO_Port   GPIOC
#define LED3_Pin         GPIO_PIN_15
#define LED3_GPIO_Port   GPIOC
#define LED4_Pin         GPIO_PIN_13
#define LED4_GPIO_Port   GPIOB
#define LED5_Pin         GPIO_PIN_14
#define LED5_GPIO_Port   GPIOB
#define LED6_Pin         GPIO_PIN_15
#define LED6_GPIO_Port   GPIOB
#define LED7_Pin         GPIO_PIN_10
#define LED7_GPIO_Port   GPIOB
#define LED8_Pin         GPIO_PIN_11
#define LED8_GPIO_Port   GPIOB
#define LED9_Pin         GPIO_PIN_15
#define LED9_GPIO_Port   GPIOA
#define LED10_Pin        GPIO_PIN_3
#define LED10_GPIO_Port  GPIOB
#define STATUS_LED_COUNT 10

#endif // MODEL_Si012ProMax

#ifdef MODEL_T55

#define KEY_A_Pin       GPIO_PIN_1
#define KEY_A_GPIO_Port GPIOB
// No cold junction compensation as its a PT1000
#define TIP_TEMP_Pin          GPIO_PIN_5
#define TIP_TEMP_GPIO_Port    GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_5
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_5

#define VIN_Pin          GPIO_PIN_4
#define VIN_GPIO_Port    GPIOA
#define VIN_ADC1_CHANNEL ADC_CHANNEL_4
#define VIN_ADC2_CHANNEL ADC_CHANNEL_4
#define KEY_B_Pin        GPIO_PIN_0
#define KEY_B_GPIO_Port  GPIOB

#define PWM_Out_Pin       GPIO_PIN_8
#define PWM_Out_GPIO_Port GPIOB
#define PWM_Out_CHANNEL   TIM_CHANNEL_3 // Timer 4; channel 3
#define SCL2_Pin          GPIO_PIN_6
#define SCL2_GPIO_Port    GPIOB
#define SDA2_Pin          GPIO_PIN_7
#define SDA2_GPIO_Port    GPIOB

#endif // MODEL_T55

#endif /* BSP_SEQURE_PINS_H_ */
