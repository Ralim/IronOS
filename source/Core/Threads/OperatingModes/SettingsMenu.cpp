#include "OperatingModes.h"
#include "ScrollMessage.hpp"

#define HELP_TEXT_TIMEOUT_TICKS (TICKS_SECOND * 3)
/*
 * The settings menu is the most complex bit of GUI code we have
 * The menu consists of a two tier menu
 * Main menu -> Categories
 * Secondary menu -> Settings
 *
 * For each entry in the menu
 */

/**
 * Prints two small lines (or one line for CJK) of short description for
 * setting items and prepares cursor after it.
 * @param settingsItemIndex Index of the setting item.
 * @param cursorCharPosition Custom cursor char position to set after printing
 * description.
 */
static void printShortDescription(SettingsItemIndex settingsItemIndex, uint16_t cursorCharPosition) {
  // print short description (default single line, explicit double line)
  uint8_t shortDescIndex = static_cast<uint8_t>(settingsItemIndex);
  OLED::printWholeScreen(translatedString(Tr->SettingsShortNames[shortDescIndex]));

  // prepare cursor for value
  // make room for scroll indicator
  OLED::setCursor(cursorCharPosition * FONT_12_WIDTH - 2, 0);
}

// Render a menu, based on the position given
// This will either draw the menu item, or the help text depending on how long its been since button press
void render_menu(const menuitem *item, guiContext *cxt) {

  // If recent interaction or not help text draw the entry
  if ((xTaskGetTickCount() - lastButtonTime < HELP_TEXT_TIMEOUT_TICKS) || item->description == 0) {

    if (item->shortDescriptionSize > 0) {
      printShortDescription(item->shortDescriptionIndex, item->shortDescriptionSize);
    }
    item->draw();
  } else {

    uint16_t *isRenderingHelp = &(cxt->scratch_state.state6);
    *isRenderingHelp          = 1;
    // Draw description
    const char *description = translatedString(Tr->SettingsDescriptions[item->description - 1]);
    drawScrollingText(description, (xTaskGetTickCount() - lastButtonTime) - HELP_TEXT_TIMEOUT_TICKS);
  }
}

uint16_t getMenuLength(const menuitem *menu, const uint16_t stop) {
  // walk this menu to find the length
  uint16_t counter = 0;
  for (uint16_t pos = 0; pos < stop; pos++) {
    // End of list
    if (menu[pos].draw == nullptr) {
      return counter;
    }
    // Otherwise increment for each visible item (null == always, or if not check function)
    if (menu[pos].isVisible == nullptr || menu[pos].isVisible()) {
      counter++;
    }
  }
  return counter;
}

OperatingMode moveToNextEntry(guiContext *cxt) {
  uint16_t *mainEntry         = &(cxt->scratch_state.state1);
  uint16_t *subEntry          = &(cxt->scratch_state.state2);
  uint16_t *currentMenuLength = &(cxt->scratch_state.state5);
  uint16_t *isRenderingHelp   = &(cxt->scratch_state.state6);

  if (*isRenderingHelp) {
    *isRenderingHelp = 0;
  } else {
    *currentMenuLength = 0; // Reset menu length
    // Scroll down
    // We can increment freely _once_
    cxt->transitionMode = TransitionAnimation::Down;
    if (*subEntry == 0) {
      (*mainEntry) += 1;

      if (rootSettingsMenu[*mainEntry].draw == nullptr) {
        // We are off the end of the menu now
        saveSettings();
        cxt->transitionMode = TransitionAnimation::Left;
        return OperatingMode::HomeScreen;
      }
      // Check if visible
      if (rootSettingsMenu[*mainEntry].isVisible != nullptr && !rootSettingsMenu[*mainEntry].isVisible()) {
        // We need to move on as this one isn't visible
        return moveToNextEntry(cxt);
      }
    } else {
      (*subEntry) += 1;

      // If the new entry is null, we need to exit
      if (subSettingsMenus[*mainEntry][(*subEntry) - 1].draw == nullptr) {
        (*subEntry)         = 0; // Reset back to the main menu
        cxt->transitionMode = TransitionAnimation::Left;
        // Have to break early to avoid the below check underflowing
        return OperatingMode::SettingsMenu;
      }
      // Check if visible
      if (subSettingsMenus[*mainEntry][(*subEntry) - 1].isVisible != nullptr && !subSettingsMenus[*mainEntry][(*subEntry) - 1].isVisible()) {
        // We need to move on as this one isn't visible
        return moveToNextEntry(cxt);
      }
    }
  }
  return OperatingMode::SettingsMenu;
}

OperatingMode gui_SettingsMenu(const ButtonState buttons, guiContext *cxt) {
  // Render out the current settings menu
  // State 1 -> Root menu
  // State 2 -> Sub entry
  // Draw main entry if sub-entry is 0, otherwise draw sub-entry

  uint16_t *mainEntry              = &(cxt->scratch_state.state1);
  uint16_t *subEntry               = &(cxt->scratch_state.state2);
  uint32_t *autoRepeatAcceleration = &(cxt->scratch_state.state3);
  uint32_t *autoRepeatTimer        = &(cxt->scratch_state.state4);
  uint16_t *currentMenuLength      = &(cxt->scratch_state.state5);
  uint16_t *isRenderingHelp        = &(cxt->scratch_state.state6);

  const menuitem *currentMenu;
  // Draw the currently on screen item
  uint16_t currentScreen;
  if (*subEntry == 0) {
    // Drawing main menu
    currentMenu   = rootSettingsMenu;
    currentScreen = *mainEntry;
  } else {
    // Drawing sub menu
    currentMenu   = subSettingsMenus[*mainEntry];
    currentScreen = (*subEntry) - 1;
  }
  render_menu(&(currentMenu[currentScreen]), cxt);

  // Update the cached menu length if unknown
  if (*currentMenuLength == 0) {
    // We walk the current menu to find the length
    *currentMenuLength = getMenuLength(currentMenu, 128 /* Max length of any menu*/);
  }
  if (*isRenderingHelp == 0) {
    //  Draw scroll

    // Get virtual pos by counting entries from start to _here_
    uint16_t currentVirtualPosition = getMenuLength(currentMenu, currentScreen + 1);
    if (currentVirtualPosition > 0) {
      currentVirtualPosition--;
    }
    // The height of the indicator is screen res height / total menu entries
    uint8_t indicatorHeight = OLED_HEIGHT / *currentMenuLength;
    if (indicatorHeight == 0) {
      indicatorHeight = 1; // always at least 1 pixel
    }

    uint16_t position = (OLED_HEIGHT * (uint16_t)currentVirtualPosition) / *currentMenuLength;
    // Draw if not last item || flash if it is
    if ((*currentMenuLength != currentVirtualPosition) || (xTaskGetTickCount() % 1000 < 500)) {
      OLED::drawScrollIndicator((uint8_t)position, indicatorHeight);
    }
  }
  // Now handle user button input

  auto callIncrementHandler = [&]() {
    if ((int)currentMenu[currentScreen].autoSettingOption < (int)SettingsOptions::SettingsOptionsLength) {
      return nextSettingValue(currentMenu[currentScreen].autoSettingOption);
    } else if (currentMenu[currentScreen].incrementHandler != nullptr) {
      return currentMenu[currentScreen].incrementHandler();
    }
    return false;
  };

  //
  OperatingMode newMode = OperatingMode::SettingsMenu;
  switch (buttons) {
  case BUTTON_NONE:
    (*autoRepeatAcceleration) = 0; // reset acceleration
    (*autoRepeatTimer)        = 0; // reset acceleration
    break;
  case BUTTON_BOTH:
    if (*subEntry == 0) {
      saveSettings();
      cxt->transitionMode = TransitionAnimation::Left;
      return OperatingMode::HomeScreen;
    } else {
      cxt->transitionMode = TransitionAnimation::Left;
      *subEntry           = 0;
      return OperatingMode::SettingsMenu;
    }
    break;

  case BUTTON_F_LONG:
    if (xTaskGetTickCount() + (*autoRepeatAcceleration) > (*autoRepeatTimer) + PRESS_ACCEL_INTERVAL_MAX) {
      if (callIncrementHandler()) {
        (*autoRepeatTimer) = TICKS_SECOND * 2;
      } else {
        (*autoRepeatTimer) = 0;
      }
      (*autoRepeatTimer) += xTaskGetTickCount();
      (*autoRepeatAcceleration) += PRESS_ACCEL_STEP;
    }
    break;
  case BUTTON_F_SHORT:
    // Increment setting
    if (*isRenderingHelp) {
      *isRenderingHelp = 0;
    } else {
      uint16_t *currentMenuLength = &(cxt->scratch_state.state5);
      *currentMenuLength          = 0;
      if (*subEntry == 0) {
        // In a root menu, if its null handler we enter the menu
        if (currentMenu[currentScreen].incrementHandler != nullptr) {
          currentMenu[currentScreen].incrementHandler();
        } else {
          (*subEntry) += 1;
          cxt->transitionMode = TransitionAnimation::Right;
        }
      } else {
        callIncrementHandler();
      }
    }
    break;
  case BUTTON_B_LONG:
    if (xTaskGetTickCount() + (*autoRepeatAcceleration) > (*autoRepeatTimer) + PRESS_ACCEL_INTERVAL_MAX) {
      (*autoRepeatTimer) = xTaskGetTickCount();
      (*autoRepeatAcceleration) += PRESS_ACCEL_STEP;
    } else {
      break;
    }
    /* Fall through*/
  case BUTTON_B_SHORT:
    // Increment menu item
    newMode = moveToNextEntry(cxt);
    break;

  default:
    break;
  }
  if ((PRESS_ACCEL_INTERVAL_MAX - (*autoRepeatAcceleration)) < PRESS_ACCEL_INTERVAL_MIN) {
    (*autoRepeatAcceleration) = PRESS_ACCEL_INTERVAL_MAX - PRESS_ACCEL_INTERVAL_MIN;
  }

  // Otherwise we stay put for next render iteration
  return newMode;
}