/*
 * Irqs.h
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#ifndef BSP_PINE64_IRQ_H_
#define BSP_PINE64_IRQ_H_

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Setup.h"
#include "main.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void timer0_comp0_callback(void);
void timer0_comp1_callback(void);
void timer0_comp2_callback(void);
void adc_fifo_irq(void);
void GPIO_IRQHandler(void);
void switchToSlowPWM(void);
#ifdef __cplusplus
}
#endif
#endif /* BSP_PINE64_IRQ_H_ */
