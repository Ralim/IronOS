// BSP mapping functions

#include "BSP.h"
#include "BootLogo.h"
#include "I2C_Wrapper.hpp"
#include "IRQ.h"
#include "Pins.h"
#include "Settings.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "main.hpp"

const uint16_t powerPWM         = 255;
const uint8_t  holdoffTicks     = 10; // ~4.3 ms
const uint8_t  tempMeasureTicks = 20;

uint16_t totalPWM = powerPWM + tempMeasureTicks + holdoffTicks; // Total length of the cycle's ticks

void resetWatchdog() { WWDG_SetCounter(0); }

int16_t getHandleTemperature(uint8_t sample) {
  // There is no cold junction sensor; like the stock firmware, assume the handle sits at 25 C
  (void)sample;
  return 25 * 10;
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

uint8_t getButtonA() { return GPIOB_ReadPortPin(KEY_A_Pin) ? 0 : 1; }
uint8_t getButtonB() { return GPIOB_ReadPortPin(KEY_B_Pin) ? 0 : 1; }

void BSPInit(void) {}

void reboot() { SYS_ResetExecute(); }

void delay_ms(uint16_t count) { DelayMs(count); }

// Same threshold as the stock firmware, well above anything the PID can reach
bool isTipDisconnected() { return TipThermoModel::getTipInC() > 800; }

void setStatusLED(const enum StatusLED state) { (void)state; }
void setBuzzer(bool on) { (void)on; }

uint8_t preStartChecks() {
  // Hold the heater off until the CH224K has settled on a voltage (see Power.cpp)
  return CH224KVoltageSettled() ? 1 : 0;
}
uint8_t preStartChecksDone() { return 1; }

uint64_t getDeviceID() {
  uint64_t id = 0;
  taskENTER_CRITICAL(); // Reads the flash info area, see flash.c
  GET_UNIQUE_ID((uint8_t *)&id);
  taskEXIT_CRITICAL();
  return id;
}

uint8_t getTipResistanceX10() {
  uint8_t user_selected_tip = getUserSelectedTipResistance();
  if (user_selected_tip == 0) {
    return TIP_RESISTANCE; // Auto mode
  }
  return user_selected_tip;
}
bool isTipShorted() { return false; }

uint16_t getTipThermalMass() { return TIP_THERMAL_MASS; }
uint16_t getTipInertia() { return TIP_THERMAL_MASS; }

void showBootLogo(void) { BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR); }
