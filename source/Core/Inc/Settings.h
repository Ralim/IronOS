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
#define SETTINGSVERSION (0x2A)
/*Change this if you change the struct below to prevent people getting \
          out of sync*/

/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from
 * flash in uint16_t chunks
 */
typedef struct {
  uint8_t version; // Used to track if a reset is needed on firmware upgrade

  uint16_t SolderingTemp;       // current set point for the iron
  uint16_t SleepTemp;           // temp to drop to in sleep
  uint8_t  SleepTime;           // minutes timeout to sleep
  uint8_t  minDCVoltageCells;   // The voltage we cut out at for under voltage when powered by DC jack
  uint8_t  minVoltageCells;     // Minimum allowed voltage per cell <3S - 3.0V (30)> <4S - 2.4V (24)> <...> (Minimum recommended 2.7V)
  uint8_t  QCIdealVoltage;      // Desired QC3.0 voltage (9,12,20V)
  uint8_t  OrientationMode : 2; // Selects between Auto,Right and left handed layouts
  uint8_t  sensitivity : 4;     // Sensitivity of accelerometer (5 bits)
  uint8_t  animationLoop : 1;   // Animation loop switch
  uint8_t  animationSpeed : 2;  // Animation speed (in miliseconds)
  uint8_t  autoStartMode : 2;   // Should the unit automatically jump straight
                                // into soldering mode when power is applied
  uint8_t ShutdownTime;         // Time until unit shuts down if left alone

  uint8_t coolingTempBlink : 1;       // Should the temperature blink on the cool
                                      // down screen until its <50C
  uint8_t detailedIDLE : 1;           // Detailed idle screen
  uint8_t detailedSoldering : 1;      // Detailed soldering screens
  uint8_t temperatureInF : 1;         // Should the temp be in F or C (true is F)
  uint8_t descriptionScrollSpeed : 1; // Description scroll speed
  uint8_t lockingMode : 2;            // Store the locking mode
  uint8_t KeepAwakePulse;             // Keep Awake pulse power in 0.1 watts (10 = 1Watt)
  uint8_t KeepAwakePulseWait;         // Time between Keep Awake pulses in 2500 ms = 2.5 s increments
  uint8_t KeepAwakePulseDuration;     // Duration of the Keep Awake pusle in 250 ms increments

  uint16_t voltageDiv;        // Voltage divisor factor
  uint16_t BoostTemp;         // Boost mode set point for the iron
  uint16_t CalibrationOffset; // This stores the temperature offset for this tip
                              // in the iron.

  uint8_t powerLimit; // Maximum power iron allowed to output

  uint8_t  ReverseButtonTempChangeEnabled; // Change the plus and minus button assigment
  uint16_t TempChangeLongStep;             // Change the plus and minus button assigment
  uint16_t TempChangeShortStep;            // Change the plus and minus button assigment
  uint8_t  hallEffectSensitivity;          // Operating mode of the hall effect sensor
  uint8_t  accelMissingWarningCounter;     // Counter of how many times we have warned we cannot detect the accelerometer
  uint8_t  pdMissingWarningCounter;        // Counter of how many times we have warned we cannot detect the pd interface
  char     uiLanguage[8];                  // Selected UI Language code, null-terminated *only if* the length is less than 8 chars
  uint8_t  QCNegotiationMode : 2;          // QC Mode (0-3)
  uint32_t padding;                        // This is here for in case we are not an even divisor so
                                           // that nothing gets cut off
                                           // MUST BE LAST

} systemSettingsType;
typedef enum {
  OFF       = 0, // Off (disabled)
  SLOW      = 1, //
  MEDIUM    = 2, //
  FAST      = 3, //
  MAX_VALUE = 4  //
} settingOffSpeed_t;
extern volatile systemSettingsType systemSettings;

void     saveSettings();
bool     restoreSettings();
uint8_t  lookupVoltageLevel();
uint16_t lookupHallEffectThreshold();
void     resetSettings();

#endif /* SETTINGS_H_ */
