#include "OperatingModes.h"

void showWarnings(void) {
  // Display alert if settings were reset
  if (settingsWereReset) {
    warnUser(translatedString(Tr->SettingsResetMessage), 10 * TICKS_SECOND);
  }
#ifdef DEVICE_HAS_VALIDATION_SUPPORT
  if (getDeviceValidationStatus()) {
    // Warn user this device might be counterfeit
    warnUser(translatedString(Tr->DeviceFailedValidationWarning), 10 * TICKS_SECOND);
  }
#endif
#ifndef NO_WARN_MISSING
  // We also want to alert if accel or pd is not detected / not responding
  // In this case though, we dont want to nag the user _too_ much
  // So only show first 2 times
  while (DetectedAccelerometerVersion == AccelType::Scanning) {
    osDelay(5);
  }
  // Display alert if accelerometer is not detected
  if (DetectedAccelerometerVersion == AccelType::None) {
    if (getSettingValue(SettingsOptions::AccelMissingWarningCounter) < 2) {
      nextSettingValue(SettingsOptions::AccelMissingWarningCounter);
      saveSettings();
      warnUser(translatedString(Tr->NoAccelerometerMessage), 10 * TICKS_SECOND);
    }
  }
#if POW_PD
  // We expect pd to be present
  if (!USBPowerDelivery::fusbPresent()) {
    if (getSettingValue(SettingsOptions::PDMissingWarningCounter) < 2) {
      nextSettingValue(SettingsOptions::PDMissingWarningCounter);
      saveSettings();
      warnUser(translatedString(Tr->NoPowerDeliveryMessage), 10 * TICKS_SECOND);
    }
  }
#endif
#endif
}