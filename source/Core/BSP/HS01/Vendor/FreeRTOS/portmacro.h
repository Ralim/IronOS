/*
 * FreeRTOS port for the WCH CH57x (QingKe RISC-V, PFIC + SysTick, no CLINT)
 *
 * Based on the WCH Qingke V4A port (CH583 EVT, Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.)
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
#include <stdint.h>
#include "CH573SFR.h"
#include "core_riscv.h"
// clang-format on
// CH573SFR.h defines these as macros, which breaks IronOS functions of the same names
#undef min
#undef max

/* Type definitions. */
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE  long
#define portUBASE_TYPE unsigned long
#define portMAX_DELAY  (TickType_t)0xffffffffUL

typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE  BaseType_t;
typedef portUBASE_TYPE UBaseType_t;
typedef portUBASE_TYPE TickType_t;

#define portCHAR   char
#define portFLOAT  float
#define portDOUBLE double
#define portLONG   long
#define portSHORT  short

#define portTICK_TYPE_IS_ATOMIC 1

/* Architecture specifics. */
#define portSTACK_GROWTH   (-1)
#define portTICK_PERIOD_MS ((TickType_t)1000 / configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT 16

/* Scheduler utilities. The context switch is done in the software interrupt (SW_Handler in portASM.S). */
extern void vTaskSwitchContext(void);
#define portYIELD() PFIC_SetPendingIRQ(SWI_IRQn)
#define portEND_SWITCHING_ISR(xSwitchRequired)                                                                                                                                                         \
  do {                                                                                                                                                                                                 \
    if (xSwitchRequired)                                                                                                                                                                               \
      portYIELD();                                                                                                                                                                                     \
  } while (0)
#define portYIELD_FROM_ISR(x) portEND_SWITCHING_ISR(x)

/* Critical section management. Interrupts never nest (all run at the same PFIC priority), so ISRs need no mask. */
extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);
#define portSET_INTERRUPT_MASK_FROM_ISR()                     0
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedStatusValue) (void)(uxSavedStatusValue)
#define portDISABLE_INTERRUPTS()                              __asm volatile("csrc mstatus, 8\n nop\n nop\n nop" ::: "memory")
#define portENABLE_INTERRUPTS()                               __asm volatile("csrs mstatus, 8" ::: "memory")
#define portENTER_CRITICAL()                                  vPortEnterCritical()
#define portEXIT_CRITICAL()                                   vPortExitCritical()

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if (configUSE_PORT_OPTIMISED_TASK_SELECTION == 1)
#if (configMAX_PRIORITIES > 32)
#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.
#endif
#define portRECORD_READY_PRIORITY(uxPriority, uxReadyPriorities)   (uxReadyPriorities) |= (1UL << (uxPriority))
#define portRESET_READY_PRIORITY(uxPriority, uxReadyPriorities)    (uxReadyPriorities) &= ~(1UL << (uxPriority))
#define portGET_HIGHEST_PRIORITY(uxTopPriority, uxReadyPriorities) uxTopPriority = (31UL - __builtin_clz(uxReadyPriorities))
#endif

#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters)       void vFunction(void *pvParameters)

#define portNOP()  __asm volatile(" nop ")
#define portINLINE __inline
#ifndef portFORCE_INLINE
#define portFORCE_INLINE inline __attribute__((always_inline))
#endif
#define portMEMORY_BARRIER() __asm volatile("" ::: "memory")

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
