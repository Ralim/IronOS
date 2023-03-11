#include "OperatingModes.h"

extern OperatingMode currentMode;

int gui_SolderingSleepingMode(bool stayOff, bool autoStarted) {
  // Drop to sleep temperature and display until movement or button press
  currentMode = OperatingMode::sleeping;

  for (;;) {
    // user moved or pressed a button, go back to soldering
    // If in the first two seconds we disable this to let accelerometer warm up

#ifdef POW_DC
    if (checkForUnderVoltage())
      return 1; // return non-zero on error
#endif
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      currentTempTargetDegC = stayOff ? 0 : TipThermoModel::convertFtoC(min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp)));
    } else {
      currentTempTargetDegC = stayOff ? 0 : min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp));
    }
    // draw the lcd
    uint16_t tipTemp;
    if (getSettingValue(SettingsOptions::TemperatureInF))
      tipTemp = TipThermoModel::getTipInF();
    else {
      tipTemp = TipThermoModel::getTipInC();
    }

    OLED::clearScreen();
    OLED::setCursor(0, 0);
    if (getSettingValue(SettingsOptions::DetailedSoldering)) {
      OLED::print(translatedString(Tr->SleepingAdvancedString), FontStyle::SMALL);
      OLED::setCursor(0, 8);
      OLED::print(translatedString(Tr->SleepingTipAdvancedString), FontStyle::SMALL);
      OLED::printNumber(tipTemp, 3, FontStyle::SMALL);
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::print(SmallSymbolDegF, FontStyle::SMALL);
      else {
        OLED::print(SmallSymbolDegC, FontStyle::SMALL);
      }

      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      printVoltage();
      OLED::print(SmallSymbolVolts, FontStyle::SMALL);
    } else {
      OLED::print(translatedString(Tr->SleepingSimpleString), FontStyle::LARGE);
      OLED::printNumber(tipTemp, 3, FontStyle::LARGE);
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::drawSymbol(0);
      else {
        OLED::drawSymbol(1);
      }
    }

    OLED::refresh();
    GUIDelay();

    if (!shouldBeSleeping(autoStarted)) {
      return 0;
    }

    if (shouldShutdown()) {
      // shutdown
      currentTempTargetDegC = 0;
      return 1; // we want to exit soldering mode
    }
  }
  return 0;
}
