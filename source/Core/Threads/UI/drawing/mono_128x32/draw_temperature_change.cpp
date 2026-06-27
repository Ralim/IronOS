#include "ui_drawing.hpp"

#ifdef OLED_128x32
void ui_draw_temperature_change(void) {
  // Which physical key is "up" depends on the screen rotation, so the +/- symbols must sit on the
  // side of the key that does each action. On a normal panel getRotation() carries that. Under
  // FRAMEBUFFER_ROTATION the shim rotates the whole frame and getRotation() is forced false, so read
  // the REAL orientation (getRawRotation); the T90 transpose polarity is inverted vs the MADCTL
  // convention this layout was written for, so negate it. (Unlike mirrored icons, +/- are plain
  // symbols, so choosing their side per orientation does not double-apply with the frame rotation.)
#ifdef FRAMEBUFFER_ROTATION
  const bool rot = !OLED::getRawRotation();
#else
  const bool rot = OLED::getRotation();
#endif

  OLED::setCursor(8, 8);
  if (rot) {
    OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolPlus : LargeSymbolMinus, FontStyle::LARGE);
  } else {
    OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolMinus : LargeSymbolPlus, FontStyle::LARGE);
  }

  OLED::print(LargeSymbolSpace, FontStyle::LARGE);
  OLED::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::LARGE);
  OLED::printSymbolDeg(FontStyle::EXTRAS);
  OLED::print(LargeSymbolSpace, FontStyle::LARGE);
  if (rot) {
    OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolMinus : LargeSymbolPlus, FontStyle::LARGE);
  } else {
    OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? LargeSymbolPlus : LargeSymbolMinus, FontStyle::LARGE);
  }
}
#endif