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
#include "Interrupt.h"
#include "Oled.h"
uint32_t LastButtonPushTime;
uint32_t LastMovementTime;
enum {
	STARTUP, //we are sitting on the prompt to push a button
	SOLDERING,
	TEMP_ADJ,
	SETTINGS,
	SLEEP,
} operatingMode;

enum {
	UVLO = 0, SLEEP_TEMP, SLEEP_TIME,

} settingsPage;
struct {
	uint32_t SolderingTemp; //current setpoint for the iron
	uint8_t SleepTime; //minutes to sleep
	uint32_t SleepTemp; //temp to drop to in sleep
	uint8_t cutoutVoltage; //X10 the voltage we cutout at for undervoltage
} systemSettings;
#endif /* MODES_H_ */
