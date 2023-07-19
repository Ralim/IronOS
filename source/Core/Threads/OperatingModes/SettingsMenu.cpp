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
    // Draw description
    const char *description = translatedString(Tr->SettingsDescriptions[item->description - 1]);
    drawScrollingText(description, xTaskGetTickCount() - lastButtonTime);
  }
}

uint16_t getMenuLength(const menuitem *menu) {
  // walk this menu to find the length
  for (uint16_t pos = 0; pos < 64; pos++) {
    if (menu[pos].draw == NULL) {
      return pos + 1;
    }
  }
  return 0; // Cant find length, be safe
}

OperatingMode gui_SettingsMenu(const ButtonState buttons, guiContext *cxt) {
  // Render out the current settings menu
  // State 1 -> Root menu
  // State 2 -> Sub entry
  // Draw main entry if sub-entry is 0, otherwise draw sub-entry

  uint16_t *mainEntry         = &(cxt->scratch_state.state1);
  uint16_t *subEntry          = &(cxt->scratch_state.state2);
  uint16_t *currentMenuLength = &(cxt->scratch_state.state5);

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
    currentScreen = *subEntry;
  }
  render_menu(&(currentMenu[currentScreen]), cxt);

  // Update the cached menu length if unknown
  if (*currentMenuLength == 0) {
    // We walk the current menu to find the length
    *currentMenuLength = getMenuLength(currentMenu);
  }
  //  Draw scroll
  uint8_t indicatorHeight = OLED_HEIGHT / *currentMenuLength;
  uint8_t position        = (OLED_HEIGHT * currentScreen) / *currentMenuLength;
  // Draw if not last item
  if ((*currentMenuLength != currentScreen) || xTaskGetTickCount() % 1000 < 500) {
    OLED::drawScrollIndicator(position, indicatorHeight);
  }

  // Now handle user button input

  switch (buttons) {
  case BUTTON_NONE:
    break;
  case BUTTON_BOTH:
    return OperatingMode::HomeScreen;
    break;

  case BUTTON_B_LONG:
    break;
  case BUTTON_F_LONG:

    break;
  case BUTTON_F_SHORT:
    break;
  case BUTTON_B_SHORT:
    break;
  default:
    break;
  }
  // Otherwise we stay put for next render iteration
  return OperatingMode::SettingsMenu;
}