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
#include "Setup.h"
#define FLASH_ADDR \
  (0x8000000 |     \
   0xFC00) /*Flash start OR'ed with the maximum amount of flash - 1024 bytes*/
#include "string.h"
volatile systemSettingsType systemSettings;

void saveSettings() {
	// First we erase the flash
	FLASH_EraseInitTypeDef pEraseInit;
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.Banks = FLASH_BANK_1;
	pEraseInit.NbPages = 1;
	pEraseInit.PageAddress = FLASH_ADDR;
	uint32_t failingAddress = 0;
	HAL_IWDG_Refresh(&hiwdg);
	__HAL_FLASH_CLEAR_FLAG(
			FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY);
	HAL_FLASH_Unlock();
	HAL_Delay(10);
	HAL_IWDG_Refresh(&hiwdg);
	HAL_FLASHEx_Erase(&pEraseInit, &failingAddress);
	//^ Erase the page of flash (1024 bytes on this stm32)
	// erased the chunk
	// now we program it
	uint16_t *data = (uint16_t*) &systemSettings;
	HAL_FLASH_Unlock();

	for (uint8_t i = 0; i < (sizeof(systemSettingsType) / 2); i++) {
		HAL_IWDG_Refresh(&hiwdg);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_ADDR + (i * 2),
				data[i]);
	}
	HAL_FLASH_Lock();
}

bool restoreSettings() {
	// We read the flash
	uint16_t *data = (uint16_t*) &systemSettings;
	for (uint8_t i = 0; i < (sizeof(systemSettingsType) / 2); i++) {
		data[i] = *((uint16_t*) (FLASH_ADDR + (i * 2)));
	}

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
uint8_t lookupVoltageLevel(uint8_t level) {
	if (level == 0)
		return 90;  // 9V since iron does not function effectively below this
	else
		return (level * 33) + (33 * 2);
}
void resetSettings() {
	memset((void*) &systemSettings, 0, sizeof(systemSettingsType));
	systemSettings.SleepTemp = 150; // Temperature the iron sleeps at - default 150.0 C
	systemSettings.SleepTime = 6; // How many seconds/minutes we wait until going
								  // to sleep - default 1 min
	systemSettings.SolderingTemp = 320;  // Default soldering temp is 320.0 C
	systemSettings.cutoutSetting = 0; // default to no cut-off voltage (or 18W for TS80)
	systemSettings.version =
	SETTINGSVERSION;  // Store the version number to allow for easier upgrades
	systemSettings.detailedSoldering = 0;  // Detailed soldering screen
	systemSettings.detailedIDLE = 0; // Detailed idle screen (off for first time users)
	systemSettings.OrientationMode = 2;  // Default to automatic
	systemSettings.sensitivity = 7;      // Default high sensitivity
#ifdef MODEL_TS80
	systemSettings.voltageDiv = 780;  // Default divider from schematic

#else
  systemSettings.voltageDiv = 467;  // Default divider from schematic
#endif
	systemSettings.ShutdownTime = 10; // How many minutes until the unit turns itself off
	systemSettings.boostModeEnabled = 1; // Default to having boost mode on as most people prefer it
	systemSettings.BoostTemp = 420;    // default to 400C
	systemSettings.autoStartMode = 0;  // Auto start off for safety
	systemSettings.coolingTempBlink = 0; // Blink the temperature on the cooling screen when its > 50C
	systemSettings.temperatureInF = 0;          // default to 0
	systemSettings.descriptionScrollSpeed = 0;  // default to slow
  systemSettings.powerLimitEnable = 0; // Default to no power limit
#ifdef MODEL_TS100
	systemSettings.CalibrationOffset = 900;  // the adc offset in uV
	systemSettings.pidPowerLimit = 70;  // Sets the max pwm power limit
	systemSettings.powerLimit = 30; // 30 watts default limit
#endif
#ifdef MODEL_TS80
	systemSettings.pidPowerLimit = 24;  // Sets the max pwm power limit
	systemSettings.CalibrationOffset = 900;  // the adc offset in uV
	systemSettings.powerLimit = 24; // 24 watts default power limit
#endif
	saveSettings();  // Save defaults
}
