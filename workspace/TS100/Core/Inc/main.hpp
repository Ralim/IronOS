#ifndef __MAIN_H
#define __MAIN_H

#include <MMA8652FC.hpp>
#include "OLED.hpp"
#include "Setup.h"
extern uint8_t PCBVersion;
extern uint32_t currentTempTargetDegC;
extern bool settingsWereReset;

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
