/*
 * gui.cpp
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#include "gui.h"
#include "string.h"
#include "hardware.h"
#include "cmsis_os.h"
static void settings_setInputVRange(void);
static void settings_displayInputVRange(void);
static void settings_setSleepTemp(void);
static void settings_displaySleepTemp(void);
static void settings_setSleepTime(void);
static void settings_displaySleepTime(void);
static void settings_setShutdownTime(void);
static void settings_displayShutdownTime(void);
static void settings_setSensitivity(void);
static void settings_displaySensitivity(void);
static void settings_setAdvancedScreens(void);
static void settings_displayAdvancedScreens(void);
static void settings_setDisplayRotation(void);
static void settings_displayDisplayRotation(void);
static void settings_setBoostModeEnabled(void);
static void settings_displayBoostModeEnabled(void);
static void settings_setBoostTemp(void);
static void settings_displayBoostTemp(void);
static void settings_setAutomaticStartMode(void);
static void settings_displayAutomaticStartMode(void);
static void settings_setCoolingBlinkEnabled(void);
static void settings_displayCoolingBlinkEnabled(void);
static void settings_setResetSettings(void);
static void settings_displayResetSettings(void);
static void settings_setCalibrate(void);
static void settings_displayCalibrate(void);

bool settingsResetRequest = false;
const menuitem settingsMenu[] = { /*Struct used for all settings options in the settings menu*/
{ (const char*) SettingsLongNames[0], { settings_setInputVRange }, { settings_displayInputVRange } },/*Voltage input*/
{ (const char*) SettingsLongNames[1], { settings_setSleepTemp }, { settings_displaySleepTemp } }, /*Sleep Temp*/
{ (const char*) SettingsLongNames[2], { settings_setSleepTime }, { settings_displaySleepTime } }, /*Sleep Time*/
{ (const char*) SettingsLongNames[3], { settings_setShutdownTime }, { settings_displayShutdownTime } }, /*Shutdown Time*/
{ (const char*) SettingsLongNames[4], { settings_setSensitivity }, { settings_displaySensitivity } },/* Motion Sensitivity*/
{ (const char*) SettingsLongNames[5], { settings_setAdvancedScreens }, { settings_displayAdvancedScreens } },/* Advanced screens*/
{ (const char*) SettingsLongNames[6], { settings_setDisplayRotation }, { settings_displayDisplayRotation } }, /**/
{ (const char*) SettingsLongNames[7], { settings_setBoostModeEnabled }, { settings_displayBoostModeEnabled } }, /**/
{ (const char*) SettingsLongNames[8], { settings_setBoostTemp }, { settings_displayBoostTemp } }, /**/
{ (const char*) SettingsLongNames[9], { settings_setAutomaticStartMode }, { settings_displayAutomaticStartMode } },/**/
{ (const char*) SettingsLongNames[10], { settings_setCoolingBlinkEnabled }, { settings_displayCoolingBlinkEnabled } }, /**/
{ (const char*) SettingsLongNames[11], { settings_setCalibrate }, { settings_displayCalibrate } }, /**/
{ (const char*) SettingsLongNames[12], { settings_setResetSettings }, { settings_displayResetSettings } }, /**/
{ NULL, { NULL }, { NULL } }    //end of menu marker. DO NOT REMOVE
};

static void settings_setInputVRange(void) {
	systemSettings.cutoutSetting = (systemSettings.cutoutSetting + 1) % 5;
}
static void settings_displayInputVRange(void) {
	lcd.print(SettingsShortNames[0]);
	if (systemSettings.cutoutSetting) {
		lcd.drawChar('0' + 2 + systemSettings.cutoutSetting);
		lcd.drawChar('S');
	} else {
		lcd.print("DC");
	}
}

static void settings_setSleepTemp(void) {
	systemSettings.SleepTemp += 10;
	if (systemSettings.SleepTemp > 300)
		systemSettings.SleepTemp = 50;
}
static void settings_displaySleepTemp(void) {
	lcd.print(SettingsShortNames[1]);
	lcd.printNumber(systemSettings.SleepTemp, 3);
}
static void settings_setSleepTime(void) {
	++systemSettings.SleepTime;    //Go up 1 minute at a time
	if (systemSettings.SleepTime > 16)
		systemSettings.SleepTime = 1;    //can't set time over 30 mins
//Remember that ^ is the time of no movement
}
static void settings_displaySleepTime(void) {
	lcd.print(SettingsShortNames[2]);
	if (systemSettings.SleepTime < 6) {
		lcd.printNumber(systemSettings.SleepTime * 10, 2);
		lcd.drawChar('S');
	} else {
		lcd.printNumber(systemSettings.SleepTime - 5, 2);
		lcd.drawChar('M');
	}
}
static void settings_setShutdownTime(void) {
	++systemSettings.ShutdownTime;
	if (systemSettings.ShutdownTime > 60)
		systemSettings.ShutdownTime = 0;    //wrap to off
}
static void settings_displayShutdownTime(void) {
	lcd.print(SettingsShortNames[3]);
	lcd.printNumber(systemSettings.ShutdownTime, 2);
}
static void settings_setSensitivity(void) {
	systemSettings.sensitivity++;
	systemSettings.sensitivity = systemSettings.sensitivity % 10;
}
static void settings_displaySensitivity(void) {
	lcd.print(SettingsShortNames[4]);
	lcd.printNumber(systemSettings.sensitivity, 1);
}

static void settings_setAdvancedScreens(void) {
	systemSettings.advancedScreens = !systemSettings.advancedScreens;
}
static void settings_displayAdvancedScreens(void) {
	lcd.print(SettingsShortNames[5]);
	if (systemSettings.advancedScreens)
		lcd.drawChar('T');
	else
		lcd.drawChar('F');
}
static void settings_setDisplayRotation(void) {
	systemSettings.OrientationMode++;
	systemSettings.OrientationMode = systemSettings.OrientationMode % 3;
}
static void settings_displayDisplayRotation(void) {
	lcd.print(SettingsShortNames[6]);
	switch (systemSettings.OrientationMode) {
		case 0:
			lcd.drawChar('R');
			break;
		case 1:
			lcd.drawChar('L');
			break;
		case 2:
			lcd.drawChar('A');
			break;
	}

}
static void settings_setBoostModeEnabled(void) {
	systemSettings.boostModeEnabled = !systemSettings.boostModeEnabled;
}
static void settings_displayBoostModeEnabled(void) {
	lcd.print(SettingsShortNames[7]);
	if (systemSettings.boostModeEnabled)
		lcd.drawChar('T');
	else
		lcd.drawChar('F');
}
static void settings_setBoostTemp(void) {
	systemSettings.BoostTemp += 10;    //Go up 10C at a time
	if (systemSettings.BoostTemp > 450)
		systemSettings.BoostTemp = 250;    //loop back at 250
}
static void settings_displayBoostTemp(void) {
	lcd.print(SettingsShortNames[8]);
	lcd.printNumber(systemSettings.BoostTemp, 3);
}
static void settings_setAutomaticStartMode(void) {
	systemSettings.autoStartMode++;
	systemSettings.autoStartMode %= 2;
}
static void settings_displayAutomaticStartMode(void) {
	lcd.print(SettingsShortNames[9]);
	switch (systemSettings.autoStartMode) {
		case 0:
			lcd.drawChar('F');
			break;
		case 1:
			lcd.drawChar('T');
			break;

	}

}
static void settings_setCoolingBlinkEnabled(void) {
	systemSettings.coolingTempBlink = !systemSettings.coolingTempBlink;
}
static void settings_displayCoolingBlinkEnabled(void) {
	lcd.print(SettingsShortNames[10]);
	if (systemSettings.coolingTempBlink)
		lcd.drawChar('T');
	else
		lcd.drawChar('F');
}
static void settings_setResetSettings(void) {
	settingsResetRequest = !settingsResetRequest;
}
static void settings_displayResetSettings(void) {
	lcd.print(SettingsShortNames[12]);
	if (settingsResetRequest)
		lcd.drawChar('T');
	else
		lcd.drawChar('F');
}

static void settings_setCalibrate(void) {
	//Calibrate the offset
	//We split off here to confirm with the user
	uint8_t maxOffset = strlen(SettingsCalibrationWarning);
	uint32_t descriptionStart = HAL_GetTick();
	lcd.setFont(0);
	lcd.clearScreen();
	lcd.setCursor(0, 0);
	for (;;) {

		int16_t descriptionOffset = ((HAL_GetTick() - descriptionStart) / 150) % maxOffset;
		lcd.setCursor(12 * (7 - descriptionOffset), 0);
		lcd.print(SettingsCalibrationWarning);
		ButtonState buttons = getButtonState();

		switch (buttons) {
			case BUTTON_F_SHORT: {
				//User confirmed
				//So we now perform the actual calculation
				lcd.clearScreen();
				lcd.setCursor(0, 0);
				lcd.print(".....");
				lcd.refresh();
				setCalibrationOffset(0);				//turn off the current offset
				for (uint8_t i = 0; i < 20; i++) {
					getTipRawTemp(1);				//cycle through the filter a fair bit to ensure were stable.
					osDelay(20);
				}
				osDelay(100);
				uint16_t rawTempC = tipMeasurementToC(getTipRawTemp(0));
				//We now measure the current reported tip temperature
				uint16_t handleTempC = getHandleTemperature() / 10;
				//We now have an error between these that we want to store as the offset
				rawTempC = rawTempC - handleTempC;
				systemSettings.CalibrationOffset = rawTempC;
				setCalibrationOffset(rawTempC);				//store the error
				osDelay(100);
				return;
			}
				break;
			case BUTTON_BOTH:
			case BUTTON_B_SHORT:
			case BUTTON_F_LONG:
			case BUTTON_B_LONG:
				return;
				break;
			case BUTTON_NONE:
				break;
		}
		lcd.refresh();
		osDelay(50);
	}

}
static void settings_displayCalibrate(void) {
	lcd.print(SettingsShortNames[11]);
}
