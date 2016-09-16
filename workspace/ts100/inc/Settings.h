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
#define SETTINGSVERSION 0x01 /*Change this if you change the struct below to prevent people getting out of sync*/
struct {
	uint32_t SolderingTemp; //current setpoint for the iron
	uint32_t SleepTemp; //temp to drop to in sleep
	uint8_t SleepTime; //minutes to sleep
	uint8_t cutoutVoltage; //X10 the voltage we cutout at for undervoltage
	uint8_t movementEnabled;
	uint8_t version;
} systemSettings;
//Settings struct used for user settings

void saveSettings();
void restoreSettings();
void resetSettings();
#endif /* SETTINGS_H_ */
