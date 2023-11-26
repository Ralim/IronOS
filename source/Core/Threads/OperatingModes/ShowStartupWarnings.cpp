#include "HUB238.hpp"
#include "OperatingModes.h"
OperatingMode showWarnings(const ButtonState buttons, guiContext *cxt) {
  // Display alert if settings were reset

  switch (cxt->scratch_state.state1) {
  case 0: // Settings reset warning
    if (settingsWereReset) {
      if (warnUser(translatedString(Tr->SettingsResetMessage), buttons)) {
        settingsWereReset         = false;
        cxt->scratch_state.state1 = 1;
      }
    } else {
      cxt->scratch_state.state1 = 1;
    }
    break;
  case 1: // Device validations
#ifdef DEVICE_HAS_VALIDATION_SUPPORT
    if (getDeviceValidationStatus()) {
      // Warn user this device might be counterfeit
      if (warnUser(translatedString(Tr->DeviceFailedValidationWarning), buttons)) {
        cxt->scratch_state.state1 = 2;
      }
    } else {
      cxt->scratch_state.state1 = 2;
    }
#else
    cxt->scratch_state.state1 = 2;
#endif
    break;
  case 2: // Accelerometer detection
    if (DetectedAccelerometerVersion == AccelType::Scanning) {
      break;
    }
    // Display alert if accelerometer is not detected
    if (DetectedAccelerometerVersion == AccelType::None) {
      if (getSettingValue(SettingsOptions::AccelMissingWarningCounter) < 2) {

        if (warnUser(translatedString(Tr->NoAccelerometerMessage), buttons)) {
          cxt->scratch_state.state1 = 3;
          nextSettingValue(SettingsOptions::AccelMissingWarningCounter);
          saveSettings();
        }
      } else {
        cxt->scratch_state.state1 = 3;
      }
    } else {
      cxt->scratch_state.state1 = 3;
    }
    break;
  case 3:

#ifdef POW_PD
    // We expect pd to be present
    if (!USBPowerDelivery::fusbPresent()) {
      if (getSettingValue(SettingsOptions::PDMissingWarningCounter) < 2) {
        if (warnUser(translatedString(Tr->NoPowerDeliveryMessage), buttons)) {
          nextSettingValue(SettingsOptions::PDMissingWarningCounter);
          saveSettings();
          cxt->scratch_state.state1 = 4;
        }
      } else {
        cxt->scratch_state.state1 = 4;
      }
    } else {
      cxt->scratch_state.state1 = 4;
    }
#else
#if POW_PD_EXT == 1
    if (!hub238_probe()) {
      if (getSettingValue(SettingsOptions::PDMissingWarningCounter) < 2) {
        if (warnUser(translatedString(Tr->NoPowerDeliveryMessage), buttons)) {
          cxt->scratch_state.state1 = 4;
          nextSettingValue(SettingsOptions::PDMissingWarningCounter);
          saveSettings();
        }
      } else {
        cxt->scratch_state.state1 = 4;
      }
    } else {
      cxt->scratch_state.state1 = 4;
    }
#else
    cxt->scratch_state.state1 = 4;
#endif /*POW_PD_EXT==1*/
#endif /*POW_PD*/

    break;
  default:
    // We are off the end, warnings done
    return OperatingMode::StartupLogo;
  }

  return OperatingMode::StartupWarnings; // Stay in warnings
}
