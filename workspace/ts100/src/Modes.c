/*
 * Modes.c
 *
 *  Created on: 17 Sep 2016
 *      Author: Ralim <ralim@ralimtek.com>
 */
#include "Modes.h"

uint8_t StatusFlags = 0;
uint32_t temporaryTempStorage = 0;

operatingModeEnum operatingMode;
settingsPageEnum settingsPage;

//This does the required processing and state changes
void ProcessUI() {
	uint8_t Buttons = getButtons(); //read the buttons status
	static uint32_t lastModeChange = 0;

	switch (operatingMode) {
	case STARTUP:

		if (Buttons == (BUT_A | BUT_B)) {
			operatingMode = THERMOMETER;
		} else if (Buttons == BUT_A) {
			//A key pressed so we are moving to soldering mode
			operatingMode = SOLDERING;
			Oled_DisplayOn();
		} else if (Buttons == BUT_B) {
			//B Button was pressed so we are moving to the Settings menu
			operatingMode = SETTINGS;
			Oled_DisplayOn();
		}
		break;
	case SOLDERING:

		//^ This is to make the button more delayed in timing for people to find A+B easier
		//We need to check the buttons if we need to jump out
		if ((Buttons == BUT_A && !systemSettings.boostModeEnabled)
				|| Buttons == BUT_B) {
			//A or B key pressed so we are moving to temp set
			operatingMode = TEMP_ADJ;
			if (StatusFlags == 8) {
				//Boost mode was enabled before
				//We need to cancel the temp
				systemSettings.SolderingTemp = temporaryTempStorage;
				StatusFlags = 0;
			}
		} else if (Buttons == (BUT_A | BUT_B)) {

			//Both buttons were pressed, exit back to the cooling screen
			operatingMode = COOLING;
			if (StatusFlags == 8) {
				//Boost mode was enabled before
				//We need to cancel the temp
				systemSettings.SolderingTemp = temporaryTempStorage;
				StatusFlags = 0;
			}

		} else if ((getRawButtons() == BUT_A && systemSettings.boostModeEnabled)) {
			if (StatusFlags != 8) {
				StatusFlags = 8;
				temporaryTempStorage = systemSettings.SolderingTemp;
				systemSettings.SolderingTemp = systemSettings.BoostTemp;
			}
			//Update the PID Loop
			int32_t newOutput = computePID(systemSettings.SolderingTemp);
			setIronTimer(newOutput);
		} else {
			if (StatusFlags == 8) {
				//Boost mode was enabled before
				//We need to cancel the temp
				systemSettings.SolderingTemp = temporaryTempStorage;
				StatusFlags = 0;
			}
			//We need to check the timer for movement in case we need to goto idle
			if (systemSettings.sensitivity) {
				uint32_t timeout = 0;
				if (systemSettings.SleepTime < 6)
					timeout = 1000 * 10 * systemSettings.SleepTime;
				else
					timeout = 60 * 1000 * (systemSettings.SleepTime - 5);

				if (millis() - getLastMovement() > (timeout)) {
					if (millis() - getLastButtonPress() > (timeout)) {
						operatingMode = SLEEP;
						return;
					}
				}
			}
			uint16_t voltage = readDCVoltage(systemSettings.voltageDiv); //get X10 voltage
			if ((voltage) < lookupVoltageLevel(systemSettings.cutoutSetting)) {
				operatingMode = UVLOWARN;
				lastModeChange = millis();
			}
			//Update the PID Loop
			int32_t newOutput = computePID(systemSettings.SolderingTemp);
			setIronTimer(newOutput);
		}
		break;
	case TEMP_ADJ:
		if (OLED_GetOrientation() == 1) {
			if (Buttons == BUT_B) {
				//A key pressed so we are moving down in temp

				if (systemSettings.SolderingTemp > 1000)
					systemSettings.SolderingTemp -= 100;
			} else if (Buttons == BUT_A) {
				//B key pressed so we are moving up in temp
				if (systemSettings.SolderingTemp < 4500)
					systemSettings.SolderingTemp += 100;
			} else {
				//we check the timeout for how long the buttons have not been pushed
				//if idle for > 3 seconds then we return to soldering
				//Or if both buttons pressed
				if (Buttons == (BUT_A | BUT_B)) {
					operatingMode = STARTUP;
					saveSettings();
				} else if ((millis() - getLastButtonPress() > 2000)) {
					operatingMode = SOLDERING;
				}

			}
		} else {
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
		}
		break;
	case SETTINGS:
		//Settings is the mode with the most logic
		//Here we are in the menu so we need to increment through the sub menus / increase the value
		if (StatusFlags == 4 && Buttons != 0) {
			//The user pressed the button to breakout of the settings help prompt
			StatusFlags = 0;
		} else {
			if (Buttons == (BUT_A | BUT_B)) {

				//Both buttons were pressed, exit back to the startup screen
				settingsPage = 0;				//reset
				operatingMode = STARTUP;	//reset back to the startup
				saveSettings();					//Save the settings
				StartUp_Accelerometer(systemSettings.sensitivity); //Start the accelerometer

			} else if (Buttons & BUT_B) {
				//A key iterates through the menu
				if (settingsPage == SETTINGSOPTIONSCOUNT) {
					//Roll off the end
					settingsPage = 0;				//reset
					operatingMode = STARTUP;	//reset back to the startup
					saveSettings();					//Save the settings
					StartUp_Accelerometer(systemSettings.sensitivity); //Start the accelerometer

				} else {
					++settingsPage;			//move to the next option
				}
			} else if (Buttons & BUT_A) {
				//B changes the value selected
				switch (settingsPage) {
				case UVCO:
					//we are incrementing the cutout voltage
					systemSettings.cutoutSetting += 1;	//Go up 1V at a jump
					systemSettings.cutoutSetting %= 5;		//wrap 0->4
					break;
				case SLEEP_TEMP:
					systemSettings.SleepTemp += 100;	//Go up 10C at a time
					if (systemSettings.SleepTemp > 3000)
						systemSettings.SleepTemp = 500;	//cant sleep higher than 300 or less than 50
					break;
				case SLEEP_TIME:
					++systemSettings.SleepTime;	//Go up 1 minute at a time
					if (systemSettings.SleepTime > 16)
						systemSettings.SleepTime = 1;//can't set time over 30 mins
					//Remember that ^ is the time of no movement
					break;
				case SHUTDOWN_TIME:
					++systemSettings.ShutdownTime;
					if (systemSettings.ShutdownTime > 60)
						systemSettings.ShutdownTime = 0;	//wrap to off
					break;
				case TEMPDISPLAY:
					systemSettings.displayTempInF =
							!systemSettings.displayTempInF;
					break;
				case SCREENROTATION:
					systemSettings.OrientationMode++;
					systemSettings.OrientationMode =
							systemSettings.OrientationMode % 3;

					break;
				case MOTIONSENSITIVITY:
					systemSettings.sensitivity++;
					systemSettings.sensitivity = systemSettings.sensitivity
							% 10;

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
				case BOOSTMODE:
					systemSettings.boostModeEnabled =
							!systemSettings.boostModeEnabled;
					break;
				case BOOSTTEMP:
					systemSettings.BoostTemp += 100;	//Go up 10C at a time
					if (systemSettings.BoostTemp > 4500)
						systemSettings.BoostTemp = 2500;	//loop back at 250
					break;
				case POWERDISPLAY:
					systemSettings.powerDisplay = !systemSettings.powerDisplay;
					break;
				case AUTOSTART:
					systemSettings.autoStart++;
					systemSettings.autoStart %= 3;
					break;
				case COOLINGBLINK:
					systemSettings.coolingTempBlink =
							!systemSettings.coolingTempBlink;
					break;
#ifdef PIDTUNING
					case PIDP:
					pidSettings.kp++;
					pidSettings.kp %= 20;
					break;
					case PIDI:
					pidSettings.ki++;
					pidSettings.ki %= 10;
					break;
					case PIDD:
					pidSettings.kd++;
					pidSettings.kd %= 30;
					break;
#endif
				default:
					break;
				}
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
		} else if (systemSettings.sensitivity && !InterruptMask) {
			if (millis() - getLastMovement() < 1000) {//moved in the last second
				operatingMode = SOLDERING; //Goto active mode again
				Oled_DisplayOn();
				return;
			}
		}
		if (systemSettings.sensitivity) {
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
		if (Buttons & (BUT_A | BUT_B)) { //we check if the user has pushed a button to exit
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
			StatusFlags = 0;
			operatingMode = DCINDISP;
		} else if (Buttons == BUT_B) {
			StatusFlags = 0;
			operatingMode = TEMPCAL;
		} else if (Buttons == (BUT_A | BUT_B)) {
			//If the user is holding both button, exit the  screen
			operatingMode = STARTUP;
		}

	}
		break;
	case DCINDISP: {
		//This lets the user check the input voltage
		if (StatusFlags == 0) {
			if (Buttons == BUT_A) {
				//Single button press, cycle over to the temp display
				operatingMode = THERMOMETER;
			} else if (Buttons == BUT_B) {
				//dc cal mode
				StatusFlags = 1;
			} else if (Buttons == (BUT_A | BUT_B)) {
				//If the user is holding both button, exit the  screen
				operatingMode = STARTUP;
			}
		} else {
			//User is calibrating the dc input
			if (Buttons == BUT_A) {
				if (!systemSettings.OrientationMode)
					systemSettings.voltageDiv++;
				else
					systemSettings.voltageDiv--;
			} else if (Buttons == BUT_B) {
				if (!systemSettings.OrientationMode)
					systemSettings.voltageDiv--;
				else
					systemSettings.voltageDiv++;
			} else if (Buttons == (BUT_A | BUT_B)) {
				StatusFlags = 0;
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
			if (StatusFlags == 0) {
				if ((readTipTemp() < 300) && (readSensorTemp() < 300)) {
					StatusFlags = 1;
					systemSettings.tempCalibration = readTipTemp();
					saveSettings();
				} else {
					StatusFlags = 2;
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
	if (systemSettings.sensitivity) {
		if (millis() - getLastMovement() > (5 * 60 * 1000)
				&& (millis() - getLastButtonPress() > (5 * 60 * 1000))
				&& (temp < 400)) {
			//OLED off
			Oled_DisplayOff();
		} else {
			Oled_DisplayOn();
		}
	}
	switch (operatingMode) {
	case STARTUP:
		//We are chilling in the idle mode
		//Check if movement in the last 5 minutes , if not sleep OLED

		OLED_DrawIDLELogo();	//Draw the icons for prompting the user
		temp = readIronTemp(0, 1, 0xFFFF);//to update the internal filter buffer

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

		Clear_Screen();
		Oled_DisplayOn();
		uint32_t tempavg = (temp + lastSolderingDrawnTemp1
				+ lastSolderingDrawnTemp2);
		tempavg /= 3;

		drawTemp(tempavg, 0, systemSettings.temperatureRounding);
		lastSolderingDrawnTemp1 = temp;
		lastSolderingDrawnTemp2 = lastSolderingDrawnTemp1;
		lastOLEDDrawTime = millis();

		//Now draw symbols
		if (StatusFlags == 8)
			OLED_DrawExtraFontChars(2, 4);
		else {
			OLED_DrawChar(' ', 4);
		}
		//Draw in battery symbol if desired
		if (systemSettings.cutoutSetting) {
			//User is on a lithium battery
			//we need to calculate which of the 10 levels they are on
			uint8_t cellCount = systemSettings.cutoutSetting + 2;
			uint16_t cellV = readDCVoltage(systemSettings.voltageDiv)
					/ cellCount;
			//Should give us approx cell voltage X10
			//Range is 42 -> 33 = 9 steps therefore we will use battery 1-10
			if (cellV < 33)
				cellV = 33;
			cellV -= 33;			//Should leave us a number of 0-9
			if (cellV > 9)
				cellV = 9;
			OLED_DrawSymbolChar(cellV + 1, 5);
		}

		if (systemSettings.displayTempInF) {
			//OLED_DrawChar(SettingTempFChar, 3);
			OLED_DrawExtraFontChars(0, 3);
		} else {
			//OLED_DrawChar(SettingTempCChar, 3);
			OLED_DrawExtraFontChars(1, 3);
		}
		//Optionally draw the arrows, or draw the power instead
		if (systemSettings.powerDisplay) {
			//We want to draw in a neat little bar graph of power being pushed to the tip
			//ofset 11
			uint16_t count = getIronTimer() / (1000 / 28);
			if (count > 28)
				count = 28;
			OLED_DrawWideChar((count), 6);
		} else {
			//Draw in the arrows if the user has the power display turned off
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
			StatusFlags = 4;
			//If the user has idled for > 3 seconds, show the long name for the selected setting instead
			//draw from settingsLongTestScrollPos through to end of screen
			uint8_t lengthLeft = strlen(SettingsLongNames[settingsPage])
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
				OLED_DrawString(SettingsShortNames[UVCO], 6);
				if (systemSettings.cutoutSetting == 0) {
					//DC
					OLED_DrawChar('D', 6);
					OLED_DrawChar('C', 7);
				} else {
					//S count
					OLED_DrawChar('2' + systemSettings.cutoutSetting, 6);
					OLED_DrawChar('S', 7);
				}
				break;
			case SLEEP_TEMP:
				OLED_DrawString(SettingsShortNames[SLEEP_TEMP], 5);
				OLED_DrawThreeNumber(systemSettings.SleepTemp / 10, 5);
				break;
			case SLEEP_TIME:
				Clear_Screen();
				OLED_DrawString(SettingsShortNames[SLEEP_TIME], 5);
				//Draw in the timescale
				if (systemSettings.SleepTime < 6) {
					OLED_DrawChar('S', 7);
					OLED_DrawTwoNumber(systemSettings.SleepTime * 10, 5);

				} else {
					OLED_DrawChar('M', 7);
					OLED_DrawTwoNumber(systemSettings.SleepTime - 5, 5);
				}
				break;
			case SHUTDOWN_TIME:
				OLED_DrawString(SettingsShortNames[SHUTDOWN_TIME], 6);
				OLED_DrawTwoNumber(systemSettings.ShutdownTime, 6);
				break;
			case TEMPDISPLAY:/*Are we showing in C or F ?*/
				OLED_DrawString(SettingsShortNames[TEMPDISPLAY], 7);
				if (systemSettings.displayTempInF)
					OLED_DrawChar(SettingTempFChar, 7);
				else
					OLED_DrawChar(SettingTempCChar, 7);
				break;
			case SCREENROTATION:
				OLED_DrawString(SettingsShortNames[SCREENROTATION], 7);

				switch (systemSettings.OrientationMode) {
				case 0:
					OLED_DrawChar(SettingRightChar, 7);
					break;
				case 1:
					OLED_DrawChar(SettingLeftChar, 7);
					break;
				case 2:
					OLED_DrawChar(SettingAutoChar, 7);
					break;
				}
				break;
			case MOTIONSENSITIVITY:
				OLED_DrawString(SettingsShortNames[MOTIONSENSITIVITY], 7);

				OLED_DrawChar('0' + systemSettings.sensitivity, 7);
				break;
			case TEMPROUNDING:
				//We are prompting the user about their display mode preferences
			{
				OLED_DrawString(SettingsShortNames[TEMPROUNDING], 7);

				switch (systemSettings.temperatureRounding) {
				case ROUNDING_NONE:
					OLED_DrawChar('1', 7);
					break;
				case ROUNDING_FIVE:
					OLED_DrawChar('5', 7);
					break;
				case ROUNDING_TEN:
					OLED_DrawChar('X', 7);
					break;
				}
			}
				break;
			case DISPUPDATERATE:
				//We are prompting the user about their display mode preferences
			{
				OLED_DrawString(SettingsShortNames[DISPUPDATERATE], 7);
				switch (systemSettings.displayUpdateSpeed) {
				case DISPLAYMODE_FAST:
					OLED_DrawChar(SettingFastChar, 7);
					break;
				case DISPLAYMODE_SLOW:
					OLED_DrawChar(SettingSlowChar, 7);
					break;
				case DISPLAYMODE_MEDIUM:
					OLED_DrawChar(SettingMediumChar, 7);
					break;

				}
			}
				break;
			case BOOSTMODE:
				OLED_DrawString(SettingsShortNames[BOOSTMODE], 7);

				switch (systemSettings.boostModeEnabled) {
				case 1:
					OLED_DrawChar(SettingTrueChar, 7);
					break;
				case 0:
					OLED_DrawChar(SettingFalseChar, 7);
					break;
				}
				break;
			case BOOSTTEMP:
				OLED_DrawString(SettingsShortNames[BOOSTTEMP], 5);
				OLED_DrawThreeNumber(systemSettings.BoostTemp / 10, 5);
				break;
			case POWERDISPLAY:
				OLED_DrawString(SettingsShortNames[POWERDISPLAY], 7);
				switch (systemSettings.powerDisplay) {
				case 1:
					OLED_DrawChar(SettingTrueChar, 7);
					break;
				case 0:
					OLED_DrawChar(SettingFalseChar, 7);
					break;

				}
				break;
			case AUTOSTART:
				OLED_DrawString(SettingsShortNames[AUTOSTART], 7);
				switch (systemSettings.autoStart) {
				case 1:
					OLED_DrawChar(SettingTrueChar, 7);
					break;
				case 0:
					OLED_DrawChar(SettingFalseChar, 7);
					break;
				case 2:
					OLED_DrawChar(SettingSleepChar, 7);
					break;
				}
				break;
			case COOLINGBLINK:
				OLED_DrawString(SettingsShortNames[COOLINGBLINK], 7);
				switch (systemSettings.coolingTempBlink) {
				case 1:
					OLED_DrawChar(SettingTrueChar, 7);
					break;
				case 0:
					OLED_DrawChar(SettingFalseChar, 7);
					break;
				}
				break;
#ifdef PIDTUNING
				case PIDP:
				OLED_DrawString("PIDP ", 5);
				OLED_DrawThreeNumber(pidSettings.kp, 5);
				break;
				case PIDI:
				OLED_DrawString("PIDI ", 5);
				OLED_DrawThreeNumber(pidSettings.ki, 5);
				break;
				case PIDD:
				OLED_DrawString("PIDD ", 5);
				OLED_DrawThreeNumber(pidSettings.kd, 5);
				break;
#endif
			default:

				break;
			}
		}
		break;
	case SLEEP:
		//The iron is in sleep temp mode
		//Draw in temp and sleep
		Clear_Screen();
		OLED_DrawString("SLP ", 4);
		drawTemp(temp, 4, systemSettings.temperatureRounding);
		Oled_DisplayOn();
		break;
	case COOLING:
		//We are warning the user the tip is cooling
		Clear_Screen();
		OLED_DrawString(CoolingPromptString, 5);
		temp = readIronTemp(0, 1, 0xFFFF);		//force temp re-reading

		if (temp > 500 && systemSettings.coolingTempBlink
				&& (millis() % 1000) > 500) {

		} else {
			drawTemp(temp, 5, systemSettings.temperatureRounding);
		}

		break;
	case UVLOWARN:
		OLED_DrawString(UVLOWarningString, 8);
		break;
	case THERMOMETER:
		temp = readIronTemp(0, 1, 0xFFFF);	//Force a reading as heater is off
		OLED_DrawString("Temp ", 5);//extra one to it clears the leftover 'L' from IDLE
		drawTemp(temp, 5, 0);
		break;
	case DCINDISP: {
		uint16_t voltage = readDCVoltage(systemSettings.voltageDiv); //get X10 voltage

		if (StatusFlags == 0 || ((millis() % 1000) > 500)) {

			OLED_DrawString("IN", 2);
			OLED_DrawChar(48 + ((voltage / 100) % 10), 2);
			voltage -= (voltage / 100) * 100;
			OLED_DrawChar(48 + ((voltage / 10) % 10), 3);
			voltage -= (voltage / 10) * 10;
			OLED_DrawChar('.', 4);
			OLED_DrawChar(48 + (voltage % 10), 5);
			OLED_DrawChar('V', 6);
			OLED_DrawChar(' ', 7);
		} else {
			OLED_DrawString("IN      ", 8);
		}
	}
		break;
	case TEMPCAL: {
		OLED_DrawString(TempCalStatus[StatusFlags], 8);
	}

		break;
	default:
		break;
	}

}
