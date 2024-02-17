#include "HUB238.hpp"
#include "OperatingModes.h"
#if POW_PD_EXT == 1
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
    if (screen > 6) {
      screen = 0;
    }
    if (screen == 0) {
      // Print the PD Debug state
      OLED::print(SmallSymbolState, FontStyle::SMALL);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      uint16_t temp = hub238_debug_state();
      OLED::drawHex(temp, FontStyle::SMALL, 4);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      // Print current selected specs
      temp = hub238_source_voltage();
      OLED::printNumber(temp, 2, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

    } else {

      // Print out the Proposed power options one by one
      const uint8_t voltages[]  = {5, 9, 12, 15, 18, 20};
      uint16_t      voltage     = voltages[screen - 1];
      uint16_t      currentx100 = hub238_getVoltagePDOCurrent(voltage);
      OLED::printNumber(voltage, 2, FontStyle::SMALL, true);
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);

      OLED::printNumber(currentx100 / 100, 1, FontStyle::SMALL, true);
      OLED::print(SmallSymbolDot, FontStyle::SMALL);
      OLED::printNumber(currentx100 % 100, 2, FontStyle::SMALL, true);
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
