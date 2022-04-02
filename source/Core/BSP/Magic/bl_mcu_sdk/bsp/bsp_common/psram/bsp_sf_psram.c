/**
 * @file bsp_sf_psram.c
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

#include "bsp_sf_psram.h"
#include "bflb_platform.h"
#include "bl702_psram.h"
#include "bl702_l1c.h"
#include "bl702_sec_eng.h"
#include "bl702_glb.h"

/* bsp sf psram private variables */

SPI_Psram_Cfg_Type apMemory1604 = {
    .readIdCmd = 0x9F,
    .readIdDmyClk = 0,
    .burstToggleCmd = 0xC0,
    .resetEnableCmd = 0x66,
    .resetCmd = 0x99,
    .enterQuadModeCmd = 0x35,
    .exitQuadModeCmd = 0xF5,
    .readRegCmd = 0xB5,
    .readRegDmyClk = 1,
    .writeRegCmd = 0xB1,
    .readCmd = 0x03,
    .readDmyClk = 0,
    .fReadCmd = 0x0B,
    .fReadDmyClk = 1,
    .fReadQuadCmd = 0xEB,
    .fReadQuadDmyClk = 3,
    .writeCmd = 0x02,
    .quadWriteCmd = 0x38,
    .pageSize = 512,
    .ctrlMode = PSRAM_SPI_CTRL_MODE,
    .driveStrength = PSRAM_DRIVE_STRENGTH_50_OHMS,
    .burstLength = PSRAM_BURST_LENGTH_512_BYTES,
};

SF_Ctrl_Cmds_Cfg cmdsCfg = {
    .cmdsEn = ENABLE,
    .burstToggleEn = ENABLE,
    .wrapModeEn = DISABLE,
    .wrapLen = SF_CTRL_WRAP_LEN_512,
};
SF_Ctrl_Psram_Cfg sfCtrlPsramCfg = {
    .owner = SF_CTRL_OWNER_SAHB,
    .padSel = SF_CTRL_PAD_SEL_DUAL_CS_SF2,
    .bankSel = SF_CTRL_SEL_PSRAM,
    .psramRxClkInvertSrc = ENABLE,
    .psramRxClkInvertSel = DISABLE,
    .psramDelaySrc = ENABLE,
    .psramClkDelay = 1,
};

/* bsp sf psram gpio init */
/**
 * @brief
 *
 * @return None
 *
 */
void ATTR_TCM_SECTION bsp_sf_psram_gpio_init(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[7];
    uint8_t i = 0;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 3;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_FLASH_PSRAM;

    gpiopins[0] = BFLB_EXTPSRAM_CLK_GPIO;
    gpiopins[1] = BFLB_EXTPSRAM_CS_GPIO;
    gpiopins[2] = BFLB_EXTPSRAM_DATA0_GPIO;
    gpiopins[3] = BFLB_EXTPSRAM_DATA1_GPIO;
    gpiopins[4] = BFLB_EXTPSRAM_DATA2_GPIO;
    gpiopins[5] = BFLB_EXTPSRAM_DATA3_GPIO;
    gpiopins[6] = BFLB_EXTFLASH_CS_GPIO;

    for (i = 0; i < sizeof(gpiopins); i++) {
        cfg.gpioPin = gpiopins[i];

        if (i == 0 || i == 1 || i == 6) {
            /*flash clk and cs is output*/
            cfg.gpioMode = GPIO_MODE_OUTPUT;
        } else {
            /*data are bidir*/
            cfg.gpioMode = GPIO_MODE_AF;
        }

        GLB_GPIO_Init(&cfg);
    }
}

/* bsp sf psram init */
/**
 * @brief
 *
 * @param sw_reset
 *
 * @return None
 *
 */
void ATTR_TCM_SECTION bsp_sf_psram_init(uint8_t sw_reset)
{
    uint8_t psramId[8] = { 0 };
    bsp_sf_psram_gpio_init();

    Psram_Init(&apMemory1604, &cmdsCfg, &sfCtrlPsramCfg);

    if (sw_reset) {
        Psram_SoftwareReset(&apMemory1604, apMemory1604.ctrlMode);
    }

    Psram_ReadId(&apMemory1604, psramId);
    Psram_Cache_Write_Set(&apMemory1604, SF_CTRL_QIO_MODE, ENABLE, DISABLE, DISABLE);
    L1C_Cache_Enable_Set(L1C_WAY_DISABLE_NONE);
}

void ATTR_TCM_SECTION bsp_sf_psram_read_id(uint8_t *data)
{
    Psram_ReadId(&apMemory1604, data);
}
