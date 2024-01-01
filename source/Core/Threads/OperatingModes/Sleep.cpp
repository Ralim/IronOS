#include "OperatingModes.h"

extern OperatingMode currentMode;

int gui_SolderingSleepingMode(bool stayOff, bool autoStarted) {
#ifndef NO_SLEEP_MODE
  // Drop to sleep temperature and display until movement or button press
  currentMode = OperatingMode::sleeping;

  for (;;) {
    // user moved or pressed a button, go back to soldering
    // If in the first two seconds we disable this to let accelerometer warm up

#ifdef POW_DC
    if (checkForUnderVoltage()) {
      // return non-zero on error
      return 1;
    }
#endif

    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      currentTempTargetDegC = stayOff ? 0 : TipThermoModel::convertFtoC(min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp)));
    } else {
      currentTempTargetDegC = stayOff ? 0 : min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp));
    }

    // draw the lcd
    TemperatureType_t tipTemp = getTipTemp();

    OLED::clearScreen();
    OLED::setCursor(0, 0);
    if (getSettingValue(SettingsOptions::DetailedSoldering)) {
      OLED::print(translatedString(Tr->SleepingAdvancedString), FontStyle::SMALL);
      OLED::setCursor(0, 8);
      OLED::print(translatedString(Tr->SleepingTipAdvancedString), FontStyle::SMALL);
      OLED::printNumber(tipTemp, 3, FontStyle::SMALL);
      OLED::printSymbolDeg(FontStyle::SMALL);
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

    if (!shouldBeSleeping(autoStarted)) {
      return 0;
    }

    if (shouldShutdown()) {
      // shutdown
      currentTempTargetDegC = 0;
      // we want to exit soldering mode
      return 1;
    }
  }
#endif

  return 0;
}
