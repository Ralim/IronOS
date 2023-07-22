/*
 * OLED.hpp
 *
 *  Created on: 20Jan.,2017
 *      Author: Ben V. Brown <Ralim>
 *      Designed for the SSD1307
 *      Cleared for release for TS100 2017/08/20
 */

#ifndef OLED_HPP_
#define OLED_HPP_
#include "Font.h"
#include "cmsis_os.h"
#include "configuration.h"
#include <BSP.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "FreeRTOS.h"
#ifdef __cplusplus
}
#endif

#if defined(OLED_I2CBB2)
#include "I2CBB2.hpp"
#define I2C_CLASS I2CBB2
#elif defined(OLED_I2CBB1)
#include "I2CBB1.hpp"
#define I2C_CLASS I2CBB1
#else
#define I2C_CLASS FRToSI2C
#include "I2C_Wrapper.hpp"
#endif

#define DEVICEADDR_OLED (0x3c << 1)

#ifdef OLED_128x32

#define OLED_WIDTH           128
#define OLED_HEIGHT          32
#define OLED_GRAM_START      0x00 // Should be 0x00 when we have full width
#define OLED_GRAM_END        0x7F // Should be 0x7F when we have full width
#define OLED_GRAM_START_FLIP 0
#define OLED_GRAM_END_FLIP   0x7F

#define OLED_VCOM_LAYOUT 0x12
#define OLED_SEGMENT_MAP_REVERSED
#define OLED_DIVIDER 0xD3

#else

#define OLED_WIDTH           96
#define OLED_HEIGHT          16
#define OLED_GRAM_START      0x20
#define OLED_GRAM_END        0x7F
#define OLED_GRAM_START_FLIP 0
#define OLED_GRAM_END_FLIP   95

#define OLED_VCOM_LAYOUT 0x02
#define OLED_SEGMENT_MAP 0xA0
#define OLED_DIVIDER     0xD5

#endif /* OLED_128x32 */

#define OLED_ON  0xAF
#define OLED_OFF 0xAE

#define FRAMEBUFFER_START 17

enum class FontStyle {
  SMALL,
  LARGE,
  EXTRAS,
};

class OLED {
public:
  enum DisplayState : bool {
    OFF = false,
    ON  = true
  };

  static void initialize(); // Startup the I2C coms (brings screen out of reset etc)
  static bool isInitDone();
  // Draw the buffer out to the LCD if any content has changed.
  static void refresh() {

    if (checkDisplayBufferChecksum()) {
      const int len = FRAMEBUFFER_START + (OLED_WIDTH * (OLED_HEIGHT / 8));
      I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, len);
      // DMA tx time is ~ 20mS Ensure after calling this you delay for at least 25ms
      // or we need to goto double buffering
    }
  }

  static void setDisplayState(DisplayState state) {
    if (state != displayState) {
      displayState    = state;
      screenBuffer[1] = (state == ON) ? OLED_ON : OLED_OFF;
      // Dump the screen state change out _now_
      I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, FRAMEBUFFER_START - 1);
      osDelay(TICKS_10MS);
    }
  }

  // Set the rotation for the screen
  static void setRotation(bool leftHanded);
  // Get the current rotation of the LCD
  static bool getRotation() {
#ifdef OLED_FLIP
    return !inLeftHandedMode;
#else
    return inLeftHandedMode;
#endif /* OLED_FLIP */
  }
  static void    setBrightness(uint8_t contrast);
  static void    setInverseDisplay(bool inverted);
  static int16_t getCursorX() { return cursor_x; }
  // Draw a string to the current location, with selected font; optionally - with MAX length only
  static void print(const char *string, FontStyle fontStyle, uint8_t length = 255);
  static void printWholeScreen(const char *string);
  // Print *F or *C - in font style of Small, Large (by default) or Extra based on input arg
  static void printSymbolDeg(FontStyle fontStyle = FontStyle::LARGE);
  // Set the cursor location by pixels
  static void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }
  // Draws an image to the buffer, at x offset from top to bottom (fixed height renders)
  static void drawImage(const uint8_t *buffer, uint8_t x, uint8_t width) { drawArea(x, 0, width, 16, buffer); }
  // Draws a number at the current cursor location
  static void printNumber(uint16_t number, uint8_t places, FontStyle fontStyle, bool noLeaderZeros = true);
  // Clears the buffer
  static void clearScreen() { memset(stripPointers[0], 0, OLED_WIDTH * (OLED_HEIGHT / 8)); }
  // Draws the battery level symbol
  static void drawBattery(uint8_t state) { drawSymbol(3 + (state > 10 ? 10 : state)); }
  // Draws a checkbox
  static void drawCheckbox(bool state) { drawSymbol((state) ? 16 : 17); }
  static void debugNumber(int32_t val, FontStyle fontStyle);
  static void drawHex(uint32_t x, FontStyle fontStyle, uint8_t digits);
  static void drawSymbol(uint8_t symbolID);                                                           // Used for drawing symbols of a predictable width
  static void drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr);        // Draw an area, but y must be aligned on 0/8 offset
  static void drawAreaSwapped(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr); // Draw an area, but y must be aligned on 0/8 offset
  static void fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value);       // Fill an area, but y must be aligned on 0/8 offset
  static void drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool clear);
  static void drawHeatSymbol(uint8_t state);
  static void drawScrollIndicator(uint8_t p, uint8_t h); // Draws a scrolling position indicator
  static void maskScrollIndicatorOnOLED();
  static void transitionSecondaryFramebuffer(const bool forwardNavigation, const TickType_t viewEnterTime);
  static void useSecondaryFramebuffer(bool useSecondary);
  static void transitionScrollDown(const TickType_t viewEnterTime);
  static void transitionScrollUp(const TickType_t viewEnterTime);

private:
  static bool checkDisplayBufferChecksum() {
    uint32_t  hash = 0;
    const int len  = sizeof(screenBuffer);
    for (int i = 0; i < len; i++) {
      hash += (i * screenBuffer[i]);
    }

    bool result     = hash != displayChecksum;
    displayChecksum = hash;
    return result;
  }
  static void         drawChar(uint16_t charCode, FontStyle fontStyle); // Draw a character to the current cursor location
  static void         setFramebuffer(uint8_t *buffer);
  static uint8_t     *stripPointers[4]; // Pointers to the strips to allow for buffer having extra content
  static bool         inLeftHandedMode; // Whether the screen is in left or not (used for offsets in GRAM)
  static bool         initDone;
  static DisplayState displayState;
  static int16_t      cursor_x, cursor_y;
  static uint8_t      displayOffset;
  static uint32_t     displayChecksum;
  static uint8_t      screenBuffer[16 + (OLED_WIDTH * (OLED_HEIGHT / 8)) + 10]; // The data buffer
  static uint8_t      secondFrameBuffer[16 + OLED_WIDTH * (OLED_HEIGHT / 8) + 10];
};

#endif /* OLED_HPP_ */
