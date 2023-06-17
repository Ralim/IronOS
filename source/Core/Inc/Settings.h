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
  SolderingTemp                  = 0,  // current set point for the iron
  SleepTemp                      = 1,  // temp to drop to in sleep
  SleepTime                      = 2,  // minutes timeout to sleep
  MinDCVoltageCells              = 3,  // The voltage we cut out at for under voltage when powered by DC jack
  MinVoltageCells                = 4,  // Minimum allowed voltage per cell <3S - 3.0V (30)> <4S - 2.4V (24)> <...> (Minimum recommended 2.7V)
  QCIdealVoltage                 = 5,  // Desired QC3.0 voltage (9,12,20V)
  OrientationMode                = 6,  // Selects between Auto,Right and left handed layouts
  Sensitivity                    = 7,  // Sensitivity of accelerometer (5 bits)
  AnimationLoop                  = 8,  // Animation loop switch
  AnimationSpeed                 = 9,  // Animation speed (in miliseconds)
  AutoStartMode                  = 10, // Should the unit automatically jump straight into soldering mode when power is applied
  ShutdownTime                   = 11, // Time until unit shuts down if left alone
  CoolingTempBlink               = 12, // Should the temperature blink on the cool down screen until its <50C
  DetailedIDLE                   = 13, // Detailed idle screen
  DetailedSoldering              = 14, // Detailed soldering screens
  TemperatureInF                 = 15, // Should the temp be in F or C (true is F)
  DescriptionScrollSpeed         = 16, // Description scroll speed
  KeepAwakePulse                 = 17, // Keep Awake pulse power in 0.1 watts (10 = 1Watt)
  KeepAwakePulseWait             = 18, // Time between Keep Awake pulses in 2500 ms = 2.5 s increments
  KeepAwakePulseDuration         = 19, // Duration of the Keep Awake pusle in 250 ms increments
  VoltageDiv                     = 20, // Voltage divisor factor
  BoostTemp                      = 21, // Boost mode set point for the iron
  CalibrationOffset              = 22, // This stores the temperature offset for this tip in the iron.
  PowerLimit                     = 23, // Maximum power iron allowed to output
  ReverseButtonTempChangeEnabled = 24, // Change the plus and minus button assigment
  TempChangeLongStep             = 25, // Temperature-change-increment on long button press
  TempChangeShortStep            = 26, // Temperature-change-increment on short button press
  HallEffectSensitivity          = 27, // Operating mode of the hall effect sensor
  AccelMissingWarningCounter     = 28, // Counter of how many times we have warned we cannot detect the accelerometer
  PDMissingWarningCounter        = 29, // Counter of how many times we have warned we cannot detect the pd interface
  UILanguage                     = 30, // Selected UI Language code, null-terminated *only if* the length is less than 8 chars
  PDNegTimeout                   = 31, // PD timeout in 100ms steps
  OLEDInversion                  = 32, // Invert the colours on the display
  OLEDBrightness                 = 33, // Brightness for the OLED display
  LOGOTime                       = 34, // Duration the logo will be displayed for
  CalibrateCJC                   = 35, // Toggle calibrate CJC at next boot
  BluetoothLE                    = 36, // Toggle BLE if present
  PDVpdo                         = 37, // Toggle PPS & EPR
  ProfilePhases                  = 38, // Number of profile mode phases
  ProfilePreheatTemp             = 39, // Temperature to preheat to before the first phase
  ProfilePreheatSpeed            = 40, // Maximum allowed preheat speed in degrees per second
  ProfilePhase1Temp              = 41, // Temperature to target for the end of phase 1
  ProfilePhase1Duration          = 42, // Target duration for phase 1
  ProfilePhase2Temp              = 43, // Temperature to target for the end of phase 2
  ProfilePhase2Duration          = 44, // Target duration for phase 2
  ProfilePhase3Temp              = 45, // Temperature to target for the end of phase 3
  ProfilePhase3Duration          = 46, // Target duration for phase 3
  ProfilePhase4Temp              = 47, // Temperature to target for the end of phase 4
  ProfilePhase4Duration          = 48, // Target duration for phase 4
  ProfilePhase5Temp              = 49, // Temperature to target for the end of phase 5
  ProfilePhase5Duration          = 50, // Target duration for phase 5
  ProfileCooldownSpeed           = 51, // Maximum allowed cooldown speed in degrees per second
  //
  SettingsOptionsLength = 52, //
};

typedef enum {
  OFF       = 0, // Off (disabled)
  SLOW      = 1, //
  MEDIUM    = 2, //
  FAST      = 3, //
  MAX_VALUE = 4  //
} settingOffSpeed_t;

typedef enum {
  NO     = 0, // Disabled
  SOLDER = 1, // Gain default soldering temp (Soldering Mode)
  SLEEP  = 2, // Gain default sleeping temp (Idle/Standby Mode)
  ZERO   = 3, // Power on only (No heat Mode)
} autoStartMode_t;

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
