

#include "OperatingModes.h"
void performCJCC(void) {
  // Calibrate Cold Junction Compensation directly at boot, before internal components get warm.
  OLED::refresh();
  osDelay(50);
  if (!isTipDisconnected() && abs(int(TipThermoModel::getTipInC() - getHandleTemperature(0) / 10)) < 10) {
    uint16_t setoffset = 0;
    // If the thermo-couple at the end of the tip, and the handle are at
    // equilibrium, then the output should be zero, as there is no temperature
    // differential.
    while (setoffset == 0) {
      uint32_t offset = 0;
      for (uint8_t i = 0; i < 16; i++) {
        offset += getTipRawTemp(1);
        // cycle through the filter a fair bit to ensure we're stable.
        OLED::clearScreen();
        OLED::setCursor(0, 0);
        OLED::print(translatedString(Tr->CJCCalibrating), FontStyle::SMALL);
        OLED::setCursor(0, 8);
        OLED::print(SmallSymbolDot, FontStyle::SMALL);
        for (uint8_t x = 0; x < (i / 4); x++)
          OLED::print(SmallSymbolDot, FontStyle::SMALL);
        OLED::refresh();
        osDelay(100);
      }
      setoffset = TipThermoModel::convertTipRawADCTouV(offset / 16, true);
    }
    setSettingValue(SettingsOptions::CalibrationOffset, setoffset);
    OLED::clearScreen();
    warnUser(translatedString(Tr->CJCCalibrationDone), 3 * TICKS_SECOND);
    OLED::refresh();
    // Preventing to repeat calibration at boot automatically (only one shot).
    setSettingValue(SettingsOptions::CalibrateCJC, 0);
    saveSettings();
  }
}
