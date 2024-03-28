#include "ui_drawing.hpp"
#ifdef OLED_128x32

void ui_draw_warning_undervoltage(void) {
  OLED::clearScreen();
  OLED::setCursor(0, 0);
  if (getSettingValue(SettingsOptions::DetailedSoldering)) {
    OLED::print(translatedString(Tr->UndervoltageString), FontStyle::SMALL);
    OLED::setCursor(0, 8);
    OLED::print(translatedString(Tr->InputVoltageString), FontStyle::SMALL);
    printVoltage();
    OLED::print(SmallSymbolVolts, FontStyle::SMALL);
  } else {
    OLED::print(translatedString(Tr->UVLOWarningString), FontStyle::LARGE);
  }

  OLED::refresh();
  GUIDelay();
  waitForButtonPress();
}
#endif