#ifndef __MAIN_H
#define __MAIN_H

#include "OLED.hpp"
#include "Setup.h"
extern uint8_t  DetectedAccelerometerVersion;
extern uint32_t currentTempTargetDegC;
extern bool     settingsWereReset;
extern bool     usb_pd_available;
#ifdef __cplusplus
extern "C" {
#endif

void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed portCHAR *pcTaskName);

#define NO_DETECTED_ACCELEROMETER 99
#define ACCELEROMETERS_SCANNING   100
// Threads
void                startGUITask(void const *argument);
void                startPIDTask(void const *argument);
void                startMOVTask(void const *argument);
void                startPOWTask(void const *argument);
extern TaskHandle_t pidTaskNotification;
extern int32_t      powerSupplyWattageLimit;
extern uint8_t      accelInit;
extern TickType_t   lastMovementTime;
#ifdef __cplusplus
}
#endif
#endif /* __MAIN_H */
