/*
 * Setup.cpp
 *
 *  Core low-level bring-up for the Alientek T90 (Nations N32L40x).
 *  Re-implemented from the Sequre (ST HAL) template against the Nations
 *  std-periph driver. No HAL_* calls.
 *
 *  Timer / ADC interleave (shared interface contract):
 *   - TIM2_CH1 on PA0 is the heater PWM carrier (~30 kHz). CCR1 is written by
 *     the TIM4 update IRQ (IRQ.cpp) from pendingPWM, gated by PWMSafetyTimer.
 *   - TIM4 is the slow ADC-schedule timer. Its TRGO (on update) triggers the
 *     ADC injected group (the 4 tip ranks) in the heater-off window, and its
 *     update IRQ runs the PWM-safety + heater-duty write.
 *   - The ADC injected-EOC IRQ (ADC_IRQHandler, IRQ.cpp) notifies the PID task.
 */
#include "BSP.h"
#include "Pins.h"
#include "Setup.h"
#include "configuration.h"
#include "history.hpp"
#include "n32l40x.h"
#include <stdint.h>
#include <string.h>

#define ADC_FILTER_LEN 4

// Bounded spins on ADC hardware flags so a wedged ADC can never hang the PID thread (it would
// otherwise stop feeding the safety timer -> heater off -> watchdog reset, but a bounded loop is
// cleaner). ~100k iterations is far longer than any real conversion at the 8 MHz ADC clock.
#define ADC_POLL_TIMEOUT 100000U

// Heater PWM carrier (TIM2 on APB1, timer clock 32 MHz at 64 MHz SYSCLK).
// Prescaler 0, ARR 1066 -> 32 MHz / 1067 ~= 30 kHz. CCR range 0..1066.
#define HEATER_TIM_PRESCALER 0
#define HEATER_TIM_ARR       1066

// ADC-schedule timer (TIM4 on APB1, timer clock 32 MHz). Mirrors the Sequre
// totalPWM concept: one schedule period = powerPWM + holdoff + tempMeasure.
// Prescaler 2000 -> 16 kHz tick; ARR = totalPWM -> ~56 Hz schedule / ADC rate.
#define SCHEDULE_TIM_PRESCALER 1999
static const uint16_t tempMeasureTicks = 15;
static const uint16_t holdoffTicks     = 15;

// Globals consumed by the Core (declared extern in BSP.h).
const uint16_t powerPWM = 255;
uint16_t       totalPWM = powerPWM + tempMeasureTicks + holdoffTicks;

// Functions
static void Clock_Config_NVIC(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM2_Init(void); // heater PWM carrier (PA0)
static void MX_TIM4_Init(void); // ADC schedule + safety IRQ
static void MX_IWDG_Init(void);

void Setup_HAL(void) {
  Clock_Config_NVIC();
  MX_GPIO_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_IWDG_Init();
}

// --- ADC read helpers (mirror Sequre Setup.cpp:67-112 on the N32 single ADC) ---

// The single ADC shares one regular data register, so Vin/NTC/current cannot be
// distinguished from a scan without DMA. Select and convert one channel on demand
// instead: program a length-1 regular sequence on the requested pad, software
// start, poll end-of-conversion, return the result.
static uint16_t readRegularChannel(uint8_t channel) {
  ADC_ConfigInjectedSequencerLength(ADC, 4); // keep injected group intact
  ADC_ConfigRegularChannel(ADC, channel, 1, ADC_SAMP_TIME_71CYCLES5);
  ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
  ADC_EnableSoftwareStartConv(ADC, ENABLE);
  uint32_t to = ADC_POLL_TIMEOUT;
  while (ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == RESET && --to) {}
  ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
  return ADC_GetDat(ADC);
}

uint16_t getADCHandleTemp(uint8_t sample) {
#ifdef TMP36_ADC_CHANNEL
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    // Cold-junction NTC on PA3, one polled conversion per call, averaged. Scale the raw 12-bit
    // reading to the 0..32768 convention with the same <<3 as Vin, because NTCHandleLookup in
    // BSP.cpp holds 15-bit-range thresholds (without this the lookup never matches -> stuck 45C).
    uint16_t latestADC = readRegularChannel(TMP36_ADC_CHANNEL);
    latestADC <<= 3;
    filter.update(latestADC);
  }
  return filter.average();
#else
  (void)sample;
  return 0;
#endif
}

uint16_t getADCVin(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    // Vin on PA2 via the ~10:1 divider. Scale to the 0..32768 convention with
    // the <<3 used by Sequre, then keep the running average.
    uint16_t latestADC = readRegularChannel(VIN_ADC_CHANNEL);
    latestADC <<= 3;
    filter.update(latestADC);
  }
  return filter.average();
}

// Returns either average or instant value. When refresh is set the four injected
// tip ranks are summed, doubled and pushed to the filter; the instant value is
// returned (the PID depends on this freshness).
uint16_t getTipRawTemp(uint8_t refresh) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (refresh) {
    uint16_t latestADC = 0;
    latestADC += ADC_GetInjectedConversionDat(ADC, ADC_INJ_CH_1);
    latestADC += ADC_GetInjectedConversionDat(ADC, ADC_INJ_CH_2);
    latestADC += ADC_GetInjectedConversionDat(ADC, ADC_INJ_CH_3);
    latestADC += ADC_GetInjectedConversionDat(ADC, ADC_INJ_CH_4);
    latestADC <<= 1;
    filter.update(latestADC);
    return latestADC;
  }
  return filter.average();
}

// --- Clock derivatives + NVIC ---
// SystemInit() (startup) already brought SYSCLK to 64 MHz via the compile-line
// SYSCLK_SRC/SYSCLK_FREQ defines and relocated VTOR (VECT_TAB_OFFSET=0x5000).
// Here we only enable the peripheral clocks and program the NVIC.
static void Clock_Config_NVIC(void) {
  // GPIO ports + AFIO live on APB2.
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOD | RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_TIM1 | RCC_APB2_PERIPH_SPI1, ENABLE);
  // Heater carrier (TIM2) and the ADC-schedule timer (TIM4) live on APB1.
  RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2 | RCC_APB1_PERIPH_TIM4, ENABLE);
  // ADC and DMA live on AHB.
  RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_ADC | RCC_AHB_PERIPH_DMA, ENABLE);

  // FreeRTOS expects all-preemption priority bits (no sub-priority).
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  NVIC_InitType n;
  // ADC injected-EOC -> notify PID. On Cortex-M, lower number = higher urgency; a FromISR API is
  // only legal from an ISR whose preemption priority is numerically >= configLIBRARY_MAX_SYSCALL_
  // INTERRUPT_PRIORITY (=5). 6/7 sit in the safe lower-urgency zone -- do NOT set these below 6.
  n.NVIC_IRQChannel                   = ADC_IRQn;
  n.NVIC_IRQChannelPreemptionPriority = 6;
  n.NVIC_IRQChannelSubPriority        = 0;
  n.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&n);

  // TIM4 update -> PWM safety + heater duty write.
  n.NVIC_IRQChannel                   = TIM4_IRQn;
  n.NVIC_IRQChannelPreemptionPriority = 7;
  n.NVIC_IRQChannelSubPriority        = 0;
  n.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&n);
}

// --- GPIO: configure every pin in Pins.h ---
static void MX_GPIO_Init(void) {
  GPIO_InitType io;
  GPIO_InitStruct(&io);

  // Analog inputs: tip (PA4), NTC (PA3), Vin (PA2), current (PA5).
  io.Pin         = TIP_TEMP_Pin | TMP36_INPUT_Pin | VIN_Pin | CURRENT_Pin;
  io.GPIO_Mode   = GPIO_Mode_Analog;
  io.GPIO_Pull   = GPIO_No_Pull;
  GPIO_InitPeripheral(GPIOA, &io);

  // Heater drive PA0 = TIM2_CH1 alternate function (sharp edges).
  GPIO_InitStruct(&io);
  io.Pin            = PWM_Out_Pin;
  io.GPIO_Mode      = GPIO_Mode_AF_PP;
  io.GPIO_Alternate = GPIO_AF2_TIM2;
  io.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
  io.GPIO_Current   = GPIO_DC_8mA;
  io.GPIO_Pull      = GPIO_No_Pull;
  GPIO_InitPeripheral(GPIOA, &io);

  // Buzzer PA7 = TIM1_CH1N alternate function.
  GPIO_InitStruct(&io);
  io.Pin            = BUZZER_Pin;
  io.GPIO_Mode      = GPIO_Mode_AF_PP;
  io.GPIO_Alternate = GPIO_AF2_TIM1;
  io.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
  io.GPIO_Current   = GPIO_DC_8mA;
  io.GPIO_Pull      = GPIO_No_Pull;
  GPIO_InitPeripheral(GPIOA, &io);

  // Display SPI1: SCK (PB3) + MOSI (PB5) as alternate function.
  GPIO_InitStruct(&io);
  io.Pin            = LCD_SCK_Pin | LCD_MOSI_Pin;
  io.GPIO_Mode      = GPIO_Mode_AF_PP;
  io.GPIO_Alternate = GPIO_AF5_SPI1;
  io.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
  io.GPIO_Current   = GPIO_DC_8mA;
  io.GPIO_Pull      = GPIO_No_Pull;
  GPIO_InitPeripheral(GPIOB, &io);

  // Display CS (PA10) + backlight (PA6) as push-pull GPIO outputs, idle high.
  GPIO_InitStruct(&io);
  io.Pin          = LCD_CS_Pin | LCD_BL_Pin;
  io.GPIO_Mode    = GPIO_Mode_Out_PP;
  io.GPIO_Current = GPIO_DC_8mA;
  io.GPIO_Pull    = GPIO_No_Pull;
  GPIO_InitPeripheral(GPIOA, &io);
  GPIO_SetBits(LCD_CS_GPIO_Port, LCD_CS_Pin);
  GPIO_SetBits(LCD_BL_GPIO_Port, LCD_BL_Pin); // backlight is active-low: start off

  // Display DC (PD0) push-pull GPIO output.
  GPIO_InitStruct(&io);
  io.Pin          = LCD_DC_Pin;
  io.GPIO_Mode    = GPIO_Mode_Out_PP;
  io.GPIO_Current = GPIO_DC_8mA;
  io.GPIO_Pull    = GPIO_No_Pull;
  GPIO_InitPeripheral(LCD_DC_GPIO_Port, &io);

  // CH224Q software I2C: SCL (PB6) + SDA (PB7) as open-drain GPIO, idle high.
  GPIO_InitStruct(&io);
  io.Pin          = SCL2_Pin | SDA2_Pin;
  io.GPIO_Mode    = GPIO_Mode_Out_OD;
  io.GPIO_Current = GPIO_DC_8mA;
  io.GPIO_Pull    = GPIO_No_Pull;
  GPIO_InitPeripheral(GPIOB, &io);
  GPIO_SetBits(SCL2_GPIO_Port, SCL2_Pin);
  GPIO_SetBits(SDA2_GPIO_Port, SDA2_Pin);

  // Buttons A (PA15) + B (PB4): inputs, internal pull-up, active-low.
  GPIO_InitStruct(&io);
  io.Pin       = KEY_A_Pin;
  io.GPIO_Mode = GPIO_Mode_Input;
  io.GPIO_Pull = GPIO_Pull_Up;
  GPIO_InitPeripheral(KEY_A_GPIO_Port, &io);
  io.Pin = KEY_B_Pin;
  GPIO_InitPeripheral(KEY_B_GPIO_Port, &io);
}

// --- ADC: injected tip group (4 ranks on PA4) + regular Vin/NTC/current ---
static void MX_ADC_Init(void) {
  // ADC conversion clock from HCLK; DIV8 -> 8 MHz at 64 MHz HCLK.
  ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB, RCC_ADCHCLK_DIV8);

  ADC_InitType a;
  ADC_InitStruct(&a);
  a.MultiChEn      = DISABLE;                // one regular channel at a time (see readRegularChannel)
  a.ContinueConvEn = DISABLE;               // one regular shot per software start
  a.ExtTrigSelect  = ADC_EXT_TRIGCONV_NONE; // regular group is software-started
  a.DatAlign       = ADC_DAT_ALIGN_R;
  a.ChsNumber      = 1;                      // single regular rank, swapped on demand
  ADC_Init(ADC, &a);

  // Default regular rank (Vin); readRegularChannel() re-points this per call to
  // Vin (PA2), NTC (PA3) or current (PA5).
  ADC_ConfigRegularChannel(ADC, VIN_ADC_CHANNEL, 1, ADC_SAMP_TIME_71CYCLES5);

  // Injected group: 4 ranks all on the tip channel (PA4) = a 4x oversample,
  // triggered by TIM4 TRGO so the sample lands in the heater-off window.
  ADC_ConfigInjectedSequencerLength(ADC, 4);
  ADC_ConfigInjectedChannel(ADC, TIP_TEMP_ADC_CHANNEL, 1, ADC_SAMP_TIME_28CYCLES5);
  ADC_ConfigInjectedChannel(ADC, TIP_TEMP_ADC_CHANNEL, 2, ADC_SAMP_TIME_28CYCLES5);
  ADC_ConfigInjectedChannel(ADC, TIP_TEMP_ADC_CHANNEL, 3, ADC_SAMP_TIME_28CYCLES5);
  ADC_ConfigInjectedChannel(ADC, TIP_TEMP_ADC_CHANNEL, 4, ADC_SAMP_TIME_28CYCLES5);
  ADC_ConfigExternalTrigInjectedConv(ADC, ADC_EXT_TRIG_INJ_CONV_T4_TRGO);
  ADC_EnableExternalTrigInjectedConv(ADC, ENABLE);

  // Injected end-of-conversion interrupt drives the PID heartbeat.
  ADC_ConfigInt(ADC, ADC_INT_JENDC, ENABLE);

  // Power up the ADC and wait until it is ready, then calibrate.
  ADC_Enable(ADC, ENABLE);
  uint32_t to = ADC_POLL_TIMEOUT;
  while (ADC_GetFlagStatusNew(ADC, ADC_FLAG_RDY) == RESET && --to) {}
  ADC_StartCalibration(ADC);
  to = ADC_POLL_TIMEOUT;
  while (ADC_GetCalibrationStatus(ADC) != RESET && --to) {}
}

// --- TIM2: heater PWM carrier on CH1 (PA0) ---
static void MX_TIM2_Init(void) {
  TIM_TimeBaseInitType tb;
  TIM_InitTimBaseStruct(&tb);
  tb.Prescaler = HEATER_TIM_PRESCALER;
  tb.Period    = HEATER_TIM_ARR;
  tb.CntMode   = TIM_CNT_MODE_UP;
  tb.ClkDiv    = 0;
  tb.RepetCnt  = 0;
  TIM_InitTimeBase(TIM2, &tb);

  OCInitType oc;
  TIM_InitOcStruct(&oc);
  oc.OcMode      = TIM_OCMODE_PWM1;
  oc.OutputState = TIM_OUTPUT_STATE_ENABLE;
  oc.Pulse       = 0; // start fully off
  oc.OcPolarity  = TIM_OC_POLARITY_HIGH;
  TIM_InitOc1(TIM2, &oc);
  // OC preload DISABLED so the heater-blanking CCR=0 written in the TIM4 update ISR (and the duty
  // re-applied in the ADC ISR) take effect immediately, not at the next carrier wrap. The worst
  // case is a single missed carrier pulse per schedule tick when the duty is re-applied mid-period
  // (~one 33us period out of an ~18ms schedule), which is negligible.
  TIM_ConfigOc1Preload(TIM2, TIM_OC_PRE_LOAD_DISABLE);
  TIM_ConfigArPreload(TIM2, ENABLE);

  TIM_Enable(TIM2, ENABLE);
}

// --- TIM4: ADC-schedule timer (TRGO on update + update IRQ) ---
static void MX_TIM4_Init(void) {
  TIM_TimeBaseInitType tb;
  TIM_InitTimBaseStruct(&tb);
  tb.Prescaler = SCHEDULE_TIM_PRESCALER; // 32 MHz / 2000 = 16 kHz tick
  tb.Period    = totalPWM;               // schedule period -> ~56 Hz update
  tb.CntMode   = TIM_CNT_MODE_UP;
  tb.ClkDiv    = 0;
  tb.RepetCnt  = 0;
  TIM_InitTimeBase(TIM4, &tb);

  // Emit TRGO on each update so the ADC injected group fires once per schedule.
  TIM_SelectOutputTrig(TIM4, TIM_TRGO_SRC_UPDATE);
  TIM_SelectMasterSlaveMode(TIM4, TIM_MASTER_SLAVE_MODE_DISABLE);

  // Update interrupt runs the PWM-safety decrement + heater duty write.
  TIM_ClearFlag(TIM4, TIM_FLAG_UPDATE);
  TIM_ConfigInt(TIM4, TIM_INT_UPDATE, ENABLE);

  TIM_Enable(TIM4, ENABLE);
}

// --- IWDG: ~prescaler 256, reload 2048; gated off when debugging (SWD_ENABLE) ---
static void MX_IWDG_Init(void) {
#ifndef SWD_ENABLE
  IWDG_WriteConfig(IWDG_WRITE_ENABLE);
  IWDG_SetPrescalerDiv(IWDG_PRESCALER_DIV256);
  IWDG_CntReload(2048);
  IWDG_ReloadKey();
  IWDG_Enable();
#endif
}
