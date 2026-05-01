#include "OperatingModeUtilities.h"
#include "main.hpp"

extern TickType_t lastMovementTime;
extern TickType_t lastHallEffectSleepStart;
#include "Buttons.hpp"

bool shouldShutdown(void) {
  // Note: freefall is handled directly in each soldering mode (returns FreefallWarning).
  // shouldShutdown() is called from Sleep mode only; propagate it from there too.
  if (getSettingValue(SettingsOptions::ShutdownTime)) { // only allow shutdown exit if time > 0
    if (lastMovementTime) {
      if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) > (TickType_t)(getSettingValue(SettingsOptions::ShutdownTime) * TICKS_MIN)) {
        return true;
      }
    }
    if (lastHallEffectSleepStart) {
      if (((TickType_t)(xTaskGetTickCount() - lastHallEffectSleepStart)) > (TickType_t)(getSettingValue(SettingsOptions::ShutdownTime) * TICKS_MIN)) {
        return true;
      }
    }
  }
  if (getButtonState() == BUTTON_B_LONG) { // allow also if back button is pressed long
    return true;
  }
  return false;
}
