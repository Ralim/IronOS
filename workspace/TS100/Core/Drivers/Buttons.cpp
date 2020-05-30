/*
 * Buttons.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */
#include <Buttons.hpp>
#include "FreeRTOS.h"
#include "task.h"
#include "gui.hpp"
uint32_t lastButtonTime = 0;

ButtonState getButtonState() {
	/*
	 * Read in the buttons and then determine if a state change needs to occur
	 */

	/*
	 * If the previous state was  00 Then we want to latch the new state if
	 * different & update time
	 * If the previous state was !00 Then we want to search if we trigger long
	 * press (buttons still down), or if release we trigger press
	 * (downtime>filter)
	 */
	static uint8_t previousState = 0;
	static uint32_t previousStateChange = 0;
	const uint16_t timeout = 40;
	uint8_t currentState;
	currentState = (getButtonA()) << 0;
	currentState |= (getButtonB()) << 1;

	if (currentState)
		lastButtonTime = xTaskGetTickCount();
	if (currentState == previousState) {
		if (currentState == 0)
			return BUTTON_NONE;
		if ((xTaskGetTickCount() - previousStateChange) > timeout) {
			// User has been holding the button down
			// We want to send a button is held message
			if (currentState == 0x01)
				return BUTTON_F_LONG;
			else if (currentState == 0x02)
				return BUTTON_B_LONG;
			else
				return BUTTON_NONE; // Both being held case, we dont long hold this
		} else
			return BUTTON_NONE;
	} else {
		// A change in button state has occurred
		ButtonState retVal = BUTTON_NONE;
		if (currentState) {
			// User has pressed a button down (nothing done on down)
			if (currentState != previousState) {
				// There has been a change in the button states
				// If there is a rising edge on one of the buttons from double press we
				// want to mask that out As users are having issues with not release
				// both at once
				if (previousState == 0x03)
					currentState = 0x03;
			}
		} else {
			// User has released buttons
			// If they previously had the buttons down we want to check if they were <
			// long hold and trigger a press
			if ((xTaskGetTickCount() - previousStateChange) < timeout) {
				// The user didn't hold the button for long
				// So we send button press

				if (previousState == 0x01)
					retVal = BUTTON_F_SHORT;
				else if (previousState == 0x02)
					retVal = BUTTON_B_SHORT;
				else
					retVal = BUTTON_BOTH;  // Both being held case
			}
		}
		previousState = currentState;
		previousStateChange = xTaskGetTickCount();
		return retVal;
	}
	return BUTTON_NONE;
}

void waitForButtonPress() {
	// we are just lazy and sleep until user confirms button press
	// This also eats the button press event!
	ButtonState buttons = getButtonState();
	while (buttons) {
		buttons = getButtonState();
		GUIDelay();
	}
	while (!buttons) {
		buttons = getButtonState();
		GUIDelay();
	}
}

void waitForButtonPressOrTimeout(uint32_t timeout) {
	timeout += xTaskGetTickCount();
	// calculate the exit point

	ButtonState buttons = getButtonState();
	while (buttons) {
		buttons = getButtonState();
		GUIDelay();
		if (xTaskGetTickCount() > timeout)
			return;
	}
	while (!buttons) {
		buttons = getButtonState();
		GUIDelay();
		if (xTaskGetTickCount() > timeout)
			return;
	}
}
