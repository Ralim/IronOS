#ifndef SCROLL_MESSAGE_HPP_
#define SCROLL_MESSAGE_HPP_

#include "portmacro.h"
#include <stdint.h>
/**
 * A helper for showing a full-screen scrolling message.
 */

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
 * @param currentTick The current tick as returned by `xTaskGetTickCount()` offset to 0 at start of scrolling.
 */
void drawScrollingText(const char *message, TickType_t currentTickOffset);

#endif /* SCROLL_MESSAGE_HPP_ */
