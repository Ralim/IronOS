#include "BSP.h"
#include "BSP_Power.h"
#include "Pins.h"
#include "QC3.h"
#include "Settings.h"
#include "USBPD.h"
#include "configuration.h"

void power_check() {
#ifdef POW_PD
  // Cant start QC until either PD works or fails
  if (!USBPowerDelivery::negotiationComplete()) {
    return;
  }
  if (USBPowerDelivery::negotiationHasWorked()) {
    return; // We are using PD
  }
#endif
#ifdef POW_QC
  QC_resync();
#endif
}

bool getIsPoweredByDCIN() { return false; }
