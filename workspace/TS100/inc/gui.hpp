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
#include "hardware.h"

#define PRESS_ACCEL_STEP			3
#define PRESS_ACCEL_INTERVAL_MIN	10
#define PRESS_ACCEL_INTERVAL_MAX	30


//GUI holds the menu structure and all its methods for the menu itself

//Declarations for all the methods for the settings menu (at end of this file)

//Wrapper for holding a function pointer
typedef struct draw_func_t {
	void (*func)(void);
} draw_func;

typedef struct incr_func_t {
	bool (*func)(void);
} incr_func;

//Struct for holding the function pointers and descriptions
typedef struct {
	const char *description;
	const incr_func incrementHandler;
	const draw_func draw;
} menuitem;

void enterSettingsMenu();
extern const menuitem rootSettingsMenu[];

#endif /* GUI_HPP_ */
