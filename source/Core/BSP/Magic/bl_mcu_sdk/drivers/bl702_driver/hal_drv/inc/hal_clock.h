/**
 * @file hal_clock.h
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
#ifndef __HAL_CLOCK__H__
#define __HAL_CLOCK__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "bl702_config.h"

/*XTAL_TYPE*/
#define XTAL_NONE         0
#define EXTERNAL_XTAL_32M 1
#define INTERNAL_RC_32M   2

/*CLOCK_32K_XTAL*/
#define EXTERNAL_XTAL_32K 0
#define INTERNAL_RC_32K   1

/*BSP_ROOT_CLOCK_SOURCE*/
#if XTAL_TYPE != EXTERNAL_XTAL_32M
#define ROOT_CLOCK_SOURCE_XCLK 0
#else
#define ROOT_CLOCK_SOURCE_XCLK 1
#endif
#define ROOT_CLOCK_SOURCE_PLL_57P6M 2
#define ROOT_CLOCK_SOURCE_PLL_96M   3
#define ROOT_CLOCK_SOURCE_PLL_144M  4
/*BSP_XXX_CLOCK_SOURCE*/
#define ROOT_CLOCK_SOURCE_32K_CLK 5
#define ROOT_CLOCK_SOURCE_FCLK    6
#define ROOT_CLOCK_SOURCE_BCLK    7
#define ROOT_CLOCK_SOURCE_1K_CLK  8

/*BSP_AUDIO_PLL_CLOCK_SOURCE*/
#define ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ 9
#define ROOT_CLOCK_SOURCE_AUPLL_11289600_HZ 10
#define ROOT_CLOCK_SOURCE_AUPLL_5644800_HZ  11
#define ROOT_CLOCK_SOURCE_AUPLL_24576000_HZ 12
#define ROOT_CLOCK_SOURCE_AUPLL_24000000_HZ 13

enum system_clock_type {
    SYSTEM_CLOCK_ROOT_CLOCK = 0, /* clock source before fclk_div*/
    SYSTEM_CLOCK_FCLK,           /* clock source after fclk_div*/
    SYSTEM_CLOCK_BCLK,           /* clock source after bclk_div*/
    SYSTEM_CLOCK_XCLK,           /* xtal clock*/
    SYSTEM_CLOCK_32K_CLK,
    SYSTEM_CLOCK_AUPLL,
};
enum peripheral_clock_type {
    PERIPHERAL_CLOCK_UART = 0,
    PERIPHERAL_CLOCK_SPI,
    PERIPHERAL_CLOCK_I2C,
    PERIPHERAL_CLOCK_ADC,
    PERIPHERAL_CLOCK_DAC,
    PERIPHERAL_CLOCK_I2S,
    PERIPHERAL_CLOCK_PWM,
    PERIPHERAL_CLOCK_CAM,
    PERIPHERAL_CLOCK_TIMER0,
    PERIPHERAL_CLOCK_TIMER1,
    PERIPHERAL_CLOCK_WDT,
};

void system_clock_init(void);
void peripheral_clock_init(void);
uint32_t system_clock_get(enum system_clock_type type);
uint32_t peripheral_clock_get(enum peripheral_clock_type type);
void system_mtimer_clock_init(void);
void system_mtimer_clock_reinit(void);
#ifdef __cplusplus
}
#endif
#endif