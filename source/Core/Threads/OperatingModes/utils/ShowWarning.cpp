#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
bool warnUser(const char *warning, const ButtonState buttons) {
  OLED::clearScreen();
  OLED::printWholeScreen(warning);
  // TODO also timeout
  return buttons != BUTTON_NONE;
}
