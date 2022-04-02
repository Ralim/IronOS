/**
 * @file hal_common.c
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

#include "hal_common.h"
#include "bl602_ef_ctrl.h"
#include "bl602_romdriver.h"

volatile uint32_t nesting = 0;

void ATTR_TCM_SECTION cpu_global_irq_enable(void)
{
    nesting--;
    if (nesting == 0) {
        __enable_irq();
    }
}

void ATTR_TCM_SECTION cpu_global_irq_disable(void)
{
    __disable_irq();
    nesting++;
}

void hal_por_reset(void)
{
    RomDriver_GLB_SW_POR_Reset();
}

void hal_system_reset(void)
{
    RomDriver_GLB_SW_System_Reset();
}

void hal_cpu_reset(void)
{
    RomDriver_GLB_SW_CPU_Reset();
}

void hal_get_chip_id(uint8_t chip_id[8])
{
    chip_id[6] = 0;
    chip_id[7] = 0;
    EF_Ctrl_Read_MAC_Address(chip_id);
}
