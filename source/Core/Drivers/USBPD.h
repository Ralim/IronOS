
#ifndef DRIVERS_USBPD_H_
#define DRIVERS_USBPD_H_
#include "configuration.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#if POW_PD
class USBPowerDelivery {
public:
  static bool      start();                   // Start the PD stack
  static bool      negotiationComplete();     // Has negotiation completed to a voltage > 5v
  static bool      negotiationInProgress();   // Is negotiation ongoing
  static bool      fusbPresent();             // Is the FUSB302 present on the bus
  static void      PPSTimerCallback();        // PPS Timer
  static void      IRQOccured();              // Thread callback that an irq occured
  static void      step();                    // Iterate the step machine
  static bool      negotiationHasWorked();    // Has PD negotiation worked (are we in a PD contract)
  static uint8_t   getStateNumber();          // Debugging - Get the internal state number
  static bool      isVBUSConnected();         // Is the VBus pin connected on the FUSB302
  static uint32_t *getLastSeenCapabilities(); // returns pointer to the last seen capabilities from the powersource
private:
  //
  static int detectionState;
};
#endif

#endif
#endif