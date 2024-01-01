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
  LockingMode                    = 17, // Store the locking mode
  KeepAwakePulse                 = 18, // Keep Awake pulse power in 0.1 watts (10 = 1Watt)
  KeepAwakePulseWait             = 19, // Time between Keep Awake pulses in 2500 ms = 2.5 s increments
  KeepAwakePulseDuration         = 20, // Duration of the Keep Awake pusle in 250 ms increments
  VoltageDiv                     = 21, // Voltage divisor factor
  BoostTemp                      = 22, // Boost mode set point for the iron
  CalibrationOffset              = 23, // This stores the temperature offset for this tip in the iron.
  PowerLimit                     = 24, // Maximum power iron allowed to output
  ReverseButtonTempChangeEnabled = 25, // Change the plus and minus button assigment
  TempChangeLongStep             = 26, // Temperature-change-increment on long button press
  TempChangeShortStep            = 27, // Temperature-change-increment on short button press
  HallEffectSensitivity          = 28, // Operating mode of the hall effect sensor
  AccelMissingWarningCounter     = 29, // Counter of how many times we have warned we cannot detect the accelerometer
  PDMissingWarningCounter        = 30, // Counter of how many times we have warned we cannot detect the pd interface
  UILanguage                     = 31, // Selected UI Language code, null-terminated *only if* the length is less than 8 chars
  PDNegTimeout                   = 32, // PD timeout in 100ms steps
  OLEDInversion                  = 33, // Invert the colours on the display
  OLEDBrightness                 = 34, // Brightness for the OLED display
  LOGOTime                       = 35, // Duration the logo will be displayed for
  CalibrateCJC                   = 36, // Toggle calibrate CJC at next boot
  BluetoothLE                    = 37, // Toggle BLE if present
  PDVpdo                         = 38, // Toggle PPS & EPR
  ProfilePhases                  = 39, // Number of profile mode phases
  ProfilePreheatTemp             = 40, // Temperature to preheat to before the first phase
  ProfilePreheatSpeed            = 41, // Maximum allowed preheat speed in degrees per second
  ProfilePhase1Temp              = 42, // Temperature to target for the end of phase 1
  ProfilePhase1Duration          = 43, // Target duration for phase 1
  ProfilePhase2Temp              = 44, // Temperature to target for the end of phase 2
  ProfilePhase2Duration          = 45, // Target duration for phase 2
  ProfilePhase3Temp              = 46, // Temperature to target for the end of phase 3
  ProfilePhase3Duration          = 47, // Target duration for phase 3
  ProfilePhase4Temp              = 48, // Temperature to target for the end of phase 4
  ProfilePhase4Duration          = 49, // Target duration for phase 4
  ProfilePhase5Temp              = 50, // Temperature to target for the end of phase 5
  ProfilePhase5Duration          = 51, // Target duration for phase 5
  ProfileCooldownSpeed           = 52, // Maximum allowed cooldown speed in degrees per second
  //
  SettingsOptionsLength = 53, //
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

typedef enum {
  RIGHT = 0, // Right-hand screen orientation
  LEFT  = 1, // Left-hand screen orientation
  AUTO  = 2, // Automatic screen orientation based on accel.data if presented
} orientationMode_t;

typedef enum {
  SKIP     = 0, // Skip boot logo
  ONETIME  = 5, // Show boot logo once (if animated) and stall until a button toggled
  INFINITY = 6, // Show boot logo on repeat (if animated) until a button toggled
} logoMode_t;

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
