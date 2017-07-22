/*
 * Modes.h
 *
 *  Created on: 17 Sep 2016
 *      Author: Ralim
 *
 *      Modes.h -> Main function for driving the application
 *      This processes the buttons then does the gui
 */

#ifndef MODES_H_
#define MODES_H_
#include "Interrupt.h"
#include "S100V0_1.h"
#include "Oled.h"
#include "PID.h"
#include "Settings.h"
#include "Analog.h"
#include <string.h>
enum {
	STARTUP, 		//we are sitting on the prompt to push a button
	SOLDERING, 		//Normal operating mode
	TEMP_ADJ,		//Adjust the set temperature
	SETTINGS,		//Settings menu
	SLEEP,			//Iron is snoozing due to lack of use
	COOLING,		//Iron is cooling down -> Warning screen
	UVLOWARN,		//Unit tripped low voltage
	THERMOMETER,	//Read the tip temp
	DCINDISP,		//Disp the input voltage && Cal the DCin voltage divider
	TEMPCAL,		//Cal tip temp offset

} operatingMode;
#define SETTINGSOPTIONSCOUNT 10 /*Number of settings in the settings menu*/

enum {
	UVCO = 0,
	SLEEP_TEMP,
	SLEEP_TIME,
	SHUTDOWN_TIME,
	MOTIONSENSITIVITY,
	TEMPDISPLAY,
	TEMPROUNDING,
	DISPUPDATERATE,
	LEFTY,
	BOOSTMODE,
	BOOSTTEMP,
} settingsPage;

void ProcessUI();
void DrawUI();
#endif /* MODES_H_ */
