#include "BSP.h"
#include "BSP_Power.h"
#include "Model_Config.h"
#include "Pins.h"
#include "QC3.h"
#include "Settings.h"
#include "fusbpd.h"
#include "int_n.h"
#include "policy_engine.h"
bool FUSB302_present = false;
bool FUSB302_probed  = false;

void power_check() {
#ifdef POW_PD
  if (FUSB302_present) {
    PolicyEngine::PPSTimerCallback();
    // Cant start QC until either PD works or fails
    if (PolicyEngine::setupCompleteOrTimedOut() == false) {
      return;
    }
    if (PolicyEngine::pdHasNegotiated()) {
      return;
    }
  }
#endif
#ifdef POW_QC
  QC_resync();
#endif
}
uint8_t usb_pd_detect() {
#ifdef POW_PD
  if (FUSB302_probed) {
    return FUSB302_present;
  } else {
    FUSB302_present = fusb302_detect();
    FUSB302_probed  = true;
  }
  return FUSB302_present;
#endif
  return false;
}

bool getIsPoweredByDCIN() {
  // We return false until we are sure we are not using PD
  if (PolicyEngine::setupCompleteOrTimedOut() == false) {
    return false;
  }
  if (PolicyEngine::pdHasNegotiated()) {
    return false; // We are using PD
  }
  if (hasQCNegotiated()) {
    return false; // We are using QC
  }
  return true;
}
