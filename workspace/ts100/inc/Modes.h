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
#include "MMA8652FC.h"
#include <string.h>
#include "Strings.h"

typedef enum {
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

} operatingModeEnum;
//#define PIDTUNING
#ifdef PIDTUNING
#define SETTINGSOPTIONSCOUNT (13+3) /*Number of settings in the settings menu*/
#else
#define SETTINGSOPTIONSCOUNT (13) /*Number of settings in the settings menu*/
#endif
typedef enum {
	UVCO = 0,
	SLEEP_TEMP,
	SLEEP_TIME,
	SHUTDOWN_TIME,
	MOTIONSENSITIVITY,
	TEMPDISPLAY,
	TEMPROUNDING,
	DISPUPDATERATE,
	SCREENROTATION,
	BOOSTMODE,
	BOOSTTEMP,
	POWERDISPLAY,
	AUTOSTART,
	COOLINGBLINK,
#ifdef PIDTUNING
PIDP,
PIDI,
PIDD,
#endif
} settingsPageEnum;

void ProcessUI();
void DrawUI();
extern operatingModeEnum operatingMode;
#endif /* MODES_H_ */
