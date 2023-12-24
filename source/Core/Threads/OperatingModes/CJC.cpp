

#include "OperatingModes.h"
OperatingMode performCJCC(const ButtonState buttons, guiContext *cxt) {
  // Calibrate Cold Junction Compensation directly at boot, before internal components get warm.
  if (!isTipDisconnected() && abs(int(TipThermoModel::getTipInC() - getHandleTemperature(0) / 10)) < 10) {
    // Take 16 samples, only sample
    if (cxt->scratch_state.state1 < 16) {
      if ((xTaskGetTickCount() - cxt->scratch_state.state4) > TICKS_100MS) {
        cxt->scratch_state.state3 += getTipRawTemp(1);
        cxt->scratch_state.state1++;
        cxt->scratch_state.state4 = xTaskGetTickCount();
      }
      OLED::setCursor(0, 0);
      OLED::print(translatedString(Tr->CJCCalibrating), FontStyle::SMALL);
      OLED::setCursor(0, 8);
      OLED::print(SmallSymbolDot, FontStyle::SMALL);
      for (uint8_t x = 0; x < (cxt->scratch_state.state1 / 4); x++) {
        OLED::print(SmallSymbolDot, FontStyle::SMALL);
      }

      return OperatingMode::CJCCalibration;
    }

    // If the thermo-couple at the end of the tip, and the handle are at
    // equilibrium, then the output should be zero, as there is no temperature
    // differential.

    uint16_t setOffset = TipThermoModel::convertTipRawADCTouV(cxt->scratch_state.state3 / 16, true);
    setSettingValue(SettingsOptions::CalibrationOffset, setOffset);
    if (warnUser(translatedString(Tr->CalibrationDone), buttons)) {
      // Preventing to repeat calibration at boot automatically (only one shot).
      setSettingValue(SettingsOptions::CalibrateCJC, 0);
      saveSettings();
      return OperatingMode::InitialisationDone;
    }
    return OperatingMode::CJCCalibration;
  }
  // Cant run calibration without the tip and for temps to be close
  return OperatingMode::StartupWarnings;
}
