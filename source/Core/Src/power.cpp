/*
 * power.cpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton <- Mostly David
 */

#include <BSP.h>
#include <Settings.h>
#include <power.hpp>

static int32_t PWMToX10Watts(uint8_t pwm, uint8_t sample);
const int      fastPWMChangeoverPoint     = 128;
const int      fastPWMChangeoverTolerance = 16;

expMovingAverage<uint32_t, wattHistoryFilter> x10WattHistory = {0};

bool shouldBeUsingFastPWMMode(const uint8_t pwmTicks) {
  // Determine if we should use slow or fast PWM mode
  // Crossover between modes set around the midpoint of the PWM control point
  static bool lastPWMWasFast = true;
  if (pwmTicks > (fastPWMChangeoverPoint + fastPWMChangeoverTolerance) && lastPWMWasFast) {
    lastPWMWasFast = false;
  } else if (pwmTicks < (fastPWMChangeoverPoint - fastPWMChangeoverTolerance) && !lastPWMWasFast) {
    lastPWMWasFast = true;
  }
  return lastPWMWasFast;
}

int32_t tempToX10Watts(int32_t rawTemp) {
  // mass is in x10J/*C, rawC is raw per degree C
  // returns x10Watts needed to raise/lower a mass by rawTemp
  //  degrees in one cycle.
  int32_t x10Watts = TIP_THERMAL_MASS * rawTemp;
  return x10Watts;
}

void setTipX10Watts(int32_t mw) {
  int32_t    outputPWMLevel   = X10WattsToPWM(mw, 1);
  const bool shouldUseFastPWM = shouldBeUsingFastPWMMode(outputPWMLevel);
  setTipPWM(outputPWMLevel, shouldUseFastPWM);
  uint32_t actualMilliWatts = PWMToX10Watts(outputPWMLevel, 0);

  x10WattHistory.update(actualMilliWatts);
}

uint32_t availableW10(uint8_t sample) {
  // P = V^2 / R, v*v = v^2 * 100
  //				R = R*10
  // P therefore is in V^2*100/R*10 = W*10.
  uint32_t v             = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), sample); // 100 = 10v
  uint32_t tipResistance = getTipResistanceX10();
  if (tipResistance == 0) {
    return 100; // say 100 watt to force scale down
  }
  uint32_t availableWattsX10 = (v * v) / tipResistance;
  // However, 100% duty cycle is not possible as there is a dead time while the ADC takes a reading
  // Therefore need to scale available milliwats by this

  // avMw=(AvMw*powerPWM)/totalPWM.
  availableWattsX10 = availableWattsX10 * powerPWM;
  availableWattsX10 /= totalPWM;

  // availableMilliWattsX10 is now an accurate representation
  return availableWattsX10;
}
uint8_t X10WattsToPWM(int32_t x10Watts, uint8_t sample) {
  // Scale input x10Watts to the pwm range available
  if (x10Watts <= 0) {
    // keep the battery voltage updating the filter
    getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), sample);
    return 0;
  }

  // Calculate desired x10Watts as a percentage of availableW10
  uint32_t pwm;
  pwm = (powerPWM * x10Watts) / availableW10(sample);
  if (pwm > powerPWM) {
    // constrain to max PWM counter
    pwm = powerPWM;
  }
  return pwm;
}

static int32_t PWMToX10Watts(uint8_t pwm, uint8_t sample) {
  uint32_t maxMW = availableW10(sample); // Get the milliwatts for the max pwm period
  // Then convert pwm into percentage of powerPWM to get the percentage of the max mw
  return (((uint32_t)pwm) * maxMW) / powerPWM;
}
