/*
 * Settings.c
 *
 *  Created on: 29 Sep 2016
 *      Author: Ralim
 *
 *      This file holds the users settings and saves / restores them to the devices flash
 */

#include "Settings.h"
#define FLASH_ADDR (0x8000000|48896)
void saveSettings() {
//First we erase the flash
	FLASH_Unlock(); //unlock flash writing
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	while (FLASH_ErasePage(FLASH_ADDR) != FLASH_COMPLETE)
		; //wait for it
	//erased the chunk
	//now we program it
	uint16_t *data = (uint16_t*) &systemSettings;
	for (uint8_t i = 0; i < (sizeof(systemSettings) / 2); i++) {
		FLASH_ProgramHalfWord(FLASH_ADDR + (i * 2), data[i]);
	}
}

void restoreSettings() {
	//We read the flash
	uint16_t *data = (uint16_t*) &systemSettings;
	for(uint8_t i=0;i<(sizeof(systemSettings)/2);i++)
	{
		data[i] = *(uint16_t *)(FLASH_ADDR + (i*2));
	}
	//if the version is correct were done
	//if not we reset and save
	if (systemSettings.version != SETTINGSVERSION) {
		//probably not setup
		resetSettings();
		saveSettings();
	}

}

void resetSettings() {

	systemSettings.SleepTemp = 900;
	systemSettings.SleepTime = 1;
	systemSettings.SolderingTemp = 3200;
	systemSettings.movementEnabled = 1; //we use movement detection
	systemSettings.cutoutVoltage = 9;
	systemSettings.version=SETTINGSVERSION;
}

