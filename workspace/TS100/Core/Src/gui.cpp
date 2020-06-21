/*
 * gui.cpp
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#include "gui.hpp"
#include "Translation.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "TipThermoModel.h"
#include "string.h"
#include "unit.h"
#include "../../configuration.h"
#include "Buttons.hpp"

void gui_Menu(const menuitem *menu);

#ifdef MODEL_TS100
static void settings_setInputVRange(void);
static void settings_displayInputVRange(void);
#else
static void settings_setInputPRange(void);
static void settings_displayInputPRange(void);
#endif
static void settings_setSleepTemp(void);
static void settings_displaySleepTemp(void);
static void settings_setSleepTime(void);
static void settings_displaySleepTime(void);
static void settings_setShutdownTime(void);
static void settings_displayShutdownTime(void);
static void settings_setSensitivity(void);
static void settings_displaySensitivity(void);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
static void settings_setTempF(void);
static void settings_displayTempF(void);
#endif
static void settings_setAdvancedSolderingScreens(void);
static void settings_displayAdvancedSolderingScreens(void);
static void settings_setAdvancedIDLEScreens(void);
static void settings_displayAdvancedIDLEScreens(void);
static void settings_setScrollSpeed(void);
static void settings_displayScrollSpeed(void);
static void settings_setPowerLimitEnable(void);
static void settings_displayPowerLimitEnable(void);
static void settings_setPowerLimit(void);
static void settings_displayPowerLimit(void);
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
static void settings_setTipGain(void);
static void settings_displayTipGain(void);
static void settings_setCalibrateVIN(void);
static void settings_displayCalibrateVIN(void);
static void settings_displayReverseButtonTempChangeEnabled(void);
static void settings_setReverseButtonTempChangeEnabled(void);
static void settings_displayTempChangeShortStep(void);
static void settings_setTempChangeShortStep(void);
static void settings_displayTempChangeLongStep(void);
static void settings_setTempChangeLongStep(void);
static void settings_displayPowerPulse(void);
static void settings_setPowerPulse(void);

// Menu functions
static void settings_displaySolderingMenu(void);
static void settings_enterSolderingMenu(void);
static void settings_displayPowerMenu(void);
static void settings_enterPowerMenu(void);
static void settings_displayUIMenu(void);
static void settings_enterUIMenu(void);
static void settings_displayAdvancedMenu(void);
static void settings_enterAdvancedMenu(void);
/*
 * Root Settings Menu
 *
 * Power Source
 * Soldering
 * 	Boost Mode Enabled
 * 	Boost Mode Temp
 * 	Auto Start
 *  Temp change short step
 *  Temp change long step

 *
 * Power Saving
 * 	Sleep Temp
 * 	Sleep Time
 * 	Shutdown Time
 * 	Motion Sensitivity
 *
 * UI
 *  // Language
 *  Scrolling Speed
 *  Temperature Unit
 *  Display orientation
 *  Cooldown blink
 *  Reverse Temp change buttons + - 
 *
 * Advanced
 *  Enable Power Limit
 *  Power Limit
 *  Detailed IDLE
 *  Detailed Soldering
 *  Logo Time
 *  Calibrate Temperature
 *  Calibrate Input V
 *  Reset Settings
 *
 */
const menuitem rootSettingsMenu[] {
/*
 * Power Source
 * Soldering Menu
 * Power Saving Menu
 * UI Menu
 * Advanced Menu
 * Exit
 */
#ifdef MODEL_TS100
		{ (const char*) SettingsDescriptions[0], { settings_setInputVRange }, {
				settings_displayInputVRange } }, /*Voltage input*/
#else
		{ (const char*) SettingsDescriptions[20], { settings_setInputPRange }, {
				settings_displayInputPRange } }, /*Voltage input*/
#endif
		{ (const char*) NULL, { settings_enterSolderingMenu }, {
				settings_displaySolderingMenu } }, /*Soldering*/
		{ (const char*) NULL, { settings_enterPowerMenu }, {
				settings_displayPowerMenu } }, /*Sleep Options Menu*/
		{ (const char*) NULL, { settings_enterUIMenu },
				{ settings_displayUIMenu } }, /*UI Menu*/
		{ (const char*) NULL, { settings_enterAdvancedMenu }, {
				settings_displayAdvancedMenu } }, /*Advanced Menu*/
		{ NULL, { NULL }, { NULL } }        // end of menu marker. DO NOT REMOVE
};

const menuitem solderingMenu[] = {
/*
 * Boost Mode Enabled
 * 	Boost Mode Temp
 * 	Auto Start
 *  Temp change short step
 *  Temp change long step
 */
{ (const char*) SettingsDescriptions[8], { settings_setBoostModeEnabled }, {
		settings_displayBoostModeEnabled } }, /*Enable Boost*/
{ (const char*) SettingsDescriptions[9], { settings_setBoostTemp }, {
		settings_displayBoostTemp } }, /*Boost Temp*/
{ (const char*) SettingsDescriptions[10], { settings_setAutomaticStartMode }, {
		settings_displayAutomaticStartMode } }, /*Auto start*/
{ (const char*) SettingsDescriptions[24], { settings_setTempChangeShortStep }, {
		settings_displayTempChangeShortStep } }, /*Temp change short step*/
{ (const char*) SettingsDescriptions[25], { settings_setTempChangeLongStep }, {
		settings_displayTempChangeLongStep } }, /*Temp change long step*/
{ NULL, { NULL }, { NULL } }                // end of menu marker. DO NOT REMOVE
};
const menuitem UIMenu[] = {
/*
 // Language
 *  Scrolling Speed
 *  Temperature Unit
 *  Display orientation
 *  Cooldown blink
 *  Reverse Temp change buttons + - 
 */
#ifdef ENABLED_FAHRENHEIT_SUPPORT
		{ (const char*) SettingsDescriptions[5], { settings_setTempF }, {
				settings_displayTempF } }, /* Temperature units*/
#endif
		{ (const char*) SettingsDescriptions[7],
				{ settings_setDisplayRotation }, {
						settings_displayDisplayRotation } }, /*Display Rotation*/
		{ (const char*) SettingsDescriptions[11], {
				settings_setCoolingBlinkEnabled }, {
				settings_displayCoolingBlinkEnabled } }, /*Cooling blink warning*/
		{ (const char*) SettingsDescriptions[16], { settings_setScrollSpeed }, {
				settings_displayScrollSpeed } }, /*Scroll Speed for descriptions*/
		{ (const char*) SettingsDescriptions[23], {
				settings_setReverseButtonTempChangeEnabled }, {
				settings_displayReverseButtonTempChangeEnabled } }, /* Reverse Temp change buttons + - */
		{ NULL, { NULL }, { NULL } }        // end of menu marker. DO NOT REMOVE
};
const menuitem PowerMenu[] = {
/*
 * Sleep Temp
 * 	Sleep Time
 * 	Shutdown Time
 * 	Motion Sensitivity
 */
{ (const char*) SettingsDescriptions[1], { settings_setSleepTemp }, {
		settings_displaySleepTemp } }, /*Sleep Temp*/
{ (const char*) SettingsDescriptions[2], { settings_setSleepTime }, {
		settings_displaySleepTime } }, /*Sleep Time*/
{ (const char*) SettingsDescriptions[3], { settings_setShutdownTime }, {
		settings_displayShutdownTime } }, /*Shutdown Time*/
{ (const char*) SettingsDescriptions[4], { settings_setSensitivity }, {
		settings_displaySensitivity } }, /* Motion Sensitivity*/
{ NULL, { NULL }, { NULL } }           // end of menu marker. DO NOT REMOVE
};
const menuitem advancedMenu[] = {

/*
 *  Power limit enable
 *  Power limit
 *  Detailed IDLE
 *  Detailed Soldering
 *  Calibrate Temperature
 *  Calibrate Input V
 *  Reset Settings
 *  Power Pulse
 */
{ (const char*) SettingsDescriptions[21], { settings_setPowerLimitEnable }, {
		settings_displayPowerLimitEnable } }, /*Power limit enable*/
{ (const char*) SettingsDescriptions[22], { settings_setPowerLimit }, {
		settings_displayPowerLimit } }, /*Power limit*/
{ (const char*) SettingsDescriptions[6], { settings_setAdvancedIDLEScreens }, {
		settings_displayAdvancedIDLEScreens } }, /* Advanced idle screen*/
{ (const char*) SettingsDescriptions[15],
		{ settings_setAdvancedSolderingScreens }, {
				settings_displayAdvancedSolderingScreens } }, /* Advanced soldering screen*/
{ (const char*) SettingsDescriptions[13], { settings_setResetSettings }, {
		settings_displayResetSettings } }, /*Resets settings*/
{ (const char*) SettingsDescriptions[12], { settings_setCalibrate }, {
		settings_displayCalibrate } }, /*Calibrate tip*/
{ (const char*) SettingsDescriptions[14], { settings_setCalibrateVIN }, {
		settings_displayCalibrateVIN } }, /*Voltage input cal*/
{ (const char*) SettingsDescriptions[26], { settings_setPowerPulse }, {
		settings_displayPowerPulse } }, /*Power Pulse adjustment */
{ (const char*) SettingsDescriptions[27], { settings_setTipGain }, {
		settings_displayTipGain } }, /*TipGain*/
{ NULL, { NULL }, { NULL } }  // end of menu marker. DO NOT REMOVE
};

static void printShortDescriptionSingleLine(uint32_t shortDescIndex) {
	OLED::setFont(0);
	OLED::setCharCursor(0, 0);
	OLED::print(SettingsShortNames[shortDescIndex][0]);
}

static void printShortDescriptionDoubleLine(uint32_t shortDescIndex) {
	OLED::setFont(1);
	OLED::setCharCursor(0, 0);
	OLED::print(SettingsShortNames[shortDescIndex][0]);
	OLED::setCharCursor(0, 1);
	OLED::print(SettingsShortNames[shortDescIndex][1]);
}

/**
 * Prints two small lines of short description
 * and prepares cursor in big font after it.
 * @param shortDescIndex Index to of short description.
 * @param cursorCharPosition Custom cursor char position to set after printing
 * description.
 */
static void printShortDescription(uint32_t shortDescIndex,
		uint16_t cursorCharPosition) {
	// print short description (default single line, explicit double line)
	if (SettingsShortNameType == SHORT_NAME_DOUBLE_LINE) {
		printShortDescriptionDoubleLine(shortDescIndex);
	} else {
		printShortDescriptionSingleLine(shortDescIndex);
	}

	// prepare cursor for value
	OLED::setFont(0);
	OLED::setCharCursor(cursorCharPosition, 0);
	// make room for scroll indicator
	OLED::setCursor(OLED::getCursorX() - 2, 0);
}

static int userConfirmation(const char *message) {
	uint16_t messageWidth = FONT_12_WIDTH * (strlen(message) + 7);
	uint32_t messageStart = xTaskGetTickCount();

	OLED::setFont(0);
	OLED::setCursor(0, 0);
	int16_t lastOffset = -1;
	bool lcdRefresh = true;

	for (;;) {
		int16_t messageOffset = ((xTaskGetTickCount() - messageStart)
				/ (systemSettings.descriptionScrollSpeed == 1 ? 1 : 2));
		messageOffset %= messageWidth;  // Roll around at the end

		if (lastOffset != messageOffset) {
			OLED::clearScreen();

			//^ Rolling offset based on time
			OLED::setCursor((OLED_WIDTH - messageOffset), 0);
			OLED::print(message);
			lastOffset = messageOffset;
			lcdRefresh = true;
		}

		ButtonState buttons = getButtonState();
		switch (buttons) {
		case BUTTON_F_SHORT:
			// User confirmed
			return 1;

		case BUTTON_NONE:
			break;
		default:
		case BUTTON_BOTH:
		case BUTTON_B_SHORT:
		case BUTTON_F_LONG:
		case BUTTON_B_LONG:
			return 0;
		}

		if (lcdRefresh) {
			OLED::refresh();
			osDelay(40);
			lcdRefresh = false;
		}
	}
	return 0;
}
#ifdef MODEL_TS100
static void settings_setInputVRange(void) {
	systemSettings.cutoutSetting = (systemSettings.cutoutSetting + 1) % 5;
	if (systemSettings.cutoutSetting)
		systemSettings.powerLimitEnable = 0; // disable power limit if switching to a lipo power source
}

static void settings_displayInputVRange(void) {
	printShortDescription(0, 6);

	if (systemSettings.cutoutSetting) {
		OLED::printNumber(2 + systemSettings.cutoutSetting, 1);
		OLED::print(SymbolCellCount);
	} else {
		OLED::print(SymbolDC);
	}
}
#else
static void settings_setInputPRange(void) {
	systemSettings.cutoutSetting = (systemSettings.cutoutSetting + 1) % 2;
}

static void settings_displayInputPRange(void) {
	printShortDescription(0, 5);
	//0 = 9V, 1=12V (Fixed Voltages, these imply 1.5A limits)
	//2 = 18W, 2=24W (Auto Adjusting V, estimated from the tip resistance???) # TODO
	// Need to come back and look at these ^ as there were issues with voltage hunting
	switch (systemSettings.cutoutSetting) {
	case 0:
		OLED::printNumber(9, 2);
		OLED::print(SymbolVolts);
		break;
	case 1:
		OLED::printNumber(12, 2);
		OLED::print(SymbolVolts);
		break;
	default:
		break;
	}

}

#endif
static void settings_setSleepTemp(void) {
	// If in C, 10 deg, if in F 20 deg
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	if (systemSettings.temperatureInF) {
		systemSettings.SleepTemp += 20;
		if (systemSettings.SleepTemp > 580)
			systemSettings.SleepTemp = 60;
	} else
#endif
	{
		systemSettings.SleepTemp += 10;
		if (systemSettings.SleepTemp > 300)
			systemSettings.SleepTemp = 10;
	}
}

static void settings_displaySleepTemp(void) {
	printShortDescription(1, 5);
	OLED::printNumber(systemSettings.SleepTemp, 3);
}

static void settings_setSleepTime(void) {
	systemSettings.SleepTime++;  // Go up 1 minute at a time
	if (systemSettings.SleepTime >= 16) {
		systemSettings.SleepTime = 0;  // can't set time over 10 mins
	}
	// Remember that ^ is the time of no movement
	if (PCBVersion == 3)
		systemSettings.SleepTime = 0;  // Disable sleep on no accel
}

static void settings_displaySleepTime(void) {
	printShortDescription(2, 5);
	if (systemSettings.SleepTime == 0) {
		OLED::print(OffString);
	} else if (systemSettings.SleepTime < 6) {
		OLED::printNumber(systemSettings.SleepTime * 10, 2);
		OLED::print(SymbolSeconds);
	} else {
		OLED::printNumber(systemSettings.SleepTime - 5, 2);
		OLED::print(SymbolMinutes);
	}
}

static void settings_setShutdownTime(void) {
	systemSettings.ShutdownTime++;
	if (systemSettings.ShutdownTime > 60) {
		systemSettings.ShutdownTime = 0;  // wrap to off
	}
	if (PCBVersion == 3)
		systemSettings.ShutdownTime = 0;  // Disable shutdown on no accel
}

static void settings_displayShutdownTime(void) {
	printShortDescription(3, 5);
	if (systemSettings.ShutdownTime == 0) {
		OLED::print(OffString);
	} else {
		OLED::printNumber(systemSettings.ShutdownTime, 2);
		OLED::print(SymbolMinutes);
	}
}
#ifdef ENABLED_FAHRENHEIT_SUPPORT
static void settings_setTempF(void) {
	systemSettings.temperatureInF = !systemSettings.temperatureInF;
	if (systemSettings.temperatureInF) {
		// Change sleep, boost and soldering temps to the F equiv
		// C to F == F= ( (C*9) +160)/5
		systemSettings.BoostTemp = ((systemSettings.BoostTemp * 9) + 160) / 5;
		systemSettings.SolderingTemp =
				((systemSettings.SolderingTemp * 9) + 160) / 5;
		systemSettings.SleepTemp = ((systemSettings.SleepTemp * 9) + 160) / 5;
	} else {
		// Change sleep, boost and soldering temps to the C equiv
		// F->C == C = ((F-32)*5)/9
		systemSettings.BoostTemp = ((systemSettings.BoostTemp - 32) * 5) / 9;
		systemSettings.SolderingTemp = ((systemSettings.SolderingTemp - 32) * 5)
				/ 9;
		systemSettings.SleepTemp = ((systemSettings.SleepTemp - 32) * 5) / 9;
	}
	// Rescale both to be multiples of 10
	systemSettings.BoostTemp = systemSettings.BoostTemp / 10;
	systemSettings.BoostTemp *= 10;
	systemSettings.SolderingTemp = systemSettings.SolderingTemp / 10;
	systemSettings.SolderingTemp *= 10;
	systemSettings.SleepTemp = systemSettings.SleepTemp / 10;
	systemSettings.SleepTemp *= 10;
}

static void settings_displayTempF(void) {
	printShortDescription(5, 7);

	OLED::print((systemSettings.temperatureInF) ? SymbolDegF : SymbolDegC);
}
#endif

static void settings_setSensitivity(void) {
	systemSettings.sensitivity++;
	systemSettings.sensitivity = systemSettings.sensitivity % 10;
}

static void settings_displaySensitivity(void) {
	printShortDescription(4, 7);
	OLED::printNumber(systemSettings.sensitivity, 1, false);
}

static void settings_setAdvancedSolderingScreens(void) {
	systemSettings.detailedSoldering = !systemSettings.detailedSoldering;
}

static void settings_displayAdvancedSolderingScreens(void) {
	printShortDescription(15, 7);

	OLED::drawCheckbox(systemSettings.detailedSoldering);
}

static void settings_setAdvancedIDLEScreens(void) {
	systemSettings.detailedIDLE = !systemSettings.detailedIDLE;
}

static void settings_displayAdvancedIDLEScreens(void) {
	printShortDescription(6, 7);

	OLED::drawCheckbox(systemSettings.detailedIDLE);
}

static void settings_setPowerLimitEnable(void) {
	systemSettings.powerLimitEnable = !systemSettings.powerLimitEnable;
}

static void settings_displayPowerLimitEnable(void) {
	printShortDescription(21, 7);
	OLED::drawCheckbox(systemSettings.powerLimitEnable);
}

static void settings_setPowerLimit(void) {
	if (systemSettings.powerLimit >= MAX_POWER_LIMIT)
		systemSettings.powerLimit = POWER_LIMIT_STEPS;
	else
		systemSettings.powerLimit += POWER_LIMIT_STEPS;
}

static void settings_displayPowerLimit(void) {
	printShortDescription(22, 5);
	OLED::printNumber(systemSettings.powerLimit, 2);
	OLED::print(SymbolWatts);
}

static void settings_setScrollSpeed(void) {
	if (systemSettings.descriptionScrollSpeed == 0)
		systemSettings.descriptionScrollSpeed = 1;
	else
		systemSettings.descriptionScrollSpeed = 0;
}
static void settings_displayScrollSpeed(void) {
	printShortDescription(16, 7);
	OLED::print(
			(systemSettings.descriptionScrollSpeed) ?
					SettingFastChar : SettingSlowChar);
}

static void settings_setDisplayRotation(void) {
	systemSettings.OrientationMode++;
	systemSettings.OrientationMode = systemSettings.OrientationMode % 3;
	switch (systemSettings.OrientationMode) {
	case 0:
		OLED::setRotation(false);
		break;
	case 1:
		OLED::setRotation(true);
		break;
	case 2:
		// do nothing on auto
		break;
	default:
		break;
	}
}

static void settings_displayDisplayRotation(void) {
	printShortDescription(7, 7);

	switch (systemSettings.OrientationMode) {
	case 0:
		OLED::print(SettingRightChar);
		break;
	case 1:
		OLED::print(SettingLeftChar);
		break;
	case 2:
		OLED::print(SettingAutoChar);
		break;
	default:
		OLED::print(SettingRightChar);
		break;
	}
}

static void settings_setBoostModeEnabled(void) {
	systemSettings.boostModeEnabled = !systemSettings.boostModeEnabled;
}

static void settings_displayBoostModeEnabled(void) {
	printShortDescription(8, 7);

	OLED::drawCheckbox(systemSettings.boostModeEnabled);
}

static void settings_setBoostTemp(void) {
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	if (systemSettings.temperatureInF) {
		systemSettings.BoostTemp += 20;  // Go up 20F at a time
		if (systemSettings.BoostTemp > 850) {
			systemSettings.BoostTemp = 480;  // loop back at 250
		}
	} else
#endif
	{
		systemSettings.BoostTemp += 10;  // Go up 10C at a time
		if (systemSettings.BoostTemp > 450) {
			systemSettings.BoostTemp = 250;  // loop back at 250
		}
	}
}

static void settings_displayBoostTemp(void) {
	printShortDescription(9, 5);
	OLED::printNumber(systemSettings.BoostTemp, 3);
}

static void settings_setAutomaticStartMode(void) {
	systemSettings.autoStartMode++;
	systemSettings.autoStartMode %= 4;
}

static void settings_displayAutomaticStartMode(void) {
	printShortDescription(10, 7);

	switch (systemSettings.autoStartMode) {
	case 0:
		OLED::print(SettingStartNoneChar);
		break;
	case 1:
		OLED::print(SettingStartSolderingChar);
		break;
	case 2:
		OLED::print(SettingStartSleepChar);
		break;
	case 3:
		OLED::print(SettingStartSleepOffChar);
		break;
	default:
		OLED::print(SettingStartNoneChar);
		break;
	}
}

static void settings_setCoolingBlinkEnabled(void) {
	systemSettings.coolingTempBlink = !systemSettings.coolingTempBlink;
}

static void settings_displayCoolingBlinkEnabled(void) {
	printShortDescription(11, 7);

	OLED::drawCheckbox(systemSettings.coolingTempBlink);
}

static void settings_setResetSettings(void) {
	if (userConfirmation(SettingsResetWarning)) {
		resetSettings();

		OLED::setFont(0);
		OLED::setCursor(0, 0);
		OLED::print(ResetOKMessage);
		OLED::refresh();

		waitForButtonPressOrTimeout(200);  // 2 second timeout
	}
}

static void settings_displayResetSettings(void) {
	printShortDescription(13, 7);
}

static void setTipOffset() {
	systemSettings.CalibrationOffset = 0;

	// If the thermo-couple at the end of the tip, and the handle are at
	// equilibrium, then the output should be zero, as there is no temperature
	// differential.
	while (systemSettings.CalibrationOffset == 0) {
		uint32_t offset = 0;
		for (uint8_t i = 0; i < 16; i++) {
			offset += getTipRawTemp(1);
			// cycle through the filter a fair bit to ensure we're stable.
			OLED::clearScreen();
			OLED::setCursor(0, 0);
			OLED::print(SymbolDot);
			for (uint8_t x = 0; x < (i / 4); x++)
				OLED::print(SymbolDot);
			OLED::refresh();
			osDelay(100);
		}
		systemSettings.CalibrationOffset = TipThermoModel::convertTipRawADCTouV(
				offset / 16);
	}
	OLED::clearScreen();
	OLED::setCursor(0, 0);
	OLED::drawCheckbox(true);
	OLED::printNumber(systemSettings.CalibrationOffset, 4);
	OLED::refresh();
	osDelay(1200);
}

//Provide the user the option to tune their own tip if custom is selected
//If not only do single point tuning as per usual
static void settings_setCalibrate(void) {

	if (userConfirmation(SettingsCalibrationWarning)) {
		// User confirmed
		// So we now perform the actual calculation
		setTipOffset();
	}
}

static void settings_displayCalibrate(void) {
	printShortDescription(12, 5);
}

static void settings_setCalibrateVIN(void) {
	// Jump to the voltage calibration subscreen
	OLED::setFont(0);
	OLED::clearScreen();
	OLED::setCursor(0, 0);

	for (;;) {
		OLED::setCursor(0, 0);
		OLED::printNumber(getInputVoltageX10(systemSettings.voltageDiv, 0) / 10,
				2);
		OLED::print(SymbolDot);
		OLED::printNumber(getInputVoltageX10(systemSettings.voltageDiv, 0) % 10,
				1, false);
		OLED::print(SymbolVolts);

		ButtonState buttons = getButtonState();
		switch (buttons) {
		case BUTTON_F_SHORT:
			systemSettings.voltageDiv++;
			break;

		case BUTTON_B_SHORT:
			systemSettings.voltageDiv--;
			break;

		case BUTTON_BOTH:
		case BUTTON_F_LONG:
		case BUTTON_B_LONG:
			saveSettings();
			return;
			break;
		case BUTTON_NONE:
		default:
			break;
		}

		OLED::refresh();
		osDelay(40);

		// Cap to sensible values
#ifdef MODEL_TS80
		if (systemSettings.voltageDiv < 500) {
			systemSettings.voltageDiv = 500;
		} else if (systemSettings.voltageDiv > 900) {
			systemSettings.voltageDiv = 900;
		}
#else
		if (systemSettings.voltageDiv < 360) {
			systemSettings.voltageDiv = 360;
		} else if (systemSettings.voltageDiv > 520) {
			systemSettings.voltageDiv = 520;
		}
#endif
	}
}

static void settings_setTipGain(void) {
	OLED::setFont(0);
	OLED::clearScreen();

	for (;;) {
		OLED::setCursor(0, 0);
		OLED::printNumber(systemSettings.TipGain / 10, 2);
		OLED::print(SymbolDot);
		OLED::printNumber(systemSettings.TipGain % 10, 1);

		ButtonState buttons = getButtonState();
		switch (buttons) {
		case BUTTON_F_SHORT:
			systemSettings.TipGain -= 1;
			break;

		case BUTTON_B_SHORT:
			systemSettings.TipGain += 1;
			break;

		case BUTTON_BOTH:
		case BUTTON_F_LONG:
		case BUTTON_B_LONG:
			saveSettings();
			return;
			break;
		case BUTTON_NONE:
		default:
			break;
		}

		OLED::refresh();
		osDelay(40);

		// Cap to sensible values
		if (systemSettings.TipGain < 150) {
			systemSettings.TipGain = 150;
		} else if (systemSettings.TipGain > 300) {
			systemSettings.TipGain = 300;
		}
	}
}

static void settings_displayTipGain(void) {
	printShortDescription(27, 5);
}

static void settings_setReverseButtonTempChangeEnabled(void) {
	systemSettings.ReverseButtonTempChangeEnabled =
			!systemSettings.ReverseButtonTempChangeEnabled;
}

static void settings_displayReverseButtonTempChangeEnabled(void) {
	printShortDescription(23, 7);
	OLED::drawCheckbox(systemSettings.ReverseButtonTempChangeEnabled);
}

static void settings_setTempChangeShortStep(void) {
	systemSettings.TempChangeShortStep += TEMP_CHANGE_SHORT_STEP;
	if (systemSettings.TempChangeShortStep > TEMP_CHANGE_SHORT_STEP_MAX) {
		systemSettings.TempChangeShortStep = TEMP_CHANGE_SHORT_STEP; // loop back at TEMP_CHANGE_SHORT_STEP_MAX
	}
}
static void settings_displayTempChangeShortStep(void) {
	printShortDescription(24, 6);
	OLED::printNumber(systemSettings.TempChangeShortStep, 2);
}

static void settings_setTempChangeLongStep(void) {
	systemSettings.TempChangeLongStep += TEMP_CHANGE_LONG_STEP;
	if (systemSettings.TempChangeLongStep > TEMP_CHANGE_LONG_STEP_MAX) {
		systemSettings.TempChangeLongStep = TEMP_CHANGE_LONG_STEP; // loop back at TEMP_CHANGE_LONG_STEP_MAX
	}
}
static void settings_displayTempChangeLongStep(void) {
	printShortDescription(25, 6);
	OLED::printNumber(systemSettings.TempChangeLongStep, 2);
}

static void settings_setPowerPulse(void) {
	systemSettings.KeepAwakePulse += POWER_PULSE_INCREMENT;
	systemSettings.KeepAwakePulse %= POWER_PULSE_MAX;

}
static void settings_displayPowerPulse(void) {
	printShortDescription(26, 5);
	if (systemSettings.KeepAwakePulse) {
		OLED::printNumber(systemSettings.KeepAwakePulse / 10, 1);
		OLED::print(SymbolDot);
		OLED::printNumber(systemSettings.KeepAwakePulse % 10, 1);
	} else {
		OLED::drawCheckbox(false);
	}

}
static void displayMenu(size_t index) {
// Call into the menu
	OLED::setFont(1);
	OLED::setCursor(0, 0);
// Draw title
	OLED::print(SettingsMenuEntries[index]);
// Draw symbol
// 16 pixel wide image
// 2 pixel wide scrolling indicator
	OLED::drawArea(96 - 16 - 2, 0, 16, 16,
			(&SettingsMenuIcons[(16 * 2) * index]));
}

static void settings_displayCalibrateVIN(void) {
	printShortDescription(14, 5);
}
static void settings_displaySolderingMenu(void) {
	displayMenu(0);
}
static void settings_enterSolderingMenu(void) {
	gui_Menu(solderingMenu);
}
static void settings_displayPowerMenu(void) {
	displayMenu(1);
}
static void settings_enterPowerMenu(void) {
	gui_Menu(PowerMenu);
}
static void settings_displayUIMenu(void) {
	displayMenu(2);
}
static void settings_enterUIMenu(void) {
	gui_Menu(UIMenu);
}
static void settings_displayAdvancedMenu(void) {
	displayMenu(3);
}
static void settings_enterAdvancedMenu(void) {
	gui_Menu(advancedMenu);
}

void gui_Menu(const menuitem *menu) {
// Draw the settings menu and provide iteration support etc
	uint8_t currentScreen = 0;
	uint32_t autoRepeatTimer = 0;
	uint8_t autoRepeatAcceleration = 0;
	bool earlyExit = false;
	uint32_t descriptionStart = 0;
	int16_t lastOffset = -1;
	bool lcdRefresh = true;
	ButtonState lastButtonState = BUTTON_NONE;
	static bool enterGUIMenu = true;
	enterGUIMenu = true;
	uint8_t scrollContentSize = 0;

	for (uint8_t i = 0; menu[i].draw.func != NULL; i++) {
		scrollContentSize += 1;
	}

// Animated menu opening.
	if (menu[currentScreen].draw.func != NULL) {
		// This menu is drawn in a secondary framebuffer.
		// Then we play a transition from the current primary
		// framebuffer to the new buffer.
		// The extra buffer is discarded at the end of the transition.
		OLED::useSecondaryFramebuffer(true);
		OLED::setFont(0);
		OLED::setCursor(0, 0);
		OLED::clearScreen();
		menu[currentScreen].draw.func();
		OLED::useSecondaryFramebuffer(false);
		OLED::transitionSecondaryFramebuffer(true);
	}

	while ((menu[currentScreen].draw.func != NULL) && earlyExit == false) {
		OLED::setFont(0);
		OLED::setCursor(0, 0);
		// If the user has hesitated for >=3 seconds, show the long text
		// Otherwise "draw" the option
		if ((xTaskGetTickCount() - lastButtonTime < 300)
				|| menu[currentScreen].description == NULL) {
			OLED::clearScreen();
			menu[currentScreen].draw.func();
			uint8_t indicatorHeight = OLED_HEIGHT / scrollContentSize;
			uint8_t position = OLED_HEIGHT * currentScreen / scrollContentSize;
			OLED::drawScrollIndicator(position, indicatorHeight);
			lastOffset = -1;
			lcdRefresh = true;
		} else {
			// Draw description
			if (descriptionStart == 0)
				descriptionStart = xTaskGetTickCount();
			// lower the value - higher the speed
			int16_t descriptionWidth =
			FONT_12_WIDTH * (strlen(menu[currentScreen].description) + 7);
			int16_t descriptionOffset =
					((xTaskGetTickCount() - descriptionStart)
							/ (systemSettings.descriptionScrollSpeed == 1 ?
									1 : 2));
			descriptionOffset %= descriptionWidth;	// Roll around at the end
			if (lastOffset != descriptionOffset) {
				OLED::clearScreen();
				OLED::setCursor((OLED_WIDTH - descriptionOffset), 0);
				OLED::print(menu[currentScreen].description);
				lastOffset = descriptionOffset;
				lcdRefresh = true;
			}
		}

		ButtonState buttons = getButtonState();

		if (buttons != lastButtonState) {
			autoRepeatAcceleration = 0;
			lastButtonState = buttons;
		}

		switch (buttons) {
		case BUTTON_BOTH:
			earlyExit = true;  // will make us exit next loop
			descriptionStart = 0;
			break;
		case BUTTON_F_SHORT:
			// increment
			if (descriptionStart == 0) {
				if (menu[currentScreen].incrementHandler.func != NULL) {
					enterGUIMenu = false;
					menu[currentScreen].incrementHandler.func();

					if (enterGUIMenu) {
						OLED::useSecondaryFramebuffer(true);
						OLED::setFont(0);
						OLED::setCursor(0, 0);
						OLED::clearScreen();
						menu[currentScreen].draw.func();
						OLED::useSecondaryFramebuffer(false);
						OLED::transitionSecondaryFramebuffer(false);
					}
					enterGUIMenu = true;
				} else {
					earlyExit = true;
				}
			} else
				descriptionStart = 0;
			break;
		case BUTTON_B_SHORT:
			if (descriptionStart == 0)
				currentScreen++;
			else
				descriptionStart = 0;
			break;
		case BUTTON_F_LONG:
			if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration >
			PRESS_ACCEL_INTERVAL_MAX) {
				menu[currentScreen].incrementHandler.func();
				autoRepeatTimer = xTaskGetTickCount();
				descriptionStart = 0;

				autoRepeatAcceleration += PRESS_ACCEL_STEP;
			}
			break;
		case BUTTON_B_LONG:
			if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration >
			PRESS_ACCEL_INTERVAL_MAX) {
				currentScreen++;
				autoRepeatTimer = xTaskGetTickCount();
				descriptionStart = 0;

				autoRepeatAcceleration += PRESS_ACCEL_STEP;
			}
			break;
		case BUTTON_NONE:
		default:
			break;
		}

		if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration) <
		PRESS_ACCEL_INTERVAL_MIN) {
			autoRepeatAcceleration =
			PRESS_ACCEL_INTERVAL_MAX - PRESS_ACCEL_INTERVAL_MIN;
		}

		if (lcdRefresh) {
			OLED::refresh();  // update the LCD
			osDelay(40);
			lcdRefresh = false;
		}
		if ((xTaskGetTickCount() - lastButtonTime) > (100 * 30)) {
			// If user has not pressed any buttons in 30 seconds, exit back a menu layer
			// This will trickle the user back to the main screen eventually
			earlyExit = true;
			descriptionStart = 0;
		}
	}
}

void enterSettingsMenu() {
	gui_Menu(rootSettingsMenu);  // Call the root menu
	saveSettings();
}
