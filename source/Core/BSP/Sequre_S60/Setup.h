/*
 * Setup.h
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef SETUP_H_
#define SETUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc1;

extern IWDG_HandleTypeDef hiwdg;

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim2;
void                     Setup_HAL();
uint16_t                 getADCHandleTemp(uint8_t sample);
uint16_t                 getADCVin(uint8_t sample);
void                     HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim); // Since the hal header file does not define this one

#ifdef __cplusplus
}
#endif

#endif /* SETUP_H_ */
