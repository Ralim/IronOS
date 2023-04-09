/*
 * IRQ.c
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#include "IRQ.h"
#include "Pins.h"
#include "configuration.h"
#include "history.hpp"

extern "C" {
#include "bflb_platform.h"
#include "bl702_adc.h"
#include "bl702_glb.h"
#include "bl702_pwm.h"
#include "bl702_timer.h"
}
void start_PWM_output(void);

#define ADC_Filter_Smooth 4
history<uint16_t, ADC_Filter_Smooth> ADC_Vin;
history<uint16_t, ADC_Filter_Smooth> ADC_Temp;
history<uint16_t, ADC_Filter_Smooth> ADC_Tip;
volatile uint8_t                     ADCBurstCounter = 0;
void                                 adc_fifo_irq(void) {
  if (ADC_GetIntStatus(ADC_INT_FIFO_READY) == SET) {
    // Read out all entries in the fifo
    while (ADC_Get_FIFO_Count()) {
      ADCBurstCounter++;
      volatile uint32_t reading = ADC_Read_FIFO();
      // As per manual, 26 bit reading; lowest 16 are the ADC
      uint16_t sample = reading & 0xFFFF;
      uint8_t  source = (reading >> 21) & 0b11111;
      switch (source) {
      case TMP36_ADC_CHANNEL:
        ADC_Temp.update(sample);
        break;
      case TIP_TEMP_ADC_CHANNEL:
        ADC_Tip.update(sample);
        break;
      case VIN_ADC_CHANNEL:
        ADC_Vin.update(sample);
        break;

      default:
        break;
      }
    }

    if (ADCBurstCounter >= 8) {
      ADCBurstCounter = 0;
      start_PWM_output();

      // unblock the PID controller thread
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (pidTaskNotification) {
          vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
          portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
      }
    }
  }
  // Clear IRQ
  ADC_IntClr(ADC_INT_ALL);
}

static bool fastPWM = false;
static void switchToFastPWM(void);

volatile uint16_t PWMSafetyTimer    = 0;
volatile uint8_t  pendingPWM        = 0;
volatile bool     lastPeriodWasFast = false;

void start_PWM_output(void) {

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
      PWM_Channel_Enable(PWM_Channel);
    } else {
      // Leave output off
      PWM_Channel_Disable(PWM_Channel);
    }
  } else {
    PWM_Channel_Disable(PWM_Channel);
  }
  TIMER_Enable(TIMER_CH0);
}

// Timer 0 is used to co-ordinate the ADC and the output PWM
void timer0_comp0_callback(void) {
  TIMER_Disable(TIMER_CH0);
  ADC_Start();
}
void timer0_comp1_callback(void) { PWM_Channel_Disable(PWM_Channel); } // Trigged at end of output cycle; turn off the tip PWM

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

void GPIO_IRQHandler(void) {
  if (SET == GLB_Get_GPIO_IntStatus(FUSB302_IRQ_GLB_Pin)) {
    GLB_GPIO_IntClear(FUSB302_IRQ_GLB_Pin, SET);
#if POW_PD
    if (POWTaskHandle != nullptr) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      xTaskNotifyFromISR(POWTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken);
      /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE.
      The macro used to do this is dependent on the port and may be called
      portEND_SWITCHING_ISR. */
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
#endif

    /* timeout check */
    uint32_t timeOut = 32;

    do {
      timeOut--;
    } while ((SET == GLB_Get_GPIO_IntStatus(FUSB302_IRQ_GLB_Pin)) && timeOut);

    if (!timeOut) {
      // MSG("WARNING: Clear GPIO interrupt status fail.\r\n");
    }

    GLB_GPIO_IntClear(FUSB302_IRQ_GLB_Pin, RESET);
  }
}

bool getFUS302IRQLow() {
  // Return true if the IRQ line is still held low
  return !gpio_read(FUSB302_IRQ_Pin);
}

uint16_t getADCHandleTemp(uint8_t sample) { return ADC_Temp.average(); }

uint16_t getADCVin(uint8_t sample) { return ADC_Vin.average(); }

// Returns either average or instant value. When sample is set the samples from the injected ADC are copied to the filter and then the raw reading is returned
uint16_t getTipRawTemp(uint8_t sample) { return ADC_Tip.average() >> 1; }
