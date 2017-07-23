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
#include "stm32f10x_flash.h"
#define SETTINGSVERSION 13 /*Change this if you change the struct below to prevent people getting out of sync*/
//Display Speeds
#define DISPLAYMODE_FAST 	(0x00)
#define DISPLAYMODE_MEDIUM 	(0x01)
#define DISPLAYMODE_SLOW	(0x02)
//Rounding Modes
#define ROUNDING_NONE			(0x00)
#define ROUNDING_FIVE			(0x01)
#define ROUNDING_TEN			(0x02)

/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from flash in uint16_t chunks
 */
struct {
	uint16_t SolderingTemp; 		//current set point for the iron
	uint32_t SleepTemp; 			//temp to drop to in sleep
	uint8_t version;				//Used to track if a reset is needed on firmware upgrade
	uint8_t SleepTime; 				//minutes timeout to sleep
	uint8_t cutoutSetting:5; 		//(3 bits) The voltage we cut out at for under voltage
	uint8_t displayTempInF:1;		//If we need to convert the C reading to F
	uint8_t flipDisplay:1;			//If true we want to invert the display for lefties
	uint8_t sensitivity:6;			//Sensitivity of accelerometer (5 bits)
	uint8_t ShutdownTime:6;			//Time until unit shuts down if left alone
	uint8_t displayUpdateSpeed:2;	//How fast the display updates / temp showing mode
	uint8_t temperatureRounding:2;	//Rounding mode for the temperature
	uint8_t boostModeEnabled:1;		//Boost mode swaps BUT_A in soldering mode to temporary soldering temp over-ride
	uint16_t tempCalibration;		//Temperature calibration value
	uint16_t voltageDiv;			//Voltage divisor factor
	uint16_t BoostTemp; 			//Boost mode set point for the iron
} systemSettings;

void saveSettings();
void restoreSettings();
uint8_t lookupVoltageLevel(uint8_t level);
void resetSettings();
void showBootLogoIfavailable();
#endif /* SETTINGS_H_ */
