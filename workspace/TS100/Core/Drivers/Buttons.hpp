/*
 * Buttons.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */
#include "BSP.h"
#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

extern uint32_t lastButtonTime;

enum ButtonState {
	BUTTON_NONE = 0, /* No buttons pressed / < filter time*/
	BUTTON_F_SHORT = 1, /* User has pressed the front button*/
	BUTTON_B_SHORT = 2, /* User has pressed the back  button*/
	BUTTON_F_LONG = 4, /* User is  holding the front button*/
	BUTTON_B_LONG = 8, /* User is  holding the back button*/
	BUTTON_BOTH = 16, /* User has pressed both buttons*/

/*
 * Note:
 * Pressed means press + release, we trigger on a full \__/ pulse
 * holding means it has gone low, and been low for longer than filter time
 */
};

//Returns what buttons are pressed (if any)
ButtonState getButtonState();
//Helpers
void waitForButtonPressOrTimeout(uint32_t timeout);
void waitForButtonPress();


#endif /* INC_BUTTONS_H_ */
