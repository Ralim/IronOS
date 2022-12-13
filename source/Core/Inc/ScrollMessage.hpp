#ifndef SCROLL_MESSAGE_HPP_
#define SCROLL_MESSAGE_HPP_

#include "portmacro.h"
#include <stdint.h>
/**
 * A helper class for showing a full-screen scrolling message.
 */
class ScrollMessage {
  TickType_t messageStart = 0;
  int16_t    lastOffset   = -1;

  /**
   * Calcualte the width in pixels of the message string, in the large
   * font and taking into account multi-byte chars.
   *
   * @param message The null-terminated message string.
   */
  static uint16_t messageWidth(const char *message);

public:
  ScrollMessage() {}

  /**
   * Resets this `ScrollMessage` instance to its initial state.
   */
  void reset() {
    messageStart = 0;
    lastOffset   = -1;
  }

  /**
   * Gets whether this `ScrollMessage` instance is in its initial state.
   */
  bool isReset() const { return messageStart == 0; }

  /**
   * Draw and update the scroll message if needed.
   *
   * This function does not call `OLED::refresh()`. If this function
   * returns `true`, the caller shall call `OLED::refresh()` to draw the
   * modified framebuffer to the OLED screen.
   *
   * @param message The null-terminated message string. This must be the
   * same string as the previous call, unless this `ScrollMessage` instance
   * is in its initial state or `reset()` has been called.
   * @param currentTick The current tick as returned by `xTaskGetTickCount()`.
   * @return Whether the OLED framebuffer has been modified.
   */
  bool drawUpdate(const char *message, TickType_t currentTick);
};

#endif /* SCROLL_MESSAGE_HPP_ */
