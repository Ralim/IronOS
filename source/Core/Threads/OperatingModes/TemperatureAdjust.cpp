#include "OperatingModes.h"
void gui_solderingTempAdjust(void) {
  TickType_t lastChange              = xTaskGetTickCount();
  currentTempTargetDegC              = 0; // Turn off heater while adjusting temp
  TickType_t  autoRepeatTimer        = 0;
  uint8_t     autoRepeatAcceleration = 0;
  bool        waitForRelease         = false;
  ButtonState buttons                = getButtonState();
  if (buttons != BUTTON_NONE) {
    // Temp adjust entered by long-pressing F button.
    waitForRelease = true;
  }
  for (;;) {
    OLED::setCursor(0, 0);
    OLED::clearScreen();
    buttons = getButtonState();
    if (buttons) {
      if (waitForRelease) {
        buttons = BUTTON_NONE;
      }
      lastChange = xTaskGetTickCount();
    } else {
      waitForRelease = false;
    }
    int16_t delta = 0;
    switch (buttons) {
    case BUTTON_NONE:
      // stay
      autoRepeatAcceleration = 0;
      break;
    case BUTTON_BOTH:
      // exit
      return;
      break;
    case BUTTON_B_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        delta           = -getSettingValue(SettingsOptions::TempChangeLongStep);
        autoRepeatTimer = xTaskGetTickCount();
        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_B_SHORT:
      delta = -getSettingValue(SettingsOptions::TempChangeShortStep);
      break;
    case BUTTON_F_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        delta           = getSettingValue(SettingsOptions::TempChangeLongStep);
        autoRepeatTimer = xTaskGetTickCount();
        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_F_SHORT:
      delta = getSettingValue(SettingsOptions::TempChangeShortStep);
      break;
    default:
      break;
    }
    if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration) < PRESS_ACCEL_INTERVAL_MIN) {
      autoRepeatAcceleration = PRESS_ACCEL_INTERVAL_MAX - PRESS_ACCEL_INTERVAL_MIN;
    }
    // If buttons are flipped; flip the delta
    if (getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled)) {
      delta = -delta;
    }
    if (delta != 0) {
      // constrain between the set temp limits, i.e. 10-450 C
      int16_t newTemp = getSettingValue(SettingsOptions::SolderingTemp);
      newTemp += delta;
      // Round to nearest increment of delta
      delta   = abs(delta);
      newTemp = (newTemp / delta) * delta;

      if (getSettingValue(SettingsOptions::TemperatureInF)) {
        if (newTemp > MAX_TEMP_F)
          newTemp = MAX_TEMP_F;
        if (newTemp < MIN_TEMP_F)
          newTemp = MIN_TEMP_F;
      } else {
        if (newTemp > MAX_TEMP_C)
          newTemp = MAX_TEMP_C;
        if (newTemp < MIN_TEMP_C)
          newTemp = MIN_TEMP_C;
      }
      setSettingValue(SettingsOptions::SolderingTemp, (uint16_t)newTemp);
    }
    if (xTaskGetTickCount() - lastChange > (TICKS_SECOND * 2))
      return; // exit if user just doesn't press anything for a bit

    if (OLED::getRotation()) {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolPlus : LargeSymbolMinus, FontStyle::LARGE);
    } else {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolMinus : LargeSymbolPlus, FontStyle::LARGE);
    }

    OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    OLED::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::LARGE);
    if (getSettingValue(SettingsOptions::TemperatureInF))
      OLED::drawSymbol(0);
    else {
      OLED::drawSymbol(1);
    }
    OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    if (OLED::getRotation()) {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolMinus : LargeSymbolPlus, FontStyle::LARGE);
    } else {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolPlus : LargeSymbolMinus, FontStyle::LARGE);
    }
    OLED::refresh();
    GUIDelay();
  }
}