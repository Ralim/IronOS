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
			if (millis() - getLastButtonPress() > 3000) {
				operatingMode = SOLDERING;
				saveSettings();
			}
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
			if (settingsPage == SETTINGSOPTIONSCOUNT) {
				//Roll off the end
				settingsPage = 0;				//reset
				operatingMode = STARTUP;		//reset back to the startup
				saveSettings();					//Save the settings
			} else
				++settingsPage;					//move to the next option
		} else if (Buttons & BUT_B) {
			resetLastButtonPress();
			//B changes the value selected
			switch (settingsPage) {
			case UVCO:
				//we are incrementing the cutout voltage
				systemSettings.cutoutVoltage += 1;		//Go up 1V at a jump
				if (systemSettings.cutoutVoltage > 24)
					systemSettings.cutoutVoltage = 10;
				break;
			case SLEEP_TEMP:
				systemSettings.SleepTemp += 100;		//Go up 10c at a time
				if (systemSettings.SleepTemp > 3000)
					systemSettings.SleepTemp = 1000;//cant sleep higher than 300
				break;
			case SLEEP_TIME:
				++systemSettings.SleepTime;		//Go up 1 minute at a time
				if (systemSettings.SleepTime > 30)
					systemSettings.SleepTime = 1;	//cant set time over 30 mins
				//Remember that ^ is the time of no movement
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
/*
 * Draws the temp with temp conversion if needed
 */
void drawTemp(uint16_t temp, uint8_t x) {
	if (systemSettings.displayTempInF)
		temp = (temp * 9 + 1600) / 5;/*Convert to F -> T*(9/5)+32*/
	OLED_DrawThreeNumber(temp / 10, x);
}

/*
 * Performs all the OLED drawing for the current operating mode
 */
void DrawUI() {
	uint16_t temp = readIronTemp(0, 0);
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
			OLED_DrawString("  IDLE  ", 8);		//write the word IDLE
		}
		break;
	case SOLDERING:
		//The user is soldering
	{
		if (getIronTimer() == 0) {
			OLED_DrawChar('C', 5);
		} else {
			if (getIronTimer() < 500) {
				OLED_DrawChar(' ', 5);
			} else {		//we are heating
				OLED_DrawChar('H', 5);
			}
		}
		drawTemp(temp, 0);
		OLED_DrawChar(' ', 3);
		OLED_DrawChar(' ', 4);
		OLED_DrawChar(' ', 6);
		OLED_DrawChar(' ', 7);

	}
		break;
	case TEMP_ADJ:
		//We are prompting the user to change the temp so we draw the current setpoint temp
		//With the nifty arrows

		OLED_DrawChar(' ', 0);
		OLED_DrawChar('<', 1);
		drawTemp(systemSettings.SolderingTemp, 2);
		OLED_DrawChar(' ', 5);
		OLED_DrawChar(' ', 6);
		OLED_DrawChar('>', 7);

		break;
	case SETTINGS:
		//We are prompting the user the setting name

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
			OLED_DrawString("STIME ", 6);
			OLED_DrawTwoNumber(systemSettings.SleepTime, 6);
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
		default:
			break;
		}
		break;
	case SLEEP:
		//The iron is in sleep temp mode
		//Draw in temp and sleep
		OLED_DrawString("SLP", 3);
		drawTemp(temp, 3);
		break;
	case COOLING:
		//We are warning the user the tip is cooling
		OLED_DrawString("COOL", 3);
		drawTemp(temp, 4);
		break;
	case UVLOWARN:
		OLED_DrawString("LOW VOLT", 8);
		break;
	default:
		break;
	}
}
