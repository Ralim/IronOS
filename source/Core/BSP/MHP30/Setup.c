/*
 * Setup.c
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Setup.h"
#include "Pins.h"
#include <string.h>
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;

IWDG_HandleTypeDef hiwdg;
TIM_HandleTypeDef  htim2;
TIM_HandleTypeDef  htim3;
#define ADC_CHANNELS 4
#define ADC_SAMPLES  16
uint32_t ADCReadings[ADC_SAMPLES * ADC_CHANNELS]; // room for 32 lots of the pair of readings

// Functions
static void SystemClock_Config(void);
static void MX_ADC1_Init(void);

static void MX_IWDG_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
static void MX_DMA_Init(void);
static void MX_GPIO_Init(void);
static void MX_ADC2_Init(void);
void        Setup_HAL() {
  SystemClock_Config();

  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  MX_GPIO_Init();
  MX_DMA_Init();

  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_IWDG_Init();
  HAL_ADC_Start(&hadc2);
  HAL_ADCEx_MultiModeStart_DMA(&hadc1, ADCReadings,
                                      (ADC_SAMPLES * ADC_CHANNELS)); // start DMA of normal readings
                                                              // HAL_ADCEx_InjectedStart(&hadc1); // enable injected readings
                                                              // HAL_ADCEx_InjectedStart(&hadc2); // enable injected readings
}

// channel 0 -> temperature sensor, 1-> VIN, 2-> tip
uint16_t getADC(uint8_t channel) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < ADC_SAMPLES; i++) {
    uint16_t adc1Sample = ADCReadings[channel + (i * ADC_CHANNELS)];
    uint16_t adc2Sample = ADCReadings[channel + (i * ADC_CHANNELS)] >> 16;

    sum += (adc1Sample + adc2Sample);
  }
  return sum >> 2;
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
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // TIM
                                                    // 2,3,4,5,6,7,12,13,14
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // 64 mhz to some peripherals and adc

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection    = RCC_CFGR_ADCPRE_DIV8; // 6 or 8 are the only non overclocked options
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
  ADC_MultiModeTypeDef multimode;

  ADC_ChannelConfTypeDef sConfig;
  /**Common config
   */
  hadc1.Instance                   = ADC1;
  hadc1.Init.ScanConvMode          = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode    = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion       = ADC_CHANNELS;
  HAL_ADC_Init(&hadc1);

  /**Configure the ADC multi-mode
   */
  multimode.Mode = ADC_DUALMODE_REGSIMULT_INJECSIMULT;
  HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode);

  /**Configure Regular Channel
   */
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

  sConfig.Channel = TMP36_ADC1_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_1;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  /**Configure Regular Channel
   */
  sConfig.Channel = VIN_ADC1_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_2;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  sConfig.Channel = TIP_TEMP_ADC1_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_3;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  sConfig.Channel = PLATE_SENSOR_ADC1_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_4;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  SET_BIT(hadc1.Instance->CR1, (ADC_CR1_EOSIE)); // Enable end of Normal
  // Run ADC internal calibration
  while (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
    ;
  }
}

/* ADC2 init function */
static void MX_ADC2_Init(void) {
  ADC_ChannelConfTypeDef sConfig;

  /**Common config
   */
  hadc2.Instance                   = ADC2;
  hadc2.Init.ScanConvMode          = ADC_SCAN_ENABLE;
  hadc2.Init.ContinuousConvMode    = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion       = ADC_CHANNELS;
  HAL_ADC_Init(&hadc2);
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

  /**Configure Regular Channel
   */
  sConfig.Channel = TMP36_ADC2_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_1;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

  sConfig.Channel = VIN_ADC2_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_2;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);
  sConfig.Channel = TIP_TEMP_ADC1_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_3;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);
  sConfig.Channel = PLATE_SENSOR_ADC2_CHANNEL;
  sConfig.Rank    = ADC_REGULAR_RANK_4;
  HAL_ADC_ConfigChannel(&hadc2, &sConfig);

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
static void MX_TIM3_Init(void) {
  TIM_ClockConfigTypeDef  sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef      sConfigOC;
  memset(&sClockSourceConfig, 0, sizeof(sClockSourceConfig));
  memset(&sMasterConfig, 0, sizeof(sMasterConfig));
  memset(&sConfigOC, 0, sizeof(sConfigOC));
  htim3.Instance               = TIM3;
  htim3.Init.Prescaler         = 1;
  htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim3.Init.Period            = 255;                           //
  htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;        // 4mhz before div
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // Preload the ARR register (though we dont use this)
  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim3);

  HAL_TIM_OC_Init(&htim3);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode     = TIM_OCMODE_PWM1;
  sConfigOC.Pulse      = 0; // Output control
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, PWM_Out_CHANNEL);
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, BUZZER_CHANNEL);
  GPIO_InitTypeDef GPIO_InitStruct;

  /**TIM3 GPIO Configuration
   PWM_Out_Pin     ------> TIM3_CH1
   */
  GPIO_InitStruct.Pin   = PWM_Out_Pin | BUZZER_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // We would like sharp rising edges
  HAL_GPIO_Init(PWM_Out_GPIO_Port, &GPIO_InitStruct);
  HAL_TIM_PWM_Start(&htim3, PWM_Out_CHANNEL);
  HAL_TIM_PWM_Start(&htim3, BUZZER_CHANNEL);
}
/* TIM3 init function */
static void MX_TIM2_Init(void) {

  TIM_ClockConfigTypeDef  sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef      sConfigOC;

  htim2.Instance       = TIM2;
  htim2.Init.Prescaler = 200; // 2 MHz timer clock/2000 = 1 kHz tick rate

  // pwm out is 10k from tim3, we want to run our PWM at around 10hz or slower on the output stage
  // These values give a rate of around 3.5 Hz for "fast" mode and 1.84 Hz for "slow"
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  // dummy value, will be reconfigured by BSPInit()
  htim2.Init.Period            = 10;
  htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1; // 8 MHz (x2 APB1) before divide
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  htim2.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  HAL_TIM_PWM_Init(&htim2);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  // dummy value, will be reconfigured by BSPInit() in the BSP.cpp
  sConfigOC.Pulse      = 5;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4);
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin   = HEAT_EN_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // We would like sharp rising edges
  HAL_GPIO_Init(HEAT_EN_GPIO_Port, &GPIO_InitStruct);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

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
  //^ remap XTAL so that pins used

  /*
   * Configure All pins as analog by default
   */
  GPIO_InitStruct.Pin  = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                        GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY_B_Pin KEY_A_Pin */
  GPIO_InitStruct.Pin  = KEY_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_B_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin  = KEY_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY_A_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OLED_RESET_Pin */
  GPIO_InitStruct.Pin   = OLED_RESET_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OLED_RESET_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin   = WS2812_Pin;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(WS2812_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(WS2812_GPIO_Port, WS2812_Pin, GPIO_PIN_RESET);
  // Pull down LCD reset
  HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(30);
  HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { asm("bkpt"); }
#endif
