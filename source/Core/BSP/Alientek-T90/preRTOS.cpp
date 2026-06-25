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
#ifdef VECT_TAB_OFFSET
  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
#endif
  // The Alientek update-mode bootloader hands control off with SysTick still running and its
  // interrupt enabled. Left as-is, that leftover SysTick fires xPortSysTickHandler before the
  // FreeRTOS scheduler (and its task lists) exist and faults. Stop it and clear any pending tick;
  // the FreeRTOS port reconfigures SysTick from scratch in xPortStartScheduler.
  SysTick->CTRL = 0;
  SCB->ICSR     = SCB_ICSR_PENDSTCLR_Msk;

  // This is a SOFT-FLOAT build (-mfloat-abi=soft; the ELF contains zero VFP instructions) running on
  // the FreeRTOS ARM_CM3 port, whose PendSV neither saves nor restores an FPU exception frame. The
  // Alientek update-mode bootloader (the factory firmware is hard-float) hands control off with
  // CONTROL.FPCA still set, so the core would stack an *extended* (FP) frame on every exception
  // (EXC_RETURN bit4=0 -> 0xFFFFFFED, observed) and lazily push S0-S15 into stack slots the CM3 port
  // never accounts for. That clobbers the saved return address of a preempted task -> the deterministic
  // UsageFault INVSTATE seen in the PID task. Force basic-frame-only behaviour: drop the inherited
  // FP-active state, disable automatic + lazy FP context stacking, and leave CP10/CP11 denied (no FP
  // code exists, so a stray FP instruction should trap loudly rather than silently re-arm FP frames).
  __set_CONTROL(__get_CONTROL() & ~CONTROL_FPCA_Msk);
  __ISB();
  FPU->FPCCR &= ~(FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk);
  SCB->CPACR &= ~(0xFUL << 20); // CP10 & CP11 = access denied (soft-float)
  __DSB();
  __ISB();

  // The bootloader hands control off with peripheral interrupts left enabled and pending (it just
  // finished flash-writing the app, so FLASH_IRQn + its EOP flag are armed). Once we unmask, that
  // leftover fires into a vector this app does not service and UsageFaults. Start from a clean NVIC
  // state; Setup_HAL re-enables only the ADC and TIM4 channels this BSP actually uses.
  for (uint32_t i = 0; i < 8U; i++) {
    NVIC->ICER[i] = 0xFFFFFFFFUL; // disable all IRQs
    NVIC->ICPR[i] = 0xFFFFFFFFUL; // clear all pending
  }
  __DSB();
  __ISB();

  // NOTE: interrupts stay MASKED through all pre-scheduler peripheral bring-up. The ADC/TIM4 ISRs
  // are armed in Setup_HAL but their RTOS consumers (PID task / pidTaskNotification) do not exist
  // yet; letting them fire here faulted the iron during the display init delay. They are unmasked
  // at the end, just before control returns to the scheduler startup.

  Setup_HAL(); // clocks, GPIO, ADC, TIM, IWDG, SPI, NVIC
  BSPInit();   // apply the production heater PWM period

#ifdef CH224_SOFT_I2C
  ch224q_init(); // request USB_PD_VMAX from the CH224Q over the bit-bang I2C bus
#endif

  GC9Display::FRToSInit(); // GC9-family color SPI display bring-up

  // the Alientek bootloader may hand off with interrupts masked; unmask now that bring-up is done.
  __enable_irq();
}
