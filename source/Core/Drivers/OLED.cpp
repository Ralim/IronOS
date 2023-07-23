/*
 * OLED.cpp
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#include "Buttons.hpp"
#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include <OLED.hpp>
#include <stdlib.h>
#include <string.h>

// rendering to the buffer
uint8_t *OLED::stripPointers[4]; // Pointers to the strips to allow for buffer having extra content

bool OLED::inLeftHandedMode; // Whether the screen is in left or not (used for
// offsets in GRAM)
OLED::DisplayState OLED::displayState;
int16_t            OLED::cursor_x, OLED::cursor_y;
bool               OLED::initDone = false;
uint8_t            OLED::displayOffset;
uint8_t            OLED::screenBuffer[16 + (OLED_WIDTH * (OLED_HEIGHT / 8)) + 10]; // The data buffer
uint8_t            OLED::secondFrameBuffer[16 + (OLED_WIDTH * (OLED_HEIGHT / 8)) + 10];
uint32_t           OLED::displayChecksum;
/*
 * Setup params for the OLED screen
 * http://www.displayfuture.com/Display/datasheet/controller/SSD1307.pdf
 * All commands are prefixed with 0x80
 * Data packets are prefixed with 0x40
 */
I2C_CLASS::I2C_REG OLED_Setup_Array[] = {
  /**/
    {0x80,         OLED_OFF, 0}, /* Display off */
    {0x80,     OLED_DIVIDER, 0}, /* Set display clock divide ratio / osc freq */
    {0x80,             0x52, 0}, /* Divide ratios */
    {0x80,             0xA8, 0}, /* Set Multiplex Ratio */
    {0x80,  OLED_HEIGHT - 1, 0}, /* Multiplex ratio adjusts how far down the matrix it scans */
    {0x80,             0xC0, 0}, /* Set COM Scan direction */
    {0x80,             0xD3, 0}, /* Set vertical Display offset */
    {0x80,             0x00, 0}, /* 0 Offset */
    {0x80,             0x40, 0}, /* Set Display start line to 0 */
#ifdef OLED_SEGMENT_MAP_REVERSED
    {0x80,             0xA1, 0}, /* Set Segment remap to normal */
#else
    {0x80, 0xA0, 0}, /* Set Segment remap to normal */
#endif
    {0x80,             0x8D, 0}, /* Charge Pump */
    {0x80,             0x14, 0}, /* Charge Pump settings */
    {0x80,             0xDA, 0}, /* Set VCOM Pins hardware config */
    {0x80, OLED_VCOM_LAYOUT, 0}, /* Combination 0x2 or 0x12 depending on OLED model */
    {0x80,             0x81, 0}, /* Brightness */
    {0x80,             0x00, 0}, /* ^0 */
    {0x80,             0xD9, 0}, /* Set pre-charge period */
    {0x80,             0xF1, 0}, /* Pre charge period */
    {0x80,             0xDB, 0}, /* Adjust VCOMH regulator ouput */
    {0x80,             0x30, 0}, /* VCOM level */
    {0x80,             0xA4, 0}, /* Enable the display GDDR */
    {0x80,             0xA6, 0}, /* Normal display */
    {0x80,             0x20, 0}, /* Memory Mode */
    {0x80,             0x00, 0}, /* Wrap memory */
    {0x80,          OLED_ON, 0}, /* Display on */
};
// Setup based on the SSD1307 and modified for the SSD1306

const uint8_t REFRESH_COMMANDS[17] = {
    // Set display ON:
    0x80,
    0xAF, // cmd

    // Set column address:
    //  A[6:0] - Column start address = 0x20
    //  B[6:0] - Column end address = 0x7F
    0x80,
    0x21, // cmd
    0x80,
    OLED_GRAM_START, // A
    0x80,
    OLED_GRAM_END, // B

    // Set COM output scan direction (normal mode, COM0 to COM[N-1])
    0x80,
    0xC0,

    // Set page address:
    //  A[2:0] - Page start address = 0
    //  B[2:0] - Page end address = 1
    0x80,
    0x22, // cmd
    0x80,
    0x00, // A
    0x80,
    (OLED_HEIGHT / 8) - 1, // B

    // Start of data
    0x40,
};

/*
 * Animation timing function that follows a bezier curve.
 * @param t A given percentage value [0..<100]
 * Returns a new percentage value with ease in and ease out.
 * Original floating point formula: t * t * (3.0f - 2.0f * t);
 */
static uint16_t easeInOutTiming(uint16_t t) { return t * t * (300 - 2 * t) / 10000; }

/*
 * Returns the value between a and b, using a percentage value t.
 * @param a The value associated with 0%
 * @param b The value associated with 100%
 * @param t The percentage [0..<100]
 */
static uint16_t lerp(uint16_t a, uint16_t b, uint16_t t) { return a + t * (b - a) / 100; }

void OLED::initialize() {
  cursor_x = cursor_y = 0;
  inLeftHandedMode    = false;

#ifdef OLED_128x32
  stripPointers[0] = &screenBuffer[FRAMEBUFFER_START];
  stripPointers[1] = &screenBuffer[FRAMEBUFFER_START + OLED_WIDTH];
  stripPointers[2] = &screenBuffer[FRAMEBUFFER_START + 2 * OLED_WIDTH];
  stripPointers[3] = &screenBuffer[FRAMEBUFFER_START + 3 * OLED_WIDTH];

#else
  stripPointers[0] = &screenBuffer[FRAMEBUFFER_START];
  stripPointers[1] = &screenBuffer[FRAMEBUFFER_START + OLED_WIDTH];

#endif /* OLED_128x32 */
  displayOffset = 0;
  memcpy(&screenBuffer[0], &REFRESH_COMMANDS[0], sizeof(REFRESH_COMMANDS));
  memcpy(&secondFrameBuffer[0], &REFRESH_COMMANDS[0], sizeof(REFRESH_COMMANDS));

  // Set the display to be ON once the settings block is sent and send the
  // initialisation data to the OLED.

  for (int tries = 0; tries < 10; tries++) {
    if (I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, OLED_Setup_Array, sizeof(OLED_Setup_Array) / sizeof(OLED_Setup_Array[0]))) {
      tries = 11;
    }
  }
  setDisplayState(DisplayState::ON);
  initDone = true;
}

void OLED::setFramebuffer(uint8_t *buffer) {
  stripPointers[0] = &buffer[FRAMEBUFFER_START];
  stripPointers[1] = &buffer[FRAMEBUFFER_START + OLED_WIDTH];

#ifdef OLED_128x32
  stripPointers[2] = &buffer[FRAMEBUFFER_START + (2 * OLED_WIDTH)];
  stripPointers[3] = &buffer[FRAMEBUFFER_START + (3 * OLED_WIDTH)];
#endif /* OLED_128x32 */
}

/*
 * Prints a char to the screen.
 * UTF font handling is done using the two input chars.
 * Precursor is the command char that is used to select the table.
 */
void OLED::drawChar(const uint16_t charCode, const FontStyle fontStyle) {
  const uint8_t *currentFont;
  static uint8_t fontWidth, fontHeight;
  uint16_t       index;
  switch (fontStyle) {
  case FontStyle::EXTRAS:
    currentFont = ExtraFontChars;
    index       = charCode;
    fontHeight  = 16;
    fontWidth   = 12;
    break;
  case FontStyle::SMALL:
  case FontStyle::LARGE:
  default:
    if (charCode == '\x01' && cursor_y == 0) { // 0x01 is used as new line char
      setCursor(0, 8);
      return;
    } else if (charCode <= 0x01) {
      return;
    }
    currentFont = nullptr;
    index       = 0;
    switch (fontStyle) {
    case FontStyle::SMALL:
      fontHeight = 8;
      fontWidth  = 6;
      break;
    case FontStyle::LARGE:
    default:
      fontHeight = 16;
      fontWidth  = 12;
      break;
    }

    currentFont = fontStyle == FontStyle::SMALL ? FontSectionInfo.font06_start_ptr : FontSectionInfo.font12_start_ptr;
    index       = charCode - 2;
    break;
  }
  const uint8_t *charPointer = currentFont + ((fontWidth * (fontHeight / 8)) * index);
  drawArea(cursor_x, cursor_y, fontWidth, fontHeight, charPointer);
  cursor_x += fontWidth;
}

/*
 * Draws a one pixel wide scrolling indicator. y is the upper vertical position
 * of the indicator in pixels (0..<16).
 */
void OLED::drawScrollIndicator(uint8_t y, uint8_t height) {
  union u_type {
    uint16_t whole;
    uint8_t  strips[2];
  } column;

  column.whole = (1 << height) - 1;
  column.whole <<= y;

  // Draw a one pixel wide bar to the left with a single pixel as
  // the scroll indicator.
  fillArea(OLED_WIDTH - 1, 0, 1, 8, column.strips[0]);
  fillArea(OLED_WIDTH - 1, 8, 1, 8, column.strips[1]);
}

/**
 * Masks (removes) the scrolling indicator, i.e. clears the rightmost column
 * on the screen. This operates directly on the OLED graphics RAM, as this
 * is intended to be used before calling `OLED::transitionScrollDown()`.
 */
void OLED::maskScrollIndicatorOnOLED() {
  // The right-most column depends on the screen rotation, so just take
  // it from the screen buffer which is updated by `OLED::setRotation`.
  uint8_t rightmostColumn = screenBuffer[7];
  uint8_t maskCommands[]  = {
      // Set column address:
      //  A[6:0] - Column start address = rightmost column
      //  B[6:0] - Column end address = rightmost column
      0x80,
      0x21, // cmd
      0x80,
      rightmostColumn, // A
      0x80,
      rightmostColumn, // B

      // Start of data
      0x40,
#ifdef OLED_128x32
      0x00,
      0x00,
#endif /* OLED_128x32 */
      // Clears two 8px strips
      0x00,
      0x00,
  };
  I2C_CLASS::Transmit(DEVICEADDR_OLED, maskCommands, sizeof(maskCommands));
}

/**
 * Plays a transition animation between two framebuffers.
 * @param forwardNavigation Direction of the navigation animation.
 *
 * If forward is true, this displays a forward navigation to the second framebuffer contents.
 * Otherwise a rewinding navigation animation is shown to the second framebuffer contents.
 */
void OLED::transitionSecondaryFramebuffer(const bool forwardNavigation, const TickType_t viewEnterTime) {
  bool     buttonsReleased = getButtonState() == BUTTON_NONE;
  uint8_t *stripBackPointers[4];
  stripBackPointers[0] = &secondFrameBuffer[FRAMEBUFFER_START + 0];
  stripBackPointers[1] = &secondFrameBuffer[FRAMEBUFFER_START + OLED_WIDTH];

#ifdef OLED_128x32
  stripBackPointers[2] = &secondFrameBuffer[OLED_WIDTH * 2];
  stripBackPointers[3] = &secondFrameBuffer[OLED_WIDTH * 3];
#endif /* OLED_128x32 */

  TickType_t totalDuration = TICKS_100MS * 5; // 500ms
  TickType_t duration      = 0;
  TickType_t start         = xTaskGetTickCount();
  uint8_t    offset        = 0;
  TickType_t startDraw     = xTaskGetTickCount();
  while (duration <= totalDuration) {
    duration          = xTaskGetTickCount() - start;
    uint16_t progress = ((duration * 100) / totalDuration); // Percentage of the period we are through for animation
    progress          = easeInOutTiming(progress);
    progress          = lerp(0, OLED_WIDTH, progress);
    // Constrain
    if (progress > OLED_WIDTH) {
      progress = OLED_WIDTH;
    }

    // When forward, current contents move to the left out.
    // Otherwise the contents move to the right out.
    uint8_t oldStart    = forwardNavigation ? 0 : progress;
    uint8_t oldPrevious = forwardNavigation ? progress - offset : offset;

    // Content from the second framebuffer moves in from the right (forward)
    // or from the left (not forward).
    uint8_t newStart = forwardNavigation ? OLED_WIDTH - progress : 0;
    uint8_t newEnd   = forwardNavigation ? 0 : OLED_WIDTH - progress;

    offset = progress;

    memmove(&stripPointers[0][oldStart], &stripPointers[0][oldPrevious], OLED_WIDTH - progress);
    memmove(&stripPointers[1][oldStart], &stripPointers[1][oldPrevious], OLED_WIDTH - progress);

#ifdef OLED_128x32
    memmove(&stripPointers[2][oldStart], &stripPointers[2][oldPrevious], OLED_WIDTH - progress);
    memmove(&stripPointers[3][oldStart], &stripPointers[3][oldPrevious], OLED_WIDTH - progress);
#endif /* OLED_128x32 */

    memmove(&stripPointers[0][newStart], &stripBackPointers[0][newEnd], progress);
    memmove(&stripPointers[1][newStart], &stripBackPointers[1][newEnd], progress);

#ifdef OLED_128x32
    memmove(&stripPointers[2][newStart], &stripBackPointers[2][newEnd], progress);
    memmove(&stripPointers[3][newStart], &stripBackPointers[3][newEnd], progress);
#endif /* OLED_128x32 */

    refresh(); // Now refresh to write out the contents to the new page
    vTaskDelayUntil(&startDraw, TICKS_100MS / 7);
    buttonsReleased |= getButtonState() == BUTTON_NONE;
    if (getButtonState() != BUTTON_NONE && buttonsReleased) {
      memcpy(screenBuffer + FRAMEBUFFER_START, secondFrameBuffer + FRAMEBUFFER_START, sizeof(screenBuffer) - FRAMEBUFFER_START);
      refresh(); // Now refresh to write out the contents to the new page
      return;
    }
  }
  refresh(); //
}

void OLED::useSecondaryFramebuffer(bool useSecondary) {
  if (useSecondary) {
    setFramebuffer(secondFrameBuffer);
  } else {
    setFramebuffer(screenBuffer);
  }
}

/**
 * This assumes that the current display output buffer has the current on screen contents
 * Then the secondary buffer has the "new" contents to be slid up onto the screen
 * Sadly we cant use the hardware scroll as some devices with the 128x32 screens dont have the GRAM for holding both screens at once
 *
 * **This function blocks until the transition has completed or user presses button**
 */
void OLED::transitionScrollDown(const TickType_t viewEnterTime) {
  TickType_t startDraw       = xTaskGetTickCount();
  bool       buttonsReleased = getButtonState() == BUTTON_NONE;

  for (uint8_t heightPos = 0; heightPos < OLED_HEIGHT; heightPos++) {
    // For each line, we shuffle all bits up a row
    for (uint8_t xPos = 0; xPos < OLED_WIDTH; xPos++) {
      const uint16_t firstStripPos  = FRAMEBUFFER_START + xPos;
      const uint16_t secondStripPos = firstStripPos + OLED_WIDTH;
#ifdef OLED_128x32
      // For 32 pixel high OLED's we have four strips to tailchain
      const uint16_t thirdStripPos  = secondStripPos + OLED_WIDTH;
      const uint16_t fourthStripPos = thirdStripPos + OLED_WIDTH;
      // Move the MSB off the first strip, and pop MSB from second strip onto the first strip
      screenBuffer[firstStripPos] = (screenBuffer[firstStripPos] >> 1) | ((screenBuffer[secondStripPos] & 0x01) << 7);
      // Now shuffle off the second strip
      screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] >> 1) | ((screenBuffer[thirdStripPos] & 0x01) << 7);
      // Now shuffle off the third strip
      screenBuffer[thirdStripPos] = (screenBuffer[thirdStripPos] >> 1) | ((screenBuffer[fourthStripPos] & 0x01) << 7);
      // Now forth strip gets the start of the new buffer
      screenBuffer[fourthStripPos] = (screenBuffer[fourthStripPos] >> 1) | ((secondFrameBuffer[firstStripPos] & 0x01) << 7);
      // Now cycle all the secondary buffers

      secondFrameBuffer[firstStripPos]  = (secondFrameBuffer[firstStripPos] >> 1) | ((secondFrameBuffer[secondStripPos] & 0x01) << 7);
      secondFrameBuffer[secondStripPos] = (secondFrameBuffer[secondStripPos] >> 1) | ((secondFrameBuffer[thirdStripPos] & 0x01) << 7);
      secondFrameBuffer[thirdStripPos]  = (secondFrameBuffer[thirdStripPos] >> 1) | ((secondFrameBuffer[fourthStripPos] & 0x01) << 7);
      // Finally on the bottom row; we shuffle it up ready
      secondFrameBuffer[fourthStripPos] >>= 1;
#else
      // Move the LSB off the first strip, and pop MSB from second strip onto the first strip
      screenBuffer[firstStripPos] = (screenBuffer[firstStripPos] >> 1) | ((screenBuffer[secondStripPos] & 0x01) << 7);
      // Now shuffle off the second strip MSB, and replace it with the LSB of the secondary buffer
      screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] >> 1) | ((secondFrameBuffer[firstStripPos] & 0x01) << 7);
      // Finally, do the shuffle on the second frame buffer
      secondFrameBuffer[firstStripPos] = (secondFrameBuffer[firstStripPos] >> 1) | ((secondFrameBuffer[secondStripPos] & 0x01) << 7);
      // Finally on the bottom row; we shuffle it up ready
      secondFrameBuffer[secondStripPos] >>= 1;
#endif /* OLED_128x32 */
    }
    buttonsReleased |= getButtonState() == BUTTON_NONE;
    if (getButtonState() != BUTTON_NONE && buttonsReleased) {
      // Exit early, but have to transition whole buffer
      memcpy(screenBuffer + FRAMEBUFFER_START, secondFrameBuffer + FRAMEBUFFER_START, sizeof(screenBuffer) - FRAMEBUFFER_START);
      refresh(); // Now refresh to write out the contents to the new page
      return;
    }
    refresh(); // Now refresh to write out the contents to the new page
    vTaskDelayUntil(&startDraw, TICKS_100MS / 7);
  }
}
/**
 * This assumes that the current display output buffer has the current on screen contents
 * Then the secondary buffer has the "new" contents to be slid down onto the screen
 * Sadly we cant use the hardware scroll as some devices with the 128x32 screens dont have the GRAM for holding both screens at once
 *
 * **This function blocks until the transition has completed or user presses button**
 */
void OLED::transitionScrollUp(const TickType_t viewEnterTime) {
  TickType_t startDraw       = xTaskGetTickCount();
  bool       buttonsReleased = getButtonState() == BUTTON_NONE;

  for (uint8_t heightPos = 0; heightPos < OLED_HEIGHT; heightPos++) {
    // For each line, we shuffle all bits down a row
    for (uint8_t xPos = 0; xPos < OLED_WIDTH; xPos++) {
      const uint16_t firstStripPos  = FRAMEBUFFER_START + xPos;
      const uint16_t secondStripPos = firstStripPos + OLED_WIDTH;
#ifdef OLED_128x32
      // For 32 pixel high OLED's we have four strips to tailchain
      const uint16_t thirdStripPos  = secondStripPos + OLED_WIDTH;
      const uint16_t fourthStripPos = thirdStripPos + OLED_WIDTH;
      // We are shffling LSB's off the end and pushing bits down
      screenBuffer[fourthStripPos] = (screenBuffer[fourthStripPos] << 1) | ((screenBuffer[thirdStripPos] & 0x80) >> 7);
      screenBuffer[thirdStripPos]  = (screenBuffer[thirdStripPos] << 1) | ((screenBuffer[secondStripPos] & 0x80) >> 7);
      screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] << 1) | ((screenBuffer[firstStripPos] & 0x80) >> 7);
      screenBuffer[firstStripPos]  = (screenBuffer[firstStripPos] << 1) | ((secondFrameBuffer[fourthStripPos] & 0x80) >> 7);

      secondFrameBuffer[fourthStripPos] = (secondFrameBuffer[fourthStripPos] << 1) | ((secondFrameBuffer[thirdStripPos] & 0x80) >> 7);
      secondFrameBuffer[thirdStripPos]  = (secondFrameBuffer[thirdStripPos] << 1) | ((secondFrameBuffer[secondStripPos] & 0x80) >> 7);
      secondFrameBuffer[secondStripPos] = (secondFrameBuffer[secondStripPos] << 1) | ((secondFrameBuffer[firstStripPos] & 0x80) >> 7);
      // Finally on the bottom row; we shuffle it up ready
      secondFrameBuffer[firstStripPos] <<= 1;
#else
      // We pop the LSB off the bottom row, and replace the MSB in that byte with the LSB of the row above
      screenBuffer[secondStripPos] = (screenBuffer[secondStripPos] << 1) | ((screenBuffer[firstStripPos] & 0x80) >> 7);
      // Move the LSB off the first strip, and pop MSB from second strip onto the first strip
      screenBuffer[firstStripPos] = (screenBuffer[firstStripPos] << 1) | ((secondFrameBuffer[secondStripPos] & 0x80) >> 7);

      // Finally, do the shuffle on the second frame buffer
      secondFrameBuffer[secondStripPos] = (secondFrameBuffer[secondStripPos] << 1) | ((secondFrameBuffer[firstStripPos] & 0x80) >> 7);
      // Finally on the bottom row; we shuffle it up ready
      secondFrameBuffer[firstStripPos] <<= 1;
#endif /* OLED_128x32 */
    }
    buttonsReleased |= getButtonState() == BUTTON_NONE;
    if (getButtonState() != BUTTON_NONE && buttonsReleased) {
      // Exit early, but have to transition whole buffer
      memcpy(screenBuffer + FRAMEBUFFER_START, secondFrameBuffer + FRAMEBUFFER_START, sizeof(screenBuffer) - FRAMEBUFFER_START);
      refresh(); // Now refresh to write out the contents to the new page
      return;
    }
    refresh(); // Now refresh to write out the contents to the new page
    vTaskDelayUntil(&startDraw, TICKS_100MS / 7);
  }
}

void OLED::setRotation(bool leftHanded) {
#ifdef OLED_FLIP
  leftHanded = !leftHanded;
#endif /* OLED_FLIP */
  if (inLeftHandedMode == leftHanded) {
    return;
  }
#ifdef OLED_SEGMENT_MAP_REVERSED
  if (!leftHanded) {
    OLED_Setup_Array[9].val = 0xA1;
  } else {
    OLED_Setup_Array[9].val = 0xA0;
  }
#else
  if (leftHanded) {
    OLED_Setup_Array[9].val = 0xA1;
  } else {
    OLED_Setup_Array[9].val = 0xA0;
  }
#endif /* OLED_SEGMENT_MAP_REVERSED */
  // send command struct again with changes
  if (leftHanded) {
    OLED_Setup_Array[5].val = 0xC8; // c1?
  } else {
    OLED_Setup_Array[5].val = 0xC0;
  }
  I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, OLED_Setup_Array, sizeof(OLED_Setup_Array) / sizeof(OLED_Setup_Array[0]));
  osDelay(TICKS_10MS);
  inLeftHandedMode = leftHanded;

  screenBuffer[5] = inLeftHandedMode ? OLED_GRAM_START_FLIP : OLED_GRAM_START; // display is shifted by 32 in left handed
                                                                               // mode as driver ram is 128 wide
  screenBuffer[7] = inLeftHandedMode ? OLED_GRAM_END_FLIP : OLED_GRAM_END;     // End address of the ram segment we are writing to (96 wide)
  screenBuffer[9] = inLeftHandedMode ? 0xC8 : 0xC0;
  // Force a screen refresh
  const int len = FRAMEBUFFER_START + (OLED_WIDTH * (OLED_HEIGHT / 8));
  I2C_CLASS::Transmit(DEVICEADDR_OLED, screenBuffer, len);
  osDelay(TICKS_10MS);
  checkDisplayBufferChecksum();
}

void OLED::setBrightness(uint8_t contrast) {
  if (OLED_Setup_Array[15].val != contrast) {
    OLED_Setup_Array[15].val = contrast;
    I2C_CLASS::writeRegistersBulk(DEVICEADDR_OLED, &OLED_Setup_Array[14], 2);
  }
}

void OLED::setInverseDisplay(bool inverse) {
  uint8_t normalInverseCmd = inverse ? 0xA7 : 0xA6;
  if (OLED_Setup_Array[21].val != normalInverseCmd) {
    OLED_Setup_Array[21].val = normalInverseCmd;
    I2C_CLASS::I2C_RegisterWrite(DEVICEADDR_OLED, 0x80, normalInverseCmd);
  }
}

// print a string to the current cursor location, len chars MAX
void OLED::print(const char *const str, FontStyle fontStyle, uint8_t len) {
  const uint8_t *next = reinterpret_cast<const uint8_t *>(str);
  if (next[0] == 0x01) {
    fontStyle = FontStyle::LARGE;
    next++;
  }
  while (next[0] && len--) {
    uint16_t index;
    if (next[0] <= 0xF0) {
      index = next[0];
      next++;
    } else {
      if (!next[1]) {
        return;
      }
      index = (next[0] - 0xF0) * 0xFF - 15 + next[1];
      next += 2;
    }
    drawChar(index, fontStyle);
  }
}

/**
 * Prints a static string message designed to use the whole screen, starting
 * from the top-left corner.
 *
 * If the message starts with a newline (`\\x01`), the string starting from
 * after the newline is printed in the large font. Otherwise, the message
 * is printed in the small font.
 *
 * @param string The string message to be printed
 */
void OLED::printWholeScreen(const char *string) {
  setCursor(0, 0);
  if (string[0] == '\x01') {
    // Empty first line means that this uses large font (for CJK).
    OLED::print(string + 1, FontStyle::LARGE);
  } else {
    OLED::print(string, FontStyle::SMALL);
  }
}

// Print *F or *C - in font style of Small, Large (by default) or Extra based on input arg
void OLED::printSymbolDeg(const FontStyle fontStyle) {
  switch (fontStyle) {
  case FontStyle::EXTRAS:
    // Picks *F or *C in ExtraFontChars[] from Font.h
    OLED::drawSymbol(getSettingValue(SettingsOptions::TemperatureInF) ? 0 : 1);
    break;
  case FontStyle::LARGE:
    OLED::print(getSettingValue(SettingsOptions::TemperatureInF) ? LargeSymbolDegF : LargeSymbolDegC, fontStyle);
    break;
  case FontStyle::SMALL:
  default:
    OLED::print(getSettingValue(SettingsOptions::TemperatureInF) ? SmallSymbolDegF : SmallSymbolDegC, fontStyle);
    break;
  }
}

inline void stripLeaderZeros(char *buffer, uint8_t places) {
  // Removing the leading zero's by swapping them to SymbolSpace
  // Stop 1 short so that we dont blank entire number if its zero
  for (int i = 0; i < (places - 1); i++) {
    if (buffer[i] == 2) {
      buffer[i] = LargeSymbolSpace[0];
    } else {
      return;
    }
  }
}

void OLED::drawHex(uint32_t x, FontStyle fontStyle, uint8_t digits) {
  // print number to hex
  for (uint_fast8_t i = 0; i < digits; i++) {
    uint16_t value = (x >> (4 * (7 - i))) & 0b1111;
    drawChar(value + 2, fontStyle);
  }
}

// maximum places is 5
void OLED::printNumber(uint16_t number, uint8_t places, FontStyle fontStyle, bool noLeaderZeros) {
  char buffer[7] = {0};

  if (places >= 5) {
    buffer[5] = 2 + number % 10;
    number /= 10;
  }
  if (places > 4) {
    buffer[4] = 2 + number % 10;
    number /= 10;
  }

  if (places > 3) {
    buffer[3] = 2 + number % 10;
    number /= 10;
  }

  if (places > 2) {
    buffer[2] = 2 + number % 10;
    number /= 10;
  }

  if (places > 1) {
    buffer[1] = 2 + number % 10;
    number /= 10;
  }

  buffer[0] = 2 + number % 10;
  if (noLeaderZeros) {
    stripLeaderZeros(buffer, places);
  }
  print(buffer, fontStyle);
}

void OLED::debugNumber(int32_t val, FontStyle fontStyle) {
  if (abs(val) > 99999) {
    OLED::print(LargeSymbolSpace, fontStyle); // out of bounds
    return;
  }
  if (val >= 0) {
    OLED::print(LargeSymbolSpace, fontStyle);
    OLED::printNumber(val, 5, fontStyle);
  } else {
    OLED::print(LargeSymbolMinus, fontStyle);
    OLED::printNumber(-val, 5, fontStyle);
  }
}

void OLED::drawSymbol(uint8_t symbolID) {
  // draw a symbol to the current cursor location
  drawChar(symbolID, FontStyle::EXTRAS);
}

// Draw an area, but y must be aligned on 0/8 offset
void OLED::drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide) {
    return; // cutoffleft
  }
  if (x > 96) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > 96) {
    visibleEnd = 96 - x;
  }

  if (y == 0) {
    // Splat first line of data
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      stripPointers[0][xx + x] = ptr[xx];
    }
  }
  if (y == 8 || height >= 16) {
    // Splat the second line
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      stripPointers[1][x + xx] = ptr[xx + (height == 16 ? wide : 0)];
    }
  }
  // TODO NEEDS HEIGHT HANDLERS for 24/32
}

// Draw an area, but y must be aligned on 0/8 offset
// For data which has octets swapped in a 16-bit word.
void OLED::drawAreaSwapped(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide) {
    return; // cutoffleft
  }
  if (x > 96) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > 96) {
    visibleEnd = 96 - x;
  }

  if (y == 0) {
    // Splat first line of data
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx += 2) {
      stripPointers[0][xx + x]     = ptr[xx + 1];
      stripPointers[0][xx + x + 1] = ptr[xx];
    }
  }
  if (y == 8 || height == 16) {
    // Splat the second line
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx += 2) {
      stripPointers[1][x + xx]     = ptr[xx + 1 + (height == 16 ? wide : 0)];
      stripPointers[1][x + xx + 1] = ptr[xx + (height == 16 ? wide : 0)];
    }
  }
}

void OLED::fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value) {
  // Splat this from x->x+wide in two strides
  if (x <= -wide) {
    return; // cutoffleft
  }
  if (x > 96) {
    return; // cutoff right
  }

  uint8_t visibleStart = 0;
  uint8_t visibleEnd   = wide;

  // trimming to draw partials
  if (x < 0) {
    visibleStart -= x; // subtract negative value == add absolute value
  }
  if (x + wide > 96) {
    visibleEnd = 96 - x;
  }

  if (y == 0) {
    // Splat first line of data
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      stripPointers[0][xx + x] = value;
    }
  }
  if (y == 8 || height == 16) {
    // Splat the second line
    for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
      stripPointers[1][x + xx] = value;
    }
  }
}

void OLED::drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool clear) {
  // Draw this in 3 sections
  // This is basically a N wide version of vertical line

  // Step 1 : Draw in the top few pixels that are not /8 aligned
  // LSB is at the top of the screen
  uint8_t mask = 0xFF;
  if (y0) {
    mask = mask << (y0 % 8);
    for (uint8_t col = x0; col < x1; col++) {
      if (clear) {
        stripPointers[0][(y0 / 8) * 96 + col] &= ~mask;
      } else {
        stripPointers[0][(y0 / 8) * 96 + col] |= mask;
      }
    }
  }
  // Next loop down the line the total number of solids
  if (y0 / 8 != y1 / 8) {
    for (uint8_t col = x0; col < x1; col++) {
      for (uint8_t r = (y0 / 8); r < (y1 / 8); r++) {
        // This gives us the row index r
        if (clear) {
          stripPointers[0][(r * 96) + col] = 0;
        } else {
          stripPointers[0][(r * 96) + col] = 0xFF;
        }
      }
    }
  }

  // Finally draw the tail
  mask = ~(mask << (y1 % 8));
  for (uint8_t col = x0; col < x1; col++) {
    if (clear) {
      stripPointers[0][(y1 / 8) * 96 + col] &= ~mask;
    } else {
      stripPointers[0][(y1 / 8) * 96 + col] |= mask;
    }
  }
}

void OLED::drawHeatSymbol(uint8_t state) {
  // Draw symbol 14
  // Then draw over it, the bottom 5 pixels always stay. 8 pixels above that are
  // the levels masks the symbol nicely
  state /= 31; // 0-> 8 range
  // Then we want to draw down (16-(5+state)
  uint8_t cursor_x_temp = cursor_x;
  drawSymbol(14);
  drawFilledRect(cursor_x_temp, 0, cursor_x_temp + 12, 2 + (8 - state), true);
}

bool OLED::isInitDone() { return initDone; }
