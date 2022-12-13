#include "Buttons.hpp"
#include "OperatingModeUtilities.h"

TickType_t        lastHallEffectSleepStart = 0;
extern TickType_t lastMovementTime;

bool shouldBeSleeping(bool inAutoStart) {
#ifndef NO_SLEEP_MODE
  // Return true if the iron should be in sleep mode
  if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {
    if (inAutoStart) {
      // In auto start we are asleep until movement
      if (lastMovementTime == 0 && lastButtonTime == 0) {
        return true;
      }
    }
    if (lastMovementTime > 0 || lastButtonTime > 0) {
      if (((xTaskGetTickCount() - lastMovementTime) > getSleepTimeout()) && ((xTaskGetTickCount() - lastButtonTime) > getSleepTimeout())) {
        return true;
      }
    }
  }

#ifdef HALL_SENSOR
  // If the hall effect sensor is enabled in the build, check if its over
  // threshold, and if so then we force sleep
  if (getHallSensorFitted() && lookupHallEffectThreshold()) {
    int16_t hallEffectStrength = getRawHallEffect();
    if (hallEffectStrength < 0)
      hallEffectStrength = -hallEffectStrength;
    // Have absolute value of measure of magnetic field strength
    if (hallEffectStrength > lookupHallEffectThreshold()) {
      if (lastHallEffectSleepStart == 0) {
        lastHallEffectSleepStart = xTaskGetTickCount();
      }
      if ((xTaskGetTickCount() - lastHallEffectSleepStart) > TICKS_SECOND) {
        return true;
      }
    } else {
      lastHallEffectSleepStart = 0;
    }
  }
#endif
#endif
  return false;
}