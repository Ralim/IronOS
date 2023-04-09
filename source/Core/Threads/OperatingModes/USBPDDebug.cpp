#include "OperatingModes.h"

#if POW_PD
#ifdef HAS_POWER_DEBUG_MENU
void showPDDebug(void) {
  // Print out the USB-PD state
  // Basically this is like the Debug menu, but instead we want to print out the PD status
  uint8_t     screen = 0;
  ButtonState b;
  for (;;) {
    OLED::clearScreen();                               // Ensure the buffer starts clean
    OLED::setCursor(0, 0);                             // Position the cursor at the 0,0 (top left)
    OLED::print(SmallSymbolPDDebug, FontStyle::SMALL); // Print Title
    OLED::setCursor(0, 8);                             // second line
    if (screen == 0) {
      // Print the PD state machine
      OLED::print(SmallSymbolState, FontStyle::SMALL);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      OLED::printNumber(USBPowerDelivery::getStateNumber(), 2, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      // Also print vbus mod status
      if (USBPowerDelivery::fusbPresent()) {
        if (USBPowerDelivery::negotiationComplete() || (xTaskGetTickCount() > (TICKS_SECOND * 10))) {
          if (!USBPowerDelivery::isVBUSConnected()) {
            OLED::print(SmallSymbolNoVBus, FontStyle::SMALL);
          } else {
            OLED::print(SmallSymbolVBus, FontStyle::SMALL);
          }
        }
      }
    } else {
      // Print out the Proposed power options one by one
      auto lastCaps = USBPowerDelivery::getLastSeenCapabilities();
      if ((screen - 1) < 11) {
        int voltage_mv     = 0;
        int min_voltage    = 0;
        int current_a_x100 = 0;
        int wattage        = 0;

        if ((lastCaps[screen - 1] & PD_PDO_TYPE) == PD_PDO_TYPE_FIXED) {
          voltage_mv     = PD_PDV2MV(PD_PDO_SRC_FIXED_VOLTAGE_GET(lastCaps[screen - 1])); // voltage in mV units
          current_a_x100 = PD_PDO_SRC_FIXED_CURRENT_GET(lastCaps[screen - 1]);            // current in 10mA units
        } else if (((lastCaps[screen - 1] & PD_PDO_TYPE) == PD_PDO_TYPE_AUGMENTED) && ((lastCaps[screen - 1] & PD_APDO_TYPE) == PD_APDO_TYPE_AVS)) {
          voltage_mv  = PD_PAV2MV(PD_APDO_AVS_MAX_VOLTAGE_GET(lastCaps[screen - 1]));
          min_voltage = PD_PAV2MV(PD_APDO_PPS_MIN_VOLTAGE_GET(lastCaps[screen - 1]));
          // Last value is wattage
          wattage = PD_APDO_AVS_MAX_POWER_GET(lastCaps[screen - 1]);
        } else if (((lastCaps[screen - 1] & PD_PDO_TYPE) == PD_PDO_TYPE_AUGMENTED) && ((lastCaps[screen - 1] & PD_APDO_TYPE) == PD_APDO_TYPE_PPS)) {
          voltage_mv     = PD_PAV2MV(PD_APDO_PPS_MAX_VOLTAGE_GET(lastCaps[screen - 1]));
          min_voltage    = PD_PAV2MV(PD_APDO_PPS_MIN_VOLTAGE_GET(lastCaps[screen - 1]));
          current_a_x100 = PD_PAI2CA(PD_APDO_PPS_CURRENT_GET(lastCaps[screen - 1])); // max current in 10mA units
        }
        // Skip not used entries
        if (voltage_mv == 0) {
          screen++;
        } else {
          // print out this entry of the proposal
          OLED::printNumber(screen, 2, FontStyle::SMALL, true); // print the entry number
          OLED::print(SmallSymbolSpace, FontStyle::SMALL);
          if (min_voltage > 0) {
            OLED::printNumber(min_voltage / 1000, 2, FontStyle::SMALL, true); // print the voltage
            OLED::print(SmallSymbolMinus, FontStyle::SMALL);
          }
          OLED::printNumber(voltage_mv / 1000, 2, FontStyle::SMALL, true); // print the voltage
          OLED::print(SmallSymbolVolts, FontStyle::SMALL);
          OLED::print(SmallSymbolSpace, FontStyle::SMALL);
          if (wattage) {
            OLED::printNumber(wattage, 3, FontStyle::SMALL, true); // print the current in 0.1A res
            OLED::print(SmallSymbolWatts, FontStyle::SMALL);
          } else {
            OLED::printNumber(current_a_x100 / 100, 2, FontStyle::SMALL, true); // print the current in 0.1A res
            OLED::print(SmallSymbolDot, FontStyle::SMALL);
            OLED::printNumber(current_a_x100 % 100, 2, FontStyle::SMALL, false); // print the current in 0.1A res
            OLED::print(SmallSymbolAmps, FontStyle::SMALL);
          }
        }
      } else {
        screen = 0;
      }
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT)
      return;
    else if (b == BUTTON_F_SHORT) {
      screen++;
    }
    GUIDelay();
  }
}
#endif
#endif
