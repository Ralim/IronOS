/**
  ******************************************************************************
  * @file    bl602_sf_cfg.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "bl602_glb.h"
#include "bl602_sf_cfg.h"
#include "softcrc.h"
#include "bl602_xip_sflash.h"

/** @addtogroup  BL602_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SF_CFG
 *  @{
 */

/** @defgroup  SF_CFG_Private_Macros
 *  @{
 */
#define BFLB_FLASH_CFG_MAGIC "FCFG"

/*@} end of group SF_CFG_Private_Macros */

/** @defgroup  SF_CFG_Private_Types
 *  @{
 */
typedef struct
{
    uint32_t jedecID;
    char *name;
    const SPI_Flash_Cfg_Type *cfg;
} Flash_Info_t;

/*@} end of group SF_CFG_Private_Types */

/** @defgroup  SF_CFG_Private_Variables
 *  @{
 */
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_80DV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 0,
    .cReadMode = 0xFF,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3d,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};
#if 0
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_80BV =
{
	.resetCreadCmd = 0xff,
	.resetCreadCmdSize = 3,
	.mid = 0xef,

	.deBurstWrapCmd = 0x77,
	.deBurstWrapCmdDmyClk = 0x3,
	.deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
	.deBurstWrapData = 0xF0,

	/*reg*/
	.writeEnableCmd = 0x06,
	.wrEnableIndex = 0x00,
	.wrEnableBit = 0x01,
	.wrEnableReadRegLen = 0x01,

	.qeIndex = 1,
	.qeBit = 0x01,
	.qeWriteRegLen = 0x02,                      /*Q08BV,Q16DV: 0x02.Q32FW,Q32FV: 0x01 */
	.qeReadRegLen = 0x1,

	.busyIndex = 0,
	.busyBit = 0x00,
	.busyReadRegLen = 0x1,
	.releasePowerDown = 0xab,

	.readRegCmd[0] = 0x05,
	.readRegCmd[1] = 0x35,
	.writeRegCmd[0] = 0x01,
	.writeRegCmd[1] = 0x01,

	.fastReadQioCmd = 0xeb,
	.frQioDmyClk = 16 / 8,
	.cReadSupport = 1,
	.cReadMode = 0x20,

	.burstWrapCmd = 0x77,
	.burstWrapCmdDmyClk = 0x3,
	.burstWrapDataMode = SF_CTRL_DATA_4_LINES,
	.burstWrapData = 0x40,
	/*erase*/
	.chipEraseCmd = 0xc7,
	.sectorEraseCmd = 0x20,
	.blk32EraseCmd = 0x52,
	.blk64EraseCmd = 0xd8,
	/*write*/
	.pageProgramCmd = 0x02,
	.qpageProgramCmd = 0x32,
	.qppAddrMode = SF_CTRL_ADDR_1_LINE,

	.ioMode = SF_CTRL_QIO_MODE,
	.clkDelay = 1,
	.clkInvert = 0x3d,

	.resetEnCmd = 0x66,
	.resetCmd = 0x99,
	.cRExit = 0xff,
	.wrEnableWriteRegLen = 0x00,

	/*id*/
	.jedecIdCmd = 0x9f,
	.jedecIdCmdDmyClk = 0,
	.qpiJedecIdCmd = 0x9f,
	.qpiJedecIdCmdDmyClk = 0x00,
	.sectorSize = 4,
	.pageSize = 256,

	/*read*/
	.fastReadCmd = 0x0b,
	.frDmyClk = 8 / 8,
	.qpiFastReadCmd = 0x0b,
	.qpiFrDmyClk = 8 / 8,
	.fastReadDoCmd = 0x3b,
	.frDoDmyClk = 8 / 8,
	.fastReadDioCmd = 0xbb,
	.frDioDmyClk = 0,
	.fastReadQoCmd = 0x6b,
	.frQoDmyClk = 8 / 8,

	.qpiFastReadQioCmd = 0xeb,
	.qpiFrQioDmyClk = 16 / 8,
	.qpiPageProgramCmd = 0x02,
	.writeVregEnableCmd = 0x50,

	/* qpi mode */
	.enterQpi = 0x38,
	.exitQpi = 0xff,

	/*AC*/
	.timeEsector = 300,
	.timeE32k = 1200,
	.timeE64k = 1200,
	.timePagePgm = 5,
	.timeCe = 20 * 1000,
	.pdDelay = 3,
	.qeData = 0,
};
#endif
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 400,
    .timeE32k = 1600,
    .timeE64k = 2000,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 3,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Issi = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x9d,

    .deBurstWrapCmd = 0xC0,
    .deBurstWrapCmdDmyClk = 0x00,
    .deBurstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .deBurstWrapData = 0x00,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0,
    .qeBit = 0x06,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA0,

    .burstWrapCmd = 0xC0,
    .burstWrapCmdDmyClk = 0x00,
    .burstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .burstWrapData = 0x06,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 5,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Gd_LQ08C_LE16C_LQ32D_WQ32E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Gd_Q80E_Q16E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Gd_WQ80E_WQ16E = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 32 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA0,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 8 / 8,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0x12,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Gd_Q32C = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xc8,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Mxic = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xC2,

    .deBurstWrapCmd = 0xC0,
    .deBurstWrapCmdDmyClk = 0x00,
    .deBurstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .deBurstWrapData = 0x10,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0,
    .qeBit = 0x06,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA5,

    .burstWrapCmd = 0xC0,
    .burstWrapCmdDmyClk = 0x00,
    .burstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .burstWrapData = 0x02,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x38,
    .qppAddrMode = SF_CTRL_ADDR_4_LINES,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 45,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Mxic_1635F = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xC2,

    .deBurstWrapCmd = 0xC0,
    .deBurstWrapCmdDmyClk = 0x00,
    .deBurstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .deBurstWrapData = 0x10,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0,
    .qeBit = 0x06,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0xA5,

    .burstWrapCmd = 0xC0,
    .burstWrapCmdDmyClk = 0x00,
    .burstWrapDataMode = SF_CTRL_DATA_1_LINE,
    .burstWrapData = 0x02,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x38,
    .qppAddrMode = SF_CTRL_ADDR_4_LINES,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 45,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Xtx_Q80B_F16B = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x0B,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0x01,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = 0x14,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,
    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 6000,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Xtx = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x0B,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0x01,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,
    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 6000,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Puya_Q80L_Q80H_Q16H = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x85,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0x01,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x02,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x01,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3d,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,
    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 8,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Puya_Q32H = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x85,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0x01,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,
    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 8,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_Boya = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0x68,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 0x01,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,
    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 20,
    .qeData = 0,
};
static const ATTR_TCM_CONST_SECTION SPI_Flash_Cfg_Type flashCfg_FT_VQ80 = {
    .resetCreadCmd = 0xff,
    .resetCreadCmdSize = 3,
    .mid = 0xef,

    .deBurstWrapCmd = 0x77,
    .deBurstWrapCmdDmyClk = 0x3,
    .deBurstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .deBurstWrapData = 0xF0,

    /*reg*/
    .writeEnableCmd = 0x06,
    .wrEnableIndex = 0x00,
    .wrEnableBit = 0x01,
    .wrEnableReadRegLen = 0x01,

    .qeIndex = 1,
    .qeBit = 0x01,
    .qeWriteRegLen = 0x01,
    .qeReadRegLen = 0x1,

    .busyIndex = 0,
    .busyBit = 0x00,
    .busyReadRegLen = 0x1,
    .releasePowerDown = 0xab,

    .readRegCmd[0] = 0x05,
    .readRegCmd[1] = 0x35,
    .writeRegCmd[0] = 0x01,
    .writeRegCmd[1] = 0x31,

    .fastReadQioCmd = 0xeb,
    .frQioDmyClk = 16 / 8,
    .cReadSupport = 1,
    .cReadMode = 0x20,

    .burstWrapCmd = 0x77,
    .burstWrapCmdDmyClk = 0x3,
    .burstWrapDataMode = SF_CTRL_DATA_4_LINES,
    .burstWrapData = 0x40,
    /*erase*/
    .chipEraseCmd = 0xc7,
    .sectorEraseCmd = 0x20,
    .blk32EraseCmd = 0x52,
    .blk64EraseCmd = 0xd8,
    /*write*/
    .pageProgramCmd = 0x02,
    .qpageProgramCmd = 0x32,
    .qppAddrMode = SF_CTRL_ADDR_1_LINE,

    .ioMode = SF_CTRL_QIO_MODE,
    .clkDelay = 1,
    .clkInvert = 0x3f,

    .resetEnCmd = 0x66,
    .resetCmd = 0x99,
    .cRExit = 0xff,
    .wrEnableWriteRegLen = 0x00,

    /*id*/
    .jedecIdCmd = 0x9f,
    .jedecIdCmdDmyClk = 0,
    .qpiJedecIdCmd = 0x9f,
    .qpiJedecIdCmdDmyClk = 0x00,
    .sectorSize = 4,
    .pageSize = 256,

    /*read*/
    .fastReadCmd = 0x0b,
    .frDmyClk = 8 / 8,
    .qpiFastReadCmd = 0x0b,
    .qpiFrDmyClk = 8 / 8,
    .fastReadDoCmd = 0x3b,
    .frDoDmyClk = 8 / 8,
    .fastReadDioCmd = 0xbb,
    .frDioDmyClk = 0,
    .fastReadQoCmd = 0x6b,
    .frQoDmyClk = 8 / 8,

    .qpiFastReadQioCmd = 0xeb,
    .qpiFrQioDmyClk = 16 / 8,
    .qpiPageProgramCmd = 0x02,
    .writeVregEnableCmd = 0x50,

    /* qpi mode */
    .enterQpi = 0x38,
    .exitQpi = 0xff,

    /*AC*/
    .timeEsector = 300,
    .timeE32k = 1200,
    .timeE64k = 1200,
    .timePagePgm = 5,
    .timeCe = 20 * 1000,
    .pdDelay = 8,
    .qeData = 0,
};

static const ATTR_TCM_CONST_SECTION Flash_Info_t flashInfos[] = {
    {
        .jedecID = 0x1440ef,
        //.name="Winb_80DV_08_33",
        .cfg = &flashCfg_Winb_80DV,
    },
    {
        .jedecID = 0x1540ef,
        //.name="Winb_16DV_16_33",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x1570ef,
        //.name="Winb_16jV_16_33_DTR",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x1640ef,
        //.name="Winb_32FV_32_33",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x1460ef,
        //.name="Winb_80EW_08_18",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x1560ef,
        //.name="Winb_16FW_16_18",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x1660ef,
        //.name="Winb_32FW_32_18",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x1860ef,
        //.name="Winb_128FW_128_18",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x1680ef,
        //.name="Winb_32JW_32_18",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x13605e,
        //.name="Zbit_04_33",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x14605e,
        //.name="Zbit_08_33",
        .cfg = &flashCfg_Winb_80EW_16JV_16FW_32JW_32FW_32FV,
    },
    {
        .jedecID = 0x14609d,
        //.name="ISSI_08_33",
        .cfg = &flashCfg_Issi,
    },
    {
        .jedecID = 0x15609d,
        //.name="ISSI_16_33",
        .cfg = &flashCfg_Issi,
    },
    {
        .jedecID = 0x16609d,
        //.name="ISSI_32_33",
        .cfg = &flashCfg_Issi,
    },
    {
        .jedecID = 0x14709d,
        //.name="ISSI_08_18",
        .cfg = &flashCfg_Issi,
    },
    {
        .jedecID = 0x15709d,
        //.name="ISSI_16_18",
        .cfg = &flashCfg_Issi,
    },
    {
        .jedecID = 0x16709d,
        //.name="ISSI_32_18",
        .cfg = &flashCfg_Issi,
    },
    {
        .jedecID = 0x1440C8,
        //.name="GD_Q08E_08_33",
        .cfg = &flashCfg_Gd_Q80E_Q16E,
    },
    {
        .jedecID = 0x1540C8,
        //.name="GD_Q16E_16_33",
        .cfg = &flashCfg_Gd_Q80E_Q16E,
    },
    {
        .jedecID = 0x1640C8,
        //.name="GD_Q32C_32_33",
        .cfg = &flashCfg_Gd_Q32C,
    },
    {
        .jedecID = 0x1460C8,
        //.name="GD_LQ08C_08_18",
        .cfg = &flashCfg_Gd_LQ08C_LE16C_LQ32D_WQ32E,
    },
    {
        .jedecID = 0x1560C8,
        //.name="GD_LE16C_16_18",
        .cfg = &flashCfg_Gd_LQ08C_LE16C_LQ32D_WQ32E,
    },
    {
        .jedecID = 0x1660C8,
        //.name="GD_LQ32D_32_18",
        .cfg = &flashCfg_Gd_LQ08C_LE16C_LQ32D_WQ32E,
    },
    {
        .jedecID = 0x1465C8,
        //.name="GD_WQ80E_80_33",
        .cfg = &flashCfg_Gd_WQ80E_WQ16E,
    },
    {
        .jedecID = 0x1565C8,
        //.name="GD_WQ16E_16_33",
        .cfg = &flashCfg_Gd_WQ80E_WQ16E,
    },
    {
        .jedecID = 0x1665C8,
        //.name="GD_WQ32E_32_33",
        .cfg = &flashCfg_Gd_LQ08C_LE16C_LQ32D_WQ32E,
    },
    {
        .jedecID = 0x3425C2,
        //.name="MX_25V80_08_18",
        .cfg = &flashCfg_Mxic,
    },
    {
        .jedecID = 0x3525C2,
        //.name="MX_25U16_35_18",
        .cfg = &flashCfg_Mxic_1635F,
    },
    {
        .jedecID = 0x3625C2,
        //.name="MX_25V32_32_18",
        .cfg = &flashCfg_Mxic,
    },
    {
        .jedecID = 0x13400B,
        //.name="XT_25F04D_04_33",
        .cfg = &flashCfg_Xtx,
    },
    {
        .jedecID = 0x15400B,
        //.name="XT_25F16B_16_33",
        .cfg = &flashCfg_Xtx_Q80B_F16B,
    },
    {
        .jedecID = 0x16400B,
        //.name="XT_25F32B_32_33",
        .cfg = &flashCfg_Xtx,
    },
    {
        .jedecID = 0x14600B,
        //.name="XT_25Q80B_08_18",
        .cfg = &flashCfg_Xtx_Q80B_F16B,
    },
    {
        .jedecID = 0x16600B,
        //.name="XT_25Q32B_32_18",
        .cfg = &flashCfg_Xtx,
    },
    {
        .jedecID = 0x146085,
        //.name="Puya_Q80L/H_08_18/33",
        .cfg = &flashCfg_Puya_Q80L_Q80H_Q16H,
    },
    {
        .jedecID = 0x156085,
        //.name="Puya_Q16H_16_33",
        .cfg = &flashCfg_Puya_Q80L_Q80H_Q16H,
    },
    {
        .jedecID = 0x166085,
        //.name="Puya_Q32H_32_33",
        .cfg = &flashCfg_Puya_Q32H,
    },
    {
        .jedecID = 0x154068,
        //.name="Boya_Q16B_16_33",
        .cfg = &flashCfg_Boya,
    },
    {
        .jedecID = 0x164068,
        //.name="Boya_Q32B_32_33",
        .cfg = &flashCfg_Boya,
    },
    {
        .jedecID = 0x174068,
        //.name="Boya_Q64A_64_33",
        .cfg = &flashCfg_Boya,
    },
    {
        .jedecID = 0x184068,
        //.name="Boya_Q128A_128_33",
        .cfg = &flashCfg_Boya,
    },
    {
        .jedecID = 0x14605E,
        //.name="FT_VQ80",
        .cfg = &flashCfg_FT_VQ80,
    }
};

/*@} end of group SF_CFG_Private_Variables */

/** @defgroup  SF_CFG_Global_Variables
 *  @{
 */

/*@} end of group SF_CFG_Global_Variables */

/** @defgroup  SF_CFG_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SF_CFG_Private_Fun_Declaration */

/** @defgroup  SF_CFG_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Init external flash GPIO according to flash GPIO config
 *
 * @param  extFlashPin: Flash GPIO config
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Cfg_Init_Ext_Flash_Gpio(uint8_t extFlashPin)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[6];
    uint8_t i = 0;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_FLASH;

    if (extFlashPin == 0) {
        gpiopins[0] = BFLB_EXTFLASH_CLK0_GPIO;
        gpiopins[1] = BFLB_EXTFLASH_CS0_GPIO;
        gpiopins[2] = BFLB_EXTFLASH_DATA00_GPIO;
        gpiopins[3] = BFLB_EXTFLASH_DATA10_GPIO;
        gpiopins[4] = BFLB_EXTFLASH_DATA20_GPIO;
        gpiopins[5] = BFLB_EXTFLASH_DATA30_GPIO;
    } else {
        gpiopins[0] = BFLB_EXTFLASH_CLK1_GPIO;
        gpiopins[1] = BFLB_EXTFLASH_CS1_GPIO;
        gpiopins[2] = BFLB_EXTFLASH_DATA01_GPIO;
        gpiopins[3] = BFLB_EXTFLASH_DATA11_GPIO;
        gpiopins[4] = BFLB_EXTFLASH_DATA21_GPIO;
        gpiopins[5] = BFLB_EXTFLASH_DATA31_GPIO;
    }

    for (i = 0; i < sizeof(gpiopins); i++) {
        cfg.gpioPin = gpiopins[i];

        if (i == 0 || i == 1) {
            /*flash clk and cs is output*/
            cfg.gpioMode = GPIO_MODE_OUTPUT;
        } else {
            /*data are bidir*/
            cfg.gpioMode = GPIO_MODE_AF;
        }

        GLB_GPIO_Init(&cfg);
    }
}

/****************************************************************************/ /**
 * @brief  Init internal flash GPIO according to flash GPIO config
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SF_Cfg_Init_Internal_Flash_Gpio(void)
{
    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = GPIO_FUN_SWGPIO,
        .gpioMode = GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_NONE,
        .drive = 0,
        .smtCtrl = 1,
    };

    /* Turn on Flash pad, GPIO23 - GPIO28 */
    for (uint32_t pin = 23; pin < 29; pin++) {
        gpioCfg.gpioPin = pin;

        if (pin == 24) {
            gpioCfg.pullType = GPIO_PULL_DOWN;
        } else {
            gpioCfg.pullType = GPIO_PULL_NONE;
        }

        GLB_GPIO_Init(&gpioCfg);
    }
}

/****************************************************************************/ /**
 * @brief  Deinit external flash GPIO according to flash GPIO config
 *
 * @param  extFlashPin: Flash GPIO config
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SF_Cfg_Deinit_Ext_Flash_Gpio(uint8_t extFlashPin)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[6];
    uint8_t i = 0;

    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_SWGPIO;

    if (extFlashPin == 0) {
        gpiopins[0] = BFLB_EXTFLASH_CLK0_GPIO;
        gpiopins[1] = BFLB_EXTFLASH_CS0_GPIO;
        gpiopins[2] = BFLB_EXTFLASH_DATA00_GPIO;
        gpiopins[3] = BFLB_EXTFLASH_DATA10_GPIO;
        gpiopins[4] = BFLB_EXTFLASH_DATA20_GPIO;
        gpiopins[5] = BFLB_EXTFLASH_DATA30_GPIO;

    } else {
        gpiopins[0] = BFLB_EXTFLASH_CLK1_GPIO;
        gpiopins[1] = BFLB_EXTFLASH_CS1_GPIO;
        gpiopins[2] = BFLB_EXTFLASH_DATA01_GPIO;
        gpiopins[3] = BFLB_EXTFLASH_DATA11_GPIO;
        gpiopins[4] = BFLB_EXTFLASH_DATA21_GPIO;
        gpiopins[5] = BFLB_EXTFLASH_DATA31_GPIO;
    }

    for (i = 0; i < sizeof(gpiopins); i++) {
        cfg.gpioPin = gpiopins[i];
        GLB_GPIO_Init(&cfg);
    }
}

/****************************************************************************/ /**
 * @brief  Restore GPIO17 function
 *
 * @param  fun: GPIO17 function
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SF_Cfg_Restore_GPIO17_Fun(uint8_t fun)
{
    GLB_GPIO_Cfg_Type cfg;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioPin = GLB_GPIO_PIN_17;
    cfg.gpioFun = fun;
    GLB_GPIO_Init(&cfg);
}

/*@} end of group SF_CFG_Private_Functions */

/** @defgroup  SF_CFG_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Get flash config according to flash ID
 *
 * @param  flashID: Flash ID
 * @param  pFlashCfg: Flash config pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SF_Cfg_Get_Flash_Cfg_Need_Lock(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t i;
    uint8_t buf[sizeof(SPI_Flash_Cfg_Type) + 8];
    uint32_t crc, *pCrc;

    if (flashID == 0) {
        XIP_SFlash_Read_Via_Cache_Need_Lock(8 + BL602_FLASH_XIP_BASE, buf, sizeof(SPI_Flash_Cfg_Type) + 8);

        if (BL602_MemCmp(buf, BFLB_FLASH_CFG_MAGIC, 4) == 0) {
            crc = BFLB_Soft_CRC32((uint8_t *)buf + 4, sizeof(SPI_Flash_Cfg_Type));
            pCrc = (uint32_t *)(buf + 4 + sizeof(SPI_Flash_Cfg_Type));

            if (*pCrc == crc) {
                BL602_MemCpy_Fast(pFlashCfg, (uint8_t *)buf + 4, sizeof(SPI_Flash_Cfg_Type));
                return SUCCESS;
            }
        }
    } else {
        for (i = 0; i < sizeof(flashInfos) / sizeof(flashInfos[0]); i++) {
            if (flashInfos[i].jedecID == flashID) {
                BL602_MemCpy_Fast(pFlashCfg, flashInfos[i].cfg, sizeof(SPI_Flash_Cfg_Type));
                return SUCCESS;
            }
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Init flash GPIO according to flash Pin config
 *
 * @param  flashPinCfg: Specify flash Pin config
 * @param  restoreDefault: Wether to restore default setting
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SF_Cfg_Init_Flash_Gpio(uint8_t flashPinCfg, uint8_t restoreDefault)
{
    if (restoreDefault) {
        /* Set Default first */
        SF_Ctrl_Select_Pad(SF_CTRL_EMBEDDED_SEL);
        GLB_Select_Internal_Flash();
        GLB_Swap_Flash_Pin();
        //SF_Cfg_Deinit_Ext_Flash_Gpio(0);
        SF_Cfg_Deinit_Ext_Flash_Gpio(1);
        SF_Cfg_Init_Internal_Flash_Gpio();
    }

    if (flashPinCfg > 0) {
        /*01: deswap flash PIN
		  10: use ext flash 1(GPIO17-22)
		  11: use ext flash 0(GPIO0-2, 20-22)
		*/
        if (flashPinCfg == BFLB_FLASH_CFG_DESWAP) {
            SF_Ctrl_Select_Pad(SF_CTRL_EMBEDDED_SEL);
            /*DONOT Swap flash PIN*/
            GLB_Deswap_Flash_Pin();
        } else {
            SF_Ctrl_Select_Pad(flashPinCfg - 1);
            GLB_Select_External_Flash();
            SF_Cfg_Init_Ext_Flash_Gpio(flashPinCfg - BFLB_FLASH_CFG_EXT0_17_22);
        }
    }
}

/****************************************************************************/ /**
 * @brief  Identify one flash
 *
 * @param  callFromFlash: code run at flash or ram
 * @param  autoScan: Auto scan all GPIO pin
 * @param  flashPinCfg: Specify flash GPIO config, not auto scan
 * @param  restoreDefault: Wether restore default flash GPIO config
 * @param  pFlashCfg: Flash config pointer
 *
 * @return Flash ID
 *
*******************************************************************************/
__WEAK
uint32_t ATTR_TCM_SECTION SF_Cfg_Flash_Identify(uint8_t callFromFlash,
                                                uint32_t autoScan, uint32_t flashPinCfg, uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t jdecId = 0;
    uint32_t i = 0;
    uint32_t offset;
    BL_Err_Type stat;
    uint8_t gpio17_fun = 0;

    BL602_MemCpy_Fast(pFlashCfg, &flashCfg_Gd_Q80E_Q16E, sizeof(SPI_Flash_Cfg_Type));

    if (callFromFlash == 1) {
        stat = XIP_SFlash_State_Save(pFlashCfg, &offset);

        if (stat != SUCCESS) {
            SFlash_Set_IDbus_Cfg(pFlashCfg, SF_CTRL_QIO_MODE, 1, 0, 32);
            return 0;
        }
    }

    if (autoScan) {
        flashPinCfg = 0;

        do {
            if (flashPinCfg > BFLB_FLASH_CFG_EXT1_0_2_20_22) {
                jdecId = 0;
                break;
            }

            if (flashPinCfg > BFLB_FLASH_CFG_EXT0_17_22) {
                /*flashPinCfg=2 has make gpio17 into jtag,but not success*/
                SF_Cfg_Restore_GPIO17_Fun(gpio17_fun);
            }

            /* select media gpio */
            if (flashPinCfg == BFLB_FLASH_CFG_EXT0_17_22) {
                gpio17_fun = GLB_GPIO_Get_Fun(GLB_GPIO_PIN_17);
            }

            SF_Cfg_Init_Flash_Gpio(flashPinCfg, restoreDefault);
            SFlash_Reset_Continue_Read(pFlashCfg);
            SFlash_DisableBurstWrap(pFlashCfg);
            jdecId = 0;
            SFlash_GetJedecId(pFlashCfg, (uint8_t *)&jdecId);
            SFlash_DisableBurstWrap(pFlashCfg);
            jdecId = jdecId & 0xffffff;
            flashPinCfg++;
        } while ((jdecId & 0x00ffff) == 0 || (jdecId & 0xffff00) == 0 || (jdecId & 0x00ffff) == 0xffff || (jdecId & 0xffff00) == 0xffff00);
    } else {
        /* select media gpio */
        SF_Cfg_Init_Flash_Gpio(flashPinCfg, restoreDefault);
        SFlash_Reset_Continue_Read(pFlashCfg);
        SFlash_DisableBurstWrap(pFlashCfg);
        SFlash_GetJedecId(pFlashCfg, (uint8_t *)&jdecId);
        SFlash_DisableBurstWrap(pFlashCfg);
        jdecId = jdecId & 0xffffff;
    }

    for (i = 0; i < sizeof(flashInfos) / sizeof(flashInfos[0]); i++) {
        if (flashInfos[i].jedecID == jdecId) {
            BL602_MemCpy_Fast(pFlashCfg, flashInfos[i].cfg, sizeof(SPI_Flash_Cfg_Type));
            break;
        }
    }

    if (i == sizeof(flashInfos) / sizeof(flashInfos[0])) {
        if (callFromFlash == 1) {
            XIP_SFlash_State_Restore(pFlashCfg, offset);
        }

        return jdecId;
    } else {
        if (callFromFlash == 1) {
            XIP_SFlash_State_Restore(pFlashCfg, offset);
        }

        return (jdecId | BFLB_FLASH_ID_VALID_FLAG);
    }
}
#endif

/*@} end of group SF_CFG_Public_Functions */

/*@} end of group SF_CFG */

/*@} end of group BL602_Peripheral_Driver */
