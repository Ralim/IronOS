/**
  ******************************************************************************
  * @file    bl602_sflash_ext.c
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

#include "bl602_l1c.h"
#include "bl602_sflash_ext.h"
#include "bl602_sf_ctrl.h"
#include "l1c_reg.h"

/** @addtogroup  BL602_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SFLASH_EXT
 *  @{
 */

/** @defgroup  SFLASH_EXT_Private_Macros
 *  @{
 */

/*@} end of group SFLASH_EXT_Private_Macros */

/** @defgroup  SFLASH_EXT_Private_Types
 *  @{
 */

/*@} end of group SFLASH_EXT_Private_Types */

/** @defgroup  SFLASH_EXT_Private_Variables
 *  @{
 */
#define SFCTRL_BUSY_STATE_TIMEOUT (5 * 160 * 1000)

/*@} end of group SFLASH_EXT_Private_Variables */

/** @defgroup  SFLASH_EXT_Global_Variables
 *  @{
 */

/*@} end of group SFLASH_EXT_Global_Variables */

/** @defgroup  SFLASH_EXT_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SFLASH_EXT_Private_Fun_Declaration */

/** @defgroup  SFLASH_EXT_Private_Functions
 *  @{
 */

/*@} end of group SFLASH_EXT_Private_Functions */

/** @defgroup  SFLASH_EXT_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Sflash restore from power down
 *
 * @param  pFlashCfg: Flash configuration pointer
 * @param  flashContRead: Whether enable continuous read
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Restore_From_Powerdown(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead)
{
    BL_Err_Type stat = SUCCESS;
    uint32_t jdecId = 0;
    uint8_t tmp[8];
    uint8_t ioMode = pFlashCfg->ioMode & 0xf;

    /* Wake flash up from power down */
    SFlash_Releae_Powerdown(pFlashCfg);
    BL602_Delay_US(120);

    SFlash_GetJedecId(pFlashCfg, (uint8_t *)&jdecId);

    if (SF_CTRL_QO_MODE == ioMode || SF_CTRL_QIO_MODE == ioMode) {
        SFlash_Qspi_Enable(pFlashCfg);
    }

    if (((pFlashCfg->ioMode >> 4) & 0x01) == 1) {
        /* unwrap */
        L1C_Set_Wrap(DISABLE);
    } else {
        /* burst wrap */
        L1C_Set_Wrap(ENABLE);
        /* For command that is setting register instead of send command, we need write enable */
        SFlash_Write_Enable(pFlashCfg);
        SFlash_SetBurstWrap(pFlashCfg);
    }

    if (flashContRead) {
        stat = SFlash_Read(pFlashCfg, ioMode, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));
        stat = SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32);
    } else {
        stat = SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 0, 0, 32);
    }

    return stat;
}

/****************************************************************************/ /**
 * @brief  Sflash enable RCV mode to recovery for erase while power drop
 *
 * @param  pFlashCfg: Flash configuration pointer
 * @param  rCmd: Read RCV register cmd
 * @param  wCmd: Write RCV register cmd
 * @param  bitPos: RCV register bit pos
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_RCV_Enable(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t rCmd, uint8_t wCmd, uint8_t bitPos)
{
    BL_Err_Type stat;
    uint32_t cnt = 0;
    uint32_t tempVal = 0;

    while (SET == SFlash_Busy(pFlashCfg)) {
        BL602_Delay_US(500);
        cnt++;

        if (cnt > 20000 * 3) {
            return ERROR;
        }
    }

    stat = SFlash_Read_Reg_With_Cmd(pFlashCfg, rCmd, (uint8_t *)&tempVal, 1);

    if (SUCCESS != stat) {
        stat = ERROR;
    }

    if (((tempVal >> bitPos) & 0x01) > 0) {
        return SUCCESS;
    }

    tempVal |= (uint32_t)(1 << bitPos);
    stat = SFlash_Write_Enable(pFlashCfg);

    if (SUCCESS != stat) {
        stat = ERROR;
    }

    stat = SFlash_Write_Reg_With_Cmd(pFlashCfg, wCmd, (uint8_t *)&tempVal, 1);

    if (SUCCESS != stat) {
        return stat;
    }

    while (SET == SFlash_Busy(pFlashCfg)) {
        BL602_Delay_US(500);
        cnt++;

        if (cnt > 20000 * 3) {
            return ERROR;
        }
    }

    stat = SFlash_Read_Reg_With_Cmd(pFlashCfg, rCmd, (uint8_t *)&tempVal, 1);

    if (SUCCESS != stat) {
        stat = ERROR;
    }

    if (((tempVal >> bitPos) & 0x01) <= 0) {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Erase flash security register one block
 *
 * @param  pFlashCfg: Flash configuration pointer
 * @param  pSecRegCfg: Security register configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Erase_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg)
{
    uint32_t cnt = 0;
    uint8_t cmd = 0;
    uint8_t secOptMode = 0;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (pSecRegCfg->enterSecOptCmd != 0x00) {
        secOptMode = 1;

        if (((uint32_t)&flashCmd) % 4 == 0) {
            BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
        } else {
            BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
        }

        flashCmd.cmdBuf[0] = (pSecRegCfg->enterSecOptCmd << 24);
        flashCmd.rwFlag = SF_CTRL_WRITE;
        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }
    }

    BL_Err_Type stat = SFlash_Write_Enable(pFlashCfg);

    if (stat != SUCCESS) {
        return stat;
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    cmd = pSecRegCfg->eraseCmd;
    flashCmd.cmdBuf[0] = (cmd << 24) | (pSecRegCfg->blockNum << 12);
    /* rwFlag don't care */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SFlash_Busy(pFlashCfg)) {
        BL602_Delay_US(500);
        cnt++;

        if (cnt > pFlashCfg->timeEsector * 3) {
            return ERROR;
        }
    }

    if (secOptMode > 0) {
        if (((uint32_t)&flashCmd) % 4 == 0) {
            BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
        } else {
            BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
        }

        flashCmd.cmdBuf[0] = (pSecRegCfg->exitSecOptCmd << 24);
        flashCmd.rwFlag = SF_CTRL_WRITE;
        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Program flash security register one block
 *
 * @param  pFlashCfg: Flash configuration pointer
 * @param  pSecRegCfg: Security register configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Program_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t i = 0, curLen = 0;
    uint32_t cnt = 0;
    BL_Err_Type stat;
    uint8_t cmd;
    uint8_t secOptMode = 0;
    uint8_t *data = pSecRegCfg->data;
    uint32_t addr = pSecRegCfg->addr;
    uint32_t len = pSecRegCfg->len;
    uint32_t currentAddr = 0;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (pSecRegCfg->enterSecOptCmd != 0x00) {
        secOptMode = 1;

        if (((uint32_t)&flashCmd) % 4 == 0) {
            BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
        } else {
            BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
        }

        flashCmd.cmdBuf[0] = (pSecRegCfg->enterSecOptCmd << 24);
        flashCmd.rwFlag = SF_CTRL_WRITE;
        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Prepare command */
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.addrSize = 3;
    cmd = pSecRegCfg->programCmd;

    for (i = 0; i < len;) {
        /* Write enable is needed for every program */
        stat = SFlash_Write_Enable(pFlashCfg);

        if (stat != SUCCESS) {
            return stat;
        }

        /* Get current programmed length within page size */
        curLen = 256 - addr % 256;

        if (curLen > len - i) {
            curLen = len - i;
        }

        currentAddr = (pSecRegCfg->blockNum << 12) | addr;

        /* Prepare command */
        BL602_MemCpy_Fast(flashCtrlBuf, data, curLen);
        flashCmd.cmdBuf[0] = (cmd << 24) | (currentAddr);
        flashCmd.nbData = curLen;

        SF_Ctrl_SendCmd(&flashCmd);

        /* Adjust address and programmed length */
        addr += curLen;
        i += curLen;
        data += curLen;

        /* Wait for write done */
        cnt = 0;

        while (SET == SFlash_Busy(pFlashCfg)) {
            BL602_Delay_US(100);
            cnt++;

            if (cnt > pFlashCfg->timePagePgm * 20) {
                return ERROR;
            }
        }
    }

    if (secOptMode > 0) {
        if (((uint32_t)&flashCmd) % 4 == 0) {
            BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
        } else {
            BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
        }

        flashCmd.cmdBuf[0] = (pSecRegCfg->exitSecOptCmd << 24);
        flashCmd.rwFlag = SF_CTRL_WRITE;
        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Read data from flash security register one block
 *
 * @param  pSecRegCfg: Security register configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Read_Security_Register(SFlash_Sec_Reg_Cfg *pSecRegCfg)
{
    uint8_t *const flashCtrlBuf = (uint8_t *)SF_CTRL_BUF_BASE;
    uint32_t curLen, i;
    uint8_t cmd;
    uint8_t secOptMode = 0;
    uint8_t *data = pSecRegCfg->data;
    uint32_t addr = pSecRegCfg->addr;
    uint32_t len = pSecRegCfg->len;
    uint32_t currentAddr = 0;
    uint32_t timeOut = 0;
    SF_Ctrl_Cmd_Cfg_Type flashCmd;

    if (pSecRegCfg->enterSecOptCmd != 0x00) {
        secOptMode = 1;

        if (((uint32_t)&flashCmd) % 4 == 0) {
            BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
        } else {
            BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
        }

        flashCmd.cmdBuf[0] = (pSecRegCfg->enterSecOptCmd << 24);
        flashCmd.rwFlag = SF_CTRL_WRITE;
        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }
    }

    if (((uint32_t)&flashCmd) % 4 == 0) {
        BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    /* Prepare command */
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.addrSize = 3;
    flashCmd.dummyClks = 1;
    cmd = pSecRegCfg->readCmd;

    /* Read data */
    for (i = 0; i < len;) {
        currentAddr = (pSecRegCfg->blockNum << 12) | addr;
        /* Prepare command */
        flashCmd.cmdBuf[0] = (cmd << 24) | (currentAddr);
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

    if (secOptMode > 0) {
        if (((uint32_t)&flashCmd) % 4 == 0) {
            BL602_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
        } else {
            BL602_MemSet(&flashCmd, 0, sizeof(flashCmd));
        }

        flashCmd.cmdBuf[0] = (pSecRegCfg->exitSecOptCmd << 24);
        flashCmd.rwFlag = SF_CTRL_WRITE;
        SF_Ctrl_SendCmd(&flashCmd);

        timeOut = SFCTRL_BUSY_STATE_TIMEOUT;

        while (SET == SF_Ctrl_GetBusyState()) {
            timeOut--;

            if (timeOut == 0) {
                return TIMEOUT;
            }
        }
    }

    return SUCCESS;
}

/****************************************************************************//**
 * @brief  Clear flash status register
 *
 * @param  pFlashCfg: Flash configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t ret = 0;
    uint32_t qeValue = 0;
    uint32_t regValue = 0;
    uint32_t readValue = 0;
    uint8_t readRegValue0 = 0;
    uint8_t readRegValue1 = 0;

    if((pFlashCfg->ioMode&0xf)==SF_CTRL_QO_MODE || (pFlashCfg->ioMode&0xf)==SF_CTRL_QIO_MODE){
        qeValue = 1;
    }

    SFlash_Read_Reg(pFlashCfg, 0, (uint8_t *)&readRegValue0, 1);
    SFlash_Read_Reg(pFlashCfg, 1, (uint8_t *)&readRegValue1, 1);
    readValue = (readRegValue0|(readRegValue1<<8));
    if ((readValue & (~((1<<(pFlashCfg->qeIndex*8+pFlashCfg->qeBit)) |
                        (1<<(pFlashCfg->busyIndex*8+pFlashCfg->busyBit)) |
                        (1<<(pFlashCfg->wrEnableIndex*8+pFlashCfg->wrEnableBit))))) == 0){
        return SUCCESS;
    }

    ret = SFlash_Write_Enable(pFlashCfg);
    if (SUCCESS != ret) {
        return ERROR;
    }
    if (pFlashCfg->qeWriteRegLen == 2) {
        regValue = (qeValue<<(pFlashCfg->qeIndex*8+pFlashCfg->qeBit));
        SFlash_Write_Reg(pFlashCfg, 0, (uint8_t *)&regValue, 2);
    } else {
        if (pFlashCfg->qeIndex == 0) {
            regValue = (qeValue<<pFlashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(pFlashCfg, 0, (uint8_t *)&regValue, 1);
        ret = SFlash_Write_Enable(pFlashCfg);
        if (SUCCESS != ret) {
            return ERROR;
        }
        if (pFlashCfg->qeIndex == 1) {
            regValue = (qeValue<<pFlashCfg->qeBit);
        } else {
            regValue = 0;
        }
        SFlash_Write_Reg(pFlashCfg, 1, (uint8_t *)&regValue, 1);
    }
    return SUCCESS;
}

/*@} end of group SFLASH_EXT_Public_Functions */

/*@} end of group SFLASH_EXT */

/*@} end of group BL602_Peripheral_Driver */
