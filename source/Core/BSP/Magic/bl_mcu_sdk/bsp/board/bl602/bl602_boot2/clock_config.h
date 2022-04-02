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

#define XTAL_TYPE             EXTERNAL_XTAL_40M
#define XTAL_32K_TYPE         INTERNAL_RC_32K
#define BSP_ROOT_CLOCK_SOURCE ROOT_CLOCK_SOURCE_PLL_160M

#define BSP_FCLK_DIV 0
#define BSP_BCLK_DIV 1

#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1)
#define BSP_UART_CLOCK_SOURCE ROOT_CLOCK_SOURCE_PLL_160M
#define BSP_UART_CLOCK_DIV    3
#endif
#if defined(BSP_USING_I2C0)
#define BSP_I2C_CLOCK_SOURCE ROOT_CLOCK_SOURCE_BCLK
#define BSP_I2C_CLOCK_DIV    0
#endif
#if defined(BSP_USING_SPI0)
#define BSP_SPI_CLOCK_SOURCE ROOT_CLOCK_SOURCE_BCLK
#define BSP_SPI_CLOCK_DIV    0
#endif
#if defined(BSP_USING_ADC0)
#define BSP_ADC_CLOCK_SOURCE ROOT_CLOCK_SOURCE_XCLK
#define BSP_ADC_CLOCK_DIV    0
#endif
#if defined(BSP_USING_DAC0)
#define BSP_DAC_CLOCK_SOURCE ROOT_CLOCK_SOURCE_AUPLL
#define BSP_DAC_CLOCK_DIV    1
#endif

#endif