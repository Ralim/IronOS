
#ifndef DRIVERS_USBPD_H_
#define DRIVERS_USBPD_H_
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
// #ifdef POW_PD
class USBPowerDelivery {
public:
  static bool start();               // Start the PD stack
  static bool negotiationComplete(); // Has negotiation completed to a voltage > 5v
  static bool fusbPresent();         // Is the FUSB302 present on the bus
  static void PPSTimerCallback();    // PPS Timer

private:
  //
  static int detectionState;
};
// #endif

#endif
#endif