#include "power.hpp"
#include "ui_drawing.hpp"

void ui_draw_soldering_power_status(void) {
  if (OLED::getRotation()) {
    OLED::setCursor(0, 0);
  } else {
    OLED::setCursor(67, 0);
  }
  // Print wattage
  {
    uint32_t x10Watt = x10WattHistory.average();
    if (x10Watt > 999) {
      // If we exceed 99.9W we drop the decimal place to keep it all fitting
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      OLED::printNumber(x10WattHistory.average() / 10, 3, FontStyle::SMALL);
    } else {
      OLED::printNumber(x10WattHistory.average() / 10, 2, FontStyle::SMALL);
      OLED::print(SmallSymbolDot, FontStyle::SMALL);
      OLED::printNumber(x10WattHistory.average() % 10, 1, FontStyle::SMALL);
    }
    OLED::print(SmallSymbolWatts, FontStyle::SMALL);
  }

  if (OLED::getRotation()) {
    OLED::setCursor(0, 8);
  } else {
    OLED::setCursor(67, 8);
  }
  printVoltage();
  OLED::print(SmallSymbolVolts, FontStyle::SMALL);
}