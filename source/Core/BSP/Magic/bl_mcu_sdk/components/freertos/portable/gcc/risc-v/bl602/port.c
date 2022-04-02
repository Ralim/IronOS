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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

#ifndef configCLINT_BASE_ADDRESS
#warning configCLINT_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a Core Local Interrupter (CLINT) then set configCLINT_BASE_ADDRESS to the CLINT base address.  Otherwise set configCLINT_BASE_ADDRESS to 0.
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
#define portTASK_RETURN_ADDRESS configTASK_RETURN_ADDRESS
#else
#define portTASK_RETURN_ADDRESS prvTaskExitError
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
static __attribute__((aligned(16))) StackType_t xISRStack[configISR_STACK_SIZE_WORDS] = { 0 };
const StackType_t xISRStackTop = (StackType_t) & (xISRStack[configISR_STACK_SIZE_WORDS & ~portBYTE_ALIGNMENT_MASK]);
#else
extern const uint32_t __freertos_irq_stack_top[];
const StackType_t xISRStackTop = (StackType_t)__freertos_irq_stack_top;
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt(void) __attribute__((weak));

/*-----------------------------------------------------------*/

/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
const size_t uxTimerIncrementsForOneTick = (size_t)(configCPU_CLOCK_HZ / configTICK_RATE_HZ); /* Assumes increment won't go over 32-bits. */
volatile uint64_t *const pullMachineTimerCompareRegisterBase = (volatile uint64_t *const)(configCLINT_BASE_ADDRESS + 0x4000);
volatile uint64_t *pullMachineTimerCompareRegister = 0;
BaseType_t TrapNetCounter = 0;
const BaseType_t *pTrapNetCounter = &TrapNetCounter;

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if (configCHECK_FOR_STACK_OVERFLOW > 2)
#warning This path not tested, or even compiled yet.
/* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
the task stacks, and so will legitimately appear in many positions within
the ISR stack. */
#define portISR_STACK_FILL_BYTE 0xee

static const uint8_t ucExpectedStackBytes[] = {
    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,
    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,
    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,
    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,
    portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE
};

#define portCHECK_ISR_STACK() configASSERT((memcmp((void *)xISRStack, (void *)ucExpectedStackBytes, sizeof(ucExpectedStackBytes)) == 0))
#else
/* Define the function away. */
#define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

/*-----------------------------------------------------------*/

#if (configCLINT_BASE_ADDRESS != 0)

void vPortSetupTimerInterrupt(void)
{
    uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
    volatile uint32_t *const pulTimeHigh = (volatile uint32_t *const)(configCLINT_BASE_ADDRESS + 0xBFFC);
    volatile uint32_t *const pulTimeLow = (volatile uint32_t *const)(configCLINT_BASE_ADDRESS + 0xBFF8);
    volatile uint32_t ulHartId = 0;

    __asm volatile("csrr %0, mhartid"
                   : "=r"(ulHartId));
    pullMachineTimerCompareRegister = &(pullMachineTimerCompareRegisterBase[ulHartId]);

    do {
        ulCurrentTimeHigh = *pulTimeHigh;
        ulCurrentTimeLow = *pulTimeLow;
    } while (ulCurrentTimeHigh != *pulTimeHigh);

    ullNextTime = (uint64_t)ulCurrentTimeHigh;
    ullNextTime <<= 32ULL;
    ullNextTime |= (uint64_t)ulCurrentTimeLow;
    ullNextTime += (uint64_t)uxTimerIncrementsForOneTick;
    *pullMachineTimerCompareRegister = ullNextTime;

    /* Prepare the time to use after the next tick interrupt. */
    ullNextTime += (uint64_t)uxTimerIncrementsForOneTick;
}

#endif /* ( configCLINT_BASE_ADDRESS != 0 ) */
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler(void)
{
    extern void xPortStartFirstTask(void);

#if (configASSERT_DEFINED == 1)
    {
        volatile uint32_t mtvec = 0;

        /* Check the least significant two bits of mtvec are 00 - indicating
        single vector mode. */
        __asm volatile("csrr %0, mtvec"
                       : "=r"(mtvec));
        //configASSERT( ( mtvec & 0x03UL ) == 0 );

        /* Check alignment of the interrupt stack - which is the same as the
        stack that was being used by main() prior to the scheduler being
        started. */
        configASSERT((xISRStackTop & portBYTE_ALIGNMENT_MASK) == 0);
    }
#endif /* configASSERT_DEFINED */

    /* If there is a CLINT then it is ok to use the default implementation
    in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
    configure whichever clock is to be used to generate the tick interrupt. */
    vPortSetupTimerInterrupt();

#if (configCLINT_BASE_ADDRESS != 0)
    {
        /* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
        for external interrupt.  _RB_ What happens here when mtime is not present as
        with pulpino? */
        __asm volatile("csrs mie, %0" ::"r"(0x880));
    }
#else
    {
        /* Enable external interrupts. */
        __asm volatile("csrs mie, %0" ::"r"(0x800));
    }
#endif /* configCLINT_BASE_ADDRESS */

    *(uint8_t *)(0x02800400 + 7) = 1;
    xPortStartFirstTask();

    /* Should not get here as after calling xPortStartFirstTask() only tasks
    should be executing. */
    return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
    /* Not implemented. */
    for (;;)
        ;
}
