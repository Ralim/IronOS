#include "ui_drawing.hpp"

extern uint8_t buttonAF[sizeof(buttonA)];
extern uint8_t buttonBF[sizeof(buttonB)];
extern uint8_t disconnectedTipF[sizeof(disconnectedTip)];

void ui_draw_homescreen_simplified(TemperatureType_t tipTemp) {
  bool tempOnDisplay          = false;
  bool tipDisconnectedDisplay = false;
  if (OLED::getRotation()) {
    OLED::drawArea(54, 0, 42, 16, buttonAF);
    OLED::drawArea(12, 0, 42, 16, buttonBF);
    OLED::setCursor(0, 0);
    ui_draw_power_source_icon();
  } else {
    OLED::drawArea(0, 0, 42, 16, buttonA);  // Needs to be flipped so button ends up
    OLED::drawArea(42, 0, 42, 16, buttonB); // on right side of screen
    OLED::setCursor(84, 0);
    ui_draw_power_source_icon();
  }
  tipDisconnectedDisplay = false;
  if (tipTemp > 55) {
    tempOnDisplay = true;
  } else if (tipTemp < 45) {
    tempOnDisplay = false;
  }
  if (isTipDisconnected()) {
    tempOnDisplay          = false;
    tipDisconnectedDisplay = true;
  }
  if (tempOnDisplay || tipDisconnectedDisplay) {
    // draw temp over the start soldering button
    // Location changes on screen rotation
    if (OLED::getRotation()) {
      // in right handed mode we want to draw over the first part
      OLED::fillArea(55, 0, 41, 16, 0); // clear the area for the temp
      OLED::setCursor(56, 0);
    } else {
      OLED::fillArea(0, 0, 41, 16, 0); // clear the area
      OLED::setCursor(0, 0);
    }
    // If we have a tip connected draw the temp, if not we leave it blank
    if (!tipDisconnectedDisplay) {
      // draw in the temp
      if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (xTaskGetTickCount() % 1000 < 300))) {
        ui_draw_tip_temperature(false, FontStyle::LARGE); // draw in the temp
      }
    } else {
      // Draw in missing tip symbol

      if (OLED::getRotation()) {
        // in right handed mode we want to draw over the first part
        OLED::drawArea(54, 0, 42, 16, disconnectedTipF);
      } else {
        OLED::drawArea(0, 0, 42, 16, disconnectedTip);
      }
    }
  }
}
