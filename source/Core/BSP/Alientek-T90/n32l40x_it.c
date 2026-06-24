/*
 * n32l40x_it.c
 *
 *  Cortex-M4 fault vectors only for the Alientek T90 (Nations N32L40x).
 *
 *  SysTick_Handler / PendSV_Handler / SVC_Handler are owned by the FreeRTOS port
 *  (aliased in FreeRTOSConfig.h via USE_RTOS_SYSTICK) and must NOT be defined here.
 *  ADC_IRQHandler / TIM4_IRQHandler are the functional ISRs defined in IRQ.cpp.
 *  Defining any of those here would create duplicate symbols at link time.
 */
#include "n32l40x.h"
#include "n32l40x_it.h"

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void) {}

void HardFault_Handler(void) {
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1) {
  }
}

void MemManage_Handler(void) {
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1) {
  }
}

void BusFault_Handler(void) {
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1) {
  }
}

void UsageFault_Handler(void) {
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1) {
  }
}

void DebugMon_Handler(void) {}
