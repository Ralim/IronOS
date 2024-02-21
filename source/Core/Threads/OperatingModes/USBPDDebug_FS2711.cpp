#include "FS2711.hpp"
#include "OperatingModes.h"
#include "stdbool.h"
// #define POW_PD_EXT 2
// #define HAS_POWER_DEBUG_MENU
#if POW_PD_EXT == 2
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
    if (screen > 7) {
      screen = 0;
    }
    if (screen == 0) {
      // Print the PD Debug state
      // OLED::drawHex(state.up, FontStyle::SMALL, 4);
      OLED::print(SmallSymbolState, FontStyle::SMALL);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      fs2711_state_t state = FS2711::get_state();
      OLED::printNumber(state.up, 1, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      OLED::printNumber(state.pdo_num, 1, FontStyle::SMALL);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      OLED::printNumber(state.req_pdo_num, 1, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      OLED::printNumber(state.failed_pdo_checks, 1, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      // OLED::printNumber(state.pps, 1, FontStyle::SMALL, true);
      // OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      // OLED::printNumber(state.negotiated, 1, FontStyle::SMALL, true);
      // OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      // OLED::printNumber(state.req_pdo_volt, 1, FontStyle::SMALL, true);
      // OLED::print(SmallSymbolSpace, FontStyle::SMALL);

    } else {

      // Print out the Proposed power options one by one
      uint16_t voltage = FS2711::debug_pdo_source_voltage(screen - 1);
      uint16_t current = FS2711::debug_pdo_source_current(screen - 1);
      uint16_t pd_type = FS2711::debug_pdo_type(screen - 1);
      OLED::printNumber(screen, 2, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      OLED::printNumber(pd_type, 1, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      OLED::printNumber(voltage, 6, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      OLED::printNumber(current, 6, FontStyle::SMALL, true);
      // OLED::print(SmallSymbolDot, FontStyle::SMALL);
      // OLED::printNumber(currentx100 % 100, 2, FontStyle::SMALL, true);
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT) {
      return;
    } else if (b == BUTTON_F_SHORT) {
      screen++;
    }

    GUIDelay();
  }
}
#endif
#endif
