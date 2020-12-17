/*
 * gui.h
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#ifndef GUI_HPP_
#define GUI_HPP_
#include "Translation.h"
#include "Settings.h"
#include "BSP.h"

#define PRESS_ACCEL_STEP			30
#define PRESS_ACCEL_INTERVAL_MIN	100
#define PRESS_ACCEL_INTERVAL_MAX	300

//GUI holds the menu structure and all its methods for the menu itself

//Declarations for all the methods for the settings menu (at end of this file)

//Struct for holding the function pointers and descriptions
typedef struct {
	const char *description;
	// return true if increment reached the maximum value
	bool (* const incrementHandler)(void);
	void (* const draw)(void);
} menuitem;

void enterSettingsMenu();
void GUIDelay();
extern const menuitem rootSettingsMenu[];

#endif /* GUI_HPP_ */
