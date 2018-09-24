// By Ben V. Brown - V2.0 of the TS100 firmware
#include <MMA8652FC.hpp>
#include <main.hpp>
#include "OLED.hpp"
#include "Settings.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "LIS2DH12.hpp"
#include <gui.hpp>
#include "FRToSI2C.hpp"

#define ACCELDEBUG 0
// C++ objects
FRToSI2C i2cDev(&hi2c1);
OLED lcd(&i2cDev);
MMA8652FC accel(&i2cDev);
LIS2DH12 accel2(&i2cDev);
uint8_t PCBVersion = 0;
// File local variables
uint16_t currentlyActiveTemperatureTarget = 0;
uint32_t lastMovementTime = 0;
uint32_t lastButtonTime = 0;

// FreeRTOS variables
osThreadId GUITaskHandle;
osThreadId PIDTaskHandle;
osThreadId MOVTaskHandle;

static TaskHandle_t pidTaskNotification = NULL;

void startGUITask(void const *argument);
void startPIDTask(void const *argument);
void startMOVTask(void const *argument);
// End FreeRTOS

// Main sets up the hardware then hands over to the FreeRTOS kernel
int main(void) {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick.
	 */
	HAL_Init();
	Setup_HAL();  // Setup all the HAL objects
	HAL_IWDG_Refresh(&hiwdg);
	setTipPWM(0);
	lcd.initialize();   // start up the LCD
	lcd.setFont(0);     // default to bigger font
	//Testing for new weird board version
	uint8_t buffer[1];
	HAL_IWDG_Refresh(&hiwdg);
	if (HAL_I2C_Mem_Read(&hi2c1, 29 << 1, 0x0F, I2C_MEMADD_SIZE_8BIT, buffer, 1,
			1000) == HAL_OK) {
		PCBVersion = 1;
		accel.initalize(); // this sets up the I2C registers and loads up the default
						   // settings
	} else if (HAL_I2C_Mem_Read(&hi2c1, 25 << 1, 0x0F, I2C_MEMADD_SIZE_8BIT,
			buffer, 1, 1000) == HAL_OK) {
		PCBVersion = 2;
		//Setup the ST Accelerometer
		accel2.initalize();						   //startup the accelerometer
	} else {
		PCBVersion = 3;
		systemSettings.SleepTime = 0;
		systemSettings.ShutdownTime = 0;			//No accel -> disable sleep
		systemSettings.sensitivity = 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	restoreSettings();  // load the settings from flash
	setCalibrationOffset(systemSettings.CalibrationOffset);
	setTipType((enum TipType)systemSettings.tipType, systemSettings.customTipGain); //apply tip type selection
	HAL_IWDG_Refresh(&hiwdg);

	/* Create the thread(s) */
	/* definition and creation of GUITask */
	osThreadDef(GUITask, startGUITask, osPriorityBelowNormal, 0, 768);  //3k
	GUITaskHandle = osThreadCreate(osThread(GUITask), NULL);

	/* definition and creation of PIDTask */
	osThreadDef(PIDTask, startPIDTask, osPriorityRealtime, 0, 512);  //2k
	PIDTaskHandle = osThreadCreate(osThread(PIDTask), NULL);
	if (PCBVersion != 3) {
		/* definition and creation of MOVTask */
		osThreadDef(MOVTask, startMOVTask, osPriorityNormal, 0, 512);  //2k
		MOVTaskHandle = osThreadCreate(osThread(MOVTask), NULL);
	}

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	while (1) {
	}
}
void printVoltage() {
	lcd.printNumber(getInputVoltageX10(systemSettings.voltageDiv) / 10, 2);
	lcd.drawChar('.');
	lcd.printNumber(getInputVoltageX10(systemSettings.voltageDiv) % 10, 1);
}
void GUIDelay() {
	//Called in all UI looping tasks,
	//This limits the re-draw rate to the LCD and also lets the DMA run
	//As the gui task can very easily fill this bus with transactions, which will prevent the movement detection from running
	osDelay(50);
}
void gui_drawTipTemp(bool symbol) {
	// Draw tip temp handling unit conversion & tolerance near setpoint
	uint16_t Temp = getTipRawTemp(0);

	if (systemSettings.temperatureInF)
		Temp = tipMeasurementToF(Temp);
	else
		Temp = tipMeasurementToC(Temp);
	//[Disabled 24/11/2017] Round if nearby
	// if (abs(Temp - systemSettings.SolderingTemp) < 3)
	//	Temp = systemSettings.SolderingTemp;

	lcd.printNumber(Temp, 3);  // Draw the tip temp out finally
	if (symbol) {
		if (systemSettings.temperatureInF)
			lcd.print("F");
		else
			lcd.print("C");
	}
}
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
	currentState = (
			HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin) == GPIO_PIN_RESET ?
					1 : 0) << 0;
	currentState |= (
			HAL_GPIO_ReadPin(KEY_B_GPIO_Port, KEY_B_Pin) == GPIO_PIN_RESET ?
					1 : 0) << 1;

	if (currentState)
		lastButtonTime = xTaskGetTickCount();
	if (currentState == previousState) {
		if (currentState == 0)
			return BUTTON_NONE;
		if ((xTaskGetTickCount() - previousStateChange) > timeout) {
			// User has been holding the button down
			// We want to send a buttong is held message
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
		lcd.refresh();
		GUIDelay();
	}
	while (!buttons) {
		buttons = getButtonState();
		lcd.refresh();
		GUIDelay();
	}
}

void waitForButtonPressOrTimeout(uint32_t timeout) {
	timeout += xTaskGetTickCount();
	// Make timeout our exit value
	for (;;) {
		ButtonState buttons = getButtonState();
		if (buttons)
			return;
		if (xTaskGetTickCount() > timeout)
			return;
		GUIDelay();
	}
}

// returns true if undervoltage has occured
static bool checkVoltageForExit() {
	uint16_t v = getInputVoltageX10(systemSettings.voltageDiv);
	if ((v < lookupVoltageLevel(systemSettings.cutoutSetting))) {
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (systemSettings.detailedSoldering) {
			lcd.setFont(1);
			lcd.print(UndervoltageString);
			lcd.setCursor(0, 8);
			lcd.print(InputVoltageString);
			printVoltage();
			lcd.print("V");

		} else {
			lcd.setFont(0);
			lcd.print(UVLOWarningString);
		}

		lcd.refresh();
		currentlyActiveTemperatureTarget = 0;
		waitForButtonPress();
		return true;
	}
	return false;
}
static void gui_drawBatteryIcon() {
	if (systemSettings.cutoutSetting) {
		// User is on a lithium battery
		// we need to calculate which of the 10 levels they are on
		uint8_t cellCount = systemSettings.cutoutSetting + 2;
		uint16_t cellV = getInputVoltageX10(systemSettings.voltageDiv)
				/ cellCount;
		// Should give us approx cell voltage X10
		// Range is 42 -> 33 = 9 steps therefore we will use battery 1-10
		if (cellV < 33)
			cellV = 33;
		cellV -= 33;  // Should leave us a number of 0-9
		if (cellV > 9)
			cellV = 9;
		lcd.drawBattery(cellV + 1);
	} else
		lcd.drawSymbol(15);  // Draw the DC Logo
}
static void gui_solderingTempAdjust() {
	uint32_t lastChange = xTaskGetTickCount();
	currentlyActiveTemperatureTarget = 0;
	uint32_t autoRepeatTimer = 0;
	uint8_t autoRepeatAcceleration = 0;
	for (;;) {
		lcd.setCursor(0, 0);
		lcd.clearScreen();
		lcd.setFont(0);
		ButtonState buttons = getButtonState();
		if (buttons)
			lastChange = xTaskGetTickCount();
		switch (buttons) {
		case BUTTON_NONE:
			// stay
			break;
		case BUTTON_BOTH:
			// exit
			return;
			break;
		case BUTTON_B_LONG:
			if (xTaskGetTickCount() - autoRepeatTimer
					+ autoRepeatAcceleration> PRESS_ACCEL_INTERVAL_MAX) {
				systemSettings.SolderingTemp -= 10;  // sub 10
				autoRepeatTimer = xTaskGetTickCount();
				autoRepeatAcceleration += PRESS_ACCEL_STEP;
			}
			break;
		case BUTTON_F_LONG:
			if (xTaskGetTickCount() - autoRepeatTimer
					+ autoRepeatAcceleration> PRESS_ACCEL_INTERVAL_MAX) {
				systemSettings.SolderingTemp += 10;
				autoRepeatTimer = xTaskGetTickCount();
				autoRepeatAcceleration += PRESS_ACCEL_STEP;
			}
			break;
		case BUTTON_F_SHORT:
			systemSettings.SolderingTemp += 10;  // add 10
			break;
		case BUTTON_B_SHORT:
			systemSettings.SolderingTemp -= 10;  // sub 10
			break;
		default:
			break;
		}
		if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration)
				< PRESS_ACCEL_INTERVAL_MIN) {
			autoRepeatAcceleration = PRESS_ACCEL_INTERVAL_MAX
					- PRESS_ACCEL_INTERVAL_MIN;
		}
		// constrain between 50-450 C
		if (systemSettings.temperatureInF) {
			if (systemSettings.SolderingTemp > 850)
				systemSettings.SolderingTemp = 850;
			if (systemSettings.SolderingTemp < 120)
				systemSettings.SolderingTemp = 120;
		} else {
			if (systemSettings.SolderingTemp > 450)
				systemSettings.SolderingTemp = 450;
			if (systemSettings.SolderingTemp < 50)
				systemSettings.SolderingTemp = 50;
		}

		if (xTaskGetTickCount() - lastChange > 200)
			return;  // exit if user just doesn't press anything for a bit

		if (lcd.getRotation())
			lcd.drawChar('-');
		else
			lcd.drawChar('+');

		lcd.drawChar(' ');
		lcd.printNumber(systemSettings.SolderingTemp, 3);
		if (systemSettings.temperatureInF)
			lcd.drawSymbol(0);
		else
			lcd.drawSymbol(1);
		lcd.drawChar(' ');
		if (lcd.getRotation())
			lcd.drawChar('+');
		else
			lcd.drawChar('-');
		lcd.refresh();
		GUIDelay();
	}
}

static uint16_t min(uint16_t a, uint16_t b) {
	if (a > b)
		return b;
	else
		return a;
}
static int gui_SolderingSleepingMode() {
	// Drop to sleep temperature and display until movement or button press

	for (;;) {
		ButtonState buttons = getButtonState();
		if (buttons)
			return 0;
		if ((xTaskGetTickCount() - lastMovementTime < 100)
				|| (xTaskGetTickCount() - lastButtonTime < 100))
			return 0;  // user moved or pressed a button, go back to soldering
		if (checkVoltageForExit())
			return 1;  // return non-zero on error

		if (systemSettings.temperatureInF) {
			currentlyActiveTemperatureTarget = ftoTipMeasurement(
					min(systemSettings.SleepTemp,
							systemSettings.SolderingTemp));
		} else {
			currentlyActiveTemperatureTarget = ctoTipMeasurement(
					min(systemSettings.SleepTemp,
							systemSettings.SolderingTemp));
		}
		// draw the lcd
		uint16_t tipTemp;
		if (systemSettings.temperatureInF)
			tipTemp = tipMeasurementToF(getTipRawTemp(0));
		else
			tipTemp = tipMeasurementToC(getTipRawTemp(0));

		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (systemSettings.detailedSoldering) {
			lcd.setFont(1);
			lcd.print(SleepingAdvancedString);
			lcd.setCursor(0, 8);
			lcd.print(SleepingTipAdvancedString);
			lcd.printNumber(tipTemp, 3);
			if (systemSettings.temperatureInF)
				lcd.print("F");
			else
				lcd.print("C");

			lcd.print(" ");
			printVoltage();
			lcd.drawChar('V');
		} else {
			lcd.setFont(0);
			lcd.print(SleepingSimpleString);
			lcd.printNumber(tipTemp, 3);
			if (systemSettings.temperatureInF)
				lcd.drawSymbol(0);
			else
				lcd.drawSymbol(1);
		}
		if (systemSettings.ShutdownTime) // only allow shutdown exit if time > 0
			if (lastMovementTime)
				if (((uint32_t) (xTaskGetTickCount() - lastMovementTime))
						> (uint32_t) (systemSettings.ShutdownTime * 60 * 100)) {
					// shutdown
					currentlyActiveTemperatureTarget = 0;
					return 1;  // we want to exit soldering mode
				}
		lcd.refresh();
		GUIDelay();
	}
	return 0;
}

static void display_countdown(int sleepThres) {
	/*
	 * Print seconds or minutes (if > 99 seconds) until sleep
	 * mode is triggered.
	 */
	int lastEventTime =
			lastButtonTime < lastMovementTime ?
					lastMovementTime : lastButtonTime;
	int downCount = sleepThres - xTaskGetTickCount() + lastEventTime;
	if (downCount > 9900) {
		lcd.printNumber(downCount / 6000 + 1, 2);
		lcd.print("M");
	} else {
		lcd.printNumber(downCount / 100 + 1, 2);
		lcd.print("S");
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
	 * ---> Just temporarily sets the system into the alternate temperature for
	 * PID control
	 * --> Long hold back button to exit
	 * --> Double button to exit
	 */
	bool boostModeOn = false;
	uint32_t sleepThres = 0;
	if (systemSettings.SleepTime < 6)
		sleepThres = systemSettings.SleepTime * 10 * 100;
	else
		sleepThres = (systemSettings.SleepTime - 5) * 60 * 100;
	for (;;) {
		uint16_t tipTemp = getTipRawTemp(0);

		ButtonState buttons = getButtonState();
		switch (buttons) {
		case BUTTON_NONE:
			// stay
			boostModeOn = false;
			break;
		case BUTTON_BOTH:
			// exit
			return;
			break;
		case BUTTON_B_LONG:
			return;  // exit on back long hold
			break;
		case BUTTON_F_LONG:
			// if boost mode is enabled turn it on
			if (systemSettings.boostModeEnabled)
				boostModeOn = true;
			break;
		case BUTTON_F_SHORT:
		case BUTTON_B_SHORT: {
			uint16_t oldTemp = systemSettings.SolderingTemp;
			gui_solderingTempAdjust();  // goto adjust temp mode
			if (oldTemp != systemSettings.SolderingTemp) {
				saveSettings();  // only save on change
			}
		}
			break;
		default:
			break;
		}
		// else we update the screen information
		lcd.setCursor(0, 0);
		lcd.clearScreen();
		lcd.setFont(0);
		if (tipTemp > 32752) {
			lcd.print(BadTipString);
			lcd.refresh();
			currentlyActiveTemperatureTarget = 0;
			waitForButtonPress();
			return;
		} else {
			if (systemSettings.detailedSoldering) {
				lcd.setFont(1);/*
				 lcd.print(SolderingAdvancedPowerPrompt);  //Power:
				 lcd.printNumber(getTipPWM(), 3);
				 lcd.print("%");*/
				lcd.printNumber(getTipRawTemp(0), 6);

				if (systemSettings.sensitivity && systemSettings.SleepTime) {
					lcd.print(" ");
					display_countdown(sleepThres);
				}

				lcd.setCursor(0, 8);
				lcd.print(SleepingTipAdvancedString);
				gui_drawTipTemp(true);
				lcd.print(" ");
				printVoltage();
				lcd.drawChar('V');
			} else {
				// We switch the layout direction depending on the orientation of the lcd.
				if (lcd.getRotation()) {
					// battery
					gui_drawBatteryIcon();

					lcd.drawChar(' '); // Space out gap between battery <-> temp
					gui_drawTipTemp(true);  // Draw current tip temp

					// We draw boost arrow if boosting, or else gap temp <-> heat indicator
					if (boostModeOn)
						lcd.drawSymbol(2);
					else
						lcd.drawChar(' ');

					// Draw heating/cooling symbols
					lcd.drawHeatSymbol(getTipPWM());
				} else {
					// Draw heating/cooling symbols
					lcd.drawHeatSymbol(getTipPWM());
					// We draw boost arrow if boosting, or else gap temp <-> heat indicator
					if (boostModeOn)
						lcd.drawSymbol(2);
					else
						lcd.drawChar(' ');
					gui_drawTipTemp(true);  // Draw current tip temp

					lcd.drawChar(' '); // Space out gap between battery <-> temp

					gui_drawBatteryIcon();
				}
			}
		}
		// Update the setpoints for the temperature
		if (boostModeOn) {
			if (systemSettings.temperatureInF)
				currentlyActiveTemperatureTarget = ftoTipMeasurement(
						systemSettings.BoostTemp);
			else
				currentlyActiveTemperatureTarget = ctoTipMeasurement(
						systemSettings.BoostTemp);

		} else {
			if (systemSettings.temperatureInF)
				currentlyActiveTemperatureTarget = ftoTipMeasurement(
						systemSettings.SolderingTemp);
			else
				currentlyActiveTemperatureTarget = ctoTipMeasurement(
						systemSettings.SolderingTemp);
		}

		// Undervoltage test
		if (checkVoltageForExit()) {
			lastButtonTime = xTaskGetTickCount();
			return;
		}

		lcd.refresh();
		if (systemSettings.sensitivity && systemSettings.SleepTime)
			if (xTaskGetTickCount() - lastMovementTime > sleepThres
					&& xTaskGetTickCount() - lastButtonTime > sleepThres) {
				if (gui_SolderingSleepingMode()) {
					lastButtonTime = xTaskGetTickCount();
					return;  // If the function returns non-0 then exit
				}
			}
		GUIDelay();
	}
}

static const char *HEADERS[] = {
__DATE__, "Heap: ", "HWMG: ", "HWMP: ", "HWMM: ", "Time: ", "Move: ", "Rtip: ",
		"Ctip: ", "Vin :", "THan: " };

void showVersion(void) {
	uint8_t screen = 0;
	ButtonState b;
	for (;;) {
		lcd.clearScreen();    // Ensure the buffer starts clean
		lcd.setCursor(0, 0);  // Position the cursor at the 0,0 (top left)
		lcd.setFont(1);       // small font
		lcd.print((char *) "V2.05 PCB");  // Print version number
		lcd.printNumber(PCBVersion, 1); //Print PCB ID number
		lcd.setCursor(0, 8);         // second line
		lcd.print(HEADERS[screen]);
		switch (screen) {
		case 1:
			lcd.printNumber(xPortGetFreeHeapSize(), 5);
			break;
		case 2:
			lcd.printNumber(uxTaskGetStackHighWaterMark(GUITaskHandle), 5);
			break;
		case 3:
			lcd.printNumber(uxTaskGetStackHighWaterMark(PIDTaskHandle), 5);
			break;
		case 4:
			lcd.printNumber(uxTaskGetStackHighWaterMark(MOVTaskHandle), 5);
			break;
		case 5:
			lcd.printNumber(xTaskGetTickCount() / 100, 5);
			break;
		case 6:
			lcd.printNumber(lastMovementTime / 100, 5);
			break;
		case 7:
			lcd.printNumber(getTipRawTemp(0), 6);
			break;
		case 8:
			lcd.printNumber(tipMeasurementToC(getTipRawTemp(0)), 5);
			break;
		case 9:
			printVoltage();
			break;
		case 10:
			lcd.printNumber(getHandleTemperature(), 3);
		default:
			break;
		}

		lcd.refresh();
		b = getButtonState();
		if (b == BUTTON_B_SHORT)
			return;
		else if (b == BUTTON_F_SHORT) {
			screen++;
			screen = screen % 11;
		}
		GUIDelay();
	}
}

/* StartGUITask function */
void startGUITask(void const *argument __unused) {
	i2cDev.FRToSInit();
	uint8_t tempWarningState = 0;
	bool buttonLockout = false;
	bool tempOnDisplay = false;
	getTipRawTemp(2);         //reset filter
	lcd.setRotation(systemSettings.OrientationMode & 1);
	uint32_t ticks = xTaskGetTickCount();
	ticks += 400;  //4 seconds from now
	while (xTaskGetTickCount() < ticks) {
		if (showBootLogoIfavailable() == false)
			ticks = xTaskGetTickCount();
		ButtonState buttons = getButtonState();
		if (buttons)
			ticks = xTaskGetTickCount();  //make timeout now so we will exit
		GUIDelay();
	}
	if (systemSettings.autoStartMode) {
		// jump directly to the autostart mode
		if (systemSettings.autoStartMode == 1)
			gui_solderingMode();
	}

#if ACCELDEBUG

	for (;;) {
		HAL_IWDG_Refresh(&hiwdg);
		osDelay(100);
	}
//^ Kept here for a way to block this thread
#endif

	for (;;) {
		ButtonState buttons = getButtonState();

		if (tempWarningState == 2)
			buttons = BUTTON_F_SHORT;
		if (buttons != BUTTON_NONE && buttonLockout)
			buttons = BUTTON_NONE;
		else
			buttonLockout = false;

		switch (buttons) {
		case BUTTON_NONE:
			// Do nothing
			break;
		case BUTTON_BOTH:
			// Not used yet
			//In multi-language this might be used to reset language on a long hold or some such
			break;

		case BUTTON_B_LONG:
			// Show the version information
			showVersion();
			break;
		case BUTTON_F_LONG:
			gui_solderingTempAdjust();
			saveSettings();
			break;
		case BUTTON_F_SHORT:
			lcd.setFont(0);
			lcd.displayOnOff(true);  // turn lcd on
			gui_solderingMode();     // enter soldering mode
			buttonLockout = true;
			break;
		case BUTTON_B_SHORT:
			lcd.setFont(0);
			lcd.displayOnOff(true);  // turn lcd on
			enterSettingsMenu();      // enter the settings menu
			saveSettings();
			buttonLockout = true;
			setCalibrationOffset(systemSettings.CalibrationOffset); // ensure cal offset is applied
			break;
		default:
			break;
		}

		currentlyActiveTemperatureTarget = 0;  // ensure tip is off

		uint16_t tipTemp = tipMeasurementToC(getTipRawTemp(1)); //This forces a faster update rate on the filtering

		if (tipTemp < 50) {

			if (systemSettings.sensitivity) {

				if ((xTaskGetTickCount() - lastMovementTime) > 6000
						&& (xTaskGetTickCount() - lastButtonTime) > 6000) {
					lcd.displayOnOff(false);  // turn lcd off when no movement
				} else
					lcd.displayOnOff(true);  // turn lcd on
			} else
				lcd.displayOnOff(true);  // turn lcd on - disabled motion sleep
		} else
			lcd.displayOnOff(true);  // turn lcd on when temp > 50C

		if (tipTemp > 600)
			tipTemp = 0;

		// Clear the lcd buffer
		lcd.clearScreen();
		lcd.setCursor(0, 0);
		if (systemSettings.detailedIDLE) {
			lcd.setFont(1);
			if (tipTemp > 470) {
				lcd.print(TipDisconnectedString);
			} else {
				lcd.print(IdleTipString);
				if (systemSettings.temperatureInF)
					lcd.printNumber(tipMeasurementToF(getTipRawTemp(0)), 3);
				else
					lcd.printNumber(tipMeasurementToC(getTipRawTemp(0)), 3);
				lcd.print(IdleSetString);
				lcd.printNumber(systemSettings.SolderingTemp, 3);
			}
			lcd.setCursor(0, 8);
			lcd.print(InputVoltageString);
			printVoltage();
			lcd.print("V");

		} else {
			lcd.setFont(0);
			if (lcd.getRotation()) {
				lcd.drawArea(12, 0, 84, 16, idleScreenBG);
				lcd.setCursor(0, 0);
				gui_drawBatteryIcon();
			} else {
				lcd.drawArea(0, 0, 84, 16, idleScreenBGF); // Needs to be flipped so button ends up on right side of screen
				lcd.setCursor(84, 0);
				gui_drawBatteryIcon();
			}
			if (tipTemp > 55)
				tempOnDisplay = true;
			else if (tipTemp < 45)
				tempOnDisplay = false;
			if (tempOnDisplay) {
				//draw temp over the start soldering button
				//Location changes on screen rotation
				if (lcd.getRotation()) {
					// in right handed mode we want to draw over the first part
					lcd.fillArea(55, 0, 41, 16, 0);	//clear the area for the temp
					lcd.setCursor(56, 0);

				} else {
					lcd.fillArea(0, 0, 41, 16, 0);				//clear the area
					lcd.setCursor(0, 0);
				}
				//draw in the temp
				lcd.setFont(0);				//big font
				if (!(systemSettings.coolingTempBlink
						&& (xTaskGetTickCount() % 50 < 25)))
					gui_drawTipTemp(false);				// draw in the temp
			}
		}

		lcd.refresh();
		GUIDelay();
	}
}

/* StartPIDTask function */
void __attribute__ ((long_call, section (".data.ramfuncs"))) startPIDTask(void const *argument __unused) {
	/*
	 * We take the current tip temperature & evaluate the next step for the tip
	 * control PWM
	 * Tip temperature is measured by getTipTemperature(1) so we get instant
	 * result
	 * This comes in Cx10 format
	 * We then control the tip temperature to aim for the setpoint in the settings
	 * struct
	 *
	 */
	setTipPWM(0); // disable the output driver if the output is set to be off
	osDelay(500);
	int32_t integralCount = 0;
	int32_t derivativeLastValue = 0;

// REMEBER ^^^^ These constants are backwards
// They act as dividers, so to 'increase' a P term, you make the number
// smaller.

	const int32_t itermMax = 100;
	pidTaskNotification = xTaskGetCurrentTaskHandle();
	for (;;) {
		if (ulTaskNotifyTake( pdTRUE, 50)) {
			//Wait a max of 50ms
			//This is a call to block this thread until the ADC does its samples
			uint16_t rawTemp = getTipRawTemp(1);  // get instantaneous reading
			if (currentlyActiveTemperatureTarget) {
				// Compute the PID loop in here
				// Because our values here are quite large for all measurements (0-32k ~=
				// 66 counts per C)
				// P I & D are divisors, so inverse logic applies (beware)

				// Cap the max set point to 450C
				if (currentlyActiveTemperatureTarget > ctoTipMeasurement(450)) {
					currentlyActiveTemperatureTarget = ctoTipMeasurement(450);
				}

				int32_t rawTempError = currentlyActiveTemperatureTarget
						- rawTemp;

				int32_t ierror = (rawTempError
						/ ((int32_t) systemSettings.PID_I));

				integralCount += ierror;

				if (integralCount > (itermMax / 2))
					integralCount = itermMax / 2;  // prevent too much lead
				else if (integralCount < -itermMax)
					integralCount = itermMax;

				int32_t dInput = (rawTemp - derivativeLastValue);

				/*Compute PID Output*/
				int32_t output = (rawTempError
						/ ((int32_t) systemSettings.PID_P));
				if (((int32_t) systemSettings.PID_I))
					output += integralCount;
				if (((int32_t) systemSettings.PID_D))
					output -= (dInput / ((int32_t) systemSettings.PID_D));

				if (output > 100) {
					output = 100;  // saturate
				} else if (output < 0) {
					output = 0;
				}

				if (currentlyActiveTemperatureTarget < rawTemp) {
					output = 0;
					integralCount = 0;
					derivativeLastValue = 0;
				}
				setTipPWM(output);
				derivativeLastValue = rawTemp;  // store for next loop

			} else {
				setTipPWM(0); // disable the output driver if the output is set to be off
				integralCount = 0;
				derivativeLastValue = 0;
			}

			HAL_IWDG_Refresh(&hiwdg);
		} else {
			if (currentlyActiveTemperatureTarget == 0) {
				setTipPWM(0); // disable the output driver if the output is set to be off
				integralCount = 0;
				derivativeLastValue = 0;
			}
		}
	}
}
#define MOVFilter 8
void startMOVTask(void const *argument __unused) {
	osDelay(250);  // wait for accelerometer to stabilize
	lcd.setRotation(systemSettings.OrientationMode & 1);
	lastMovementTime = 0;
	int16_t datax[MOVFilter] = { 0 };
	int16_t datay[MOVFilter] = { 0 };
	int16_t dataz[MOVFilter] = { 0 };
	uint8_t currentPointer = 0;
	int16_t tx = 0, ty = 0, tz = 0;
	int32_t avgx = 0, avgy = 0, avgz = 0;
	if (systemSettings.sensitivity > 9)
		systemSettings.sensitivity = 9;
#if ACCELDEBUG
	uint32_t max = 0;
#endif
	Orientation rotation = ORIENTATION_FLAT;
	for (;;) {
		int32_t threshold = 1500 + (9 * 200);
		threshold -= systemSettings.sensitivity * 200; // 200 is the step size

		if (PCBVersion == 2) {
			accel2.getAxisReadings(&tx, &ty, &tz);
			rotation = accel2.getOrientation();
		} else if (PCBVersion == 1) {
			accel.getAxisReadings(&tx, &ty, &tz);
			rotation = accel.getOrientation();
		}
		if (systemSettings.OrientationMode == 2) {
			if (rotation != ORIENTATION_FLAT) {
				lcd.setRotation(rotation == ORIENTATION_LEFT_HAND); // link the data through
			}
		}
		datax[currentPointer] = (int32_t) tx;
		datay[currentPointer] = (int32_t) ty;
		dataz[currentPointer] = (int32_t) tz;
		currentPointer = (currentPointer + 1) % MOVFilter;

		// calculate averages
		for (uint8_t i = 0; i < MOVFilter; i++) {
			avgx += datax[i];
			avgy += datay[i];
			avgz += dataz[i];
		}
		avgx /= MOVFilter;
		avgy /= MOVFilter;
		avgz /= MOVFilter;

		//Sum the deltas
		int32_t error = (abs(avgx - tx) + abs(avgy - ty) + abs(avgz - tz));

#if ACCELDEBUG
		// Debug for Accel

		lcd.setFont(1);
		lcd.setCursor(0, 0);
		lcd.printNumber(abs(avgx - (int32_t) tx), 5);
		lcd.print(" ");
		lcd.printNumber(abs(avgy - (int32_t) ty), 5);
		if (error > max) {
			max = (abs(avgx - tx) + abs(avgy - ty) + abs(avgz - tz));
		}
		lcd.setCursor(0, 8);
		lcd.printNumber(max, 5);
		lcd.print(" ");

		lcd.printNumber((abs(avgx - tx) + abs(avgy - ty) + abs(avgz - tz)), 5);
		lcd.refresh();
		if (HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin) == GPIO_PIN_RESET) {
			max = 0;
		}
#endif

		// So now we have averages, we want to look if these are different by more
		// than the threshold

		// If error has occurred then we update the tick timer
		if (error > threshold) {
			lastMovementTime = xTaskGetTickCount();
		}

		osDelay(100);  // Slow down update rate
	}
}

#define FLASH_LOGOADDR \
  (0x8000000 | 0xF800) /*second last page of flash set aside for logo image*/

bool showBootLogoIfavailable() {
// check if the header is there (0xAA,0x55,0xF0,0x0D)
// If so display logo
	//TODO REDUCE STACK ON THIS ONE, USE DRAWING IN THE READ LOOP
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

	lcd.drawArea(0, 0, 96, 16, (uint8_t *) (temp8 + 4));
	lcd.refresh();
	return true;
}

/*
 * Catch the IRQ that says that the conversion is done on the temperature readings coming in
 * Once these have come in we can unblock the PID so that it runs again
 */
void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_ADCEx_InjectedConvCpltCallback(
		ADC_HandleTypeDef* hadc) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (hadc == &hadc1) {
		if (pidTaskNotification) {
			vTaskNotifyGiveFromISR(pidTaskNotification,
					&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
}

void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_I2C_MasterRxCpltCallback(
		I2C_HandleTypeDef *hi2c __unused) {
	i2cDev.CpltCallback();
}
void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_I2C_MasterTxCpltCallback(
		I2C_HandleTypeDef *hi2c __unused) {
	i2cDev.CpltCallback();
}
void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_I2C_MemTxCpltCallback(
		I2C_HandleTypeDef *hi2c __unused) {
	i2cDev.CpltCallback();
}
void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_I2C_ErrorCallback(
		I2C_HandleTypeDef *hi2c __unused) {
	i2cDev.CpltCallback();
}
void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_I2C_AbortCpltCallback(
		I2C_HandleTypeDef *hi2c __unused) {
	i2cDev.CpltCallback();
}
void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_I2C_MemRxCpltCallback(
		I2C_HandleTypeDef *hi2c __unused) {
	i2cDev.CpltCallback();
}
void vApplicationStackOverflowHook( xTaskHandle *pxTask __unused,
		signed portCHAR *pcTaskName __unused) {
//We dont have a good way to handle a stack overflow at this point in time
	NVIC_SystemReset();

}
