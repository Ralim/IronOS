/**
 * @file hal_pm.c
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
#include "bl702_sflash.h"
#include "bl702_glb.h"
#include "hal_pm.h"
#include "hal_clock.h"
#include "hal_rtc.h"
#include "hal_flash.h"
#include "risc-v/Core/Include/clic.h"

/* Cache Way Disable, will get from l1c register */
uint8_t cacheWayDisable = 0;

/* PSRAM IO Configuration, will get from glb register */
uint32_t psramIoCfg = 0;

/* Flash offset value, will get from sf_ctrl register */
uint32_t flash_offset = 0;

SPI_Flash_Cfg_Type *flash_cfg;

#define PM_PDS_FLASH_POWER_OFF   1
#define PM_PDS_DLL_POWER_OFF     1
#define PM_PDS_PLL_POWER_OFF     1
#define PM_PDS_RF_POWER_OFF      1
#define PM_PDS_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_1P10V
#define PM_HBN_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_0P90V

void HBN_OUT0_IRQ(void);
void HBN_OUT1_IRQ(void);

/** @defgroup  Hal_Power_Global_Variables
 *
 *  @brief PDS level config
 *  @{
 */
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel0 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 3,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 0,
        .BzIsoEn = 0,
        .BleIsoEn = 1,
        .UsbIsoEn = 0,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 0,
        .cpuRst = 0,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 0,
        .BzRst = 0,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 0,
        .UsbRst = 0,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel1 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 3,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 0,
        .BzIsoEn = 0,
        .BleIsoEn = 1,
        .UsbIsoEn = 1,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 0,
        .cpuRst = 0,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 0,
        .BzRst = 0,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 1,
        .UsbRst = 1,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel2 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 2,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 0,
        .BzIsoEn = 1,
        .BleIsoEn = 1,
        .UsbIsoEn = 0,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 0,
        .cpuRst = 0,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 1,
        .BzRst = 1,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 0,
        .UsbRst = 0,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel3 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 2,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 0,
        .BzIsoEn = 1,
        .BleIsoEn = 1,
        .UsbIsoEn = 1,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 0,
        .cpuRst = 0,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 1,
        .BzRst = 1,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 1,
        .UsbRst = 1,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
#if 0
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel4 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 3,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 1,
        .BzIsoEn = 0,
        .BleIsoEn = 1,
        .UsbIsoEn = 0,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 0,
        .BzRst = 0,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 0,
        .UsbRst = 0,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel5 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 3,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 1,
        .BzIsoEn = 0,
        .BleIsoEn = 1,
        .UsbIsoEn = 1,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 0,
        .BzRst = 0,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 1,
        .UsbRst = 1,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel6 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 2,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 1,
        .BzIsoEn = 1,
        .BleIsoEn = 1,
        .UsbIsoEn = 0,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 1,
        .BzRst = 1,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 0,
        .UsbRst = 0,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel7 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 2,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 1,
        .BzIsoEn = 1,
        .BleIsoEn = 1,
        .UsbIsoEn = 1,
        .MiscIsoEn = 0,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 1,
        .BzRst = 1,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 1,
        .UsbRst = 1,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 0,
        .MiscRst = 0,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};
#endif
static PDS_DEFAULT_LV_CFG_Type ATTR_TCM_CONST_SECTION pdsCfgLevel31 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .saveWifiState = 0,
        .dcdc18Off = 1,
        .bgSysOff = 1,
        .gpioIePuPd = 1,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 0,
        .socEnbForceOn = 0,
        .pdsRstSocEn = 0,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 0,
        .pdsRamLowPowerWithClkEn = 1,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsForceRamClkEn = 0,
        .pdsLdoVol = 0xA,
        .pdsCtlRfSel = 2,
        .pdsCtlPllSel = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff = 0,
        .forceBzPwrOff = 0,
        .forceUsbPwrOff = 0,
        .forceCpuIsoEn = 0,
        .forceBzIsoEn = 0,
        .forceUsbIsoEn = 0,
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceUsbPdsRst = 0,
        .forceCpuMemStby = 0,
        .forceBzMemStby = 0,
        .forceUsbMemStby = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
        .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceBlePwrOff = 0,
        .forceBleIsoEn = 0,
        .forceMiscPdsRst = 0,
        .forceBlePdsRst = 0,
        .forceMiscMemStby = 0,
        .forceBleMemStby = 0,
        .forceMiscGateClk = 0,
        .forceBleGateClk = 0,
        .CpuIsoEn = 1,
        .BzIsoEn = 1,
        .BleIsoEn = 1,
        .UsbIsoEn = 1,
        .MiscIsoEn = 1,
    },
    .pdsCtl4 = {
        .cpuPwrOff = 1,
        .cpuRst = 1,
        .cpuMemStby = 1,
        .cpuGateClk = 1,
        .BzPwrOff = 1,
        .BzRst = 1,
        .BzMemStby = 1,
        .BzGateClk = 1,
        .BlePwrOff = 1,
        .BleRst = 1,
        .BleMemStby = 1,
        .BleGateClk = 1,
        .UsbPwrOff = 1,
        .UsbRst = 1,
        .UsbMemStby = 1,
        .UsbGateClk = 1,
        .MiscPwrOff = 1,
        .MiscRst = 1,
        .MiscMemStby = 1,
        .MiscGateClk = 1,
        .MiscAnaPwrOff = 1,
        .MiscDigPwrOff = 1,
    }
};

/****************************************************************************/ /**
 * @brief  PDS update flash_ctrl setting
 *
 * @param  fastClock: fast clock
 *
 * @return None
 *
*******************************************************************************/
static ATTR_TCM_SECTION void PDS_Update_Flash_Ctrl_Setting(uint8_t fastClock)
{
    if (fastClock) {
        GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_72M, 0);
    } else {
        GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_XCLK, 0);
    }

    SF_Ctrl_Set_Clock_Delay(fastClock);
}

ATTR_TCM_SECTION void pm_pds_enter_done(void)
{
    __asm__ __volatile__(
        "la     a2,     __ld_pds_bak_addr\n\t"
        "sw     ra,     0(a2)\n\t");

    __WFI(); /* if(.wfiMask==0){CPU won't power down until PDS module had seen __wfi} */
}
/**
 * @brief power management in pds(power down sleep) mode
 *
 * cpu's behavior after wakeup depend on psd level,see flow table if cpu off , cpu will reset after wakeup
 *
 *                      PD_CORE     PD_CORE_MISC_DIG    PD_CORE_MISC_ANA    PD_CORE_CPU     PD_BZ   PD_USB
 *          PDS0          ON               ON                  ON               ON            ON      ON
 *          PDS1          ON               ON                  ON               ON            ON      OFF
 *          PDS2          ON               ON                  ON               ON            OFF     ON
 *          PDS3          ON               ON                  ON               ON            OFF     OFF
 *          PDS4          ON               ON                  ON               OFF           ON      ON
 *          PDS5          ON               ON                  ON               OFF           ON      OFF
 *          PDS6          ON               ON                  ON               OFF           OFF     ON
 *          PDS7          ON               ON                  ON               OFF           OFF     OFF
 *          PDS31         ON               OFF                 OFF              OFF           OFF     OFF
 */
ATTR_TCM_SECTION void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time)
{
    PDS_DEFAULT_LV_CFG_Type *pPdsCfg = NULL;
    uint32_t tmpVal;
    uint32_t flash_cfg_len;

    /* To make it simple and safe*/
    cpu_global_irq_disable();

    flash_get_cfg((uint8_t **)&flash_cfg, &flash_cfg_len);
    HBN_Set_Ldo11_All_Vout(PM_PDS_LDO_LEVEL_DEFAULT);

    PDS_WAKEUP_IRQHandler_Install();
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal &= ~(1 << 8); //unmask pds wakeup

    if (!BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_GPIO_INT), PDS_GPIO_INT_MASK) || !(BL_RD_REG(GLB_BASE, GLB_GPIO_INT_MASK1) == 0xffffffff)) {
        tmpVal |= (1 << 19); //enable gpio wakeup for pds
    }
    if (BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_IRQ_MODE), HBN_REG_AON_PAD_IE_SMT)) {
        tmpVal |= (1 << 17); //enable hbn out0 wakeup for pds
    }

    if (sleep_time) {
        tmpVal |= (1 << 16); //unmask pds sleep time wakeup
    }
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    PDS_Set_Vddcore_GPIO_IntClear();
    PDS_IntClear();

    /* enable PDS interrupt to wakeup CPU (PDS1:CPU not powerdown, CPU __WFI) */
    CPU_Interrupt_Enable(PDS_WAKEUP_IRQn);

    switch (pds_level) {
        case PM_PDS_LEVEL_0:
            pPdsCfg = &pdsCfgLevel0;
            break;
        case PM_PDS_LEVEL_1:
            pPdsCfg = &pdsCfgLevel1;
            break;
        case PM_PDS_LEVEL_2:
            pPdsCfg = &pdsCfgLevel2;
            break;
        case PM_PDS_LEVEL_3:
            pPdsCfg = &pdsCfgLevel3;
            break;
        case PM_PDS_LEVEL_4:
            return;
        case PM_PDS_LEVEL_5:
            return;
        case PM_PDS_LEVEL_6:
            return;
        case PM_PDS_LEVEL_7:
            return;
        case PM_PDS_LEVEL_31:
            pPdsCfg = &pdsCfgLevel31;
            break;
    }

#if PM_PDS_FLASH_POWER_OFF
    HBN_Power_Down_Flash(flash_cfg);
    /* turn_off_ext_flash_pin, GPIO23 - GPIO28 */
    for (uint32_t pin = 23; pin <= 28; pin++) {
        GLB_GPIO_Set_HZ(pin);
    }
    /* SF io select from efuse value */
    uint32_t flash_select = BL_RD_WORD(0x40007074);
    if (((flash_select >> 26) & 7) == 0) {
        HBN_Set_Pad_23_28_Pullup();
    }
    pPdsCfg->pdsCtl.puFlash = 1;
#endif

#if PM_PDS_PLL_POWER_OFF
    PDS_Power_Off_PLL();
#endif
#if PM_PDS_DLL_POWER_OFF
    GLB_Set_System_CLK(GLB_DLL_XTAL_NONE, GLB_SYS_CLK_RC32M);
    AON_Power_Off_XTAL();
    GLB_Power_Off_DLL();
    PDS_Update_Flash_Ctrl_Setting(0);
#endif

    /* pds0-pds7 : ldo11rt_iload_sel=3 */
    /* pds31     : ldo11rt_iload_sel=1 */
    if (pds_level <= PM_PDS_LEVEL_7) {
        HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_25_250UA);
    } else if (pds_level == PM_PDS_LEVEL_31) {
        HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_10_100UA);
    } else {
        /* pdsLevel error */
    }

    pPdsCfg->pdsCtl.pdsLdoVol = PM_PDS_LDO_LEVEL_DEFAULT;
    pPdsCfg->pdsCtl.pdsLdoVselEn = 1;

    if (BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_GPIO_INT), PDS_GPIO_INT_MASK)) {
        pPdsCfg->pdsCtl.gpioIePuPd = 0;
    }

#if PM_PDS_RF_POWER_OFF == 0
    pPdsCfg->pdsCtl.pdsCtlRfSel = 0;
#endif
    /* config  ldo11soc_sstart_delay_aon =2 , cr_pds_pd_ldo11=0 to speedup ldo11soc_rdy_aon */
    AON_Set_LDO11_SOC_Sstart_Delay(0x2);

    PDS_Default_Level_Config(pPdsCfg, sleep_time);

    __WFI(); /* if(.wfiMask==0){CPU won't power down until PDS module had seen __wfi} */

#if PM_PDS_PLL_POWER_OFF
    GLB_Set_System_CLK(XTAL_TYPE, BSP_ROOT_CLOCK_SOURCE);
    PDS_Update_Flash_Ctrl_Setting(1);
#endif

#if PM_PDS_FLASH_POWER_OFF
    HBN_Set_Pad_23_28_Pullnone();
    /* Init flash gpio */
    SF_Cfg_Init_Flash_Gpio(0, 1);

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    SFlash_Restore_From_Powerdown(flash_cfg, 0);
#endif

    cpu_global_irq_enable();
}

ATTR_TCM_SECTION void pm_pds31_fast_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time)
{
    PDS_DEFAULT_LV_CFG_Type *pPdsCfg = &pdsCfgLevel31;
    uint32_t tmpVal;
    uint32_t flash_cfg_len;

    // Get cache way disable setting
    cacheWayDisable = (*(volatile uint32_t *)(L1C_BASE + 0x00) >> 8) & 0x0F;

    // Get psram io configuration
    psramIoCfg = *(volatile uint32_t *)(GLB_BASE + 0x88);

    // Get flash offset
    flash_offset = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_ID0_OFFSET);

    /* To make it simple and safe*/
    cpu_global_irq_disable();

    flash_get_cfg((uint8_t **)&flash_cfg, &flash_cfg_len);
    HBN_Set_Ldo11_All_Vout(PM_PDS_LDO_LEVEL_DEFAULT);

    PDS_WAKEUP_IRQHandler_Install();
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal &= ~(1 << 8); //unmask pds wakeup

    if (!BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_GPIO_INT), PDS_GPIO_INT_MASK)) {
        tmpVal |= (1 << 19); //enable gpio wakeup for pds
    }
    if (BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_IRQ_MODE), HBN_REG_AON_PAD_IE_SMT)) {
        tmpVal |= (1 << 17); //enable hbn out0 wakeup for pds
    }

    if (sleep_time) {
        tmpVal |= (1 << 16); //unmask pds sleep time wakeup
    }
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    PDS_IntClear();

    /* enable PDS interrupt to wakeup CPU (PDS1:CPU not powerdown, CPU __WFI) */
    CPU_Interrupt_Enable(PDS_WAKEUP_IRQn);

#if PM_PDS_FLASH_POWER_OFF
    HBN_Power_Down_Flash(flash_cfg);
    /* turn_off_ext_flash_pin, GPIO23 - GPIO28 */
    for (uint32_t pin = 23; pin <= 28; pin++) {
        GLB_GPIO_Set_HZ(pin);
    }
    /* SF io select from efuse value */
    uint32_t flash_select = BL_RD_WORD(0x40007074);
    if (((flash_select >> 26) & 7) == 0) {
        HBN_Set_Pad_23_28_Pullup();
    }
    pPdsCfg->pdsCtl.puFlash = 1;
#endif

#if PM_PDS_PLL_POWER_OFF
    PDS_Power_Off_PLL();
#endif
#if PM_PDS_DLL_POWER_OFF
    GLB_Set_System_CLK(GLB_DLL_XTAL_NONE, GLB_SYS_CLK_RC32M);
    AON_Power_Off_XTAL();
    GLB_Power_Off_DLL();
    PDS_Update_Flash_Ctrl_Setting(0);
#endif

    /* pds31     : ldo11rt_iload_sel=1 */
    HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_10_100UA);

    pPdsCfg->pdsCtl.pdsLdoVol = PM_PDS_LDO_LEVEL_DEFAULT;
    pPdsCfg->pdsCtl.pdsLdoVselEn = 1;

    if (BL_GET_REG_BITS_VAL(BL_RD_REG(PDS_BASE, PDS_GPIO_INT), PDS_GPIO_INT_MASK)) {
        pPdsCfg->pdsCtl.gpioIePuPd = 0;
    }

#if PM_PDS_RF_POWER_OFF == 0
    pPdsCfg->pdsCtl.pdsCtlRfSel = 0;
#endif
    /* config  ldo11soc_sstart_delay_aon =2 , cr_pds_pd_ldo11=0 to speedup ldo11soc_rdy_aon */
    AON_Set_LDO11_SOC_Sstart_Delay(0x2);

    PDS_Default_Level_Config(pPdsCfg, sleep_time);

    __asm__ __volatile__(
        "csrr   a0,     mtvec\n\t"
        "csrr   a1,     mstatus\n\t"
        "la     a2,     __ld_pds_bak_addr\n\t"
        "sw     sp,     1*4(a2)\n\t"
        "sw     tp,     2*4(a2)\n\t"
        "sw     t0,     3*4(a2)\n\t"
        "sw     t1,     4*4(a2)\n\t"
        "sw     t2,     5*4(a2)\n\t"
        "sw     fp,     6*4(a2)\n\t"
        "sw     s1,     7*4(a2)\n\t"
        "sw     a0,     8*4(a2)\n\t"
        "sw     a1,     9*4(a2)\n\t"
        "sw     a3,     10*4(a2)\n\t"
        "sw     a4,     11*4(a2)\n\t"
        "sw     a5,     12*4(a2)\n\t"
        "sw     a6,     13*4(a2)\n\t"
        "sw     a7,     14*4(a2)\n\t"
        "sw     s2,     15*4(a2)\n\t"
        "sw     s3,     16*4(a2)\n\t"
        "sw     s4,     17*4(a2)\n\t"
        "sw     s5,     18*4(a2)\n\t"
        "sw     s6,     19*4(a2)\n\t"
        "sw     s7,     20*4(a2)\n\t"
        "sw     s8,     21*4(a2)\n\t"
        "sw     s9,     22*4(a2)\n\t"
        "sw     s10,    23*4(a2)\n\t"
        "sw     s11,    24*4(a2)\n\t"
        "sw     t3,     25*4(a2)\n\t"
        "sw     t4,     26*4(a2)\n\t"
        "sw     t5,     27*4(a2)\n\t"
        "sw     t6,     28*4(a2)\n\t");

    pm_pds_enter_done();

    cpu_global_irq_enable();

    // Get cache way disable setting
    *(volatile uint32_t *)(L1C_BASE + 0x00) &= ~(0x0F < 8);
    *(volatile uint32_t *)(L1C_BASE + 0x00) |= cacheWayDisable;

    // Get psram io configuration
    *(volatile uint32_t *)(GLB_BASE + 0x88) = psramIoCfg;
}
/**
 * @brief
 *
 * power management in hbn(hibernation) mode
 * cpu will reset after wakeup
 *
 * HBN_LEVEL   PD_AON       PD_AON_HNBRTC        PD_AON_HBNCORE        PD_CORE     PD_CORE_MISC_DIG    PD_CORE_MISC_ANA    PD_CORE_CPU     PD_BZ   PD_USB
 * HBN0          ON               ON                  ON                OFF            OFF                  OFF                 OFF         OFF     OFF
 * HBN1          ON               ON                  OFF               OFF            OFF                  OFF                 OFF         OFF     OFF
 * HBN2          ON               OFF                 OFF               OFF            OFF                  OFF                 OFF         OFF     OFF
 * @param hbn_level
 */
ATTR_TCM_SECTION void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint8_t sleep_time)
{
    uint32_t tmpVal;

    /* To make it simple and safe*/
    cpu_global_irq_disable();

    if (sleep_time && (hbn_level < PM_HBN_LEVEL_2))
        rtc_init(sleep_time); //sleep time,unit is second

    if (hbn_level >= PM_HBN_LEVEL_2)
        HBN_Power_Off_RC32K();
    else
        HBN_Power_On_RC32K();

    HBN_Power_Down_Flash(NULL);
    /* SF io select from efuse value */
    uint32_t flash_select = BL_RD_WORD(0x40007074);
    if (((flash_select >> 26) & 7) == 0) {
        HBN_Set_Pad_23_28_Pullup();
    }

    /* Select RC32M */
    GLB_Set_System_CLK(GLB_DLL_XTAL_NONE, GLB_SYS_CLK_RC32M);
    /* power off xtal */
    AON_Power_Off_XTAL();
    GLB_Power_Off_DLL();
    PDS_Power_Off_PLL();

    /* HBN mode LDO level */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_AON_VOUT_SEL, PM_HBN_LDO_LEVEL_DEFAULT);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_RT_VOUT_SEL, PM_HBN_LDO_LEVEL_DEFAULT);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);

    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbn_level) {
        case PM_HBN_LEVEL_0:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case PM_HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case PM_HBN_LEVEL_2:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;
        default:
            break;
    }

    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIP + HBN_OUT0_IRQn) = 0;
    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIP + HBN_OUT1_IRQn) = 0;

    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    /* Enable HBN mode */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    while (1) {
        BL702_Delay_MS(1000);
    }
}

ATTR_HBN_RAM_SECTION void pm_hbn_enter_again(bool reset)
{
    uint32_t tmpVal;
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    if (!reset)
        /* Enable HBN mode */
        BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);
}

void pm_set_wakeup_callback(void (*wakeup_callback)(void))
{
    BL_WR_REG(HBN_BASE, HBN_RSV1, (uint32_t)wakeup_callback);
    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);
}

enum pm_event_type pm_get_wakeup_event(void)
{
    if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_GPIO9)) {
        return PM_HBN_GPIO9_WAKEUP_EVENT;
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_GPIO10)) {
        return PM_HBN_GPIO10_WAKEUP_EVENT;
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_GPIO11)) {
        return PM_HBN_GPIO11_WAKEUP_EVENT;
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_GPIO12)) {
        return PM_HBN_GPIO12_WAKEUP_EVENT;
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_GPIO10)) {
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_PIR)) {
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_BOR)) {
        return PM_HBN_BOR_WAKEUP_EVENT;
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_ACOMP0)) {
        return PM_HBN_ACOMP0_WAKEUP_EVENT;
    } else if (BL_RD_REG(HBN_BASE, HBN_IRQ_STAT) & (1 << HBN_INT_ACOMP1)) {
        return PM_HBN_ACOMP1_WAKEUP_EVENT;
    }

    return PM_HBN_WAKEUP_EVENT_NONE;
}

void pm_bor_init(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_BOR);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_BOR);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOR_VTH, HBN_BOR_THRES_2P4V);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOR_SEL, HBN_BOR_MODE_POR_INDEPENDENT);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
}

void pm_hbn_out0_irq_register(void)
{
    Interrupt_Handler_Register(HBN_OUT1_IRQn, HBN_OUT0_IRQ);
    CPU_Interrupt_Enable(HBN_OUT0_IRQn);
}

void pm_hbn_out1_irq_register(void)
{
    Interrupt_Handler_Register(HBN_OUT1_IRQn, HBN_OUT1_IRQ);
    CPU_Interrupt_Enable(HBN_OUT1_IRQn);
}

void HBN_OUT0_IRQ(void)
{
    if (SET == HBN_Get_INT_State(HBN_INT_GPIO9)) {
        HBN_Clear_IRQ(HBN_INT_GPIO9);
        pm_irq_callback(PM_HBN_GPIO9_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO10)) {
        HBN_Clear_IRQ(HBN_INT_GPIO10);
        pm_irq_callback(PM_HBN_GPIO10_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO11)) {
        HBN_Clear_IRQ(HBN_INT_GPIO11);
        pm_irq_callback(PM_HBN_GPIO11_WAKEUP_EVENT);
    } else if (SET == HBN_Get_INT_State(HBN_INT_GPIO12)) {
        HBN_Clear_IRQ(HBN_INT_GPIO12);
        pm_irq_callback(PM_HBN_GPIO12_WAKEUP_EVENT);
    } else {
        HBN_Clear_IRQ(HBN_INT_RTC);
        HBN_Clear_RTC_INT();
        pm_irq_callback(PM_HBN_RTC_WAKEUP_EVENT);
    }
}

void HBN_OUT1_IRQ(void)
{
    /* PIR */
    if (SET == HBN_Get_INT_State(HBN_INT_PIR)) {
        HBN_Clear_IRQ(HBN_INT_PIR);
    }
    /* BOR */
    else if (SET == HBN_Get_INT_State(HBN_INT_BOR)) {
        HBN_Clear_IRQ(HBN_INT_BOR);
        pm_irq_callback(PM_HBN_BOR_WAKEUP_EVENT);
    }
    /* ACOMP0 */
    else if (SET == HBN_Get_INT_State(HBN_INT_ACOMP0)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP0);
        pm_irq_callback(PM_HBN_ACOMP0_WAKEUP_EVENT);
    }
    /* ACOMP1 */
    else if (SET == HBN_Get_INT_State(HBN_INT_ACOMP1)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP1);
        pm_irq_callback(PM_HBN_ACOMP1_WAKEUP_EVENT);
    }
}

__WEAK void pm_irq_callback(enum pm_event_type event)
{
}
