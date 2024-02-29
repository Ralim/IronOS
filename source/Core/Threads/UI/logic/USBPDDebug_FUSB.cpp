#include "OperatingModes.h"
#include "ui_drawing.hpp"
#ifdef POW_PD
#ifdef HAS_POWER_DEBUG_MENU
OperatingMode showPDDebug(const ButtonState buttons, guiContext *cxt) {
  // Print out the USB-PD state
  // Basically this is like the Debug menu, but instead we want to print out the PD status
  uint16_t *screen = &(cxt->scratch_state.state1);

  if ((*screen) == 0) {
    // Print the PD state machine
    uint8_t vbusState = 0;
    if (USBPowerDelivery::fusbPresent()) {
      if (USBPowerDelivery::negotiationComplete() || (xTaskGetTickCount() > (TICKS_SECOND * 10))) {
        if (!USBPowerDelivery::isVBUSConnected()) {
          vbusState = 2;
        } else {
          vbusState = 1;
        }
      }
    }
    ui_draw_usb_pd_debug_state(vbusState, USBPowerDelivery::getStateNumber());
  } else {
    // Print out the Proposed power options one by one
    auto lastCaps = USBPowerDelivery::getLastSeenCapabilities();
    if (((*screen) - 1) < 11) {
      int voltage_mv     = 0;
      int min_voltage    = 0;
      int current_a_x100 = 0;
      int wattage        = 0;

      if ((lastCaps[(*screen) - 1] & PD_PDO_TYPE) == PD_PDO_TYPE_FIXED) {
        voltage_mv     = PD_PDV2MV(PD_PDO_SRC_FIXED_VOLTAGE_GET(lastCaps[(*screen) - 1])); // voltage in mV units
        current_a_x100 = PD_PDO_SRC_FIXED_CURRENT_GET(lastCaps[(*screen) - 1]);            // current in 10mA units
      } else if (((lastCaps[(*screen) - 1] & PD_PDO_TYPE) == PD_PDO_TYPE_AUGMENTED) && ((lastCaps[(*screen) - 1] & PD_APDO_TYPE) == PD_APDO_TYPE_AVS)) {
        voltage_mv  = PD_PAV2MV(PD_APDO_AVS_MAX_VOLTAGE_GET(lastCaps[(*screen) - 1]));
        min_voltage = PD_PAV2MV(PD_APDO_PPS_MIN_VOLTAGE_GET(lastCaps[(*screen) - 1]));
        // Last value is wattage
        wattage = PD_APDO_AVS_MAX_POWER_GET(lastCaps[(*screen) - 1]);
      } else if (((lastCaps[(*screen) - 1] & PD_PDO_TYPE) == PD_PDO_TYPE_AUGMENTED) && ((lastCaps[(*screen) - 1] & PD_APDO_TYPE) == PD_APDO_TYPE_PPS)) {
        voltage_mv     = PD_PAV2MV(PD_APDO_PPS_MAX_VOLTAGE_GET(lastCaps[(*screen) - 1]));
        min_voltage    = PD_PAV2MV(PD_APDO_PPS_MIN_VOLTAGE_GET(lastCaps[(*screen) - 1]));
        current_a_x100 = PD_PAI2CA(PD_APDO_PPS_CURRENT_GET(lastCaps[(*screen) - 1])); // max current in 10mA units
      }
      // Skip not used entries
      if (voltage_mv == 0) {
        (*screen) += 1;
      } else {
        ui_draw_usb_pd_debug_pdo(*screen, min_voltage / 1000, voltage_mv / 1000, current_a_x100, wattage);
      }
    } else {
      (*screen) = 0;
    }
  }
  if (buttons == BUTTON_B_SHORT) {
    return OperatingMode::InitialisationDone;
  } else if (buttons == BUTTON_F_SHORT) {
    (*screen) += 1;
  }
  return OperatingMode::UsbPDDebug;
}
#endif
#endif
