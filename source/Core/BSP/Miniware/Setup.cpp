/*
 * Setup.c
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Setup.h"
#include "BSP.h"
#include "Pins.h"
#include "configuration.h"
#include "history.hpp"
#include <stdint.h>
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;

IWDG_HandleTypeDef hiwdg;
TIM_HandleTypeDef  htimADC;
TIM_HandleTypeDef  htimTip;
#define ADC_FILTER_LEN 4
#define ADC_SAMPLES    16
uint16_t ADCReadings[ADC_SAMPLES]; // Used to store the adc readings for the handle cold junction temp

// Functions
static void SystemClock_Config(void);
static void MX_ADC1_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIP_CONTROL_TIMER_Init(void);
static void MX_ADC_CONTROL_TIMER_Init(void);
static void MX_DMA_Init(void);
static void MX_GPIO_Init(void);
static void MX_ADC2_Init(void);
void        Setup_HAL() {
  SystemClock_Config();

#ifndef SWD_ENABLE
  __HAL_AFIO_REMAP_SWJ_DISABLE();
#else
  __HAL_AFIO_REMAP_SWJ_NOJTAG();
#endif

  MX_GPIO_Init();
  MX_DMA_Init();
#ifndef I2C_SOFT_BUS_1
#error "Only Bit-Bang now"
#endif
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIP_CONTROL_TIMER_Init();
  MX_ADC_CONTROL_TIMER_Init();
  MX_IWDG_Init();
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADCReadings, (ADC_SAMPLES)); // start DMA of normal readings
  HAL_ADCEx_InjectedStart(&hadc1);                                   // enable injected readings
  HAL_ADCEx_InjectedStart(&hadc2);                                   // enable injected readings
}

uint16_t getADCHandleTemp(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < ADC_SAMPLES; i++) {
      sum += ADCReadings[i];
    }
    filter.update(sum);
  }
  return filter.average() >> 1;
}

#ifdef HAS_SPLIT_POWER_PATH
static history<uint16_t, ADC_FILTER_LEN> filteredDC = {{0}, 0, 0};
static history<uint16_t, ADC_FILTER_LEN> filteredPD = {{0}, 0, 0};

uint16_t getRawDCVin() { return filteredDC.average(); }
uint16_t getRawPDVin() { return filteredPD.average(); }
#endif

uint16_t getADCVin(uint8_t sample) {
#ifdef HAS_SPLIT_POWER_PATH
  // In split power path operation, we need to read both inputs, and return the larger

  if (sample) {
    {
      uint16_t latestADC = 0;
      latestADC += hadc2.Instance->JDR1;
      latestADC += hadc2.Instance->JDR2;
      latestADC <<= 3;
      filteredDC.update(latestADC);
    }
    {
      uint16_t latestADC = 0;
      latestADC += hadc2.Instance->JDR3;
      latestADC += hadc2.Instance->JDR4;
      latestADC <<= 3;
      filteredPD.update(latestADC);
    }
  }
  uint16_t dc = filteredDC.average();
  uint16_t pd = filteredPD.average();
  if (dc > pd) {
    return dc;
  }
  return pd;
#else
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    uint16_t latestADC = 0;

    latestADC += hadc2.Instance->JDR1;
    latestADC += hadc2.Instance->JDR2;
    latestADC += hadc2.Instance->JDR3;
    latestADC += hadc2.Instance->JDR4;
    latestADC <<= 1;
    filter.update(latestADC);
  }
  return filter.average();
#endif
}
// Returns either average or instant value. When sample is set the samples from the injected ADC are copied to the filter and then the raw reading is returned
uint16_t getTipRawTemp(uint8_t sample) {
  static history<uint16_t, ADC_FILTER_LEN> filter = {{0}, 0, 0};
  if (sample) {
    uint16_t latestADC = 0;

    latestADC += hadc1.Instance->JDR1;
    latestADC += hadc1.Instance->JDR2;
    latestADC += hadc1.Instance->JDR3;
    latestADC += hadc1.Instance->JDR4;
    latestADC <<= 1;
    filter.update(latestADC);
    return latestADC;
  }
  return filter.average();
}
/** System Clock Configuration
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef       RCC_OscInitStruct;
  RCC_ClkInitTypeDef       RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState            = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL16; // 64MHz
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /**Initializes the CPU, AHB and APB busses clocks
   */
  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16; // TIM
                                                     // 2,3,4,5,6,7,12,13,14
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // 64 mhz to some peripherals and adc

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6; // 6 or 8 are the only non overclocked options
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  /**Configure the Systick interrupt time
   */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  /**Configure the Systick
   */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void) {

  ADC_ChannelConfTypeDef   sConfig;
  ADC_InjectionConfTypeDef sConfigInjected;
  /**Common config
   */
  hadc1.Instance                   = ADC1;
  hadc1.Init.ScanConvMode          = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode    = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion       = 1;
  HAL_ADC_Init(&hadc1);

  /**Configure Regular Channel
   */
  sConfig.Channel      = TMP36_ADC1_CHANNEL;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  /**Configure Injected Channel
   */
  // F in = 10.66 MHz
  /*
   * Injected time is 1 delay clock + (12 adc cycles*4)+4*sampletime =~217
   * clocks = 0.2ms Charge time is 0.016 uS ideally So Sampling time must be >=
   * 0.016uS 1/10.66MHz is 0.09uS, so 1 CLK is *should* be enough
   * */
  sConfigInjected.InjectedChannel               = TIP_TEMP_ADC1_CHANNEL;
  sConfigInjected.InjectedRank                  = 1;
  sConfigInjected.InjectedNbrOfConversion       = 4;
  sConfigInjected.InjectedSamplingTime          = ADC_SAMPLETIME_28CYCLES_5;
  sConfigInjected.ExternalTrigInjecConv         = ADC_TRIGGER;
  sConfigInjected.AutoInjectedConv              = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset                = 0;

  HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
  sConfigInjected.InjectedRank = 2;
  HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
  sConfigInjected.InjectedRank = 3;
  HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
  sConfigInjected.InjectedRank = 4;
  HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected);
  SET_BIT(hadc1.Instance->CR1, (ADC_CR1_JEOCIE)); // Enable end of injected conv irq
  // Run ADC internal calibration
  while (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
    ;
  }
}

/* ADC2 init function */
static void MX_ADC2_Init(void) {
  ADC_InjectionConfTypeDef sConfigInjected;

  /**Common config
   */
  hadc2.Instance                   = ADC2;
  hadc2.Init.ScanConvMode          = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode    = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion       = 0;
  HAL_ADC_Init(&hadc2);

  /**Configure Injected Channel
   */
  sConfigInjected.InjectedChannel               = VIN_ADC2_CHANNEL;
  sConfigInjected.InjectedRank                  = ADC_INJECTED_RANK_1;
  sConfigInjected.InjectedNbrOfConversion       = 4;
  sConfigInjected.InjectedSamplingTime          = ADC_SAMPLETIME_28CYCLES_5;
  sConfigInjected.ExternalTrigInjecConv         = ADC_TRIGGER;
  sConfigInjected.AutoInjectedConv              = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset                = 0;
  HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_2;
  HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

#ifdef HAS_SPLIT_POWER_PATH
  sConfigInjected.InjectedChannel = PD_VIN_ADC2_CHANNEL;
#endif

  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_3;
  HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_4;
  HAL_ADCEx_InjectedConfigChannel(&hadc2, &sConfigInjected);

  // Run ADC internal calibration
  while (HAL_ADCEx_Calibration_Start(&hadc2) != HAL_OK) {
    ;
  }
}

/* IWDG init function */
static void MX_IWDG_Init(void) {
  hiwdg.Instance       = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload    = 100;
#ifndef SWD_ENABLE
  HAL_IWDG_Init(&hiwdg);
#endif
}

/* TIM3 init function */
static void MX_TIP_CONTROL_TIMER_Init(void) {
  TIM_ClockConfigTypeDef  sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef      sConfigOC;

  htimTip.Instance = TIP_CONTROL_TIMER;
#ifdef TIP_HAS_DIRECT_PWM
  htimTip.Init.Prescaler = 100;
#else
  htimTip.Init.Prescaler = 3;
#endif
  htimTip.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htimTip.Init.Period            = 255;                           // 5 Khz PWM freq
  htimTip.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV4;        // 4mhz before div
  htimTip.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // Preload the ARR register (though we dont use this)
  HAL_TIM_Base_Init(&htimTip);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htimTip, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htimTip);

  HAL_TIM_OC_Init(&htimTip);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htimTip, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
#ifdef TIP_HAS_DIRECT_PWM
  sConfigOC.Pulse = 0; // PWM is direct to tip
#else
  sConfigOC.Pulse = 127; // 50% duty cycle, that is AC coupled through the cap to provide an on signal (This does not do tip at 50% duty cycle)
#endif
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  HAL_TIM_PWM_ConfigChannel(&htimTip, &sConfigOC, PWM_Out_CHANNEL);

  GPIO_InitTypeDef GPIO_InitStruct;

  /**TIM3 GPIO Configuration
   PWM_Out_Pin     ------> TIM3_CH1
   */
  GPIO_InitStruct.Pin   = PWM_Out_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // We would like sharp rising edges
  HAL_GPIO_Init(PWM_Out_GPIO_Port, &GPIO_InitStruct);
#ifdef MODEL_TS100
  // Remap TIM3_CH1 to be on PB4
  __HAL_AFIO_REMAP_TIM3_PARTIAL();
#else
  // No re-map required
#endif
  HAL_TIM_PWM_Start(&htimTip, PWM_Out_CHANNEL);
}
/* TIM3 init function */
static void MX_ADC_CONTROL_TIMER_Init(void) {
  /*
   * We use the channel 1 to trigger the ADC at end of PWM period
   * And we use the channel 4 as the PWM modulation source using Interrupts
   * */
  TIM_ClockConfigTypeDef  sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef      sConfigOC;

  // Timer 2 is fairly slow as its being used to run the PWM and trigger the ADC
  // in the PWM off time.
  htimADC.Instance = ADC_CONTROL_TIMER;
  // dummy value, will be reconfigured by BSPInit()
  htimADC.Init.Prescaler = 2000; // 2 MHz timer clock/2000 = 1 kHz tick rate

  // pwm out is 10k from tim3, we want to run our PWM at around 10hz or slower on the output stage
  // These values give a rate of around 3.5 Hz for "fast" mode and 1.84 Hz for "slow"
  htimADC.Init.CounterMode = TIM_COUNTERMODE_UP;
  // dummy value, will be reconfigured by BSPInit()
  htimADC.Init.Period = powerPWM + 14 * 2;

  htimADC.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV4; // 8 MHz (x2 APB1) before divide
  htimADC.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  htimADC.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htimADC);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htimADC, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htimADC);
  HAL_TIM_OC_Init(&htimADC);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htimADC, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  // dummy value, will be reconfigured by BSPInit() in the BSP.cpp
  sConfigOC.Pulse = powerPWM + 14; // 13 -> Delay of 7 ms
  // 255 is the largest time period of the drive signal, and then offset ADC sample to be a bit delayed after this
  /*
   * It takes 4 milliseconds for output to be stable after PWM turns off.
   * Assume ADC samples in 0.5ms
   * We need to set this to 100% + 4.5ms
   * */
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  HAL_TIM_PWM_ConfigChannel(&htimADC, &sConfigOC, TIM_CHANNEL_1);
  sConfigOC.Pulse = 0; // default to entirely off
  HAL_TIM_OC_ConfigChannel(&htimADC, &sConfigOC, TIM_CHANNEL_4);

  HAL_TIM_Base_Start_IT(&htimADC);
  HAL_TIM_PWM_Start(&htimADC, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(&htimADC, TIM_CHANNEL_4);
  HAL_NVIC_SetPriority(ADC_CONTROL_TIMER_IRQ, 15, 0);
  HAL_NVIC_EnableIRQ(ADC_CONTROL_TIMER_IRQ);
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 * Free pins are configured automatically as Analog
 PB0   ------> ADCx_IN8
 PB1   ------> ADCx_IN9
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  /*Configure GPIO pins : PD0 PD1 */
  GPIO_InitStruct.Pin  = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  /*Configure peripheral I/O remapping */
  __HAL_AFIO_REMAP_PD01_ENABLE();
  //^ remap XTAL so that pins can be analog (all input buffers off).
  // reduces power consumption

  /*
   * Configure All pins as analog by default
   */
  GPIO_InitStruct.Pin  = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
#ifdef MODEL_TS100
                        GPIO_PIN_3 |
#endif
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

#ifdef MODEL_TS100
#ifndef SWD_ENABLE
  /* Pull USB and SWD lines low to prevent enumeration attempts and EMI affecting
   * the debug core */
  GPIO_InitStruct.Pin   = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
#else
  /* Make all lines affecting SWD floating to allow debugging */
  GPIO_InitStruct.Pin  = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#else
  /* TS80 */
  /* Leave USB lines open circuit*/

#endif

  /*Configure GPIO pins : KEY_B_Pin KEY_A_Pin */
  GPIO_InitStruct.Pin  = KEY_B_Pin | KEY_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_B_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OLED_RESET_Pin */
  GPIO_InitStruct.Pin   = OLED_RESET_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OLED_RESET_GPIO_Port, &GPIO_InitStruct);

  // Pull down LCD reset
  HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(30);
  HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);

#ifdef DC_SELECT_Pin
  GPIO_InitStruct.Pin  = DC_SELECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DC_SELECT_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(DC_SELECT_GPIO_Port, DC_SELECT_Pin, GPIO_PIN_RESET);
#endif

#ifdef PD_SELECT_Pin
  GPIO_InitStruct.Pin  = PD_SELECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PD_SELECT_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(PD_SELECT_GPIO_Port, PD_SELECT_Pin, GPIO_PIN_RESET);

#endif

#ifdef TIP_RESISTANCE_SENSE_Pin
  GPIO_InitStruct.Pin  = TIP_RESISTANCE_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TIP_RESISTANCE_SENSE_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(TIP_RESISTANCE_SENSE_GPIO_Port, TIP_RESISTANCE_SENSE_Pin, GPIO_PIN_RESET);

#endif

#ifdef INT_PD_Pin
  GPIO_InitStruct.Pin  = INT_PD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(INT_PD_GPIO_Port, &GPIO_InitStruct);

#endif
}
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { asm("bkpt"); }
#endif
