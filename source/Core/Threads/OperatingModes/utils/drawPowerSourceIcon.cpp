#include "OperatingModeUtilities.h"

void gui_drawBatteryIcon(void) {
#if defined(POW_PD) || defined(POW_QC)
  if (!getIsPoweredByDCIN()) {
    // On non-DC inputs we replace this symbol with the voltage we are operating on
    // If <9V then show single digit, if not show dual small ones vertically stacked
    uint16_t V = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    if (V % 10 >= 5)
      V = (V / 10) + 1; // round up
    else
      V = V / 10;
    if (V > 9) {
      int16_t xPos = OLED::getCursorX();
      OLED::printNumber(V / 10, 1, FontStyle::SMALL);
      OLED::setCursor(xPos, 8);
      OLED::printNumber(V % 10, 1, FontStyle::SMALL);
      OLED::setCursor(xPos + 12, 0); // need to reset this as if we drew a wide char
    } else {
      OLED::printNumber(V, 1, FontStyle::LARGE);
    }
    return;
  }
#endif
#ifdef POW_DC
  if (getSettingValue(SettingsOptions::MinDCVoltageCells)) {
    // User is on a lithium battery
    // we need to calculate which of the 10 levels they are on
    uint8_t  cellCount = getSettingValue(SettingsOptions::MinDCVoltageCells) + 2;
    uint32_t cellV     = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0) / cellCount;
    // Should give us approx cell voltage X10
    // Range is 42 -> Minimum voltage setting (systemSettings.minVoltageCells) = 9 steps therefore we will use battery 0-9
    if (cellV < getSettingValue(SettingsOptions::MinVoltageCells))
      cellV = getSettingValue(SettingsOptions::MinVoltageCells);
    cellV -= getSettingValue(SettingsOptions::MinVoltageCells); // Should leave us a number of 0-9
    if (cellV > 9)
      cellV = 9;
    OLED::drawBattery(cellV + 1);
  } else {
    OLED::drawSymbol(15); // Draw the DC Logo
  }
#endif
}