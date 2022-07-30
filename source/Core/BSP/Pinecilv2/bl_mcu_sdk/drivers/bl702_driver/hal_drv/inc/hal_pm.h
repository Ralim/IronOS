/**
 * @file hal_pm.h
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
#ifndef __HAL_PM__H__
#define __HAL_PM__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"

enum pm_pds_sleep_level {
    PM_PDS_LEVEL_0,
    PM_PDS_LEVEL_1,
    PM_PDS_LEVEL_2,
    PM_PDS_LEVEL_3,
    PM_PDS_LEVEL_4, /*do not recommend to use*/
    PM_PDS_LEVEL_5, /*do not recommend to use*/
    PM_PDS_LEVEL_6, /*do not recommend to use*/
    PM_PDS_LEVEL_7, /*do not recommend to use*/
    PM_PDS_LEVEL_31 = 31,
};

enum pm_hbn_sleep_level {
    PM_HBN_LEVEL_0,
    PM_HBN_LEVEL_1,
    PM_HBN_LEVEL_2,
};

enum pm_event_type {
    PM_HBN_WAKEUP_EVENT_NONE,
    PM_HBN_GPIO9_WAKEUP_EVENT,
    PM_HBN_GPIO10_WAKEUP_EVENT,
    PM_HBN_GPIO11_WAKEUP_EVENT,
    PM_HBN_GPIO12_WAKEUP_EVENT,
    PM_HBN_RTC_WAKEUP_EVENT,
    PM_HBN_BOR_WAKEUP_EVENT,
    PM_HBN_ACOMP0_WAKEUP_EVENT,
    PM_HBN_ACOMP1_WAKEUP_EVENT,
};

void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);
void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint8_t sleep_time);
void pm_hbn_enter_again(bool reset);
void pm_set_wakeup_callback(void (*wakeup_callback)(void));
enum pm_event_type pm_get_wakeup_event(void);
void pm_bor_init(void);
void pm_hbn_out0_irq_register(void);
void pm_hbn_out1_irq_register(void);
void pm_irq_callback(enum pm_event_type event);

#ifdef __cplusplus
}
#endif
#endif