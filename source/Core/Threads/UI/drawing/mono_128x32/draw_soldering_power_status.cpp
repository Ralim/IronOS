#include "power.hpp"
#include "ui_drawing.hpp"
#include <OperatingModes.h>
#ifdef OLED_128x32

void ui_draw_soldering_power_status(bool boost_mode_on) {
  // One-line LARGE (12x24) tip temperature flush to one edge, vertically
  // centred; two SMALL (8x16) status rows (wattage, voltage) flush to the
  // other edge. The sleep countdown and power-source label are dropped to keep
  // two larger rows. Sides flip with rotation.
  (void)boost_mode_on;
  const bool    rot      = OLED::getRotation();
  const uint8_t statusW  = 5 * 8;  // "NN.NW" / "NN.NV" are 5 cells in the 8x16 small font
  const uint8_t tempW    = 4 * 12; // 3 digits + degree, fixed width in the 12x24 large font
  const uint8_t tempZone = 80;     // temperature lives right-aligned within the first 80px
  const int16_t tempX    = rot ? (OLED_WIDTH - tempZone) : (tempZone - tempW);
  const int16_t statusX  = rot ? 0 : (OLED_WIDTH - statusW);

  // Big temperature, vertically centred (4px above/below the 24px glyph)
  OLED::setCursor(tempX, 4);
  ui_draw_tip_temperature(true, FontStyle::LARGE);

  // Wattage (top row)
  OLED::setCursor(statusX, 0);
  {
    uint32_t x10Watt = x10WattHistory.average();
    if (x10Watt > 999) {
      // Above 99.9W drop the decimal place to keep it to 5 cells
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      OLED::printNumber(x10Watt / 10, 3, FontStyle::SMALL);
    } else {
      OLED::printNumber(x10Watt / 10, 2, FontStyle::SMALL);
      OLED::print(SmallSymbolDot, FontStyle::SMALL);
      OLED::printNumber(x10Watt % 10, 1, FontStyle::SMALL);
    }
    OLED::print(SmallSymbolWatts, FontStyle::SMALL);
  }

  // Input voltage (bottom row)
  OLED::setCursor(statusX, 16);
  printVoltage();
  OLED::print(SmallSymbolVolts, FontStyle::SMALL);
}
#endif
