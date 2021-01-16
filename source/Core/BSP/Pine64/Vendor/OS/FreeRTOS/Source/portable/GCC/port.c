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
 * Implementation of functions defined in portable.h for the Nuclei N/NX Processor port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

//#define ENABLE_KERNEL_DEBUG

#ifdef ENABLE_KERNEL_DEBUG
    #define FREERTOS_PORT_DEBUG(...)                printf(__VA_ARGS__)
#else
    #define FREERTOS_PORT_DEBUG(...)
#endif

#ifndef configSYSTICK_CLOCK_HZ
    #define configSYSTICK_CLOCK_HZ                  SOC_TIMER_FREQ
#endif

#ifndef configKERNEL_INTERRUPT_PRIORITY
    #define configKERNEL_INTERRUPT_PRIORITY         0
#endif

#ifndef configMAX_SYSCALL_INTERRUPT_PRIORITY
    // See function prvCheckMaxSysCallPrio and prvCalcMaxSysCallMTH
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    255
#endif

/* Constants required to check the validity of an interrupt priority. */
#define portFIRST_USER_INTERRUPT_NUMBER ( 18 )

#define SYSTICK_TICK_CONST          (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ)

/* Masks off all bits but the ECLIC MTH bits in the MTH register. */
#define portMTH_MASK                ( 0xFFUL )

/* Constants required to set up the initial stack. */
#define portINITIAL_MSTATUS         ( MSTATUS_MPP | MSTATUS_MPIE | MSTATUS_FS_INITIAL)
#define portINITIAL_EXC_RETURN      ( 0xfffffffd )

/* The systick is a 64-bit counter. */
#define portMAX_BIT_NUMBER          ( SysTimer_MTIMER_Msk )

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR    ( 45UL )

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS prvTaskExitError
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortSysTickHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
extern void prvPortStartFirstTask( void ) __attribute__ (( naked ));

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

#define xPortSysTickHandler     eclic_mtip_handler

/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

/*
 * Record the real MTH calculated by the configMAX_SYSCALL_INTERRUPT_PRIORITY
 * The configMAX_SYSCALL_INTERRUPT_PRIORITY is not the left-aligned level value,
 * See equations below:
 * Level Bits number: lvlbits = min(nlbits, CLICINTCTLBITS)
 * Left align Bits number: lfabits = 8-lvlbits
 * 0 < configMAX_SYSCALL_INTERRUPT_PRIORITY <= (2^lvlbits-1)
 * uxMaxSysCallMTH = (configMAX_SYSCALL_INTERRUPT_PRIORITY << lfabits) | ((2^lfabits)-1)
 * If nlbits = 3, CLICINTCTLBITS=3, then lvlbits = 3, lfabits = 5
 * Set configMAX_SYSCALL_INTERRUPT_PRIORITY to 6
 * Then uxMaxSysCallMTH = (6<<5) | (2^5 - 1) = 223
 *
 * See function prvCheckMaxSysCallPrio and prvCalcMaxSysCallMTH
 */
uint8_t uxMaxSysCallMTH = 255;

/*
 * The number of SysTick increments that make up one tick period.
 */
#if( configUSE_TICKLESS_IDLE == 1 )
    static TickType_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if( configUSE_TICKLESS_IDLE == 1 )
    static TickType_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if( configUSE_TICKLESS_IDLE == 1 )
    static TickType_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Used by the portASSERT_IF_INTERRUPT_PRIORITY_INVALID() macro to ensure
 * FreeRTOS API functions are not called from interrupts that have been assigned
 * a priority above configMAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#if( configASSERT_DEFINED == 1 )
     static uint8_t ucMaxSysCallPriority = 0;
#endif /* configASSERT_DEFINED */

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 * As per the standard RISC-V ABI pxTopcOfStack is passed in in a0, pxCode in
 * a1, and pvParameters in a2.  The new top of stack is passed out in a0.
 *
 * RISC-V maps registers to ABI names as follows (X1 to X31 integer registers
 * for the 'I' profile, X1 to X15 for the 'E' profile, currently I assumed).
 *
 * Register        ABI Name    Description                         Saver
 * x0              zero        Hard-wired zero                     -
 * x1              ra          Return address                      Caller
 * x2              sp          Stack pointer                       Callee
 * x3              gp          Global pointer                      -
 * x4              tp          Thread pointer                      -
 * x5-7            t0-2        Temporaries                         Caller
 * x8              s0/fp       Saved register/Frame pointer        Callee
 * x9              s1          Saved register                      Callee
 * x10-11          a0-1        Function Arguments/return values    Caller
 * x12-17          a2-7        Function arguments                  Caller
 * x18-27          s2-11       Saved registers                     Callee
 * x28-31          t3-6        Temporaries                         Caller
 *
 * The RISC-V context is saved RTOS tasks in the following stack frame,
 * where the global and thread pointers are currently assumed to be constant so
 * are not saved:
 *
 * mstatus
 * #ifndef __riscv_32e
 * x31
 * x30
 * x29
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * x18
 * x17
 * x16
 * #endif
 * x15
 * x14
 * x13
 * x12
 * x11
 * pvParameters
 * x9
 * x8
 * x7
 * x6
 * x5
 * portTASK_RETURN_ADDRESS
 * pxCode
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* Simulate the stack frame as it would be created by a context switch
    interrupt. */

    /* Offset added to account for the way the MCU uses the stack on entry/exit
    of interrupts, and to ensure alignment. */
    pxTopOfStack--;
    *pxTopOfStack = portINITIAL_MSTATUS;    /* MSTATUS */

    /* Save code space by skipping register initialisation. */
#ifndef __riscv_32e
    pxTopOfStack -= 22;    /* X11 - X31. */
#else
    pxTopOfStack -= 6;    /* X11 - X15. */
#endif
    *pxTopOfStack = ( StackType_t ) pvParameters;    /* X10/A0 */
    pxTopOfStack -= 6; /* X5 - X9 */
    *pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS;    /* RA, X1 */

    pxTopOfStack --;
    *pxTopOfStack = ( ( StackType_t ) pxCode ) ;    /* PC */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
    volatile uint32_t ulDummy = 0;

    /* A function that implements a task must not exit or attempt to return to
    its caller as there is nothing to return to.  If a task wants to exit it
    should instead call vTaskDelete( NULL ).

    Artificially force an assert() to be triggered if configASSERT() is
    defined, then stop here so application writers can catch the error. */
    configASSERT( uxCriticalNesting == ~0UL );
    portDISABLE_INTERRUPTS();
    while( ulDummy == 0 )
    {
        /* This file calls prvTaskExitError() after the scheduler has been
        started to remove a compiler warning about the function being defined
        but never called.  ulDummy is used purely to quieten other warnings
        about code appearing after this function is called - making ulDummy
        volatile makes the compiler think the function could return and
        therefore not output an 'unreachable code' warning for code that appears
        after it. */
        /* Sleep and wait for interrupt */
        __WFI();
    }
}
/*-----------------------------------------------------------*/

static uint8_t prvCheckMaxSysCallPrio( uint8_t max_syscall_prio )
{
    uint8_t nlbits = __ECLIC_GetCfgNlbits();
    uint8_t intctlbits = __ECLIC_INTCTLBITS;
    uint8_t lvlbits, temp;

    if (nlbits <= intctlbits) {
        lvlbits = nlbits;
    } else {
        lvlbits = intctlbits;
    }

    temp = ((1<<lvlbits) - 1);
    if (max_syscall_prio > temp) {
        max_syscall_prio = temp;
    }
    return max_syscall_prio;
}

static uint8_t prvCalcMaxSysCallMTH( uint8_t max_syscall_prio )
{
    uint8_t nlbits = __ECLIC_GetCfgNlbits();
    uint8_t intctlbits = __ECLIC_INTCTLBITS;
    uint8_t lvlbits, lfabits;
    uint8_t maxsyscallmth = 0;
    uint8_t temp;

    if (nlbits <= intctlbits) {
        lvlbits = nlbits;
    } else {
        lvlbits = intctlbits;
    }

    lfabits = 8 - lvlbits;

    temp = ((1<<lvlbits) - 1);
    if (max_syscall_prio > temp) {
        max_syscall_prio = temp;
    }

    maxsyscallmth = (max_syscall_prio << lfabits) | ((1<<lfabits) - 1);

    return maxsyscallmth;
}

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
    /* configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0. */
    configASSERT( configMAX_SYSCALL_INTERRUPT_PRIORITY );

    /* Get the real MTH should be set to ECLIC MTH register */
    uxMaxSysCallMTH = prvCalcMaxSysCallMTH(configMAX_SYSCALL_INTERRUPT_PRIORITY);
    FREERTOS_PORT_DEBUG("Max SysCall MTH is set to 0x%x\n", uxMaxSysCallMTH);

    #if( configASSERT_DEFINED == 1 )
    {
        /* Use the same mask on the maximum system call priority. */
        ucMaxSysCallPriority = prvCheckMaxSysCallPrio(configMAX_SYSCALL_INTERRUPT_PRIORITY);
        FREERTOS_PORT_DEBUG("Max SysCall Priority is set to %d\n", ucMaxSysCallPriority);
    }
    #endif /* conifgASSERT_DEFINED */

    __disable_irq();
    /* Start the timer that generates the tick ISR.  Interrupts are disabled
    here already. */
    vPortSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    /* Start the first task. */
    prvPortStartFirstTask();

    /* Should never get here as the tasks will now be executing!  Call the task
    exit error function to prevent compiler warnings about a static function
    not being called in the case that the application writer overrides this
    functionality by defining configTASK_RETURN_ADDRESS.  Call
    vTaskSwitchContext() so link time optimisation does not remove the
    symbol. */
    vTaskSwitchContext();
    prvTaskExitError();

    /* Should not get here! */
    return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented in ports where there is nothing to return to.
    Artificially force an assert. */
    configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;

    /* This is not the interrupt safe version of the enter critical function so
    assert() if it is being called from an interrupt context.  Only API
    functions that end in "FromISR" can be used in an interrupt.  Only assert if
    the critical nesting count is 1 to protect against recursive calls if the
    assert function also uses a critical section. */
    if( uxCriticalNesting == 1 )
    {
        configASSERT( ( __ECLIC_GetMth() & portMTH_MASK ) == uxMaxSysCallMTH );
    }
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
    configASSERT( uxCriticalNesting );
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

void vPortAssert( int32_t x )
{
    TaskHandle_t th;
    if ((x) == 0) {
        taskDISABLE_INTERRUPTS();
#if (INCLUDE_xTaskGetCurrentTaskHandle == 1)
        th = xTaskGetCurrentTaskHandle();
        if (th) {
            printf("Assert in task %s\n", pcTaskGetName(th));
        }
#endif
        while(1) {
            /* Sleep and wait for interrupt */
            __WFI();
        };
    }
}
/*-----------------------------------------------------------*/


void xPortTaskSwitch( void )
{
    portDISABLE_INTERRUPTS();
    /* Clear Software IRQ, A MUST */
    SysTimer_ClearSWIRQ();
    vTaskSwitchContext();
    portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
    /* The SysTick runs at the lowest interrupt priority, so when this interrupt
    executes all interrupts must be unmasked.  There is therefore no need to
    save and then restore the interrupt mask value as its value is already
    known. */
    portDISABLE_INTERRUPTS();
    {
        SysTick_Reload(SYSTICK_TICK_CONST);
        /* Increment the RTOS tick. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /* A context switch is required.  Context switching is performed in
            the SWI interrupt.  Pend the SWI interrupt. */
            portYIELD();
        }
    }
    portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

#if( configUSE_TICKLESS_IDLE == 1 )

    __attribute__((weak)) void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
    {
        uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements;
        volatile TickType_t xModifiableIdleTime, xTickCountBeforeSleep, XLastLoadValue;

        FREERTOS_PORT_DEBUG("Enter TickLess %d\n", (uint32_t)xExpectedIdleTime);

        /* Make sure the SysTick reload value does not overflow the counter. */
        if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
        {
            xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
        }

        /* Stop the SysTick momentarily.  The time the SysTick is stopped for
        is accounted for as best it can be, but using the tickless mode will
        inevitably result in some tiny drift of the time maintained by the
        kernel with respect to calendar time. */
        SysTimer_Stop();

        /* Calculate the reload value required to wait xExpectedIdleTime
        tick periods.  -1 is used because this code will execute part way
        through one of the tick periods. */
        ulReloadValue = ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
        if( ulReloadValue > ulStoppedTimerCompensation )
        {
            ulReloadValue -= ulStoppedTimerCompensation;
        }

        /* Enter a critical section but don't use the taskENTER_CRITICAL()
        method as that will mask interrupts that should exit sleep mode. */
        __disable_irq();

        /* If a context switch is pending or a task is waiting for the scheduler
        to be unsuspended then abandon the low power entry. */
        if( eTaskConfirmSleepModeStatus() == eAbortSleep )
        {
            /* Restart from whatever is left in the count register to complete
            this tick period. */
            /* Restart SysTick. */
            SysTimer_Start();

            /* Reset the reload register to the value required for normal tick
               periods. */
            SysTick_Reload(ulTimerCountsForOneTick);

            /* Re-enable interrupts - see comments above the cpsid instruction()
               above. */
            __enable_irq();
        }
        else
        {
            xTickCountBeforeSleep = xTaskGetTickCount();

            /* Set the new reload value. */
            SysTick_Reload(ulReloadValue);

            /* Get System timer load value before sleep */
            XLastLoadValue = SysTimer_GetLoadValue();

            /* Restart SysTick. */
            SysTimer_Start();
            ECLIC_EnableIRQ(SysTimer_IRQn);
            __RWMB();

            /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
            set its parameter to 0 to indicate that its implementation contains
            its own wait for interrupt or wait for event instruction, and so wfi
            should not be executed again.  However, the original expected idle
            time variable must remain unmodified, so a copy is taken. */
            xModifiableIdleTime = xExpectedIdleTime;
            configPRE_SLEEP_PROCESSING( xModifiableIdleTime );
            if( xModifiableIdleTime > 0 )
            {
                __WFI();
            }
            configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

            /* Re-enable interrupts to allow the interrupt that brought the MCU
            out of sleep mode to execute immediately. */
            __enable_irq();

            /* Make sure interrupt enable is executed */
            __RWMB();
            __FENCE_I();
            __NOP();

            /* Disable interrupts again because the clock is about to be stopped
               and interrupts that execute while the clock is stopped will increase
               any slippage between the time maintained by the RTOS and calendar
               time. */
            __disable_irq();

            /* Disable the SysTick clock.  Again,
               the time the SysTick is stopped for is accounted for as best it can
               be, but using the tickless mode will inevitably result in some tiny
               drift of the time maintained by the kernel with respect to calendar
               time*/
            ECLIC_DisableIRQ(SysTimer_IRQn);

            /* Determine if SysTimer Interrupt is not yet happened,
            (in which case an interrupt other than the SysTick
            must have brought the system out of sleep mode). */
            if (SysTimer_GetLoadValue() >= (XLastLoadValue + ulReloadValue))
            {
                /* As the pending tick will be processed as soon as this
                function exits, the tick value maintained by the tick is stepped
                forward by one less than the time spent waiting. */
                ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
                FREERTOS_PORT_DEBUG("TickLess - SysTimer Interrupt Entered!\n");
            }
            else
            {
                /* Something other than the tick interrupt ended the sleep.
                Work out how long the sleep lasted rounded to complete tick
                periods (not the ulReload value which accounted for part
                ticks). */
                xModifiableIdleTime = SysTimer_GetLoadValue();
                if ( xModifiableIdleTime > XLastLoadValue ) {
                    ulCompletedSysTickDecrements = (xModifiableIdleTime - XLastLoadValue);
                } else {
                    ulCompletedSysTickDecrements = (xModifiableIdleTime + portMAX_BIT_NUMBER - XLastLoadValue);
                }

                /* How many complete tick periods passed while the processor
                was waiting? */
                ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

                /* The reload value is set to whatever fraction of a single tick
                period remains. */
                SysTick_Reload(ulTimerCountsForOneTick);
                FREERTOS_PORT_DEBUG("TickLess - External Interrupt Happened!\n");
            }

            FREERTOS_PORT_DEBUG("End TickLess %d\n", (uint32_t)ulCompleteTickPeriods);

            /* Restart SysTick */
            vTaskStepTick( ulCompleteTickPeriods );

            /* Exit with interrupts enabled. */
            ECLIC_EnableIRQ(SysTimer_IRQn);
            __enable_irq();
        }
    }

#endif /* #if configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__attribute__(( weak )) void vPortSetupTimerInterrupt( void )
{
    /* Calculate the constants required to configure the tick interrupt. */
    #if( configUSE_TICKLESS_IDLE == 1 )
    {
        ulTimerCountsForOneTick = ( SYSTICK_TICK_CONST );
        xMaximumPossibleSuppressedTicks = portMAX_BIT_NUMBER / ulTimerCountsForOneTick;
        ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / ( configCPU_CLOCK_HZ / configSYSTICK_CLOCK_HZ );
        FREERTOS_PORT_DEBUG("CountsForOneTick, SuppressedTicks and TimerCompensation: %u, %u, %u\n", \
                (uint32_t)ulTimerCountsForOneTick, (uint32_t)xMaximumPossibleSuppressedTicks, (uint32_t)ulStoppedTimerCompensation);
    }
    #endif /* configUSE_TICKLESS_IDLE */
    TickType_t ticks = SYSTICK_TICK_CONST;

    /* Make SWI and SysTick the lowest priority interrupts. */
    /* Stop and clear the SysTimer. SysTimer as Non-Vector Interrupt */
    SysTick_Config(ticks);
    ECLIC_DisableIRQ(SysTimer_IRQn);
    ECLIC_SetLevelIRQ(SysTimer_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_SetShvIRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT);
    ECLIC_EnableIRQ(SysTimer_IRQn);

    /* Set SWI interrupt level to lowest level/priority, SysTimerSW as Vector Interrupt */
    ECLIC_SetShvIRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT);
    ECLIC_SetLevelIRQ(SysTimerSW_IRQn, configKERNEL_INTERRUPT_PRIORITY);
    ECLIC_EnableIRQ(SysTimerSW_IRQn);
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

#if( configASSERT_DEFINED == 1 )

    void vPortValidateInterruptPriority( void )
    {
        uint32_t ulCurrentInterrupt;
        uint8_t ucCurrentPriority;

        /* Obtain the number of the currently executing interrupt. */
        CSR_MCAUSE_Type mcause = (CSR_MCAUSE_Type)__RV_CSR_READ(CSR_MCAUSE);
        /* Make sure current trap type is interrupt */
        configASSERT(mcause.b.interrupt == 1);
        if (mcause.b.interrupt) {
            ulCurrentInterrupt = mcause.b.exccode;
            /* Is the interrupt number a user defined interrupt? */
            if ( ulCurrentInterrupt >= portFIRST_USER_INTERRUPT_NUMBER ) {
                /* Look up the interrupt's priority. */
                ucCurrentPriority = __ECLIC_GetLevelIRQ(ulCurrentInterrupt);
                /* The following assertion will fail if a service routine (ISR) for
                an interrupt that has been assigned a priority above
                ucMaxSysCallPriority calls an ISR safe FreeRTOS API
                function.  ISR safe FreeRTOS API functions must *only* be called
                from interrupts that have been assigned a priority at or below
                ucMaxSysCallPriority.

                Numerically low interrupt priority numbers represent logically high
                interrupt priorities, therefore the priority of the interrupt must
                be set to a value equal to or numerically *higher* than
                ucMaxSysCallPriority.

                Interrupts that use the FreeRTOS API must not be left at their
                default priority of zero as that is the highest possible priority,
                which is guaranteed to be above ucMaxSysCallPriority,
                and therefore also guaranteed to be invalid.

                FreeRTOS maintains separate thread and ISR API functions to ensure
                interrupt entry is as fast and simple as possible.

                The following links provide detailed information:
                http://www.freertos.org/FAQHelp.html */
                configASSERT( ucCurrentPriority <= ucMaxSysCallPriority );
            }
        }
    }

#endif /* configASSERT_DEFINED */
