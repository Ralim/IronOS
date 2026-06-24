/*
 * Pins.h
 *
 *  Alientek T90 (Nations N32L40x) pin map.
 *  Verified against the stock-firmware reverse engineering (t90b-gc9d01-pinmap-re.md).
 *  Ports are N32 GPIO_Module* (GPIOA/GPIOB/GPIOD), pin masks GPIO_PIN_n from n32l40x_gpio.h,
 *  ADC channels are the single-ADC ADC_CH_n_PAx macros from n32l40x_adc.h.
 *  The T90 has ONE ADC, so each analog signal exposes a single *_ADC_CHANNEL macro.
 */

#ifndef BSP_ALIENTEK_T90_PINS_H_
#define BSP_ALIENTEK_T90_PINS_H_
#include "configuration.h"
#include "n32l40x.h"

#ifdef MODEL_T90

// --- Buttons (active-low, internal pull-up, polled) ---
#define KEY_A_Pin       GPIO_PIN_15
#define KEY_A_GPIO_Port GPIOA
#define KEY_B_Pin       GPIO_PIN_4
#define KEY_B_GPIO_Port GPIOB

// --- Tip thermocouple (external op-amp -> PA4, ADC ch5, injected group / PID input) ---
#define TIP_TEMP_Pin         GPIO_PIN_4
#define TIP_TEMP_GPIO_Port   GPIOA
#define TIP_TEMP_ADC_CHANNEL ADC_CH_5_PA4

// --- Cold-junction NTC (PA3, ADC ch4). Reuses the TMP36/cold-junction vocabulary. ---
#define TMP36_INPUT_Pin       GPIO_PIN_3
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC_CHANNEL     ADC_CH_4_PA3

// --- Input supply voltage (PA2, ADC ch3, via ~10:1 divider) ---
#define VIN_Pin         GPIO_PIN_2
#define VIN_GPIO_Port   GPIOA
#define VIN_ADC_CHANNEL ADC_CH_3_PA2

// --- Tip / output current sense (PA5, ADC ch6, via shunt). No Sequre equivalent. ---
#define CURRENT_Pin         GPIO_PIN_5
#define CURRENT_GPIO_Port   GPIOA
#define CURRENT_ADC_CHANNEL ADC_CH_6_PA5

// --- Heater drive: PA0 = TIM2_CH1, direct AF to the MOSFET gate ---
#define PWM_Out_Pin       GPIO_PIN_0
#define PWM_Out_GPIO_Port GPIOA
#define PWM_Out_CHANNEL   TIM_CH_1 // Timer 2; channel 1

// --- Buzzer: PA7 = TIM1_CH1N (complementary, BDTR/MOE) ---
#define BUZZER_Pin       GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOA
#define BUZZER_CHANNEL   TIM_CH_1 // Timer 1; channel 1 (CH1N output)

// --- CH224Q USB-PD over software (bit-bang) I2C, device addr 0x22 ---
#define SCL2_Pin       GPIO_PIN_6
#define SCL2_GPIO_Port GPIOB
#define SDA2_Pin       GPIO_PIN_7
#define SDA2_GPIO_Port GPIOB

// --- GC9-family color LCD over hardware SPI1 ---
#define LCD_CS_Pin         GPIO_PIN_10 // chip select, idle HIGH, framed per byte
#define LCD_CS_GPIO_Port   GPIOA
#define LCD_DC_Pin         GPIO_PIN_0 // command/data select (low = command)
#define LCD_DC_GPIO_Port   GPIOD
#define LCD_SCK_Pin        GPIO_PIN_3 // SPI1_SCK (AF)
#define LCD_SCK_GPIO_Port  GPIOB
#define LCD_MOSI_Pin       GPIO_PIN_5 // SPI1_MOSI (AF), half-duplex TX-only
#define LCD_MOSI_GPIO_Port GPIOB
#define LCD_BL_Pin         GPIO_PIN_6 // backlight enable, ACTIVE-LOW, GPIO (not PWM)
#define LCD_BL_GPIO_Port   GPIOA
// LCD_RST: no GPIO reset on the T90; reset relies on the GC9 0xFE/0xEF software unlock.

#endif // MODEL_T90

#endif /* BSP_ALIENTEK_T90_PINS_H_ */
