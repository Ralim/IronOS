/**
 * @file hal_keyscan.h
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
#ifndef __HAL_KEYSCAN__H__
#define __HAL_KEYSCAN__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define DEVICE_CTRL_KEYSCAN_GET_KEYCODE 0x10

enum keyscan_index_type {
#ifdef BSP_USING_KEYSCAN
    KEYSCAN_INDEX,
#endif
    KEYSCAN_MAX_INDEX
};

enum col_num_type {
    COL_NUM_1 = 1,
    COL_NUM_2,
    COL_NUM_3,
    COL_NUM_4,
    COL_NUM_5,
    COL_NUM_6,
    COL_NUM_7,
    COL_NUM_8,
    COL_NUM_9,
    COL_NUM_10,
    COL_NUM_11,
    COL_NUM_12,
    COL_NUM_13,
    COL_NUM_14,
    COL_NUM_15,
    COL_NUM_16,
    COL_NUM_17,
    COL_NUM_18,
    COL_NUM_19,
    COL_NUM_20
};

enum row_num_type {
    ROW_NUM_1 = 1,
    ROW_NUM_2,
    ROW_NUM_3,
    ROW_NUM_4,
    ROW_NUM_5,
    ROW_NUM_6,
    ROW_NUM_7,
    ROW_NUM_8,
};

enum keyscan_event_type {
    KEYSCAN_EVENT_TRIG,
    KEYSCAN_EVENT_UNKNOWN
};

typedef struct keyscan_device {
    struct device parent;
    enum col_num_type col_num;
    enum row_num_type row_num;
    uint8_t deglitch_count;

} keyscan_device_t;

#define KEYSCAN_DEV(dev) ((keyscan_device_t *)dev)

int keyscan_register(enum keyscan_index_type index, const char *name);

#ifdef __cplusplus
}
#endif
#endif
