// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "IRQ.h"
#include "Pins.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "gd32vf103_timer.h"
#include "history.hpp"
#include "main.hpp"

const uint16_t powerPWM         = 255;
const uint8_t  holdoffTicks     = 10;
const uint8_t  tempMeasureTicks = 14;

uint16_t totalPWM; // Total length of the cycle's ticks

void resetWatchdog() { fwdgt_counter_reload(); }

uint16_t getHandleTemperature(uint8_t sample) {
#ifdef TEMP_TMP36
  // We return the current handle temperature in X10 C
  // TMP36 in handle, 0.5V offset and then 10mV per deg C (0.75V @ 25C for
  // example) STM32 = 4096 count @ 3.3V input -> But We oversample by 32/(2^2) =
  // 8 times oversampling Therefore 32768 is the 3.3V input, so 0.1007080078125
  // mV per count So we need to subtract an offset of 0.5V to center on 0C
  // (4964.8 counts)
  //
  int32_t result = getADCHandleTemp(sample);
  result -= 4965; // remove 0.5V offset
  // 10mV per C
  // 99.29 counts per Deg C above 0C
  result *= 100;
  result /= 993;
  return result;
#else
#error Pinecil only uses TMP36
#endif
}
uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

void unstick_I2C() {
  /* configure SDA/SCL for GPIO */
  GPIO_BC(GPIOB) |= SDA_Pin | SCL_Pin;
  gpio_init(SDA_GPIO_Port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, SDA_Pin | SCL_Pin);
  for (int i = 0; i < 8; i++) {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    GPIO_BOP(GPIOB) |= SCL_Pin;
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    GPIO_BOP(GPIOB) &= SCL_Pin;
  }
  /* connect PB6 to I2C0_SCL */
  /* connect PB7 to I2C0_SDA */
  gpio_init(SDA_GPIO_Port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, SDA_Pin | SCL_Pin);
}

uint8_t getButtonA() { return (gpio_input_bit_get(KEY_A_GPIO_Port, KEY_A_Pin) == SET) ? 1 : 0; }
uint8_t getButtonB() { return (gpio_input_bit_get(KEY_B_GPIO_Port, KEY_B_Pin) == SET) ? 1 : 0; }

void reboot() { eclic_system_reset(); }

void     delay_ms(uint16_t count) { delay_1ms(count); }
uint32_t __get_IPSR(void) {
  return 0; // To shut-up CMSIS
}

bool isTipDisconnected() {

  uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
  uint32_t tipTemp              = TipThermoModel::getTipInC();
  return tipTemp > tipDisconnectedThres;
}

void setStatusLED(const enum StatusLED state) {}

uint8_t  preStartChecks() { return 1; }
uint64_t getDeviceID() { return dbg_id_get(); }

uint8_t getTipResistanceX10() { return TIP_RESISTANCE; }

uint8_t preStartChecksDone() { return 1; }

uint8_t getTipThermalMass() { return TIP_THERMAL_MASS; }