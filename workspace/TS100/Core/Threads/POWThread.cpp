/*
 * POWThread.cpp
 *
 *  Created on: 16 Jan 2021
 *      Author: Ralim
 */

#include "BSP.h"
#include "FreeRTOS.h"
#include "QC3.h"
#include "Settings.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "stdlib.h"
#include "task.h"

// Small worker thread to handle power (mostly QC) related steps

void startPOWTask(void const *argument __unused) {
	postRToSInit();
	for (;;) {
		osDelay(TICKS_100MS); // Slow down update rate
		power_check();
	}
}
