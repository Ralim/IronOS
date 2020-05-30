#ifndef __MAIN_H
#define __MAIN_H

#include "OLED.hpp"
#include "Setup.h"
extern uint8_t PCBVersion;
extern uint32_t currentTempTargetDegC;
extern bool settingsWereReset;

#ifdef __cplusplus
extern "C" {
#endif

void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed portCHAR *pcTaskName);

//Threads
void startGUITask(void const *argument);
void startPIDTask(void const *argument);
void startMOVTask(void const *argument);
extern TaskHandle_t pidTaskNotification;
extern uint8_t accelInit;
extern uint32_t lastMovementTime;
#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */
