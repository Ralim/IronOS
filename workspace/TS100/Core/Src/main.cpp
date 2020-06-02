// By Ben V. Brown - V2.0 of the TS100 firmware

/*
 * Main.cpp bootstraps the device and then hands over to FreeRTOS and the threads
 */

#include "BSP.h"
#include <main.hpp>
#include "LIS2DH12.hpp"
#include <MMA8652FC.hpp>
#include <power.hpp>
#include "Settings.h"
#include "cmsis_os.h"
uint8_t PCBVersion = 0;
// File local variables

bool settingsWereReset = false;
// FreeRTOS variables

osThreadId GUITaskHandle;
static const size_t GUITaskStackSize = 1024 / 4;
uint32_t GUITaskBuffer[GUITaskStackSize];
osStaticThreadDef_t GUITaskControlBlock;

osThreadId PIDTaskHandle;
static const size_t PIDTaskStackSize = 512 / 4;
uint32_t PIDTaskBuffer[PIDTaskStackSize];
osStaticThreadDef_t PIDTaskControlBlock;
osThreadId MOVTaskHandle;
static const size_t MOVTaskStackSize = 512 / 4;
uint32_t MOVTaskBuffer[MOVTaskStackSize];
osStaticThreadDef_t MOVTaskControlBlock;

// End FreeRTOS

// Main sets up the hardware then hands over to the FreeRTOS kernel
int main(void) {
	preRToSInit();

	setTipX10Watts(0);  // force tip off
	resetWatchdog();
	OLED::initialize();  // start up the LCD
	OLED::setFont(0);    // default to bigger font
	// Testing for which accelerometer is mounted
	resetWatchdog();
	settingsWereReset = restoreSettings();  // load the settings from flash
	if (MMA8652FC::detect()) {
		PCBVersion = 1;
		MMA8652FC::initalize();  // this sets up the I2C registers
	} else if (LIS2DH12::detect()) {
		PCBVersion = 2;
		// Setup the ST Accelerometer
		LIS2DH12::initalize();  // startup the accelerometer
	} else {
		PCBVersion = 3;
		systemSettings.SleepTime = 0;
		systemSettings.ShutdownTime = 0;  // No accel -> disable sleep
		systemSettings.sensitivity = 0;
	}

	resetWatchdog();

	/* Create the thread(s) */
	/* definition and creation of GUITask */

	osThreadStaticDef(GUITask, startGUITask, osPriorityBelowNormal, 0,
			GUITaskStackSize, GUITaskBuffer, &GUITaskControlBlock);
	GUITaskHandle = osThreadCreate(osThread(GUITask), NULL);

	/* definition and creation of PIDTask */
	osThreadStaticDef(PIDTask, startPIDTask, osPriorityRealtime, 0,
			PIDTaskStackSize, PIDTaskBuffer, &PIDTaskControlBlock);
	PIDTaskHandle = osThreadCreate(osThread(PIDTask), NULL);

	osThreadStaticDef(MOVTask, startMOVTask, osPriorityNormal, 0,
			MOVTaskStackSize, MOVTaskBuffer, &MOVTaskControlBlock);
	MOVTaskHandle = osThreadCreate(osThread(MOVTask), NULL);

	resetWatchdog();

	/* Start scheduler */
	osKernelStart();
	/* We should never get here as control is now taken by the scheduler */
	for (;;) {
	}
}
