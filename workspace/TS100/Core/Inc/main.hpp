#ifndef __MAIN_H
#define __MAIN_H

#include <MMA8652FC.hpp>
#include "OLED.hpp"
#include "Setup.h"
extern uint8_t PCBVersion;
extern uint32_t currentTempTargetDegC;
extern bool settingsWereReset;
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
void GUIDelay();
#ifdef __cplusplus
extern "C" {
#endif

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed portCHAR *pcTaskName);

#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */
