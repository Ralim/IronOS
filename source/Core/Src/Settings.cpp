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

#include "string.h"
volatile systemSettingsType systemSettings;

void saveSettings() {
  // First we erase the flash
  flash_save_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));
}

bool restoreSettings() {
  // We read the flash
  flash_read_buffer((uint8_t *)&systemSettings, sizeof(systemSettingsType));

  // if the version is correct were done
  // if not we reset and save
  if (systemSettings.version != SETTINGSVERSION) {
    // probably not setup
    resetSettings();
    return true;
  }
  return false;
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
  if (systemSettings.minDCVoltageCells == 0)
    return 90; // 9V since iron does not function effectively below this
  else
    return (systemSettings.minDCVoltageCells * systemSettings.minVoltageCells) + (systemSettings.minVoltageCells * 2);
}
void resetSettings() {
  memset((void *)&systemSettings, 0, sizeof(systemSettingsType));
  systemSettings.SleepTemp = SLEEP_TEMP; // Temperature the iron sleeps at - default 150.0 C
  systemSettings.SleepTime = SLEEP_TIME; // How many seconds/minutes we wait until going
  // to sleep - default 1 min
  systemSettings.SolderingTemp                  = SOLDERING_TEMP;               // Default soldering temp is 320.0 C
  systemSettings.minDCVoltageCells              = CUT_OUT_SETTING;              // default to no cut-off voltage
  systemSettings.minVoltageCells                = RECOM_VOL_CELL;               // Minimum voltage per cell (Recommended 3.3V (33))
  systemSettings.QCIdealVoltage                 = 0;                            // Default to 9V for QC3.0 Voltage
  systemSettings.version                        = SETTINGSVERSION;              // Store the version number to allow for easier upgrades
  systemSettings.detailedSoldering              = DETAILED_SOLDERING;           // Detailed soldering screen
  systemSettings.detailedIDLE                   = DETAILED_IDLE;                // Detailed idle screen (off for first time users)
  systemSettings.OrientationMode                = ORIENTATION_MODE;             // Default to automatic
  systemSettings.sensitivity                    = SENSITIVITY;                  // Default high sensitivity
  systemSettings.voltageDiv                     = VOLTAGE_DIV;                  // Default divider from schematic
  systemSettings.ShutdownTime                   = SHUTDOWN_TIME;                // How many minutes until the unit turns itself off
  systemSettings.BoostTemp                      = BOOST_TEMP;                   // default to 400C
  systemSettings.autoStartMode                  = AUTO_START_MODE;              // Auto start off for safety
  systemSettings.lockingMode                    = LOCKING_MODE;                 // Disable locking for safety
  systemSettings.coolingTempBlink               = COOLING_TEMP_BLINK;           // Blink the temperature on the cooling screen when its > 50C
  systemSettings.temperatureInF                 = TEMPERATURE_INF;              // default to 0
  systemSettings.descriptionScrollSpeed         = DESCRIPTION_SCROLL_SPEED;     // default to slow
  systemSettings.animationLoop                  = ANIMATION_LOOP;               // Default false
  systemSettings.animationSpeed                 = ANIMATION_SPEED;              // Default 400 ms (Medium)
  systemSettings.CalibrationOffset              = CALIBRATION_OFFSET;           // the adc offset in uV
  systemSettings.powerLimit                     = POWER_LIMIT;                  // 30 watts default limit
  systemSettings.ReverseButtonTempChangeEnabled = REVERSE_BUTTON_TEMP_CHANGE;   //
  systemSettings.TempChangeShortStep            = TEMP_CHANGE_SHORT_STEP;       //
  systemSettings.TempChangeLongStep             = TEMP_CHANGE_LONG_STEP;        //
  systemSettings.KeepAwakePulse                 = POWER_PULSE_DEFAULT;          // Power of the power pulse
  systemSettings.KeepAwakePulseWait             = POWER_PULSE_WAIT_DEFAULT;     // Time between Keep Awake pulses in 2.5 second increments
  systemSettings.KeepAwakePulseDuration         = POWER_PULSE_DURATION_DEFAULT; // Duration of the Keep Awake pusle in 250ms increments
  systemSettings.hallEffectSensitivity          = 1;                            // How sensitive is the hall sensor for putting the unit to sleep
  systemSettings.accelMissingWarningCounter     = 0;                            // Have we arned for missing accel
  systemSettings.pdMissingWarningCounter        = 0;                            // Have we warned for missing FUSB302
  systemSettings.QCNegotiationMode              = 0;                            // What set of tunings are we using for the FUSB302

  saveSettings(); // Save defaults
}

uint16_t lookupHallEffectThreshold() {
  // Return the threshold above which the hall effect sensor is "activated"
  switch (systemSettings.hallEffectSensitivity) {
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
