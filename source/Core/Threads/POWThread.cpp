/*
 * POWThread.cpp
 *
 *  Created on: 16 Jan 2021
 *      Author: Ralim
 */

#include "BSP.h"
#include "FS2711.hpp"
#include "FreeRTOS.h"
#include "HUB238.hpp"
#include "QC3.h"
#include "Settings.h"
#include "USBPD.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "main.hpp"
#include "stdbool.h"
#include "stdlib.h"
#include "task.h"

volatile bool hasDoneSecondStagePDRenegotiate = false;
// Small worker thread to handle power (PD + QC) related steps

void startPOWTask(void const *argument __unused) {
  // Init any other misc sensors
  postRToSInit();
  BaseType_t res;
#ifdef POW_PD
  USBPowerDelivery::start();
  // Crank the handle at boot until we are stable
  USBPowerDelivery::step();
#endif

  while (preStartChecksDone() == 0) {
#ifdef POW_PD
    USBPowerDelivery::step();
    if (!getFUS302IRQLow()) {
      res = xTaskNotifyWait(0x0, 0xFFFFFF, NULL, TICKS_100MS / 4);
    }
    if (res != pdFALSE || getFUS302IRQLow()) {
      USBPowerDelivery::IRQOccured();
    }
    USBPowerDelivery::step();
#else
    osDelay(3);
#endif
  }
#if POW_PD_EXT == 2
  FS2711::start();
  FS2711::negotiate();
#endif

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

#ifdef POW_PD
    if (res != pdFALSE || getFUS302IRQLow()) {
      USBPowerDelivery::IRQOccured();
    }
    if (USBPowerDelivery::negotiationHasWorked() && !hasDoneSecondStagePDRenegotiate) {
      USBPowerDelivery::renegotiate();
      hasDoneSecondStagePDRenegotiate = true;
    }
    USBPowerDelivery::PPSTimerCallback();
    USBPowerDelivery::step();

#else
    (void)res;
#endif
#if POW_PD_EXT == 1
    hub238_check_negotiation();
#endif
#if POW_PD_EXT == 2
    FS2711::negotiate();
#endif
    power_check();
  }
}
