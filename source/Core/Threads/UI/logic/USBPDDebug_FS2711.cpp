#include "FS2711.hpp"
#include "OperatingModes.h"
#include "stdbool.h"
#if POW_PD_EXT == 2
#ifdef HAS_POWER_DEBUG_MENU

OperatingMode showPDDebug(const ButtonState buttons, guiContext *cxt) {
  // Print out the USB-PD state
  // Basically this is like the Debug menu, but instead we want to print out the PD status
  uint8_t     screen = 0;
  ButtonState b;
  for (;;) {
    OLED::clearScreen();                               // Ensure the buffer starts clean
    OLED::setCursor(0, 0);                             // Position the cursor at the 0,0 (top left)
    OLED::print(SmallSymbolPDDebug, FontStyle::SMALL); // Print Title
    OLED::setCursor(0, 8);                             // second line
    if (screen > 7) {
      screen = 0;
    }
    if (screen == 0) {
      // Print the PD Debug state
      OLED::print(SmallSymbolState, FontStyle::SMALL);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      fs2711_state_t state = FS2711::debug_get_state();

      OLED::printNumber(state.pdo_num, 1, FontStyle::SMALL);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      //     OLED::drawHex(state.req_pdo_num, FontStyle::SMALL, 4);
      OLED::printNumber(state.req_pdo_num > 7 ? 0 : state.req_pdo_num + 1, 1, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      uint8_t protocol = FS2711::selected_protocol();
      OLED::printNumber(protocol, 2, FontStyle::SMALL);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
    } else {

      // Print out the Proposed power options one by one
      uint16_t max_voltage = FS2711::debug_pdo_max_voltage(screen - 1);
      if (max_voltage == 0) {
        screen += 1;
      } else {
        uint16_t min_voltage = FS2711::debug_pdo_min_voltage(screen - 1);
        uint16_t current     = FS2711::debug_pdo_source_current(screen - 1);
        uint16_t pdo_type    = FS2711::debug_pdo_type(screen - 1);

        OLED::printNumber(screen, 1, FontStyle::SMALL, true);
        OLED::print(SmallSymbolSpace, FontStyle::SMALL);

        if (pdo_type == 1) {
          OLED::printNumber(min_voltage / 1000, 2, FontStyle::SMALL, true);
          OLED::print(SmallSymbolMinus, FontStyle::SMALL);
          OLED::printNumber(max_voltage / 1000, 2, FontStyle::SMALL, false);
        } else {
          OLED::printNumber(max_voltage / 1000, 2, FontStyle::SMALL, true);
        }
        OLED::print(SmallSymbolVolts, FontStyle::SMALL);
        OLED::print(SmallSymbolSpace, FontStyle::SMALL);

        OLED::printNumber(current / 1000, 2, FontStyle::SMALL, true);
        OLED::print(SmallSymbolDot, FontStyle::SMALL);
        OLED::printNumber(current % 1000, 1, FontStyle::SMALL, false);
        OLED::print(SmallSymbolAmps, FontStyle::SMALL);
        // OLED::printNumber(currentx100 % 100, 2, FontStyle::SMALL, true);
      }
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT) {
      return OperatingMode::InitialisationDone;
    } else if (b == BUTTON_F_SHORT) {
      screen++;
    }

    GUIDelay();
  }
  return OperatingMode::UsbPDDebug;
}
#endif
#endif
