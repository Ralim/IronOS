/*
 * IRQ.c
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#include "IRQ.h"
/*
 * Catch the IRQ that says that the conversion is done on the temperature
 * readings coming in Once these have come in we can unblock the PID so that it
 * runs again
 */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (hadc == &hadc1) {
		if (pidTaskNotification) {
			vTaskNotifyGiveFromISR(pidTaskNotification,
					&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c __unused) {

	FRToSI2C::CpltCallback();
}
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c __unused) {

	FRToSI2C::CpltCallback();
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
