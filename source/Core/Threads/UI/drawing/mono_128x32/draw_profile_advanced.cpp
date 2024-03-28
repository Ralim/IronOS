#include "ui_drawing.hpp"

#ifdef OLED_128x32
void ui_draw_soldering_profile_advanced(TemperatureType_t tipTemp, TemperatureType_t profileCurrentTargetTemp, uint32_t phaseElapsedSeconds, uint32_t phase, const uint32_t phaseTimeGoal) {
  // print temperature
  if (OLED::getRotation()) {
    OLED::setCursor(48, 0);
  } else {
    OLED::setCursor(0, 0);
  }

  OLED::printNumber(tipTemp, 3, FontStyle::SMALL);
  OLED::print(SmallSymbolSlash, FontStyle::SMALL);
  OLED::printNumber(profileCurrentTargetTemp, 3, FontStyle::SMALL);

  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    OLED::print(SmallSymbolDegF, FontStyle::SMALL);
  } else {
    OLED::print(SmallSymbolDegC, FontStyle::SMALL);
  }

  // print phase
  if (phase > 0 && phase <= getSettingValue(SettingsOptions::ProfilePhases)) {
    if (OLED::getRotation()) {
      OLED::setCursor(36, 0);
    } else {
      OLED::setCursor(55, 0);
    }
    OLED::printNumber(phase, 1, FontStyle::SMALL);
  }

  // print time progress / preheat / cooldown
  if (OLED::getRotation()) {
    OLED::setCursor(42, 8);
  } else {
    OLED::setCursor(0, 8);
  }

  if (phase == 0) {
    OLED::print(translatedString(Tr->ProfilePreheatString), FontStyle::SMALL);
  } else if (phase > getSettingValue(SettingsOptions::ProfilePhases)) {
    OLED::print(translatedString(Tr->ProfileCooldownString), FontStyle::SMALL);
  } else {
    OLED::printNumber(phaseElapsedSeconds / 60, 1, FontStyle::SMALL);
    OLED::print(SmallSymbolColon, FontStyle::SMALL);
    OLED::printNumber(phaseElapsedSeconds % 60, 2, FontStyle::SMALL, false);

    OLED::print(SmallSymbolSlash, FontStyle::SMALL);

    // blink if we can't keep up with the time goal
    if (phaseElapsedSeconds < phaseTimeGoal + 2 || (xTaskGetTickCount() / TICKS_SECOND) % 2 == 0) {
      OLED::printNumber(phaseTimeGoal / 60, 1, FontStyle::SMALL);
      OLED::print(SmallSymbolColon, FontStyle::SMALL);
      OLED::printNumber(phaseTimeGoal % 60, 2, FontStyle::SMALL, false);
    }
  }
}

#endif