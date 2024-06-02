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

void read_adc_fifo(void) {
  // Read out all entries in the fifo
  uint8_t pending_readings = ADC_Get_FIFO_Count();

  // There _should_ always be 8 readings here. If there are not, it means that the adc didnt start when we wanted and timing slipped
  // So if there isn't 8 readings, we throw them out
  if (pending_readings != 8) {
    MSG((char *)"Discarding out of sync adc %d\r\n", pending_readings);
  } else {
    while (pending_readings) {
      pending_readings--;
      uint32_t        raw_reading = ADC_Read_FIFO();
      ADC_Result_Type parsed      = {0, 0, 0};
      ADC_Parse_Result(&raw_reading, 1, &parsed);
      // Rollover prevention
      if (parsed.value > ((1 << 14) - 1)) {
        parsed.value = ((1 << 14) - 1);
      }

      switch (parsed.posChan) {
      case TMP36_ADC_CHANNEL:
        ADC_Temp.update(parsed.value << 2);
        break;
      case TIP_TEMP_ADC_CHANNEL: {
        ADC_Tip.update(parsed.value << 2);
      } break;
      case VIN_ADC_CHANNEL:
        ADC_Vin.update(parsed.value << 2);
        break;
      default:
        break;
      }
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

volatile bool inFastPWMMode = false;

static void switchToFastPWM(void);
static void switchToSlowPWM(void);

volatile uint16_t PWMSafetyTimer          = 0;
volatile uint8_t  pendingPWM              = 0;
volatile bool     pendingNextPeriodIsFast = false;

void timer0_comp0_callback(void) {
  // Trigged at end of output cycle; turn off the tip PWM
  PWM_Channel_Disable(PWM_Channel);
  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_0);
}

// Timer 0 is used to co-ordinate the ADC and the output PWM
void timer0_comp1_callback(void) {
  ADC_FIFO_Clear();
  ADC_Start();
  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_1);
}
void timer0_comp2_callback(void) {
  // Triggered at end of timer cycle; re-start the tip driver
  ADC_Stop();
  TIMER_Disable(TIMER_CH0);
  // Read the ADC data _now_. So that if things have gone out of sync, we know about it
  read_adc_fifo();

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
      TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_0, pendingPWM - 1);
      // Turn on output
      PWM_Channel_Enable(PWM_Channel);
    } else {
      PWM_Channel_Disable(PWM_Channel);
    }
  } else {
    PWM_Channel_Disable(PWM_Channel);
  }
  TIMER_Enable(TIMER_CH0);
  TIMER_ClearIntStatus(TIMER_CH0, TIMER_COMP_ID_2);
}

void switchToFastPWM(void) {
  inFastPWMMode    = true;
  holdoffTicks     = 20;
  tempMeasureTicks = 10;
  totalPWM         = powerPWM + tempMeasureTicks + holdoffTicks;

  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_1, powerPWM + holdoffTicks);

  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_2, totalPWM);
  // Set divider to 10 ~= 10.5Hz
  uint32_t tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR2, 10);

  BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);
}

void switchToSlowPWM(void) {
  // 5Hz
  inFastPWMMode    = false;
  holdoffTicks     = 10;
  tempMeasureTicks = 5;
  totalPWM         = powerPWM + tempMeasureTicks + holdoffTicks;

  // Adjust ADC
  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_1, powerPWM + holdoffTicks);

  TIMER_SetCompValue(TIMER_CH0, TIMER_COMP_ID_2, totalPWM);
  // Set divider for ~ 5Hz

  uint32_t tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR2, 20);

  BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);
}

void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  PWMSafetyTimer = 10;
  // This is decremented in the handler for PWM so that the tip pwm is
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
