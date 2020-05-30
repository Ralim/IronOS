#include "BSP.h"
#include "FreeRTOS.h"
#include "QC3.h"
#include "Settings.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "task.h"

void postRToSInit() {
#ifdef MODEL_TS80
    startQC(systemSettings.voltageDiv);
    while (pidTaskNotification == 0) osDelay(30);  // Wait for PID to start

    seekQC((systemSettings.cutoutSetting) ? 120 : 90,
           systemSettings.voltageDiv);  // this will move the QC output to the preferred voltage to start with

#else
    while (pidTaskNotification == 0) osDelay(30);  // Wait for PID to start
    osDelay(200);                                  // wait for accelerometer to stabilize
#endif
}
