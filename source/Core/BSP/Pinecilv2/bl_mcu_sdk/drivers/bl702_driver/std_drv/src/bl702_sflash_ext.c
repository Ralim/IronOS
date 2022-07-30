/**
  ******************************************************************************
  * @file    bl702_sflash_ext.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Bouffalo Lab</center></h2>
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

#include "bl702_sflash_ext.h"
#include "bl702_sf_ctrl.h"
#include "l1c_reg.h"

/** @addtogroup  BL702_Peripheral_Driver
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
 * @brief  KH25V40 flash write protect set
 *
 * @param  flashCfg: Serial flash parameter configuration pointer
 * @param  protect: protect area
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION SFlash_KH25V40_Write_Protect(SPI_Flash_Cfg_Type *flashCfg, SFlash_Protect_Kh25v40_Type protect)
{
    uint32_t stat = 0, ret;

    SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
    if (((stat >> 2) & 0xf) == protect) {
        return SUCCESS;
    }

    stat |= ((protect << 2) & 0xff);

    ret = SFlash_Write_Enable(flashCfg);
    if (SUCCESS != ret) {
        return ERROR;
    }

    SFlash_Write_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
    SFlash_Read_Reg(flashCfg, 0, (uint8_t *)&stat, 1);
    if (((stat >> 2) & 0xf) == protect) {
        return SUCCESS;
    }

    return ERROR;
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
        BL702_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL702_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    flashCmd.cmdBuf[0] = readRegCmd << 24;
    flashCmd.rwFlag = SF_CTRL_READ;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    while (SET == SF_Ctrl_GetBusyState()) {
        BL702_Delay_US(1);
        cnt++;

        if (cnt > 1000) {
            return ERROR;
        }
    }

    BL702_MemCpy(regValue, flashCtrlBuf, regLen);
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
        BL702_MemSet4((uint32_t *)&flashCmd, 0, sizeof(flashCmd) / 4);
    } else {
        BL702_MemSet(&flashCmd, 0, sizeof(flashCmd));
    }

    BL702_MemCpy(flashCtrlBuf, regValue, regLen);

    flashCmd.cmdBuf[0] = writeRegCmd << 24;
    flashCmd.rwFlag = SF_CTRL_WRITE;
    flashCmd.nbData = regLen;

    SF_Ctrl_SendCmd(&flashCmd);

    /* take 40ms for tw(write status register) as default */
    while (SET == SFlash_Busy(flashCfg)) {
        BL702_Delay_US(100);
        cnt++;

        if (cnt > 400) {
            return ERROR;
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

/*@} end of group BL702_Peripheral_Driver */
