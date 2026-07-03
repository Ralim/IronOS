#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
#ifdef OLED_128x32
bool warnUser(const char *warning, const ButtonState buttons) {
  OLED::clearScreen();
  // Centre the warning on the panel instead of top-left aligning it.
  const uint8_t *text = reinterpret_cast<const uint8_t *>(warning);
  FontStyle      font = FontStyle::SMALL;
  if (text[0] == 0x01) { // a leading newline selects the large font (e.g. single-line warnings)
    font = FontStyle::LARGE;
    text++;
  }
  const uint8_t charWidth  = (font == FontStyle::LARGE) ? 12 : 8;
  const uint8_t lineHeight = (font == FontStyle::LARGE) ? 24 : 16;
  // Measure the widest line and the line count (lines are split on the 0x01 marker).
  uint16_t chars = 0, widest = 0, lines = 1;
  for (const uint8_t *p = text; p[0];) {
    if (p[0] == 0x01) {
      widest = (chars > widest) ? chars : widest;
      chars  = 0;
      lines++;
      p++;
    } else if (p[0] <= 0xF0) {
      chars++;
      p++;
    } else {
      if (!p[1]) {
        break;
      }
      chars++;
      p += 2;
    }
  }
  widest    = (chars > widest) ? chars : widest;
  int16_t x = (OLED_WIDTH - static_cast<int16_t>(widest * charWidth)) / 2;
  int16_t y = (OLED_HEIGHT - static_cast<int16_t>(lines * lineHeight)) / 2;
  if (x < 0) {
    x = 0;
  }
  if (y < 0) {
    y = 0;
  }
  OLED::setCursor(x, y);
  OLED::print(reinterpret_cast<const char *>(text), font, 255, x); // soft_x_limit=x wraps a second line under the first
  // Also timeout after 5 seconds
  if ((xTaskGetTickCount() - lastButtonTime) > TICKS_SECOND * 5) {
    return true;
  }
  return buttons != BUTTON_NONE;
}
#endif