// BSP mapping functions for the Alientek T90 (Nations N32L40x).
//
// Re-implements the BSP.h + BSP_Power.h contract against the N32 std-periph driver, following
// the Sequre S60 algorithms (the only other std-periph / Cortex-M4 IronOS target) but with no
// ST HAL: every HAL_* call is replaced with the equivalent Nations driver call or register poke.

#include "BSP.h"
#include "BootLogo.h"
#include "IRQ.h"
#include "Pins.h"
#include "Power.h"
#include "Settings.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "main.hpp"
#include "n32l40x.h"

// --- PWM globals (declared extern in IRQ.h, consumed by the TIM4 update ISR) ---
// powerPWM / totalPWM are defined in Setup.cpp (it owns the timer hardware config).
volatile uint16_t PWMSafetyTimer = 0;
volatile uint8_t  pendingPWM     = 0;

void resetWatchdog() {
  // Reload the independent watchdog (write the reload key). Setup.cpp owns the prescaler/reload.
  IWDG_ReloadKey();
}

// Cold-junction reference for the tip thermocouple, returned in deci-degC.
//
// The tip uV->degC curve in ThermoModel.cpp was reversed from the stock-firmware LUT, which is an
// ABSOLUTE temperature curve (factory `tip = lut_interp(opamp_input)`, disassembled at 0x800ce18): the
// factory does NOT add a live handle/cold-junction term at soldering temperatures - the breakpoints are
// absolute, anchored at a ~38 degC cold junction (the upper segment extrapolates to 38 degC at zero
// input). Our IronOS LUT stores those same breakpoints minus 38 (delta model), and getTipInC() then adds
// getHandleTemperature(). So returning a fixed 38 degC here makes `tip = (factory_abs - 38) + 38 =
// factory_abs`, i.e. it reproduces the factory reading exactly. The on-board NTC (PA2) runs through an
// uncharacterised divider, so a live reading just injects tens-of-degrees of error (the cause of the
// observed overheating); the small residual room-temp offset is trimmed by the on-device tip calibration.
// TODO: reverse the factory NTC curve to restore a live cold junction once a reference thermometer exists.
uint16_t getHandleTemperature(uint8_t sample) {
  (void)sample;
  return 38 * 10;
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  // ADC full-scale 32767 == 3.3V at the divider input. The 10:1 divider maps that to the DC rail.
  // Multiply by 4 for extra calibration headroom (ideal divisor ~= 467), then divide by the
  // runtime-adjustable divisor (the user VoltageDiv setting).
  if (divisor == 0) {
    divisor = VOLTAGE_DIV; // guard against a corrupted/zero VoltageDiv setting (no div-by-zero fault)
  }
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

static void switchToFastPWM(void) {
  // Setup.cpp (Setup_HAL) already programmed the production carrier (TIM2) and the ADC-schedule
  // timer (TIM4 ARR = totalPWM). Reassert the schedule period from totalPWM (idempotent) and
  // force the heater off; the TIM4 update ISR drives the duty from pendingPWM thereafter.
  TIM_SetAutoReload(TIM4, totalPWM);
  TIM_SetCmp1(TIM2, 0);
}

// Buzzer carrier on TIM1_CH1N (PA7). Setup.cpp enables the TIM1 clock and the PA7 alternate
// function but leaves the timer itself unconfigured, so set up the carrier + complementary
// output here. The master output enable (MOE) stays off until setBuzzer(true) gates it on.
static void buzzerInit(void) {
  TIM_TimeBaseInitType tb;
  TIM_InitTimBaseStruct(&tb);
  // TIM1 timer clock is 64 MHz at 64 MHz SYSCLK. PSC 63 -> 1 MHz tick, ARR 370 -> ~2.7 kHz.
  tb.Prescaler = 63;
  tb.Period    = 370;
  tb.CntMode   = TIM_CNT_MODE_UP;
  tb.ClkDiv    = 0;
  tb.RepetCnt  = 0;
  TIM_InitTimeBase(TIM1, &tb);

  OCInitType oc;
  TIM_InitOcStruct(&oc);
  oc.OcMode       = TIM_OCMODE_PWM1;
  oc.OutputState  = TIM_OUTPUT_STATE_DISABLE; // CH1 main output unused
  oc.OutputNState = TIM_OUTPUT_NSTATE_ENABLE; // CH1N drives the buzzer on PA7
  oc.Pulse        = 185;                      // ~50% duty for volume
  oc.OcPolarity   = TIM_OC_POLARITY_HIGH;
  oc.OcNPolarity  = TIM_OCN_POLARITY_HIGH;
  oc.OcIdleState  = TIM_OC_IDLE_STATE_RESET;
  oc.OcNIdleState = TIM_OCN_IDLE_STATE_RESET;
  TIM_InitOc1(TIM1, &oc);

  TIM_BDTRInitType bd;
  TIM_InitBkdtStruct(&bd);
  TIM_ConfigBkdt(TIM1, &bd); // defaults: no dead-time, break disabled

  TIM_Enable(TIM1, ENABLE);
  // MOE off: CH1N is high-Z (silent) until setBuzzer(true).
  TIM_EnableCtrlPwmOutputs(TIM1, DISABLE);
}

void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  (void)shouldUseFastModePWM; // fast mode is always used; slow-PWM is selected in preStartChecks
  // Arm the safety timeout: the TIM4 update ISR decrements this and forces the heater off if the
  // PID task stops scheduling often enough.
  // write the duty before re-arming the safety enable, with a barrier between, so the TIM4 update
  // ISR can never observe the timer armed against a stale (previous-cycle) duty value.
  pendingPWM = pulse;
  __DMB();
  PWMSafetyTimer = 20;
}

void unstick_I2C() {
#ifdef CH224_SOFT_I2C
  // Bit-bang bus recovery: toggle SCL (PB6) up to 100x until the slave releases SDA (PB7) high,
  // then leave both lines idle high. The CH224Q bus has no hardware-I2C PE/SWRST to clear.
  int timeout = 100;

  GPIO_SetBits(SCL2_GPIO_Port, SCL2_Pin);
  GPIO_SetBits(SDA2_GPIO_Port, SDA2_Pin);

  while (GPIO_ReadInputDataBit(SDA2_GPIO_Port, SDA2_Pin) != Bit_SET) {
    GPIO_ResetBits(SCL2_GPIO_Port, SCL2_Pin);
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    GPIO_SetBits(SCL2_GPIO_Port, SCL2_Pin);

    if (--timeout <= 0) {
      return;
    }
  }

  GPIO_SetBits(SCL2_GPIO_Port, SCL2_Pin);
  GPIO_SetBits(SDA2_GPIO_Port, SDA2_Pin);
#endif
}

// The T90 panel is mounted rotated 180 deg from the reference orientation (handle pointing up, which
// is the comfortable right-hand grip), so the two physical keys sit opposite the firmware's A/B
// convention. Map logical button A (front, the "+"/increase key) to the physically-upper key on PB4
// and logical button B (back, the "-"/decrease key) to PA15, so +/- match their on-screen meaning.
uint8_t getButtonA() { return GPIO_ReadInputDataBit(KEY_B_GPIO_Port, KEY_B_Pin) == Bit_RESET ? 1 : 0; }
uint8_t getButtonB() { return GPIO_ReadInputDataBit(KEY_A_GPIO_Port, KEY_A_Pin) == Bit_RESET ? 1 : 0; }

void BSPInit(void) {
  switchToFastPWM();
  buzzerInit();
}

void reboot() { NVIC_SystemReset(); }

void delay_ms(uint16_t count) {
  // Pre-RTOS busy delay calibrated for a 64 MHz core. ~64000 cycles per ms; the inner loop is a
  // few cycles per iteration, so ~16000 nops approximate 1 ms. Only used before the scheduler.
  for (uint16_t ms = 0; ms < count; ms++) {
    for (volatile uint32_t i = 0; i < 16000; i++) {
      asm("nop");
    }
  }
}

// --- Tip presence -------------------------------------------------------------------------------
// The T90's open thermocouple does NOT rail to the ADC ceiling like a classic Miniware tip (so the
// inherited rail-high test is useless), and the current-sense node is an unreliable floating input.
// Detect a missing tip the way it actually presents: while the iron is IDLE (no setpoint) a fitted
// tip sits near ambient, whereas an empty handle reads the external op-amp's open-circuit output
// (~360 degC, steady). So an idle reading that high == no tip. During soldering the reading is
// legitimately high, so we report present and let the standard thermal-runaway net cover a tip pulled
// mid-solder. HomeScreen gates entering soldering on this, so the iron never heats an empty handle.
#define TIP_DISCONNECT_IDLE_TEMP_C 300 // an idle tip reading above this == open-circuit (no tip)

bool isTipDisconnected() { return currentTempTargetDegC == 0 && TipThermoModel::getTipInC() > TIP_DISCONNECT_IDLE_TEMP_C; }

void setStatusLED(const enum StatusLED state) { (void)state; } // no status LED on the T90

uint8_t preStartChecks() {
#if defined(POW_PD_EXT) && (POW_PD_EXT == 3)
  // Wait up to 5s for the CH224Q PD negotiation to settle.
  if (ch224q_source_voltage() == 0 && (xTaskGetTickCount() < (TICKS_SECOND * 5))) {
    return 0;
  }
  uint16_t voltage     = ch224q_source_voltage();
  uint16_t currentx100 = ch224q_source_currentX100();

  uint16_t thresholdResistancex10 = ((voltage * 1000) / currentx100) + 5;

  if (getTipResistanceX10() <= thresholdResistancex10) {
    // Limited by tip resistance (not the supply current limit): slow the heater PWM carrier to
    // kill audible whine. The carrier is TIM2 (TIM4 is the ADC/safety schedule timer and must NOT
    // be represcaled here, or the safety countdown + sample cadence would speed up ~40x).
    TIM_ConfigPrescaler(TIM2, 50, TIM_PSC_RELOAD_MODE_IMMEDIATE);
  }
#endif
  return 1; // done
}

uint64_t getDeviceID() {
  // Compose a 64-bit ID from the N32L40x 96-bit UID (first two words).
  uint32_t w0 = *(volatile uint32_t *)(UID_BASE + 0);
  uint32_t w1 = *(volatile uint32_t *)(UID_BASE + 4);
  return ((uint64_t)w0) | ((uint64_t)w1 << 32);
}

uint32_t getDeviceValidation() { return 0; }
uint8_t  getDeviceValidationStatus() { return 0; } // validation passes

uint8_t getTipResistanceX10() {
  // T245 is a fixed-element cartridge (not a copper coil). Use the configured resistance unless
  // the user overrode it. Must never return 0.
  uint8_t user_selected_tip = getUserSelectedTipResistance();
  if (user_selected_tip == 0) {
    return TIP_RESISTANCE; // Auto mode
  }
  return user_selected_tip;
}

bool    isTipShorted() { return false; }
uint8_t preStartChecksDone() { return 1; }

uint16_t getTipThermalMass() { return TIP_THERMAL_MASS; }
uint16_t getTipInertia() { return TIP_THERMAL_INERTIA; }

bool    getHallSensorFitted() { return false; }
int16_t getRawHallEffect() { return 0; }

void setBuzzer(bool on) {
  // PA7 = TIM1_CH1N. Setup.cpp configures TIM1 (carrier, CH1N duty, BDTR). The complementary
  // output only drives when the master output enable (MOE) is set, so toggle MOE to gate sound.
  TIM_EnableCtrlPwmOutputs(TIM1, on ? ENABLE : DISABLE);
}

void log_system_state(int32_t PWMWattsx10) { (void)PWMWattsx10; } // no debug UART wired

void showBootLogo(void) { BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR); }

#ifdef CUSTOM_MAX_TEMP_C
TemperatureType_t getCustomTipMaxInC() { return MAX_TEMP_C; }
#endif
