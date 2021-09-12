/*
 * Settings.c
 *
 *  Created on: 29 Sep 2016
 *      Author: Ralim
 *
 *      This file holds the users settings and saves / restores them to the
 * devices flash
 */

#include "Settings.h"
#include "BSP.h"
#include "Setup.h"
#include "configuration.h"
#include <string.h> // for memset
bool sanitiseSettings();

/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from
 * flash in uint16_t chunks
 */
typedef struct {
  uint16_t length; // Length of valid bytes following
  uint16_t settingsValues[SettingsOptionsLength];
  // used to make this nicely "good enough" aligned to 32 butes to make driver code trivial
  uint32_t padding;

} systemSettingsType;

//~1024 is common programming size, setting threshold to be lower so we have warning
static_assert(sizeof(systemSettingsType) < 512);
// char (*__kaboom)[sizeof(systemSettingsType)] = 1; // Uncomment to print size at compile time
volatile systemSettingsType systemSettings;

// For every setting we need to store the min/max/increment values
typedef struct {
  const uint16_t min;          // Inclusive minimum value
  const uint16_t max;          // Exclusive maximum value
  const uint16_t increment;    // Standard increment
  const uint16_t defaultValue; // Default vaue after reset
} SettingConstants;
static const SettingConstants settingsConstants[(int)SettingsOptions::SettingsOptionsLength] = {
    //{min,max,increment,default}
    {0, 0, 0, 0},                                                    // SolderingTemp
    {10, 580, 0, 0},                                                 // SleepTemp
    {0, 16, 1, 0},                                                   // SleepTime
    {0, 5, 1, 0},                                                    // MinDCVoltageCells
    {24, 38, 1, 31},                                                 // MinVoltageCells
    {0, QC_SETTINGS_MAX, 1, 0},                                      // QCIdealVoltage
    {0, 0, 0, 0},                                                    // OrientationMode
    {0, 10, 0, 0},                                                   // Sensitivity
    {0, 1, 1, 1},                                                    // AnimationLoop
    {0, settingOffSpeed_t::MAX_VALUE, 1, settingOffSpeed_t::MEDIUM}, // AnimationSpeed
    {0, 4, 1, 0},                                                    // AutoStartMode
    {0, 60, 1, 0},                                                   // ShutdownTime
    {0, 1, 1, 0},                                                    // CoolingTempBlink
    {0, 1, 1, 0},                                                    // DetailedIDLE
    {0, 1, 1, 0},                                                    // DetailedSoldering
    {0, 1, 1, 0},                                                    // TemperatureInF
    {0, 0, 0, 0},                                                    // DescriptionScrollSpeed
    {0, 3, 1, 0},                                                    // LockingMode
    {0, 100, 1, 0},                                                  // KeepAwakePulse
    {1, POWER_PULSE_WAIT_MAX, 1, 0},                                 // KeepAwakePulseWait
    {1, POWER_PULSE_DURATION_MAX, 1, 0},                             // KeepAwakePulseDuration
    {360, 900, 1, 0},                                                // VoltageDiv
    {0, 0, 0, 0},                                                    // BoostTemp
    {0, 0, 0, 0},                                                    // CalibrationOffset
    {0, MAX_POWER_LIMIT, POWER_LIMIT_STEPS, 0},                      // PowerLimit
    {0, 1, 1, 0},                                                    // ReverseButtonTempChangeEnabled
    {5, 90, 5, 10},                                                  // TempChangeLongStep
    {1, TEMP_CHANGE_SHORT_STEP_MAX, 1, 1},                           // TempChangeShortStep
    {0, 4, 1, 0},                                                    // HallEffectSensitivity
    {0, 10, 1, 0},                                                   // AccelMissingWarningCounter
    {0, 10, 1, 0},                                                   // PDMissingWarningCounter
    {0, 0, 0, 0},                                                    // UILanguage
    {0, 50, 1, 0},                                                   // PDNegTimeout

};

void saveSettings() { flash_save_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType)); }

bool loadSettings() {
  // We read the flash
  flash_read_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));
  // Then ensure all values are valid
  return sanitiseSettings();
}

bool sanitiseSettings() {
  // For all settings, need to ensure settings are in a valid range
  // First for any not know about due to array growth, reset them and update the length value
  bool dirty = false;
  if (systemSettings.length < (int)SettingsOptions::SettingsOptionsLength) {
    dirty = true;
    for (int i = systemSettings.length; i < (int)SettingsOptions::SettingsOptionsLength; i++) {
      systemSettings.settingsValues[i] = 0xFFFF; // Ensure its as if it was erased
    }
    systemSettings.length = (int)SettingsOptions::SettingsOptionsLength;
  }
  for (int i = 0; i < (int)SettingsOptions::SettingsOptionsLength; i++) {
    // Check min max for all settings, if outside the range, move to default
    if (systemSettings.settingsValues[i] < settingsConstants[i].min || systemSettings.settingsValues[i] > settingsConstants[i].max) {
      systemSettings.settingsValues[i] = settingsConstants[i].defaultValue;

      dirty = true;
    }
  }
  if (dirty) {
    saveSettings();
  }
  return dirty;
}
void resetSettings() {
  memset((void *)&systemSettings, 0xFF, sizeof(systemSettingsType));
  sanitiseSettings();
  saveSettings(); // Save defaults
}

void setSettingValue(const enum SettingsOptions option, const uint16_t newValue) {
  const auto constants                       = settingsConstants[(int)option];
  systemSettings.settingsValues[(int)option] = newValue;
  if (systemSettings.settingsValues[(int)option] < constants.min) {
    systemSettings.settingsValues[(int)option] = constants.min;
  }
  // If hit max, constrain
  if (systemSettings.settingsValues[(int)option] >= constants.max) {
    systemSettings.settingsValues[(int)option] = constants.max - 1;
  }
}
uint16_t getSettingValue(const enum SettingsOptions option) { return systemSettings.settingsValues[(int)option]; }

bool nextSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  if (systemSettings.settingsValues[(int)option] == (constants.max - 1)) {
    systemSettings.settingsValues[(int)option] = constants.min;
  } else {
    systemSettings.settingsValues[(int)option] += constants.increment;
  }
  return systemSettings.settingsValues[(int)option] == constants.max - 1;
}

bool prevSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  int        value     = systemSettings.settingsValues[(int)option];
  if (value == constants.min) {
    value = constants.max;
  } else {
    value -= constants.increment;
  }
  systemSettings.settingsValues[(int)option] = value;
  return systemSettings.settingsValues[(int)option] == constants.min;
}
uint16_t lookupHallEffectThreshold() {
  // Return the threshold above which the hall effect sensor is "activated"
  switch (getSettingValue(SettingsOptions::HallEffectSensitivity)) {
  case 0:
    return 0;
  case 1: // Low
    return 1000;
  case 2: // Medium
    return 500;
  case 3: // High
    return 100;
  default:
    return 0; // Off
  }
}
// Lookup function for cutoff setting -> X10 voltage
/*
 * 0=DC
 * 1=3S
 * 2=4S
 * 3=5S
 * 4=6S
 */
uint8_t lookupVoltageLevel() {
  auto minVoltageOnCell    = getSettingValue(SettingsOptions::MinDCVoltageCells);
  auto minVoltageCellCount = getSettingValue(SettingsOptions::MinVoltageCells);
  if (minVoltageOnCell == 0)
    return 90; // 9V since iron does not function effectively below this
  else
    return (minVoltageOnCell * minVoltageCellCount) + (minVoltageCellCount * 2);
}