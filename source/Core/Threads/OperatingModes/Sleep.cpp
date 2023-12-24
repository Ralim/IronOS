#include "OperatingModes.h"

OperatingMode gui_SolderingSleepingMode(const ButtonState buttons, guiContext *cxt) {
#ifdef NO_SLEEP_MODE
  return OperatingMode::Soldering;
#endif
  // Drop to sleep temperature and display until movement or button press

  // user moved or pressed a button, go back to soldering
  // If in the first two seconds we disable this to let accelerometer warm up

#ifdef POW_DC
  if (checkForUnderVoltage()) {
    return OperatingMode::HomeScreen; // return non-zero on error
  }
#endif

  if (cxt->scratch_state.state4) {
    // Hibernating mode
    currentTempTargetDegC = 0;
  } else {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      currentTempTargetDegC = TipThermoModel::convertFtoC(min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp)));
    } else {
      currentTempTargetDegC = min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp));
    }
  }
  // draw the lcd
  uint16_t tipTemp = getSettingValue(SettingsOptions::TemperatureInF) ? TipThermoModel::getTipInF() : TipThermoModel::getTipInC();

  OLED::clearScreen();
  OLED::setCursor(0, 0);
  if (getSettingValue(SettingsOptions::DetailedSoldering)) {
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
  } else {
    OLED::print(translatedString(Tr->SleepingSimpleString), FontStyle::LARGE);
    OLED::printNumber(tipTemp, 3, FontStyle::LARGE);
    OLED::printSymbolDeg(FontStyle::EXTRAS);
  }

  OLED::refresh();
  GUIDelay();

  if (!shouldBeSleeping()) {
    return cxt->previousMode;
  }

  if (shouldShutdown()) {
    // shutdown
    currentTempTargetDegC = 0;
    return OperatingMode::HomeScreen;
  }
  if (cxt->scratch_state.state4) {
    return OperatingMode::Hibernating;
  } else {
    return OperatingMode::Sleeping;
  }
}
