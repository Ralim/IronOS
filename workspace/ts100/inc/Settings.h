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
#define SETTINGSVERSION 0x02 /*Change this if you change the struct below to prevent people getting out of sync*/
#define SETTINGSOPTIONSCOUNT 5 /*Number of settings in the settings menu*/
/*
 * This struct must be a multiple of 2 bytes as it is saved / restored from flash in uint16_t chunks
 */
struct {
	uint32_t SolderingTemp; 	//current setpoint for the iron
	uint32_t SleepTemp; 		//temp to drop to in sleep
	uint8_t version;			//Used to track if a reset is needed on firmware upgrade
	uint8_t SleepTime; 			//minutes timeout to sleep
	uint8_t cutoutVoltage; 		//The voltage we cutout at for undervoltage
	uint8_t movementEnabled;	//If movement is enabled
	uint8_t displayTempInF;		//If we need to convert the C reading to F
	uint8_t flipDisplay;		//If true we want to invert the display for lefties
} systemSettings;

void saveSettings();
void restoreSettings();
void resetSettings();
#endif /* SETTINGS_H_ */
