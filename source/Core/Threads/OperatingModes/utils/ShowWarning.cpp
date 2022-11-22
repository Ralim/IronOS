#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
void warnUser(const char *warning, const int timeout) {
  OLED::clearScreen();
  OLED::printWholeScreen(warning);
  OLED::refresh();
  waitForButtonPressOrTimeout(timeout);
}
