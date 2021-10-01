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
#include "configuration.h"
#include "main.hpp"
#include "stdlib.h"
#include "task.h"

// Small worker thread to handle power (PD + QC) related steps

void startPOWTask(void const *argument __unused) {
  // Init any other misc sensors
  postRToSInit();
  // You have to run this once we are willing to answer PD messages
  // Setting up too early can mean that we miss the ~20ms window to respond on some chargers

#if POW_PD
  USBPowerDelivery::start();

#endif
  for (;;) {
#if POW_PD

    if (getFUS302IRQLow()) {
      USBPowerDelivery::IRQOccured();
    }
    USBPowerDelivery::step();
    USBPowerDelivery::PPSTimerCallback();

#endif
    power_check();
    xTaskNotifyWait(0x0, 0xFFFFFF, NULL, TICKS_10MS);
  }
}
