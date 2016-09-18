/*
 * Modes.c
 *
 *  Created on: 17 Sep 2016
 *      Author: Ralim
 */
#include "Modes.h"
//This does the required processing and state changes
void ProcessUI() {
	uint8_t Buttons = Get_gKey(); //read the buttons status
	switch (operatingMode) {
	case STARTUP:
		if (Buttons & KEY_A) {
			//A key pressed so we are moving to soldering mode
			operatingMode = SOLDERING;
		} else if (Buttons & KEY_B) {
			//B Button was pressed so we are moving to the Settings menu
			operatingMode = SETTINGS;
		} else {
			//no buttons pressed so we sit tight
		}
		break;
	case SOLDERING:
		//We need to check the buttons if we need to jump out

		break;
	case TEMP_ADJ:
		break;
	case SETTINGS:
		break;
	default:
		break;
	}
}

void DrawUI() {
	switch (operatingMode) {
	case STARTUP:
		break;
	case SOLDERING:
		break;
	case TEMP_ADJ:
		break;
	case SETTINGS:
		break;
	default:
		break;
	}
}
