#include "power.hpp"
#include "ui_drawing.hpp"
#ifdef OLED_128x32

void ui_draw_soldering_basic_status(bool boostModeOn) {
  OLED::setCursor(0, 0);
  // We switch the layout direction depending on the orientation of the oled
  if (OLED::getRotation()) {
    // battery
    ui_draw_power_source_icon();
    // Space out gap between battery <-> temp
    OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    // Draw current tip temp (y=4 centres the 24px number; +12 nudges it one digit right; restore after)
    OLED::setCursor(OLED::getCursorX() + 12, 4);
    ui_draw_tip_temperature(true, FontStyle::LARGE);
    OLED::setCursor(OLED::getCursorX(), 0);

    // We draw boost arrow if boosting,
    // or else gap temp <-> heat indicator
    if (boostModeOn) {
      OLED::drawSymbol(2);
    } else {
      OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    }

    // Draw heating/cooling symbols
    OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
  } else {
    // Draw heating/cooling symbols
    OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
    // We draw boost arrow if boosting,
    // or else gap temp <-> heat indicator
    if (boostModeOn) {
      OLED::drawSymbol(2);
    } else {
      OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    }
    // Draw current tip temp (y=4 centres the 24px number; +12 nudges it one digit right; restore after)
    OLED::setCursor(OLED::getCursorX() + 12, 4);
    ui_draw_tip_temperature(true, FontStyle::LARGE);
    OLED::setCursor(OLED::getCursorX(), 0);
    // Power source icon near the right edge (matches the simplified idle screen)
    OLED::setCursor(116, 0);
    ui_draw_power_source_icon();
  }
}

#endif