#include "BSP.h"
#include "Interrupts.h"
#include "main.hpp"

extern "C" void vPortSetupTimerInterrupt(void) {
  // Keep the pre-RTOS free-running clock used by the bounded I2C/ADC delays.
  NVIC_SetPriority(Software_IRQn, 0xF0);
  NVIC_SetPriority(SysTicK_IRQn, 0xF0);
  SysTick->SR   = 0;
  SysTick->CMP  = ts21_timer_now() + TS21_TIMER_HZ / configTICK_RATE_HZ;
  SysTick->CTLR = 3;
}

extern "C" void ts21_tick_reload(void) {
  SysTick->SR         = 0;
  uint64_t       next = SysTick->CMP + TS21_TIMER_HZ / configTICK_RATE_HZ;
  const uint64_t now  = ts21_timer_now();
  // Leave 100 us to write both compare halves before the deadline.
  if (next <= now + 100 * TS21_TIMER_TICKS_PER_US) {
    next = now + TS21_TIMER_HZ / configTICK_RATE_HZ;
  }
  SysTick->CMP = next;
}

extern "C" void vApplicationTickHook(void) {
  static unsigned sampleTicks;
#ifdef TS21_HEATER_TEST
  ts21_heater_check();
#endif
  ts21_uart_poll();
  if (++sampleTicks == configTICK_RATE_HZ / PID_TIM_HZ) {
    sampleTicks = 0;
    if (pidTaskNotification != nullptr) {
      BaseType_t wake = pdFALSE;
      vTaskNotifyGiveFromISR(pidTaskNotification, &wake);
      portYIELD_FROM_ISR(wake);
    }
  }
}
