/*
 * Modes.c
 *
 *  Created on: 17 Sep 2016
 *      Author: Ralim <ralim@ralimtek.com>
 */
#include "Modes.h"
const char *SettingsLongNames[] = {
		"      Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",
		"      Sleep Temperature <C>", "      Sleep Timeout <Minutes>",
		"      Shutdown Timeout <Minutes>",
		"      Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",
		"      Temperature Unit", "      Temperature Rounding Amount",
		"      Temperature Display Update Rate",
		"      Flip Display for Left Hand",
		"      Enable front key boost 450C mode when soldering",
		"      Temperature when in boost mode" };
uint8_t StatusFlags = 0;
uint32_t temporaryTempStorage = 0;
//This does the required processing and state changes
void ProcessUI() {
	uint8_t Buttons = getButtons(); //read the buttons status
	static uint32_t lastModeChange = 0;
	if (getRawButtons() && ((millis() - getLastButtonPress()) > 1000)) {
		lastKeyPress = millis() - 600;
		Buttons = getRawButtons();
	} else if (millis() - getLastButtonPress() < 80) {
		Buttons = 0;
	} else if (Buttons != 0) {
		resetButtons();
	}

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
			if (systemSettings.sensitivity)
				if (millis() - getLastMovement()
						> (systemSettings.SleepTime * 60000)) {
					if (millis() - getLastButtonPress()
							> (systemSettings.SleepTime * 60000)) {
						operatingMode = SLEEP;
						return;
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
		if (StatusFlags == 4 && Buttons != 0) {
			//The user pressed the button to breakout of the settings help prompt
			StatusFlags = 0;
		} else {
			if (Buttons & BUT_B) {
				//A key iterates through the menu
				if (settingsPage == SETTINGSOPTIONSCOUNT) {
					//Roll off the end
					settingsPage = 0;				//reset
					operatingMode = STARTUP;		//reset back to the startup
					saveSettings();					//Save the settings
				} else {
					++settingsPage;					//move to the next option
				}
			} else if (Buttons & BUT_A) {
				//B changes the value selected
				switch (settingsPage) {
				case UVCO:
					//we are incrementing the cutout voltage
					systemSettings.cutoutSetting += 1;		//Go up 1V at a jump
					systemSettings.cutoutSetting %= 5;		//wrap 0->4
					break;
				case SLEEP_TEMP:
					systemSettings.SleepTemp += 100;	//Go up 10C at a time
					if (systemSettings.SleepTemp > 3000)
						systemSettings.SleepTemp = 1000;//cant sleep higher than 300
					break;
				case SLEEP_TIME:
					++systemSettings.SleepTime;		//Go up 1 minute at a time
					if (systemSettings.SleepTime > 30)
						systemSettings.SleepTime = 1;//can't set time over 30 mins
					//Remember that ^ is the time of no movement
					break;
				case SHUTDOWN_TIME:
					++systemSettings.ShutdownTime;
					if (systemSettings.ShutdownTime > 60)
						systemSettings.ShutdownTime = 0;		//wrap to off
					break;
				case TEMPDISPLAY:
					systemSettings.displayTempInF =
							!systemSettings.displayTempInF;
					break;
				case LEFTY:
					systemSettings.flipDisplay = !systemSettings.flipDisplay;
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
		} else if (systemSettings.sensitivity) {
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
		if (systemSettings.sensitivity) {
			if (millis() - getLastMovement()
					> (systemSettings.ShutdownTime * 60000)) {
				if ((millis() - getLastButtonPress()
						> systemSettings.ShutdownTime * 60000)) {
					Oled_DisplayOff();
				}
			} else {
				Oled_DisplayOn();
			}
		} else
			Oled_DisplayOn();
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
		if (StatusFlags == 8)
			OLED_DrawChar('B', 4);
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
			OLED_DrawExtendedChar(cellV + 1, 5);
		} else {
			OLED_DrawChar(' ', 5);
		}
		OLED_BlankSlot(6 * 12 + 16, 24 - 16);//blank out the tail after the arrows
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
			OLED_DrawChar('F', 3);
		} else {
			OLED_DrawChar('C', 3);
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
				OLED_DrawString("PWRSC ", 6);
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
				OLED_DrawString("MSENSE ", 7);
				OLED_DrawChar('0' + systemSettings.sensitivity, 7);
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
			case BOOSTMODE:
				switch (systemSettings.boostModeEnabled) {
				case 1:
					OLED_DrawString("BOOST  T", 8);
					break;
				case 0:
					OLED_DrawString("BOOST  F", 8);
					break;
				}
				break;
			case BOOSTTEMP:
				OLED_DrawString("BTMP ", 5);
				OLED_DrawThreeNumber(systemSettings.BoostTemp / 10, 5);
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
		temp = readIronTemp(0, 1, 0xFFFF);		//force temp re-reading
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

		if (StatusFlags == 0 || ((millis() % 1000) > 500)) {
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

		if (StatusFlags == 0) {
			OLED_DrawString("CAL TEMP", 8);
		} else if (StatusFlags == 1) {
			OLED_DrawString("CAL OK  ", 8);
		} else if (StatusFlags == 2) {
			OLED_DrawString("CAL FAIL", 8);
		}
	}

		break;
	default:
		break;
	}

}
