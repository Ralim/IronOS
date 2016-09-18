/*
 * Modes.h
 *
 *  Created on: 17 Sep 2016
 *      Author: Ralim
 */

#ifndef MODES_H_
#define MODES_H_
#include "CTRL.h"
#include "Hardware.h"
enum
{
STARTUP,//we are sitting on the prompt to push a button
SOLDERING,
TEMP_ADJ,
SETTINGS,
} operatingMode;

enum {
	UVLO,
	SLEEP_TEMP,
	SLEEP_TIME,

}settingsPage;
#endif /* MODES_H_ */
