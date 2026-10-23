#include "ui_drawing.hpp"

#ifdef OLED_128x32
void ui_draw_soldering_detailed_sleep(TemperatureType_t tipTemp) {

  OLED::clearScreen();
  OLED::setCursor(0, 0);
  OLED::print(translatedString(Tr->SleepingAdvancedString), FontStyle::SMALL);
  OLED::setCursor(0, 16);
  OLED::print(translatedString(Tr->SleepingTipAdvancedString), FontStyle::SMALL);
  OLED::printNumber(tipTemp, 3, FontStyle::SMALL);
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    OLED::print(SmallSymbolDegF, FontStyle::SMALL);
  } else {
    OLED::print(SmallSymbolDegC, FontStyle::SMALL);
  }

  OLED::print(SmallSymbolSpace, FontStyle::SMALL);
  printVoltage();
  OLED::print(SmallSymbolVolts, FontStyle::SMALL);

  OLED::refresh();
}

void ui_draw_soldering_basic_sleep(TemperatureType_t tipTemp) {

  OLED::clearScreen();
  // Centre "Zzz NNN°" on the panel: 48px sleep symbol + 36px (3 digits) + 12px degree = 96px,
  // and y=4 centres the 24px large font vertically.
  OLED::setCursor(16, 4);

  OLED::print(LargeSymbolSleep, FontStyle::LARGE);
  OLED::printNumber(tipTemp, 3, FontStyle::LARGE);
  OLED::printSymbolDeg(FontStyle::EXTRAS);

  OLED::refresh();
}
#endif