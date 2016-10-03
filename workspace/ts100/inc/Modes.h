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
enum {
	STARTUP, //we are sitting on the prompt to push a button
	SOLDERING,
	TEMP_ADJ,
	SETTINGS,
	SLEEP,
	COOLING,
	UVLOWARN,
} operatingMode;

enum {
	UVCO = 0, SLEEP_TEMP, SLEEP_TIME, MOTIONDETECT, TEMPDISPLAY,LEFTY

} settingsPage;

void ProcessUI();
void DrawUI();
#endif /* MODES_H_ */
