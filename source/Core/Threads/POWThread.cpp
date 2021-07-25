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
#include "USBPD.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "stdlib.h"
#include "task.h"

// Small worker thread to handle power (mostly QC) related steps

void startPOWTask(void const *argument __unused) {
  // You have to run this once we are willing to answer PD messages
  // Setting up too early can mean that we miss the ~20ms window to respond on some chargers
#ifdef POW_PD
  USBPowerDelivery::start();
#endif
  vTaskDelay(TICKS_100MS);
  // Init any other misc sensors
  postRToSInit();

  for (;;) {
    power_check();
    osDelay(TICKS_100MS); // Slow down update rate
  }
}
