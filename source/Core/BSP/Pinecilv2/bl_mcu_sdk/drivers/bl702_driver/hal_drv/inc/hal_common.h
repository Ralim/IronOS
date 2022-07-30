/**
 * @file hal_common.h
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
#ifndef __HAL_COMMON__H__
#define __HAL_COMMON__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bl702_common.h"

void cpu_global_irq_enable(void);
void cpu_global_irq_disable(void);
void hal_por_reset(void);
void hal_system_reset(void);
void hal_cpu_reset(void);
void hal_get_chip_id(uint8_t chip_id[8]);
void hal_enter_usb_iap(void);
void hal_jump2app(uint32_t flash_offset);
int hal_get_trng_seed(void);

#ifdef __cplusplus
}
#endif

#endif
