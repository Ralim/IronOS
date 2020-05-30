/*
 * PIDThread.cpp
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "main.hpp"
#include "BSP.h"
#include "power.hpp"
#include "history.hpp"
#include "TipThermoModel.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Settings.h"
static TickType_t powerPulseRate = 1000;
static TickType_t powerPulseDuration = 50;
TaskHandle_t pidTaskNotification = NULL;
uint32_t currentTempTargetDegC = 0; // Current temperature target in C

/* StartPIDTask function */
void startPIDTask(void const *argument __unused) {
	/*
	 * We take the current tip temperature & evaluate the next step for the tip
	 * control PWM.
	 */
	setTipX10Watts(0); // disable the output driver if the output is set to be off
	TickType_t lastPowerPulseStart = 0;
	TickType_t lastPowerPulseEnd = 0;

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
			//If the user turns on the option of using an occasional pulse to keep the power bank on
			if (systemSettings.KeepAwakePulse) {

				if (xTaskGetTickCount() - lastPowerPulseStart
						> powerPulseRate) {
					lastPowerPulseStart = xTaskGetTickCount();
					lastPowerPulseEnd = lastPowerPulseStart
							+ powerPulseDuration;
				}

				//If current PID is less than the pulse level, check if we want to constrain to the pulse as the floor
				if (x10WattsOut < systemSettings.KeepAwakePulse
						&& xTaskGetTickCount() < lastPowerPulseEnd) {
					x10WattsOut = systemSettings.KeepAwakePulse;
				}
			}

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
