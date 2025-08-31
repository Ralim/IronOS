/*
 * Irqs.h
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_IRQ_H_
#define BSP_MINIWARE_IRQ_H_

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Setup.h"
#include "main.hpp"
#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_GPIO_EXTI_Callback(uint16_t);

#ifdef __cplusplus
}
#endif
#endif /* BSP_MINIWARE_IRQ_H_ */
