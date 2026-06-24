/*
 * n32l40x_it.h
 *
 *  Cortex-M4 fault vector declarations for the Alientek T90 (Nations N32L40x).
 *  SysTick/PendSV/SVC are provided by the FreeRTOS port; ADC/TIM ISRs by IRQ.cpp.
 */
#ifndef __N32L40X_IT_H__
#define __N32L40X_IT_H__

#ifdef __cplusplus
extern "C" {
#endif

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __N32L40X_IT_H__ */
