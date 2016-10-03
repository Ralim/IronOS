/*
 * Settings.c
 *
 *  Created on: 29 Sep 2016
 *      Author: Ralim
 *
 *      This file holds the users settings and saves / restores them to the devices flash
 */

#include "Settings.h"
#define FLASH_ADDR (0x8000000|48896)/*Flash start OR'ed with the maximum amount of flash - 256 bytes*/
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
	for (uint8_t i = 0; i < (sizeof(systemSettings) / 2); i++) {
		data[i] = *(uint16_t *) (FLASH_ADDR + (i * 2));
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

	systemSettings.SleepTemp = 1500;			//Temperature the iron sleeps at - default 150.0 C
	systemSettings.SleepTime = 1;				//How many minutes we wait until going to sleep - default 1 min
	systemSettings.SolderingTemp = 3200;		//Default soldering temp is 320.0 C
	systemSettings.movementEnabled = 1; 		//we use movement detection by default
	systemSettings.cutoutVoltage = 10;			//10V is the minium cutout voltage as the unit V measurement is unstable below 9.5V
	systemSettings.version = SETTINGSVERSION;	//Store the version number to allow for easier upgrades
	systemSettings.displayTempInF =0;			//default to C
	systemSettings.flipDisplay=0;				//Default to right handed mode
}

