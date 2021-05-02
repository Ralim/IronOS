// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Model_Config.h"
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
static const uint8_t holdoffTicks     = 14; // delay of 8 ms
static const uint8_t tempMeasureTicks = 14;

uint16_t totalPWM; // htim2.Init.Period, the full PWM cycle

static bool fastPWM;

// 2 second filter (ADC is PID_TIM_HZ Hz)
history<uint16_t, PID_TIM_HZ> rawTempFilter = {{0}, 0, 0};
void                          resetWatchdog() { HAL_IWDG_Refresh(&hiwdg); }
#ifdef TEMP_NTC
// Lookup table for the NTC
// Stored as ADCReading,Temp in degC
static const uint16_t NTCHandleLookup[] = {
    // ADC Reading , Temp in C
    29189, 0,  //
    29014, 1,  //
    28832, 2,  //
    28644, 3,  //
    28450, 4,  //
    28249, 5,  //
    28042, 6,  //
    27828, 7,  //
    27607, 8,  //
    27380, 9,  //
    27146, 10, //
    26906, 11, //
    26660, 12, //
    26407, 13, //
    26147, 14, //
    25882, 15, //
    25610, 16, //
    25332, 17, //
    25049, 18, //
    24759, 19, //
    24465, 20, //
    24164, 21, //
    23859, 22, //
    23549, 23, //
    23234, 24, //
    22915, 25, //
    22591, 26, //
    22264, 27, //
    21933, 28, //
    21599, 29, //
    21261, 30, //
    20921, 31, //
    20579, 32, //
    20234, 33, //
    19888, 34, //
    19541, 35, //
    19192, 36, //
    18843, 37, //
    18493, 38, //
    18143, 39, //
    17793, 40, //
    17444, 41, //
    17096, 42, //
    16750, 43, //
    16404, 44, //
    16061, 45, //
               //		15719, 46, //
               //		15380, 47, //
               //		15044, 48, //
               //		14710, 49, //
               //		14380, 50, //
               //		14053, 51, //
               //		13729, 52, //
               //		13410, 53, //
               //		13094, 54, //
               //		12782, 55, //
               //		12475, 56, //
               //		12172, 57, //
               //		11874, 58, //
               //		11580, 59, //
               //		11292, 60, //
};
#endif

uint16_t getHandleTemperature() {
#ifdef TEMP_NTC
  // TS80P uses 100k NTC resistors instead
  // NTCG104EF104FT1X from TDK
  // For now not doing interpolation
  int32_t result = getADC(0);
  for (uint32_t i = 0; i < (sizeof(NTCHandleLookup) / (2 * sizeof(uint16_t))); i++) {
    if (result > NTCHandleLookup[(i * 2) + 0]) {
      return NTCHandleLookup[(i * 2) + 1] * 10;
    }
  }
  return 45 * 10;
#endif
#ifdef TEMP_TMP36
  // We return the current handle temperature in X10 C
  // TMP36 in handle, 0.5V offset and then 10mV per deg C (0.75V @ 25C for
  // example) STM32 = 4096 count @ 3.3V input -> But We oversample by 32/(2^2) =
  // 8 times oversampling Therefore 32768 is the 3.3V input, so 0.1007080078125
  // mV per count So we need to subtract an offset of 0.5V to center on 0C
  // (4964.8 counts)
  //
  int32_t result = getADC(0);
  result -= 4965; // remove 0.5V offset
  // 10mV per C
  // 99.29 counts per Deg C above 0C. Tends to read a tad over across all of my sample units
  result *= 100;
  result /= 994;
  return result;
#endif
}

uint16_t getTipInstantTemperature() {
  uint16_t sum = 0; // 12 bit readings * 8 -> 15 bits
  uint16_t readings[8];
  // Looking to reject the highest outlier readings.
  // As on some hardware these samples can run into the op-amp recovery time
  // Once this time is up the signal stabilises quickly, so no need to reject minimums
  readings[0] = hadc1.Instance->JDR1;
  readings[1] = hadc1.Instance->JDR2;
  readings[2] = hadc1.Instance->JDR3;
  readings[3] = hadc1.Instance->JDR4;
  readings[4] = hadc2.Instance->JDR1;
  readings[5] = hadc2.Instance->JDR2;
  readings[6] = hadc2.Instance->JDR3;
  readings[7] = hadc2.Instance->JDR4;

  for (int i = 0; i < 8; i++) {
    sum += readings[i];
  }
  return sum; // 8x over sample
}

uint16_t getTipRawTemp(uint8_t refresh) {
  if (refresh) {
    uint16_t lastSample = getTipInstantTemperature();
    rawTempFilter.update(lastSample);
    return lastSample;
  } else {
    return rawTempFilter.average();
  }
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
// ADC maximum is 32767 == 3.3V at input == 28.05V at VIN
// Therefore we can divide down from there
// Multiplying ADC max by 4 for additional calibration options,
// ideal term is 467
#ifdef MODEL_TS100
#define BATTFILTERDEPTH 32
#else
#define BATTFILTERDEPTH 8

#endif
  static uint8_t  preFillneeded = 10;
  static uint32_t samples[BATTFILTERDEPTH];
  static uint8_t  index = 0;
  if (preFillneeded) {
    for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
      samples[i] = getADC(1);
    preFillneeded--;
  }
  if (sample) {
    samples[index] = getADC(1);
    index          = (index + 1) % BATTFILTERDEPTH;
  }
  uint32_t sum = 0;

  for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
    sum += samples[i];

  sum /= BATTFILTERDEPTH;
  if (divisor == 0) {
    divisor = 1;
  }
  return sum * 4 / divisor;
}

void setTipPWM(uint8_t pulse) {
  PWMSafetyTimer = 10; // This is decremented in the handler for PWM so that the tip pwm is
                       // disabled if the PID task is not scheduled often enough.

  pendingPWM = pulse;
}

static void switchToFastPWM(void) {
  fastPWM             = true;
  totalPWM            = powerPWM + tempMeasureTicks * 2 + holdoffTicks;
  htim2.Instance->ARR = totalPWM;
  // ~3.5 Hz rate
  htim2.Instance->CCR1 = powerPWM + holdoffTicks * 2;
  // 2 MHz timer clock/2000 = 1 kHz tick rate
  htim2.Instance->PSC = 2000;
}

static void switchToSlowPWM(void) {
  fastPWM             = false;
  totalPWM            = powerPWM + tempMeasureTicks + holdoffTicks;
  htim2.Instance->ARR = totalPWM;
  // ~1.84 Hz rate
  htim2.Instance->CCR1 = powerPWM + holdoffTicks;
  // 2 MHz timer clock/4000 = 500 Hz tick rate
  htim2.Instance->PSC = 4000;
}

bool tryBetterPWM(uint8_t pwm) {
  if (fastPWM && pwm == powerPWM) {
    // maximum power for fast PWM reached, need to go slower to get more
    switchToSlowPWM();
    return true;
  } else if (!fastPWM && pwm < 230) {
    // 254 in fast PWM mode gives the same power as 239 in slow
    // allow for some reasonable hysteresis by switching only when it goes
    // below 230 (equivalent to 245 in fast mode)
    switchToFastPWM();
    return true;
  }
  return false;
}

// These are called by the HAL after the corresponding events from the system
// timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Period has elapsed
  if (htim->Instance == TIM2) {
    // we want to turn on the output again
    PWMSafetyTimer--;
    // We decrement this safety value so that lockups in the
    // scheduler will not cause the PWM to become locked in an
    // active driving state.
    // While we could assume this could never happen, its a small price for
    // increased safety
    htim2.Instance->CCR4 = pendingPWM;
    if (htim2.Instance->CCR4 && PWMSafetyTimer) {
      HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    } else {
      HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    }
  } else if (htim->Instance == TIM1) {
    // STM uses this for internal functions as a counter for timeouts
    HAL_IncTick();
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  // This was a when the PWM for the output has timed out
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
  }
}
void unstick_I2C() {
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

void setStatusLED(const enum StatusLED state) {}
