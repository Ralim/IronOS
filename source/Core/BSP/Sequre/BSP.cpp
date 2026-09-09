// BSP mapping functions

#include "BSP.h"
#include "BootLogo.h"
#include "FS2711.hpp"
#include "HUB238.hpp"
#include "I2C_Wrapper.hpp"
#include "Pins.h"
#include "Settings.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include <IRQ.h>

volatile uint16_t    PWMSafetyTimer   = 0;
volatile uint8_t     pendingPWM       = 0;
const uint16_t       powerPWM         = 255;
static const uint8_t holdoffTicks     = 15; // delay of 8 ish ms
static const uint8_t tempMeasureTicks = 15;

uint16_t totalPWM = powerPWM + tempMeasureTicks + holdoffTicks; // htim2.Init.Period, the full PWM cycle

void resetWatchdog() { HAL_IWDG_Refresh(&hiwdg); }
// Lookup table for the NTC
// We dont know exact specs, but it loooks to be roughly a 10K B=4000 NTC
// Stored as ADCReading,Temp in degC
// 10K NTC to 3V3 with a 13K series resistor, read as 32767 * R / (R + 13K). The rows up to 44 C are the
// original ones; above that they are computed from the same curve (B = 4100, which reproduces those rows
// to within one ADC count) so the handle temperature stays usable for cold junction compensation and for
// derating when the handle and the MOSFET next to it get hot.
static const uint16_t NTCHandleLookup[] = {
    // ADC Reading , Temp in C
    23931, 0,  //
    23210, 2,  //
    22466, 4,  //
    21703, 6,  //
    20924, 8,  //
    20135, 10, //
    19338, 12, //
    18538, 14, //
    17738, 16, //
    16943, 18, //
    16156, 20, //
    15381, 22, //
    14621, 24, //
    13878, 26, //
    13155, 28, //
    12455, 30, //
    11778, 32, //
    11126, 34, //
    10501, 36, //
    9902,  38, //
    9330,  40, //
    8786,  42, //
    8269,  44, //
    7313,  48, //
    6460,  52, //
    5702,  56, //
    5031,  60, //
    4440,  64, //
    3921,  68, //
    3465,  72, //
    3065,  76, //
    2715,  80, //
};

int16_t getHandleTemperature(uint8_t sample) {
#ifdef TMP36_ADC1_CHANNEL
  int32_t result = getADCHandleTemp(sample);
  // S60 uses 10k NTC resistor
  // For now not doing interpolation
  for (uint32_t i = 0; i < (sizeof(NTCHandleLookup) / (2 * sizeof(uint16_t))); i++) {
    if (result > NTCHandleLookup[(i * 2) + 0]) {
      return NTCHandleLookup[(i * 2) + 1] * 10;
    }
  }
  return 85 * 10;
#else
  return 0; // Not implemented
#endif
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  // ADC maximum is 32767 == 3.3V at input == 28.05V at VIN
  // Therefore we can divide down from there
  // Multiplying ADC max by 4 for additional calibration options,
  // ideal term is 467
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

static void switchToFastPWM(void) {
  // 20Hz
  totalPWM             = powerPWM + tempMeasureTicks + holdoffTicks;
  htim2.Instance->ARR  = totalPWM;
  htim2.Instance->CCR1 = powerPWM + holdoffTicks;
  htim2.Instance->CCR4 = powerPWM;
  htim2.Instance->PSC  = 1500;
}

#ifdef TIP_CURRENT_LIMIT_CHOP
/*
 * The S99 has no inductor, so the tip is driven the same way as the S60: TIM4 runs a fast PWM whose duty
 * is the PID output, TIM2 only frames the ADC measurement window. On top of that the duty is capped at
 * I_limit / I_tip so the average current can never exceed what the supply allows (negotiated PDO, or the
 * user power limit on DC) - the S60 relies on the supply wattage limit in the PID for that, which leaves
 * the cold start uncovered. Realising the power as bursts (a 20 Hz envelope with the chop inside) was
 * tried first and trips the over-current protection of some chargers, so it is deliberately not done.
 */
static const uint16_t tipChopPeriodTicks = 64 + 1; // TIM4 ARR + 1
/*
 * PWM frequency. Switching loss is proportional to frequency x tip current and the gate drive is weak, so
 * the MOSFET runs hot at 11 kHz even with the 5.5 ohm cartridge (~0.4 W of switching loss on its own),
 * while switching slowly makes the current pulses longer, which supplies with little output capacitance
 * do not like. Rather than guessing a compromise from the cartridge resistance, the frequency is regulated
 * by the temperature of the MOSFET itself: the handle NTC sits next to it (which is why the power derate
 * uses it too), so as long as the handle stays cool the fastest sensible frequency is used, and each
 * temperature step drops it further (TIP_PWM_SLOWDOWN_1_C / _2_C, 5 C of hysteresis on the way back).
 *
 * Base step: while the duty is capped to the supply current the pulses should be as short as possible, so
 * it starts at the S60's 11.2 kHz (one step lower above 5.5 A, where a 2.5 ohm cartridge would otherwise
 * burn ~0.9 W in switching loss alone). Starting fast is only safe because the loop below walks it back
 * down as soon as the FET warms up. With no cap - or on DC, where there is no source to upset - it starts
 * at ~2.4 kHz, which is what the S60 does once it is resistance limited (see preStartChecks below).
 */
static const uint16_t tipChopPrescalers[]   = {10, 20, 40, 50, 80}; // ~11.2 / 5.9 / 3.0 / 2.4 / 1.5 kHz
static const uint8_t  tipChopPrescalerCount = sizeof(tipChopPrescalers) / sizeof(tipChopPrescalers[0]);

static uint8_t tipChopThermalStep(void) {
  static uint8_t step    = 0;
  const int16_t  handleC = getHandleTemperature(0) / 10;
  const int16_t  upper   = TIP_PWM_SLOWDOWN_2_C;
  const int16_t  lower   = upper - (TIP_PWM_SLOWDOWN_2_C - TIP_PWM_SLOWDOWN_1_C);
  if (step == 0 && handleC >= lower) {
    step = 1;
  } else if (step == 1 && handleC >= upper) {
    step = 2;
  } else if (step == 2 && handleC < (upper - 5)) {
    step = 1;
  } else if (step == 1 && handleC < (lower - 5)) {
    step = 0;
  }
  return step;
}

static uint16_t          tipChopPrescaler = 50;
static volatile uint16_t tipDutyCapTicks  = tipChopPeriodTicks; // Largest TIM4 CCR3 the supply allows; ARR+1 == no limit
static uint16_t          tipChopDutyX256  = 256;

static void applyTipChopPrescaler(uint16_t prescaler) {
  tipChopPrescaler = prescaler;
  if (htim4.Instance->PSC != tipChopPrescaler) {
    htim4.Instance->PSC = tipChopPrescaler;
    htim4.Instance->EGR = TIM_EGR_UG; // Load the new prescaler now rather than at next update
  }
}

uint32_t getTipChopFrequencyHzX10() { return (8000000UL * 10) / ((uint32_t)(tipChopPrescaler + 1) * tipChopPeriodTicks); }

uint16_t getTipChopDutyX256Latched() { return tipChopDutyX256; }

uint16_t getTipChopDutyX256() {
  // Tip current at the present input voltage
  uint32_t voltageX10     = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
  uint32_t tipRx10        = getTipResistanceX10();
  uint32_t tipCurrentx100 = 0;
  if (tipRx10 > 0 && voltageX10 > 0) {
    tipCurrentx100 = (voltageX10 * 100) / tipRx10;
  }

  // Current the supply is allowed to deliver
  uint32_t limitx100 = 0;
#if POW_PD_EXT == 2
  if (!getIsPoweredByDCIN() && FS2711::has_run_selection()) {
    limitx100 = FS2711::source_currentx100();
  }
#endif
  // The user power limit is also treated as a supply limit (this is how DC users describe their supply)
  const uint32_t userLimitW = getSettingValue(SettingsOptions::PowerLimit);
  if (userLimitW && voltageX10) {
    uint32_t userLimitx100 = (userLimitW * 1000) / voltageX10;
    if (limitx100 == 0 || userLimitx100 < limitx100) {
      limitx100 = userLimitx100;
    }
  }

  uint32_t dutyX256 = 256;
  if (limitx100 && tipCurrentx100 > limitx100) {
    // 10 % margin: the cartridge is below its nominal resistance when cold, and the meters / protections
    // on the supply side see the peaks, not just the average
    dutyX256 = (limitx100 * 256 * 9) / (tipCurrentx100 * 10);
    if (dutyX256 < 8) {
      dutyX256 = 8; // ~3 %, keeps the output alive so the tip still gets measured meaningfully
    }
  }
  tipChopDutyX256 = dutyX256;

  uint16_t cap = tipChopPeriodTicks;
  if (dutyX256 < 256) {
    cap = (dutyX256 * tipChopPeriodTicks) / 256;
    if (cap < 1) {
      cap = 1;
    }
  }
  tipDutyCapTicks = cap;

  uint8_t step = (dutyX256 >= 256 || getIsPoweredByDCIN()) ? 3 : (tipCurrentx100 > 550 ? 1 : 0);
  step += tipChopThermalStep();
  if (step >= tipChopPrescalerCount) {
    step = tipChopPrescalerCount - 1;
  }
  applyTipChopPrescaler(tipChopPrescalers[step]);
  return tipChopDutyX256;
}
#endif /* TIP_CURRENT_LIMIT_CHOP */

void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  (void)shouldUseFastModePWM;
  PWMSafetyTimer = 20; // This is decremented in the handler for PWM so that the tip pwm is
                       // disabled if the PID task is not scheduled often enough.
  pendingPWM = pulse;
#ifdef TIP_CURRENT_LIMIT_CHOP
  getTipChopDutyX256(); // Re-evaluate the supply current cap every PID cycle (voltage / PDO / user limit can change)
#endif
}
// These are called by the HAL after the corresponding events from the system
// timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Period has elapsed
  if (htim->Instance == TIM2) {
    // we want to turn on the output again
    if (PWMSafetyTimer) {
      PWMSafetyTimer--;
    }
    // We decrement this safety value so that lockups in the
    // scheduler will not cause the PWM to become locked in an
    // active driving state.
    // While we could assume this could never happen, its a small price for
    // increased safety
    if (PWMSafetyTimer == 0) {
      htim4.Instance->CCR3 = 0;
    } else {
      uint16_t duty = pendingPWM / 4; // 0..63 of the 65 tick fast PWM period
#ifdef TIP_CURRENT_LIMIT_CHOP
      if (duty > tipDutyCapTicks) {
        duty = tipDutyCapTicks; // never draw more than the supply can deliver, whatever the PID asks for
      }
#endif
      htim4.Instance->CCR3 = duty;
    }
  } else if (htim->Instance == TIM1) {
    // STM uses this for internal functions as a counter for timeouts
    HAL_IncTick();
  }
#ifdef BUZZER_Pin
  else if (htim->Instance == TIM3) {
    // Buzzer tone: square wave on a plain GPIO
    HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
  }
#endif
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  // End of the drive window: output off while the ADC samples the tip
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
    htim4.Instance->CCR3 = 0;
  }
}

uint8_t getButtonA() { return HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin) == GPIO_PIN_RESET ? 1 : 0; }
uint8_t getButtonB() { return HAL_GPIO_ReadPin(KEY_B_GPIO_Port, KEY_B_Pin) == GPIO_PIN_RESET ? 1 : 0; }

void BSPInit(void) { switchToFastPWM(); }

void reboot() { NVIC_SystemReset(); }

void delay_ms(uint16_t count) { HAL_Delay(count); }

bool isTipDisconnected() {

  uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
  uint32_t tipTemp              = TipThermoModel::getTipInC();
  return tipTemp > tipDisconnectedThres;
}

void    setStatusLED(const enum StatusLED state) {}
uint8_t preStartChecks() {
#if POW_PD_EXT == 1
  if (!hub238_has_run_selection() && (xTaskGetTickCount() < TICKS_SECOND * 5)) {
    return 0;
  }
  // We check if we are in a "Limited" mode; where we have to run the PWM really fast
  // Where as if we are on 9V for example, the tip resistance is enough
  uint16_t voltage     = hub238_source_voltage();
  uint16_t currentx100 = hub238_source_currentX100();
#endif
#if POW_PD_EXT == 2
  if (!FS2711::has_run_selection() && (xTaskGetTickCount() < TICKS_SECOND * 5)) {
    return 0;
  }
  uint16_t voltage     = FS2711::source_voltage();
  uint16_t currentx100 = FS2711::source_currentx100();
#endif

#ifdef TIP_CURRENT_LIMIT_CHOP
  (void)voltage;
  (void)currentx100;
  getTipChopDutyX256(); // Latch chop duty + prescaler for the negotiated supply before heating starts
#else
  uint16_t thresholdResistancex10 = ((voltage * 1000) / currentx100) + 5;

  if (getTipResistanceX10() <= thresholdResistancex10) {
    // We are limited by resistance, not our current limiting, we can slow down PWM to avoid audible noise
    htim4.Instance->PSC = 50; // 10 -> 500 removes audible noise
  }
#endif

  return 1; // We are done now
}
uint64_t getDeviceID() {
  //
  return HAL_GetUIDw0() | ((uint64_t)HAL_GetUIDw1() << 32);
}

uint8_t getTipResistanceX10() {
#ifdef COPPER_HEATER_COIL

  // TODO
  //! Warning, must never return 0.
  TemperatureType_t measuredTemperature = TipThermoModel::getTipInC(false);
  if (measuredTemperature < 25) {
    return 50; // Start assuming under spec to soft-start
  }

  // Assuming a temperature rise of 0.00393 per deg c over 20C

  uint32_t scaler = 393 * (measuredTemperature - 20);

  return TIP_RESISTANCE + ((TIP_RESISTANCE * scaler) / 100000);
#else
  uint8_t user_selected_tip = getUserSelectedTipResistance();
  if (user_selected_tip == 0) {
    return TIP_RESISTANCE; // Auto mode
  }
  return user_selected_tip;
#endif
}
bool    isTipShorted() { return false; }
uint8_t preStartChecksDone() { return 1; }

uint16_t getTipThermalMass() { return TIP_THERMAL_MASS; }
uint16_t getTipInertia() { return TIP_THERMAL_INERTIA; }

void setBuzzer(bool on) {
#ifdef BUZZER_Pin
  if (on && !getSettingValue(SettingsOptions::BuzzerEnabled)) {
    on = false; // Muted by the user
  }
  if (on) {
    HAL_TIM_Base_Start_IT(&htim3);
  } else {
    HAL_TIM_Base_Stop_IT(&htim3);
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
  }
#else
  (void)on;
#endif
}

void showBootLogo(void) { BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR); }

#ifdef CUSTOM_MAX_TEMP_C
TemperatureType_t getCustomTipMaxInC() { return MAX_TEMP_C; }
#endif
