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
#include "hal_adc.h"
#include "hal_clock.h"
#include "hal_timer.h"
}

#define ADC_Filter_Smooth 4
history<uint16_t, ADC_Filter_Smooth> ADC_Vin;
history<uint16_t, ADC_Filter_Smooth> ADC_Temp;
history<uint16_t, ADC_Filter_Smooth> ADC_Tip;

void adc_fifo_irq(void) {

  if (ADC_GetIntStatus(ADC_INT_FIFO_READY) == SET) {
    ADC_IntClr(ADC_INT_FIFO_READY);

    // Read out all entries in the fifo
    const uint8_t cnt = ADC_Get_FIFO_Count();
    for (uint8_t i = 0; i < cnt; i++) {
      const uint32_t reading = ADC_Read_FIFO();
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
        MSG((char *)"ADC Invalid chan %d\r\n", source);
        break;
      }
    }
    MSG((char *)"ADC Reading %d %d %d\r\n", ADC_Temp.average(), ADC_Vin.average(), ADC_Tip.average());
    // Clear IRQ
  }
}

void start_adc_tip(void) {
  // Reconfigure the ADC to measure the tip temp
  // Single channel input mode
  // The ADC has a 32 sample FiFo; we set this up to fire and interrupt at 16 samples
  // Then using that IRQ to know that sampling is done and can be stored
  ADC_Start();
}

void stop_adc(void) {}

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
    // Used to start the ADC
    start_adc_tip();
  } else if (state == TIMER_EVENT_COMP1) {
    // MSG((char *)"timer event comp1! \r\n");
    // Used to turn tip off at set point in cycle
    PWM_Channel_Disable(PWM_Channel);
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
        PWM_Channel_Enable(PWM_Channel);
      } else {
        TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_1, 0);
        // Leave output off
        PWM_Channel_Disable(PWM_Channel);
      }
    }
    // unblock the PID controller thread
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      if (pidTaskNotification) {
        vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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
  // MSG((char *)"PWM Output %d, %d\r\n", pulse, (int)shouldUseFastModePWM);
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
  return false;
  // return (RESET == gpio_input_bit_get(FUSB302_IRQ_GPIO_Port, FUSB302_IRQ_Pin));
}
uint16_t rescaleADC(const uint16_t value) {
  // return value;
  uint32_t temp = value * 33;
  uint16_t res  = temp / 32;
  return res;
}
uint16_t getADCHandleTemp(uint8_t sample) { return ADC_Temp.average(); }

uint16_t getADCVin(uint8_t sample) { return ADC_Vin.average(); }

// Returns either average or instant value. When sample is set the samples from the injected ADC are copied to the filter and then the raw reading is returned
uint16_t getTipRawTemp(uint8_t sample) { return rescaleADC(ADC_Tip.average() >> 1); }
