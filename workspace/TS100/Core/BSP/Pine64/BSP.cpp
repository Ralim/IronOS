// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Pins.h"
#include "Setup.h"
#include "gd32vf103_timer.h"
#include "history.hpp"
#include "main.hpp"
#include "systick.h"
#include <IRQ.h>

void resetWatchdog() {
  //TODO
}

uint16_t getTipInstantTemperature() {
  uint16_t sum = 0; // 12 bit readings * 8 -> 15 bits

  for (int i = 0; i < 4; i++) {
    sum += adc_inserted_data_read(ADC0, i);
    sum += adc_inserted_data_read(ADC1, i);
  }
  return sum; // 8x over sample
}

// Timer callbacks
// TODO
// Handle callback of the PWM modulator to enable / disable the output PWM

void unstick_I2C() {
  // TODO
}

uint8_t getButtonA() {
  return (gpio_input_bit_get(KEY_A_GPIO_Port, KEY_A_Pin) == SET) ? 1 : 0;
}
uint8_t getButtonB() {
  return (gpio_input_bit_get(KEY_B_GPIO_Port, KEY_B_Pin) == SET) ? 1 : 0;
}

void reboot() {
  // TODO
  for (;;) {
  }
}

void delay_ms(uint16_t count) { delay_1ms(count); }
