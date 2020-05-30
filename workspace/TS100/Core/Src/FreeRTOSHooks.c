/*
 * FreeRTOSHooks.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "FreeRTOSHooks.h"
#include "BSP.h"
void vApplicationIdleHook(void) {
	resetWatchdog();
}

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}

void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed portCHAR *pcTaskName) {
	(void) pxTask;
	(void) pcTaskName;
// We dont have a good way to handle a stack overflow at this point in time
	reboot();
}
