/*
 * FreeRTOS port for the WCH CH57x (QingKe RISC-V, PFIC + SysTick, no CLINT)
 *
 * Based on the WCH Qingke V4A port (CH583 EVT, Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.)
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "task.h"

#include "CH57x_common.h"
#include <string.h>

#define portCONTEXT_WORDS   32
#define portINITIAL_MSTATUS 0x1880 // MPP = M, MPIE = 1

// Interrupts stay masked in critical sections taken before the scheduler runs
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

extern void xPortStartFirstTask(void);

static void prvTaskExitError(void) {
  configASSERT(uxCriticalNesting == ~0UL);
  portDISABLE_INTERRUPTS();
  for (;;) {
  }
}

StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters) {
  pxTopOfStack -= portCONTEXT_WORDS;
  memset(pxTopOfStack, 0, portCONTEXT_WORDS * sizeof(StackType_t));
  pxTopOfStack[0]  = (StackType_t)pxCode;           // mepc
  pxTopOfStack[1]  = (StackType_t)prvTaskExitError; // x1 / ra
  pxTopOfStack[7]  = (StackType_t)pvParameters;     // x10 / a0
  pxTopOfStack[29] = portINITIAL_MSTATUS;           // x4 slot, read as mstatus by xPortStartFirstTask
  return pxTopOfStack;
}

// The tick and the context switch share the lowest (and only) PFIC priority with every other interrupt, so no
// interrupt can preempt the kernel while it is inside one of them.
static void vPortSetupTimerInterrupt(void) {
  PFIC_EnableIRQ(SWI_IRQn);
  SysTick->CMP  = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1;
  SysTick->CTLR = (1 << 8) | (1 << 2) | (1 << 1) | (1 << 0); // auto reload | HCLK | interrupt | enable
  PFIC_EnableIRQ(SysTick_IRQn);
}

BaseType_t xPortStartScheduler(void) {
  vPortSetupTimerInterrupt();
  uxCriticalNesting = 0;
  xPortStartFirstTask();
  return pdFAIL;
}

void vPortEndScheduler(void) {
  for (;;) {
  }
}

__attribute__((interrupt("machine"), section(".highcode"))) void SysTick_Handler(void) {
  SysTick->CNTFG &= ~(1 << 1);
  if (xTaskIncrementTick() != pdFALSE) {
    portYIELD();
  }
}

void vPortEnterCritical(void) {
  portDISABLE_INTERRUPTS();
  uxCriticalNesting++;
}

void vPortExitCritical(void) {
  configASSERT(uxCriticalNesting);
  uxCriticalNesting--;
  if (uxCriticalNesting == 0) {
    portENABLE_INTERRUPTS();
  }
}
