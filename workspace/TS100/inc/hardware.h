/*
 * Hardware.h
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_
#include "stm32f1xx_hal.h"
#include "Setup.h"
#ifdef __cplusplus
extern "C" {
#endif

enum Orientation {
    ORIENTATION_LEFT_HAND = 0,
    ORIENTATION_RIGHT_HAND = 1,
    ORIENTATION_FLAT = 3
};

#define KEY_B_Pin GPIO_PIN_6
#define KEY_B_GPIO_Port GPIOA
#define TMP36_INPUT_Pin GPIO_PIN_7
#define TMP36_INPUT_GPIO_Port GPIOA
#define TIP_TEMP_Pin GPIO_PIN_0
#define TIP_TEMP_GPIO_Port GPIOB
#define VIN_Pin GPIO_PIN_1
#define VIN_GPIO_Port GPIOB
#define OLED_RESET_Pin GPIO_PIN_8
#define OLED_RESET_GPIO_Port GPIOA
#define KEY_A_Pin GPIO_PIN_9
#define KEY_A_GPIO_Port GPIOA
#define INT_Orientation_Pin GPIO_PIN_3
#define INT_Orientation_GPIO_Port GPIOB
#define PWM_Out_Pin GPIO_PIN_4
#define PWM_Out_GPIO_Port GPIOB
#define INT_Movement_Pin GPIO_PIN_5
#define INT_Movement_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

uint16_t getHandleTemperature();
uint16_t getTipRawTemp(uint8_t instant);
uint16_t getInputVoltageX10(uint16_t divisor);
uint16_t getTipInstantTemperature();
uint8_t getTipPWM();
void setTipPWM(uint8_t pulse);
uint16_t ctoTipMeasurement(uint16_t temp);
uint16_t tipMeasurementToC(uint16_t raw);
uint16_t ftoTipMeasurement(uint16_t temp);
uint16_t tipMeasurementToF(uint16_t raw);

void setCalibrationOffset(int16_t offSet);
#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_H_ */
