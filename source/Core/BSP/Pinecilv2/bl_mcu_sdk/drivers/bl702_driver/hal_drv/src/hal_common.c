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
#include "bl702_ef_ctrl.h"
#include "bl702_romdriver.h"
#include "bl702_sec_eng.h"
#include "bl702_l1c.h"
#include "hbn_reg.h"

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
    EF_Ctrl_Read_MAC_Address(chip_id);
}

void hal_enter_usb_iap(void)
{
    BL_WR_WORD(HBN_BASE + HBN_RSV0_OFFSET, 0x00425355); //"\0BSU"

    arch_delay_ms(1000);
    RomDriver_GLB_SW_POR_Reset();
}

void ATTR_TCM_SECTION hal_jump2app(uint32_t flash_offset)
{
    /*flash_offset from 48K to 3.98M*/
    if ((flash_offset >= 0xc000) && (flash_offset < (0x400000 - 20 * 1024))) {
        void (*app_main)(void) = (void (*)(void))0x23000000;
        BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_ID0_OFFSET, flash_offset);
        L1C_Cache_Flush_Ext();
        app_main();
    } else {
        while(1)
        {}
    }
}

int hal_get_trng_seed(void)
{
    uint32_t seed[8];
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    tmpVal |= (1 << BL_AHB_SLAVE1_SEC);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

    Sec_Eng_Trng_Enable();
    Sec_Eng_Trng_Read((uint8_t *)seed);
    Sec_Eng_Trng_Disable();
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    tmpVal &= (~(1 << BL_AHB_SLAVE1_SEC));
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

    return seed[0];
}