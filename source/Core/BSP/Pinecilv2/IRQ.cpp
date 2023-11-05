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

#define ADC_Filter_Smooth 4 /* This basically smooths over one PWM cycle / set of readings */
history<uint16_t, ADC_Filter_Smooth> ADC_Vin;
history<uint16_t, ADC_Filter_Smooth> ADC_Temp;
history<uint16_t, ADC_Filter_Smooth> ADC_Tip;

// IRQ is called at the end of the 8 set readings, pop these from the FIFO and send to filters
void adc_fifo_irq(void) {
  if (ADC_GetIntStatus(ADC_INT_FIFO_READY) == SET) {
    // Read out all entries in the fifo
    while (ADC_Get_FIFO_Count()) {
      uint32_t reading = ADC_Read_FIFO();
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
    // unblock the PID controller thread
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      if (pidTaskNotification) {
        vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
    }
  }
  // Clear IRQ
  ADC_IntClr(ADC_INT_ALL);
}

volatile bool inFastPWMMode = false;

static void switchToFastPWM(void);
static void switchToSlowPWM(void);

volatile uint16_t PWMSafetyTimer          = 0;
volatile uint8_t  pendingPWM              = 0;
volatile bool     pendingNextPeriodIsFast = false;

void start_PWM_output(void) {

  if (PWMSafetyTimer) {
    PWMSafetyTimer--;
    if (pendingNextPeriodIsFast != inFastPWMMode) {
      if (pendingNextPeriodIsFast) {
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
    switchToFastPWM();
  }
}

// Timer 0 is used to co-ordinate the ADC and the output PWM
void timer0_comp0_callback(void) {
  if (PWM_Channel_Is_Enabled(PWM_Channel)) {
    // So there appears to be a bug _somewhere_ where sometimes the comparator doesn't fire
    // Its not re-occurring with specific values, so suspect its a weird bug
    // For now, we just skip the cycle and throw away the ADC readings. Its a waste but
    // It stops stupid glitches in readings, i'd take slight instability from the time jump
    // Over the readings we get that are borked as the header is left on
    // <Ralim 2023/10/14>
    PWM_Channel_Disable(PWM_Channel);
    // MSG("ALERT PWM Glitch\r\n");
    // Triger the PID now instead
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      if (pidTaskNotification) {
        vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
    }
  } else {
    ADC_Start();
  }
  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_0);
}
void timer0_comp1_callback(void) {
  // Trigged at end of output cycle; turn off the tip PWM
  PWM_Channel_Disable(PWM_Channel);
  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_1);
}

void timer0_comp2_callback(void) {
  // Triggered at end of timer cycle; re-start the tip driver
  start_PWM_output();
  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_2);
}
void switchToFastPWM(void) {
  inFastPWMMode    = true;
  holdoffTicks     = 10;
  tempMeasureTicks = 10;
  totalPWM         = powerPWM + tempMeasureTicks + holdoffTicks;
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_2, totalPWM);

  // ~10Hz
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_0, powerPWM + holdoffTicks);

  // Set divider to 10 ~= 10.5Hz
  uint32_t tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR2, 10);

  BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);
}

void switchToSlowPWM(void) {
  // 5Hz
  inFastPWMMode    = false;
  holdoffTicks     = 5;
  tempMeasureTicks = 5;
  totalPWM         = powerPWM + tempMeasureTicks + holdoffTicks;

  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_2, totalPWM);
  // Adjust ADC
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_0, powerPWM + holdoffTicks);

  // Set divider for ~ 5Hz

  uint32_t tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR2, 20);

  BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);
}
void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  PWMSafetyTimer = 10; // This is decremented in the handler for PWM so that the tip pwm is
                       // disabled if the PID task is not scheduled often enough.
  pendingPWM              = pulse;
  pendingNextPeriodIsFast = shouldUseFastModePWM;
}
extern osThreadId POWTaskHandle;

void GPIO_IRQHandler(void) {
  if (SET == GLB_Get_GPIO_IntStatus(FUSB302_IRQ_GLB_Pin)) {
    GLB_GPIO_IntClear(FUSB302_IRQ_GLB_Pin, SET);
#ifdef POW_PD
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

// Returns the current raw tip reading after any cleanup filtering
// For Pinecil V2 we dont do any rolling filtering other than just averaging all 4 readings in the adc snapshot
uint16_t getTipRawTemp(uint8_t sample) { return ADC_Tip.average() >> 1; }
