#include "power.hpp"
#include "ui_drawing.hpp"
#include <OperatingModes.h>
#ifdef OLED_96x16

void ui_draw_soldering_power_status(bool boost_mode_on) {
  if (OLED::getRotation()) {
    OLED::setCursor(50, 0);
  } else {
    OLED::setCursor(-1, 0);
  }

  ui_draw_tip_temperature(true, FontStyle::LARGE);

  if (boost_mode_on) { // Boost mode is on
    if (OLED::getRotation()) {
      OLED::setCursor(34, 0);
    } else {
      OLED::setCursor(50, 0);
    }
    OLED::print(LargeSymbolPlus, FontStyle::LARGE);
  } else {
#ifndef NO_SLEEP_MODE
    if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {
      if (OLED::getRotation()) {
        OLED::setCursor(32, 0);
      } else {
        OLED::setCursor(47, 0);
      }
      printCountdownUntilSleep(getSleepTimeout());
    }
#endif
    if (OLED::getRotation()) {
      OLED::setCursor(32, 8);
    } else {
      OLED::setCursor(47, 8);
    }
    OLED::print(PowerSourceNames[getPowerSourceNumber()], FontStyle::SMALL, 2);
  }

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
#endif