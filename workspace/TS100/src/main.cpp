// By Ben V. Brown - V2.0 of the TS100 firmware
#include <main.hpp>
#include <MMA8652FC.hpp>
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "OLED.hpp"
#include "Settings.h"
#include "Translation.h"
#include "string.h"
#include "gui.h"
#include "stdlib.h"
//C++ objects
OLED lcd(&hi2c1);
MMA8652FC accel(&hi2c1);

//File local variables
uint16_t currentlyActiveTemperatureTarget = 0;
uint32_t lastMovementTime = 0;
uint32_t lastButtonTime = 0;

// FreeRTOS variables
osThreadId GUITaskHandle;
osThreadId PIDTaskHandle;
osThreadId ROTTaskHandle;
osThreadId MOVTaskHandle;
SemaphoreHandle_t rotationChangedSemaphore = NULL;
SemaphoreHandle_t accelDataAvailableSemaphore = NULL;

void startGUITask(void const * argument);
void startPIDTask(void const * argument);
void startMOVTask(void const * argument);
void startRotationTask(void const * argument);
// End FreeRTOS

//Main inits hardware then hands over to the FreeRTOS kernel
int main(void) {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	Setup_HAL();    //Setup all the HAL objects
	setTipPWM(0);
	lcd.initialize();    //start up the LCD
	lcd.setFont(0);    //default to bigger font
	accel.initalize();    //this sets up the I2C registers and loads up the default settings
	HAL_IWDG_Refresh(&hiwdg);
	restoreSettings();    //load the settings from flash
	setCalibrationOffset(systemSettings.CalibrationOffset);
	HAL_IWDG_Refresh(&hiwdg);
	/* Create the thread(s) */
	/* definition and creation of GUITask */
	osThreadDef(GUITask, startGUITask, osPriorityBelowNormal, 0, 512);
	GUITaskHandle = osThreadCreate(osThread(GUITask), NULL);

	/* definition and creation of PIDTask */
	osThreadDef(PIDTask, startPIDTask, osPriorityHigh, 0, 256);
	PIDTaskHandle = osThreadCreate(osThread(PIDTask), NULL);

	/* definition and creation of ROTTask */
	osThreadDef(ROTTask, startRotationTask, osPriorityLow, 0, 256);
	ROTTaskHandle = osThreadCreate(osThread(ROTTask), NULL);
	/* definition and creation of MOVTask */
	osThreadDef(MOVTask, startMOVTask, osPriorityNormal, 0, 256);
	MOVTaskHandle = osThreadCreate(osThread(MOVTask), NULL);

	/* Create the objects*/
	rotationChangedSemaphore = xSemaphoreCreateBinary();    // Used to unlock rotation thread
	accelDataAvailableSemaphore = xSemaphoreCreateBinary();    // Used to unlock the movement thread
	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	while (1) {
	}
}

ButtonState getButtonState() {
	/*
	 * Read in the buttons and then determine if a state change needs to occur
	 */

	/*
	 * If the previous state was  00 Then we want to latch the new state if different & update time
	 * If the previous state was !00 Then we want to search if we trigger long press (buttons still down), or if release we trigger press (downtime>filter)
	 */
	static uint8_t previousState = 0;
	static uint32_t previousStateChange = 0;
	const uint16_t timeout = 400;
	uint8_t currentState;
	currentState = (HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin) == GPIO_PIN_RESET ? 1 : 0) << 0;
	currentState |= (HAL_GPIO_ReadPin(KEY_B_GPIO_Port, KEY_B_Pin) == GPIO_PIN_RESET ? 1 : 0) << 1;

	if (currentState)
		lastButtonTime = HAL_GetTick();
	if (currentState == previousState) {
		if (currentState == 0)
			return BUTTON_NONE;
		if ((HAL_GetTick() - previousStateChange) > timeout) {
			// User has been holding the button down
			// We want to send a buttong is held message
			if (currentState == 0x01)
				return BUTTON_F_LONG;
			else if (currentState == 0x02)
				return BUTTON_B_LONG;
			else
				return BUTTON_NONE;    // Both being held case, we dont long hold this
		} else
			return BUTTON_NONE;
	} else {
		// A change in button state has occurred
		ButtonState retVal = BUTTON_NONE;
		if (currentState) {
			//User has pressed a button down (nothing done on down)

		} else {
			// User has released buttons
			// If they previously had the buttons down we want to check if they were < long hold and trigger a press
			if ((HAL_GetTick() - previousStateChange) < timeout) {
				// The user didn't hold the button for long
				// So we send button press

				if (previousState == 0x01)
					retVal = BUTTON_F_SHORT;
				else if (previousState == 0x02)
					retVal = BUTTON_B_SHORT;
				else
					retVal = BUTTON_BOTH;    // Both being held case
			}

		}
		previousState = currentState;
		previousStateChange = HAL_GetTick();
		return retVal;
	}
	return BUTTON_NONE;
}

static void waitForButtonPress() {
	//we are just lazy and sleep until user confirms button press
	//This also eats the button press event!
	ButtonState buttons = getButtonState();
	while (buttons) {
		buttons = getButtonState();
		osDelay(100);
		HAL_IWDG_Refresh(&hiwdg);
		lcd.refresh();
	}
	while (!buttons) {
		buttons = getButtonState();

		osDelay(100);
		HAL_IWDG_Refresh(&hiwdg);
		lcd.refresh();
	}
}
static void waitForButtonPressOrTimeout(uint32_t timeout) {
	timeout += HAL_GetTick();
	//Make timeout our exit value
	for (;;) {
		ButtonState buttons = getButtonState();
		if (buttons)
			return;
		if (HAL_GetTick() > timeout)
			return;
		osDelay(50);
		HAL_IWDG_Refresh(&hiwdg);

	}
}

//returns true if undervoltage has occured
static bool checkVoltageForExit() {
	uint16_t v = getInputVoltageX10();
	if ((v < lookupVoltageLevel(systemSettings.cutoutSetting))) {
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (systemSettings.advancedScreens) {
			lcd.setFont(1);
			lcd.print("Undervoltage");
			lcd.setCursor(0, 8);
			lcd.print("Input V: ");
			lcd.printNumber(getInputVoltageX10() / 10, 2);
			lcd.drawChar('.');
			lcd.printNumber(getInputVoltageX10() % 10, 1);
			lcd.print("V");

		} else {
			lcd.setFont(0);
			lcd.print("DC LOW");
		}

		lcd.refresh();
		currentlyActiveTemperatureTarget = 0;
		waitForButtonPress();
		return true;
	}
	return false;
}

static void gui_solderingTempAdjust() {
	uint32_t lastChange = HAL_GetTick();
	currentlyActiveTemperatureTarget = 0;
	for (;;) {
		lcd.setCursor(0, 0);
		lcd.clearScreen();
		lcd.setFont(0);
		ButtonState buttons = getButtonState();
		if (buttons)
			lastChange = HAL_GetTick();
		switch (buttons) {
			case BUTTON_NONE:
				//stay
				break;
			case BUTTON_BOTH:
				//exit
				return;
				break;
			case BUTTON_B_LONG:

				break;
			case BUTTON_F_LONG:

				break;
			case BUTTON_F_SHORT:
				if (lcd.getRotation()) {
					systemSettings.SolderingTemp += 10;    //add 10C
					if (systemSettings.SolderingTemp > 450)
						systemSettings.SolderingTemp = 450;
				} else {
					systemSettings.SolderingTemp -= 10;    //sub 10C
					if (systemSettings.SolderingTemp <= 50)
						systemSettings.SolderingTemp = 50;
				}
				break;
			case BUTTON_B_SHORT:
				if (!lcd.getRotation()) {
					systemSettings.SolderingTemp += 10;    //add 10C
					if (systemSettings.SolderingTemp > 450)
						systemSettings.SolderingTemp = 450;
				} else {
					systemSettings.SolderingTemp -= 10;    //sub 10C
					if (systemSettings.SolderingTemp <= 50)
						systemSettings.SolderingTemp = 50;
				}
				break;
		}
		if (HAL_GetTick() - lastChange > 1500)
			return;    // exit if user just doesn't press anything for a bit
		lcd.drawChar('<');
		lcd.drawChar(' ');
		lcd.printNumber(systemSettings.SolderingTemp, 3);
		lcd.drawSymbol(1);
		lcd.drawChar('>');
		lcd.refresh();
		osDelay(10);
	}
}
static void gui_settingsMenu() {
//Draw the settings menu and provide iteration support etc
	uint8_t currentScreen = 0;
	uint32_t autoRepeatTimer = 0;
	settingsResetRequest = false;
	bool earlyExit = false;
	uint32_t descriptionStart = 0;
	while ((settingsMenu[currentScreen].description != NULL) && earlyExit == false) {
		lcd.setFont(0);
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (HAL_GetTick() - lastButtonTime < 4000) {
			settingsMenu[currentScreen].draw.func();
			descriptionStart = 0;
		} else {
			//Draw description
			//draw string starting from descriptionOffset

			int16_t maxOffset = strlen(settingsMenu[currentScreen].description);
			if (descriptionStart == 0)
				descriptionStart = HAL_GetTick();
			int16_t descriptionOffset = ((HAL_GetTick() - descriptionStart) / 150) % maxOffset;
			lcd.setCursor(12 * (7 - descriptionOffset), 0);
			lcd.print(settingsMenu[currentScreen].description);
		}
		ButtonState buttons = getButtonState();
		if (descriptionStart | (HAL_GetTick() - descriptionStart < 500))
			buttons = BUTTON_NONE;
		else {
			switch (buttons) {
				case BUTTON_BOTH:
					earlyExit = true;    //will make us exit next loop
					break;
				case BUTTON_F_SHORT:
					//increment
					settingsMenu[currentScreen].incrementHandler.func();
					break;
				case BUTTON_B_SHORT:
					currentScreen++;
					break;
				case BUTTON_F_LONG:
					if (HAL_GetTick() - autoRepeatTimer > 200) {
						settingsMenu[currentScreen].incrementHandler.func();
						autoRepeatTimer = HAL_GetTick();
					}
					break;
				case BUTTON_B_LONG:
					if (HAL_GetTick() - autoRepeatTimer > 200) {
						currentScreen++;
						autoRepeatTimer = HAL_GetTick();
					}
					break;
				case BUTTON_NONE:
					break;
			}
		}
		lcd.refresh();    //update the LCD
		osDelay(20);    //pause for a sec
		HAL_IWDG_Refresh(&hiwdg);

	}
	if (settingsResetRequest)
		resetSettings();
	saveSettings();
}
static void gui_showTipTempWarning() {
	for (;;) {
		uint16_t tipTemp = tipMeasurementToC(getTipRawTemp(0));
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (systemSettings.advancedScreens) {
			lcd.setFont(1);
			lcd.print("WARNING! TIP HOT!");
			lcd.setCursor(0, 8);
			lcd.print("Tip Temp: ");
			lcd.printNumber(tipTemp, 3);
			lcd.print("C");
		} else {
			lcd.setFont(0);

			lcd.print("WARN!");
			lcd.printNumber(tipTemp, 3);
		}
		if (systemSettings.coolingTempBlink) {
			if (HAL_GetTick() % 500 < 250)
				lcd.clearScreen();
		}
		lcd.refresh();
		ButtonState buttons = getButtonState();
		if (buttons == BUTTON_B_SHORT || buttons == BUTTON_F_SHORT || buttons == BUTTON_BOTH)
			return;
		if (tipTemp < 30)
			return;
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(200);
	}

}
static int gui_SolderingSleepingMode() {
//Drop to sleep temperature and display until movement or button press

	for (;;) {
		ButtonState buttons = getButtonState();
		if (buttons)
			return 0;
		if ((HAL_GetTick() - lastMovementTime < 1000) || (HAL_GetTick() - lastButtonTime < 1000))
			return 0;    //user moved or pressed a button, go back to soldering
		if (checkVoltageForExit())
			return 1;    //return non-zero on error

		currentlyActiveTemperatureTarget = ctoTipMeasurement(systemSettings.SleepTemp);
		//draw the lcd
		uint16_t tipTemp = tipMeasurementToC(getTipRawTemp(0));

		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (systemSettings.advancedScreens) {
			lcd.setFont(1);
			lcd.print("Sleeping...");
			lcd.setCursor(0, 8);
			lcd.print("Tip:");
			lcd.printNumber(tipTemp, 3);
			lcd.print(" VIN:");
			lcd.printNumber(getInputVoltageX10() / 10, 2);
			lcd.drawChar('.');
			lcd.printNumber(getInputVoltageX10() % 10, 1);
			lcd.print("V");
		} else {
			lcd.setFont(0);
			lcd.print("SLEEP");
			lcd.printNumber(tipTemp, 3);
		}
		if (lastMovementTime)
			if (((uint32_t) (HAL_GetTick() - lastMovementTime))
					> (uint32_t) (systemSettings.ShutdownTime * 60 * 1000)) {
				//shutdown
				currentlyActiveTemperatureTarget = 0;
				return 1;    //we want to exit soldering mode
			}
		lcd.refresh();
		osDelay(100);
		HAL_IWDG_Refresh(&hiwdg);

	}
}
static void gui_solderingMode() {
	/*
	 * * Soldering (gui_solderingMode)
	 * -> Main loop where we draw temp, and animations
	 * --> User presses buttons and they goto the temperature adjust screen
	 * ---> Display the current setpoint temperature
	 * ---> Use buttons to change forward and back on temperature
	 * ---> Both buttons or timeout for exiting
	 * --> Long hold front button to enter boost mode
	 * ---> Just temporarily sets the system into the alternate temperature for PID control
	 * --> Long hold back button to exit
	 * --> Double button to exit
	 */
	bool boostModeOn = false;
	uint32_t sleepThres = 0;
	if (systemSettings.SleepTime < 6)
		sleepThres = systemSettings.SleepTime * 10 * 1000;
	else
		sleepThres = (systemSettings.SleepTime - 5) * 60 * 1000;
	for (;;) {
		uint16_t tipTemp = getTipRawTemp(0);

		ButtonState buttons = getButtonState();
		switch (buttons) {
			case BUTTON_NONE:
				//stay
				boostModeOn = false;
				break;
			case BUTTON_BOTH:
				//exit
				return;
				break;
			case BUTTON_B_LONG:
				return;    //exit on back long hold
				break;
			case BUTTON_F_LONG:
				//if boost mode is enabled turn it on
				if (systemSettings.boostModeEnabled)
					boostModeOn = true;
				break;
			case BUTTON_F_SHORT:
			case BUTTON_B_SHORT: {
				uint16_t oldTemp = systemSettings.SolderingTemp;
				gui_solderingTempAdjust();    //goto adjust temp mode
				if (oldTemp != systemSettings.SolderingTemp) {
					saveSettings();    //only save on change
				}
			}
				break;
		}
		//else we update the screen information
		lcd.setCursor(0, 0);
		lcd.clearScreen();
		lcd.setFont(0);
		if (tipTemp > 16300) {
			lcd.print("BAD TIP");
			lcd.refresh();
			currentlyActiveTemperatureTarget = 0;
			waitForButtonPress();
			return;
		} else {
			//We switch the layout direction depending on the orientation of the lcd.
			if (lcd.getRotation()) {
				// battery
				if (systemSettings.cutoutSetting) {
					//User is on a lithium battery
					//we need to calculate which of the 10 levels they are on
					uint8_t cellCount = systemSettings.cutoutSetting + 2;
					uint16_t cellV = getInputVoltageX10() / cellCount;
					//Should give us approx cell voltage X10
					//Range is 42 -> 33 = 9 steps therefore we will use battery 1-10
					if (cellV < 33)
						cellV = 33;
					cellV -= 33;			//Should leave us a number of 0-9
					if (cellV > 9)
						cellV = 9;
					lcd.drawBattery(cellV + 1);
				} else
					lcd.drawChar(' ');			//print a blank spot if there is no battery symbol

				lcd.drawChar(' ');    // Space out gap between battery <-> temp

				lcd.printNumber(tipMeasurementToC(tipTemp), 3);    //Draw current tip temp
				lcd.drawSymbol(1);    //deg C

				//We draw boost arrow if boosting, or else gap temp <-> heat indicator
				if (boostModeOn)
					lcd.drawSymbol(2);
				else
					lcd.drawChar(' ');

				// Draw heating/cooling symbols
				//If tip PWM > 25% then we are 'heating'
				if (getTipPWM() > 25)
					lcd.drawSymbol(14);
				else
					lcd.drawSymbol(15);
			} else {

				// Draw heating/cooling symbols
				//If tip PWM > 25% then we are 'heating'
				if (getTipPWM() > 25)
					lcd.drawSymbol(14);
				else
					lcd.drawSymbol(15);
				//We draw boost arrow if boosting, or else gap temp <-> heat indicator
				if (boostModeOn)
					lcd.drawSymbol(2);
				else
					lcd.drawChar(' ');

				lcd.printNumber(tipMeasurementToC(tipTemp), 3);    //Draw current tip temp
				lcd.drawSymbol(1);    //deg C

				lcd.drawChar(' ');    // Space out gap between battery <-> temp

				if (systemSettings.cutoutSetting) {
					//User is on a lithium battery
					//we need to calculate which of the 10 levels they are on
					uint8_t cellCount = systemSettings.cutoutSetting + 2;
					uint16_t cellV = getInputVoltageX10() / cellCount;
					//Should give us approx cell voltage X10
					//Range is 42 -> 33 = 9 steps therefore we will use battery 1-10
					if (cellV < 33)
						cellV = 33;
					cellV -= 33;			//Should leave us a number of 0-9
					if (cellV > 9)
						cellV = 9;
					lcd.drawBattery(cellV + 1);
				} else
					lcd.drawChar(' ');			//print a blank spot if there is no battery symbol
			}
		}

		//Update the setpoints for the temperature
		if (boostModeOn)
			currentlyActiveTemperatureTarget = ctoTipMeasurement(systemSettings.BoostTemp);
		else
			currentlyActiveTemperatureTarget = ctoTipMeasurement(systemSettings.SolderingTemp);

		//Undervoltage test
		if (checkVoltageForExit()) {
			return;
		}

		lcd.refresh();
		if (systemSettings.sensitivity)
			if (HAL_GetTick() - lastMovementTime > sleepThres && HAL_GetTick() - lastButtonTime > sleepThres) {
				if (gui_SolderingSleepingMode())
					return;    //If the function returns non-0 then exit
			}
		osDelay(100);
		HAL_IWDG_Refresh(&hiwdg);
	}

}

/* StartGUITask function */
void startGUITask(void const * argument) {
	/*
	 * Main program states:
	 *
	 * * Soldering (gui_solderingMode)
	 * -> Main loop where we draw temp, and animations
	 * --> User presses buttons and they goto the temperature adjust screen
	 * ---> Display the current setpoint temperature
	 * ---> Use buttons to change forward and back on temperature
	 * ---> Both buttons or timeout for exiting
	 * --> Long hold front button to enter boost mode
	 * ---> Just temporarily sets the system into the alternate temperature for PID control
	 * --> Long hold back button to exit
	 * --> Double button to exit
	 * * Settings Menu (gui_settingsMenu)
	 * -> Show setting name
	 * --> If no button press for > 3 Seconds, scroll description
	 * -> If user presses back button, adjust the setting
	 * -> Currently the same as 1.x (future to make more depth based)
	 */

	uint8_t animationStep = 0;
	uint8_t tempWarningState = 0;
	if (systemSettings.autoStartMode) {
		//jump directly to the autostart mode
		if (systemSettings.autoStartMode == 1)
			gui_solderingMode();
	}
	HAL_IWDG_Refresh(&hiwdg);
	if (showBootLogoIfavailable())
		waitForButtonPressOrTimeout(1000);
	HAL_IWDG_Refresh(&hiwdg);
	/*
	 for (;;) {
	 HAL_IWDG_Refresh(&hiwdg);
	 lcd.clearScreen();
	 lcd.setCursor(0, 0);
	 lcd.setFont(0);
	 lcd.print("");
	 lcd.refresh();
	 osDelay(100);
	 HAL_IWDG_Refresh(&hiwdg);

	 }*/
	//^ Kept here for a way to block this thread
	for (;;) {
		ButtonState buttons = getButtonState();
		switch (buttons) {
			case BUTTON_NONE:
				//Do nothing
				break;
			case BUTTON_BOTH:
				//Not used yet
				break;

			case BUTTON_B_LONG:
				//Show the version information
			{
				lcd.clearScreen();			//Ensure the buffer starts clean
				lcd.setCursor(0, 0);		//Position the cursor at the 0,0 (top left)
				lcd.setFont(1);					//small font
				lcd.print((char*) "V2.00");    //Print version number
				lcd.setCursor(0, 8);    //second line
				lcd.print(__DATE__);    //print the compile date
				lcd.refresh();
				waitForButtonPress();
			}
				break;
			case BUTTON_F_LONG:
				gui_solderingTempAdjust();
				saveSettings();
				break;
			case BUTTON_F_SHORT:
				lcd.setFont(0);
				lcd.displayOnOff(true);    //turn lcd on
				gui_solderingMode();    //enter soldering mode
				tempWarningState = 0;    //make sure warning can show
				HAL_IWDG_Refresh(&hiwdg);
				osDelay(500);
				break;
			case BUTTON_B_SHORT:
				lcd.setFont(0);
				lcd.displayOnOff(true);    //turn lcd on
				gui_settingsMenu();    //enter the settings menu
				saveSettings();
				setCalibrationOffset(systemSettings.CalibrationOffset);
				HAL_IWDG_Refresh(&hiwdg);
				osDelay(500);
				//tempWarningState=0;//make sure warning can show
				break;
		}
		currentlyActiveTemperatureTarget = 0;    //ensure tip is off

		if (systemSettings.sensitivity) {
			if ((HAL_GetTick() - lastMovementTime) > 60000 && (HAL_GetTick() - lastButtonTime) > 60000)
				lcd.displayOnOff(false);    // turn lcd off when no movement
			else if (HAL_GetTick() - lastMovementTime < 1000 || HAL_GetTick() - lastButtonTime < 1000) /*Use short time for test, and prevent lots of I2C writes for no need*/
				lcd.displayOnOff(true);    //turn lcd back on

		}
		uint16_t tipTemp = tipMeasurementToC(getTipRawTemp(0));
		if (tipTemp > 600)
			tipTemp = 0;
		if (tipTemp > 50) {
			if (tempWarningState == 0) {
				currentlyActiveTemperatureTarget = 0;    //ensure tip is off
				gui_showTipTempWarning();
				tempWarningState = 1;
			}
		} else
			tempWarningState = 0;
		// Clear the lcd buffer
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (systemSettings.advancedScreens) {
			lcd.setFont(1);
			if (tipTemp > 470) {
				lcd.print("Tip Disconnected!");
			} else {
				lcd.print("Tip:");
				lcd.printNumber(tipTemp, 3);
				lcd.print(" ");
				lcd.print("Set:");
				lcd.printNumber(systemSettings.SolderingTemp, 3);
			}
			lcd.setCursor(0, 8);
			lcd.print("Input V: ");
			lcd.printNumber(getInputVoltageX10() / 10, 2);
			lcd.drawChar('.');
			lcd.printNumber(getInputVoltageX10() % 10, 1);
			lcd.print("V");

		} else {
			lcd.setFont(0);

			if (animationStep & 0x80) {
				if (animationStep & 0x08)
					lcd.drawArea(0, 0, 96, 8, Iron_RightArrow_UP);
				else
					lcd.drawArea(0, 0, 96, 8, Iron_RightArrow_DOWN);

				lcd.drawArea(0, 8, 96, 8, Iron_Base);
			} else {
				if (animationStep & 0x08)
					lcd.drawArea(0, 0, 96, 8, Iron_LeftArrow_UP);
				else
					lcd.drawArea(0, 0, 96, 8, Iron_LeftArrow_DOWN);
				lcd.drawArea(0, 8, 96, 8, Iron_Base);
			}
		}

		lcd.refresh();
		animationStep++;
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(80);

	}
}

/* StartPIDTask function */
void startPIDTask(void const * argument) {
	/*
	 * We take the current tip temperature & evaluate the next step for the tip control PWM
	 * Tip temperature is measured by getTipTemperature(1) so we get instant result
	 * This comes in Cx10 format
	 * We then control the tip temperature to aim for the setpoint in the settings struct
	 *
	 */
	int32_t integralCount = 0;
	int32_t derivativeLastValue = 0;
	int32_t kp, ki, kd, kb;
	int32_t backoffOverflow = 0;
	kp = 20;
	ki = 50;
	kd = 40;
	kb = 0;
	const int32_t itermMax = 40;
	for (;;) {
		uint16_t rawTemp = getTipRawTemp(1);    //get instantaneous reading
		if (currentlyActiveTemperatureTarget) {
			//Compute the PID loop in here
			//Because our values here are quite large for all measurements (0-16k ~= 33 counts per C)
			//P I & D are divisors, so inverse logic applies (beware)

			int32_t rawTempError = currentlyActiveTemperatureTarget - rawTemp;
			int32_t ierror = (rawTempError / ki);
			integralCount += ierror;
			if (integralCount > (itermMax / 2))
				integralCount = itermMax / 2;			//prevent too much lead
			else if (integralCount < -itermMax)
				integralCount = itermMax;

			int32_t dInput = (rawTemp - derivativeLastValue);

			/*Compute PID Output*/
			int32_t output = (rawTempError / kp);
			if (ki)
				output += integralCount;
			if (kd)
				output -= (dInput / kd);
			if (kb)
				output -= backoffOverflow / kb;

			if (output > 100) {
				backoffOverflow = output;
				output = 100;			//saturate
			} else if (output < 0) {
				backoffOverflow = output;
				output = 0;
			} else
				backoffOverflow = 0;

			setTipPWM(output);
		} else {
			setTipPWM(0);    //disable the output driver if the output is set to be off elsewhere
			integralCount = 0;
			backoffOverflow = 0;
		}
		derivativeLastValue = rawTemp;			//store for next loop
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(100);    // 10 Hz temp loop
	}
}
#define MOVFilter 4
void startMOVTask(void const * argument) {
	osDelay(4000);    //wait for accel to stabilize
	int16_t datax[MOVFilter];
	int16_t datay[MOVFilter];
	int16_t dataz[MOVFilter];
	uint8_t currentPointer = 0;
	memset(datax, 0, MOVFilter);
	memset(datay, 0, MOVFilter);
	memset(dataz, 0, MOVFilter);
	int16_t tx, ty, tz;
	int32_t avgx, avgy, avgz;

	for (;;) {
		int32_t threshold = 600 + (9 * 120);
		threshold -= systemSettings.sensitivity * 120;
		accel.getAxisReadings(&tx, &ty, &tz);

		datax[currentPointer] = tx;
		datay[currentPointer] = ty;
		dataz[currentPointer] = tz;
		currentPointer = (currentPointer + 1) % MOVFilter;

		//Only run the actual processing if the sensitivity is set (aka we are enabled)
		if (systemSettings.sensitivity) {
			//calculate averages
			avgx = avgy = avgz = 0;
			for (uint8_t i = 0; i < MOVFilter; i++) {
				avgx += datax[i];
				avgy += datay[i];
				avgz += dataz[i];
			}
			avgx /= MOVFilter;
			avgy /= MOVFilter;
			avgz /= MOVFilter;

			//So now we have averages, we want to look if these are different by more than the threshold
			int32_t error = (abs(avgx - tx) + abs(avgy - ty) + abs(avgz - tz));

			if (error > threshold) {
				lastMovementTime = HAL_GetTick();

			}

		}

		osDelay(10);

	}
}
/* StartRotationTask function */
void startRotationTask(void const * argument) {
	/*
	 * This task is used to manage rotation of the LCD screen & button re-mapping
	 *
	 */
	osDelay(1000);    //wait for accel to stabilize
	HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
//^ We hold off enabling these until now to ensure the semaphore is available to be used first
	switch (systemSettings.OrientationMode) {
		case 0:
			lcd.setRotation(false);
			break;
		case 1:
			lcd.setRotation(true);
			break;
		case 2:
			lcd.setRotation(false);
			break;
	}
	for (;;) {
		if ( xSemaphoreTake( rotationChangedSemaphore, portMAX_DELAY ) == pdTRUE
				|| (HAL_GPIO_ReadPin(INT_Orientation_GPIO_Port, INT_Orientation_Pin) == GPIO_PIN_RESET)) {
			// a rotation event has occured
			bool rotation = accel.getOrientation();
			if (systemSettings.OrientationMode == 2)
				lcd.setRotation(rotation);    // link the data through
		}
		osDelay(300);

	}
}

//Handler called by HAL when a EXTI occurs, but after IRQ bit is cleared
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	static signed long xHigherPriorityTaskWoken;
	if (GPIO_Pin == INT_Orientation_Pin) {
		xSemaphoreGiveFromISR(rotationChangedSemaphore, &xHigherPriorityTaskWoken);
	} else if (GPIO_Pin == INT_Movement_Pin) {
		//New data is available for reading from the unit
		//xSemaphoreGiveFromISR(accelDataAvailableSemaphore, &xHigherPriorityTaskWoken);

	}
}

#define FLASH_LOGOADDR 	(0x8000000|0xB800) /*second last page of flash set aside for logo image*/

bool showBootLogoIfavailable() {
//check if the header is there (0xAA,0x55,0xF0,0x0D)
//If so display logo
	uint16_t temp[98];

	for (uint8_t i = 0; i < (98); i++) {
		temp[i] = *(uint16_t *) (FLASH_LOGOADDR + (i * 2));
	}
	uint8_t temp8[98 * 2];
	for (uint8_t i = 0; i < 98; i++) {
		temp8[i * 2] = temp[i] >> 8;
		temp8[i * 2 + 1] = temp[i] & 0xFF;

	}

	if (temp8[0] != 0xAA)
		return false;
	if (temp8[1] != 0x55)
		return false;
	if (temp8[2] != 0xF0)
		return false;
	if (temp8[3] != 0x0D)
		return false;

	lcd.drawArea(0, 0, 96, 16, (uint8_t*) (temp8 + 4));
	lcd.refresh();
	return true;

}
