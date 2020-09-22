/*
 * MOVThread.cpp
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "FreeRTOS.h"
#include "I2C_Wrapper.hpp"
#include "LIS2DH12.hpp"
#include "MMA8652FC.hpp"
#include "QC3.h"
#include "Settings.h"
#include "TipThermoModel.h"
#include "cmsis_os.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "BMA223.hpp"
#include "task.h"
#define MOVFilter 8
uint8_t accelInit = 0;
uint32_t lastMovementTime = 0;
void detectAccelerometerVersion() {
#ifdef ACCEL_MMA
	if (MMA8652FC::detect()) {
		PCBVersion = 1;
		if(!MMA8652FC::initalize()) {
			PCBVersion = 99;
		}
	} else
#endif
#ifdef ACCEL_LIS
	if (LIS2DH12::detect()) {
		PCBVersion = 2;
		// Setup the ST Accelerometer
		if(!LIS2DH12::initalize()) {
			PCBVersion = 99;
		}
	} else
#endif
#ifdef ACCEL_BMA
	if (BMA223::detect()) {
		PCBVersion = 3;
		// Setup the ST Accelerometer
		if (!BMA223::initalize()) {
			PCBVersion = 99;
		}
	} else
#endif
	{
		PCBVersion = 99;
		systemSettings.SleepTime = 0;
		systemSettings.ShutdownTime = 0;  // No accel -> disable sleep
		systemSettings.sensitivity = 0;
	}

}
inline void readAccelerometer(int16_t& tx, int16_t& ty, int16_t& tz, Orientation &rotation) {
#ifdef ACCEL_LIS
	if (PCBVersion == 2) {
		LIS2DH12::getAxisReadings(tx, ty, tz);
		rotation = LIS2DH12::getOrientation();
	} else
#endif
#ifdef ACCEL_MMA
	if (PCBVersion == 1) {
		MMA8652FC::getAxisReadings(tx, ty, tz);
		rotation = MMA8652FC::getOrientation();
	} else
#endif
#ifdef ACCEL_BMA
	if (PCBVersion == 3) {
		BMA223::getAxisReadings(tx, ty, tz);
		rotation = BMA223::getOrientation();
	} else
#endif
	{
		//do nothing :(
	}
}
void startMOVTask(void const *argument __unused) {
	osDelay(10);//Make oled init happen first
	postRToSInit();
	OLED::setRotation(systemSettings.OrientationMode & 1);
	detectAccelerometerVersion();
	if ((systemSettings.autoStartMode == 2 || systemSettings.autoStartMode == 3))
		osDelay(2000);

	lastMovementTime = 0;
	int16_t datax[MOVFilter] = { 0 };
	int16_t datay[MOVFilter] = { 0 };
	int16_t dataz[MOVFilter] = { 0 };
	uint8_t currentPointer = 0;
	int16_t tx = 0, ty = 0, tz = 0;
	int32_t avgx, avgy, avgz;
	if (systemSettings.sensitivity > 9)
		systemSettings.sensitivity = 9;
	Orientation rotation = ORIENTATION_FLAT;
//	OLED::setFont(1);
//	for (;;) {
//		OLED::clearScreen();
//		OLED::setCursor(0, 0);
//		readAccelerometer(tx, ty, tz, rotation);
//		OLED::printNumber(tx, 5, 0);
//		OLED::setCursor(0, 8);
//		OLED::printNumber(xTaskGetTickCount() / 10, 5, 1);
//		OLED::refresh();
//		osDelay(50);
//	}
	for (;;) {
		int32_t threshold = 1500 + (9 * 200);
		threshold -= systemSettings.sensitivity * 200;  // 200 is the step size
		readAccelerometer(tx, ty, tz, rotation);
		if (systemSettings.OrientationMode == 2) {
			if (rotation != ORIENTATION_FLAT) {
				OLED::setRotation(rotation == ORIENTATION_LEFT_HAND); // link the data through
			}
		}
		datax[currentPointer] = (int32_t) tx;
		datay[currentPointer] = (int32_t) ty;
		dataz[currentPointer] = (int32_t) tz;
		if (!accelInit) {
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
		power_check();
	}
}
