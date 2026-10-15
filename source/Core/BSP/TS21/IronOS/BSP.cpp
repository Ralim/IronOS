#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "TipThermoModel.h"
#include "main.hpp"
#include "ts21.h"
#include <limits.h>
#ifdef TS21_HEATER_TEST
#include "OperatingModes.h"
#include "Settings.h"
#include "power.hpp"
#endif

volatile struct TS21Status ts21_status;
const uint16_t             powerPWM = 255;
uint16_t                   totalPWM = 255;

static StaticSemaphore_t sampleLockBuffer;
static SemaphoreHandle_t sampleLock;
static TS21Sample        sensors;
static bool              sensorsValid;
static bool              sensorsEverValid;
static int16_t           handleTemperature;

#ifdef TS21_HEATER_TEST
static volatile bool heaterArmed;
static bool          heaterButtonPressed;

bool ts21_heater_permitted(void) {
  return heaterArmed && currentOperatingMode == OperatingMode::Soldering && currentTempTargetDegC > 0 && currentTempTargetDegC <= TS21_HEATER_CUTOFF_C && heaterThermalRunawayCounter <= 8;
}

void ts21_heater_failed(void) {
  heaterArmed                 = false;
  currentTempTargetDegC       = 0;
  heaterThermalRunawayCounter = 255;
}
#endif

static void sampleSensors(bool resetHeaterFaults = false) {
  configASSERT(xSemaphoreTake(sampleLock, portMAX_DELAY) == pdTRUE);
  TS21Sample next = {};
  sensorsValid    = ts21_sample_sensors(&next);
  if (sensorsValid) {
    sensors                = next;
    sensorsValid           = ts21_handle_c_x10(next.handle, &handleTemperature);
    ts21_status.raw_adc[0] = next.tip;
    ts21_status.raw_adc[1] = next.handle;
    ts21_status.raw_adc[2] = next.auxiliary;
    ts21_status.raw_adc[3] = next.input;
  }
  ts21_status.adc_valid_mask = sensorsValid ? 15 : 0;
  // A single failed conversion is transient; only a sustained failure is a real sensor fault.
  static uint8_t sensorFailures;
  if (sensorsValid) {
    sensorsEverValid = true;
    sensorFailures   = 0;
  } else {
    if (sensorFailures < 4) {
      sensorFailures++;
    }
    if (sensorFailures >= 4) {
      ts21_status.errors |= TS21_ERROR_SENSOR;
    }
  }
#ifdef TS21_HEATER_TEST
  // Reset before releasing the sample lock; the PID can consume the sample on wakeup.
  if (resetHeaterFaults && ts21_heater_reset_faults()) {
    requestPIDReset();
    heaterThermalRunawayCounter = 0;
  }
#else
  (void)resetHeaterFaults;
#endif
  xSemaphoreGive(sampleLock);
}

#ifdef TS21_HEATER_TEST
void ts21_heater_acknowledge(void) {
  heaterArmed           = false;
  heaterButtonPressed   = false;
  currentTempTargetDegC = 0;
  sampleSensors(true);
}
#endif

extern "C" void ts21_run_constructors(void) {
  extern void (*__init_array_start[])(void);
  extern void (*__init_array_end[])(void);
  for (auto function = __init_array_start; function != __init_array_end; ++function) {
    (*function)();
  }
}

void ts21_fault(uint32_t error) {
  __asm volatile("csrrci zero, mstatus, 8" ::: "memory");
  ts21_status.errors |= error;
  for (;;) {
    ts21_heater_inhibit();
    ts21_watchdog_refresh();
  }
}

void BSPInit(void) {
  ts21_status.signature = 0x54533231;
  if (!ts21_board_init()) {
    ts21_fault(TS21_ERROR_CLOCK);
  }
  sampleLock = xSemaphoreCreateMutexStatic(&sampleLockBuffer);
  configASSERT(sampleLock != nullptr);
  FRToSI2C::FRToSInit();
  sampleSensors();
}

void preRToSInit() { BSPInit(); }
void postRToSInit() {}
void resetWatchdog() {
#ifdef TS21_HEATER_TEST
  ts21_heater_check();
#endif
  ts21_watchdog_refresh();
}

int16_t getHandleTemperature(uint8_t) { return sensorsValid ? handleTemperature : INT16_MAX; }

uint16_t getTipRawTemp(uint8_t refresh) {
  if (refresh) {
    sampleSensors();
  }
  // Hold the last good reading over a dropped conversion; a pegged value would look like a stuck ADC
  // to the shared thermal-runaway monitor.
  if (sensorsEverValid) {
    return sensors.tip;
  }
  return ADC_MAX_READING - 1;
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t) {
  if (!sensorsValid || divisor == 0) {
    ts21_status.errors |= TS21_ERROR_SENSOR;
    return 0;
  }
  return ts21_input_voltage_x10(sensors.input) * VOLTAGE_DIV / divisor;
}

uint8_t getButtonA() {
  const uint8_t buttons = ts21_buttons();
#ifdef TS21_HEATER_TEST
  if (currentOperatingMode == OperatingMode::HomeScreen && !(ts21_status.errors & TS21_HEATER_INHIBIT_ERRORS)) {
    if (buttons == 1) {
      heaterButtonPressed = true;
    } else {
      heaterArmed |= buttons == 0 && heaterButtonPressed;
      heaterButtonPressed = false;
    }
  } else {
    heaterButtonPressed = false;
  }
#endif
  return (buttons & 1) != 0;
}
uint8_t getButtonB() { return (ts21_buttons() & 2) != 0; }

void delay_ms(uint16_t count) {
  while (count--) {
    if (!ts21_delay_us(1000)) {
      ts21_fault(TS21_ERROR_CLOCK);
    }
    resetWatchdog();
  }
}

void reboot() {
  __asm volatile("csrrci zero, mstatus, 8" ::: "memory");
  ts21_heater_inhibit();
  *(volatile uint32_t *)0xE000E048 = 0xBEEF0080;
  ts21_fault(TS21_ERROR_TRAP);
}

void unstick_I2C() { ts21_fault(TS21_ERROR_I2C_BUS); }
void showBootLogoIfavailable() {}
void showBootLogo() {
#ifdef TS21_HEATER_TEST
  // Keep the experimental heater manual-start and power-limited.
  setSettingValue(SettingsOptions::AutoStartMode, 0);
  setSettingValue(SettingsOptions::BoostTemp, 0);
  setSettingValue(SettingsOptions::KeepAwakePulse, 0);
  setSettingValue(SettingsOptions::PowerLimit, POWER_LIMIT);
#endif
}
bool     getHallSensorFitted() { return false; }
int16_t  getRawHallEffect() { return 0; }
bool     getIsPoweredByDCIN() { return false; }
void     log_system_state(int32_t) {}
bool     getFUS302IRQLow() { return false; }
void     power_check() { ts21_uart_poll(); }
uint8_t  getTipResistanceX10() { return sensorsValid ? ts21_tip_resistance_x10(&sensors) : 0; }
uint16_t getTipThermalMass() { return TIP_THERMAL_MASS; }
uint16_t getTipInertia() { return TIP_THERMAL_INERTIA; }
uint8_t  preStartChecks() { return sensorsValid; }
uint8_t  preStartChecksDone() { return sensorsValid; }
bool     isTipShorted() {
#ifdef TS21_HEATER_TEST
  return sensorsValid && sensors.tip < 4090 && sensors.tip_probe < 4090 && ts21_tip_resistance_x10(&sensors) < TS21_HEATER_MIN_R_X10;
#else
  return false;
#endif
}
bool isTipDisconnected() {
#ifdef TS21_HEATER_TEST
  const uint8_t resistance = getTipResistanceX10();
  if (resistance < TS21_HEATER_MIN_R_X10 || resistance > TS21_HEATER_MAX_R_X10) {
    return true;
  }
#endif
  return !sensorsValid || sensors.tip == 0 || sensors.tip >= 4090;
}
void setStatusLED(const enum StatusLED) {}
void setBuzzer(bool) {}

uint64_t getDeviceID() {
  const volatile uint32_t *uid = (const volatile uint32_t *)0x1FFFF7E8;
  return ((uint64_t)uid[1] << 32) | uid[0];
}

TemperatureType_t getCustomTipMaxInC() {
#ifdef TS21_HEATER_TEST
  return TS21_HEATER_CUTOFF_C;
#else
  return 500;
#endif
}
TemperatureType_t TipThermoModel::convertTipRawADCToDegC(uint16_t rawADC) {
  int16_t value;
  if (!ts21_tip_delta_c(rawADC, &value)) {
    ts21_status.errors |= TS21_ERROR_SENSOR;
    return 500;
  }
  return value;
}
