#ifndef __MAIN_H
#define __MAIN_H

#include <MMA8652FC.hpp>
#include "Setup.h"
#include "OLED.hpp"
#include "FRToSI2C.hpp"
#include "OLED.hpp"

extern uint8_t PCBVersion;
enum ButtonState {
	BUTTON_NONE = 0, /* No buttons pressed / < filter time*/
	BUTTON_F_SHORT = 1, /* User has pressed the front button*/
	BUTTON_B_SHORT = 2, /* User has pressed the back  button*/
	BUTTON_F_LONG = 4, /* User is  holding the front button*/
	BUTTON_B_LONG = 8, /* User is  holding the back button*/
	BUTTON_BOTH = 16, /* User has pressed both buttons*/

/*
 * Note:
 * Pressed means press + release, we trigger on a full \__/ pulse
 * holding means it has gone low, and been low for longer than filter time
 */
};

ButtonState getButtonState();
void waitForButtonPressOrTimeout(uint32_t timeout);
void waitForButtonPress();
#ifdef __cplusplus
extern "C" {
#endif


inline void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
inline void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
inline void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
inline void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
inline void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
inline void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc);

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName );

#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */
