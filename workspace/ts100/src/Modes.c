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
	static uint32_t lastModeChange = 0;
	if (millis() - getLastButtonPress() < 200)
		Buttons = 0;
	//rough prevention for de-bouncing and allocates settling time

	switch (operatingMode) {
	case STARTUP:
		if ((millis() - getLastButtonPress() > 1000)) {
			if (Buttons & BUT_A) {
				//A key pressed so we are moving to soldering mode
				operatingMode = SOLDERING;
				resetLastButtonPress();
				resetButtons();
			} else if (Buttons & BUT_B) {
				//B Button was pressed so we are moving to the Settings menu
				operatingMode = SETTINGS;
				resetLastButtonPress();
				resetButtons();
			}
		}
		//Nothing else to check here
		break;
	case SOLDERING:
		//We need to check the buttons if we need to jump out
		if (Buttons & BUT_A) {
			//A key pressed so we are moving to temp set
			operatingMode = TEMP_ADJ;
			resetLastButtonPress();
			resetButtons();
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving back to idle
			operatingMode = COOLING;
			resetLastButtonPress();
			resetButtons();
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
			uint16_t voltage = readDCVoltage(); //get X10 voltage
			if ((voltage / 10) < systemSettings.cutoutVoltage) {
				operatingMode = UVLOWARN;
				resetLastButtonPress();
				resetButtons();
				lastModeChange = millis();
			}
			//If no buttons pushed we need to perform the PID loop for the iron temp
			int32_t newOutput = computePID(systemSettings.SolderingTemp);

				setIronTimer(newOutput);

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
		if (millis() - getLastButtonPress() < 400)
			return;

		if (Buttons & BUT_A) {
			resetLastButtonPress();
			//A key iterates through the menu
			if (settingsPage == 3) {
				//Roll off the end
				settingsPage = 0;		//reset
				operatingMode = STARTUP;
				saveSettings();		//Save the settings
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
			case MOTIONDETECT:
				systemSettings.movementEnabled =
						!systemSettings.movementEnabled;
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
			resetLastButtonPress();
			resetButtons();
			return;
		} else if (Buttons & BUT_B) {
			//B Button was pressed so we are moving back to soldering
			operatingMode = SOLDERING;
			resetLastButtonPress();
			resetButtons();
			return;
		} else if (systemSettings.movementEnabled)
			if (millis() - getLastMovement() < 1000) {//moved in the last second
				operatingMode = SOLDERING; //Goto active mode again
				return;
			}
		//else if nothing has been pushed we need to compute the PID to keep the iron at the sleep temp
		int32_t newOutput = computePID(systemSettings.SleepTemp);

			setIronTimer(newOutput);

		break;
	case COOLING: {
		setIronTimer(0); //turn off heating
		//This mode warns the user the iron is still cooling down
		uint16_t temp = readIronTemp(0, 1); //take a new reading as the heater code is not taking new readings
		if (temp < 500) { //if the temp is < 50C then we can go back to IDLE
			operatingMode = STARTUP;
			resetLastButtonPress();
			resetButtons();
		} else { //we check if the user has pushed a button to ack
			if ((millis() - getLastButtonPress() > 200)
					&& (millis() - getLastButtonPress() < 2000)) {
				if (getButtons() && (BUT_A | BUT_B)) {
					//A button was pushed
					operatingMode = STARTUP;
					resetLastButtonPress();
					resetButtons();
				}
			}
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
	default:
		break;
	}
}

void DrawUI() {
	uint16_t temp = readIronTemp(0, 0) / 10;
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
			OLED_DrawString("IDLE   ", 7);		//write the word IDLE
		}
		break;
	case SOLDERING:
		//The user is soldering
	{
		if (getIronTimer() == 0) {
			OLED_DrawChar('C', 14 * 4);
		} else {
			if (getIronTimer() < 500) {
				OLED_DrawChar(' ', 14 * 4);
			} else {		//we are heating
				OLED_DrawChar('H', 14 * 4);
			}
		}
		OLED_DrawThreeNumber(temp, 0);
		OLED_DrawChar(' ', 14 * 3);
		OLED_DrawChar(' ', 14 * 5);
		OLED_DrawChar(' ', 14 * 6);
	}
		break;
	case TEMP_ADJ:
		//We are prompting the user to change the temp so we draw the current setpoint temp
		//With the nifty arrows
		OLED_DrawChar('<', 0);
		OLED_DrawThreeNumber(systemSettings.SolderingTemp / 10, 14 * 1);
		OLED_DrawChar(' ', 14 * 4);
		OLED_DrawChar('>', 14 * 5);
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
		case MOTIONDETECT:/*Toggle the mode*/
			if (systemSettings.movementEnabled)
				OLED_DrawString("MOTN  T", 7);
			else
				OLED_DrawString("MOTN  F", 7);
			break;
		default:
			break;
		}
		break;
	case SLEEP:
		//The iron is in sleep temp mode
		//Draw in temp and sleep
		OLED_DrawString("SLP", 3);
		OLED_DrawThreeNumber(temp, 14 * 3);
		break;
	case COOLING:
		//We are warning the user the tip is cooling
		OLED_DrawString("COL", 3);
		OLED_DrawThreeNumber(temp, 14 * 3);
		break;
	case UVLOWARN:
		OLED_DrawString("UND VL", 6);
		break;
	default:
		break;
	}
}
