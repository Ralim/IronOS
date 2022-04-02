/**
 * @file hal_timer.h
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
#ifndef __HAL_TIMER__H__
#define __HAL_TIMER__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

enum timer_index_type {
#ifdef BSP_USING_TIMER0
    TIMER0_INDEX,
#endif
#ifdef BSP_USING_TIMER1
    TIMER1_INDEX,
#endif
    TIMER_MAX_INDEX
};

enum timer_preload_trigger_type {
    TIMER_PRELOAD_TRIGGER_NONE,
    TIMER_PRELOAD_TRIGGER_COMP0,
    TIMER_PRELOAD_TRIGGER_COMP1,
    TIMER_PRELOAD_TRIGGER_COMP2,
};

enum timer_cnt_mode_type {
    TIMER_CNT_PRELOAD,
    TIMER_CNT_FREERUN,
};

enum timer_compare_id_type {
    TIMER_COMPARE_ID_0,
    TIMER_COMPARE_ID_1,
    TIMER_COMPARE_ID_2,
};

enum timer_it_type {
    TIMER_COMP0_IT = 1 << 0,
    TIMER_COMP1_IT = 1 << 1,
    TIMER_COMP2_IT = 1 << 2,
};

enum timer_event_type {
    TIMER_EVENT_COMP0,
    TIMER_EVENT_COMP1,
    TIMER_EVENT_COMP2,
    TIMER_EVENT_UNKNOWN
};

typedef struct timer_timeout_cfg {
    enum timer_compare_id_type timeout_id;
    uint32_t timeout_val;
} timer_timeout_cfg_t;

typedef struct timer_device {
    struct device parent;
    uint8_t id;
    enum timer_cnt_mode_type cnt_mode;
    enum timer_preload_trigger_type trigger;
    uint32_t reload;
    uint32_t timeout1;
    uint32_t timeout2;
    uint32_t timeout3;
} timer_device_t;

#define TIMER_DEV(dev) ((timer_device_t *)dev)

int timer_register(enum timer_index_type index, const char *name);
#ifdef __cplusplus
}
#endif
#endif