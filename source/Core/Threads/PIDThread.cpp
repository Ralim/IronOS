/*
 * PIDThread.cpp
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "FreeRTOS.h"
#include "Settings.h"
#include "TipThermoModel.h"
#include "cmsis_os.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "task.h"

static TickType_t powerPulseWaitUnit      = 25 * TICKS_100MS;      // 2.5 s
static TickType_t powerPulseDurationUnit  = (5 * TICKS_100MS) / 2; // 250 ms
TaskHandle_t      pidTaskNotification     = NULL;
volatile uint32_t currentTempTargetDegC   = 0; // Current temperature target in C
int32_t           powerSupplyWattageLimit = 0;
bool              heaterThermalRunaway    = false;

static int32_t getPIDResultX10Watts(int32_t tError);
static void    detectThermalRunaway(const int16_t currentTipTempInC, const int tError);
static void    setOutputx10WattsViaFilters(int32_t x10Watts);
static int32_t getX10WattageLimits();

/* StartPIDTask function */
void startPIDTask(void const *argument __unused) {
  /*
   * We take the current tip temperature & evaluate the next step for the tip
   * control PWM.
   */
  setTipX10Watts(0); // disable the output at startup

  currentTempTargetDegC = 0; // Force start with no output (off). If in sleep / soldering this will
                             // be over-ridden rapidly
  pidTaskNotification    = xTaskGetCurrentTaskHandle();
  uint32_t PIDTempTarget = 0;
  // Pre-seed the adc filters
  for (int i = 0; i < 32; i++) {
    ulTaskNotifyTake(pdTRUE, 5);
    TipThermoModel::getTipInC(true);
    getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 1);
  }

  while (preStartChecks() == 0) {
    resetWatchdog();
    ulTaskNotifyTake(pdTRUE, 2000);
  }

  int32_t x10WattsOut = 0;

  for (;;) {
    x10WattsOut = 0;
    // This is a call to block this thread until the ADC does its samples
    if (ulTaskNotifyTake(pdTRUE, TICKS_SECOND * 2)) {
      // Do the reading here to keep the temp calculations churning along
      uint32_t currentTipTempInC = TipThermoModel::getTipInC(true);
      PIDTempTarget              = currentTempTargetDegC;
      if (PIDTempTarget > 0) {
        // Cap the max set point to 450C
        if (PIDTempTarget > (450)) {
          // Maximum allowed output
          PIDTempTarget = (450);
        }
        // Safety check that not aiming higher than current tip can measure
        if (PIDTempTarget > TipThermoModel::getTipMaxInC()) {
          PIDTempTarget = TipThermoModel::getTipMaxInC();
        }
        int32_t tError = PIDTempTarget - currentTipTempInC;

        detectThermalRunaway(currentTipTempInC, tError);
        x10WattsOut = getPIDResultX10Watts(tError);
      } else {
        detectThermalRunaway(currentTipTempInC, 0);
      }
      setOutputx10WattsViaFilters(x10WattsOut);
    } else {
      // ADC interrupt timeout
      setTipPWM(0, false);
    }
#ifdef DEBUG_UART_OUTPUT
    log_system_state(x10WattsOut);
#endif
  }
}

template <class T = int32_t> struct Integrator {
  T sum;

  T update(const T val, const int32_t inertia, const int32_t gain, const int32_t rate, const int32_t limit) {
    // Decay the old value. This is a simplified formula that still works with decent results
    // Ideally we would have used an exponential decay but the computational effort required
    // by exp function is just not justified here in respect to the outcome
    sum = (sum * (100 - (inertia / rate))) / 100;
    // Add the new value x integration interval ( 1 / rate)
    sum += (gain * val) / rate;

    // limit the output
    if (sum > limit)
      sum = limit;
    else if (sum < -limit)
      sum = -limit;

    return sum;
  }

  void set(T const val) { sum = val; }

  T get(bool positiveOnly = true) const { return (positiveOnly) ? ((sum > 0) ? sum : 0) : sum; }
};
int32_t getPIDResultX10Watts(int32_t setpointDelta) {
  static TickType_t          lastCall   = 0;
  static Integrator<int32_t> powerStore = {0};

  const TickType_t rate = 1000 / (xTaskGetTickCount() - lastCall);
  lastCall              = xTaskGetTickCount();
  // Sandman note:
  // PID Challenge - we have a small thermal mass that we to want heat up as fast as possible but we don't
  // want to overshot excessively (if at all) the setpoint temperature. In the same time we have 'imprecise'
  // instant temperature measurements. The nature of temperature reading imprecision is not necessarily
  // related to the sensor (thermocouple) or DAQ system, that otherwise are fairly decent. The real issue	is
  // the thermal inertia. We basically read the temperature in the window between two heating sessions when
  // the output is off. However, the heater temperature does not dissipate instantly into the tip mass so
  // at any moment right after heating, the thermocouple would sense a temperature significantly higher than
  // moments later. We could use longer delays but that would slow the PID loop and that would lead to other
  // negative side effects. As a result, we can only rely on the I term but with a twist. Instead of a simple
  // integrator we are going to use a self decaying integrator that acts more like a dual I term / P term
  // rather than a plain I term. Depending on the circumstances, like when the delta temperature is large,
  // it acts more like a P term whereas on closing to setpoint it acts increasingly closer to a plain I term.
  // So in a sense, we have a bit of both.
  //																		 So there we go...

  // P = (Thermal Mass) x (Delta Temperature ) / 1sec, where thermal mass is in X10 J / °C and
  // delta temperature is in °C. The result is the power in X10 W needed to raise (or decrease!) the
  // tip temperature with (Delta Temperature ) °C in 1 second.
  // Note on powerStore. On update, if the value is provided in X10 (W) units then inertia shall be provided
  // in X10 (J / °C) units as well. Also, powerStore is updated with a gain of 2. Where this comes from: The actual
  // power CMOS is controlled by TIM3->CTR1 (that is software modulated - on/off - by TIM2-CTR4 interrupts). However,
  // TIM3->CTR1 is configured with a duty cycle of 50% so, in real, we get only 50% of the presumed power output
  // so we basically double the need (gain = 2) to get what we want.
  return powerStore.update(TIP_THERMAL_MASS * setpointDelta, // the required power
                           getTipThermalMass(),              // Inertia, smaller numbers increase dominance of the previous value
                           2,                                // gain
                           rate,                             // PID cycle frequency
                           getX10WattageLimits());
}

void detectThermalRunaway(const int16_t currentTipTempInC, const int tError) {
  static uint16_t   tipTempCRunawayTemp   = 0;
  static TickType_t runawaylastChangeTime = 0;

  // Check for thermal runaway, where it has been x seconds with negligible (y) temp rise
  // While trying to actively heat

  // If we are more than 20C below the setpoint
  if ((tError > THERMAL_RUNAWAY_TEMP_C)) {

    // If we have heated up by more than 20C since last sample point, snapshot time and tip temp
    int16_t delta = (int16_t)currentTipTempInC - (int16_t)tipTempCRunawayTemp;
    if (delta > THERMAL_RUNAWAY_TEMP_C) {
      // We have heated up more than the threshold, reset the timer
      tipTempCRunawayTemp   = currentTipTempInC;
      runawaylastChangeTime = xTaskGetTickCount();
    } else {
      if ((xTaskGetTickCount() - runawaylastChangeTime) > (THERMAL_RUNAWAY_TIME_SEC * TICKS_SECOND)) {
        // It has taken too long to rise
        heaterThermalRunaway = true;
      }
    }
  } else {
    tipTempCRunawayTemp   = currentTipTempInC;
    runawaylastChangeTime = xTaskGetTickCount();
  }
}

int32_t getX10WattageLimits() {
  int32_t limit = availableW10(0);

  if (getSettingValue(SettingsOptions::PowerLimit) && limit > (getSettingValue(SettingsOptions::PowerLimit) * 10)) {
    limit = getSettingValue(SettingsOptions::PowerLimit) * 10;
  }
  if (powerSupplyWattageLimit && limit > powerSupplyWattageLimit * 10) {
    limit = powerSupplyWattageLimit * 10;
  }
  return limit;
}

void setOutputx10WattsViaFilters(int32_t x10WattsOut) {
  static TickType_t lastPowerPulseStart = 0;
  static TickType_t lastPowerPulseEnd   = 0;
#ifdef SLEW_LIMIT
  static int32_t x10WattsOutLast = 0;
#endif

  // If the user turns on the option of using an occasional pulse to keep the power bank on
  if (getSettingValue(SettingsOptions::KeepAwakePulse)) {
    const TickType_t powerPulseWait = powerPulseWaitUnit * getSettingValue(SettingsOptions::KeepAwakePulseWait);
    if (xTaskGetTickCount() - lastPowerPulseStart > powerPulseWait) {
      const TickType_t powerPulseDuration = powerPulseDurationUnit * getSettingValue(SettingsOptions::KeepAwakePulseDuration);
      lastPowerPulseStart                 = xTaskGetTickCount();
      lastPowerPulseEnd                   = lastPowerPulseStart + powerPulseDuration;
    }

    // If current PID is less than the pulse level, check if we want to constrain to the pulse as the floor
    if (x10WattsOut < getSettingValue(SettingsOptions::KeepAwakePulse) && xTaskGetTickCount() < lastPowerPulseEnd) {
      x10WattsOut = getSettingValue(SettingsOptions::KeepAwakePulse);
    }
  }

  // Secondary safety check to forcefully disable header when within ADC noise of top of ADC
  if (getTipRawTemp(0) > (0x7FFF - 32)) {
    x10WattsOut = 0;
  }
  if (heaterThermalRunaway) {
    x10WattsOut = 0;
  }
#ifdef SLEW_LIMIT
  if (x10WattsOut - x10WattsOutLast > SLEW_LIMIT) {
    x10WattsOut = x10WattsOutLast + SLEW_LIMIT;
  }
  if (x10WattsOut < 0) {
    x10WattsOut = 0;
  }
  x10WattsOutLast = x10WattsOut;
#endif
  setTipX10Watts(x10WattsOut);
  resetWatchdog();
}