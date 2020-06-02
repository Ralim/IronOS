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
#include "task.h"
#define MOVFilter 8
uint8_t accelInit = 0;
uint32_t lastMovementTime = 0;
void startMOVTask(void const *argument __unused) {
	OLED::setRotation(systemSettings.OrientationMode & 1);
	postRToSInit();
	power_probe();
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
