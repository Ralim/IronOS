/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */
#include "FreeRTOSConfig.h"
#include <stddef.h>
#include <stdint.h>

/* Type definitions. */
#if __riscv_xlen == 64
#define portSTACK_TYPE        uint64_t
#define portBASE_TYPE         int64_t
#define portUBASE_TYPE        uint64_t
#define portMAX_DELAY         (TickType_t)0xffffffffffffffffUL
#define portPOINTER_SIZE_TYPE uint64_t
#elif __riscv_xlen == 32
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE  int /* int32_t */
#define portUBASE_TYPE uint32_t
#define portMAX_DELAY  (TickType_t)0xffffffffUL
#ifdef __riscv_float_abi_single
/* better to use float replace double here,
 * so that it will generates floating point instructions
 */
#define portDOUBLE float /* double */
#else
#define portDOUBLE double
#endif
#else
#error Assembler did not define __riscv_xlen
#endif

typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE  BaseType_t;
typedef portUBASE_TYPE UBaseType_t;
typedef portUBASE_TYPE TickType_t;

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC 1
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH   (-1)
#define portTICK_PERIOD_MS ((TickType_t)1000 / configTICK_RATE_HZ)
#ifdef __riscv64
#error This is the RV32 port that has not yet been adapted for 64.
#define portBYTE_ALIGNMENT 16
#else
#define portBYTE_ALIGNMENT 16
#endif
/*-----------------------------------------------------------*/

/* Scheduler utilities. */
extern BaseType_t TrapNetCounter;
extern void       vTaskSwitchContext(void);
#define portYIELD() __asm volatile("ecall");
#define portEND_SWITCHING_ISR(xSwitchRequired) \
  if (xSwitchRequired)                         \
  vTaskSwitchContext()
#define portYIELD_FROM_ISR(x) portEND_SWITCHING_ISR(x)
/*-----------------------------------------------------------*/

/* Critical section management. */
#define portCRITICAL_NESTING_IN_TCB 1
extern void vTaskEnterCritical(void);
extern void vTaskExitCritical(void);

#define portSET_INTERRUPT_MASK_FROM_ISR()                     0
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedStatusValue) (void)uxSavedStatusValue
#define portDISABLE_INTERRUPTS()                              __asm volatile("csrc mstatus, 8")
#define portENABLE_INTERRUPTS()                               __asm volatile("csrs mstatus, 8")
#define portENTER_CRITICAL()                                  vTaskEnterCritical()
#define portEXIT_CRITICAL()                                   vTaskExitCritical()

/*-----------------------------------------------------------*/

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if (configUSE_PORT_OPTIMISED_TASK_SELECTION == 1)

/* Check the configuration. */
#if (configMAX_PRIORITIES > 32)
#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
#endif

/* Store/clear the ready priorities in a bit map. */
#define portRECORD_READY_PRIORITY(uxPriority, uxReadyPriorities) (uxReadyPriorities) |= (1UL << (uxPriority))
#define portRESET_READY_PRIORITY(uxPriority, uxReadyPriorities)  (uxReadyPriorities) &= ~(1UL << (uxPriority))

/*-----------------------------------------------------------*/

#define portGET_HIGHEST_PRIORITY(uxTopPriority, uxReadyPriorities) uxTopPriority = (31UL - __builtin_clz(uxReadyPriorities))

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not necessary for to use this port.  They are defined so the common demo files
(which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters)       void vFunction(void *pvParameters)

/*-----------------------------------------------------------*/

#define portNOP() __asm volatile(" nop ")

#define portINLINE __inline

#ifndef portFORCE_INLINE
#define portFORCE_INLINE inline __attribute__((always_inline))
#endif

#define portMEMORY_BARRIER() __asm volatile("" ::: "memory")

portFORCE_INLINE static BaseType_t xPortIsInsideInterrupt(void) { return TrapNetCounter ? 1 : 0; }

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
