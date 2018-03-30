#ifndef __MAIN_H
#define __MAIN_H

#include <MMA8652FC.hpp>
#include "Setup.h"
#include "OLED.hpp"

extern OLED lcd;
extern MMA8652FC accel;
extern uint8_t PCBVersion;
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

ButtonState getButtonState();
void waitForButtonPressOrTimeout(uint32_t timeout);

#endif /* __MAIN_H */
