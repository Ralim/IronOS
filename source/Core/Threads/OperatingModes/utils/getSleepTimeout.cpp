#include "OperatingModeUtilities.h"

#ifndef NO_SLEEP_MODE

uint32_t getSleepTimeout(void) {

  if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {

    uint32_t sleepThres = 0;
    if (getSettingValue(SettingsOptions::SleepTime) < 6)
      sleepThres = getSettingValue(SettingsOptions::SleepTime) * 10 * 1000;
    else
      sleepThres = (getSettingValue(SettingsOptions::SleepTime) - 5) * 60 * 1000;
    return sleepThres;
  }
  return 0;
}
#endif