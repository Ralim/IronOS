#include "FreeRTOSConfig.h"
//
#include "FreeRTOS.h"
#include "gd32vf103.h"
#include "n200_eclic.h"
#include "n200_func.h"
#include "n200_timer.h"
#include "portmacro.h"
#include "riscv_encoding.h"
#include "task.h"
/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Each task maintains its own interrupt status in the critical nesting variable. */
UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

#if USER_MODE_TASKS
#ifdef __riscv_flen
unsigned long MSTATUS_INIT = (MSTATUS_MPIE | (0x1 << 13));
#else
unsigned long MSTATUS_INIT = (MSTATUS_MPIE);
#endif
#else
#ifdef __riscv_flen
unsigned long MSTATUS_INIT = (MSTATUS_MPP | MSTATUS_MPIE | (0x1 << 13));
#else
unsigned long MSTATUS_INIT = (MSTATUS_MPP | MSTATUS_MPIE);
#endif
#endif

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError(void);

/**
 * @brief System Call Trap
 * 
 * @param mcause csr
 * @param sp 触发系统调用时的栈地址
 * @param arg1 ECALL macro stores argument in a2
 * @return unsigned long 传入的sp
 */
unsigned long ulSynchTrap(unsigned long mcause, unsigned long sp, unsigned long arg1) {
    switch (mcause & 0X00000fff) {
    //on User and Machine ECALL, handler the request
    case 8:
    case 11: {
        if (arg1 == IRQ_DISABLE) {
            //zero out mstatus.mpie
            clear_csr(mstatus, MSTATUS_MPIE);
        } else if (arg1 == IRQ_ENABLE) {
            //set mstatus.mpie
            set_csr(mstatus, MSTATUS_MPIE);
        } else if (arg1 == PORT_YIELD) {
            //always yield from machine mode
            //fix up mepc on sync trap
            unsigned long epc = read_csr(mepc);
            vPortYield_from_ulSynchTrap(sp, epc + 4);
        } else if (arg1 == PORT_YIELD_TO_RA) {
            vPortYield_from_ulSynchTrap(sp, (*(unsigned long *)(sp + 1 * sizeof(sp))));
        }
        break;
    }
    default: {
        /* 异常处理 */
        extern uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp);
        handle_trap(mcause, sp);
    }
    }

    //fix mepc and return
    unsigned long epc = read_csr(mepc);

    write_csr(mepc, epc + 4);
    return sp;
}
/*-----------------------------------------------------------*/

/**
 * @brief 设置触发软中断
 * @note 目的是在软中断内进行任务上下文切换
 * 
 */
void vPortSetMSIPInt(void) {
    *(volatile uint8_t *)(TIMER_CTRL_ADDR + TIMER_MSIP) |= 0x01;
    __asm volatile("fence");
    __asm volatile("fence.i");
}
/*-----------------------------------------------------------*/

/**
 * @brief 清除软中断
 * 
 */
void vPortClearMSIPInt(void) {
    *(volatile uint8_t *)(TIMER_CTRL_ADDR + TIMER_MSIP) &= ~0x01;
}
/*-----------------------------------------------------------*/

/**
 * @brief 执行任务上下文切换,在portasm.S中被调用
 * 
 * @param sp 触发任务切换时的栈地址
 * @param arg1 
 * @return unsigned long sp地址
 */
unsigned long taskswitch(unsigned long sp, unsigned long arg1) {
    //always yield from machine mode
    //fix up mepc on
    unsigned long epc = read_csr(mepc);
    vPortYield(sp, epc); //never returns

    return sp;
}
/*-----------------------------------------------------------*/

/**
 * @brief 调研freertos内建函数vTaskSwitchContext,在portasm.S中被调用
 * 
 */
void vDoTaskSwitchContext(void) {
    portDISABLE_INTERRUPTS();
    vTaskSwitchContext();
    portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/**
 * @brief 进入临界段
 * 
 */
void vPortEnterCritical(void) {
#if USER_MODE_TASKS
    ECALL(IRQ_DISABLE);
#else
    portDISABLE_INTERRUPTS();
#endif

    uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

/**
 * @brief 退出临界段
 * 
 */
void vPortExitCritical(void) {
    configASSERT(uxCriticalNesting);
    uxCriticalNesting--;
    if (uxCriticalNesting == 0) {
#if USER_MODE_TASKS
        ECALL(IRQ_ENABLE);
#else
        portENABLE_INTERRUPTS();
#endif
    }
    return;
}
/*-----------------------------------------------------------*/

/**
 * @brief Clear current interrupt mask and set given mask
 * 
 * @param int_mask mth值
 */
void vPortClearInterruptMask(int int_mask) {
    eclic_set_mth(int_mask);
}
/*-----------------------------------------------------------*/

/**
 * @brief Set interrupt mask and return current interrupt enable register
 * 
 * @return int 
 */
int xPortSetInterruptMask(void) {
    int int_mask = 0;
    int_mask = eclic_get_mth();

    portDISABLE_INTERRUPTS();
    return int_mask;
}
/*-----------------------------------------------------------*/

/**
 * @brief 初始化任务栈帧
 * 
 * @param pxTopOfStack 栈顶
 * @param pxCode 任务入口
 * @param pvParameters 任务参数
 * @return StackType_t* 完成初始化后的栈顶
 */
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters) {
    /* Simulate the stack frame as it would be created by a context switch
	interrupt. */
#ifdef __riscv_flen
    pxTopOfStack -= 32; /* 浮点寄存器 */
#endif

    pxTopOfStack--;
    *pxTopOfStack = 0xb8000000; /* CSR_MCAUSE */

    pxTopOfStack--;
    *pxTopOfStack = 0x40; /* CSR_SUBM */

    pxTopOfStack--;
    *pxTopOfStack = (portSTACK_TYPE)pxCode; /* Start address */

    pxTopOfStack--;
    *pxTopOfStack = MSTATUS_INIT; /* CSR_MSTATUS */

    pxTopOfStack -= 22;
    *pxTopOfStack = (portSTACK_TYPE)pvParameters; /* Register a0 */

    pxTopOfStack -= 9;
    *pxTopOfStack = (portSTACK_TYPE)prvTaskExitError; /* Register ra */
    pxTopOfStack--;

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

/**
 * @brief 任务退出函数
 * 
 */
void prvTaskExitError(void) {
    /* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).
	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
    configASSERT(uxCriticalNesting == ~0UL);
    portDISABLE_INTERRUPTS();
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

/**
 * @brief tick中断
 * @note 由于该中断配置为向量模式，则中断到来会调用portasm.S的MTIME_HANDLER,进行栈帧保存之后该函数会调用vPortSysTickHandler
 * 
 */
void vPortSysTickHandler(void) {
    volatile uint64_t *mtime = (uint64_t *)(TIMER_CTRL_ADDR + TIMER_MTIME);
    volatile uint64_t *mtimecmp = (uint64_t *)(TIMER_CTRL_ADDR + TIMER_MTIMECMP);

    UBaseType_t uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();

#if CONFIG_SYSTEMVIEW_EN
    traceISR_ENTER();
#endif

    uint64_t now = *mtime;
    now += (configRTC_CLOCK_HZ / configTICK_RATE_HZ);
    *mtimecmp = now;

    /* 调用freertos的tick增加接口 */
    if (xTaskIncrementTick() != pdFALSE) {
#if CONFIG_SYSTEMVIEW_EN
        traceISR_EXIT_TO_SCHEDULER();
#endif
        portYIELD();
    }
#if CONFIG_SYSTEMVIEW_EN
    else {
        traceISR_EXIT();
    }
#endif

    portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
}
/*-----------------------------------------------------------*/

/**
 * @brief 初始化tick
 * 
 */
void vPortSetupTimer(void) {
    /* 内核timer定时器使用64位的计数器来实现 */
    volatile uint64_t *mtime = (uint64_t *)(TIMER_CTRL_ADDR + TIMER_MTIME);
    volatile uint64_t *mtimecmp = (uint64_t *)(TIMER_CTRL_ADDR + TIMER_MTIMECMP);

    portENTER_CRITICAL();
    uint64_t now = *mtime;
    now += (configRTC_CLOCK_HZ / configTICK_RATE_HZ);
    *mtimecmp = now;
    portEXIT_CRITICAL();

    eclic_set_vmode(CLIC_INT_TMR);
    eclic_irq_enable(CLIC_INT_TMR, configKERNEL_INTERRUPT_PRIORITY >> configPRIO_BITS, 0);
}
/*-----------------------------------------------------------*/

/**
 * @brief 初始化软中断
 * 
 */
void vPortSetupMSIP(void) {
    eclic_set_vmode(CLIC_INT_SFT);
    eclic_irq_enable(CLIC_INT_SFT, configKERNEL_INTERRUPT_PRIORITY >> configPRIO_BITS, 0);
}
/*-----------------------------------------------------------*/

/**
 * @brief 调度启动前的初始化准备
 * 
 */
void vPortSetup(void) {
    vPortSetupTimer();
    vPortSetupMSIP();
    uxCriticalNesting = 0;
}
/*-----------------------------------------------------------*/
