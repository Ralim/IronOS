/*
 * Modes.c
 *
 *  Created on: 17 Sep 2016
 *      Author: Ralim <ralim@ralimtek.com>
 */
#include "Modes.h"
//This does the required processing and state changes
void ProcessUI() {
	uint8_t Buttons = getButtons(); //read the buttons status

	if (millis() - getLastButtonPress() < 150)
		Buttons = 0;
	//rough prevention for debouncing and allocates settling time
	//OLED_DrawThreeNumber(Buttons, 0);
	switch (operatingMode) {
	case STARTUP:
		if (Buttons & BUT_A) {
			//A key pressed so we are moving to soldering mode
			operatingMode = SOLDERING;
			resetLastButtonPress();
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving to the Settings menu
			operatingMode = SETTINGS;
			resetLastButtonPress();
		}
		//Nothing else to check here
		break;
	case SOLDERING:
		//We need to check the buttons if we need to jump out
		if (Buttons & BUT_A) {
			//A key pressed so we are moving to temp set
			operatingMode = TEMP_ADJ;
			resetLastButtonPress();
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving back to idle
			operatingMode = STARTUP;
			resetLastButtonPress();
		} else {
			//We need to check the timer for movement in case we need to goto idle
			if (systemSettings.movementEnabled)
				if (millis() - getLastMovement()
						> (systemSettings.SleepTime * 60000)) {
					operatingMode = SLEEP; //Goto Sleep Mode
					return;
				}
			/*if (millis() - getLastButtonPress()
			 > (systemSettings.SleepTime * 60000))
			 operatingMode = SLEEP;
			 return;*/
			//If no buttons pushed we need to perform the PID loop for the iron temp
			int32_t newOutput = computePID(systemSettings.SolderingTemp);
			if (newOutput >= 0) {
				setIronTimer(newOutput);
			}
		}
		break;
	case TEMP_ADJ:
		if (Buttons & BUT_A) {
			//A key pressed so we are moving down in temp
			resetLastButtonPress();
			if (systemSettings.SolderingTemp > 1000)
				systemSettings.SolderingTemp -= 100;
		} else if (Buttons & BUT_B) {
			//B key pressed so we are moving up in temp
			resetLastButtonPress();
			if (systemSettings.SolderingTemp < 4500)
				systemSettings.SolderingTemp += 100;
		} else {
			//we check the timeout for how long the buttons have not been pushed
			//if idle for > 3 seconds then we return to soldering
			if (millis() - getLastButtonPress() > 3000)
				operatingMode = SOLDERING;
		}
		break;
	case SETTINGS:
		//Settings is the mode with the most logic
		//Here we are in the menu so we need to increment through the sub menus / increase the value
		if (millis() - getLastButtonPress() < 300)
			return;

		if (Buttons & BUT_A) {
			resetLastButtonPress();
			//A key iterates through the menu
			if (settingsPage == 2) {
				//Roll off the end
				settingsPage = 0;		//reset
				operatingMode = STARTUP;
				//TODO Save settings
			} else
				++settingsPage;		//move to the next option
		} else if (Buttons & BUT_B) {
			resetLastButtonPress();
			//B changes the value selected
			switch (settingsPage) {
			case UVLO:
				//we are incrementing the cutout voltage
				systemSettings.cutoutVoltage += 1;		//Go up 1V at a jump
				if (systemSettings.cutoutVoltage > 24)
					systemSettings.cutoutVoltage = 9;
				else if (systemSettings.cutoutVoltage < 9)
					systemSettings.cutoutVoltage = 9;	//cant set UVLO below 9V
				break;
			case SLEEP_TEMP:
				systemSettings.SleepTemp += 100;		//Go up 10c at a time
				if (systemSettings.SleepTemp > 3000)
					systemSettings.SleepTemp = 1000;//cant sleep higher than 300
				break;
			case SLEEP_TIME:
				++systemSettings.SleepTime;		//Go up 1 minute at a time
				if (systemSettings.SleepTime > 60)
					systemSettings.SleepTime = 2;	//cant set time over an hour
				//Remember that ^ is the time of no movement
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
			return;
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving back to soldering
			operatingMode = SOLDERING;
			return;
		} else if (systemSettings.movementEnabled)
			if (millis() - getLastMovement() > 100) {
				operatingMode = SOLDERING; //Goto active mode again
				return;
			}
		//else if nothing has been pushed we need to compute the PID to keep the iron at the sleep temp
		int32_t newOutput = computePID(systemSettings.SleepTemp);
		if (newOutput >= 0) {
			setIronTimer(newOutput);
		}
		break;
	default:
		break;
	}
}

void DrawUI() {
	switch (operatingMode) {
	case STARTUP:
		//We are chilling in the idle mode
		//Check if movement in the last 5 minutes , if not sleep OLED
		if (millis() - getLastMovement() > (5 * 60 * 1000)) {
			//OLED off
			Oled_DisplayOff();
		} else {
			Oled_DisplayOn();
			OLED_DrawString("IDLE   ", 7);		//write the word IDLE
		}
		break;
	case SOLDERING:
		//The user is soldering
	{
		uint16_t temp = readIronTemp(0, 0);
		if (systemSettings.SolderingTemp < temp) {
			OLED_DrawChar('C', 14 * 4);
		} else {
			if (systemSettings.SolderingTemp - temp < 20) {
				OLED_DrawChar(' ', 14 * 4);
			} else {		//we are heating
				OLED_DrawChar('H', 14 * 4);
			}

		}
		OLED_DrawThreeNumber(temp / 10, 0);
		OLED_DrawChar('C', 14 * 3);
		OLED_DrawChar(' ', 14 * 4);
		OLED_DrawChar(' ', 14 * 5);
	}
		break;
	case TEMP_ADJ:
		//We are prompting the user to change the temp so we draw the current setpoint temp
		//With the nifty arrows
		OLED_DrawChar(' ', 0);
		OLED_DrawThreeNumber(systemSettings.SolderingTemp / 10, 14 * 1);
		OLED_DrawChar(' ', 14 * 4);
		OLED_DrawChar(' ', 14 * 5);

		break;
	case SETTINGS:
		//We are prompting the user the setting name

		switch (settingsPage) {
		case UVLO:
			OLED_DrawString("UVLO", 4);
			OLED_DrawTwoNumber(systemSettings.cutoutVoltage, 14 * 4);
			//OLED_DrawChar('V', 14 * 5);

			break;
		case SLEEP_TEMP:
			OLED_DrawString("STMP", 4);
			OLED_DrawThreeNumber(systemSettings.SleepTemp / 10, 14 * 4);
			//OLED_DrawChar('V', 14 * 5);

			break;
		case SLEEP_TIME:
			OLED_DrawString("STME ", 5);
			OLED_DrawTwoNumber(systemSettings.SleepTime, 14 * 5);
			break;
		default:
			break;
		}
		break;
	case SLEEP:
		//The iron is in sleep temp mode
		//Draw in temp and sleep
		OLED_DrawString("SLEP  ", 6);
		break;
	default:
		break;
	}
}
