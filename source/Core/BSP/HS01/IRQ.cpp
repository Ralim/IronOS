/*
 * IRQ.cpp
 *
 * TMR0 is the 20 kHz heater PWM. TMR1 frames it: drive, heater-off holdoff, ADC sample, measure window.
 */

#include "IRQ.h"
#include "Pins.h"
#include "configuration.h"

volatile uint16_t PWMSafetyTimer = 0;
volatile uint8_t  pendingPWM     = 0;

enum class FramePhase { Drive, Holdoff, Measure };
static FramePhase framePhase = FramePhase::Drive;

static void frameTimerTick() {
  switch (framePhase) {
  case FramePhase::Drive:
    TMR0_PWMActDataWidth(0);
    R32_TMR1_CNT_END = FRAME_TICK_CYCLES * holdoffTicks;
    framePhase       = FramePhase::Holdoff;
    break;
  case FramePhase::Holdoff:
    sampleADC();
    R32_TMR1_CNT_END = FRAME_TICK_CYCLES * tempMeasureTicks;
    framePhase       = FramePhase::Measure;
    // unblock the PID controller thread
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED && pidTaskNotification) {
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    break;
  case FramePhase::Measure:
  default:
    // We decrement this safety value so that lockups in the scheduler will not cause the PWM to become locked in an
    // active driving state.
    if (PWMSafetyTimer) {
      PWMSafetyTimer--;
    }
    if (PWMSafetyTimer) {
      TMR0_PWMActDataWidth(((uint32_t)pendingPWM * HEATER_PWM_PERIOD) / powerPWM);
    } else {
      TMR0_PWMActDataWidth(0);
    }
    R32_TMR1_CNT_END = FRAME_TICK_CYCLES * powerPWM;
    framePhase       = FramePhase::Drive;
    break;
  }
}

// The vector table jumps here with a plain j, so the entry has to sit in RAM next to it
extern "C" __attribute__((interrupt("machine"), section(".highcode"))) void TMR1_IRQHandler(void) {
  TMR1_ClearITFlag(TMR0_3_IT_CYC_END);
  frameTimerTick();
}

void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  (void)shouldUseFastModePWM;
  PWMSafetyTimer = 4; // This is decremented in the handler for PWM so that the tip pwm is
                      // disabled if the PID task is not scheduled often enough.
  pendingPWM = pulse;
}

// The ball switch only makes brief contact, so catch every edge and hold it for MOVThread
static volatile bool movementLatched = false;

void armMovementInterrupt() { GPIOA_ITModeCfg(MOVEMENT_Pin, GPIOA_ReadPortPin(MOVEMENT_Pin) ? GPIO_ITMode_FallEdge : GPIO_ITMode_RiseEdge); }

extern "C" __attribute__((interrupt("machine"), section(".highcode"))) void GPIOA_IRQHandler(void) {
  GPIOA_ClearITFlagBit(MOVEMENT_Pin);
  movementLatched = true;
  armMovementInterrupt();
}

bool takeMovementLatch() {
  if (movementLatched) {
    movementLatched = false;
    return true;
  }
  return false;
}

// No FUSB302 support
bool getFUS302IRQLow() { return false; }
