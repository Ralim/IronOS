/*
 * Modes.c
 *
 *  Created on: 17 Sep 2016
 *      Author: Ralim <ralim@ralimtek.com>
 */
#include "Modes.h"
const char *SettingsLongNames[] = { "      Undervoltage Cutout (V)",
		"      Sleep Temperature (C)", "      Sleep Timeout (Minutes)",
		"      Shutdown Timeout (Minutes)", "      Motion Detection",
		"      Motion Sensitivity", "      Temperature Unit",
		"      Temperature Rounding Amount",
		"      Temperature Display Update Rate",
		"      Flip Display for Left Hand" };
const uint8_t SettingsLongNamesLengths[] = { 29, 27, 29, 32, 22, 24, 22, 33, 37,
		25 };
uint8_t CalStatus = 0;
//This does the required processing and state changes
void ProcessUI() {
	uint8_t Buttons = getButtons(); //read the buttons status
	static uint32_t lastModeChange = 0;
	if (millis() - getLastButtonPress() < 30)
		Buttons = 0;
	else if (Buttons != 0) {
		resetLastButtonPress();
		resetButtons();
	}
	//rough prevention for de-bouncing and allocates settling time

	switch (operatingMode) {
	case STARTUP:
		if (Buttons == (BUT_A | BUT_B)) {
			operatingMode = THERMOMETER;
		} else if (Buttons == BUT_A) {
			//A key pressed so we are moving to soldering mode
			operatingMode = SOLDERING;
		} else if (Buttons == BUT_B) {
			//B Button was pressed so we are moving to the Settings menu
			operatingMode = SETTINGS;
		}
		break;
	case SOLDERING:
		//We need to check the buttons if we need to jump out
		if (Buttons == BUT_A || Buttons == BUT_B) {
			//A or B key pressed so we are moving to temp set
			operatingMode = TEMP_ADJ;
		} else if (Buttons == (BUT_A | BUT_B)) {

			//Both buttons were pressed, exit back to the cooling screen
			operatingMode = COOLING;

		} else {
			//We need to check the timer for movement in case we need to goto idle
			if (systemSettings.movementEnabled)
				if (millis() - getLastMovement()
						> (systemSettings.SleepTime * 60000)) {
					if (millis() - getLastButtonPress()
							> (systemSettings.SleepTime * 60000)) {
						operatingMode = SLEEP;
						return;
					}
				}
			uint16_t voltage = readDCVoltage(systemSettings.voltageDiv); //get X10 voltage
			if ((voltage / 10) < systemSettings.cutoutVoltage) {
				operatingMode = UVLOWARN;
				lastModeChange = millis();
			}
			//Update the PID Loop
			int32_t newOutput = computePID(systemSettings.SolderingTemp);
			setIronTimer(newOutput);
		}
		break;
	case TEMP_ADJ:
		if (Buttons == BUT_A) {
			//A key pressed so we are moving down in temp

			if (systemSettings.SolderingTemp > 1000)
				systemSettings.SolderingTemp -= 100;
		} else if (Buttons == BUT_B) {
			//B key pressed so we are moving up in temp
			if (systemSettings.SolderingTemp < 4500)
				systemSettings.SolderingTemp += 100;
		} else {
			//we check the timeout for how long the buttons have not been pushed
			//if idle for > 3 seconds then we return to soldering
			//Or if both buttons pressed
			if ((millis() - getLastButtonPress() > 2000)
					|| Buttons == (BUT_A | BUT_B)) {
				operatingMode = SOLDERING;
				saveSettings();
			}
		}
		break;
	case SETTINGS:
		//Settings is the mode with the most logic
		//Here we are in the menu so we need to increment through the sub menus / increase the value

		if (Buttons & BUT_A) {
			//A key iterates through the menu
			if (settingsPage == SETTINGSOPTIONSCOUNT) {
				//Roll off the end
				settingsPage = 0;				//reset
				operatingMode = STARTUP;		//reset back to the startup
				saveSettings();					//Save the settings
			} else {
				++settingsPage;					//move to the next option
			}
		} else if (Buttons & BUT_B) {
			//B changes the value selected
			switch (settingsPage) {
			case UVCO:
				//we are incrementing the cutout voltage
				systemSettings.cutoutVoltage += 1;		//Go up 1V at a jump
				if (systemSettings.cutoutVoltage > 24)
					systemSettings.cutoutVoltage = 10;
				break;
			case SLEEP_TEMP:
				systemSettings.SleepTemp += 100;		//Go up 10C at a time
				if (systemSettings.SleepTemp > 3000)
					systemSettings.SleepTemp = 1000;//cant sleep higher than 300
				break;
			case SLEEP_TIME:
				++systemSettings.SleepTime;		//Go up 1 minute at a time
				if (systemSettings.SleepTime > 30)
					systemSettings.SleepTime = 1;	//cant set time over 30 mins
				//Remember that ^ is the time of no movement
				break;
			case SHUTDOWN_TIME:
				++systemSettings.ShutdownTime;
				if (systemSettings.ShutdownTime > 60)
					systemSettings.ShutdownTime = 0;			//wrap to off
				break;
			case MOTIONDETECT:
				systemSettings.movementEnabled =
						!systemSettings.movementEnabled;
				break;
			case TEMPDISPLAY:
				systemSettings.displayTempInF = !systemSettings.displayTempInF;
				break;
			case LEFTY:
				systemSettings.flipDisplay = !systemSettings.flipDisplay;
				break;
			case MOTIONSENSITIVITY:
				systemSettings.sensitivity++;
				systemSettings.sensitivity = systemSettings.sensitivity % 3;

				break;
			case TEMPROUNDING:
				systemSettings.temperatureRounding++;
				systemSettings.temperatureRounding =
						systemSettings.temperatureRounding % 3;
				break;
			case DISPUPDATERATE:
				systemSettings.displayUpdateSpeed++;
				systemSettings.displayUpdateSpeed =
						systemSettings.displayUpdateSpeed % 3;
				break;

			default:
				break;
			}
		}
		break;
	case SLEEP:
		//The iron is sleeping at a lower temperature due to lack of movement
		if (Buttons & BUT_A) {
			//A Button was pressed so we are moving back to soldering
			operatingMode = SOLDERING;
			Oled_DisplayOn();
			return;
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving back to soldering
			operatingMode = SOLDERING;
			Oled_DisplayOn();
			return;
		} else if (systemSettings.movementEnabled)
			if (millis() - getLastMovement() < 1000) {//moved in the last second
				operatingMode = SOLDERING; //Goto active mode again
				Oled_DisplayOn();
				return;
			}
		if (systemSettings.movementEnabled) {
			//Check if we should shutdown
			if ((millis() - getLastMovement()
					> (systemSettings.ShutdownTime * 60000))
					|| (millis() - getLastButtonPress()
							> systemSettings.ShutdownTime * 60000)) {
				operatingMode = COOLING; //shutdown the tip
				Oled_DisplayOn();
			}
		}
		//else if nothing has been pushed we need to compute the PID to keep the iron at the sleep temp
		int32_t newOutput = computePID(systemSettings.SleepTemp);
		setIronTimer(newOutput);
		break;
	case COOLING: {
		setIronTimer(0); //turn off heating
		//This mode warns the user the iron is still cooling down
		uint16_t temp = readIronTemp(0, 1, 0xFFFF); //take a new reading as the heater code is not taking new readings
		if (temp < 400) { //if the temp is < 40C then we can go back to IDLE
			operatingMode = STARTUP;
		} else if (Buttons & (BUT_A | BUT_B)) { //we check if the user has pushed a button to ack
			//Either button was pushed
			operatingMode = STARTUP;
		}
	}
		break;
	case UVLOWARN:
		//We are here if the DC voltage went too low
		//We want to jump back to IDLE after a bit
		if (millis() - lastModeChange > 3000) {		//its been 3 seconds
			operatingMode = STARTUP;		//jump back to idle mode
		}
		break;
	case THERMOMETER: {
		//This lets the user check the tip temp without heating the iron.. And eventually calibration will be added here

		if (Buttons == BUT_A) {
			//Single button press, cycle over to the DC display
			CalStatus = 0;
			operatingMode = DCINDISP;
		} else if (Buttons == BUT_B) {
			CalStatus = 0;
			operatingMode = TEMPCAL;
		} else if (Buttons == (BUT_A | BUT_B)) {
			//If the user is holding both button, exit the  screen
			operatingMode = STARTUP;
		}

	}
		break;
	case DCINDISP: {
		//This lets the user check the input voltage
		if (CalStatus == 0) {
			if (Buttons == BUT_A) {
				//Single button press, cycle over to the temp display
				operatingMode = THERMOMETER;
			} else if (Buttons == BUT_B) {
				//dc cal mode
				CalStatus = 1;
			} else if (Buttons == (BUT_A | BUT_B)) {
				//If the user is holding both button, exit the  screen
				operatingMode = STARTUP;
			}
		} else {
			//User is calibrating the dc input
			if (Buttons == BUT_A) {
				if (!systemSettings.flipDisplay)
					systemSettings.voltageDiv++;
				else
					systemSettings.voltageDiv--;
			} else if (Buttons == BUT_B) {
				if (!systemSettings.flipDisplay)
					systemSettings.voltageDiv--;
				else
					systemSettings.voltageDiv++;
			} else if (Buttons == (BUT_A | BUT_B)) {
				CalStatus = 0;
				saveSettings();
			}
			if (systemSettings.voltageDiv < 120)
				systemSettings.voltageDiv = 160;
			else if (systemSettings.voltageDiv > 160)
				systemSettings.voltageDiv = 120;
		}

	}
		break;
	case TEMPCAL: {
		if (Buttons == BUT_B) {
			//Single button press, cycle over to the DC IN
			operatingMode = THERMOMETER;
		} else if (Buttons == BUT_A) {
			//Try and calibrate
			if (CalStatus == 0) {
				if ((readTipTemp() < 300) && (readSensorTemp() < 300)) {
					CalStatus = 1;
					systemSettings.tempCalibration = readTipTemp();
					saveSettings();
				} else {
					CalStatus = 2;
				}
			}
		} else if (Buttons == (BUT_A | BUT_B)) {
			//If the user is holding both button, exit the  screen
			operatingMode = STARTUP;
		}

	}
		break;
	default:
		break;
	}
}
/*
 * Draws the temp with temp conversion if needed
 */
void drawTemp(uint16_t temp, uint8_t x, uint8_t roundingMode) {
	if (systemSettings.displayTempInF)
		temp = (temp * 9 + 1600) / 5;/*Convert to F -> T*(9/5)+32*/
	if (temp % 10 > 5)
		temp += 10;		//round up
	temp /= 10;
	//handle rounding modes
	if (roundingMode == ROUNDING_FIVE) {
		if (temp % 10 < 5)
			temp = (temp / 10) * 10;
		else
			temp = ((temp / 10) * 10) + 5;
	} else if (roundingMode == ROUNDING_TEN) {
		temp = (temp / 10) * 10;
	}

	OLED_DrawThreeNumber(temp, x);
}

/*
 * Performs all the OLED drawing for the current operating mode
 */
void DrawUI() {
	static uint32_t lastOLEDDrawTime = 0;
	static uint16_t lastSolderingDrawnTemp1 = 0;
	static uint16_t lastSolderingDrawnTemp2 = 0;

	static uint8_t settingsLongTestScrollPos = 0;
	uint16_t temp = readIronTemp(0, 0, 0xFFFF);
	switch (operatingMode) {
	case STARTUP:
		//We are chilling in the idle mode
		//Check if movement in the last 5 minutes , if not sleep OLED
		if (millis() - getLastMovement() > (5 * 60 * 1000)
				&& (millis() - getLastButtonPress() > (5 * 60 * 1000))) {
			//OLED off
			Oled_DisplayOff();
		} else {
			Oled_DisplayOn();
			OLED_DrawIDLELogo();		//Draw the icons for prompting the user
		}
		break;
	case SOLDERING:
		//The user is soldering
	{
		if (systemSettings.displayUpdateSpeed == DISPLAYMODE_SLOW
				&& (millis() - lastOLEDDrawTime < 200))
			return;
		else if (systemSettings.displayUpdateSpeed == DISPLAYMODE_MEDIUM
				&& (millis() - lastOLEDDrawTime < 100))
			return;
		else if (systemSettings.displayUpdateSpeed == DISPLAYMODE_FAST
				&& (millis() - lastOLEDDrawTime < 50))
			return;

		uint32_t tempavg = (temp + lastSolderingDrawnTemp1
				+ lastSolderingDrawnTemp2);
		tempavg /= 3;
		drawTemp(tempavg, 0, systemSettings.temperatureRounding);
		lastSolderingDrawnTemp1 = temp;
		lastSolderingDrawnTemp2 = lastSolderingDrawnTemp1;
		lastOLEDDrawTime = millis();
		//Now draw symbols
		OLED_DrawChar(' ', 3);
		OLED_BlankSlot(6 * 12 + 16, 24 - 16);//blank out the tail after the arrows
		OLED_BlankSlot(4 * 12 + 16, 24 - 16);//blank out the tail after the temp
		if (getIronTimer() == 0
				&& (temp / 10) > (systemSettings.SolderingTemp / 10)) {
			//Cooling
			OLED_DrawSymbol(6, 5);
		} else {
			if (getIronTimer() < 1500) {
				//Maintaining
				OLED_DrawSymbol(6, 7);
			} else {		//we are heating
				OLED_DrawSymbol(6, 6);
			}
		}
		if (systemSettings.displayTempInF) {
			OLED_DrawSymbol(4, 1);
		} else {
			OLED_DrawSymbol(4, 0);
		}

	}
		break;
	case TEMP_ADJ:
		//We are prompting the user to change the temp so we draw the current setpoint temp
		//With the nifty arrows
		OLED_DrawChar(' ', 0);
		OLED_DrawChar('<', 1);
		drawTemp(systemSettings.SolderingTemp, 2, 0);
		OLED_DrawChar(' ', 5);
		OLED_DrawChar(' ', 7);
		OLED_DrawChar('>', 6);
		break;
	case SETTINGS:
		//We are prompting the user the setting name
		if (millis() - getLastButtonPress() > 3000) {
			//If the user has idled for > 3 seconds, show the long name for the selected setting instead
			//draw from settingsLongTestScrollPos through to end of screen
			uint8_t lengthLeft = SettingsLongNamesLengths[settingsPage]
					- settingsLongTestScrollPos;
			if (lengthLeft < 1)
				settingsLongTestScrollPos = 0;
			//^ Reset once not much left
			if (lengthLeft > 8)
				lengthLeft = 8;
			OLED_DrawString(
					SettingsLongNames[(uint8_t) settingsPage]
							+ settingsLongTestScrollPos, lengthLeft);
			if (lengthLeft < 8)
				for (uint8_t i = lengthLeft; i < 8; i++)
					OLED_DrawChar(' ', i);
			if (millis() - lastOLEDDrawTime > 120) {
				settingsLongTestScrollPos++;
				lastOLEDDrawTime = millis();
			}
		} else {
			settingsLongTestScrollPos = 0;
			switch (settingsPage) {
			case UVCO:
				OLED_DrawString("UVCO ", 5);
				OLED_DrawTwoNumber(systemSettings.cutoutVoltage, 5);
				OLED_DrawChar('V', 7);
				break;
			case SLEEP_TEMP:
				OLED_DrawString("STMP ", 5);
				OLED_DrawThreeNumber(systemSettings.SleepTemp / 10, 5);
				break;
			case SLEEP_TIME:
				OLED_DrawString("SLTME ", 6);
				OLED_DrawTwoNumber(systemSettings.SleepTime, 6);
				break;
			case SHUTDOWN_TIME:
				OLED_DrawString("SHTME ", 6);
				OLED_DrawTwoNumber(systemSettings.ShutdownTime, 6);
				break;
			case MOTIONDETECT:/*Toggle the mode*/
				if (systemSettings.movementEnabled)
					OLED_DrawString("MOTION T", 8);
				else
					OLED_DrawString("MOTION F", 8);
				break;
			case TEMPDISPLAY:/*Are we showing in C or F ?*/
				if (systemSettings.displayTempInF)
					OLED_DrawString("TMPUNT F", 8);
				else
					OLED_DrawString("TMPUNT C", 8);
				break;

			case LEFTY:

				if (systemSettings.flipDisplay)
					OLED_DrawString("FLPDSP T", 8);
				else
					OLED_DrawString("FLPDSP F", 8);
				break;
			case MOTIONSENSITIVITY:
				switch (systemSettings.sensitivity) {
				case MOTION_HIGH:
					OLED_DrawString("SENSE H ", 8);
					break;
				case MOTION_MED:
					OLED_DrawString("SENSE M ", 8);
					break;
				case MOTION_LOW:
					OLED_DrawString("SENSE L ", 8);
					break;
				default:
					OLED_DrawString("SENSE   ", 8);
					break;
				}

				break;
			case TEMPROUNDING:
				//We are prompting the user about their display mode preferences
			{
				switch (systemSettings.temperatureRounding) {
				case ROUNDING_NONE:
					OLED_DrawString("TMPRND 1", 8);
					break;
				case ROUNDING_FIVE:
					OLED_DrawString("TMPRND 5", 8);
					break;
				case ROUNDING_TEN:
					OLED_DrawString("TMPRND10", 8);
					break;
				default:
					OLED_DrawString("TMPRND 1", 8);
					break;
				}
			}
				break;
			case DISPUPDATERATE:
				//We are prompting the user about their display mode preferences
			{
				switch (systemSettings.displayUpdateSpeed) {
				case DISPLAYMODE_FAST:
					OLED_DrawString("TMPSPD F", 8);
					break;
				case DISPLAYMODE_SLOW:
					OLED_DrawString("TMPSPD S", 8);
					break;
				case DISPLAYMODE_MEDIUM:
					OLED_DrawString("TMPSPD M", 8);
					break;

				}
			}
				break;
			default:
				break;
			}
		}
		break;
	case SLEEP:
		//The iron is in sleep temp mode
		//Draw in temp and sleep
		OLED_DrawString("SLP", 3);
		drawTemp(temp, 4, systemSettings.temperatureRounding);
		OLED_BlankSlot(84, 96 - 85);		//blank out after the temp

		if (millis() - getLastMovement() > (10 * 60 * 1000)
				&& (millis() - getLastButtonPress() > (10 * 60 * 1000))) {
			//OLED off
			Oled_DisplayOff();
		} else {
			Oled_DisplayOn();
		}

		break;
	case COOLING:
		//We are warning the user the tip is cooling
		OLED_DrawString("COOL ", 5);
		drawTemp(temp, 5, systemSettings.temperatureRounding);
		break;
	case UVLOWARN:
		OLED_DrawString("LOW VOLT", 8);
		break;
	case THERMOMETER:
		temp = readIronTemp(0, 1, 0xFFFF);	//Force a reading as heater is off
		OLED_DrawString("TEMP ", 5);//extra one to it clears the leftover 'L' from IDLE
		drawTemp(temp, 5, 0);
		break;
	case DCINDISP: {
		uint16_t voltage = readDCVoltage(systemSettings.voltageDiv); //get X10 voltage

		if (CalStatus == 0 || ((millis() % 1000) > 500)) {
			OLED_DrawString("IN", 2);
			OLED_DrawChar((voltage / 100) % 10, 2);
			voltage -= (voltage / 100) * 100;
			OLED_DrawChar((voltage / 10) % 10, 3);
			voltage -= (voltage / 10) * 10;
			OLED_DrawChar('.', 4);
			OLED_DrawChar(voltage % 10, 5);
			OLED_DrawChar('V', 6);
			OLED_DrawChar(' ', 7);
		} else {
			OLED_DrawString("IN      ", 8);
		}
	}
		break;
	case TEMPCAL: {

		if (CalStatus == 0) {
			OLED_DrawString("CAL TEMP", 8);
		} else if (CalStatus == 1) {
			OLED_DrawString("CAL OK  ", 8);
		} else if (CalStatus == 2) {
			OLED_DrawString("CAL FAIL", 8);
		}
	}

		break;
	default:
		break;
	}

}
