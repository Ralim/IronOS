#include "ui_drawing.hpp"
#ifdef OLED_128x32
void ui_draw_power_source_icon(void) {
  const int16_t xPos = OLED::getCursorX();
#if defined(POW_PD) || defined(POW_QC) || defined(POW_PD_EXT)
  if (!getIsPoweredByDCIN()) {
    // On non-DC inputs we replace this symbol with the voltage we are operating on,
    // as two 8x16 digits stacked to fill the 32px panel height.
    uint16_t V = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    if (V % 10 >= 5) {
      V = (V / 10) + 1; // round up
    } else {
      V = V / 10;
    }
    OLED::setCursor(xPos, 0);
    OLED::printNumber(V / 10, 1, FontStyle::SMALL);
    OLED::setCursor(xPos, 16);
    OLED::printNumber(V % 10, 1, FontStyle::SMALL);
    return;
  }
#endif
#ifdef POW_DC
  OLED::setCursor(xPos, 8); // vertically centre the 16px battery / DC icon
  if (getSettingValue(SettingsOptions::MinDCVoltageCells)) {
    // User is on a lithium battery
    // we need to calculate which of the 10 levels they are on
    uint8_t  cellCount = getSettingValue(SettingsOptions::MinDCVoltageCells) + 2;
    uint32_t cellV     = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0) / cellCount;
    // Should give us approx cell voltage X10
    // Range is 42 -> Minimum voltage setting (systemSettings.minVoltageCells) = 9 steps therefore we will use battery 0-9
    if (cellV < getSettingValue(SettingsOptions::MinVoltageCells)) {
      cellV = getSettingValue(SettingsOptions::MinVoltageCells);
    }
    cellV -= getSettingValue(SettingsOptions::MinVoltageCells); // Should leave us a number of 0-9
    if (cellV > 9) {
      cellV = 9;
    }
    OLED::drawBattery(cellV + 1);
  } else {
    OLED::drawSymbol(15); // Draw the DC Logo
  }
#endif
}

#endif