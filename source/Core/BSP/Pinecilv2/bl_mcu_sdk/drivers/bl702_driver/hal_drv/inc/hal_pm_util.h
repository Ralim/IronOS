/**
 * @file hal_pm_util.h
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
#ifndef __HAL_PM_UTIL_H__
#define __HAL_PM_UTIL_H__

#include "hal_common.h"

#define ATTR_PDS_RAM_SECTION       __attribute__((section(".pds_ram_code")))
#define ATTR_PDS_RAM_CONST_SECTION __attribute__((section(".pds_ram_data")))

uint32_t hal_pds_enter_with_time_compensation(uint32_t pdsLevel, uint32_t pdsSleepCycles);
void pm_set_hardware_recovery_callback(void (*hardware_recovery_cb)(void));
#endif