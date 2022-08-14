#include "BootLogo.h"
#include "BSP.h"
#include "Buttons.hpp"
#include "OLED.hpp"
#include "cmsis_os.h"
#define LOGO_PAGE_LENGTH 1024

void delay() {
  if (getSettingValue(SettingsOptions::LOGOTime) == 5) {
    waitForButtonPress();
  } else {
    waitForButtonPressOrTimeout(TICKS_SECOND * getSettingValue(SettingsOptions::LOGOTime));
  }
}

void BootLogo::handleShowingLogo(const uint8_t *ptrLogoArea) {
  // Read the first few bytes and figure out what format we are looking at
  if (OLD_LOGO_HEADER_VALUE == *(reinterpret_cast<const uint32_t *>(ptrLogoArea))) {
    showOldFormat(ptrLogoArea);
  } else if (ptrLogoArea[0] == 0xAA) {
    showNewFormat(ptrLogoArea + 1);
  }
  OLED::clearScreen();
  OLED::refresh();
}

void BootLogo::showOldFormat(const uint8_t *ptrLogoArea) {

  OLED::drawAreaSwapped(0, 0, 96, 16, (uint8_t *)(ptrLogoArea + 4));
  OLED::refresh();

  // Delay here until button is pressed or its been the amount of seconds set by the user
  delay();
}

void BootLogo::showNewFormat(const uint8_t *ptrLogoArea) {
  if (getSettingValue(SettingsOptions::LOGOTime) == 0) {
    return;
  }
  // New logo format (a) fixes long standing byte swap quirk and (b) supports animation
  uint8_t interFrameDelay = ptrLogoArea[0];
  OLED::clearScreen();
  ButtonState buttons = getButtonState();

  // Now draw in the frames
  int position = 1;
  do {

    int len = (showNewFrame(ptrLogoArea + position));
    OLED::refresh();
    position += len;
    buttons = getButtonState();

    if (interFrameDelay) {
      osDelay(interFrameDelay * 4);
    }
    // 1024 less the header type byte and the inter-frame-delay
    if (getSettingValue(SettingsOptions::LOGOTime) > 0 && (position >= 1022 || len == 0)) {
      // Delay here until button is pressed or its been the amount of seconds set by the user
      delay();
      return;
    }
  } while (buttons == BUTTON_NONE);
}
int BootLogo::showNewFrame(const uint8_t *ptrLogoArea) {
  uint8_t length = ptrLogoArea[0];

  if (length == 0xFF) {
    // Full frame update
    OLED::drawArea(0, 0, 96, 16, ptrLogoArea + 1);
    length = 96;
  } else if (length == 0xFE) {
    return 1;
  } else if (length == 0) {
    return 0; // end
  } else {
    length /= 2;
    // Draw length patches
    for (int p = 0; p < length; p++) {
      uint8_t index = ptrLogoArea[1 + (p * 2)];
      uint8_t value = ptrLogoArea[2 + (p * 2)];
      OLED::drawArea(index % 96, index >= 96 ? 8 : 0, 1, 8, &value);
    }
  }

  OLED::refresh();
  return (length * 2) + 1;
}
