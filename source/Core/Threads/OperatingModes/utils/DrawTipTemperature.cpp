#include "OperatingModeUtilities.h"
#include "TipThermoModel.h"

void gui_drawTipTemp(bool symbol, const FontStyle font) {
  // Draw tip temp handling unit conversion & tolerance near setpoint
  uint32_t Temp = 0;
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    Temp = TipThermoModel::getTipInF();
  } else {
    Temp = TipThermoModel::getTipInC();
  }

  OLED::printNumber(Temp, 3, font); // Draw the tip temp out
  if (symbol) {
    if (font == FontStyle::LARGE) {
      // Big font, can draw nice symbols
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::drawSymbol(0);
      else
        OLED::drawSymbol(1);
    } else {
      // Otherwise fall back to chars
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::print(SmallSymbolDegF, FontStyle::SMALL);
      else
        OLED::print(SmallSymbolDegC, FontStyle::SMALL);
    }
  }
}