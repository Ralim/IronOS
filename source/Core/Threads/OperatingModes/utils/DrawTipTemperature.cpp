#include "OperatingModeUtilities.h"
#include "TipThermoModel.h"

void gui_drawTipTemp(bool symbol, const FontStyle font) {
  // Draw tip temp handling unit conversion & tolerance near setpoint
  uint32_t Temp = getSettingValue(SettingsOptions::TemperatureInF) ? TipThermoModel::getTipInF() : TipThermoModel::getTipInC();

  OLED::printNumber(Temp, 3, font); // Draw the tip temp out
  if (symbol) {
    // For big font, can draw nice symbols, otherwise fall back to chars
    OLED::printSymbolDeg(font == FontStyle::LARGE ? FontStyle::EXTRAS : font);
  }
}
