#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
#include "SolderingCommon.h"
#include "TipThermoModel.h"
#ifdef OLED_128x32

void ui_draw_tip_temperature(bool symbol, const FontStyle font) {
  // Draw tip temp handling unit conversion & tolerance near setpoint
  TemperatureType_t Temp = getTipTemp();

  OLED::printNumber(Temp, 3, font); // Draw the tip temp out
  if (symbol) {
    // For big font, can draw nice symbols, otherwise fall back to chars
    OLED::printSymbolDeg(font == FontStyle::LARGE ? FontStyle::EXTRAS : font);
  }
}

static uint8_t tip_temp_digits() {
  TemperatureType_t Temp   = getTipTemp();
  uint16_t          v      = Temp < 0 ? 0 : (uint16_t)Temp;
  uint8_t           digits = 1;
  for (uint16_t n = v; n >= 10; n /= 10) {
    digits++;
  }
  return digits;
}

// Tip temperature for the detailed readouts: left-aligned with exactly its digit
// count (no leading blanks) in the LARGE (12x24) font, so it can be flushed to
// an edge. The degree symbol is the matching large glyph.
void ui_draw_tip_temperature_readout(bool symbol) {
  OLED::printNumber(getTipTemp(), tip_temp_digits(), FontStyle::LARGE, false);
  if (symbol) {
    OLED::printSymbolDeg(FontStyle::LARGE);
  }
}

// Pixel width the above occupies (LARGE cells are 12px wide on 128x32).
uint8_t ui_tip_temperature_readout_width(bool symbol) { return (tip_temp_digits() + (symbol ? 2 : 0)) * 12; }
#endif