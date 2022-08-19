/*
 * IRQ.c
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#include "IRQ.h"
#include "Pins.h"
#include "configuration.h"
volatile uint8_t  i2c_read_process  = 0;
volatile uint8_t  i2c_write_process = 0;
volatile uint8_t  i2c_slave_address = 0;
volatile uint8_t  i2c_error_code    = 0;
volatile uint8_t *i2c_write;
volatile uint8_t *i2c_read;
volatile uint16_t i2c_nbytes;
volatile uint16_t i2c_write_dress;
volatile uint16_t i2c_read_dress;
volatile uint8_t  i2c_process_flag = 0;
static bool       fastPWM;
static void       switchToSlowPWM(void);
static void       switchToFastPWM(void);
void              ADC0_1_IRQHandler(void) {

  adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
  // unblock the PID controller thread
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (pidTaskNotification) {
      vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  }
}

volatile uint16_t PWMSafetyTimer = 0;
volatile uint8_t  pendingPWM     = 0;
void              TIMER1_IRQHandler(void) {
  static bool lastPeriodWasFast = false;

  if (timer_interrupt_flag_get(TIMER1, TIMER_INT_UP) == SET) {
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);
    // rollover turn on output if required
    if (PWMSafetyTimer) {
      PWMSafetyTimer--;
      if (lastPeriodWasFast != fastPWM) {
        if (fastPWM) {
          switchToFastPWM();
        } else {
          switchToSlowPWM();
        }
      }
      if (pendingPWM) {
        timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, pendingPWM);
        timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 50);
      } else {
        timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 0);
      }
    }
  }
  if (timer_interrupt_flag_get(TIMER1, TIMER_INT_CH1) == SET) {
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_CH1);
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 0);
  }
}

void switchToFastPWM(void) {
  fastPWM           = true;
  totalPWM          = powerPWM + tempMeasureTicks + holdoffTicks;
  TIMER_CAR(TIMER1) = (uint32_t)totalPWM;

  // ~10Hz
  TIMER_CH0CV(TIMER1) = powerPWM + holdoffTicks;
  // 1 kHz tick rate
  TIMER_PSC(TIMER1) = 18000;
}

void switchToSlowPWM(void) {
  // 5Hz
  fastPWM             = false;
  totalPWM            = powerPWM + tempMeasureTicks / 2 + holdoffTicks / 2;
  TIMER_CAR(TIMER1)   = (uint32_t)totalPWM;
  TIMER_CH0CV(TIMER1) = powerPWM + holdoffTicks / 2;
  TIMER_PSC(TIMER1)   = 36000;
}
void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  PWMSafetyTimer = 10; // This is decremented in the handler for PWM so that the tip pwm is
                       // disabled if the PID task is not scheduled often enough.
  pendingPWM = pulse;
  fastPWM    = shouldUseFastModePWM;
}
extern osThreadId POWTaskHandle;

void EXTI5_9_IRQHandler(void) {
#if POW_PD
  if (RESET != exti_interrupt_flag_get(EXTI_5)) {
    exti_interrupt_flag_clear(EXTI_5);

    if (POWTaskHandle != nullptr) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      xTaskNotifyFromISR(POWTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken);
      /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
      The macro used to do this is dependent on the port and may be called
      portEND_SWITCHING_ISR. */
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  }
#endif
}

bool getFUS302IRQLow() {
  // Return true if the IRQ line is still held low
  return (RESET == gpio_input_bit_get(FUSB302_IRQ_GPIO_Port, FUSB302_IRQ_Pin));
}
// These are unused for now
void I2C0_EV_IRQHandler(void) {}

void I2C0_ER_IRQHandler(void) {}
