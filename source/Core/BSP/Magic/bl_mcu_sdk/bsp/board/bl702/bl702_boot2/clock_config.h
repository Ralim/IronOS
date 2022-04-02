/**
 * @file clock_config.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#ifndef _CLOCK_CONFIG_H
#define _CLOCK_CONFIG_H

#define XTAL_TYPE                  EXTERNAL_XTAL_32M
#define XTAL_32K_TYPE              INTERNAL_RC_32K
#define BSP_ROOT_CLOCK_SOURCE      ROOT_CLOCK_SOURCE_PLL_144M
#define BSP_AUDIO_PLL_CLOCK_SOURCE ROOT_CLOCK_SOURCE_AUPLL_24000000_HZ

#define BSP_FCLK_DIV 0
#define BSP_BCLK_DIV 1

#if defined(BSP_ROOT_CLOCK_SOURCE)
#define BSP_HCLK_DIV 0
#define BSP_BCLK_DIV 1
#endif

#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1)
#define BSP_UART_CLOCK_SOURCE ROOT_CLOCK_SOURCE_PLL_96M
#define BSP_UART_CLOCK_DIV    0
#endif
#if defined(BSP_USING_I2C0)
#define BSP_I2C_CLOCK_SOURCE ROOT_CLOCK_SOURCE_BCLK
#define BSP_I2C_CLOCK_DIV    9
#endif
#if defined(BSP_USING_SPI0)
#define BSP_SPI_CLOCK_SOURCE ROOT_CLOCK_SOURCE_BCLK
#define BSP_SPI_CLOCK_DIV    0
#endif
#if defined(BSP_USING_TIMER0)
#define BSP_TIMER0_CLOCK_SOURCE ROOT_CLOCK_SOURCE_FCLK
#define BSP_TIMER0_CLOCK_DIV    0
#endif
#if defined(BSP_USING_TIMER1)
#define BSP_TIMER1_CLOCK_SOURCE ROOT_CLOCK_SOURCE_FCLK
#define BSP_TIMER1_CLOCK_DIV    0
#endif
#if defined(BSP_USING_PWM_CH0) || defined(BSP_USING_PWM_CH1) || defined(BSP_USING_PWM_CH2) || defined(BSP_USING_PWM_CH3) || defined(BSP_USING_PWM_CH4) || defined(BSP_USING_PWM_CH5)
#define BSP_PWM_CLOCK_SOURCE ROOT_CLOCK_SOURCE_XCLK
#define BSP_PWM_CLOCK_DIV    0
#endif
#if defined(BSP_USING_IR)
#define BSP_IR_CLOCK_SOURCE ROOT_CLOCK_SOURCE_XCLK
#define BSP_IR_CLOCK_DIV    0
#endif
#if defined(BSP_USING_I2S0)
#define BSP_I2S_CLOCK_SOURCE ROOT_CLOCK_SOURCE_XCLK
#define BSP_I2S_CLOCK_DIV    0
#endif
#if defined(BSP_USING_ADC0)
#define BSP_ADC_CLOCK_SOURCE ROOT_CLOCK_SOURCE_XCLK
#define BSP_ADC_CLOCK_DIV    0
#endif
#if defined(BSP_USING_DAC0)
#define BSP_DAC_CLOCK_SOURCE ROOT_CLOCK_SOURCE_AUPLL_24000000_HZ
#define BSP_DAC_CLOCK_DIV    2
#endif
#if defined(BSP_USING_CAM)
#define BSP_CAM_CLOCK_SOURCE ROOT_CLOCK_SOURCE_PLL_96M
#define BSP_CAM_CLOCK_DIV    3
#endif
#if defined(BSP_USING_QDEC0) || defined(BSP_USING_QDEC1) || defined(BSP_USING_QDEC2) || defined(BSP_USING_KEYSCAN)
#define BSP_QDEC_KEYSCAN_CLOCK_SOURCE ROOT_CLOCK_SOURCE_XCLK
#define BSP_QDEC_KEYSCAN_CLOCK_DIV    31
#endif

#endif