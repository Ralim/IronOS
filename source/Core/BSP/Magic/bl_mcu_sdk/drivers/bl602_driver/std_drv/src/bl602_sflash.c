/**
  ******************************************************************************
  * @file    bl602_sflash.c
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

#include "string.h"
#include "bl602_sflash.h"
#include "bl602_sf_ctrl.h"
#include "bl602_l1c.h"

/** @addtogroup  BL602_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SFLASH
 *  @{
 */

/** @defgroup  SFLASH_Private_Macros
 *  @{
 */

/*@} end of group SFLASH_Private_Macros */

/** @defgroup  SFLASH_Private_Types
 *  @{
 */

/*@} end of group SFLASH_Private_Types */

/** @defgroup  SFLASH_Private_Variables
 *  @{
 */
#define SFCTRL_BUSY_STATE_TIMEOUT (5 * 160 * 1000)

/*@} end of group SFLASH_Private_Variables */

/** @defgroup  SFLASH_Global_Variables
 *  @{
 */

/*@} end of group SFLASH_Global_Variables */

/** @defgroup  SFLASH_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SFLASH_Private_Fun_Declaration */

/** @defgroup  SFLASH_Private_Functions
 *  @{
 */

/*@} end of group SFLASH_Private_Functions */

/** @defgroup  SFLASH_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Init serial flash control interface
 *
 * @param  pSfCtrlCfg: Serial flash controller configuration pointer
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SFlash_Init(const SF_Ctrl_Cfg_Type *pSfCtrlCfg)
{
    SF_Ctrl_Enable(pSfCtrlCfg);
}

/****************************************************************************/ /**
 * @brief  Set serial flash control interface SPI or QPI mode
 *
 * @param  mode: Serial flash interface mode
 *
 * @return BFLB_RET:SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_SetSPIMode(SF_Ctrl_Mode_Type mode)
{
    BL_Err_Type stat = SUCCESS;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_MODE_TYPE(mode));

    return stat;
}

/****************************************************************************/ /**
 * @brief  Read flash register
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  regIndex: register index
 * @param  regValue: register value pointer to store data
 * @param  regLen: register value length
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Read_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->readRegCmd[regIndex]) << 24;
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
        BL602_Delay_US(1);
        cnt++;

        if (cnt > 1000) {
            return ERROR;
        }
    }

    BL602_MemCpy(regValue, flashCtrlBuf, regLen);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Write flash register
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  regIndex: register index
 * @param  regValue: register value pointer storing data
 * @param  regLen: register value length
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Write_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    BL602_MemCpy(flashCtrlBuf, regValue, regLen);

    flashCmd.cmdBuf[0] = (flashCfg->writeRegCmd[regIndex]) << 24;
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    /* take 40ms for tw(write status register) as default */
    while (SET == SFlash_Busy(flashCfg)) {
        BL602_Delay_US(100);
        cnt++;

        if (cnt > 400) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Check flash busy status
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return SET for busy or RESET for not busy
 *
*******************************************************************************/
__WEAK
BL_Sts_Type ATTR_TCM_SECTION SFlash_Busy(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t stat = 0;
    SFlash_Read_Reg(flashCfg, flashCfg->busyIndex, (uint8_t *)&stat, flashCfg->busyReadRegLen);

    if ((stat & (1 << flashCfg->busyBit)) == 0) {
        return RESET;
    }

    return SET;
}

/****************************************************************************/ /**
 * @brief  Enable flash write function
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Write_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t stat = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Write enable*/
    flashCmd.cmdBuf[0] = (flashCfg->writeEnableCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    SF_Ctrl_SendCmd(&flashCmd);

    SFlash_Read_Reg(flashCfg, flashCfg->wrEnableIndex, (uint8_t *)&stat, flashCfg->wrEnableReadRegLen);

    if ((stat & (1 << flashCfg->wrEnableBit)) != 0) {
        return SUCCESS;
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Enable flash flash controller QSPI interface
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Qspi_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    uint32_t stat = 0, ret;

    if (flashCfg->qeReadRegLen == 0) {
        ret = SFlash_Write_Enable(flashCfg);

        if (SUCCESS != ret) {
            return ERROR;
        }

        SFlash_Write_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeWriteRegLen);
        return SUCCESS;
    }

    SFlash_Read_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeReadRegLen);

    if (flashCfg->qeData == 0) {
        if ((stat & (1 << flashCfg->qeBit)) != 0) {
            return SUCCESS;
        }
    } else {
        if (((stat >> (flashCfg->qeBit & 0x08)) & 0xff) == flashCfg->qeData) {
            return SUCCESS;
        }
    }

    if (flashCfg->qeWriteRegLen != 1) {
        /* This is  read r0,read r1 write r0,r1 case*/
        SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
        SFlash_Read_Reg(flashCfg, 1, ((uint8_t *)&stat) + 1, 1);

        if (flashCfg->qeData == 0) {
            stat |= (1 << (flashCfg->qeBit + 8 * flashCfg->qeIndex));
        } else {
            stat = stat & (~(0xff << (8 * flashCfg->qeIndex)));
            stat |= (flashCfg->qeData << (8 * flashCfg->qeIndex));
        }
    } else {
        if (flashCfg->qeData == 0) {
            stat |= (1 << (flashCfg->qeBit % 8));
        } else {
            stat = flashCfg->qeData;
        }
    }

    ret = SFlash_Write_Enable(flashCfg);

    if (SUCCESS != ret) {
        return ERROR;
    }

    SFlash_Write_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeWriteRegLen);
    SFlash_Read_Reg(flashCfg, flashCfg->qeIndex, (uint8_t *)&stat, flashCfg->qeReadRegLen);

    if (flashCfg->qeData == 0) {
        if ((stat & (1 << flashCfg->qeBit)) != 0) {
            return SUCCESS;
        }
    } else {
        if (((stat >> (flashCfg->qeBit & 0x08)) & 0xff) == flashCfg->qeData) {
            return SUCCESS;
        }
    }

    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Enable flash volatile register write enable
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_Volatile_Reg_Write_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->writeVregEnableCmd) << 24;
    flashCmd.rwFlag = SF_CTRL_WRITE;

    SF_Ctrl_SendCmd(&flashCmd);
}

/****************************************************************************/ /**
 * @brief  Erase flash whole chip
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Chip_Erase(SPI_Flash_Cfg_Type *flashCfg)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;
    BL_Err_Type stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->chipEraseCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        BL602_Delay_US(500);
        cnt++;

        if (cnt > flashCfg->timeCe * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Erase flash one sector
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  secNum: flash sector number
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Sector_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t secNum)
{
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    BL_Err_Type stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->sectorEraseCmd << 24) | (flashCfg->sectorSize * 1024 * secNum);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        BL602_Delay_US(500);
        cnt++;

        if (cnt > flashCfg->timeEsector * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Erase flash one 32K block
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  blkNum: flash 32K block number
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    BL_Err_Type stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->blk32EraseCmd << 24) | (BFLB_SPIFLASH_BLK32K_SIZE * blkNum);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        BL602_Delay_US(500);
        cnt++;

        if (cnt > flashCfg->timeE32k * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Erase flash one 64K block
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  blkNum: flash 64K block number
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Blk64_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;
    BL_Err_Type stat = SFlash_Write_Enable(flashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = (flashCfg->blk64EraseCmd << 24) | (BFLB_SPIFLASH_BLK64K_SIZE * blkNum);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(flashCfg)) {
        BL602_Delay_US(500);
        cnt++;

        if (cnt > flashCfg->timeE64k * 3) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Erase flash one region
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  startaddr: start address to erase
 * @param  endaddr: end address(include this address) to erase
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr)
{
    uint32_t len = 0;
    uint32_t eraseLen = 0;
    BL_Err_Type ret = SUCCESS;

    while (startaddr <= endaddr) {
        len = endaddr - startaddr + 1;

        if (flashCfg->blk64EraseCmd != BFLB_SPIFLASH_CMD_INVALID &&
            (startaddr & (BFLB_SPIFLASH_BLK64K_SIZE - 1)) == 0 &&
            len > (uint32_t)(BFLB_SPIFLASH_BLK64K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 64K margin address,and length > 64K-sector size, erase one first */
            ret = SFlash_Blk64_Erase(flashCfg, startaddr / BFLB_SPIFLASH_BLK64K_SIZE);
            eraseLen = BFLB_SPIFLASH_BLK64K_SIZE;
        } else if (flashCfg->blk32EraseCmd != BFLB_SPIFLASH_CMD_INVALID &&
                   (startaddr & (BFLB_SPIFLASH_BLK32K_SIZE - 1)) == 0 &&
                   len > (uint32_t)(BFLB_SPIFLASH_BLK32K_SIZE - flashCfg->sectorSize * 1024)) {
            /* 32K margin address,and length > 32K-sector size, erase one first */
            ret = SFlash_Blk32_Erase(flashCfg, startaddr / BFLB_SPIFLASH_BLK32K_SIZE);
            eraseLen = BFLB_SPIFLASH_BLK32K_SIZE;
        } else {
            /* Sector erase */
            startaddr = ((startaddr) & (~(flashCfg->sectorSize * 1024 - 1)));
            ret = SFlash_Sector_Erase(flashCfg, startaddr / flashCfg->sectorSize / 1024);
            eraseLen = flashCfg->sectorSize * 1024;
        }

        startaddr += eraseLen;

        if (ret != SUCCESS) {
            return ERROR;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Program flash one region
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  ioMode: progran mode:SPI mode or QPI mode
 * @param  addr: start address to be programed
 * @param  data: data pointer to be programed
 * @param  len: data length to be programed
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Program(SPI_Flash_Cfg_Type *flashCfg,
                                            SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t i = 0, curLen = 0;
    uint32_t cnt = 0;
    BL_Err_Type stat;
    uint8_t cmd;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode || SF_CTRL_DO_MODE == ioMode || SF_CTRL_DIO_MODE == ioMode) {
        cmd = flashCfg->pageProgramCmd;
    } else if (SF_CTRL_QIO_MODE == ioMode || SF_CTRL_QO_MODE == ioMode) {
        flashCmd.addrMode = (SF_Ctrl_Addr_Mode_Type)flashCfg->qppAddrMode;
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->qpageProgramCmd;
    } else {
        return ERROR;
    }

    /* Prepare command */
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.addrSize = 3;

    for (i = 0; i < len;) {
        /* Write enable is needed for every program */
        stat = SFlash_Write_Enable(flashCfg);

        if (stat != SUCCESS) {
            return stat;
        }

        /* Get current programmed length within page size */
        curLen = flashCfg->pageSize - addr % flashCfg->pageSize;

        if (curLen > len - i) {
            curLen = len - i;
        }

        /* Prepare command */
        BL602_MemCpy_Fast(flashCtrlBuf, data, curLen);
        flashCmd.cmdBuf[0] = (cmd << 24) | (addr);
        flashCmd.nbData = curLen;

        SF_Ctrl_SendCmd(&flashCmd);

        /* Adjust address and programmed length */
        addr += curLen;
        i += curLen;
        data += curLen;

        /* Wait for write done */
        cnt = 0;

        while (SET == SFlash_Busy(flashCfg)) {
            BL602_Delay_US(100);
            cnt++;

            if (cnt > flashCfg->timePagePgm * 20) {
                return ERROR;
            }
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get flash unique ID
 *
 * @param  data: data pointer to store read data
 * @param  idLen: unique ID len
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_GetUniqueId(uint8_t *data, uint8_t idLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    dummyClks = 4;
    cmd = 0x4B;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = idLen;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    BL602_MemCpy(data, flashCtrlBuf, idLen);
}

/****************************************************************************/ /**
 * @brief  Get flash jedec ID
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  data: data pointer to store read data
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_GetJedecId(SPI_Flash_Cfg_Type *flashCfg, uint8_t *data)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    dummyClks = flashCfg->jedecIdCmdDmyClk;
    cmd = flashCfg->jedecIdCmd;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    BL602_MemCpy(data, flashCtrlBuf, 3);
}

/****************************************************************************/ /**
 * @brief  Get flash device ID
 *
 * @param  data: data pointer to store read data
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_GetDeviceId(uint8_t *data)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t addr = 0x00000001;
    uint8_t readMode = 0xFF;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.addrMode = SF_CTRL_ADDR_4_LINES;
    flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
    dummyClks = 2;
    cmd = 0x94;
    flashCmd.cmdBuf[0] = (cmd << 24) | (addr);
    flashCmd.cmdBuf[1] = (readMode << 24);
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 4;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 2;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    BL602_MemCpy(data, flashCtrlBuf, 2);
}

/****************************************************************************/ /**
 * @brief  Set flash power down
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_Powerdown(void)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint8_t cmd = 0;
    uint32_t timeOut = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    cmd = 0xB9;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }
}

/****************************************************************************/ /**
 * @brief  Release flash power down for wake up
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_Releae_Powerdown(SPI_Flash_Cfg_Type *flashCfg)
{
    uint8_t cmd;
    uint32_t timeOut = 0;

    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    cmd = flashCfg->releasePowerDown;
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;

    SF_Ctrl_SendCmd(&flashCmd);

    timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }
}

/****************************************************************************/ /**
 * @brief  Set flash burst wrap config
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_SetBurstWrap(SPI_Flash_Cfg_Type *flashCfg)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t wrapData;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((flashCfg->ioMode >> 4) & 0x01) == 1) {
        /* Disable burst wrap ,just return */
        return;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.addrMode = (SF_Ctrl_Addr_Mode_Type)flashCfg->burstWrapDataMode;
    flashCmd.dataMode = (SF_Ctrl_Data_Mode_Type)flashCfg->burstWrapDataMode;
    dummyClks = flashCfg->burstWrapCmdDmyClk;
    cmd = flashCfg->burstWrapCmd;
    wrapData = flashCfg->burstWrapData;
    BL602_MemCpy4((uint32_t *)flashCtrlBuf, &wrapData, 4);
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 1;

    SF_Ctrl_SendCmd(&flashCmd);
}

/****************************************************************************/ /**
 * @brief  Disable flash burst wrap config
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_DisableBurstWrap(SPI_Flash_Cfg_Type *flashCfg)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint8_t cmd, dummyClks;
    uint32_t wrapData;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.addrMode = (SF_Ctrl_Addr_Mode_Type)flashCfg->deBurstWrapDataMode;
    flashCmd.dataMode = (SF_Ctrl_Data_Mode_Type)flashCfg->deBurstWrapDataMode;
    dummyClks = flashCfg->deBurstWrapCmdDmyClk;
    cmd = flashCfg->deBurstWrapCmd;
    wrapData = flashCfg->deBurstWrapData;
    BL602_MemCpy4((uint32_t *)flashCtrlBuf, &wrapData, 4);
    flashCmd.cmdBuf[0] = (cmd << 24);
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = 1;

    SF_Ctrl_SendCmd(&flashCmd);
}

/****************************************************************************/ /**
 * @brief  Software reset flash
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Software_Reset(SPI_Flash_Cfg_Type *flashCfg)
{
    uint16_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Reset enable */
    flashCmd.cmdBuf[0] = (flashCfg->resetEnCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;

    /* Wait for write done */
    while (SET == SFlash_Busy(flashCfg)) {
        BL602_Delay_US(100);
        cnt++;

        if (cnt > 20) {
            return ERROR;
        }
    }

    SF_Ctrl_SendCmd(&flashCmd);

    /* Reset */
    flashCmd.cmdBuf[0] = (flashCfg->resetCmd) << 24;
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    SF_Ctrl_SendCmd(&flashCmd);

    BL602_Delay_US(50);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Reset flash continous read mode
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_Reset_Continue_Read(SPI_Flash_Cfg_Type *flashCfg)
{
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Reset continous read */
    BL602_MemSet(&flashCmd.cmdBuf[0], flashCfg->resetCreadCmd, 4);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = flashCfg->resetCreadCmdSize;
    SF_Ctrl_SendCmd(&flashCmd);
}

/****************************************************************************/ /**
 * @brief  Set I/D bus read flash configuration in flash controller
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  ioMode: flash controller interface mode
 * @param  contRead: Wether enable cont read mode
 * @param  addr: address to read/write
 * @param  len: data length to read/write
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Set_IDbus_Cfg(SPI_Flash_Cfg_Type *flashCfg,
                                                  SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint32_t len)
{
    uint8_t cmd, dummyClks;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint8_t cmdValid = 1;
    uint8_t noReadModeCfg = 0;
    uint8_t cReadSupport = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_IAHB);

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = flashCfg->fastReadCmd;
        dummyClks = flashCfg->frDmyClk;
    } else if (SF_CTRL_DO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDoCmd;
        dummyClks = flashCfg->frDoDmyClk;
    } else if (SF_CTRL_DIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_2_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDioCmd;
        dummyClks = flashCfg->frDioDmyClk;
    } else if (SF_CTRL_QO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQoCmd;
        dummyClks = flashCfg->frQoDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQioCmd;
        dummyClks = flashCfg->frQioDmyClk;
    } else {
        return ERROR;
    }

    /*prepare command**/
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;
    flashCmd.cmdBuf[0] = (cmd << 24) | addr;

    if (SF_CTRL_QIO_MODE == ioMode || SF_CTRL_DIO_MODE == ioMode) {
        noReadModeCfg = flashCfg->cReadSupport & 0x02;
        cReadSupport = flashCfg->cReadSupport & 0x01;

        if (noReadModeCfg == 0) {
            /* Read mode must be set*/
            if (cReadSupport == 0) {
                /* Not support cont read,but we still need set read mode(winbond 80dv)*/
                flashCmd.cmdBuf[1] = (flashCfg->cReadMode << 24);
            } else {
                /* Flash support cont read, setting depend on user parameter */
                if (contRead) {
                    flashCmd.cmdBuf[0] = (addr << 8) | flashCfg->cReadMode;
                    cmdValid = 0;
                } else {
                    flashCmd.cmdBuf[1] = ((!flashCfg->cReadMode) << 24);
                }
            }

            flashCmd.addrSize++;
        }
    }

    flashCmd.dummyClks = dummyClks;
    flashCmd.nbData = len;
    SF_Ctrl_Icache_Set(&flashCmd, cmdValid);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable I/D bus read from flash
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  ioMode: flash controller interface mode
 * @param  contRead: Wether enable cont read mode
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_IDbus_Read_Enable(SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead)
{
    BL_Err_Type stat;

    stat = SFlash_Set_IDbus_Cfg(flashCfg, ioMode, contRead, 0, 4);

    if (SUCCESS != stat) {
        return stat;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable cache
 *
 * @param  wayDisable: cache way disable config
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Cache_Enable_Set(uint8_t wayDisable)
{
    uint32_t tmpVal;
    uint32_t cnt = 0;

    /* Set cacheable to 0 */
    tmpVal = BL_RD_REG(L1C_BASE, L1C_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_CACHEABLE);
    tmpVal = BL_SET_REG_BIT(tmpVal, L1C_BYPASS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_WAY_DIS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_CNT_EN);
    BL_WR_REG(L1C_BASE, L1C_CONFIG, tmpVal);

    tmpVal = BL_RD_REG(L1C_BASE, L1C_CONFIG);
    /*Set Tag RAM to zero */
    tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_INVALID_EN);
    BL_WR_REG(L1C_BASE, L1C_CONFIG, tmpVal);
    /* Left space for hardware change status*/
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    tmpVal = BL_SET_REG_BIT(tmpVal, L1C_INVALID_EN);
    BL_WR_REG(L1C_BASE, L1C_CONFIG, tmpVal);
    /* Left space for hardware change status*/
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Polling for invalid done */
    do {
        BL602_Delay_US(1);
        cnt++;
        tmpVal = BL_RD_REG(L1C_BASE, L1C_CONFIG);
    } while (!BL_IS_REG_BIT_SET(tmpVal, L1C_INVALID_DONE) && cnt < 100);

    tmpVal = BL_SET_REG_BIT(tmpVal, L1C_BYPASS);
    BL_WR_REG(L1C_BASE, L1C_CONFIG, tmpVal);

    tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_BYPASS);
    tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_WAY_DIS);
    tmpVal = BL_SET_REG_BIT(tmpVal, L1C_CNT_EN);
    BL_WR_REG(L1C_BASE, L1C_CONFIG, tmpVal);

    tmpVal |= (wayDisable << L1C_WAY_DIS_POS);

    /* If way disable is 0x0f, cacheable can't be set */
    if (wayDisable != 0x0f) {
        tmpVal = BL_SET_REG_BIT(tmpVal, L1C_CACHEABLE);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_CACHEABLE);
    }

    BL_WR_REG(L1C_BASE, L1C_CONFIG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Flush cache
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Cache_Flush(void)
{
    uint32_t tmpVal;

    /* Disable early respone */
    tmpVal = BL_RD_REG(L1C_BASE, L1C_CONFIG);
    SFlash_Cache_Enable_Set((tmpVal >> L1C_WAY_DIS_POS) & 0xf);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable cache read from flash with cache
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  ioMode: flash controller interface mode
 * @param  contRead: Wether enable cont read mode
 * @param  wayDisable: cache way disable config
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Cache_Read_Enable(SPI_Flash_Cfg_Type *flashCfg,
                                                      SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint8_t wayDisable)
{
    BL_Err_Type stat;

    /* Cache now only support 32 bytes read */
    stat = SFlash_Set_IDbus_Cfg(flashCfg, ioMode, contRead, 0, 32);

    if (SUCCESS != stat) {
        return stat;
    }

    return SFlash_Cache_Enable_Set(wayDisable);
}

/****************************************************************************/ /**
 * @brief  Get cache hit count
 *
 * @param  hitCountLow: hit count low 32 bits pointer
 * @param  hitCountHigh: hit count high 32 bits pointer
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_Cache_Hit_Count_Get(uint32_t *hitCountLow, uint32_t *hitCountHigh)
{
    *hitCountLow = BL_RD_REG(L1C_BASE, L1C_HIT_CNT_LSB);
    *hitCountHigh = BL_RD_REG(L1C_BASE, L1C_HIT_CNT_MSB);
}

/****************************************************************************/ /**
 * @brief  Get cache miss count
 *
 * @param  None
 *
 * @return Cache miss count
 *
*******************************************************************************/
__WEAK
uint32_t ATTR_TCM_SECTION SFlash_Cache_Miss_Count_Get(void)
{
    return BL_RD_REG(L1C_BASE, L1C_MISS_CNT);
}

/****************************************************************************/ /**
 * @brief  Disable read from flash with cache
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
__WEAK
void ATTR_TCM_SECTION SFlash_Cache_Read_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(L1C_BASE, L1C_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, L1C_CACHEABLE);
    BL_WR_REG(L1C_BASE, L1C_CONFIG, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Read data from flash
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  ioMode: flash controller interface mode
 * @param  contRead: Wether enable cont read mode
 * @param  addr: flash read start address
 * @param  data: data pointer to store data read from flash
 * @param  len: data length to read
 *
 * @return None
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Read(SPI_Flash_Cfg_Type *flashCfg,
                                         SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t curLen, i;
    uint8_t cmd, dummyClks;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint8_t noReadModeCfg = 0;
    uint8_t cReadSupport = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    if (SF_CTRL_NIO_MODE == ioMode) {
        cmd = flashCfg->fastReadCmd;
        dummyClks = flashCfg->frDmyClk;
    } else if (SF_CTRL_DO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDoCmd;
        dummyClks = flashCfg->frDoDmyClk;
    } else if (SF_CTRL_DIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_2_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_2_LINES;
        cmd = flashCfg->fastReadDioCmd;
        dummyClks = flashCfg->frDioDmyClk;
    } else if (SF_CTRL_QO_MODE == ioMode) {
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQoCmd;
        dummyClks = flashCfg->frQoDmyClk;
    } else if (SF_CTRL_QIO_MODE == ioMode) {
        flashCmd.addrMode = SF_CTRL_ADDR_4_LINES;
        flashCmd.dataMode = SF_CTRL_DATA_4_LINES;
        cmd = flashCfg->fastReadQioCmd;
        dummyClks = flashCfg->frQioDmyClk;
    } else {
        return ERROR;
    }

    /* Prepare command */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    if (SF_CTRL_QIO_MODE == ioMode || SF_CTRL_DIO_MODE == ioMode) {
        noReadModeCfg = flashCfg->cReadSupport & 0x02;
        cReadSupport = flashCfg->cReadSupport & 0x01;

        if (noReadModeCfg == 0) {
            /* Read mode must be set*/
            if (cReadSupport == 0) {
                /* Not support cont read,but we still need set read mode(winbond 80dv)*/
                flashCmd.cmdBuf[1] = (flashCfg->cReadMode << 24);
            } else {
                /* Flash support cont read, setting depend on user parameter */
                if (contRead) {
                    flashCmd.cmdBuf[1] = (flashCfg->cReadMode << 24);
                } else {
                    flashCmd.cmdBuf[1] = ((!flashCfg->cReadMode) << 24);
                }
            }

            flashCmd.addrSize++;
        }
    }

    flashCmd.dummyClks = dummyClks;

    /* Read data */
    for (i = 0; i < len;) {
        /* Prepare command */
        flashCmd.cmdBuf[0] = (cmd << 24) | (addr);
        curLen = len - i;

        if (curLen >= FLASH_CTRL_BUF_SIZE) {
            curLen = FLASH_CTRL_BUF_SIZE;
            flashCmd.nbData = curLen;
        } else {
            /* Make sf_ctrl word read */
            flashCmd.nbData = ((curLen + 3) >> 2) << 2;
        }

        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }

        BL602_MemCpy_Fast(data, flashCtrlBuf, curLen);

        addr += curLen;
        i += curLen;
        data += curLen;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Read flash register with read command
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  readRegCmd: read command
 * @param  regValue: register value pointer to store data
 * @param  regLen: register value length
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Read_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;
    uint32_t cnt = 0;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = readRegCmd << 24;
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
        BL602_Delay_US(1);
        cnt++;

        if (cnt > 1000) {
            return ERROR;
        }
    }

    BL602_MemCpy(regValue, flashCtrlBuf, regLen);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Write flash register with write command
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  writeRegCmd: write command
 * @param  regValue: register value pointer storing data
 * @param  regLen: register value length
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_Write_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t cnt = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    BL602_MemCpy(flashCtrlBuf, regValue, regLen);

    flashCmd.cmdBuf[0] = writeRegCmd << 24;
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    /* take 40ms for tw(write status register) as default */
    while (SET == SFlash_Busy(flashCfg)) {
        BL602_Delay_US(100);
        cnt++;

        if (cnt > 400) {
            return ERROR;
        }
    }

    return SUCCESS;
}
#endif

/*@} end of group SFLASH_Public_Functions */

/*@} end of group SFLASH */

/*@} end of group BL602_Peripheral_Driver */
