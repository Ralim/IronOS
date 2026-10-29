/*
 * IRQ.h
 *
 * FNIRSI HS-01 (WCH CH571F)
 */

#ifndef BSP_HS01_IRQ_H_
#define BSP_HS01_IRQ_H_

#include "BSP.h"
#include "Setup.h"
#include "main.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void TMR1_IRQHandler(void);
void GPIOA_IRQHandler(void);
#ifdef __cplusplus
}
#endif
#endif /* BSP_HS01_IRQ_H_ */
