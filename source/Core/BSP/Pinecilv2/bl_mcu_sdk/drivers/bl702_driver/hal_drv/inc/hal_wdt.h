/**
 * @file hal_wdt.h
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
#ifndef __HAL_WDT__H__
#define __HAL_WDT__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define DEVICE_CTRL_GET_WDT_COUNTER (0x10)
#define DEVICE_CTRL_RST_WDT_COUNTER (0x11)
#define DEVICE_CTRL_GET_RST_STATUS  (0x12)
#define DEVICE_CTRL_CLR_RST_STATUS  (0x13)

enum wdt_index_type {
#ifdef BSP_USING_WDT
    WDT_INDEX,
#endif
    WDT_MAX_INDEX
};

enum wdt_event_type {
    WDT_EVENT,
    WDT_EVENT_UNKNOWN
};

typedef struct wdt_device {
    struct device parent;
    uint8_t id;
    uint32_t wdt_timeout;
} wdt_device_t;

#define WDT_DEV(dev) ((wdt_device_t *)dev)

int wdt_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);
int wdt_register(enum wdt_index_type index, const char *name);
#ifdef __cplusplus
}
#endif
#endif
