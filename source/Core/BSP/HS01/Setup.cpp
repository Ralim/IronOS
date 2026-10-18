/*
 * Setup.cpp
 *
 * FNIRSI HS-01 (WCH CH571F)
 */
#include "Setup.h"
#include "BSP.h"
#include "Pins.h"
#include "history.hpp"
#include <stdint.h>

#define ADC_FILTER_LEN 4
#define ADC_SAMPLES    8 // Conversions summed per reading, 4095 * 8 fits ADC_MAX_READING

static int16_t           adcOffset = 0;
static volatile uint16_t tipADCSum = 0;
static volatile uint16_t vinADCSum = 0;

static void setupGPIO();
static void setupHeaterPWM();
static void setupADC();
static void setupFrameTimer();

void hardware_init() {
  SetSysClock(CLK_SOURCE_PLL_60MHz);
  setupGPIO();
  setupHeaterPWM();
  setupADC();
  setupFrameTimer();
  WWDG_SetCounter(0);
  WWDG_ResetCfg(ENABLE); // ~0.56 s at 60 MHz, fed from the idle hook
}

static void setupGPIO() {
  GPIOB_ModeCfg(KEY_A_Pin | KEY_B_Pin, GPIO_ModeIN_PU);
  GPIOA_ModeCfg(MOVEMENT_Pin, GPIO_ModeIN_PU);
  armMovementInterrupt();
  PFIC_EnableIRQ(GPIO_A_IRQn);
  GPIOA_ModeCfg(VIN_Pin | TIP_TEMP_Pin, GPIO_ModeIN_Floating);

  // Ask for 20 V straight away so the charger has settled by the time the PID starts; Power.cpp walks it down if
  // the charger does not offer it
  setCH224KVoltage(20);
  GPIOA_ModeCfg(CH224K_CFG2_Pin | CH224K_CFG3_Pin, GPIO_ModeOut_PP_5mA);

  GPIOB_SetBits(OLED_RESET_Pin);
  GPIOB_ModeCfg(OLED_RESET_Pin, GPIO_ModeOut_PP_5mA);
}

// CH224K with CFG1 tied low: CFG2/CFG3 = 0/0 9 V, 0/1 12 V, 1/1 15 V, 1/0 20 V (same mapping the stock menu uses)
void setCH224KVoltage(uint8_t volts) {
  const bool cfg2 = volts >= 15;
  const bool cfg3 = volts == 12 || volts == 15;
  if (cfg2) {
    GPIOA_SetBits(CH224K_CFG2_Pin);
  } else {
    GPIOA_ResetBits(CH224K_CFG2_Pin);
  }
  if (cfg3) {
    GPIOA_SetBits(CH224K_CFG3_Pin);
  } else {
    GPIOA_ResetBits(CH224K_CFG3_Pin);
  }
}

static void setupHeaterPWM() {
  // PB23 is already driven low by the startup code
  GPIOB_ResetBits(TIP_PWM_Pin);
  GPIOB_ModeCfg(TIP_PWM_Pin, GPIO_ModeOut_PP_20mA);
  GPIOPinRemap(ENABLE, RB_PIN_TMR0);
  TMR0_PWMCycleCfg(HEATER_PWM_PERIOD);
  TMR0_PWMActDataWidth(0);
  TMR0_PWMInit(High_Level, PWM_Times_1); // Idles low, high for the duty count
}

static void setupADC() {
  // PGA 0 dB: 0..2.1 V input range
  ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);
  adcOffset = ADC_DataCalib_Rough();
}

static void setupFrameTimer() {
  TMR1_TimerInit(FRAME_TICK_CYCLES * powerPWM);
  TMR1_ITCfg(ENABLE, TMR0_3_IT_CYC_END);
  PFIC_EnableIRQ(TMR1_IRQn);
}

static uint16_t convert() {
  const int16_t raw = ADC_ExcutSingleConver();
  // A pegged ADC reads one count over full scale, so IronOS's "ADC pegged" checks can see it
  if (raw >= 4095) {
    return 4096;
  }
  const int16_t value = raw + adcOffset;
  return value < 0 ? 0 : (value > 4095 ? 4095 : value);
}

// Like the stock firmware: spread the conversions out, drop the highest and lowest, average the rest.
// Scaled to the full scale of an ADC_SAMPLES sum.
static uint16_t sampleChannel(uint8_t channel, uint8_t samples, uint8_t spacingUs) {
  ADC_ChannelCfg(channel);
  convert(); // Throw away the first conversion after switching the channel
  uint32_t sum = 0;
  uint16_t lo  = 4096;
  uint16_t hi  = 0;
  for (uint8_t i = 0; i < samples; i++) {
    mDelayuS(spacingUs);
    const uint16_t value = convert();
    sum += value;
    lo = value < lo ? value : lo;
    hi = value > hi ? value : hi;
  }
  return ((sum - lo - hi) * ADC_SAMPLES) / (samples - 2);
}

// About 0.6 ms in total, inside the frame timer interrupt
void sampleADC() {
  tipADCSum = sampleChannel(TIP_TEMP_ADC_CHANNEL, 16, 25);
  vinADCSum = sampleChannel(VIN_ADC_CHANNEL, 10, 10);
}

uint16_t getADCVin(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    filter.update(vinADCSum);
  }
  return filter.average();
}

// Returns either average or instant value. When sample is set the latest samples are copied to the filter and then the raw reading is returned
uint16_t getTipRawTemp(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter  = {{0}, 0, 0};
  static uint16_t                          last[3] = {0};
  if (sample) {
    last[2] = last[1];
    last[1] = last[0];
    last[0] = tipADCSum;
    filter.update(last[0]);
    // Median of the last three frames, so a single noisy frame doesn't swing the PID
    const uint16_t a = last[0], b = last[1], c = last[2];
    return a > b ? (b > c ? b : (a > c ? c : a)) : (a > c ? a : (b > c ? c : b));
  }
  return filter.average();
}
