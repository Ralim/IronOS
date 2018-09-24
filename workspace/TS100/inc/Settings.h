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
#define SETTINGSVERSION 0x15 /*Change this if you change the struct below to prevent people getting out of sync*/

/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from flash in uint16_t chunks
 */
typedef struct {
	uint16_t SolderingTemp; 			//current set point for the iron
	uint16_t SleepTemp; 				//temp to drop to in sleep
	uint8_t SleepTime; 					//minutes timeout to sleep
	uint8_t cutoutSetting;   	 		// The voltage we cut out at for under voltage
	uint8_t OrientationMode:2; 	 	  	//If true we want to invert the display for lefties
	uint8_t sensitivity :4;				//Sensitivity of accelerometer (5 bits)
	uint8_t autoStartMode :2;  			//Should the unit automatically jump straight into soldering mode when power is applied
	uint8_t ShutdownTime;				//Time until unit shuts down if left alone
	uint8_t boostModeEnabled :1;	    //Boost mode swaps BUT_A in soldering mode to temporary soldering temp over-ride
	uint8_t coolingTempBlink :1;	    //Should the temperature blink on the cool down screen until its <50C
	uint8_t detailedIDLE :1;			//Detailed idle screen
	uint8_t detailedSoldering :1;		//Detailed soldering screens
	uint8_t temperatureInF;				//Should the temp be in F or C (true is F)
	uint8_t descriptionScrollSpeed:1; 	// Description scroll speed
	uint16_t voltageDiv;				//Voltage divisor factor
	uint16_t BoostTemp; 				//Boost mode set point for the iron
	int16_t CalibrationOffset;			//This stores the temperature offset for this tip in the iron.
	uint8_t PID_P;						//PID P Term
	uint8_t PID_I;						//PID I Term
	uint8_t PID_D;						//PID D Term
	uint8_t version;					//Used to track if a reset is needed on firmware upgrade
	uint8_t customTipGain;				// Tip gain value if custom tuned, or 0 if using a tipType param
	uint8_t tipType;
	uint32_t padding;    				//This is here for in case we are not an even divisor so that nothing gets cut off
} systemSettingsType;

extern volatile systemSettingsType systemSettings;

void saveSettings();
void restoreSettings();
uint8_t lookupVoltageLevel(uint8_t level);
void resetSettings();
bool showBootLogoIfavailable();
#endif /* SETTINGS_H_ */
