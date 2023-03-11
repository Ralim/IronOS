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

#ifdef OLED_I2CBB
#include "I2CBB.hpp"
#define I2C_CLASS I2CBB
#else
#define I2C_CLASS FRToSI2C
#include "I2C_Wrapper.hpp"
#endif

#define DEVICEADDR_OLED   (0x3c << 1)
#define OLED_WIDTH        96
#define OLED_HEIGHT       16
#define FRAMEBUFFER_START 17

enum class FontStyle {
  SMALL,
  LARGE,
  EXTRAS,
};

class OLED {
public:
  enum DisplayState : bool { OFF = false, ON = true };

  static void initialize(); // Startup the I2C coms (brings screen out of reset etc)
  static bool isInitDone();
  // Draw the buffer out to the LCD if any content has changed.
  static void refresh() {
   
    if (checkDisplayBufferChecksum()) {
      const int len  = FRAMEBUFFER_START + (OLED_WIDTH * 2);
      I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, len);
      // DMA tx time is ~ 20mS Ensure after calling this you delay for at least 25ms
      // or we need to goto double buffering
    }
  }

  static void setDisplayState(DisplayState state) {
    if (state != displayState) {
      displayState    = state;
      screenBuffer[1] = (state == ON) ? 0xAF : 0xAE;
      // Dump the screen state change out _now_
      I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, FRAMEBUFFER_START - 1);
      osDelay(TICKS_10MS);
    }
  }

  static void setRotation(bool leftHanded); // Set the rotation for the screen
  // Get the current rotation of the LCD
  static bool getRotation() {
#ifdef OLED_FLIP
    return !inLeftHandedMode;
#else
    return inLeftHandedMode;
#endif
  }
  static void    setBrightness(uint8_t contrast);
  static void    setInverseDisplay(bool inverted);
  static int16_t getCursorX() { return cursor_x; }
  static void    print(const char *string, FontStyle fontStyle); // Draw a string to the current location, with selected font
  static void    printWholeScreen(const char *string);
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
  static void clearScreen() { memset(firstStripPtr, 0, OLED_WIDTH * 2); }
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
  static void transitionSecondaryFramebuffer(bool forwardNavigation);
  static void useSecondaryFramebuffer(bool useSecondary);
  static void transitionScrollDown();

private:
static bool checkDisplayBufferChecksum(){
   uint32_t  hash = 0;
    const int len  = FRAMEBUFFER_START + (OLED_WIDTH * 2);
    for (int i = 0; i < len; i++) {
      hash += (i * screenBuffer[i]);
    }

    bool result =  hash!=displayChecksum;
    displayChecksum= hash;
    return result;
}
  static void         drawChar(uint16_t charCode, FontStyle fontStyle); // Draw a character to the current cursor location
  static void         setFramebuffer(uint8_t *buffer);
  static uint8_t     *firstStripPtr;    // Pointers to the strips to allow for buffer having extra content
  static uint8_t     *secondStripPtr;   // Pointers to the strips
  static bool         inLeftHandedMode; // Whether the screen is in left or not (used for offsets in GRAM)
  static bool         initDone;
  static DisplayState displayState;
  static int16_t      cursor_x, cursor_y;
  static uint8_t      displayOffset;
  static uint32_t     displayChecksum;
  static uint8_t      screenBuffer[16 + (OLED_WIDTH * 2) + 10]; // The data buffer
  static uint8_t      secondFrameBuffer[OLED_WIDTH * 2];
};

#endif /* OLED_HPP_ */
