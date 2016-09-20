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

	if (millis() - getLastButtonPress() < 100)
		Buttons = 0;
	//rough prevention for debouncing and allocates settling time
	//OLED_DrawThreeNumber(Buttons, 0);
	switch (operatingMode) {
	case STARTUP:
		if (Buttons & BUT_A) {
			//A key pressed so we are moving to soldering mode
			operatingMode = SOLDERING;
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving to the Settings menu
			operatingMode = SETTINGS;
		}
		//Nothing else to check here
		break;
	case SOLDERING:
		//We need to check the buttons if we need to jump out
		if (Buttons & BUT_A) {
			//A key pressed so we are moving to temp set
			operatingMode = TEMP_ADJ;
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving back to idle
			operatingMode = STARTUP;
		} else {
			//We need to check the timer for movement in case we need to goto idle
			if (systemSettings.movementEnabled)
				if (millis() - getLastMovement()
						> (systemSettings.SleepTime * 60000)) {
					operatingMode = SLEEP; //Goto Sleep Mode
					return;
				}
			//If no buttons pushed we need to read the current temperatures
			// and then setup the drive signal time for the iron
		}
		break;
	case TEMP_ADJ:
		if (Buttons & BUT_A) {
			//A key pressed so we are moving down in temp
		} else if (Buttons & BUT_B) {
			//B key pressed so we are moving up in temp

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
		if (Buttons & BUT_A) {
			//A key iterates through the menu
			if (settingsPage == 2) {
				//Roll off the end
				settingsPage = 0;		//reset
				operatingMode = STARTUP;
				//TODO Save settings
			} else
				++settingsPage;		//move to the next option
		} else if (Buttons & BUT_B) {
			//B changes the value selected
			switch (settingsPage) {
			case UVLO:
				//we are incrementing the cutout voltage
				systemSettings.cutoutVoltage += 5;		//Go up 0.5V at a jump
				if (systemSettings.cutoutVoltage < 90)
					systemSettings.cutoutVoltage = 90;	//cant set UVLO below 9V
				break;
			case SLEEP_TEMP:
				systemSettings.SleepTemp += 10;		//Go up 10c at a time
				if (systemSettings.SleepTemp > 300)
					systemSettings.SleepTemp = 100;	//cant sleep higher than 300
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
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving back to soldering
			operatingMode = SOLDERING;
		} else if (systemSettings.movementEnabled)
			if (millis() - getLastMovement() > 100) {
				operatingMode = SOLDERING; //Goto active mode again
				return;
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
			OLED_DrawString("IDLE  ", 6);		//write the word IDLE
		}
		break;
	case SOLDERING:
		//The user is soldering
		//We draw in the current system temp and an indicator if the iron is heating or not
		//First lets draw the current tip temp
		//OLED_DrawString("SOLD  ", 6);
		OLED_DrawThreeNumber(readIronTemp(0), 0);

		break;
	case TEMP_ADJ:
		//We are prompting the user to change the temp so we draw the current setpoint temp
		//With the nifty arrows
		OLED_DrawString("TEMP  ", 6);
		break;
	case SETTINGS:
		//We are prompting the user the setting name
		OLED_DrawString("SETT  ", 6);
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
