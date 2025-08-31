#include "ScrollMessage.hpp"
#include "OLED.hpp"
#include "Settings.h"
#include "configuration.h"

/**
 * Counts the number of chars in the string excluding the null terminator.
 * This is a custom version of `strlen` which takes into account our custom
 * double-byte char encoding.
 * @param str The input string.
 * @return The length of the string.
 */
static uint16_t str_display_len(const char *const str) {
  const uint8_t *next  = reinterpret_cast<const uint8_t *>(str);
  uint16_t       count = 0;
  while (next[0]) {
    if (next[0] <= 0xF0) {
      count++;
      next++;
    } else {
      if (!next[1]) {
        break;
      }
      count++;
      next += 2;
    }
  }
  return count;
}

/**
 * Calculate the width in pixels of the message string, in the large
 * font and taking into account multi-byte chars.
 *
 * @param message The null-terminated message string.
 */
uint16_t messageWidth(const char *message) { return FONT_12_WIDTH * str_display_len(message); }

void drawScrollingText(const char *message, TickType_t currentTickOffset) {
  OLED::clearScreen();
  int16_t  messageOffset;
  uint16_t msgWidth = messageWidth(message);
  if (msgWidth > OLED_WIDTH) {
    messageOffset = (currentTickOffset / (getSettingValue(SettingsOptions::DescriptionScrollSpeed) == 1 ? TICKS_100MS / 10 : (TICKS_100MS / 5)));
    messageOffset %= msgWidth + OLED_WIDTH; // Roll around at the end
    if (messageOffset < OLED_WIDTH) {
      // Snap the message to the left edge.
      messageOffset = OLED_WIDTH;
    } else if (messageOffset > msgWidth) {
      // Snap the message to the right edge.
      messageOffset = msgWidth;
    }
  } else {
    // Centre the message without scrolling.
    messageOffset = (OLED_WIDTH - msgWidth) / 2 + msgWidth;
  }

  //^ Rolling offset based on time
  OLED::setCursor((OLED_WIDTH - messageOffset), 0);
  OLED::print(message, FontStyle::LARGE);
}
