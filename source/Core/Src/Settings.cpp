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

#ifdef POW_QC_20V
#define QC_VOLTAGE_MAX 222
#else
#define QC_VOLTAGE_MAX 142
#endif

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
    {10, MAX_TEMP_F, 5, 320},                                       // SolderingTemp
    {10, MAX_TEMP_F, 5, 150},                                       // SleepTemp
    {0, 16, 1, SLEEP_TIME},                                         // SleepTime
    {0, 5, 1, CUT_OUT_SETTING},                                     // MinDCVoltageCells
    {24, 38, 1, RECOM_VOL_CELL},                                    // MinVoltageCells
    {90, QC_VOLTAGE_MAX, 2, 90},                                    // QCIdealVoltage
    {0, 3, 1, ORIENTATION_MODE},                                    // OrientationMode
    {0, 10, 1, SENSITIVITY},                                        // Sensitivity
    {0, 2, 1, ANIMATION_LOOP},                                      // AnimationLoop
    {0, settingOffSpeed_t::MAX_VALUE, 1, ANIMATION_SPEED},          // AnimationSpeed
    {0, 4, 1, AUTO_START_MODE},                                     // AutoStartMode
    {0, 61, 1, SHUTDOWN_TIME},                                      // ShutdownTime
    {0, 2, 1, COOLING_TEMP_BLINK},                                  // CoolingTempBlink
    {0, 2, 1, DETAILED_IDLE},                                       // DetailedIDLE
    {0, 2, 1, DETAILED_SOLDERING},                                  // DetailedSoldering
    {0, 2, 1, TEMPERATURE_INF},                                     // TemperatureInF
    {0, 2, 1, DESCRIPTION_SCROLL_SPEED},                            // DescriptionScrollSpeed
    {0, 3, 1, LOCKING_MODE},                                        // LockingMode
    {0, 100, 1, POWER_PULSE_DEFAULT},                               // KeepAwakePulse
    {1, POWER_PULSE_WAIT_MAX, 1, POWER_PULSE_WAIT_DEFAULT},         // KeepAwakePulseWait
    {1, POWER_PULSE_DURATION_MAX, 1, POWER_PULSE_DURATION_DEFAULT}, // KeepAwakePulseDuration
    {360, 900, 1, VOLTAGE_DIV},                                     // VoltageDiv
    {0, MAX_TEMP_F, 10, BOOST_TEMP},                                // BoostTemp
    {100, 2500, 1, CALIBRATION_OFFSET},                             // CalibrationOffset
    {0, MAX_POWER_LIMIT, POWER_LIMIT_STEPS, POWER_LIMIT},           // PowerLimit
    {0, 2, 1, REVERSE_BUTTON_TEMP_CHANGE},                          // ReverseButtonTempChangeEnabled
    {5, TEMP_CHANGE_LONG_STEP_MAX, 5, TEMP_CHANGE_LONG_STEP},       // TempChangeLongStep
    {1, TEMP_CHANGE_SHORT_STEP_MAX, 1, TEMP_CHANGE_SHORT_STEP},     // TempChangeShortStep
    {0, 4, 1, 1},                                                   // HallEffectSensitivity
    {0, 10, 1, 0},                                                  // AccelMissingWarningCounter
    {0, 10, 1, 0},                                                  // PDMissingWarningCounter
    {0, 0xFFFF, 0, 41431 /*EN*/},                                   // UILanguage
    {0, 51, 1, 0},                                                  // PDNegTimeout
    {0, 2, 1, 0},                                                   // OLEDInversion
    {0, 100, 11, 33},                                               // OLEDBrightness

};
static_assert((sizeof(settingsConstants) / sizeof(SettingConstants)) == ((int)SettingsOptions::SettingsOptionsLength));

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
  if (systemSettings.padding != 0xFFFFFFFF) {
    systemSettings.padding = 0xFFFFFFFF; // Force padding to 0xFFFFFFFF so that rolling forwards / back should be easier
    dirty                  = true;
  }
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
  if (systemSettings.settingsValues[(int)option] >= (constants.max - constants.increment)) {
    systemSettings.settingsValues[(int)option] = constants.min;
  } else {
    systemSettings.settingsValues[(int)option] += constants.increment;
  }
  return (constants.max - systemSettings.settingsValues[(int)option]) <= constants.increment;
}

bool prevSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  int        value     = systemSettings.settingsValues[(int)option];
  if (value <= constants.min) {
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
