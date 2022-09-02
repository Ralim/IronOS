// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Pins.h"
#include "Setup.h"
#include "TipThermoModel.h"
#include "Utils.h"
#include "WS2812.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include <IRQ.h>

WS2812<GPIOA_BASE, WS2812_Pin, 1> ws2812;
volatile uint16_t                 PWMSafetyTimer            = 0;
volatile uint8_t                  pendingPWM                = 0;
uint16_t                          totalPWM                  = 255;
const uint16_t                    powerPWM                  = 255;
uint16_t                          tipSenseResistancex10Ohms = 0;
volatile bool                     tipMeasurementOccuring    = false;
history<uint16_t, PID_TIM_HZ>     rawTempFilter             = {{0}, 0, 0};

void resetWatchdog() { HAL_IWDG_Refresh(&hiwdg); }

#ifdef TEMP_NTC
// Lookup table for the NTC
// Stored as ADCReading,Temp in degC
static const int32_t NTCHandleLookup[] = {
    // ADC Reading , Temp in Cx10
    808,   1600, //
    832,   1590, //
    848,   1580, //
    872,   1570, //
    888,   1560, //
    912,   1550, //
    936,   1540, //
    960,   1530, //
    984,   1520, //
    1008,  1510, //
    1032,  1500, //
    1056,  1490, //
    1080,  1480, //
    1112,  1470, //
    1136,  1460, //
    1168,  1450, //
    1200,  1440, //
    1224,  1430, //
    1256,  1420, //
    1288,  1410, //
    1328,  1400, //
    1360,  1390, //
    1392,  1380, //
    1432,  1370, //
    1464,  1360, //
    1504,  1350, //
    1544,  1340, //
    1584,  1330, //
    1632,  1320, //
    1672,  1310, //
    1720,  1300, //
    1760,  1290, //
    1808,  1280, //
    1856,  1270, //
    1912,  1260, //
    1960,  1250, //
    2016,  1240, //
    2072,  1230, //
    2128,  1220, //
    2184,  1210, //
    2248,  1200, //
    2304,  1190, //
    2368,  1180, //
    2440,  1170, //
    2504,  1160, //
    2576,  1150, //
    2648,  1140, //
    2720,  1130, //
    2792,  1120, //
    2872,  1110, //
    2952,  1100, //
    3040,  1090, //
    3128,  1080, //
    3216,  1070, //
    3304,  1060, //
    3400,  1050, //
    3496,  1040, //
    3592,  1030, //
    3696,  1020, //
    3800,  1010, //
    3912,  1000, //
    4024,  990,  //
    4136,  980,  //
    4256,  970,  //
    4376,  960,  //
    4504,  950,  //
    4632,  940,  //
    4768,  930,  //
    4904,  920,  //
    5048,  910,  //
    5192,  900,  //
    5336,  890,  //
    5488,  880,  //
    5648,  870,  //
    5808,  860,  //
    5976,  850,  //
    6144,  840,  //
    6320,  830,  //
    6504,  820,  //
    6688,  810,  //
    6872,  800,  //
    7072,  790,  //
    7264,  780,  //
    7472,  770,  //
    7680,  760,  //
    7896,  750,  //
    8112,  740,  //
    8336,  730,  //
    8568,  720,  //
    8800,  710,  //
    9040,  700,  //
    9288,  690,  //
    9536,  680,  //
    9792,  670,  //
    10056, 660,  //
    10320, 650,  //
    10592, 640,  //
    10872, 630,  //
    11152, 620,  //
    11440, 610,  //
    11728, 600,  //
    12024, 590,  //
    12320, 580,  //
    12632, 570,  //
    12936, 560,  //
    13248, 550,  //
    13568, 540,  //
    13888, 530,  //
    14216, 520,  //
    14544, 510,  //
    14880, 500,  //
    15216, 490,  //
    15552, 480,  //
    15888, 470,  //
    16232, 460,  //
    16576, 450,  //
    16920, 440,  //
    17272, 430,  //
    17616, 420,  //
    17968, 410,  //
    18320, 400,  //
    18664, 390,  //
    19016, 380,  //
    19368, 370,  //
    19712, 360,  //
    20064, 350,  //
    20408, 340,  //
    20752, 330,  //
    21088, 320,  //
    21432, 310,  //
    21768, 300,  //
    22096, 290,  //
    22424, 280,  //
    22752, 270,  //
    23072, 260,  //
    23392, 250,  //
    23704, 240,  //
    24008, 230,  //
    24312, 220,  //
    24608, 210,  //
    24904, 200,  //
    25192, 190,  //
    25472, 180,  //
    25744, 170,  //
    26016, 160,  //
    26280, 150,  //
    26536, 140,  //
    26784, 130,  //
    27024, 120,  //
    27264, 110,  //
    27496, 100,  //
    27720, 90,   //
    27936, 80,   //
    28144, 70,   //
    28352, 60,   //
    28544, 50,   //
    28736, 40,   //
    28920, 30,   //
    29104, 20,   //
    29272, 10,   //
};
const int NTCHandleLookupItems = sizeof(NTCHandleLookup) / (2 * sizeof(uint16_t));
#endif

// These are called by the HAL after the corresponding events from the system
// timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // Period has elapsed
  if (htim->Instance == TIM4) {
    // STM uses this for internal functions as a counter for timeouts
    HAL_IncTick();
  }
}
uint16_t getHandleTemperature(uint8_t sample) {
  int32_t result = getADC(0);
  return Utils::InterpolateLookupTable(NTCHandleLookup, NTCHandleLookupItems, result);
}

uint16_t getTipInstantTemperature() { return getADC(2); }

uint16_t getTipRawTemp(uint8_t refresh) {
  if (refresh && (tipMeasurementOccuring == false)) {

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
void setTipPWM(const uint8_t pulse, const bool shouldUseFastModePWM) {
  // We can just set the timer directly
  if (htim3.Instance->PSC > 20) {
    htim3.Instance->CCR1 = 0;
  } else {
    htim3.Instance->CCR1 = pulse;
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

void BSPInit(void) { ws2812.init(); }

void reboot() { NVIC_SystemReset(); }

void delay_ms(uint16_t count) { HAL_Delay(count); }

void setPlatePullup(bool pullingUp) {
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pin   = PLATE_SENSOR_PULLUP_Pin;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  if (pullingUp) {
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_WritePin(PLATE_SENSOR_PULLUP_GPIO_Port, PLATE_SENSOR_PULLUP_Pin, GPIO_PIN_SET);
  } else {
    // Hi-z
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_WritePin(PLATE_SENSOR_PULLUP_GPIO_Port, PLATE_SENSOR_PULLUP_Pin, GPIO_PIN_RESET);
  }
  HAL_GPIO_Init(PLATE_SENSOR_PULLUP_GPIO_Port, &GPIO_InitStruct);
}

void performTipMeasurementStep(bool start) {
  static uint16_t   adcReadingPD1Set = 0;
  static TickType_t lastMeas         = 0;
  // Inter state that performs the steps to measure the resistor on the tip
  // Return 1 if a measurement is ongoing

  // We want to perform our startup measurements of the tip resistance until we detect one fitted

  // Step 1; if not setup, we turn on pullup and then wait
  if (tipMeasurementOccuring == false && (start || tipSenseResistancex10Ohms == 0 || lastMeas == 0)) {
    tipMeasurementOccuring    = true;
    tipSenseResistancex10Ohms = 0;
    lastMeas                  = xTaskGetTickCount();
    adcReadingPD1Set          = 0;
    setPlatePullup(true);
    return;
  }

  // Wait 100ms for settle time
  if ((xTaskGetTickCount() - lastMeas) < (TICKS_100MS)) {
    return;
  }

  lastMeas = xTaskGetTickCount();
  // We are sensing the resistance
  if (adcReadingPD1Set == 0) {
    // We will record the reading for PD1 being set
    adcReadingPD1Set = getADC(3);
    setPlatePullup(false);
    return;
  }
  // Taking reading two
  uint16_t adcReadingPD1Cleared = getADC(3);
  uint32_t a                    = ((int)adcReadingPD1Set - (int)adcReadingPD1Cleared);
  a *= 10000;
  uint32_t b = ((int)adcReadingPD1Cleared + (32768 - (int)adcReadingPD1Set));
  if (b) {
    tipSenseResistancex10Ohms = a / b;
  } else {
    tipSenseResistancex10Ohms = adcReadingPD1Set = lastMeas = 0;
  }
  if (tipSenseResistancex10Ohms > 1100 || tipSenseResistancex10Ohms < 900) {
    tipSenseResistancex10Ohms = 0; // out of range
    adcReadingPD1Set          = 0;
    lastMeas                  = 0;
    return;
  }
  tipMeasurementOccuring = false;
}
bool isTipDisconnected() {
  static bool lastDisconnectedState = false;
  // For the MHP30 we want to include a little extra logic in here
  // As when the tip is first connected we want to measure the ~100 ohm resistor on the base of the tip
  // And likewise if its removed we want to clear that measurement
  /*
   * plate_sensor_res = ((adc5_value_PD1_set - adc5_value_PD1_cleared) / (adc5_value_PD1_cleared + 4096 - adc5_value_PD1_set)) * 1000.0;
   * */
  if (tipMeasurementOccuring) {
    performTipMeasurementStep(false);
    return true; // We fake no tip disconnection during the measurement cycle to mask it
  }

  // If we are too close to the top, most likely disconnected tip
  bool tipDisconnected = getTipInstantTemperature() > (4090 * 8);
  if (tipDisconnected == false && lastDisconnectedState == true) {
    // Tip is now disconnected
    performTipMeasurementStep(true);
  }
  lastDisconnectedState = tipDisconnected;
  return tipDisconnected;
}

uint8_t preStartChecks() {
  performTipMeasurementStep(false);
  return tipMeasurementOccuring ? 0 : 1;
}
void setBuzzer(bool on) {
  if (on) {
    htim3.Instance->CCR2 = 128;
    htim3.Instance->PSC  = 100; // drop down into audible range
  } else {
    htim3.Instance->CCR2 = 0;
    htim3.Instance->PSC  = 1; // revert back out of hearing range
  }
}
void setStatusLED(const enum StatusLED state) {
  static enum StatusLED lastState = LED_UNKNOWN;
  static TickType_t     buzzerEnd = 0;

  if (lastState != state || state == LED_HEATING) {
    switch (state) {
    default:
    case LED_UNKNOWN:
    case LED_OFF:
      ws2812.led_set_color(0, 0, 0, 0);
      break;
    case LED_STANDBY:
      ws2812.led_set_color(0, 0, 0xFF, 0); // green
      break;
    case LED_HEATING: {
      ws2812.led_set_color(0, ((HAL_GetTick() / 10) % 192) + 64, 0, 0); // Red fade
    } break;
    case LED_HOT:
      ws2812.led_set_color(0, 0xFF, 0, 0); // red
      // We have hit the right temp, run buzzer for a short period
      buzzerEnd = xTaskGetTickCount() + TICKS_SECOND / 3;
      break;
    case LED_COOLING_STILL_HOT:
      ws2812.led_set_color(0, 0xFF, 0x8C, 0x00); // Orange
      break;
    }
    ws2812.led_update();
    lastState = state;
  }
  if (state == LED_HOT && xTaskGetTickCount() < buzzerEnd) {
    setBuzzer(true);
  } else {
    setBuzzer(false);
  }
}
uint64_t getDeviceID() {
  //
  return HAL_GetUIDw0() | ((uint64_t)HAL_GetUIDw1() << 32);
}

uint8_t preStartChecksDone() { return 1; }

uint8_t getTipThermalMass() { return TIP_THERMAL_MASS; }