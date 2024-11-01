#include "BootLogo.h"
#include "BSP.h"
#include "Buttons.hpp"
#include "OLED.hpp"
#include "Settings.h"
#include "cmsis_os.h"

#define LOGO_PAGE_LENGTH 1024

void delay() {
  if (getSettingValue(SettingsOptions::LOGOTime) >= logoMode_t::ONETIME) {
    waitForButtonPress();
  } else {
    waitForButtonPressOrTimeout(TICKS_SECOND * getSettingValue(SettingsOptions::LOGOTime));
  }
}

void BootLogo::handleShowingLogo(const uint8_t *ptrLogoArea) {
  OLED::clearScreen();
  // Read the first few bytes and figure out what format we are looking at
  if (OLD_LOGO_HEADER_VALUE == *(reinterpret_cast<const uint32_t *>(ptrLogoArea))) {
    showOldFormat(ptrLogoArea);
  } else if (ptrLogoArea[0] == 0xAA) {
    showNewFormat(ptrLogoArea + 1);
  }

  OLED::clearScreen();
}

void BootLogo::showOldFormat(const uint8_t *ptrLogoArea) {
#ifdef OLED_128x32
  // Draw in middle
  OLED::drawAreaSwapped(16, 8, 96, 16, (uint8_t *)(ptrLogoArea + 4));

#else
  OLED::drawAreaSwapped(0, 0, 96, 16, (uint8_t *)(ptrLogoArea + 4));

#endif
  OLED::refresh();
  // Delay here with static logo until a button is pressed or its been the amount of seconds set by the user
  delay();
}

void BootLogo::showNewFormat(const uint8_t *ptrLogoArea) {
  if (getSettingValue(SettingsOptions::LOGOTime) == logoMode_t::SKIP) {
    return;
  }

  // New logo format (a) fixes long standing byte swap quirk and (b) supports animation
  uint8_t interFrameDelay = ptrLogoArea[0];
  OLED::clearScreen();

  // Now draw in the frames
  int position = 1;
  while (getButtonState() == BUTTON_NONE) {
    int len = (showNewFrame(ptrLogoArea + position));
    OLED::refresh();
    position += len;

    if (interFrameDelay) {
      osDelay(interFrameDelay * 4);
    }

    // 1024 less the header type byte and the inter-frame-delay
    if (getSettingValue(SettingsOptions::LOGOTime) && (position >= 1022 || len == 0)) {
      // Animated logo stops here ...
      if (getSettingValue(SettingsOptions::LOGOTime) == logoMode_t::INFINITY) {
        // ... but if it's infinite logo setting then keep it rolling over again until a button is pressed
        osDelay(4 * TICKS_100MS);
        OLED::clearScreen();
        position = 1;
        continue;
      }
    } else {
      // Animation in progress so jumping to the next frame
      continue;
    }

    // Static logo case ends up right here, so delay until a button is pressed or its been the amount of seconds set by the user
    delay();
    return;
  }
}

int BootLogo::showNewFrame(const uint8_t *ptrLogoArea) {
  uint8_t length = ptrLogoArea[0];
  switch (length) {
  case 0:
    // End
    return 0;
    break;
  case 0xFE:
    return 1;
    break;
  case 0xFF:
// Full frame update
#ifdef OLED_128x32
    OLED::drawArea(16, 8, 96, 16, ptrLogoArea + 1);
#else
    OLED::drawArea(0, 0, 96, 16, ptrLogoArea + 1);
#endif
    length = 96;
    break;
  default:
    length /= 2;
    // Draw length patches
    for (int p = 0; p < length; p++) {
      uint8_t index = ptrLogoArea[1 + (p * 2)];
      uint8_t value = ptrLogoArea[2 + (p * 2)];
#ifdef OLED_128x32
      OLED::drawArea(16 + (index % 96), index >= 96 ? 16 : 8, 1, 8, &value);
#else
      OLED::drawArea(index % 96, index >= 96 ? 8 : 0, 1, 8, &value);
#endif
    }
  }

  OLED::refresh();
  return (length * 2) + 1;
}
