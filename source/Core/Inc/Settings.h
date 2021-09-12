/*
 * Settings.h
 *
 *  Created on: 29 Sep 2016
 *      Author: Ralim
 *
 *      Houses the system settings and allows saving / restoring from flash
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_
#include <stdbool.h>
#include <stdint.h>
#define SETTINGSVERSION (0x2A) // This number is frozen, do not edit

enum SettingsOptions {
  SolderingTemp,                  // current set point for the iron
  SleepTemp,                      // temp to drop to in sleep
  SleepTime,                      // minutes timeout to sleep
  MinDCVoltageCells,              // The voltage we cut out at for under voltage when powered by DC jack
  MinVoltageCells,                // Minimum allowed voltage per cell <3S - 3.0V (30)> <4S - 2.4V (24)> <...> (Minimum recommended 2.7V)
  QCIdealVoltage,                 // Desired QC3.0 voltage (9,12,20V)
  OrientationMode,                // Selects between Auto,Right and left handed layouts
  Sensitivity,                    // Sensitivity of accelerometer (5 bits)
  AnimationLoop,                  // Animation loop switch
  AnimationSpeed,                 // Animation speed (in miliseconds)
  AutoStartMode,                  // Should the unit automatically jump straight into soldering mode when power is applied
  ShutdownTime,                   // Time until unit shuts down if left alone
  CoolingTempBlink,               // Should the temperature blink on the cool down screen until its <50C
  DetailedIDLE,                   // Detailed idle screen
  DetailedSoldering,              // Detailed soldering screens
  TemperatureInF,                 // Should the temp be in F or C (true is F)
  DescriptionScrollSpeed,         // Description scroll speed
  LockingMode,                    // Store the locking mode
  KeepAwakePulse,                 // Keep Awake pulse power in 0.1 watts (10 = 1Watt)
  KeepAwakePulseWait,             // Time between Keep Awake pulses in 2500 ms = 2.5 s increments
  KeepAwakePulseDuration,         // Duration of the Keep Awake pusle in 250 ms increments
  VoltageDiv,                     // Voltage divisor factor
  BoostTemp,                      // Boost mode set point for the iron
  CalibrationOffset,              // This stores the temperature offset for this tip in the iron.
  PowerLimit,                     // Maximum power iron allowed to output
  ReverseButtonTempChangeEnabled, // Change the plus and minus button assigment
  TempChangeLongStep,             // Change the plus and minus button assigment
  TempChangeShortStep,            // Change the plus and minus button assigment
  HallEffectSensitivity,          // Operating mode of the hall effect sensor
  AccelMissingWarningCounter,     // Counter of how many times we have warned we cannot detect the accelerometer
  PDMissingWarningCounter,        // Counter of how many times we have warned we cannot detect the pd interface
  UILanguage,                     // Selected UI Language code, null-terminated *only if* the length is less than 8 chars
  PDNegTimeout,                   // PD timeout in 100ms steps

  //
  SettingsOptionsLength, //
};

typedef enum {
  OFF       = 0, // Off (disabled)
  SLOW      = 1, //
  MEDIUM    = 2, //
  FAST      = 3, //
  MAX_VALUE = 4  //
} settingOffSpeed_t;
// Settings wide operations
void saveSettings();
bool loadSettings();
void resetSettings();

// Settings access

uint16_t getSettingValue(const enum SettingsOptions option);
// Returns true if setting is now on the last value (next iteration will wrap)
bool nextSettingValue(const enum SettingsOptions option);
bool prevSettingValue(const enum SettingsOptions option);

void setSettingValue(const enum SettingsOptions option, const uint16_t newValue);

// Special access
uint8_t  lookupVoltageLevel();
uint16_t lookupHallEffectThreshold();

#endif /* SETTINGS_H_ */
