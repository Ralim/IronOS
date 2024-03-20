#include "ui_drawing.hpp"
#ifdef OLED_96x16

void ui_draw_soldering_detailed_sleep(TemperatureType_t tipTemp) {

  OLED::clearScreen();
  OLED::setCursor(0, 0);
  OLED::print(translatedString(Tr->SleepingAdvancedString), FontStyle::SMALL);
  OLED::setCursor(0, 8);
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
  OLED::setCursor(0, 0);

  OLED::print(translatedString(Tr->SleepingSimpleString), FontStyle::LARGE);
  OLED::printNumber(tipTemp, 3, FontStyle::LARGE);
  OLED::printSymbolDeg(FontStyle::EXTRAS);

  OLED::refresh();
}
#endif