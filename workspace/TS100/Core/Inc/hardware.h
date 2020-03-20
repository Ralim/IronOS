/*
 * Hardware.h
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_
#include "Setup.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "unit.h"

#ifdef __cplusplus
extern "C" {
#endif

enum Orientation {
	ORIENTATION_LEFT_HAND = 0, ORIENTATION_RIGHT_HAND = 1, ORIENTATION_FLAT = 3
};
#define PID_TIM_HZ (8)
#if defined(MODEL_TS100) + defined(MODEL_TS80) > 1
#error "Multiple models defined!"
#elif defined(MODEL_TS100) + defined(MODEL_TS80) == 0
#error "No model defined!"
#endif

#ifdef MODEL_TS100

#define KEY_B_Pin GPIO_PIN_6
#define KEY_B_GPIO_Port GPIOA
#define TMP36_INPUT_Pin GPIO_PIN_7
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL ADC_CHANNEL_7
#define TIP_TEMP_Pin GPIO_PIN_0
#define TIP_TEMP_GPIO_Port GPIOB
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_8
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_8
#define VIN_Pin GPIO_PIN_1
#define VIN_GPIO_Port GPIOB
#define VIN_ADC1_CHANNEL ADC_CHANNEL_9
#define VIN_ADC2_CHANNEL ADC_CHANNEL_9
#define OLED_RESET_Pin GPIO_PIN_8
#define OLED_RESET_GPIO_Port GPIOA
#define KEY_A_Pin GPIO_PIN_9
#define KEY_A_GPIO_Port GPIOA
#define INT_Orientation_Pin GPIO_PIN_3
#define INT_Orientation_GPIO_Port GPIOB
#define PWM_Out_Pin GPIO_PIN_4
#define PWM_Out_GPIO_Port GPIOB
#define PWM_Out_CHANNEL TIM_CHANNEL_1
#define PWM_Out_CCR
#define INT_Movement_Pin GPIO_PIN_5
#define INT_Movement_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

#else
// TS80 pin map
#define KEY_B_Pin GPIO_PIN_0
#define KEY_B_GPIO_Port GPIOB
#define TMP36_INPUT_Pin GPIO_PIN_4
#define TMP36_INPUT_GPIO_Port GPIOA
#define TMP36_ADC1_CHANNEL ADC_CHANNEL_4
#define TIP_TEMP_Pin GPIO_PIN_3
#define TIP_TEMP_GPIO_Port GPIOA
#define TIP_TEMP_ADC1_CHANNEL ADC_CHANNEL_3
#define TIP_TEMP_ADC2_CHANNEL ADC_CHANNEL_3

#define VIN_Pin GPIO_PIN_2
#define VIN_GPIO_Port GPIOA
#define VIN_ADC1_CHANNEL ADC_CHANNEL_2
#define VIN_ADC2_CHANNEL ADC_CHANNEL_2
#define OLED_RESET_Pin GPIO_PIN_15
#define OLED_RESET_GPIO_Port GPIOA
#define KEY_A_Pin GPIO_PIN_1
#define KEY_A_GPIO_Port GPIOB
#define INT_Orientation_Pin GPIO_PIN_4
#define INT_Orientation_GPIO_Port GPIOB
#define PWM_Out_Pin GPIO_PIN_6
#define PWM_Out_GPIO_Port GPIOA
#define PWM_Out_CHANNEL TIM_CHANNEL_1
#define INT_Movement_Pin GPIO_PIN_5
#define INT_Movement_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

#endif

/*
 * Keep in a uint8_t range for the ID's
 */
#ifdef MODEL_TS100
enum TipType {
	TS_B2 = 0,
	TS_D24 = 1,
	TS_BC2 = 2,
	TS_C1 = 3,
	Tip_MiniWare = 4,
	HAKKO_BC2 = 4,
	Tip_Hakko = 5,
	Tip_Custom = 5,
};
#endif
#ifdef MODEL_TS80
enum TipType {
	TS_B02 = 0, TS_D25 = 1, Tip_MiniWare = 2, Tip_Custom = 3,
};
#endif
extern uint16_t tipGainCalValue ;

uint16_t lookupTipDefaultCalValue(enum TipType tipID);
 uint16_t getHandleTemperature();
uint16_t getTipRawTemp(uint8_t refresh);
uint16_t getInputVoltageX10(uint16_t divisor,uint8_t sample);

void setTipPWM(uint8_t pulse);
uint16_t ctoTipMeasurement(uint16_t temp);
uint16_t tipMeasurementToC(uint16_t raw);
uint16_t ftoTipMeasurement(uint16_t temp);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
uint16_t tipMeasurementToF(uint16_t raw);
#endif
void seekQC(int16_t Vx10, uint16_t divisor);
void setCalibrationOffset(int16_t offSet);
void setTipType(enum TipType tipType, uint8_t manualCalGain);
uint32_t calculateTipR();
int16_t calculateMaxVoltage(uint8_t useHP);
void startQC(uint16_t divisor); // Tries to negotiate QC for highest voltage, must be run after
// RToS
// This will try for 12V, failing that 9V, failing that 5V
// If input is over 12V returns -1
// If the input is [5-12] Will return the value.
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) ;
void vApplicationIdleHook(void);
#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_H_ */
