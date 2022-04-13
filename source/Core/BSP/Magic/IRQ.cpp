/*
 * IRQ.c
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#include "IRQ.h"
#include "Pins.h"
#include "configuration.h"
extern "C" {
#include "bflb_platform.h"
#include "bl702_glb.h"
#include "bl702_pwm.h"
#include "bl702_timer.h"
#include "hal_clock.h"
#include "hal_pwm.h"
#include "hal_timer.h"
}
void ADC0_1_IRQHandler(void) {

  // adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
  // unblock the PID controller thread
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (pidTaskNotification) {
      vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  }
}

static bool fastPWM;
static void switchToSlowPWM(void);
static void switchToFastPWM(void);

volatile uint16_t PWMSafetyTimer    = 0;
volatile uint8_t  pendingPWM        = 200;
volatile bool     lastPeriodWasFast = false;

// Timer 0 is used to co-ordinate the ADC and the output PWM
void timer0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state) {
  if (state == TIMER_EVENT_COMP0) {
    // MSG((char *)"timer event comp0! \r\n");
    // We use channel 0 to trigger the ADC, this occurs after the main PWM is done with a delay

  } else if (state == TIMER_EVENT_COMP1) {
    // MSG((char *)"timer event comp1! \r\n");
    // Channel 1 is end of the main PWM section; so turn off the output PWM
  } else if (state == TIMER_EVENT_COMP2) {
    // This occurs at timer rollover, so if we want to turn on the output PWM; we do so
    if (PWMSafetyTimer) {
      PWMSafetyTimer--;
      if (lastPeriodWasFast != fastPWM) {
        if (fastPWM) {
          switchToFastPWM();
        } else {
          switchToSlowPWM();
        }
      }
      // Update trigger for the end point of the PWM cycle
      if (pendingPWM > 0) {
        TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_1, pendingPWM - 1);
        // Turn on output
      } else {
        TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_1, 0);
        // Leave output off
      }
    }
    // MSG((char *)"timer event comp2! \r\n");
  }
}

void switchToFastPWM(void) {
  fastPWM  = true;
  totalPWM = powerPWM + tempMeasureTicks + holdoffTicks;
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_2, totalPWM);

  // ~10Hz
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_0, powerPWM + holdoffTicks);
  // Set divider to 11

  uint32_t tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR2, 11);

  BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);
}

void switchToSlowPWM(void) {
  // 5Hz
  fastPWM  = false;
  totalPWM = powerPWM + tempMeasureTicks / 2 + holdoffTicks / 2;

  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_2, totalPWM);
  // Adjust ADC
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_0, powerPWM + (holdoffTicks / 2));

  // Set divider to 22

  uint32_t tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR2, 22);

  BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);
}
void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  PWMSafetyTimer = 10; // This is decremented in the handler for PWM so that the tip pwm is
                       // disabled if the PID task is not scheduled often enough.
  pendingPWM = pulse;
  fastPWM    = shouldUseFastModePWM;
}
extern osThreadId POWTaskHandle;

// void EXTI5_9_IRQHandler(void) {
//   // #if POW_PD
//   //   if (RESET != exti_interrupt_flag_get(EXTI_5)) {
//   //     exti_interrupt_flag_clear(EXTI_5);

//   //     if (POWTaskHandle != nullptr) {
//   //       BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//   //       xTaskNotifyFromISR(POWTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken);
//   //       /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
//   //       The macro used to do this is dependent on the port and may be called
//   //       portEND_SWITCHING_ISR. */
//   //       portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//   //     }
//   //   }
//   // #endif
// }

bool getFUS302IRQLow() {
  // Return true if the IRQ line is still held low
  return false;
  // return (RESET == gpio_input_bit_get(FUSB302_IRQ_GPIO_Port, FUSB302_IRQ_Pin));
}
