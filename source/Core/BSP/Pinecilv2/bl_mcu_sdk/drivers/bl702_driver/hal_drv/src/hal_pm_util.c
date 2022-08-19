/**
 * @file hal_pm_util.c
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

#include "bl702_romdriver.h"
#include "bl702_sf_ctrl.h"
#include "bl702_glb.h"
#include "hal_clock.h"
#include "hal_pm.h"
#include "hal_pm_util.h"

/* Cache Way Disable, will get from l1c register */
extern uint8_t cacheWayDisable;

/* PSRAM IO Configuration, will get from glb register */
extern uint32_t psramIoCfg;

/* Flash offset value, will get from sf_ctrl register */
extern uint32_t flash_offset;

extern void pm_pds31_fast_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);

extern SPI_Flash_Cfg_Type *flash_cfg;

void ATTR_PDS_RAM_SECTION pm_pds_fastboot_entry(void);

void (*hardware_recovery)(void) = NULL;

/**
 * @brief hal_pds_enter_with_time_compensation
 *
 * @param pdsLevel pds level support 0~3,31
 * @param pdsSleepCycles user set sleep time, clock of pds_time is 32768hz
 * @return uint32_t actual sleep time(ms)
 *
 * @note If necessary，please application layer call vTaskStepTick，
 */
uint32_t hal_pds_enter_with_time_compensation(uint32_t pdsLevel, uint32_t pdsSleepCycles)
{
    uint32_t rtcLowBeforeSleep = 0, rtcHighBeforeSleep = 0;
    uint32_t rtcLowAfterSleep = 0, rtcHighAfterSleep = 0;
    uint32_t actualSleepDuration_32768cycles = 0;
    uint32_t actualSleepDuration_ms = 0;

    pm_set_wakeup_callback(pm_pds_fastboot_entry);

    HBN_Get_RTC_Timer_Val(&rtcLowBeforeSleep, &rtcHighBeforeSleep);

    pm_pds31_fast_mode_enter(pdsLevel, pdsSleepCycles);

    HBN_Get_RTC_Timer_Val(&rtcLowAfterSleep, &rtcHighAfterSleep);

    CHECK_PARAM((rtcHighAfterSleep - rtcHighBeforeSleep) <= 1); // make sure sleep less than 1 hour (2^32 us > 1 hour)

    actualSleepDuration_32768cycles = (rtcLowAfterSleep - rtcLowBeforeSleep);

    actualSleepDuration_ms = (actualSleepDuration_32768cycles >> 5) - (actualSleepDuration_32768cycles >> 11) - (actualSleepDuration_32768cycles >> 12);

    // vTaskStepTick(actualSleepDuration_ms);

    return actualSleepDuration_ms;
}
/**
 * @brief get delay value of spi flash init
 *
 * @param delay_index
 * @return uint8_t
 */
static uint8_t ATTR_PDS_RAM_SECTION bflb_spi_flash_get_delay_val(uint8_t delay_index)
{
    switch (delay_index) {
        case 0:
            return 0x00;
        case 1:
            return 0x80;
        case 2:
            return 0xc0;
        case 3:
            return 0xe0;
        case 4:
            return 0xf0;
        case 5:
            return 0xf8;
        case 6:
            return 0xfc;
        case 7:
            return 0xfe;
        default:
            return 0x00;
    }
}
/**
 * @brief config spi flash
 *
 * @param pFlashCfg flash parameter
 */
static void ATTR_PDS_RAM_SECTION bflb_spi_flash_set_sf_ctrl(SPI_Flash_Cfg_Type *pFlashCfg)
{
    SF_Ctrl_Cfg_Type sfCtrlCfg;
    uint8_t delay_index;

    sfCtrlCfg.owner = SF_CTRL_OWNER_SAHB;

    /* bit0-3 for clk delay */
    sfCtrlCfg.clkDelay = (pFlashCfg->clkDelay & 0x0f);
    /* bit0 for clk invert */
    sfCtrlCfg.clkInvert = pFlashCfg->clkInvert & 0x01;
    /* bit1 for rx clk invert */
    sfCtrlCfg.rxClkInvert = (pFlashCfg->clkInvert >> 1) & 0x01;
    /* bit4-6 for do delay */
    delay_index = (pFlashCfg->clkDelay >> 4) & 0x07;
    sfCtrlCfg.doDelay = bflb_spi_flash_get_delay_val(delay_index);
    /* bit2-4 for di delay */
    delay_index = (pFlashCfg->clkInvert >> 2) & 0x07;
    sfCtrlCfg.diDelay = bflb_spi_flash_get_delay_val(delay_index);
    /* bit5-7 for oe delay */
    delay_index = (pFlashCfg->clkInvert >> 5) & 0x07;
    sfCtrlCfg.oeDelay = bflb_spi_flash_get_delay_val(delay_index);

    RomDriver_SFlash_Init(&sfCtrlCfg);
}

/**
 * @brief
 *
 * @param media_boot
 * @return int32_t
 */
int32_t ATTR_PDS_RAM_SECTION pm_spi_flash_init(uint8_t media_boot)
{
    uint32_t stat;
    uint32_t jdecId = 0;
    uint32_t flash_read_try = 0;

    /*use fclk as flash clok */
    RomDriver_GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_XCLK, 0); // 32M
    RomDriver_SF_Ctrl_Set_Clock_Delay(0);

    bflb_spi_flash_set_sf_ctrl(flash_cfg);

    /* Wake flash up from power down */
    RomDriver_SFlash_Releae_Powerdown(flash_cfg);
    //ARCH_Delay_US(15*((pFlashCfg->pdDelay&0x7)+1));
    RomDriver_BL702_Delay_US(120);

    do {
        if (flash_read_try > 4) {
            // bflb_bootrom_printd("Flash read id TO\r\n");
            break;
        } else if (flash_read_try > 0) {
            RomDriver_BL702_Delay_US(500);
        }

        // bflb_bootrom_printd("reset flash\r\n");
        /* Exit form continous read for accepting command */
        RomDriver_SFlash_Reset_Continue_Read(flash_cfg);
        /* Send software reset command(80bv has no this command)to deburst wrap for ISSI like */
        RomDriver_SFlash_Software_Reset(flash_cfg);
        /* Disable burst may be removed(except for 80BV) and only work with winbond,but just for make sure */
        RomDriver_SFlash_Write_Enable(flash_cfg);
        /* For disable command that is setting register instaed of send command, we need write enable */
        RomDriver_SFlash_DisableBurstWrap(flash_cfg);

        stat = RomDriver_SFlash_SetSPIMode(SF_CTRL_SPI_MODE);
        if (SUCCESS != stat) {
            // bflb_bootrom_printe("enter spi mode fail %d\r\n", stat);
            // return BFLB_BOOTROM_FLASH_INIT_ERROR;
            return -1;
        }

        RomDriver_SFlash_GetJedecId(flash_cfg, (uint8_t *)&jdecId);

        /* Dummy disable burstwrap for make sure */
        RomDriver_SFlash_Write_Enable(flash_cfg);
        /* For disable command that is setting register instead of send command, we need write enable */
        RomDriver_SFlash_DisableBurstWrap(flash_cfg);

        jdecId = jdecId & 0xffffff;
        // bflb_bootrom_printd("ID =%08x\r\n", jdecId);
        flash_read_try++;
    } while ((jdecId & 0x00ffff) == 0 || (jdecId & 0xffff00) == 0 || (jdecId & 0x00ffff) == 0xffff || (jdecId & 0xffff00) == 0xffff00);

    /*clear offset setting*/

    // reset image offset
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_ID0_OFFSET, flash_offset);

    /* set read mode */
    if ((flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        stat = RomDriver_SFlash_Qspi_Enable(flash_cfg);
    }

    if (media_boot) {
        RomDriver_L1C_Set_Wrap(DISABLE);

        RomDriver_SFlash_Cache_Read_Enable(flash_cfg, flash_cfg->ioMode & 0xf, 0, 0x00);
    }

    return jdecId;
}

// can be placed in flash, here placed in pds section to reduce fast boot time
static void ATTR_PDS_RAM_SECTION pm_pds_restore_cpu_reg(void)
{
    __asm__ __volatile__(
        "la     a2,     __ld_pds_bak_addr\n\t"
        "lw     ra,     0(a2)\n\t"
        "lw     sp,     1*4(a2)\n\t"
        "lw     tp,     2*4(a2)\n\t"
        "lw     t0,     3*4(a2)\n\t"
        "lw     t1,     4*4(a2)\n\t"
        "lw     t2,     5*4(a2)\n\t"
        "lw     fp,     6*4(a2)\n\t"
        "lw     s1,     7*4(a2)\n\t"
        "lw     a0,     8*4(a2)\n\t"
        "lw     a1,     9*4(a2)\n\t"
        "lw     a3,     10*4(a2)\n\t"
        "lw     a4,     11*4(a2)\n\t"
        "lw     a5,     12*4(a2)\n\t"
        "lw     a6,     13*4(a2)\n\t"
        "lw     a7,     14*4(a2)\n\t"
        "lw     s2,     15*4(a2)\n\t"
        "lw     s3,     16*4(a2)\n\t"
        "lw     s4,     17*4(a2)\n\t"
        "lw     s5,     18*4(a2)\n\t"
        "lw     s6,     19*4(a2)\n\t"
        "lw     s7,     20*4(a2)\n\t"
        "lw     s8,     21*4(a2)\n\t"
        "lw     s9,     22*4(a2)\n\t"
        "lw     s10,    23*4(a2)\n\t"
        "lw     s11,    24*4(a2)\n\t"
        "lw     t3,     25*4(a2)\n\t"
        "lw     t4,     26*4(a2)\n\t"
        "lw     t5,     27*4(a2)\n\t"
        "lw     t6,     28*4(a2)\n\t"
        "csrw   mtvec,  a0\n\t"
        "csrw   mstatus,a1\n\t"
        "ret\n\t");
}
void ATTR_PDS_RAM_SECTION sf_io_select(void)
{
    uint32_t tmpVal = 0;
    uint8_t flashCfg = 0;
    uint8_t psramCfg = 0;
    uint8_t isInternalFlash = 0;
    uint8_t isInternalPsram = 0;

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
}
// must be placed in pds section
void ATTR_PDS_RAM_SECTION pm_pds_fastboot_entry(void)
{
    // reload gp register
    __asm__ __volatile__(
        ".option push\n\t"
        ".option norelax\n\t"
        "la gp, __global_pointer$\n\t"
        ".option pop\n\t");

#if XTAL_TYPE != INTERNAL_RC_32M
    /* power on Xtal_32M*/
    (*(volatile uint32_t *)(AON_BASE + AON_RF_TOP_AON_OFFSET)) |= (3 << 4);
#endif

    // recovery flash pad and param
    RomDriver_SF_Cfg_Init_Flash_Gpio(0, 1);
    pm_spi_flash_init(1);
    sf_io_select();

    /* Recovery hardware , include tcm , gpio and clock */
    if (hardware_recovery) {
        hardware_recovery();
    }

    // Restore cpu registers
    pm_pds_restore_cpu_reg();
}

void pm_set_hardware_recovery_callback(void (*hardware_recovery_cb)(void))
{
    hardware_recovery = hardware_recovery_cb;
}