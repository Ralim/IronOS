#pragma once
#include <stdint.h>
enum class FontStyle {
  SMALL,
  LARGE,
  EXTRAS,
  FROM_TEXT,   // Magically pick from the text
  FROM_HEIGHT, // Pick font to best fill the height
};

constexpr uint8_t   get_fontstyle_height(const FontStyle font);
constexpr uint8_t   get_fontstyle_width(const FontStyle font);
constexpr FontStyle get_fontstyle_fromHeight(const uint8_t height);