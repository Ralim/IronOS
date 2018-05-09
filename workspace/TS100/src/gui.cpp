/*
 * gui.cpp
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#include "gui.hpp"
#include "main.hpp"
#include "cmsis_os.h"
#include "Translation.h"

#include "string.h"
extern uint32_t lastButtonTime;
void gui_Menu(const menuitem* menu);
static bool settings_setPowerSourceType(void);
static void settings_displayPowerSourceType(void);
static bool settings_setBatteryCellCount(void);
static void settings_displayBatteryCellCount(void);
static bool settings_setBatteryCellVoltage(void);
static void settings_displayBatteryCellVoltage(void);
static bool settings_setPowerSourceCustom(void);
static void settings_displayPowerSourceCustom(void);
static bool settings_setSleepTemp(void);
static void settings_displaySleepTemp(void);
static bool settings_setSleepTime(void);
static void settings_displaySleepTime(void);
static bool settings_setShutdownTime(void);
static void settings_displayShutdownTime(void);
static bool settings_setSensitivity(void);
static void settings_displaySensitivity(void);
static bool settings_setTempF(void);
static void settings_displayTempF(void);
static bool settings_setAdvancedSolderingScreens(void);
static void settings_displayAdvancedSolderingScreens(void);
static bool settings_setAdvancedIDLEScreens(void);
static void settings_displayAdvancedIDLEScreens(void);
static bool settings_setScrollSpeed(void);
static void settings_displayScrollSpeed(void);

static bool settings_setDisplayRotation(void);
static void settings_displayDisplayRotation(void);
static bool settings_setBoostModeEnabled(void);
static void settings_displayBoostModeEnabled(void);
static bool settings_setBoostTemp(void);
static void settings_displayBoostTemp(void);
static bool settings_setAutomaticStartMode(void);
static void settings_displayAutomaticStartMode(void);
static bool settings_setCoolingBlinkEnabled(void);
static void settings_displayCoolingBlinkEnabled(void);
static bool settings_setResetSettings(void);
static void settings_displayResetSettings(void);
static bool settings_setCalibrate(void);
static void settings_displayCalibrate(void);
static bool settings_setCalibrateVIN(void);
static void settings_displayCalibrateVIN(void);

//Menu functions
static void settings_displaySolderingMenu(void);
static bool settings_enterSolderingMenu(void);
static void settings_displayPowerMenu(void);
static bool settings_enterPowerMenu(void);
static void settings_displayPowerSourceMenu(void);
static bool settings_enterPowerSourceMenu(void);
static void settings_displayUIMenu(void);
static bool settings_enterUIMenu(void);
static void settings_displayAdvancedMenu(void);
static bool settings_enterAdvancedMenu(void);


static bool settings_exitMenuLevel(void) {
	return true;
}
const menuitem END_OF_MENU = {NULL, {settings_exitMenuLevel}, {NULL}};

/*
 * Root Settings Menu
 *
 * Power Source
 *  DC (9V)
 *  Lithium battery
 *   - cell count
 *   - voltage per cell
 *  Custom
 *
 * Soldering
 *  Boost Mode Enabled
 *  Boost Mode Temp
 *  Auto Start
 *
 * Power Saving
 *  Sleep Temp
 *  Sleep Time
 *  Shutdown Time
 *  Motion Sensitivity
 *
 * UI
 *  // Language
 *  Scrolling Speed
 *  Temperature Unit
 *  Display orientation
 *  Cooldown blink
 *
 * Advanced
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
 * Power Source Menu
 * Soldering Menu
 * Power Saving Menu
 * UI Menu
 * Advanced Menu
 * Exit
 */
//XXX
//{ (const char*) SettingsDescriptions[0], { settings_setInputVRange }, {
//		settings_displayInputVRange } }, /*Voltage input*/
{ (const char*) SettingsMenuEntriesDescriptions[4], { settings_enterPowerSourceMenu }, {
		settings_displayPowerSourceMenu } }, /*Power Source Menu*/
{ (const char*) SettingsMenuEntriesDescriptions[0], {
		settings_enterSolderingMenu }, { settings_displaySolderingMenu } }, /*Soldering*/
{ (const char*) SettingsMenuEntriesDescriptions[1], { settings_enterPowerMenu },
		{ settings_displayPowerMenu } }, /*Sleep Options Menu*/
{ (const char*) SettingsMenuEntriesDescriptions[2], { settings_enterUIMenu }, {
		settings_displayUIMenu } }, /*UI Menu*/
{ (const char*) SettingsMenuEntriesDescriptions[3],
		{ settings_enterAdvancedMenu }, { settings_displayAdvancedMenu } }, /*Advanced Menu*/
END_OF_MENU            // end of menu marker. DO NOT REMOVE
};

const menuitem solderingMenu[] = {
/*
 * Boost Mode Enabled
 * 	Boost Mode Temp
 * 	Auto Start
 */
{ (const char*) SettingsDescriptions[8], { settings_setBoostModeEnabled }, {
		settings_displayBoostModeEnabled } }, /*Enable Boost*/
{ (const char*) SettingsDescriptions[9], { settings_setBoostTemp }, {
		settings_displayBoostTemp } }, /*Boost Temp*/
{ (const char*) SettingsDescriptions[10], { settings_setAutomaticStartMode }, {
		settings_displayAutomaticStartMode } }, /*Auto start*/
END_OF_MENU            // end of menu marker. DO NOT REMOVE
};

const menuitem UIMenu[] = {
/*
 * Language
 *  Scrolling Speed
 *  Temperature Unit
 *  Display orientation
 *  Cooldown blink
 */
{ (const char*) SettingsDescriptions[5], { settings_setTempF }, {
		settings_displayTempF } }, /* Temperature units*/
{ (const char*) SettingsDescriptions[7], { settings_setDisplayRotation }, {
		settings_displayDisplayRotation } }, /*Display Rotation*/
{ (const char*) SettingsDescriptions[11], { settings_setCoolingBlinkEnabled }, {
		settings_displayCoolingBlinkEnabled } }, /*Cooling blink warning*/
{ (const char*) SettingsDescriptions[16], { settings_setScrollSpeed }, {
		settings_displayScrollSpeed } }, /*Scroll Speed for descriptions*/
END_OF_MENU            // end of menu marker. DO NOT REMOVE
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
END_OF_MENU            // end of menu marker. DO NOT REMOVE
};

const menuitem advancedMenu[] = {
/*
 * Detailed IDLE
 *  Detailed Soldering
 *  Logo Time
 *  Calibrate Temperature
 *  Calibrate Input V
 *  Reset Settings
 */
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
END_OF_MENU            // end of menu marker. DO NOT REMOVE
};

const menuitem PowerSourceMenu[][4] = {	//XXX array dimensions?
//DC (9V)
{
	{(const char*) SettingsDescriptions[17], {settings_setPowerSourceType}, {settings_displayPowerSourceType}},
	END_OF_MENU,
	END_OF_MENU,
	END_OF_MENU            // end of menu marker. DO NOT REMOVE
},
//Lithium battery
// - cell count
// - voltage per cell
{
	{(const char*) SettingsDescriptions[18], {settings_setPowerSourceType}, {settings_displayPowerSourceType}},
	{(const char*) SettingsDescriptions[19], {settings_setBatteryCellCount}, {settings_displayBatteryCellCount}},
	{(const char*) SettingsDescriptions[20], {settings_setBatteryCellVoltage}, {settings_displayBatteryCellVoltage}},
	END_OF_MENU            // end of menu marker. DO NOT REMOVE
},
//Custom
{
	{(const char*) SettingsDescriptions[21], {settings_setPowerSourceType}, {settings_displayPowerSourceType}},
	{(const char*) SettingsDescriptions[22], {settings_setPowerSourceCustom}, {settings_displayPowerSourceCustom}},
	END_OF_MENU,
	END_OF_MENU            // end of menu marker. DO NOT REMOVE
}
};

static void printShortDescriptionSingleLine(uint32_t shortDescIndex) {
	lcd.setFont(0);
	lcd.setCharCursor(0, 0);
	lcd.print(SettingsShortNames[shortDescIndex][0]);
}

static void printShortDescriptionDoubleLine(uint32_t shortDescIndex) {
	lcd.setFont(1);
	lcd.setCharCursor(0, 0);
	lcd.print(SettingsShortNames[shortDescIndex][0]);
	lcd.setCharCursor(0, 1);
	lcd.print(SettingsShortNames[shortDescIndex][1]);
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
	lcd.setFont(0);
	lcd.setCharCursor(cursorCharPosition, 0);
}

static int userConfirmation(const char* message) {
	uint16_t messageWidth = FONT_12_WIDTH * (strlen(message) + 7);
	uint32_t messageStart = xTaskGetTickCount();

	lcd.setFont(0);
	lcd.setCursor(0, 0);
	int16_t lastOffset = -1;
	bool lcdRefresh = true;

	for (;;) {

		int16_t messageOffset = ((xTaskGetTickCount() - messageStart)
				/ (systemSettings.descriptionScrollSpeed == 1 ? 1 : 2));
		messageOffset %= messageWidth;		//Roll around at the end

		if (lastOffset != messageOffset) {
			lcd.clearScreen();

			//^ Rolling offset based on time
			lcd.setCursor((OLED_WIDTH - messageOffset), 0);
			lcd.print(message);
			lastOffset = messageOffset;
			lcdRefresh = true;
		}

		ButtonState buttons = getButtonState();
		switch (buttons) {
		case BUTTON_F_SHORT:
			//User confirmed
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
			lcd.refresh();
			osDelay(20);
			lcdRefresh = false;
		}
	}
	return 0;
}

//TODO copied from main.cpp
void printVoltageCutout(uint16_t voltageX10) {
	lcd.printNumber(voltageX10 / 10, 2);
	lcd.drawChar('.');
	lcd.printNumber(voltageX10 % 10, 1);
	lcd.drawChar('V');
}

//XXX
static bool settings_setPowerSourceType(void) {
	systemSettings.cutoutSetting = (systemSettings.cutoutSetting + 1) % 3;
	return true;
}
static void settings_displayPowerSourceType(void) {
	switch(systemSettings.cutoutSetting) {
		//DC
		case 0:
			printShortDescription(17, 0);
			break;
		//Battery
		case 1:
			printShortDescription(18, 0);
			break;
		//Custom
		case 2:
			printShortDescription(19, 0);
			break;
	}
	lcd.setFont(1);
	lcd.setCharCursor(10, 1);
	printVoltageCutout(lookupVoltageLevel(systemSettings.cutoutSetting));
}

static bool settings_setBatteryCellCount(void) {
	//3S-6S
	if(++systemSettings.cutoutCellCount > 6) {
		systemSettings.cutoutCellCount = 3;
	}
	//ensure minimum 9V
	if(systemSettings.cutoutCellCount == 3 && systemSettings.cutoutCellVoltage < 30) {
		systemSettings.cutoutCellVoltage = 30;
	}
	return false;
}
static void settings_displayBatteryCellCount(void) {
	printShortDescription(20, 7);
	lcd.drawChar('0' + systemSettings.cutoutCellCount);
}

static bool settings_setBatteryCellVoltage(void) {
	//2.7V-4.2V
	if(++systemSettings.cutoutCellVoltage > 42) {
		//ensure minimum 9V
		if(systemSettings.cutoutCellCount == 3) {
			systemSettings.cutoutCellVoltage = 30;
		}
		//4+ cells allowed down to 2.7V
		else {
			systemSettings.cutoutCellVoltage = 27;
		}
	}
	return false;
}
static void settings_displayBatteryCellVoltage(void) {
	printShortDescription(21, 5);
	printVoltageCutout(lookupVoltageLevel(systemSettings.cutoutSetting));
}

static bool settings_setPowerSourceCustom(void) {
	//9V-24V
	if(++systemSettings.cutoutCustom > 240) {
		systemSettings.cutoutCustom = 90;
	}
	return false;
}
static void settings_displayPowerSourceCustom(void) {
	printShortDescription(21, 5);
	printVoltageCutout(lookupVoltageLevel(systemSettings.cutoutSetting));
}

static bool settings_setSleepTemp(void) {
	//If in C, 10 deg, if in F 20 deg
	if (systemSettings.temperatureInF) {
		systemSettings.SleepTemp += 20;
		if (systemSettings.SleepTemp > 580)
			systemSettings.SleepTemp = 120;
	} else {
		systemSettings.SleepTemp += 10;
		if (systemSettings.SleepTemp > 300)
			systemSettings.SleepTemp = 50;
	}
	return false;
}

static void settings_displaySleepTemp(void) {
	printShortDescription(1, 5);
	lcd.printNumber(systemSettings.SleepTemp, 3);
}

static bool settings_setSleepTime(void) {
	systemSettings.SleepTime++;  // Go up 1 minute at a time
	if (systemSettings.SleepTime >= 16) {
		systemSettings.SleepTime = 0;  // can't set time over 10 mins
	}
	// Remember that ^ is the time of no movement
	if (PCBVersion == 3)
		systemSettings.SleepTime = 0;  //Disable sleep on no accel
	return false;
}

static void settings_displaySleepTime(void) {
	printShortDescription(2, 5);
	if (systemSettings.SleepTime == 0) {
		lcd.print(OffString);
	} else if (systemSettings.SleepTime < 6) {
		lcd.printNumber(systemSettings.SleepTime * 10, 2);
		lcd.drawChar('S');
	} else {
		lcd.printNumber(systemSettings.SleepTime - 5, 2);
		lcd.drawChar('M');
	}
}

static bool settings_setShutdownTime(void) {
	systemSettings.ShutdownTime++;
	if (systemSettings.ShutdownTime > 60) {
		systemSettings.ShutdownTime = 0;  // wrap to off
	}
	if (PCBVersion == 3)
		systemSettings.ShutdownTime = 0;  //Disable shutdown on no accel
	return false;
}

static void settings_displayShutdownTime(void) {
	printShortDescription(3, 5);
	if (systemSettings.ShutdownTime == 0) {
		lcd.print(OffString);
	} else {
		lcd.printNumber(systemSettings.ShutdownTime, 2);
		lcd.drawChar('M');
	}
}

static bool settings_setTempF(void) {
	systemSettings.temperatureInF = !systemSettings.temperatureInF;
	if (systemSettings.temperatureInF) {
		//Change sleep, boost and soldering temps to the F equiv
		//C to F == F= ( (C*9) +160)/5
		systemSettings.BoostTemp = ((systemSettings.BoostTemp * 9) + 160) / 5;
		systemSettings.SolderingTemp =
				((systemSettings.SolderingTemp * 9) + 160) / 5;
		systemSettings.SleepTemp = ((systemSettings.SleepTemp * 9) + 160) / 5;
	} else {
		//Change sleep, boost and soldering temps to the C equiv
		// F->C == C = ((F-32)*5)/9
		systemSettings.BoostTemp = ((systemSettings.BoostTemp - 32) * 5) / 9;
		systemSettings.SolderingTemp = ((systemSettings.SolderingTemp - 32) * 5)
				/ 9;
		systemSettings.SleepTemp = ((systemSettings.SleepTemp - 32) * 5) / 9;

	}
	// Rescale both to be multiples of 10
	systemSettings.BoostTemp = systemSettings.BoostTemp/10;
	systemSettings.BoostTemp *=10;
	systemSettings.SolderingTemp = systemSettings.SolderingTemp/10;
	systemSettings.SolderingTemp *=10;
	systemSettings.SleepTemp = systemSettings.SleepTemp/10;
	systemSettings.SleepTemp *=10;

	return false;
}

static void settings_displayTempF(void) {
	printShortDescription(5, 7);

	lcd.drawChar((systemSettings.temperatureInF) ? 'F' : 'C');
}

static bool settings_setSensitivity(void) {
	systemSettings.sensitivity++;
	systemSettings.sensitivity = systemSettings.sensitivity % 10;
	return false;
}

static void settings_displaySensitivity(void) {
	printShortDescription(4, 7);
	lcd.printNumber(systemSettings.sensitivity, 1);
}

static bool settings_setAdvancedSolderingScreens(void) {
	systemSettings.detailedSoldering = !systemSettings.detailedSoldering;
	return false;
}

static void settings_displayAdvancedSolderingScreens(void) {
	printShortDescription(15, 7);

	lcd.drawCheckbox(systemSettings.detailedSoldering);
}

static bool settings_setAdvancedIDLEScreens(void) {
	systemSettings.detailedIDLE = !systemSettings.detailedIDLE;
	return false;
}

static void settings_displayAdvancedIDLEScreens(void) {
	printShortDescription(6, 7);

	lcd.drawCheckbox(systemSettings.detailedIDLE);
}
static bool settings_setScrollSpeed(void) {
	if (systemSettings.descriptionScrollSpeed == 0)	//TODO 1 bit -> just to increment should be enough?
		systemSettings.descriptionScrollSpeed = 1;
	else
		systemSettings.descriptionScrollSpeed = 0;
	return false;
}
static void settings_displayScrollSpeed(void) {
	printShortDescription(16, 7);
	lcd.drawChar(
			(systemSettings.descriptionScrollSpeed) ?
					SettingFastChar : SettingSlowChar);
}

static bool settings_setDisplayRotation(void) {
	systemSettings.OrientationMode++;
	systemSettings.OrientationMode = systemSettings.OrientationMode % 3;
	switch (systemSettings.OrientationMode) {
	case 0:
		lcd.setRotation(false);
		break;
	case 1:
		lcd.setRotation(true);
		break;
	case 2:
		//do nothing on auto
		break;
	default:
		break;
	}
	return false;
}

static void settings_displayDisplayRotation(void) {
	printShortDescription(7, 7);

	switch (systemSettings.OrientationMode) {
	case 0:
		lcd.drawChar(SettingRightChar);
		break;
	case 1:
		lcd.drawChar(SettingLeftChar);
		break;
	case 2:
		lcd.drawChar(SettingAutoChar);
		break;
	default:
		lcd.drawChar(SettingRightChar);
		break;
	}
}

static bool settings_setBoostModeEnabled(void) {
	systemSettings.boostModeEnabled = !systemSettings.boostModeEnabled;
	return false;
}

static void settings_displayBoostModeEnabled(void) {
	printShortDescription(8, 7);

	lcd.drawCheckbox(systemSettings.boostModeEnabled);
}

static bool settings_setBoostTemp(void) {
	if (systemSettings.temperatureInF) {
		systemSettings.BoostTemp += 20;  // Go up 20F at a time
		if (systemSettings.BoostTemp > 850) {
			systemSettings.BoostTemp = 480;  // loop back at 250
		}
	} else {
		systemSettings.BoostTemp += 10;  // Go up 10C at a time
		if (systemSettings.BoostTemp > 450) {
			systemSettings.BoostTemp = 250;  // loop back at 250
		}
	}
	return false;
}

static void settings_displayBoostTemp(void) {
	printShortDescription(9, 5);
	lcd.printNumber(systemSettings.BoostTemp, 3);
}

static bool settings_setAutomaticStartMode(void) {
	systemSettings.autoStartMode++;
	systemSettings.autoStartMode %= 2;
	return false;
}

static void settings_displayAutomaticStartMode(void) {
	printShortDescription(10, 7);

	lcd.drawCheckbox(systemSettings.autoStartMode);
}

static bool settings_setCoolingBlinkEnabled(void) {
	systemSettings.coolingTempBlink = !systemSettings.coolingTempBlink;	//TODO same with description speed?
	return false;
}

static void settings_displayCoolingBlinkEnabled(void) {
	printShortDescription(11, 7);

	lcd.drawCheckbox(systemSettings.coolingTempBlink);
}

static bool settings_setResetSettings(void) {
	if (userConfirmation(SettingsResetWarning)) {
		resetSettings();

		lcd.setFont(0);
		lcd.setCursor(0, 0);
		lcd.print("RESET OK");
		lcd.refresh();

		waitForButtonPressOrTimeout(200);
	}
	return false;
}

static void settings_displayResetSettings(void) {
	printShortDescription(13, 7);
}

static bool settings_setCalibrate(void) {
	if (userConfirmation(SettingsCalibrationWarning)) {
		//User confirmed
		//So we now perform the actual calculation
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		lcd.print(".....");
		lcd.refresh();

		setCalibrationOffset(0);            //turn off the current offset
		for (uint8_t i = 0; i < 20; i++) {
			getTipRawTemp(1); //cycle through the filter a fair bit to ensure we're stable.
			osDelay(20);
		}
		osDelay(100);

		uint16_t rawTempC = tipMeasurementToC(getTipRawTemp(0));
		//We now measure the current reported tip temperature
		uint16_t handleTempC = getHandleTemperature() / 10;
		//We now have an error between these that we want to store as the offset
		rawTempC = rawTempC - handleTempC;
		systemSettings.CalibrationOffset = rawTempC;
		setCalibrationOffset(rawTempC);       //store the error
		osDelay(100);
	}
	return false;
}

static void settings_displayCalibrate(void) {
	printShortDescription(12, 5);
}

static bool settings_setCalibrateVIN(void) {
	// Jump to the voltage calibration subscreen
	lcd.setFont(0);
	lcd.clearScreen();
	lcd.setCursor(0, 0);

	for (;;) {
		lcd.setCursor(0, 0);
		lcd.printNumber(getInputVoltageX10(systemSettings.voltageDiv) / 10, 2);
		lcd.print(".");
		lcd.printNumber(getInputVoltageX10(systemSettings.voltageDiv) % 10, 1);
		lcd.print("V");

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
			return false;

		case BUTTON_NONE:
		default:
			break;
		}

		lcd.refresh();
		osDelay(50);

		// Cap to sensible values
		if (systemSettings.voltageDiv < 90) {
			systemSettings.voltageDiv = 90;
		} else if (systemSettings.voltageDiv > 130) {
			systemSettings.voltageDiv = 130;
		}
	}
	return false;
}

static void settings_displayCalibrateVIN(void) {
	printShortDescription(14, 5);
}
static void settings_displaySolderingMenu(void) {
	//Call into the menu
	lcd.setFont(1);
	lcd.setCursor(0, 0);
	//Draw title
	lcd.print(SettingsMenuEntries[0]);
	//Draw symbol
	//16 pixel wide image
	lcd.drawArea(96 - 16, 0, 16, 16, (&SettingsMenuIcons[(16 * 2) * 0]));
}
static bool settings_enterSolderingMenu(void) {
	gui_Menu(solderingMenu);
	return false;
}
static void settings_displayPowerMenu(void) {
	lcd.setFont(1);
	lcd.setCursor(0, 0);
	//Draw title
	lcd.print(SettingsMenuEntries[1]);
	//Draw symbol
	//16 pixel wide image
	lcd.drawArea(96 - 16, 0, 16, 16, (&SettingsMenuIcons[(16 * 2) * 1]));
}
static bool settings_enterPowerMenu(void) {
	gui_Menu(PowerMenu);
	return false;
}
//XXX
static void settings_displayPowerSourceMenu(void) {
	lcd.setFont(1);
	lcd.setCursor(0, 0);
	//Draw title
	lcd.print(SettingsMenuEntries[4]);
	//Draw symbol
	//16 pixel wide image
	lcd.drawArea(96 - 16, 0, 16, 16, (&SettingsMenuIcons[(16 * 2) * 1]));	//TODO icon
}
static bool settings_enterPowerSourceMenu(void) {
	uint8_t selectedType = -1;
	//when user changes power source type, increment handler returns here
	//so we can switch menu to corresponding one for new source type
	do {
		selectedType = systemSettings.cutoutSetting;
		gui_Menu(PowerSourceMenu[selectedType]);
	}
	while(selectedType != systemSettings.cutoutSetting);
	return false;
}
static void settings_displayUIMenu(void) {
	lcd.setFont(1);
	lcd.setCursor(0, 0);
	//Draw title
	lcd.print(SettingsMenuEntries[2]);
	//Draw symbol
	//16 pixel wide image
	lcd.drawArea(96 - 16, 0, 16, 16, (&SettingsMenuIcons[(16 * 2) * 2]));
}
static bool settings_enterUIMenu(void) {
	gui_Menu(UIMenu);
	return false;
}
static void settings_displayAdvancedMenu(void) {
	lcd.setFont(1);
	lcd.setCursor(0, 0);
	//Draw title
	lcd.print(SettingsMenuEntries[3]);
	//Draw symbol
	//16 pixel wide image
	lcd.drawArea(96 - 16, 0, 16, 16, (&SettingsMenuIcons[(16 * 2) * 3]));

}
static bool settings_enterAdvancedMenu(void) {
	gui_Menu(advancedMenu);
	return false;
}

void gui_Menu(const menuitem* menu) {
	// Draw the settings menu and provide iteration support etc
	uint8_t currentScreen = 0;
	uint32_t autoRepeatTimer = 0;
	uint8_t autoRepeatAcceleration = 0;
	bool earlyExit = false;
	uint32_t descriptionStart = 0;
	int16_t lastOffset = -1;
	bool lcdRefresh = true;
	ButtonState lastButtonState = BUTTON_NONE;

	while ((menu[currentScreen].draw.func != NULL) && earlyExit == false) {
		lcd.setFont(0);
		lcd.setCursor(0, 0);
		//If the user has hesitated for >=3 seconds, show the long text
		//Otherwise "draw" the option
		if (xTaskGetTickCount() - lastButtonTime < 300) {
			lcd.clearScreen();
			menu[currentScreen].draw.func();
			lastOffset = -1;
			lcdRefresh = true;
		} else {
			// Draw description
			if (descriptionStart == 0)
				descriptionStart = xTaskGetTickCount();
			// lower the value - higher the speed
			int16_t descriptionWidth = FONT_12_WIDTH
					* (strlen(menu[currentScreen].description) + 7);
			int16_t descriptionOffset =
					((xTaskGetTickCount() - descriptionStart)
							/ (systemSettings.descriptionScrollSpeed == 1 ?
									1 : 2));
			descriptionOffset %= descriptionWidth;		//Roll around at the end

			if (lastOffset != descriptionOffset) {
				lcd.clearScreen();

				//^ Rolling offset based on time
				lcd.setCursor((OLED_WIDTH - descriptionOffset), 0);
				lcd.print(menu[currentScreen].description);
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
				if (menu[currentScreen].incrementHandler.func != NULL)
					menu[currentScreen].incrementHandler.func();
				else
					earlyExit = true;
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
			if (xTaskGetTickCount() - autoRepeatTimer
					+ autoRepeatAcceleration> PRESS_ACCEL_INTERVAL_MAX) {
				menu[currentScreen].incrementHandler.func();
				autoRepeatTimer = xTaskGetTickCount();
				descriptionStart = 0;

				autoRepeatAcceleration += PRESS_ACCEL_STEP;
			}
			break;
		case BUTTON_B_LONG:
			if (xTaskGetTickCount() - autoRepeatTimer
					+ autoRepeatAcceleration> PRESS_ACCEL_INTERVAL_MAX) {
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

		if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration)
				< PRESS_ACCEL_INTERVAL_MIN) {
			autoRepeatAcceleration = PRESS_ACCEL_INTERVAL_MAX
					- PRESS_ACCEL_INTERVAL_MIN;
		}

		if (lcdRefresh) {
			lcd.refresh();  // update the LCD
			osDelay(20);
			lcdRefresh = false;
		}
	}

}

void enterSettingsMenu() {
	gui_Menu(rootSettingsMenu);  //Call the root menu
	saveSettings();
}
