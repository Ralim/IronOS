// BSP mapping functions

#include "BSP.h"
#include "BootLogo.h"
#include "I2C_Wrapper.hpp"
#include "Pins.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "USBPD.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include <IRQ.h>

volatile uint16_t PWMSafetyTimer = 0;
volatile uint8_t  pendingPWM     = 0;

const uint16_t       powerPWM         = 255;
static const uint8_t holdoffTicks     = 14; // delay of 8 ms
static const uint8_t tempMeasureTicks = 14;

uint16_t totalPWM; // htimADC.Init.Period, the full PWM cycle

static bool fastPWM;
static bool infastPWM;

void resetWatchdog() { HAL_IWDG_Refresh(&hiwdg); }
#ifdef TEMP_NTC
// Lookup table for the NTC
// Stored as ADCReading,Temp in degC
static const uint16_t NTCHandleLookup[] = {
    // ADC Reading , Temp in C
    29189, 0,  //
    28832, 2,  //
    28450, 4,  //
    28042, 6,  //
    27607, 8,  //
    27146, 10, //
    26660, 12, //
    26147, 14, //
    25610, 16, //
    25049, 18, //
    24465, 20, //
    23859, 22, //
    23234, 24, //
    22591, 26, //
    21933, 28, //
    21261, 30, //
    20579, 32, //
    19888, 34, //
    19192, 36, //
    18493, 38, //
    17793, 40, //
    17096, 42, //
    16404, 44, //
    16061, 45, //
};
#endif

uint16_t getHandleTemperature(uint8_t sample) {
  int32_t result = getADCHandleTemp(sample);
#ifdef TEMP_NTC
  // TS80P uses 100k NTC resistors instead
  // NTCG104EF104FT1X from TDK
  // For now not doing interpolation
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
  result -= 4965; // remove 0.5V offset
  // 10mV per C
  // 99.29 counts per Deg C above 0C. Tends to read a tad over across all of my sample units
  result *= 100;
  result /= 994;
  return result;
#endif
  return 0;
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
  // 10Hz
  infastPWM              = true;
  totalPWM               = powerPWM + tempMeasureTicks + holdoffTicks;
  htimADC.Instance->ARR  = totalPWM;
  htimADC.Instance->CCR1 = powerPWM + holdoffTicks;
  htimADC.Instance->PSC  = 2690;
}

static void switchToSlowPWM(void) {
  // 5Hz
  infastPWM              = false;
  totalPWM               = powerPWM + tempMeasureTicks / 2 + holdoffTicks / 2;
  htimADC.Instance->ARR  = totalPWM;
  htimADC.Instance->CCR1 = powerPWM + holdoffTicks / 2;
  htimADC.Instance->PSC  = 2690 * 2;
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
  if (htim->Instance == ADC_CONTROL_TIMER) {
    // we want to turn on the output again
    PWMSafetyTimer--;
// We decrement this safety value so that lockups in the
// scheduler will not cause the PWM to become locked in an
// active driving state.
// While we could assume this could never happen, its a small price for
// increased safety
#ifdef TIP_HAS_DIRECT_PWM
    htimADC.Instance->CCR4 = powerPWM;
    if (pendingPWM && PWMSafetyTimer) {
      htimTip.Instance->CCR1 = pendingPWM;
      HAL_TIM_PWM_Start(&htimTip, PWM_Out_CHANNEL);
    } else {
      HAL_TIM_PWM_Stop(&htimTip, PWM_Out_CHANNEL);
    }
#else
    htimADC.Instance->CCR4 = pendingPWM;
    if (htimADC.Instance->CCR4 && PWMSafetyTimer) {
      HAL_TIM_PWM_Start(&htimTip, PWM_Out_CHANNEL);
    } else {
      HAL_TIM_PWM_Stop(&htimTip, PWM_Out_CHANNEL);
    }
#endif
    if (fastPWM != infastPWM) {
      if (fastPWM) {
        switchToFastPWM();
      } else {
        switchToSlowPWM();
      }
    }

  } else if (htim->Instance == TIM1) {
    // STM uses this for internal functions as a counter for timeouts
    HAL_IncTick();
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  // This was a when the PWM for the output has timed out
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
    HAL_TIM_PWM_Stop(&htimTip, PWM_Out_CHANNEL);
  }
}
void unstick_I2C() {
#ifndef I2C_SOFT_BUS_1
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
    if (timeout_cnt > timeout) {
      return;
    }
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

uint8_t       lastTipResistance        = 0; // default to unknown
const uint8_t numTipResistanceReadings = 3;
uint32_t      tipResistanceReadings[3] = {0, 0, 0};
uint8_t       tipResistanceReadingSlot = 0;
bool          isTipDisconnected() {

  uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
  uint32_t tipTemp              = TipThermoModel::getTipInC();
  return tipTemp > tipDisconnectedThres;
}

void setStatusLED(const enum StatusLED state) {}
void setBuzzer(bool on) {}
#ifdef TIP_RESISTANCE_SENSE_Pin
// We want to calculate lastTipResistance
// If tip is connected, and the tip is cold and the tip is not being heated
// We can use the GPIO to inject a small current into the tip and measure this
// The gpio is 100k -> diode -> tip -> gnd
// Source is 3.3V-0.5V
// Which is around 0.028mA this will induce:
// 6 ohm tip -> 3.24mV (Real world ~= 3320)
// 8 ohm tip -> 4.32mV (Real world ~= 4500)
// Which is definitely measureable
// Taking shortcuts here as we know we only really have to pick apart 6 and 8 ohm tips
// These are reported as 60 and 75 respectively
void performTipResistanceSampleReading() {
  // 0 = read then turn on pullup, 1 = read then turn off pullup, 2 = read again
  tipResistanceReadings[tipResistanceReadingSlot] = TipThermoModel::convertTipRawADCTouV(getTipRawTemp(1));

  HAL_GPIO_WritePin(TIP_RESISTANCE_SENSE_GPIO_Port, TIP_RESISTANCE_SENSE_Pin, (tipResistanceReadingSlot == 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);

  tipResistanceReadingSlot++;
}
bool tipShorted = false;
void FinishMeasureTipResistance() {

  // Otherwise we now have the 4 samples;
  //  _^_ order, 2 delta's, combine these

  int32_t calculatedSkew = tipResistanceReadings[0] - tipResistanceReadings[2]; // If positive tip is cooling
  calculatedSkew /= 2;                                                          // divide by two to get offset per time constant

  int32_t reading = (((tipResistanceReadings[1] - tipResistanceReadings[0]) + calculatedSkew) // jump 1 - skew
                     +                                                                        // +
                     ((tipResistanceReadings[1] - tipResistanceReadings[2]) + calculatedSkew) // jump 2 - skew
                     )                                                                        //
                    / 2;                                                                      // Take average
  // // As we are only detecting two resistances; we can split the difference for now
  uint8_t newRes = 0;
  if (reading > 1200) {
    // return; // Change nothing as probably disconnected tip
    tipResistanceReadingSlot = lastTipResistance = 0;
    return;
  } else if (reading < 200) {
    tipShorted = true;
  } else if (reading < 520) {
    newRes = 40;
  } else if (reading < 800) {
    newRes = 62;
  } else {
    newRes = 80;
  }
  lastTipResistance = newRes;
}
volatile bool       tipMeasurementOccuring = true;
volatile TickType_t nextTipMeasurement     = 100;

void performTipMeasurementStep() {

  // Wait 200ms for settle time
  if (xTaskGetTickCount() < (nextTipMeasurement)) {
    return;
  }
  nextTipMeasurement = xTaskGetTickCount() + (TICKS_100MS * 5);
  if (tipResistanceReadingSlot < numTipResistanceReadings) {
    performTipResistanceSampleReading();
    return;
  }

  // We are sensing the resistance
  FinishMeasureTipResistance();

  tipMeasurementOccuring = false;
}
#endif
uint8_t preStartChecks() {
#ifdef TIP_RESISTANCE_SENSE_Pin
  performTipMeasurementStep();
  if (preStartChecksDone() != 1) {
    return 0;
  }
#endif
#ifdef HAS_SPLIT_POWER_PATH

  // We want to enable the power path that has the highest voltage
  // Nominally one will be ~=0 and one will be high. Unless you jamb both in, then both _may_ be high, or device may be dead
  {
    uint16_t dc = getRawDCVin();
    uint16_t pd = getRawPDVin();
    if (dc > pd) {
      HAL_GPIO_WritePin(DC_SELECT_GPIO_Port, DC_SELECT_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(PD_SELECT_GPIO_Port, PD_SELECT_Pin, GPIO_PIN_RESET);
    } else {
      HAL_GPIO_WritePin(PD_SELECT_GPIO_Port, PD_SELECT_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(DC_SELECT_GPIO_Port, DC_SELECT_Pin, GPIO_PIN_RESET);
    }
  }

#endif

  return 1;
}
uint64_t getDeviceID() {
  //
  return HAL_GetUIDw0() | ((uint64_t)HAL_GetUIDw1() << 32);
}

uint8_t preStartChecksDone() {
#ifdef TIP_RESISTANCE_SENSE_Pin
  return (lastTipResistance == 0 || tipResistanceReadingSlot < numTipResistanceReadings || tipMeasurementOccuring || tipShorted) ? 0 : 1;
#else
  return 1;
#endif
}

uint8_t getTipResistanceX10() {
#ifdef TIP_RESISTANCE_SENSE_Pin
  // Return tip resistance in x10 ohms
  // We can measure this using the op-amp
  uint8_t user_selected_tip = getUserSelectedTipResistance();
  if (user_selected_tip == 0) {
    return lastTipResistance; // Auto mode
  }
  return user_selected_tip;

#else
  uint8_t user_selected_tip = getUserSelectedTipResistance();
  if (user_selected_tip == 0) {
    return TIP_RESISTANCE; // Auto mode
  }
  return user_selected_tip;
#endif
}
#ifdef TIP_RESISTANCE_SENSE_Pin
bool isTipShorted() { return tipShorted; }
#else
bool isTipShorted() { return false; }
#endif
uint16_t getTipThermalMass() {
#ifdef TIP_RESISTANCE_SENSE_Pin
  if (lastTipResistance >= 80) {
    return TIP_THERMAL_MASS;
  }
  return 45;
#else
  return TIP_THERMAL_MASS;
#endif
}
uint16_t getTipInertia() {
#ifdef TIP_RESISTANCE_SENSE_Pin
  if (lastTipResistance >= 80) {
    return TIP_THERMAL_MASS;
  }
  return 10;
#else
  return TIP_THERMAL_MASS;
#endif
}

void showBootLogo(void) { BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR); }
