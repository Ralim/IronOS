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
static void settings_setTempF(void);
static void settings_displayTempF(void);
static void settings_setAdvancedSolderingScreens(void);
static void settings_displayAdvancedSolderingScreens(void);
static void settings_setAdvancedIDLEScreens(void);
static void settings_displayAdvancedIDLEScreens(void);
static void settings_setScrollSpeed(void);
static void settings_displayScrollSpeed(void);
#ifdef PIDSETTINGS

static void settings_setPIDP(void);
static void settings_displayPIDP(void);
static void settings_setPIDI(void);
static void settings_displayPIDI(void);
static void settings_setPIDD(void);
static void settings_displayPIDD(void);
#endif
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
static void settings_setTipModel(void);
static void settings_displayTipModel(void);
static void settings_setCalibrate(void);
static void settings_displayCalibrate(void);
static void settings_setCalibrateVIN(void);
static void settings_displayCalibrateVIN(void);

//Calibration Menu
static void calibration_displaySimpleCal(void); // Hot water cal
static void calibration_enterSimpleCal(void);
static void calibration_displayAdvancedCal(void); // two point cal
static void calibration_enterAdvancedCal(void);

//Menu functions
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
 * Power Source
 * Soldering Menu
 * Power Saving Menu
 * UI Menu
 * Advanced Menu
 * Exit
 */
{ (const char*) SettingsDescriptions[0], { settings_setInputVRange }, {
		settings_displayInputVRange } }, /*Voltage input*/
{ (const char*) SettingsMenuEntriesDescriptions[0], {
		settings_enterSolderingMenu }, { settings_displaySolderingMenu } }, /*Soldering*/
{ (const char*) SettingsMenuEntriesDescriptions[1], { settings_enterPowerMenu },
		{ settings_displayPowerMenu } }, /*Sleep Options Menu*/
{ (const char*) SettingsMenuEntriesDescriptions[2], { settings_enterUIMenu }, {
		settings_displayUIMenu } }, /*UI Menu*/
{ (const char*) SettingsMenuEntriesDescriptions[3],
		{ settings_enterAdvancedMenu }, { settings_displayAdvancedMenu } }, /*Advanced Menu*/
{ NULL, { NULL }, { NULL } }            // end of menu marker. DO NOT REMOVE
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
{ NULL, { NULL }, { NULL } }            // end of menu marker. DO NOT REMOVE
};
const menuitem UIMenu[] = {
/*
 // Language
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
{ NULL, { NULL }, { NULL } }            // end of menu marker. DO NOT REMOVE
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
{ NULL, { NULL }, { NULL } }            // end of menu marker. DO NOT REMOVE
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
{ (const char*) SettingsDescriptions[17], { settings_setTipModel }, {
		settings_displayTipModel } }, /*Select tip Model */
{ (const char*) SettingsDescriptions[12], { settings_setCalibrate }, {
		settings_displayCalibrate } }, /*Calibrate tip*/
{ (const char*) SettingsDescriptions[14], { settings_setCalibrateVIN }, {
		settings_displayCalibrateVIN } }, /*Voltage input cal*/
#ifdef PIDSETTINGS

		{	(const char*) SettingsDescriptions[17], {settings_setPIDP}, {
				settings_displayPIDP}}, /*Voltage input cal*/
		{	(const char*) SettingsDescriptions[18], {settings_setPIDI}, {
				settings_displayPIDI}}, /*Voltage input cal*/
		{	(const char*) SettingsDescriptions[19], {settings_setPIDD}, {
				settings_displayPIDD}}, /*Voltage input cal*/
#endif
		{ NULL, { NULL }, { NULL } }        // end of menu marker. DO NOT REMOVE
};

const menuitem calibrationMenu[] { { (const char*) SettingsDescriptions[6], {
		calibration_enterSimpleCal }, { calibration_displaySimpleCal } },
/* Simple Cal*/
{ (const char*) SettingsDescriptions[6], { calibration_enterAdvancedCal }, {
		calibration_displayAdvancedCal } }, /* Advanced Cal */
{ NULL, { NULL }, { NULL } } };

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
			osDelay(40);
			lcdRefresh = false;
		}
	}
	return 0;
}

static void settings_setInputVRange(void) {
	systemSettings.cutoutSetting = (systemSettings.cutoutSetting + 1) % 5;
}

static void settings_displayInputVRange(void) {
	printShortDescription(0, 6);

	if (systemSettings.cutoutSetting) {
		lcd.drawChar('0' + 2 + systemSettings.cutoutSetting);
		lcd.drawChar('S');
	} else {
		lcd.print("DC");
	}
}

static void settings_setSleepTemp(void) {
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
}

static void settings_displaySleepTemp(void) {
	printShortDescription(1, 5);
	lcd.printNumber(systemSettings.SleepTemp, 3);
}

static void settings_setSleepTime(void) {
	systemSettings.SleepTime++;  // Go up 1 minute at a time
	if (systemSettings.SleepTime >= 16) {
		systemSettings.SleepTime = 0;  // can't set time over 10 mins
	}
	// Remember that ^ is the time of no movement
	if (PCBVersion == 3)
		systemSettings.SleepTime = 0;  //Disable sleep on no accel
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

static void settings_setShutdownTime(void) {
	systemSettings.ShutdownTime++;
	if (systemSettings.ShutdownTime > 60) {
		systemSettings.ShutdownTime = 0;  // wrap to off
	}
	if (PCBVersion == 3)
		systemSettings.ShutdownTime = 0;  //Disable shutdown on no accel
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

static void settings_setTempF(void) {
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
	systemSettings.BoostTemp = systemSettings.BoostTemp / 10;
	systemSettings.BoostTemp *= 10;
	systemSettings.SolderingTemp = systemSettings.SolderingTemp / 10;
	systemSettings.SolderingTemp *= 10;
	systemSettings.SleepTemp = systemSettings.SleepTemp / 10;
	systemSettings.SleepTemp *= 10;

}

static void settings_displayTempF(void) {
	printShortDescription(5, 7);

	lcd.drawChar((systemSettings.temperatureInF) ? 'F' : 'C');
}

static void settings_setSensitivity(void) {
	systemSettings.sensitivity++;
	systemSettings.sensitivity = systemSettings.sensitivity % 10;
}

static void settings_displaySensitivity(void) {
	printShortDescription(4, 7);
	lcd.printNumber(systemSettings.sensitivity, 1);
}

static void settings_setAdvancedSolderingScreens(void) {
	systemSettings.detailedSoldering = !systemSettings.detailedSoldering;
}

static void settings_displayAdvancedSolderingScreens(void) {
	printShortDescription(15, 7);

	lcd.drawCheckbox(systemSettings.detailedSoldering);
}

static void settings_setAdvancedIDLEScreens(void) {
	systemSettings.detailedIDLE = !systemSettings.detailedIDLE;
}

static void settings_displayAdvancedIDLEScreens(void) {
	printShortDescription(6, 7);

	lcd.drawCheckbox(systemSettings.detailedIDLE);
}
static void settings_setScrollSpeed(void) {

	if (systemSettings.descriptionScrollSpeed == 0)
		systemSettings.descriptionScrollSpeed = 1;
	else
		systemSettings.descriptionScrollSpeed = 0;
}
static void settings_displayScrollSpeed(void) {
	printShortDescription(16, 7);
	lcd.drawChar(
			(systemSettings.descriptionScrollSpeed) ?
					SettingFastChar : SettingSlowChar);
}

static void settings_setDisplayRotation(void) {
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

static void settings_setBoostModeEnabled(void) {
	systemSettings.boostModeEnabled = !systemSettings.boostModeEnabled;
}

static void settings_displayBoostModeEnabled(void) {
	printShortDescription(8, 7);

	lcd.drawCheckbox(systemSettings.boostModeEnabled);
}

static void settings_setBoostTemp(void) {

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
}

static void settings_displayBoostTemp(void) {
	printShortDescription(9, 5);
	lcd.printNumber(systemSettings.BoostTemp, 3);
}

#ifdef PIDSETTINGS
static void settings_setPIDP(void) {
	systemSettings.PID_P++;
	systemSettings.PID_P %= 100;
}
static void settings_displayPIDP(void) {
	printShortDescription(17, 6);
	lcd.printNumber(systemSettings.PID_P, 2);
}
static void settings_setPIDI(void) {
	systemSettings.PID_I++;
	systemSettings.PID_I %= 100;
}
static void settings_displayPIDI(void) {
	printShortDescription(18, 6);
	lcd.printNumber(systemSettings.PID_I, 2);
}
static void settings_setPIDD(void) {
	systemSettings.PID_D++;
	systemSettings.PID_D %= 100;
}
static void settings_displayPIDD(void) {
	printShortDescription(19, 6);
	lcd.printNumber(systemSettings.PID_D, 2);
}
#endif

static void settings_setAutomaticStartMode(void) {
	systemSettings.autoStartMode++;
	systemSettings.autoStartMode %= 2;
}

static void settings_displayAutomaticStartMode(void) {
	printShortDescription(10, 7);
	lcd.drawCheckbox(systemSettings.autoStartMode);
}

static void settings_setCoolingBlinkEnabled(void) {
	systemSettings.coolingTempBlink = !systemSettings.coolingTempBlink;
}

static void settings_displayCoolingBlinkEnabled(void) {
	printShortDescription(11, 7);

	lcd.drawCheckbox(systemSettings.coolingTempBlink);
}

static void settings_setResetSettings(void) {
	if (userConfirmation(SettingsResetWarning)) {
		resetSettings();

		lcd.setFont(0);
		lcd.setCursor(0, 0);
		lcd.print("RESET OK");
		lcd.refresh();

		waitForButtonPressOrTimeout(200);
	}
}

static void settings_displayResetSettings(void) {
	printShortDescription(13, 7);
}

static void settings_setTipModel(void) {
	systemSettings.tipType++;
	systemSettings.tipType %= (Tip_Custom + 1);  //Wrap after custom

}
static void settings_displayTipModel(void) {
	printShortDescription(17, 4);
	//Print in small text the tip model
	lcd.setFont(1);
	//set the cursor
	//Print the mfg
	lcd.setCursor(40, 0);
	if (systemSettings.tipType < Tip_MiniWare) {
		lcd.print("TS100");
	} else if (systemSettings.tipType < Tip_Hakko) {
		lcd.print("HAKKO");
	} else if (systemSettings.tipType == Tip_Custom) {
		lcd.print("User");
	}
	lcd.setCursor(40, 8);
	switch ((enum TipType) systemSettings.tipType) {
	case TS_B2:
		lcd.print(" B2 ");
		break;
	case TS_D24:
		lcd.print(" D24 ");
		break;
	case TS_BC2:
		lcd.print(" BC2 ");
		break;
	case TS_C1:
		lcd.print(" C1 ");
		break;
	case HAKKO_BC2:
		lcd.print(" BC2 ");
		break;
	case Tip_Custom:
		lcd.print("Tuned");
		break;
	default:
		lcd.print("????");
		break;
	}
}
static void calibration_displaySimpleCal(void) {
	printShortDescription(18, 5);
}
static void dotDelay() {
	for (uint8_t i = 0; i < 20; i++) {
		getTipRawTemp(1); //cycle through the filter a fair bit to ensure we're stable.
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		for (uint8_t x = 0; x < i / 4; x++)
			lcd.print(".");
		lcd.refresh();
		osDelay(50);
	}
}
static void setTipOffset() {
	setCalibrationOffset(0);            //turn off the current offset
	dotDelay();

	//If the thermocouple at the end of the tip, and the handle are at equalibrium, then the output should be zero, as there is no temperature differential.

	int32_t offset = 0;
	for (uint8_t i = 0; i < 15; i++) {
		offset += getTipRawTemp(1); //cycle through the filter a fair bit to ensure we're stable.

		lcd.clearScreen();
		lcd.setCursor(0, 0);

		for (uint8_t x = 0; x < i / 4; x++)
			lcd.print(".");
		lcd.refresh();
		osDelay(200);
	}
	systemSettings.CalibrationOffset = offset / 15;
	setCalibrationOffset(systemSettings.CalibrationOffset); //store the error
	osDelay(100);
}
static void calibration_enterSimpleCal(void) {
//User has entered into the simple cal routine
	if (userConfirmation(SettingsCalibrationWarning)) {
		//User has confirmed their handle is at ambient
		//So take the offset measurement
		setTipOffset();
		//Next we want the user to put the tip into 100C water so we can calculate their tip's gain
		//Gain is the m term from rise/run plot of raw readings vs (tip-handle)
		//Thus we want to calculate  ([TipRawHot-TipRawCold])/(ActualHot-HandleHot)-(ActualCold-HandleCold)
		//Thus we first need to store -> TiprawCold,HandleCold,ActualCold==HandleCold -> RawTipCold
		uint32_t RawTipCold = getTipRawTemp(0) * 10;
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		lcd.setFont(1);
		lcd.print("Please Insert Tip\nInto Boiling Water");
		lcd.refresh();
		osDelay(200);
		waitForButtonPress();
		dotDelay();		//cycle the filter a bit
		//Now take the three hot measurements
		//Assume water is boiling at 100C
		uint32_t RawTipHot = getTipRawTemp(0) * 10;
		uint32_t HandleTempHot = getHandleTemperature() / 10;

		uint32_t gain = (RawTipHot - RawTipCold) / (100 - HandleTempHot);

		//Show this to the user
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		lcd.print("Your G: ");
		lcd.printNumber(gain, 6);
		lcd.print("\n~= 120-140");
		lcd.refresh();
		osDelay(2000);
		waitForButtonPress();
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		lcd.print("H: ");
		lcd.printNumber(RawTipHot, 8);
		lcd.setCursor(0, 8);
		lcd.print("C: ");
		lcd.printNumber(RawTipCold, 8);
		lcd.refresh();
		osDelay(2000);
		waitForButtonPress();
	}
}
static void calibration_displayAdvancedCal(void) {
	printShortDescription(19, 5);
}
static void calibration_enterAdvancedCal(void) {
	//Advanced cal
	if (userConfirmation(SettingsCalibrationWarning)) {
		//User has confirmed their handle is at ambient
		//So take the offset measurement
		setTipOffset();
		//The tip now has a known ADC offset
		//Head up until it is at 350C
		//Then let the user adjust the gain value until it converges
		systemSettings.customTipGain = 120;
		bool exit = false;

		while (exit == false) {
			//Set tip to 350C
			currentlyActiveTemperatureTarget = ctoTipMeasurement(350);
			//Check if user has pressed button to change the gain
			ButtonState buttons = getButtonState();
			switch (buttons) {
			case BUTTON_NONE:
				break;
			case BUTTON_BOTH:
			case BUTTON_B_LONG:
			case BUTTON_F_LONG:
				exit = true;
				break;
			case BUTTON_F_SHORT:
				systemSettings.customTipGain++;
				break;
			case BUTTON_B_SHORT: {
				systemSettings.customTipGain--;
			}
				break;
			default:
				break;
			}
			if (systemSettings.customTipGain > 200)
				systemSettings.customTipGain = 200;
			else if (systemSettings.customTipGain <= 100)
				systemSettings.customTipGain = 100;
			lcd.setCursor(0, 0);
			lcd.clearScreen();
			lcd.setFont(0);
			if (lcd.getRotation())
				lcd.drawChar('-');
			else
				lcd.drawChar('+');

			lcd.drawChar(' ');
			lcd.printNumber(systemSettings.customTipGain, 4);
			lcd.drawChar(' ');
			if (lcd.getRotation())
				lcd.drawChar('+');
			else
				lcd.drawChar('-');
			lcd.refresh();
			GUIDelay();
		}

	}
}
//Provide the user the option to tune their own tip if custom is selected
//If not only do single point tuning as per usual
static void settings_setCalibrate(void) {
	if (systemSettings.tipType == Tip_Custom) {
		//Two types of calibration
		//1. Basic, idle temp + hot water (100C)
		//2. Advanced, 100C + 350C, we keep PID tracking to a temperature target
		return gui_Menu(calibrationMenu);
	}
	//Else
	// Ask user if handle is at the tip temperature
	// Any error between handle and the tip will be a direct offset in the control loop

	else if (userConfirmation(SettingsCalibrationWarning)) {
		//User confirmed
		//So we now perform the actual calculation
		setTipOffset();

	}
}

static void settings_displayCalibrate(void) {
	printShortDescription(12, 5);
}

static void settings_setCalibrateVIN(void) {
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
			return;
			break;
		case BUTTON_NONE:
		default:
			break;
		}

		lcd.refresh();
		osDelay(40);

		// Cap to sensible values
		if (systemSettings.voltageDiv < 360) {
			systemSettings.voltageDiv = 360;
		} else if (systemSettings.voltageDiv > 520) {
			systemSettings.voltageDiv = 520;
		}
	}
}

static void displayMenu(size_t index) {
	//Call into the menu
	lcd.setFont(1);
	lcd.setCursor(0, 0);
	//Draw title
	lcd.print(SettingsMenuEntries[index]);
	//Draw symbol
	//16 pixel wide image
	lcd.drawArea(96 - 16, 0, 16, 16, (&SettingsMenuIcons[(16 * 2) * index]));
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
			osDelay(40);
			lcdRefresh = false;
		}
	}

}

void enterSettingsMenu() {
	gui_Menu(rootSettingsMenu);  //Call the root menu
	saveSettings();
}
