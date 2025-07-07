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
#include "Translation.h"
#include "configuration.h"
#include <string.h> // for memset
bool sanitiseSettings();

/*
 * Used to constrain the QC 3.0 Voltage selection to suit hardware.
 * We allow a little overvoltage for users who want to push it
 */
#ifdef POW_QC_20V
#define QC_VOLTAGE_MAX 220
#else
#define QC_VOLTAGE_MAX 140
#endif /* POW_QC_20V */

// char (*__kaboom)[sizeof(systemSettingsType)] = 1; // Uncomment to print size at compile time
volatile systemSettingsType   systemSettings;
extern const SettingConstants settingsConstants[(int)SettingsOptions::SettingsOptionsLength];

void saveSettings() {
#ifdef CANT_DIRECT_READ_SETTINGS
  // For these devices flash is not 1:1 mapped, so need to read into staging buffer
  systemSettingsType settings;
  flash_read_buffer((uint8_t *)&settings, sizeof(systemSettingsType));
  if (memcmp((void *)&settings, (void *)&systemSettings, sizeof(systemSettingsType))) {
    flash_save_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));
  }

#else
  if (memcmp((void *)SETTINGS_START_PAGE, (void *)&systemSettings, sizeof(systemSettingsType))) {
    flash_save_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));
  }

#endif /* CANT_DIRECT_READ_SETTINGS */
}

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
  if (systemSettings.versionMarker != SETTINGSVERSION) {
    memset((void *)&systemSettings, 0xFF, sizeof(systemSettings));
    systemSettings.versionMarker = SETTINGSVERSION;
    dirty                        = true;
  }
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
      dirty                            = true;
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
  const auto constants        = settingsConstants[(int)option];
  uint16_t   constrainedValue = newValue;
  if (constrainedValue < constants.min) {
    // If less than min, constrain
    constrainedValue = constants.min;
  } else if (constrainedValue > constants.max) {
    // If hit max, constrain
    constrainedValue = constants.max;
  }
  systemSettings.settingsValues[(int)option] = constrainedValue;
}

// Lookup wrapper for ease of use (with typing)
uint16_t getSettingValue(const enum SettingsOptions option) { return systemSettings.settingsValues[(int)option]; }

// Increment by the step size to the next value. If past the end wrap to the minimum
// Returns true if we are on the _last_ value
void nextSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  if (systemSettings.settingsValues[(int)option] == (constants.max)) {
    // Already at max, wrap to the start
    systemSettings.settingsValues[(int)option] = constants.min;
  } else if (systemSettings.settingsValues[(int)option] >= (constants.max - constants.increment)) {
    // If within one increment of the end, constrain to the end
    systemSettings.settingsValues[(int)option] = constants.max;
  } else {
    // Otherwise increment
    systemSettings.settingsValues[(int)option] += constants.increment;
  }
}

bool isLastSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  uint16_t   max       = constants.max;
  // handle temp unit limitations
  if (option == SettingsOptions::SolderingTemp) {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      max = MAX_TEMP_F;
    } else {
      max = MAX_TEMP_C;
    }
  } else if (option == SettingsOptions::BoostTemp) {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      max = MAX_TEMP_F;
    } else {
      max = MAX_TEMP_C;
    }
  } else if (option == SettingsOptions::SleepTemp) {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      max = 580;
    } else {
      max = 300;
    }
  } else if (option == SettingsOptions::UILanguage) {
    return isLastLanguageOption();
  }
  return systemSettings.settingsValues[(int)option] > (max - constants.increment);
}
// Step backwards on the settings item
// Return true if we are at the end (min)
void prevSettingValue(const enum SettingsOptions option) {
  const auto constants = settingsConstants[(int)option];
  if (systemSettings.settingsValues[(int)option] == (constants.min)) {
    // Already at min, wrap to the max
    systemSettings.settingsValues[(int)option] = constants.max;
  } else if (systemSettings.settingsValues[(int)option] <= (constants.min + constants.increment)) {
    // If within one increment of the start, constrain to the start
    systemSettings.settingsValues[(int)option] = constants.min;
  } else {
    // Otherwise decrement
    systemSettings.settingsValues[(int)option] -= constants.increment;
  }
}

uint16_t lookupHallEffectThreshold() {
  // Return the threshold above which the hall effect sensor is "activated"
  // We want this to be roughly exponentially mapped from 0-1000
  switch (getSettingValue(SettingsOptions::HallEffectSensitivity)) {
  case 0:
    return 0;
  case 1:
    return 1000;
  case 2:
    return 750;
  case 3:
    return 500;
  case 4:
    return 250;
  case 5:
    return 150;
  case 6:
    return 100;
  case 7:
    return 75;
  case 8:
    return 50;
  case 9:
    return 25;
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
  if (minVoltageOnCell == 0) {
    return 90; // 9V since iron does not function effectively below this
  } else {
    return (minVoltageOnCell * minVoltageCellCount) + (minVoltageCellCount * 2);
  }
}

#ifdef TIP_TYPE_SUPPORT
const char *lookupTipName() {
  // Get the name string for the current soldering tip
  tipType_t value = (tipType_t)getSettingValue(SettingsOptions::SolderingTipType);

  switch (value) {
#ifdef TIPTYPE_T12
  case tipType_t::T12_8_OHM:
    return translatedString(Tr->TipTypeT12Long);
    break;
  case tipType_t::T12_6_2_OHM:
    return translatedString(Tr->TipTypeT12Short);
    break;
  case tipType_t::T12_4_OHM:
    return translatedString(Tr->TipTypeT12PTS);
    break;
#endif
#ifdef TIPTYPE_TS80
  case tipType_t::TS80_4_5_OHM:
    return translatedString(Tr->TipTypeTS80);
    break;
#endif
#ifdef TIPTYPE_JBC
  case tipType_t::JBC_210_2_5_OHM:
    return translatedString(Tr->TipTypeJBCC210);
    break;
#endif
#ifdef AUTO_TIP_SELECTION
  case tipType_t::TIP_TYPE_AUTO:
#endif
  default:
    return translatedString(Tr->TipTypeAuto);
    break;
  }
}
#endif /* TIP_TYPE_SUPPORT */

// Returns the resistance for the current tip selected by the user or 0 for auto
#ifdef TIP_TYPE_SUPPORT
uint8_t getUserSelectedTipResistance() {
  tipType_t value = (tipType_t)getSettingValue(SettingsOptions::SolderingTipType);

  switch (value) {
#ifdef AUTO_TIP_SELECTION
  case tipType_t::TIP_TYPE_AUTO:
    return 0;
    break;
#endif
#ifdef TIPTYPE_T12
  case tipType_t::T12_8_OHM:
    return 80;
    break;
  case tipType_t::T12_6_2_OHM:
    return 62;
    break;
  case tipType_t::T12_4_OHM:
    return 40;
    break;
#endif
#ifdef TIPTYE_TS80
  case tipType_t::TS80_4_5_OHM:
    return 45;
    break;
#endif
#ifdef TIPTYPE_JBC
  case tipType_t::JBC_210_2_5_OHM:
    return 25;
    break;
#endif
  default:
    return 0;
    break;
  }
}
#else
uint8_t getUserSelectedTipResistance() { return tipType_t::TIP_TYPE_AUTO; }
#endif /* TIP_TYPE_SUPPORT */
