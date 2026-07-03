#include "ui_drawing.hpp"
#ifdef OLED_128x32

extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

void ui_draw_homescreen_detailed(TemperatureType_t tipTemp) {
  if (isTipDisconnected()) {
    if (OLED::getRotation()) {
      // in right handed mode we want to draw over the first part
      OLED::drawArea(54, 0, 56, 32, disconnectedTipF);
    } else {
      OLED::drawArea(0, 0, 56, 32, disconnectedTip);
    }
    if (OLED::getRotation()) {
      OLED::setCursor(-1, 0);
    } else {
      OLED::setCursor(56, 0);
    }
    uint32_t Vlt = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    OLED::printNumber(Vlt / 10, 2, FontStyle::LARGE);
    OLED::print(LargeSymbolDot, FontStyle::LARGE);
    OLED::printNumber(Vlt % 10, 1, FontStyle::LARGE);
    if (OLED::getRotation()) {
      OLED::setCursor(48, 8);
    } else {
      OLED::setCursor(91, 8);
    }
    OLED::print(SmallSymbolVolts, FontStyle::SMALL);
  } else {
    // One-line LARGE (12x24) tip temperature flush to one edge, vertically
    // centred; two SMALL (8x16) status rows (set-temp, voltage) flush to the
    // other edge. Sides flip with rotation.
    const bool    rot      = OLED::getRotation();
    const uint8_t statusW  = 5 * 8;              // "NNN°C" / "NN.NV" are 5 cells in the 8x16 small font
    const uint8_t tempW    = (3 * 12) + (2 * 8); // 3 large digits + small "°C" = 52px
    const uint8_t tempZone = 68;                 // temperature right-aligned within this zone (left of the status block)
    const int16_t tempX    = rot ? (OLED_WIDTH - tempZone) : (tempZone - tempW);
    const int16_t statusX  = rot ? 0 : (OLED_WIDTH - statusW);

    if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (tipTemp > 55) && (xTaskGetTickCount() % 1000 < 300))) {
      // Blink temp if setting enable and temp < 55° (OFF 300ms / ON 700ms)
      OLED::setCursor(tempX, 4); // 24px number, vertically centred (4px above/below)
      ui_draw_tip_temperature(false, FontStyle::LARGE);
      // Degree + unit in the small font, bottom-aligned with the 24px number (like the TS100)
      OLED::setCursor(OLED::getCursorX(), 12);
      OLED::printSymbolDeg(FontStyle::SMALL);
    }

    // Set temperature (top row)
    OLED::setCursor(statusX, 0);
    OLED::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::SMALL);
    OLED::printSymbolDeg(FontStyle::SMALL);

    // Input voltage (bottom row)
    OLED::setCursor(statusX, 16);
    printVoltage(); // draw voltage then symbol (v)
    OLED::print(SmallSymbolVolts, FontStyle::SMALL);
  }
}
#endif