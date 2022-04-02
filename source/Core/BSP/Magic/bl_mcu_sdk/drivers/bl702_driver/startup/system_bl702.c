/**
 * @file system_bl702.c
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
#include "bl702.h"
#include "bl702_glb.h"
#include "bl702_hbn.h"
#include "risc-v/Core/Include/clic.h"

#ifdef BFLB_EFLASH_LOADER
#include "bl702_usb.h"
void USB_DoNothing_IRQHandler(void)
{
    /* clear all USB int sts */
    USB_Clr_IntStatus(32);
}
#endif

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define SYSTEM_CLOCK (32000000UL)

/*----------------------------------------------------------------------------
  Vector Table
 *----------------------------------------------------------------------------*/
#define VECT_TAB_OFFSET 0x00 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x200. */

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/

void system_bor_init(void)
{
    HBN_BOR_CFG_Type borCfg = { 0 /* pu_bor */, 0 /* irq_bor_en */, 1 /* bor_vth */, 0 /* bor_sel */ };
    HBN_Set_BOR_Cfg(&borCfg);
}

void SystemInit(void)
{
    uint32_t *p;
    uint32_t i = 0;
    uint32_t tmpVal = 0;
    uint8_t flashCfg = 0;
    uint8_t psramCfg = 0;
    uint8_t isInternalFlash = 0;
    uint8_t isInternalPsram = 0;

    /* global IRQ disable */
    __disable_irq();

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal |= (1 << 8);      /*mask pds wakeup*/
    tmpVal |= (1 << 10);     /*mask rf done*/
    tmpVal |= (1 << 11);     /*mask pll done*/
    tmpVal &= ~(0xff << 16); /*mask all pds wakeup source int*/
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    /* GLB_Set_EM_Sel(GLB_EM_0KB); */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SEAM_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, GLB_EM_0KB);
    BL_WR_REG(GLB_BASE, GLB_SEAM_MISC, tmpVal);

    /* Restore default setting*/
    /* GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_NONE); */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_UART_SWAP_SET, UART_SIG_SWAP_NONE);
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    /* fix 57.6M */
    if (SystemCoreClockGet() == 57 * 6000 * 1000) {
        SystemCoreClockSet(57.6 * 1000 * 1000)
    }

    /* CLear all interrupt */
    p = (uint32_t *)(CLIC_HART0_ADDR + CLIC_INTIE);

    for (i = 0; i < (IRQn_LAST + 3) / 4; i++) {
        p[i] = 0;
    }

    p = (uint32_t *)(CLIC_HART0_ADDR + CLIC_INTIP);

    for (i = 0; i < (IRQn_LAST + 3) / 4; i++) {
        p[i] = 0;
    }

    /* SF io select from efuse value */
    tmpVal = BL_RD_WORD(0x40007074);
    flashCfg = ((tmpVal >> 26) & 7);
    psramCfg = ((tmpVal >> 24) & 3);
    if (flashCfg == 1 || flashCfg == 2) {
        isInternalFlash = 1;
    } else {
        isInternalFlash = 0;
    }
    if (psramCfg == 1) {
        isInternalPsram = 1;
    } else {
        isInternalPsram = 0;
    }
    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_USE_PSRAM__IO);
    if (isInternalFlash == 1 && isInternalPsram == 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_GPIO_USE_PSRAM_IO, 0x3f);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_GPIO_USE_PSRAM_IO, 0x00);
    }
    BL_WR_REG(GLB_BASE, GLB_GPIO_USE_PSRAM__IO, tmpVal);

#ifdef BFLB_EFLASH_LOADER
    Interrupt_Handler_Register(USB_IRQn, USB_DoNothing_IRQHandler);
#endif
    /* init bor for all platform */
    system_bor_init();
    /* global IRQ enable */
    __enable_irq();
}

void System_Post_Init(void)
{
    PDS_Trim_RC32M();
    HBN_Trim_RC32K();
}