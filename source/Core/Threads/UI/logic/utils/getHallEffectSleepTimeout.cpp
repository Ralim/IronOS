#include "OperatingModeUtilities.h"

#ifndef NO_SLEEP_MODE
#ifdef HALL_SENSOR
uint32_t getHallEffectSleepTimeout(void) {

  if (getSettingValue(SettingsOptions::HallEffectSensitivity) && getSettingValue(SettingsOptions::HallEffectSleepTime)) {

    uint32_t sleepThres = getSettingValue(SettingsOptions::HallEffectSleepTime) * 5 * TICKS_SECOND;
    return sleepThres;

  }
  return TICKS_SECOND;
}
#endif
#endif
