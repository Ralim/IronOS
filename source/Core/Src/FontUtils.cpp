#include "FontUtils.h"
constexpr uint8_t get_fontstyle_height(const FontStyle font) {
  if (font == FontStyle::SMALL)
    return 8;
  return 16;
}

constexpr uint8_t get_fontstyle_width(const FontStyle font) {
  if (font == FontStyle::SMALL)
    return 6;
  return 12;
}

constexpr FontStyle get_fontstyle_fromHeight(const uint8_t height) { return (height <= 8) ? FontStyle::SMALL : FontStyle::LARGE; }