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
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "unit.h"
#define SETTINGSVERSION   ( 0x20 )
/*Change this if you change the struct below to prevent people getting \
          out of sync*/

/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from
 * flash in uint16_t chunks
 */
typedef struct {
	uint8_t version;   // Used to track if a reset is needed on firmware upgrade

	uint16_t SolderingTemp;  // current set point for the iron
	uint16_t SleepTemp;      // temp to drop to in sleep
	uint8_t SleepTime;       // minutes timeout to sleep
	uint8_t cutoutSetting; // The voltage we cut out at for under voltage OR Power level for TS80
	uint8_t OrientationMode :2; // If true we want to invert the display for lefties
	uint8_t sensitivity :4;  // Sensitivity of accelerometer (5 bits)
	uint8_t autoStartMode :2;  // Should the unit automatically jump straight
							   // into soldering mode when power is applied
	uint8_t ShutdownTime;          // Time until unit shuts down if left alone
	uint8_t boostModeEnabled :1;  // Boost mode swaps BUT_A in soldering mode to
								  // temporary soldering temp over-ride
	uint8_t coolingTempBlink :1;  // Should the temperature blink on the cool
								  // down screen until its <50C
	uint8_t detailedIDLE :1;       // Detailed idle screen
	uint8_t detailedSoldering :1;  // Detailed soldering screens
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	uint8_t temperatureInF :1;       // Should the temp be in F or C (true is F)
#endif
	uint8_t descriptionScrollSpeed :1;  // Description scroll speed
	uint8_t KeepAwakePulse;  // Keep Awake pulse power in 0.1 watts (10 = 1Watt)

	uint16_t voltageDiv;                 // Voltage divisor factor
	uint16_t BoostTemp;                  // Boost mode set point for the iron
	uint16_t CalibrationOffset; // This stores the temperature offset for this tip
								// in the iron.

	uint8_t powerLimitEnable; // Allow toggling of power limit without changing value
	uint8_t powerLimit;       // Maximum power iron allowed to output

	uint16_t TipGain; // uV/C * 10, it can be used to convert tip thermocouple voltage to temperateture TipV/TipGain = TipTemp

	uint8_t ReverseButtonTempChangeEnabled; // Change the plus and minus button assigment
	uint16_t TempChangeLongStep;   // Change the plus and minus button assigment
	uint16_t TempChangeShortStep;  // Change the plus and minus button assigment

	uint32_t padding;  // This is here for in case we are not an even divisor so
					   // that nothing gets cut off
					   //MUST BE LAST

} systemSettingsType;

extern volatile systemSettingsType systemSettings;

void saveSettings();
bool restoreSettings();
uint8_t lookupVoltageLevel(uint8_t level);
void resetSettings();

#endif /* SETTINGS_H_ */
