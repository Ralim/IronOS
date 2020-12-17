/*
 * FreeRTOSHooks.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#ifndef INC_FREERTOSHOOKS_H_
#define INC_FREERTOSHOOKS_H_

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "unit.h"

#ifdef __cplusplus
extern "C" {
#endif

// RToS
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
void vApplicationIdleHook(void);
#ifdef __cplusplus
}
#endif


#endif /* INC_FREERTOSHOOKS_H_ */
