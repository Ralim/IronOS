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
void timer0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state);
void adc_fifo_irq(void);
void start_adc_misc(void);
void GPIO_IRQHandler(void);
#ifdef __cplusplus
}
#endif
#endif /* BSP_PINE64_IRQ_H_ */
