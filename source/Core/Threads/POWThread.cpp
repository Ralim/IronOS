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
  while (preStartChecksDone() == 0) {
    osDelay(3);
  }
  // You have to run this once we are willing to answer PD messages
  // Setting up too early can mean that we miss the ~20ms window to respond on some chargers
#if POW_PD
  USBPowerDelivery::start();
  // Crank the handle at boot until we are stable and waiting for IRQ
  USBPowerDelivery::step();

#endif
  BaseType_t res;
  for (;;) {
    res = pdFALSE;
    // While the interrupt is low, dont delay
    /*This is due to a possible race condition, where:
     * IRQ fires
     * We read interrupt register but dont see the Good CRC
     * Then Good CRC is set while reading it out (racing on I2C read)
     * Then we would sleep as nothing to do, but 100ms> 20ms power supply typical timeout
     */
    if (!getFUS302IRQLow()) {
      res = xTaskNotifyWait(0x0, 0xFFFFFF, NULL, TICKS_100MS / 2);
    }

#if POW_PD
    if (res != pdFALSE || getFUS302IRQLow()) {
      USBPowerDelivery::IRQOccured();
    }
    USBPowerDelivery::PPSTimerCallback();
    USBPowerDelivery::step();

#else
    (void)res;
#endif
    power_check();
  }
}
