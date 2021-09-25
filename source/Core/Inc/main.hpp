#ifndef __MAIN_H
#define __MAIN_H

#include "OLED.hpp"
#include "Setup.h"
extern uint32_t currentTempTargetDegC;
extern bool     settingsWereReset;
extern bool     usb_pd_available;
#ifdef __cplusplus
extern "C" {
#endif

void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed portCHAR *pcTaskName);

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
// Accelerometer type
enum class AccelType { MMA = 1, LIS = 2, BMA = 3, MSA = 4, SC7 = 5, None = 99, Scanning = 100 };
extern AccelType DetectedAccelerometerVersion;

#endif
#endif /* __MAIN_H */
