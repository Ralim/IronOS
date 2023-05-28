// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Pins.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include <IRQ.h>

volatile uint16_t PWMSafetyTimer = 0;
volatile uint8_t  pendingPWM     = 0;

const uint16_t       powerPWM         = 255;
static const uint8_t holdoffTicks     = 30; // delay of 8 ish ms
static const uint8_t tempMeasureTicks = 30;

uint16_t totalPWM; // htim4.Init.Period, the full PWM cycle

static bool fastPWM;
static bool infastPWM;

void resetWatchdog() { HAL_IWDG_Refresh(&hiwdg); }
// Lookup table for the NTC
// We dont know exact specs, but it loooks to be roughly a 10K B=4000 NTC
// Stored as ADCReading,Temp in degC
static const uint16_t NTCHandleLookup[] = {
    // ADC Reading , Temp in C
    23931, 0,  //
    23210, 2,  //
    22466, 4,  //
    21703, 6,  //
    20924, 8,  //
    20135, 10, //
    19338, 12, //
    18538, 14, //
    17738, 16, //
    16943, 18, //
    16156, 20, //
    15381, 22, //
    14621, 24, //
    13878, 26, //
    13155, 28, //
    12455, 30, //
    11778, 32, //
    11126, 34, //
    10501, 36, //
    9902,  38, //
    9330,  40, //
    8786,  42, //
    8269,  44, //
};

uint16_t getHandleTemperature(uint8_t sample) {
  int32_t result = getADCHandleTemp(sample);
  // S60 uses 10k NTC resistor
  // For now not doing interpolation
  for (uint32_t i = 0; i < (sizeof(NTCHandleLookup) / (2 * sizeof(uint16_t))); i++) {
    if (result > NTCHandleLookup[(i * 2) + 0]) {
      return NTCHandleLookup[(i * 2) + 1] * 10;
    }
  }
  return 45 * 10;
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
  // ADC maximum is 32767 == 3.3V at input == 28.05V at VIN
  // Therefore we can divide down from there
  // Multiplying ADC max by 4 for additional calibration options,
  // ideal term is 467
  uint32_t res = getADCVin(sample);
  res *= 4;
  res /= divisor;
  return res;
}

static void switchToFastPWM(void) {
  // 20Hz
  infastPWM            = true;
  totalPWM             = powerPWM + tempMeasureTicks + holdoffTicks;
  htim4.Instance->ARR  = totalPWM;
  htim4.Instance->CCR1 = powerPWM + holdoffTicks;
  htim4.Instance->PSC  = 1500;
}

static void switchToSlowPWM(void) {
  // 10Hz
  infastPWM            = false;
  totalPWM             = powerPWM + tempMeasureTicks / 2 + holdoffTicks / 2;
  htim4.Instance->ARR  = totalPWM;
  htim4.Instance->CCR1 = powerPWM + holdoffTicks / 2;
  htim4.Instance->PSC  = 1500 * 2;
}

void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  PWMSafetyTimer = 20; // This is decremented in the handler for PWM so that the tip pwm is
                       // disabled if the PID task is not scheduled often enough.
  fastPWM    = shouldUseFastModePWM;
  pendingPWM = pulse;
}
// These are called by the HAL after the corresponding events from the system
// timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Period has elapsed
  if (htim->Instance == TIM4) {
    // we want to turn on the output again
    PWMSafetyTimer--;
    // We decrement this safety value so that lockups in the
    // scheduler will not cause the PWM to become locked in an
    // active driving state.
    // While we could assume this could never happen, its a small price for
    // increased safety
    if (PWMSafetyTimer) {
      htim4.Instance->CCR3 = pendingPWM;
    } else {
      htim4.Instance->CCR3 = 0;
    }
    // if (fastPWM != infastPWM) {
    //   if (fastPWM) {
    //     switchToFastPWM();
    //   } else {
    switchToSlowPWM();
    // }
    // }
  } else if (htim->Instance == TIM1) {
    // STM uses this for internal functions as a counter for timeouts
    HAL_IncTick();
  }
}

void unstick_I2C() {
#ifdef SCL_Pin
  GPIO_InitTypeDef GPIO_InitStruct;
  int              timeout     = 100;
  int              timeout_cnt = 0;

  // 1. Clear PE bit.
  hi2c1.Instance->CR1 &= ~(0x0001);
  /**I2C1 GPIO Configuration
   PB6     ------> I2C1_SCL
   PB7     ------> I2C1_SDA
   */
  //  2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  GPIO_InitStruct.Pin = SCL_Pin;
  HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = SDA_Pin;
  HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);

  while (GPIO_PIN_SET != HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin)) {
    // Move clock to release I2C
    HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET);
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);

    timeout_cnt++;
    if (timeout_cnt > timeout)
      return;
  }

  // 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  GPIO_InitStruct.Pin = SCL_Pin;
  HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SDA_Pin;
  HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);

  HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);

  // 13. Set SWRST bit in I2Cx_CR1 register.
  hi2c1.Instance->CR1 |= 0x8000;

  asm("nop");

  // 14. Clear SWRST bit in I2Cx_CR1 register.
  hi2c1.Instance->CR1 &= ~0x8000;

  asm("nop");

  // 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register
  hi2c1.Instance->CR1 |= 0x0001;

  // Call initialization function.
  HAL_I2C_Init(&hi2c1);
#endif
}

uint8_t getButtonA() { return HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin) == GPIO_PIN_RESET ? 1 : 0; }
uint8_t getButtonB() { return HAL_GPIO_ReadPin(KEY_B_GPIO_Port, KEY_B_Pin) == GPIO_PIN_RESET ? 1 : 0; }

void BSPInit(void) { switchToFastPWM(); }

void reboot() { NVIC_SystemReset(); }

void delay_ms(uint16_t count) { HAL_Delay(count); }

bool isTipDisconnected() {

  uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
  uint32_t tipTemp              = TipThermoModel::getTipInC();
  return tipTemp > tipDisconnectedThres;
}

void     setStatusLED(const enum StatusLED state) {}
uint8_t  preStartChecks() { return 1; }
uint64_t getDeviceID() {
  //
  return HAL_GetUIDw0() | ((uint64_t)HAL_GetUIDw1() << 32);
}

uint8_t getTipResistanceX10() { return TIP_RESISTANCE; }

uint8_t preStartChecksDone() { return 1; }

uint8_t getTipThermalMass() { return TIP_THERMAL_INERTIA; }

void setBuzzer(bool on) {}