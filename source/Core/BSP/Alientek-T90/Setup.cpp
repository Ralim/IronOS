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
#include "Setup.h"
#include "BSP.h"
#include "FreeRTOS.h"
#include "Pins.h"
#include "configuration.h"
#include "history.hpp"
#include "n32l40x.h"
#include "semphr.h"
#include "task.h"
#include <stdint.h>
#include <string.h>

#define ADC_FILTER_LEN 4

// The single ADC's regular group is reconfigured + software-started on demand by readRegularChannel,
// which is called from BOTH the PID thread (Vin ch2) and the GUI thread (NTC ch3). Without
// serialisation those reads race: one thread re-points the regular rank while another is mid-conversion,
// so a caller gets a different channel's value. This mutex makes each select+convert+read atomic.
static StaticSemaphore_t adcRegularMutexBuffer; // FreeRTOS here is static-allocation only
static SemaphoreHandle_t adcRegularMutex = NULL;

// Bounded spins on ADC hardware flags so a wedged ADC can never hang the PID thread. A real 71.5-cycle
// conversion at the 8 MHz ADC clock finishes in a few hundred iterations; keep the bound small so the
// regular-read mutex (held across the poll) can never stall the PID safety path for long.
#define ADC_POLL_TIMEOUT 5000U

// Heater PWM carrier (TIM2 on APB1, timer clock 32 MHz at 64 MHz SYSCLK).
// Prescaler 0, ARR 1066 -> 32 MHz / 1067 ~= 30 kHz. CCR range 0..1066.
#define HEATER_TIM_PRESCALER 0
#define HEATER_TIM_ARR       1066

// ADC-schedule timer (TIM4 on APB1, timer clock 32 MHz). Mirrors the Sequre
// totalPWM concept: one schedule period = powerPWM + holdoff + tempMeasure.
// Prescaler 2000 -> 16 kHz tick; ARR = totalPWM -> ~56 Hz schedule / ADC rate.
#define SCHEDULE_TIM_PRESCALER 1999
static const uint16_t tempMeasureTicks = 15;
// Settling holdoff (TIM4 ticks, 62.5us each) between the heater blanking (TIM4 UPDATE) and the
// injected tip sample (OC2REF->TRGO at this count). The high-impedance thermocouple op-amp needs
// ~ms to recover after the heater switches off; sampling at the heater-off instant reads the
// transient far too low, so the PID never converges and drives full power. ~3ms. TODO scope-tune
// for power vs accuracy (larger = more settling but lower max heater duty).
static const uint16_t holdoffTicks = 48;

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
  // Created before the scheduler starts (single-threaded here), so readRegularChannel can serialise
  // the cross-thread ADC access once tasks are running.
  if (adcRegularMutex == NULL) {
    adcRegularMutex = xSemaphoreCreateMutexStatic(&adcRegularMutexBuffer);
  }
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
  // Serialise against the other thread once the scheduler is running (mutex yields rather than
  // disabling interrupts, so the heater schedule/safety IRQs keep firing during the ~us conversion).
  const bool lock = (adcRegularMutex != NULL) && (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING);
  if (lock) {
    xSemaphoreTake(adcRegularMutex, portMAX_DELAY);
  }
  // Only the regular sequence is reprogrammed here. The injected tip group (length + channels) is set
  // once in MX_ADC_Init and lives in separate registers, so it stays intact without re-asserting it.
  // Do NOT re-write the injected length on every read: that JSEQ write can abort an in-flight
  // TIM4-triggered tip conversion (the PID-critical sample).
  ADC_ConfigRegularChannel(ADC, channel, 1, ADC_SAMP_TIME_71CYCLES5);
  // Convert twice and keep the second pass: the regular rank is re-pointed to a different pad every
  // call, so the first conversion's sample-and-hold still carries charge from the previously selected
  // channel (cross-channel bleed). Discarding it gives Vin/NTC a steady reading.
  uint16_t result = 0;
  for (uint8_t pass = 0; pass < 2; pass++) {
    ADC_ClearFlag(ADC, ADC_FLAG_ENDC);
    ADC_EnableSoftwareStartConv(ADC, ENABLE);
    uint32_t to = ADC_POLL_TIMEOUT;
    while (ADC_GetFlagStatus(ADC, ADC_FLAG_ENDC) == RESET && --to) {
    }
    result = ADC_GetDat(ADC);
  }
  if (lock) {
    xSemaphoreGive(adcRegularMutex);
  }
  return result;
}

// Averaged cold-junction NTC reading. Currently unused: getHandleTemperature() returns a fixed 38 degC
// (the factory tip LUT is absolute, see BSP.cpp). Retained as the hardware NTC reader the live
// cold-junction TODO will use once a reference thermometer is available; the linker GC-strips it meanwhile.
uint16_t getADCHandleTemp(uint8_t sample) {
#ifdef TMP36_ADC_CHANNEL
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    // Cold-junction NTC on PA2, one polled conversion per call, averaged. Scale the raw 12-bit
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
  // GPIO ports + TIM1 + SPI1 live on APB2. (AFIO is not needed: no EXTI/remap is used, and SPI/TIM
  // alternate functions are selected per-pin via GPIO_InitPeripheral's AFL/AFH writes.)
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_GPIOB | RCC_APB2_PERIPH_GPIOD | RCC_APB2_PERIPH_TIM1 | RCC_APB2_PERIPH_SPI1, ENABLE);
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

  // Analog inputs: the stock firmware drives PA1..PA5 as the ADC front-end (Vin=PA1, NTC=PA2,
  // tip=PA3, current=PA4, tip-detect=PA5). Configure all five as analog so every channel is valid.
  io.Pin       = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  io.GPIO_Mode = GPIO_Mode_Analog;
  io.GPIO_Pull = GPIO_No_Pull;
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

  // Display SPI1 alternate function. N32L40x uses PER-PIN AF numbers: SCK (PB3) = AF1, MOSI (PB5)
  // = AF0 (confirmed from the T90 stock-firmware register writes and the N32L43 AF table; a uniform
  // AF5 here would leave SCK/MOSI unrouted -> blank panel).
  GPIO_InitStruct(&io);
  io.GPIO_Mode      = GPIO_Mode_AF_PP;
  io.GPIO_Slew_Rate = GPIO_Slew_Rate_High;
  io.GPIO_Current   = GPIO_DC_8mA;
  io.GPIO_Pull      = GPIO_No_Pull;
  io.Pin            = LCD_SCK_Pin;
  io.GPIO_Alternate = GPIO_AF1_SPI1;
  GPIO_InitPeripheral(GPIOB, &io);
  io.Pin            = LCD_MOSI_Pin;
  io.GPIO_Alternate = GPIO_AF0_SPI1;
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
  // N32 ADC also needs the 1 MHz calibration/bandgap clock and the internal reference enabled, or
  // calibration + conversions (which feed the heater PID) are untrustworthy.
  RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV16);
  ADC_EnableTempSensorVrefint(ENABLE);

  ADC_InitType a;
  ADC_InitStruct(&a);
  a.MultiChEn      = DISABLE;               // one regular channel at a time (see readRegularChannel)
  a.ContinueConvEn = DISABLE;               // one regular shot per software start
  a.ExtTrigSelect  = ADC_EXT_TRIGCONV_NONE; // regular group is software-started
  a.DatAlign       = ADC_DAT_ALIGN_R;
  a.ChsNumber      = 1; // single regular rank, swapped on demand
  ADC_Init(ADC, &a);

  // Default regular rank (Vin); readRegularChannel() re-points this per call to
  // Vin (ch2/PA1) or NTC (ch3/PA2).
  ADC_ConfigRegularChannel(ADC, VIN_ADC_CHANNEL, 1, ADC_SAMP_TIME_71CYCLES5);

  // Injected group: 4 ranks all on the tip channel (ch4/PA3) = a 4x oversample,
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
  while (ADC_GetFlagStatusNew(ADC, ADC_FLAG_RDY) == RESET && --to) {
  }
  ADC_StartCalibration(ADC);
  to = ADC_POLL_TIMEOUT;
  while (ADC_GetCalibrationStatus(ADC) != RESET && --to) {
  }
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

  // Phase the injected tip sample a settling holdoff AFTER the heater is blanked. The heater is
  // blanked on the UPDATE event (counter wrap) in TIM4_IRQHandler; OC2 in PWM mode 2 raises OC2REF
  // when the counter reaches CCR2 = holdoffTicks, and OC2REF drives TRGO -> the injected tip
  // conversion. So the tip is sampled ~holdoffTicks*62.5us after the heater stops, once the
  // high-impedance thermocouple front-end has settled (sampling at the heater-off instant reads the
  // transient far too low -> PID never converges -> full-power runaway). The UPDATE interrupt still
  // fires at the wrap and blanks the heater; only the TRGO source moves from UPDATE to OC2REF.
  OCInitType oc;
  TIM_InitOcStruct(&oc);
  oc.OcMode      = TIM_OCMODE_PWM2;         // OC2REF rises when CNT reaches CCR2
  oc.OutputState = TIM_OUTPUT_STATE_ENABLE; // generate OC2REF internally (TIM4_CH2 is not pin-routed)
  oc.Pulse       = holdoffTicks;            // sample holdoffTicks after the wrap
  oc.OcPolarity  = TIM_OC_POLARITY_HIGH;
  TIM_InitOc2(TIM4, &oc);
  TIM_ConfigOc2Preload(TIM4, TIM_OC_PRE_LOAD_DISABLE);
  TIM_SelectOutputTrig(TIM4, TIM_TRGO_SRC_OC2REF);
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
