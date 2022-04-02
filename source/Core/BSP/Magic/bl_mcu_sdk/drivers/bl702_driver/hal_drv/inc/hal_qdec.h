/**
 * @file hal_qdec.h
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

#ifndef __HAL_QDEC__H__
#define __HAL_QDEC__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define DEVICE_CTRL_GET_SAMPLE_VAL (0x10)
#define DEVICE_CTRL_GET_SAMPLE_DIR (0x11)
#define DEVICE_CTRL_GET_ERROR_CNT  (0x12)

enum qdec_index_type {
#ifdef BSP_USING_QDEC0
    QDEC0_INDEX,
#endif
#ifdef BSP_USING_QDEC1
    QDEC1_INDEX,
#endif
#ifdef BSP_USING_QDEC2
    QDEC2_INDEX,
#endif
    QDEC_MAX_INDEX,
};

enum qdec_event_type {
    QDEC_REPORT_EVENT = 1 << 0,   /*!< report interrupt */
    QDEC_SAMPLE_EVENT = 1 << 1,   /*!< sample interrupt */
    QDEC_ERROR_EVENT = 1 << 2,    /*!< error interrupt */
    QDEC_OVERFLOW_EVENT = 1 << 3, /*!< ACC1 and ACC2 overflow interrupt */
    QDEC_ALL_EVENT = 1 << 4,      /*!< interrupt max num */
};

typedef struct qdec_device {
    struct device parent;
    uint8_t id;

    uint8_t acc_mode;
    uint8_t sample_mode;
    uint8_t sample_period;
    uint8_t report_mode;
    uint32_t report_period;
    uint8_t led_en;
    uint8_t led_swap;
    uint16_t led_period;
    uint8_t deglitch_en;
    uint8_t deglitch_strength;

} qdec_device_t;

#define QDEC_DEV(dev) ((qdec_device_t *)dev)

int qdec_register(enum qdec_index_type index, const char *name);
#ifdef __cplusplus
}
#endif
#endif // __HAL_QDEC_H__
