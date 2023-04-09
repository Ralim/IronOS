#ifndef __MAIN_H
#define __MAIN_H
#include "OLED.hpp"
#include "Setup.h"
#include <stdint.h>
extern volatile uint32_t currentTempTargetDegC;
extern bool              settingsWereReset;
extern bool              usb_pd_available;
#ifdef __cplusplus
extern "C" {
#endif

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

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
enum class AccelType {
  Scanning = 0,
  None     = 1,
  MMA      = 2,
  LIS      = 3,
  BMA      = 4,
  MSA      = 5,
  SC7      = 6,
};
extern AccelType DetectedAccelerometerVersion;

#endif
#endif /* __MAIN_H */
