#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
#include "SolderingCommon.h"
#include "TipThermoModel.h"

void ui_draw_tip_temperature(bool symbol, const FontStyle font) {
  // Draw tip temp handling unit conversion & tolerance near setpoint
  TemperatureType_t Temp = getTipTemp();

  OLED::printNumber(Temp, 3, font); // Draw the tip temp out
  if (symbol) {
    // For big font, can draw nice symbols, otherwise fall back to chars
    OLED::printSymbolDeg(font == FontStyle::LARGE ? FontStyle::EXTRAS : font);
  }
}
