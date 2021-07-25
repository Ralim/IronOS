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
  // Init any other misc sensors
  postRToSInit();
  uint32_t notificationValue = 0;
  for (;;) {
    notificationValue = 0;
    xTaskNotifyWait(0x0, 0xFFFFFF, &notificationValue, TICKS_100MS);
#ifdef POW_PD
    if (notificationValue) {
      USBPowerDelivery::IRQOccured();
    }
    USBPowerDelivery::step();
    USBPowerDelivery::PPSTimerCallback();
#endif
    power_check();
  }
}
