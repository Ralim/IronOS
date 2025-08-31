#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
#ifdef OLED_128x32
bool warnUser(const char *warning, const ButtonState buttons) {
  OLED::clearScreen();
  OLED::printWholeScreen(warning);
  // Also timeout after 5 seconds
  if ((xTaskGetTickCount() - lastButtonTime) > TICKS_SECOND * 5) {
    return true;
  }
  return buttons != BUTTON_NONE;
}
#endif