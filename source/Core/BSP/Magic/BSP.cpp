// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "IRQ.h"
#include "Pins.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"

// These control the period's of time used for the PWM
const uint16_t powerPWM         = 255;
const uint8_t  holdoffTicks     = 10;
const uint8_t  tempMeasureTicks = 14;

uint16_t totalPWM = 255; // Total length of the cycle's ticks

void resetWatchdog() {
  //#TODO
}

uint16_t getHandleTemperature(uint8_t sample) {

  int32_t result = getADCHandleTemp(sample);
  result -= 10240; // remove 0.5V offset
  // 10mV per C
  // 204.7 counts per Deg C above 0C
  result *= 10;
  result /= 205;
  if (result < 0) {
    result = 0;
  }
  return result;
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

void unstick_I2C() {
  /* configure SDA/SCL for GPIO */
  // GPIO_BC(GPIOB) |= SDA_Pin | SCL_Pin;
  // gpio_init(SDA_GPIO_Port, GPIO_MODE_OUT_OD, GPIO_OSPEED_50MHZ, SDA_Pin | SCL_Pin);
  // for (int i = 0; i < 8; i++) {
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   GPIO_BOP(GPIOB) |= SCL_Pin;
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   asm("nop");
  //   GPIO_BOP(GPIOB) &= SCL_Pin;
  // }
  // /* connect PB6 to I2C0_SCL */
  // /* connect PB7 to I2C0_SDA */
  // gpio_init(SDA_GPIO_Port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, SDA_Pin | SCL_Pin);
}

uint8_t getButtonA() {
  uint8_t val = gpio_read(KEY_A_Pin);
  return val;
}
uint8_t getButtonB() {
  uint8_t val = gpio_read(KEY_B_Pin);
  return val;
}

void reboot() {
  // Spin for watchdog
  for (;;) {}
}

void delay_ms(uint16_t count) {
  // delay_1ms(count);
  BL702_Delay_MS(count);
}

uint32_t __get_IPSR(void) {
  return 0; // To shut-up CMSIS
}

bool isTipDisconnected() {

  uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
  uint32_t tipTemp              = TipThermoModel::getTipInC();
  return tipTemp > tipDisconnectedThres;
}

void setStatusLED(const enum StatusLED state) {
  // Dont have one
}
