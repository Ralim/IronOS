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
#define SETTINGSVERSION 0x10 /*Change this if you change the struct below to prevent people getting out of sync*/

/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from flash in uint16_t chunks
 */
typedef struct {
	uint16_t SolderingTemp; 		//current set point for the iron
	uint16_t SleepTemp; 			//temp to drop to in sleep
	uint8_t SleepTime; 				//minutes timeout to sleep
	uint8_t cutoutSetting;    	// The voltage we cut out at for under voltage
	uint8_t powerDisplay;    	//Toggle to swap the arrows with a power readout instead
	uint8_t OrientationMode;    	//If true we want to invert the display for lefties
	uint8_t sensitivity;			//Sensitivity of accelerometer (5 bits)
	uint8_t autoStartMode;    //Should the unit automatically jump straight into soldering mode when power is applied
	uint8_t ShutdownTime;		//Time until unit shuts down if left alone
	uint8_t boostModeEnabled;    //Boost mode swaps BUT_A in soldering mode to temporary soldering temp over-ride
	uint8_t coolingTempBlink;    //Should the temperature blink on the cool down screen until its <50C
	uint8_t advancedScreens;		//If enabled we draw more detailed screens with smaller fonts
	uint16_t voltageDiv;			//Voltage divisor factor
	uint16_t BoostTemp; 			//Boost mode set point for the iron
	int16_t CalibrationOffset;		//This stores the temperature offset for this tip in the iron.
	uint8_t version;				//Used to track if a reset is needed on firmware upgrade
	uint32_t padding;    			//This is here for in case we are not an even divisor so that nothing gets cut off
} systemSettingsType;

extern systemSettingsType systemSettings;

void saveSettings();
void restoreSettings();
uint8_t lookupVoltageLevel(uint8_t level);
void resetSettings();
void showBootLogoIfavailable();
#endif /* SETTINGS_H_ */
