/**
 * @file peripheral_config.h
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

#ifndef _PERIPHERAL_CONFIG_H_
#define _PERIPHERAL_CONFIG_H_

/* PERIPHERAL USING LIST */
#define BSP_USING_UART0
#define BSP_USING_UART1
/* ----------------------*/

/* PERIPHERAL With DMA LIST */

#define BSP_USING_DMA0_CH0
#define BSP_USING_DMA0_CH1
#define BSP_USING_DMA0_CH2
#define BSP_USING_DMA0_CH3
#define BSP_USING_DMA0_CH4
#define BSP_USING_DMA0_CH5
#define BSP_USING_DMA0_CH6
#define BSP_USING_DMA0_CH7

/* PERIPHERAL CONFIG */
#if defined(BSP_USING_UART0)
#ifndef UART0_CONFIG
#define UART0_CONFIG                 \
    {                                \
        .id = 0,                     \
        .baudrate = 2000000,         \
        .databits = UART_DATA_LEN_8, \
        .stopbits = UART_STOP_ONE,   \
        .parity = UART_PAR_NONE,     \
        .fifo_threshold = 16,        \
    }
#endif
#endif

#if defined(BSP_USING_UART1)
#ifndef UART1_CONFIG
#define UART1_CONFIG                 \
    {                                \
        .id = 1,                     \
        .baudrate = 2000000,         \
        .databits = UART_DATA_LEN_8, \
        .stopbits = UART_STOP_ONE,   \
        .parity = UART_PAR_NONE,     \
        .fifo_threshold = 64,        \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH0)
#ifndef DMA0_CH0_CONFIG
#define DMA0_CH0_CONFIG                            \
    {                                              \
        .id = 0,                                   \
        .ch = 0,                                   \
        .direction = DMA_MEMORY_TO_MEMORY,         \
        .transfer_mode = DMA_LLI_ONCE_MODE,        \
        .src_req = DMA_REQUEST_NONE,               \
        .dst_req = DMA_REQUEST_NONE,               \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .src_burst_size = DMA_BURST_1BYTE,         \
        .dst_burst_size = DMA_BURST_1BYTE,         \
        .src_width = DMA_TRANSFER_WIDTH_32BIT,     \
        .dst_width = DMA_TRANSFER_WIDTH_32BIT,     \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH1)
#ifndef DMA0_CH1_CONFIG
#define DMA0_CH1_CONFIG                            \
    {                                              \
        .id = 0,                                   \
        .ch = 1,                                   \
        .direction = DMA_MEMORY_TO_MEMORY,         \
        .transfer_mode = DMA_LLI_ONCE_MODE,        \
        .src_req = DMA_REQUEST_NONE,               \
        .dst_req = DMA_REQUEST_NONE,               \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .src_burst_size = DMA_BURST_1BYTE,         \
        .dst_burst_size = DMA_BURST_1BYTE,         \
        .src_width = DMA_TRANSFER_WIDTH_16BIT,     \
        .dst_width = DMA_TRANSFER_WIDTH_16BIT,     \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH2)
#ifndef DMA0_CH2_CONFIG
#define DMA0_CH2_CONFIG                             \
    {                                               \
        .id = 0,                                    \
        .ch = 2,                                    \
        .direction = DMA_MEMORY_TO_PERIPH,          \
        .transfer_mode = DMA_LLI_ONCE_MODE,         \
        .src_req = DMA_REQUEST_NONE,                \
        .dst_req = DMA_REQUEST_UART1_TX,            \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  \
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE, \
        .src_burst_size = DMA_BURST_1BYTE,          \
        .dst_burst_size = DMA_BURST_1BYTE,          \
        .src_width = DMA_TRANSFER_WIDTH_8BIT,       \
        .dst_width = DMA_TRANSFER_WIDTH_8BIT,       \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH3)
#ifndef DMA0_CH3_CONFIG
#define DMA0_CH3_CONFIG                             \
    {                                               \
        .id = 0,                                    \
        .ch = 3,                                    \
        .direction = DMA_MEMORY_TO_PERIPH,          \
        .transfer_mode = DMA_LLI_ONCE_MODE,         \
        .src_req = DMA_REQUEST_NONE,                \
        .dst_req = DMA_REQUEST_SPI0_TX,             \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  \
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE, \
        .src_burst_size = DMA_BURST_1BYTE,          \
        .dst_burst_size = DMA_BURST_1BYTE,          \
        .src_width = DMA_TRANSFER_WIDTH_8BIT,       \
        .dst_width = DMA_TRANSFER_WIDTH_8BIT,       \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH4)
#ifndef DMA0_CH4_CONFIG
#define DMA0_CH4_CONFIG                             \
    {                                               \
        .id = 0,                                    \
        .ch = 4,                                    \
        .direction = DMA_PERIPH_TO_MEMORY,          \
        .transfer_mode = DMA_LLI_ONCE_MODE,         \
        .src_req = DMA_REQUEST_SPI0_RX,             \
        .dst_req = DMA_REQUEST_NONE,                \
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE, \
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  \
        .src_burst_size = DMA_BURST_1BYTE,          \
        .dst_burst_size = DMA_BURST_1BYTE,          \
        .src_width = DMA_TRANSFER_WIDTH_8BIT,       \
        .dst_width = DMA_TRANSFER_WIDTH_8BIT,       \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH5)
#ifndef DMA0_CH5_CONFIG
#define DMA0_CH5_CONFIG                             \
    {                                               \
        .id = 0,                                    \
        .ch = 5,                                    \
        .direction = DMA_MEMORY_TO_PERIPH,          \
        .transfer_mode = DMA_LLI_CYCLE_MODE,        \
        .src_req = DMA_REQUEST_NONE,                \
        .dst_req = DMA_REQUEST_I2S_TX,              \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  \
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE, \
        .src_burst_size = DMA_BURST_1BYTE,          \
        .dst_burst_size = DMA_BURST_1BYTE,          \
        .src_width = DMA_TRANSFER_WIDTH_16BIT,      \
        .dst_width = DMA_TRANSFER_WIDTH_16BIT,      \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH6)
#ifndef DMA0_CH6_CONFIG
#define DMA0_CH6_CONFIG                             \
    {                                               \
        .id = 0,                                    \
        .ch = 6,                                    \
        .direction = DMA_MEMORY_TO_PERIPH,          \
        .transfer_mode = DMA_LLI_CYCLE_MODE,        \
        .src_req = DMA_REQUEST_NONE,                \
        .dst_req = DMA_REQUEST_I2S_TX,              \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,  \
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE, \
        .src_burst_size = DMA_BURST_1BYTE,          \
        .dst_burst_size = DMA_BURST_1BYTE,          \
        .src_width = DMA_TRANSFER_WIDTH_16BIT,      \
        .dst_width = DMA_TRANSFER_WIDTH_16BIT,      \
    }
#endif
#endif

#if defined(BSP_USING_DMA0_CH7)
#ifndef DMA0_CH7_CONFIG
#define DMA0_CH7_CONFIG                            \
    {                                              \
        .id = 0,                                   \
        .ch = 7,                                   \
        .direction = DMA_MEMORY_TO_MEMORY,         \
        .transfer_mode = DMA_LLI_ONCE_MODE,        \
        .src_req = DMA_REQUEST_NONE,               \
        .dst_req = DMA_REQUEST_NONE,               \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .src_burst_size = DMA_BURST_1BYTE,         \
        .dst_burst_size = DMA_BURST_1BYTE,         \
        .src_width = DMA_TRANSFER_WIDTH_32BIT,     \
        .dst_width = DMA_TRANSFER_WIDTH_32BIT,     \
    }
#endif
#endif

#endif
