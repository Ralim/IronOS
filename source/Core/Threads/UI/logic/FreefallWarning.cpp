
#include "OperatingModes.h"

OperatingMode gui_freefallWarning(const ButtonState buttons, guiContext *cxt) {
  currentTempTargetDegC = 0; // Keep heater off

  OLED::clearScreen();
  OLED::printWholeScreen(translatedString(Tr->WarningFreefallDetected));

  // Dismiss on any button press or after 2.5 seconds
  if (buttons != BUTTON_NONE || (xTaskGetTickCount() - cxt->viewEnterTime) > (TICKS_SECOND * 2 + TICKS_SECOND / 2)) {
    return OperatingMode::HomeScreen;
  }
  return OperatingMode::FreefallWarning;
}
