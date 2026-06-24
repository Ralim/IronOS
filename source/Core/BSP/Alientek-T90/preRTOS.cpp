/*
 * preRTOS.cpp
 *
 *  Pre-scheduler hardware bring-up for the Alientek T90 (Nations N32L40x).
 *  Mirrors Sequre's preRTOS order, retargeted to the N32 std-periph driver.
 */

#include "BSP.h"
#include "GC9Display.hpp"
#include "Pins.h"
#include "Power.h"
#include "Setup.h"
#include "n32l40x.h"

void preRToSInit() {
  // SystemInit (startup) already relocated VTOR using the build flag -D VECT_TAB_OFFSET.
  // Reassert defensively so interrupts vector into the app table at the flash origin, not the
  // bootloader table at 0x08000000. Uses the same build macro the linker ORIGIN is derived from.
#ifdef VECT_TAB_OFFSET
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
#endif
  // the Alientek bootloader may hand off with interrupts masked; unmask before we set up peripherals.
  __enable_irq();

  Setup_HAL(); // clocks, GPIO, ADC, TIM, IWDG, SPI, NVIC
  BSPInit();   // apply the production heater PWM period

#ifdef CH224_SOFT_I2C
  ch224q_init(); // request USB_PD_VMAX from the CH224Q over the bit-bang I2C bus
#endif

  GC9Display::FRToSInit(); // GC9-family color SPI display bring-up
}
