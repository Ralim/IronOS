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
const uint8_t  holdoffTicks     = 25; // delay of 7 ms
const uint8_t  tempMeasureTicks = 25;

uint16_t totalPWM; // htim2.Init.Period, the full PWM cycle

// 2 second filter (ADC is PID_TIM_HZ Hz)
history<uint16_t, PID_TIM_HZ> rawTempFilter = {{0}, 0, 0};
void                          resetWatchdog() { fwdgt_counter_reload(); }

uint16_t getTipInstantTemperature() {
  volatile uint16_t sum = 0; // 12 bit readings * 8*2 -> 16 bits

  for (int i = 0; i < 4; i++) {
    sum += adc_inserted_data_read(ADC0, i);
    sum += adc_inserted_data_read(ADC1, i);
  }
  return sum; // 8x over sample
}

uint16_t getTipRawTemp(uint8_t refresh) {
  if (refresh) {
    uint16_t lastSample = getTipInstantTemperature();
    rawTempFilter.update(lastSample);
    return lastSample;
  } else {
    return rawTempFilter.average();
  }
}

uint16_t getHandleTemperature() {
#ifdef TEMP_TMP36
  // We return the current handle temperature in X10 C
  // TMP36 in handle, 0.5V offset and then 10mV per deg C (0.75V @ 25C for
  // example) STM32 = 4096 count @ 3.3V input -> But We oversample by 32/(2^2) =
  // 8 times oversampling Therefore 32768 is the 3.3V input, so 0.1007080078125
  // mV per count So we need to subtract an offset of 0.5V to center on 0C
  // (4964.8 counts)
  //
  int32_t result = getADC(0);
  result -= 4965; // remove 0.5V offset
  // 10mV per C
  // 99.29 counts per Deg C above 0C
  result *= 100;
  result /= 993;
  return result;
#else
#error
#endif
}
uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {

  static uint8_t  preFillneeded = 10;
  static uint32_t samples[BATTFILTERDEPTH];
  static uint8_t  index = 0;
  if (preFillneeded) {
    for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
      samples[i] = getADC(1);
    preFillneeded--;
  }
  if (sample) {
    samples[index] = getADC(1);
    index          = (index + 1) % BATTFILTERDEPTH;
  }
  uint32_t sum = 0;

  for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
    sum += samples[i];

  sum /= BATTFILTERDEPTH;
  if (divisor == 0) {
    divisor = 1;
  }
  return sum * 4 / divisor;
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

void reboot() {
  // Spin for watchdog
  for (;;) {}
}

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
