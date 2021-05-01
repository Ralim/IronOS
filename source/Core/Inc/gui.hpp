/*
 * gui.h
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#ifndef GUI_HPP_
#define GUI_HPP_
#include "BSP.h"
#include "Settings.h"
#include "Translation.h"

#define PRESS_ACCEL_STEP         (TICKS_100MS / 3)
#define PRESS_ACCEL_INTERVAL_MIN TICKS_100MS
#define PRESS_ACCEL_INTERVAL_MAX (TICKS_100MS * 3)

// GUI holds the menu structure and all its methods for the menu itself

// Declarations for all the methods for the settings menu (at end of this file)

// Struct for holding the function pointers and descriptions
typedef struct {
  // The settings description index, please use the `SETTINGS_DESC` macro with
  // the `SettingsItemIndex` enum. Use 0 for no description.
  uint8_t description;
  // return true if increment reached the maximum value
  bool (*const incrementHandler)(void);
  bool (*const draw)(void);
} menuitem;

void                  enterSettingsMenu();
void                  GUIDelay();
void                  warnUser(const char *warning, const int timeout);
extern const menuitem rootSettingsMenu[];

/**
 * A helper class for showing a full-screen scrolling message.
 */
class ScrollMessage {
  uint32_t messageStart = 0;
  int16_t  lastOffset   = -1;

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
  bool drawUpdate(const char *message, uint32_t currentTick);
};

#endif /* GUI_HPP_ */
