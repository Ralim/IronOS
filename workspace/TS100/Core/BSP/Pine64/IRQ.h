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
#include "gd32vf103.h"
#include "main.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void ADC0_1_IRQHandler(void);
void TIMER1_IRQHandler(void);
#ifdef __cplusplus
}
#endif
#endif /* BSP_MINIWARE_IRQ_H_ */
