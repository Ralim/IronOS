#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
#include "configuration.h"
#ifdef POW_DC
extern volatile uint32_t currentTempTargetDegC;
// returns true if undervoltage has occured
bool checkForUnderVoltage(void) {
  if (!getIsPoweredByDCIN()) {
    return false;
  }
  uint16_t v = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);

  // Dont check for first 2 seconds while the ADC stabilizes and the DMA fills
  // the buffer
  if (xTaskGetTickCount() > (TICKS_SECOND * 2)) {
    if ((v < lookupVoltageLevel())) {
      currentTempTargetDegC = 0;
      OLED::clearScreen();
      OLED::setCursor(0, 0);
      if (getSettingValue(SettingsOptions::DetailedSoldering)) {
        OLED::print(translatedString(Tr->UndervoltageString), FontStyle::SMALL);
        OLED::setCursor(0, 8);
        OLED::print(translatedString(Tr->InputVoltageString), FontStyle::SMALL);
        printVoltage();
        OLED::print(SmallSymbolVolts, FontStyle::SMALL);
      } else {
        OLED::print(translatedString(Tr->UVLOWarningString), FontStyle::LARGE);
      }

      OLED::refresh();
      GUIDelay();
      waitForButtonPress();
      return true;
    }
  }
  return false;
}
#endif