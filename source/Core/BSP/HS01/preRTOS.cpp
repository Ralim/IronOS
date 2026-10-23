/*
 * preRTOS.cpp
 *
 * FNIRSI HS-01 (WCH CH571F)
 */

#include "BSP.h"
#include "Pins.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>

void preRToSInit() {
  // Normal system bringup -- GPIO etc
  hardware_init();
  BSPInit();
  I2CInit();
  // Same reset pulse as the stock firmware
  GPIOB_ResetBits(OLED_RESET_Pin);
  delay_ms(1);
  GPIOB_SetBits(OLED_RESET_Pin);
  delay_ms(1);
  FRToSI2C::FRToSInit();
}

// newlib's __libc_init_array calls this before the constructors; nothing to do here
extern "C" void _init(void) {}
