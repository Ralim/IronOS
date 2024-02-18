#include "ui_drawing.hpp"

void ui_draw_cjc_sampling(const uint8_t num_dots) {
  OLED::setCursor(0, 0);
  OLED::print(translatedString(Tr->CJCCalibrating), FontStyle::SMALL);
  OLED::setCursor(0, 8);
  OLED::print(SmallSymbolDot, FontStyle::SMALL);
  for (uint8_t x = 0; x < num_dots; x++) {
    OLED::print(SmallSymbolDot, FontStyle::SMALL);
  }
}