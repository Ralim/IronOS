// By Ben V. Brown - V2.0 of the TS100 firmware
#include <MMA8652FC.hpp>
#include <gui.hpp>
#include <main.hpp>
#include "LIS2DH12.hpp"
#include <history.hpp>
#include <power.hpp>
#include "Settings.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "TipThermoModel.h"
uint8_t PCBVersion = 0;
// File local variables
uint32_t currentTempTargetDegC = 0; // Current temperature target in C
uint8_t accelInit = 0;
uint32_t lastMovementTime = 0;

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

static TaskHandle_t pidTaskNotification = NULL;

void startGUITask(void const *argument);
void startPIDTask(void const *argument);
void startMOVTask(void const *argument);
// End FreeRTOS

static const int maxPowerIdleTicks = 1000;
static const int powerPulseTicks = 50;
static const int x10PowerPulseWatts = 3;

// Main sets up the hardware then hands over to the FreeRTOS kernel
int main(void) {
	/* Reset of all peripherals, Initializes the Flash interface and the Systick.
	 */
	HAL_Init();
	Setup_HAL();  // Setup all the HAL objects
	HAL_IWDG_Refresh(&hiwdg);
	setTipX10Watts(0);  // force tip off
	FRToSI2C::init(&hi2c1);
	OLED::initialize();  // start up the LCD
	OLED::setFont(0);    // default to bigger font
	// Testing for which accelerometer is mounted
	uint8_t buffer[1];
	HAL_IWDG_Refresh(&hiwdg);
	if (HAL_I2C_Mem_Read(&hi2c1, 29 << 1, 0x0F, I2C_MEMADD_SIZE_8BIT, buffer, 1,
			1000) == HAL_OK) {
		PCBVersion = 1;
		MMA8652FC::initalize();  // this sets up the I2C registers
	} else if (HAL_I2C_Mem_Read(&hi2c1, 25 << 1, 0x0F, I2C_MEMADD_SIZE_8BIT,
			buffer, 1, 1000) == HAL_OK) {
		PCBVersion = 2;
		// Setup the ST Accelerometer
		LIS2DH12::initalize();  // startup the accelerometer
	} else {
		PCBVersion = 3;
		systemSettings.SleepTime = 0;
		systemSettings.ShutdownTime = 0;  // No accel -> disable sleep
		systemSettings.sensitivity = 0;
	}
	HAL_IWDG_Refresh(&hiwdg);
	settingsWereReset = restoreSettings();  // load the settings from flash

	HAL_IWDG_Refresh(&hiwdg);

	/* Create the thread(s) */
	/* definition and creation of GUITask */

	osThreadStaticDef(GUITask, startGUITask, osPriorityBelowNormal, 0,
			GUITaskStackSize, GUITaskBuffer, &GUITaskControlBlock);
	GUITaskHandle = osThreadCreate(osThread(GUITask), NULL);

	/* definition and creation of PIDTask */
	osThreadStaticDef(PIDTask, startPIDTask, osPriorityRealtime, 0,
			PIDTaskStackSize, PIDTaskBuffer, &PIDTaskControlBlock);
	PIDTaskHandle = osThreadCreate(osThread(PIDTask), NULL);

	if (PCBVersion < 3) {
		osThreadStaticDef(MOVTask, startMOVTask, osPriorityNormal, 0,
				MOVTaskStackSize, MOVTaskBuffer, &MOVTaskControlBlock);
		MOVTaskHandle = osThreadCreate(osThread(MOVTask), NULL);
	}

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	while (1) {
	}
}

/* StartPIDTask function */
void startPIDTask(void const *argument __unused) {
	/*
	 * We take the current tip temperature & evaluate the next step for the tip
	 * control PWM.
	 */
	setTipX10Watts(0); // disable the output driver if the output is set to be off

#ifdef MODEL_TS80
	//Set power management code to the tip resistance in ohms * 10

	TickType_t lastPowerPulse = 0;
#else

#endif
	history<int32_t, PID_TIM_HZ> tempError = { { 0 }, 0, 0 };
	currentTempTargetDegC = 0; // Force start with no output (off). If in sleep / soldering this will
							   // be over-ridden rapidly
	pidTaskNotification = xTaskGetCurrentTaskHandle();
	uint32_t PIDTempTarget = 0;
	for (;;) {

		if (ulTaskNotifyTake(pdTRUE, 2000)) {
			// This is a call to block this thread until the ADC does its samples
			int32_t x10WattsOut = 0;
			// Do the reading here to keep the temp calculations churning along
			uint32_t currentTipTempInC = TipThermoModel::getTipInC(true);
			PIDTempTarget = currentTempTargetDegC;
			if (PIDTempTarget) {
				// Cap the max set point to 450C
				if (PIDTempTarget > (450)) {
					//Maximum allowed output
					PIDTempTarget = (450);
				}
				//Safety check that not aiming higher than current tip can measure
				if (PIDTempTarget > TipThermoModel::getTipMaxInC()) {
					PIDTempTarget = TipThermoModel::getTipMaxInC();
				}
				// Convert the current tip to degree's C

				// As we get close to our target, temp noise causes the system
				//  to be unstable. Use a rolling average to dampen it.
				// We overshoot by roughly 1 degree C.
				//  This helps stabilize the display.
				int32_t tError = PIDTempTarget - currentTipTempInC + 1;
				tError = tError > INT16_MAX ? INT16_MAX : tError;
				tError = tError < INT16_MIN ? INT16_MIN : tError;
				tempError.update(tError);

				// Now for the PID!

				// P term - total power needed to hit target temp next cycle.
				// thermal mass = 1690 milliJ/*C for my tip.
				//  = Watts*Seconds to raise Temp from room temp to +100*C, divided by 100*C.
				// we divide milliWattsNeeded by 20 to let the I term dominate near the set point.
				//  This is necessary because of the temp noise and thermal lag in the system.
				// Once we have feed-forward temp estimation we should be able to better tune this.

				int32_t x10WattsNeeded = tempToX10Watts(tError);
//						tempError.average());
				// note that milliWattsNeeded is sometimes negative, this counters overshoot
				//  from I term's inertia.
				x10WattsOut += x10WattsNeeded;

				// I term - energy needed to compensate for heat loss.
				// We track energy put into the system over some window.
				// Assuming the temp is stable, energy in = energy transfered.
				//  (If it isn't, P will dominate).
				x10WattsOut += x10WattHistory.average();

				// D term - use sudden temp change to counter fast cooling/heating.
				//  In practice, this provides an early boost if temp is dropping
				//  and counters extra power if the iron is no longer losing temp.
				// basically: temp - lastTemp
				//  Unfortunately, our temp signal is too noisy to really help.

			}
#ifdef MODEL_TS80
			//If its a TS80, we want to have the option of using an occasional pulse to keep the power bank on
			if (((xTaskGetTickCount() - lastPowerPulse) > maxPowerIdleTicks)
					&& (x10WattsOut < x10PowerPulseWatts)) {
				x10WattsOut = x10PowerPulseWatts;
			}
			if (((xTaskGetTickCount() - lastPowerPulse)
					> (maxPowerIdleTicks + powerPulseTicks))
					&& (x10WattsOut >= x10PowerPulseWatts)) {
				lastPowerPulse = xTaskGetTickCount();
			}
#endif
			//Secondary safety check to forcefully disable header when within ADC noise of top of ADC
			if (getTipRawTemp(0) > (0x7FFF - 150)) {
				x10WattsOut = 0;
			}
			if (systemSettings.powerLimitEnable
					&& x10WattsOut > (systemSettings.powerLimit * 10)) {
				setTipX10Watts(systemSettings.powerLimit * 10);
			} else {
				setTipX10Watts(x10WattsOut);
			}

			HAL_IWDG_Refresh(&hiwdg);
		} else {
			//ADC interrupt timeout
			setTipPWM(0);
		}
	}
}

#define MOVFilter 8
void startMOVTask(void const *argument __unused) {
	OLED::setRotation(true);

#ifdef MODEL_TS80
	startQC(systemSettings.voltageDiv);
	while (pidTaskNotification == 0)
		osDelay(30);  // To ensure we return after idealQCVoltage/tip resistance

	seekQC((systemSettings.cutoutSetting) ? 120 : 90,
			systemSettings.voltageDiv); // this will move the QC output to the preferred voltage to start with

#else
	osDelay(250);  // wait for accelerometer to stabilize
#endif

	OLED::setRotation(systemSettings.OrientationMode & 1);
	lastMovementTime = 0;
	int16_t datax[MOVFilter] = { 0 };
	int16_t datay[MOVFilter] = { 0 };
	int16_t dataz[MOVFilter] = { 0 };
	uint8_t currentPointer = 0;
	int16_t tx = 0, ty = 0, tz = 0;
	int32_t avgx = 0, avgy = 0, avgz = 0;
	if (systemSettings.sensitivity > 9)
		systemSettings.sensitivity = 9;
#ifdef ACCELDEBUG
	uint32_t max = 0;
#endif
	Orientation rotation = ORIENTATION_FLAT;
	for (;;) {
		int32_t threshold = 1500 + (9 * 200);
		threshold -= systemSettings.sensitivity * 200;  // 200 is the step size

		if (PCBVersion == 2) {
			LIS2DH12::getAxisReadings(tx, ty, tz);
			rotation = LIS2DH12::getOrientation();
		} else if (PCBVersion == 1) {
			MMA8652FC::getAxisReadings(tx, ty, tz);
			rotation = MMA8652FC::getOrientation();
		}
		if (systemSettings.OrientationMode == 2) {
			if (rotation != ORIENTATION_FLAT) {
				OLED::setRotation(rotation == ORIENTATION_LEFT_HAND); // link the data through
			}
		}
		datax[currentPointer] = (int32_t) tx;
		datay[currentPointer] = (int32_t) ty;
		dataz[currentPointer] = (int32_t) tz;
		if (!accelInit)
		{
			for (uint8_t i = currentPointer + 1; i < MOVFilter; i++) {
				datax[i] = (int32_t) tx;
				datay[i] = (int32_t) ty;
				dataz[i] = (int32_t) tz;
			}
			accelInit = 1;
		}
		currentPointer = (currentPointer + 1) % MOVFilter;
		avgx = avgy = avgz = 0;
		// calculate averages
		for (uint8_t i = 0; i < MOVFilter; i++) {
			avgx += datax[i];
			avgy += datay[i];
			avgz += dataz[i];
		}
		avgx /= MOVFilter;
		avgy /= MOVFilter;
		avgz /= MOVFilter;

		// Sum the deltas
		int32_t error = (abs(avgx - tx) + abs(avgy - ty) + abs(avgz - tz));
		// So now we have averages, we want to look if these are different by more
		// than the threshold

		// If error has occurred then we update the tick timer
		if (error > threshold) {
			lastMovementTime = xTaskGetTickCount();
		}

		osDelay(100);  // Slow down update rate
#ifdef MODEL_TS80
		seekQC((systemSettings.cutoutSetting) ? 120 : 90,
				systemSettings.voltageDiv); // Run the QC seek again if we have drifted too much
#endif
	}
}

#define FLASH_LOGOADDR \
  (0x8000000 | 0xF800) /*second last page of flash set aside for logo image*/

/* The header value is (0xAA,0x55,0xF0,0x0D) but is stored in little endian 16
 * bits words on the flash */
const uint8_t LOGO_HEADER_VALUE[] = { 0x55, 0xAA, 0x0D, 0xF0 };

bool showBootLogoIfavailable() {
	uint8_t *header = (uint8_t*) (FLASH_LOGOADDR);

	// check if the header is correct. 
	for (int i = 0; i < 4; i++) {
		if (header[i] != LOGO_HEADER_VALUE[i]) {
			return false;
		}
	}

	OLED::drawAreaSwapped(0, 0, 96, 16, (uint8_t*) (FLASH_LOGOADDR + 4));
	OLED::refresh();
	return true;
}

/*
 * Catch the IRQ that says that the conversion is done on the temperature
 * readings coming in Once these have come in we can unblock the PID so that it
 * runs again
 */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (hadc == &hadc1) {
		if (pidTaskNotification) {
			vTaskNotifyGiveFromISR(pidTaskNotification,
					&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c __unused) {
	asm("bkpt");

	FRToSI2C::CpltCallback();
}
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	//asm("bkpt");

	FRToSI2C::CpltCallback();
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c __unused) {
	FRToSI2C::CpltCallback();
}
void vApplicationStackOverflowHook(xTaskHandle *pxTask __unused,
		signed portCHAR *pcTaskName __unused) {
	asm("bkpt");
	// We dont have a good way to handle a stack overflow at this point in time
	NVIC_SystemReset();
}
