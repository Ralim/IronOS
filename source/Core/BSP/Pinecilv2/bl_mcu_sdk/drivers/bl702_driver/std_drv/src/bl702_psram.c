/**
  ******************************************************************************
  * @file    bl702_psram.c
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

#include "bl702_psram.h"
#include "bl702_l1c.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PSRAM
 *  @{
 */

/** @defgroup  PSRAM_Private_Macros
 *  @{
 */

/*@} end of group PSRAM_Private_Macros */

/** @defgroup  PSRAM_Private_Types
 *  @{
 */

/*@} end of group PSRAM_Private_Types */

/** @defgroup  PSRAM_Private_Variables
 *  @{
 */

/*@} end of group PSRAM_Private_Variables */

/** @defgroup  PSRAM_Global_Variables
 *  @{
 */

/*@} end of group PSRAM_Global_Variables */

/** @defgroup  PSRAM_Private_Fun_Declaration
 *  @{
 */

/*@} end of group PSRAM_Private_Fun_Declaration */

/** @defgroup  PSRAM_Private_Functions
 *  @{
 */

/*@} end of group PSRAM_Private_Functions */

/** @defgroup  PSRAM_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Init serial psram control interface
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  cmdsCfg: Serial Serial Flash controller configuration pointer
 * @param  sfCtrlPsramCfg: Serial psram controller configuration pointer
 *
 * @return None
 *
*******************************************************************************/
//#ifndef BFLB_USE_ROM_DRIVER
//__WEAK
void ATTR_TCM_SECTION Psram_Init(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg)
{
    SF_Ctrl_Psram_Init(sfCtrlPsramCfg);
    SF_Ctrl_Cmds_Set(cmdsCfg);

    Psram_SetDriveStrength(psramCfg);
    Psram_SetBurstWrap(psramCfg);
}
//#endif

/****************************************************************************/ /**
 * @brief  Read psram register
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  regValue: Register value pointer to store data
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION Psram_ReadReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue)
{
    /* Check the parameters */
    CHECK_PARAM(IS_PSRAM_CTRL_MODE(psramCfg->ctrlMode));

    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    if (psramCfg->ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    psramCmd.cmdBuf[0] = (psramCfg->readRegCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.dummyClks = psramCfg->readRegDmyClk;
    psramCmd.nbData = 1;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    BL702_MemCpy(regValue, psramCtrlBuf, 1);
}
#endif

/****************************************************************************/ /**
 * @brief  Write psram register
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  regValue: Register value pointer storing data
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION Psram_WriteReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue)
{
    /* Check the parameters */
    CHECK_PARAM(IS_PSRAM_CTRL_MODE(psramCfg->ctrlMode));

    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    BL702_MemCpy(psramCtrlBuf, regValue, 1);

    if (psramCfg->ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    psramCmd.cmdBuf[0] = (psramCfg->writeRegCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_WRITE;
    psramCmd.addrSize = 3;
    psramCmd.nbData = 1;

    SF_Ctrl_SendCmd(&psramCmd);
}
#endif

/****************************************************************************/ /**
 * @brief  Set psram driver strength
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_SetDriveStrength(SPI_Psram_Cfg_Type *psramCfg)
{
    uint32_t stat = 0;
    /* Check the parameters */
    CHECK_PARAM(IS_PSRAM_DRIVE_STRENGTH(psramCfg->driveStrength));

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if ((stat & 0x3) == psramCfg->driveStrength) {
        return SUCCESS;
    }

    stat &= (~0x3);
    stat |= psramCfg->driveStrength;

    Psram_WriteReg(psramCfg, (uint8_t *)&stat);
    /* Wait for write done */

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if ((stat & 0x3) == psramCfg->driveStrength) {
        return SUCCESS;
    }

    return ERROR;
}
#endif

/****************************************************************************/ /**
 * @brief  Set psram burst wrap size
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_SetBurstWrap(SPI_Psram_Cfg_Type *psramCfg)
{
    uint32_t stat = 0;
    /* Check the parameters */
    CHECK_PARAM(IS_PSRAM_BURST_LENGTH(psramCfg->burstLength));

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if (((stat >> 5) & 0x3) == psramCfg->burstLength) {
        return SUCCESS;
    }

    stat &= (~(0x3 << 5));
    stat |= (psramCfg->burstLength << 5);

    Psram_WriteReg(psramCfg, (uint8_t *)&stat);
    /* Wait for write done */

    Psram_ReadReg(psramCfg, (uint8_t *)&stat);

    if (((stat >> 5) & 0x3) == psramCfg->burstLength) {
        return SUCCESS;
    }

    return ERROR;
}
#endif

/****************************************************************************/ /**
 * @brief  Get psram ID
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  data: Data pointer to store read data
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION Psram_ReadId(SPI_Psram_Cfg_Type *psramCfg, uint8_t *data)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    psramCmd.cmdBuf[0] = (psramCfg->readIdCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.dummyClks = psramCfg->readIdDmyClk;
    psramCmd.nbData = 8;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    BL702_MemCpy(data, psramCtrlBuf, 8);
}
#endif

/****************************************************************************/ /**
 * @brief  Psram enter quad mode
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_EnterQuadMode(SPI_Psram_Cfg_Type *psramCfg)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    psramCmd.cmdBuf[0] = (psramCfg->enterQuadModeCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Psram exit quad mode
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_ExitQuadMode(SPI_Psram_Cfg_Type *psramCfg)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
    psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
    psramCmd.dataMode = SF_CTRL_DATA_4_LINES;

    psramCmd.cmdBuf[0] = (psramCfg->exitQuadModeCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Psram toggle burst length
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  ctrlMode: Psram ctrl mode type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_ToggleBurstLength(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;
    /* Check the parameters */
    CHECK_PARAM(IS_PSRAM_CTRL_MODE(ctrlMode));

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    if (ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    psramCmd.cmdBuf[0] = (psramCfg->burstToggleCmd) << 24;
    psramCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Psram software reset
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  ctrlMode: Psram ctrl mode type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_SoftwareReset(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode)
{
    SF_Ctrl_Cmd_Cfg_Type psramCmd;
    /* Check the parameters */
    CHECK_PARAM(IS_PSRAM_CTRL_MODE(ctrlMode));

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    if (ctrlMode == PSRAM_QPI_CTRL_MODE) {
        psramCmd.cmdMode = SF_CTRL_CMD_4_LINES;
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
    }

    /* Reset enable */
    psramCmd.cmdBuf[0] = (psramCfg->resetEnableCmd) << 24;
    /* rwFlag don't care */
    psramCmd.rwFlag = SF_CTRL_READ;
    /* Wait for write done */

    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    /* Reset */
    psramCmd.cmdBuf[0] = (psramCfg->resetCmd) << 24;
    /* rwFlag don't care */
    psramCmd.rwFlag = SF_CTRL_READ;
    SF_Ctrl_SendCmd(&psramCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
    }

    BL702_Delay_US(50);
    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Psram set IDbus config
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  ioMode: Psram ctrl mode type
 * @param  addr: Address to read/write
 * @param  len: Data length to read/write
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_Set_IDbus_Cfg(SPI_Psram_Cfg_Type *psramCfg,
                                                 SF_Ctrl_IO_Type ioMode, uint32_t addr, uint32_t len)
{
    uint8_t cmd, dummyClks;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;
    uint8_t cmdValid = 1;
    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_IO_TYPE(ioMode));

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_IAHB);

    /* read mode cache set */
    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->fReadCmd;
        dummyClks = psramCfg->fReadDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->fReadQuadCmd;
        dummyClks = psramCfg->fReadQuadDmyClk;
    } else {
        return ERROR;
    }

    /* prepare command */
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.cmdBuf[0] = (cmd << 24) | addr;
    psramCmd.dummyClks = dummyClks;
    psramCmd.nbData = len;
    SF_Ctrl_Psram_Read_Icache_Set(&psramCmd, cmdValid);

    /* write mode cache set */
    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->writeCmd;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->quadWriteCmd;
    } else {
        return ERROR;
    }

    dummyClks = 0;

    /* prepare command */
    psramCmd.rwFlag = SF_CTRL_WRITE;
    psramCmd.addrSize = 3;
    psramCmd.cmdBuf[0] = (cmd << 24) | addr;
    psramCmd.dummyClks = dummyClks;
    psramCmd.nbData = len;
    SF_Ctrl_Psram_Write_Icache_Set(&psramCmd, cmdValid);
    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Set cache write to psram with cache
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  ioMode: Psram controller interface mode
 * @param  wtEn: Psram cache write through enable
 * @param  wbEn: Psram cache write back enable
 * @param  waEn: Psram cache write allocate enable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_Cache_Write_Set(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode,
                                                   BL_Fun_Type wtEn, BL_Fun_Type wbEn, BL_Fun_Type waEn)
{
    BL_Err_Type stat;

    /* Cache now only support 32 bytes read */
    stat = Psram_Set_IDbus_Cfg(psramCfg, ioMode, 0, 32);

    if (SUCCESS != stat) {
        return stat;
    }

    L1C_Cache_Write_Set(wtEn, wbEn, waEn);
    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Write psram one region
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  ioMode: Write mode: SPI mode or QPI mode
 * @param  addr: Start address to be write
 * @param  data: Data pointer to be write
 * @param  len: Data length to be write
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_Write(SPI_Psram_Cfg_Type *psramCfg,
                                         SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t i = 0, curLen = 0;
    uint32_t burstLen = 512;
    uint8_t cmd;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;
    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_IO_TYPE(ioMode));

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->writeCmd;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->quadWriteCmd;
    } else {
        return ERROR;
    }

    /* Prepare command */
    psramCmd.rwFlag = SF_CTRL_WRITE;
    psramCmd.addrSize = 3;

    if (psramCfg->burstLength == PSRAM_BURST_LENGTH_16_BYTES) {
        burstLen = 16;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_32_BYTES) {
        burstLen = 32;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_64_BYTES) {
        burstLen = 64;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_512_BYTES) {
        burstLen = 512;
    }

    for (i = 0; i < len;) {
        /* Get current programmed length within page size */
        curLen = burstLen - addr % burstLen;

        if (curLen > len - i) {
            curLen = len - i;
        }

        /* Prepare command */
        BL702_MemCpy_Fast(psramCtrlBuf, data, curLen);
        psramCmd.cmdBuf[0] = (cmd << 24) | (addr);
        psramCmd.nbData = curLen;

        SF_Ctrl_SendCmd(&psramCmd);

        /* Adjust address and programmed length */
        addr += curLen;
        i += curLen;
        data += curLen;

        /* Wait for write done */
    }

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Read data from psram
 *
 * @param  psramCfg: Serial psram parameter configuration pointer
 * @param  ioMode: IoMode: psram controller interface mode
 * @param  addr: Psram read start address
 * @param  data: Data pointer to store data read from psram
 * @param  len: Data length to read
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION Psram_Read(SPI_Psram_Cfg_Type *psramCfg,
                                        SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const psramCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t curLen, i;
    uint32_t burstLen = 512;
    uint8_t cmd, dummyClks;
    SF_Ctrl_Cmd_Cfg_Type psramCmd;
    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_IO_TYPE(ioMode));

    if (((uint32_t)&psramCmd) % 4 == 0) {
        BL702_MemSet4((uint32_t *)&psramCmd, 0, sizeof(psramCmd) / 4);
    } else {
        BL702_MemSet(&psramCmd, 0, sizeof(psramCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = psramCfg->fReadCmd;
        dummyClks = psramCfg->fReadDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        psramCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        psramCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = psramCfg->fReadQuadCmd;
        dummyClks = psramCfg->fReadQuadDmyClk;
    } else {
        return ERROR;
    }

    /* Prepare command */
    psramCmd.rwFlag = SF_CTRL_READ;
    psramCmd.addrSize = 3;
    psramCmd.dummyClks = dummyClks;

    if (psramCfg->burstLength == PSRAM_BURST_LENGTH_16_BYTES) {
        burstLen = 16;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_32_BYTES) {
        burstLen = 32;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_64_BYTES) {
        burstLen = 64;
    } else if (psramCfg->burstLength == PSRAM_BURST_LENGTH_512_BYTES) {
        burstLen = 512;
    }

    /* Read data */
    for (i = 0; i < len;) {
        /* Prepare command */
        psramCmd.cmdBuf[0] = (cmd << 24) | (addr);
        curLen = burstLen - addr % burstLen;

        if (curLen > len - i) {
            curLen = len - i;
        }

        if (curLen >= FLASH_CTRL_BUF_SIZE) {
            curLen = FLASH_CTRL_BUF_SIZE;
            psramCmd.nbData = curLen;
        } else {
            /* Make sf_ctrl word read */
            psramCmd.nbData = ((curLen + 3) >> 2) << 2;
        }

        SF_Ctrl_SendCmd(&psramCmd);

        while (SET == SF_Ctrl_GetBusyState()) {
        }

        BL702_MemCpy_Fast(data, psramCtrlBuf, curLen);

        addr += curLen;
        i += curLen;
        data += curLen;
    }

    return SUCCESS;
}
#endif

/*@} end of group PSRAM_Public_Functions */

/*@} end of group PSRAM */

/*@} end of group BL702_Peripheral_Driver */
