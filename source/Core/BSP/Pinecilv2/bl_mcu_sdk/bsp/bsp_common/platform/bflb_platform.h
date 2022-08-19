/**
 * @file bflb_platform.h
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

#ifndef _BFLB_PLATFORM_H
#define _BFLB_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#define MSG(a, ...)     bflb_platform_printf(a, ##__VA_ARGS__)
#define MSG_DBG(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#define MSG_ERR(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#define BL_CASE_FAIL          \
    {                         \
        MSG("case fail\r\n"); \
    }
#define BL_CASE_SUCCESS          \
    {                            \
        MSG("case success\r\n"); \
    }

/* compatible with old version */
#ifndef DBG_TAG
#define DBG_TAG "DEBUG"
#endif

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */
#define _DBG_COLOR(n) bflb_platform_printf("\033[" #n "m")
#define _DBG_LOG_HDR(lvl_name, color_n) \
    bflb_platform_printf("\033[" #color_n "m[" lvl_name "/" DBG_TAG "] ")
#define _DBG_LOG_X_END \
    bflb_platform_printf("\033[0m\n")

#define dbg_log_line(lvl, color_n, fmt, ...)      \
    do {                                          \
        _DBG_LOG_HDR(lvl, color_n);               \
        bflb_platform_printf(fmt, ##__VA_ARGS__); \
        _DBG_LOG_X_END;                           \
    } while (0)

#define LOG_D(fmt, ...) dbg_log_line("D", 0, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) dbg_log_line("I", 35, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...) dbg_log_line("W", 33, fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#define LOG_RAW(...)    bflb_platform_printf(__VA_ARGS__)

void bflb_platform_init(uint32_t baudrate);
void bflb_platform_printf(char *fmt, ...);
void bflb_platform_print_set(uint8_t disable);
uint8_t bflb_platform_print_get(void);
void bflb_platform_dump(uint8_t *data, uint32_t len);
void bflb_platform_reg_dump(uint32_t addr);
uint32_t bflb_platform_get_log(uint8_t *data, uint32_t maxlen);
void bflb_platform_deinit(void);

void bflb_platform_init_time(void);
void bflb_platform_clear_time(void);
uint64_t bflb_platform_get_time_ms(void);
uint64_t bflb_platform_get_time_us(void);
void bflb_platform_start_time(void);
void bflb_platform_stop_time(void);
void bflb_platform_set_alarm_time(uint64_t time, void (*interruptFun)(void));
void bflb_platform_deinit_time(void);
void bflb_platform_delay_ms(uint32_t ms);
void bflb_platform_delay_us(uint32_t us);

void bflb_print_device_list(void);
#ifdef __cplusplus
}
#endif

#endif