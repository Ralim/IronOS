#include "BSP.h"
#include "BSP_Power.h"
#include "Pins.h"
#include "QC3.h"
#include "Settings.h"
#include "USBPD.h"
#include "configuration.h"

void power_check() {
#if POW_PD

  // Cant start QC until either PD works or fails
  if (USBPowerDelivery::negotiationComplete()) {
    return;
  }
#endif
}

bool getIsPoweredByDCIN() { return false; }

uint8_t getTipResistanceX10() { return TIP_RESISTANCE; }