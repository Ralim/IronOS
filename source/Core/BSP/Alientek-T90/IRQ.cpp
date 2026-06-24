/*
 * IRQ.cpp
 *
 *  Functional interrupt handlers for the Alientek T90 (Nations N32L40x).
 *
 *  The timing spine mirrors Sequre but is retargeted to the N32 std-periph driver:
 *   - ADC injected-conversion-complete (ADC_IRQHandler) wakes the PID task. The injected
 *     group is hardware-triggered by the TIM4 schedule timer in the heater-off window, so
 *     each completion gives the PID a quiet-tip sample.
 *   - The TIM4 update IRQ (TIM4_IRQHandler) runs the PWM-safety countdown and writes the
 *     heater duty into TIM2_CH1 (PA0). If the PID stops scheduling, PWMSafetyTimer reaches
 *     zero and the heater is forced off.
 */

#include "IRQ.h"
#include "Pins.h"
#include "configuration.h"
#include "main.hpp"
#include "n32l40x.h"

#include "FreeRTOS.h"
#include "task.h"

extern "C" {

/*
 * Tip ADC injected group finished -> unblock the PID so it can run again.
 * The PID task blocks on ulTaskNotifyTake; this give is its heartbeat.
 */
void ADC_IRQHandler(void) {
  if (ADC_GetIntStatus(ADC, ADC_INT_JENDC) != RESET) {
    // STS is write-complement-to-clear; this clears the injected end-of-conversion flag.
    ADC_ClearIntPendingBit(ADC, ADC_INT_JENDC);
    // the tip was sampled with the heater blanked (TIM4 update forced it off, below). now the
    // quiet-tip sample is captured, restore the heater duty until the next schedule tick. this is
    // the measure-with-heater-off interleave (note: the off window here is only the injected
    // conversion time ~20us; the analog-front-end settling holdoff still needs scope tuning on
    // real hardware, see the review notes).
    if (PWMSafetyTimer == 0) {
      TIM_SetCmp1(TIM2, 0);
    } else {
      uint32_t ccr = ((uint32_t)pendingPWM * (uint32_t)TIM2->AR) / powerPWM;
      TIM_SetCmp1(TIM2, (uint16_t)ccr);
    }
    if (pidTaskNotification) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  }
}

/*
 * Heater-schedule timer update. Runs the safety countdown and BLANKS the heater so the tip
 * sample triggered by this same update (TIM4 TRGO -> ADC injected group) lands on a quiet tip.
 * The heater duty is re-applied in ADC_IRQHandler once the conversion completes. (TIM2 OC preload
 * is disabled in Setup.cpp so this CCR=0 takes effect immediately, not at the next carrier wrap.)
 */
void TIM4_IRQHandler(void) {
  if (TIM_GetIntStatus(TIM4, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(TIM4, TIM_INT_UPDATE);
    // Decrement the safety value so a stalled scheduler cannot leave the PWM latched on.
    if (PWMSafetyTimer) {
      PWMSafetyTimer--;
    }
    // Blank the heater for the upcoming injected sample (and keep it off if the safety timer
    // expired, i.e. the PID stopped scheduling).
    TIM_SetCmp1(TIM2, 0);
  }
}
} // extern "C"

// Declared in BSP_PD.h with C++ linkage; the T90 uses the CH224Q (no FUSB302 interrupt line),
// and the POW thread polls this unconditionally, so it must always be defined.
bool getFUS302IRQLow() { return false; }
