/**
 * @file hal_clock.h
 * @brief
 *
 * Copyright 2019-2030 Bouffalolab team
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

#include "hal_common.h"
#include "bl602_config.h"

/*XTAL_TYPE*/
#define XTAL_NONE           0
#define EXTERNAL_XTAL_24M   1
#define EXTERNAL_XTAL_32M   2
#define EXTERNAL_XTAL_38P4M 3
#define EXTERNAL_XTAL_40M   4
#define EXTERNAL_XTAL_26M   5
#define INTERNAL_RC_32M     6

/*CLOCK_32K_XTAL*/
#define EXTERNAL_XTAL_32K 1
#define INTERNAL_RC_32K   0

/*BSP_ROOT_CLOCK_SOURCE*/
#if (XTAL_TYPE == INTERNAL_RC_32M) || (XTAL_TYPE == XTAL_NONE)
#define ROOT_CLOCK_SOURCE_XCLK 0
#else
#define ROOT_CLOCK_SOURCE_XCLK 1
#endif
#define ROOT_CLOCK_SOURCE_PLL_48M  2
#define ROOT_CLOCK_SOURCE_PLL_120M 3
#define ROOT_CLOCK_SOURCE_PLL_160M 4
#define ROOT_CLOCK_SOURCE_PLL_192M 5

/*BSP_XXX_CLOCK_SOURCE*/
#define ROOT_CLOCK_SOURCE_FCLK 6
#define ROOT_CLOCK_SOURCE_BCLK 7

enum system_clock_type {
    SYSTEM_CLOCK_ROOT_CLOCK = 0,
    SYSTEM_CLOCK_FCLK,
    SYSTEM_CLOCK_BCLK,
    SYSTEM_CLOCK_XCLK,
};
enum peripheral_clock_type {
    PERIPHERAL_CLOCK_UART = 0,
    PERIPHERAL_CLOCK_SPI,
    PERIPHERAL_CLOCK_I2C,
    PERIPHERAL_CLOCK_ADC,
    PERIPHERAL_CLOCK_DAC,
};

void system_clock_init(void);
void system_mtimer_clock_init(void);
void system_mtimer_clock_reinit(void);
void peripheral_clock_init(void);
uint32_t system_clock_get(enum system_clock_type type);
uint32_t peripheral_clock_get(enum peripheral_clock_type type);
#endif