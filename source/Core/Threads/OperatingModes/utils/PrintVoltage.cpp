#include "OperatingModeUtilities.h"

void printVoltage(void) {
  uint32_t volt = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
  OLED::printNumber(volt / 10, 2, FontStyle::SMALL);
  OLED::print(SmallSymbolDot, FontStyle::SMALL);
  OLED::printNumber(volt % 10, 1, FontStyle::SMALL);
}