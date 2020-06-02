/*
 * Irqs.h
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_IRQ_H_
#define BSP_MINIWARE_IRQ_H_

#include "BSP.h"
#include "stm32f1xx_hal.h"
#include "I2C_Wrapper.hpp"
#include "Setup.h"
#include "main.hpp"

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

#ifdef __cplusplus
}
#endif
#endif /* BSP_MINIWARE_IRQ_H_ */
