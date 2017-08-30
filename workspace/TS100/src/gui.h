/*
 * gui.h
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#ifndef GUI_H_
#define GUI_H_

//GUI holds the menu structure and all its methods for the menu itself

#include "main.hpp"
#include "Settings.h"
#include "Translation.h"
//Declarations for all the methods for the settings menu (at end of this file)


//Wrapper for holding a function pointer
typedef struct state_func_t {
	void (*func)(void);
} state_func;

//Struct for holding the function pointers and descriptions
typedef struct {
	const char *description;
	const state_func incrementHandler;
	const state_func draw;
} menuitem;

extern bool settingsResetRequest;
extern const menuitem settingsMenu[];
#endif /* GUI_H_ */
