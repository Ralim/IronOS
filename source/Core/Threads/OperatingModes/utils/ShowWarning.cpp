#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
void warnUser(const char *warning, const TickType_t timeout) {
  OLED::clearScreen();
  OLED::printWholeScreen(warning);
  OLED::refresh();
  waitForButtonPressOrTimeout(timeout);
}
