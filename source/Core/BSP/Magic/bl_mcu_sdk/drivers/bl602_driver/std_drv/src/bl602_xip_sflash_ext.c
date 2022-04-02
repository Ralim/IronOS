/**
  ******************************************************************************
  * @file    bl602_xip_sflash_ext.c
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
#include "bl602_sf_cfg.h"
#include "bl602_sf_cfg_ext.h"
#include "bl602_xip_sflash.h"
#include "bl602_xip_sflash_ext.h"

/** @addtogroup  BL602_Peripheral_Driver
 *  @{
 */

/** @addtogroup  XIP_SFLASH_EXT
 *  @{
 */

/** @defgroup  XIP_SFLASH_EXT_Private_Macros
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Private_Macros */

/** @defgroup  XIP_SFLASH_EXT_Private_Types
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Private_Types */

/** @defgroup  XIP_SFLASH_EXT_Private_Variables
 *  @{
 */

static SPI_Flash_Cfg_Type flashCfg;
static uint8_t aesEnable;

/*@} end of group XIP_SFLASH_EXT_Private_Variables */

/** @defgroup  XIP_SFLASH_EXT_Global_Variables
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Global_Variables */

/** @defgroup  XIP_SFLASH_EXT_Private_Fun_Declaration
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Private_Fun_Declaration */

/** @defgroup  XIP_SFLASH_EXT_Private_Functions
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Private_Functions */

/** @defgroup  XIP_SFLASH_EXT_Public_Functions
 *  @{
 */

/****************************************************************************//**
 * @brief  Save flash controller state
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  offset: CPU XIP flash offset pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_State_Save_Ext(SPI_Flash_Cfg_Type *pFlashCfg,uint32_t *offset)
{
    /* XIP_SFlash_Delay */
    volatile uint32_t i=32*2;
    while(i--);
    
    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
    /* Exit form continous read for accepting command */
    SFlash_Reset_Continue_Read(pFlashCfg);
    /* Send software reset command(80bv has no this command)to deburst wrap for ISSI like */
    SFlash_Software_Reset(pFlashCfg);
    /* For disable command that is setting register instaed of send command, we need write enable */
    SFlash_DisableBurstWrap(pFlashCfg);
    if ((pFlashCfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        /* Enable QE again in case reset command make it reset */
        SFlash_Qspi_Enable(pFlashCfg);
    }
    /* Deburst again to make sure */
    SFlash_DisableBurstWrap(pFlashCfg);

    /* Clear offset setting*/
    *offset=SF_Ctrl_Get_Flash_Image_Offset();
    SF_Ctrl_Set_Flash_Image_Offset(0);

    return SUCCESS;
}

/****************************************************************************//**
 * @brief  Restore flash controller state
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  offset: CPU XIP flash offset
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_State_Restore_Ext(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t offset)
{
    uint32_t tmp[1];
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    SF_Ctrl_Set_Flash_Image_Offset(offset);

    if (((pFlashCfg->ioMode >> 4) & 0x01) == 0) {
        if ((pFlashCfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(pFlashCfg);
        }
    }
    SFlash_Read(pFlashCfg, ioMode, 1, 0x0, (uint8_t *)tmp, sizeof(tmp));
    SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32);

    return SUCCESS;
}

/*@} end of group XIP_SFLASH_EXT_Public_Functions */

/** @defgroup  XIP_SFLASH_EXT_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Erase flash one region
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  startaddr: start address to erase
 * @param  endaddr: end address(include this address) to erase
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Erase_Need_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr, uint32_t endaddr)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode&0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat=XIP_SFlash_State_Save(pFlashCfg,&offset);
    if(stat!=SUCCESS){
        SFlash_Set_IDbus_Cfg(pFlashCfg,ioMode,1,0,32);
    }else{
        stat=SFlash_Erase(pFlashCfg,startaddr,endaddr);
        XIP_SFlash_State_Restore_Ext(pFlashCfg,offset);
    }
    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

/****************************************************************************/ /**
 * @brief  Program flash one region
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  addr: start address to be programed
 * @param  data: data pointer to be programed
 * @param  len: data length to be programed
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Write_Need_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode&0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat=XIP_SFlash_State_Save(pFlashCfg,&offset);
    if(stat!=SUCCESS){
        SFlash_Set_IDbus_Cfg(pFlashCfg,ioMode,1,0,32);
    }else{
        stat= SFlash_Program(pFlashCfg,ioMode,addr,data,len);
        XIP_SFlash_State_Restore_Ext(pFlashCfg,offset);
    }
    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

/****************************************************************************/ /**
 * @brief  Read data from flash
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  addr: flash read start address
 * @param  data: data pointer to store data read from flash
 * @param  len: data length to read
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Read_Need_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode&0xf;

    XIP_SFlash_Opt_Enter(&aesEnable);
    stat=XIP_SFlash_State_Save(pFlashCfg,&offset);
    if(stat!=SUCCESS){
        SFlash_Set_IDbus_Cfg(pFlashCfg,ioMode,1,0,32);
    }else{
        stat=SFlash_Read(pFlashCfg,ioMode,0,addr, data,len);
        XIP_SFlash_State_Restore_Ext(pFlashCfg,offset);
    }
    XIP_SFlash_Opt_Exit(aesEnable);

    return stat;
}

/****************************************************************************//**
 * @brief  Clear flash status register need lock
 *
 * @param  pFlashCfg: Flash config pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode&0xf;

    stat=XIP_SFlash_State_Save(pFlashCfg, &offset);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32);
    } else {
        stat=SFlash_Clear_Status_Register(pFlashCfg);
        XIP_SFlash_State_Restore_Ext(pFlashCfg, offset);
    }

    return stat;
}

/****************************************************************************//**
 * @brief  Get Flash Jedec ID
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  data: data pointer to store Jedec ID Read from flash
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_GetJedecId_Need_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    stat = XIP_SFlash_State_Save(pFlashCfg, &offset);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32);
    } else {
        SFlash_GetJedecId(pFlashCfg, data);
        XIP_SFlash_State_Restore_Ext(pFlashCfg, offset);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Flash Device ID
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  data: data pointer to store Device ID Read from flash
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_GetDeviceId_Need_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    stat = XIP_SFlash_State_Save(pFlashCfg, &offset);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32);
    } else {
        SFlash_GetDeviceId(data);
        XIP_SFlash_State_Restore_Ext(pFlashCfg, offset);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get Flash Unique ID
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  data: data pointer to store Device ID Read from flash
 * @param  idLen: Unique id len
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_GetUniqueId_Need_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t idLen)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    stat = XIP_SFlash_State_Save(pFlashCfg, &offset);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32);
    } else {
        SFlash_GetUniqueId(data, idLen);
        XIP_SFlash_State_Restore_Ext(pFlashCfg, offset);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Sflash enable RCV mode to recovery for erase while power drop need lock
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  rCmd: Read RCV register cmd
 * @param  wCmd: Write RCV register cmd
 * @param  bitPos: RCV register bit pos
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_RCV_Enable_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t rCmd, uint8_t wCmd, uint8_t bitPos)
{
    BL_Err_Type stat;
    uint32_t offset;
    SF_Ctrl_IO_Type ioMode = (SF_Ctrl_IO_Type)pFlashCfg->ioMode & 0xf;

    stat = XIP_SFlash_State_Save(pFlashCfg, &offset);
    if (stat != SUCCESS) {
        SFlash_Set_IDbus_Cfg(pFlashCfg, ioMode, 1, 0, 32);
    } else {
        stat = SFlash_RCV_Enable(pFlashCfg, rCmd, wCmd, bitPos);
        XIP_SFlash_State_Restore_Ext(pFlashCfg, offset);
    }

    return stat;
}

/****************************************************************************//**
 * @brief  Read data from flash with lock
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  addr: flash read start address
 * @param  dst: data pointer to store data read from flash
 * @param  len: data length to read
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Read_With_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg,uint32_t addr, uint8_t *dst, int len)
{
    __disable_irq();
    XIP_SFlash_Read_Need_Lock_Ext(pFlashCfg, addr, dst, len);
    __enable_irq();
    return 0;
}

/****************************************************************************//**
 * @brief  Program flash one region with lock
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  addr: Start address to be programed
 * @param  src: Data pointer to be programed
 * @param  len: Data length to be programed
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Write_With_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg,uint32_t addr, uint8_t *src, int len)
{
    __disable_irq();
    XIP_SFlash_Write_Need_Lock_Ext(pFlashCfg, addr, src, len);
    __enable_irq();
    return 0;
}

/****************************************************************************//**
 * @brief  Erase flash one region with lock
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  addr: Start address to be erased
 * @param  len: Data length to be erased
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Erase_With_Lock_Ext(SPI_Flash_Cfg_Type *pFlashCfg,uint32_t addr, int len)
{
    __disable_irq();
    XIP_SFlash_Erase_Need_Lock_Ext(pFlashCfg, addr, addr + len - 1);
    __enable_irq();
    return 0;
}

/****************************************************************************//**
 * @brief  Clear flash status register with lock
 *
 * @param  pFlashCfg: Flash config pointer
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Clear_Status_Register_With_Lock(SPI_Flash_Cfg_Type *pFlashCfg)
{
    __disable_irq();
    XIP_SFlash_Clear_Status_Register_Need_Lock(pFlashCfg);
    __enable_irq();
    return 0;
}

/****************************************************************************//**
 * @brief  Sflash enable RCV mode to recovery for erase while power drop with lock
 *
 * @param  pFlashCfg: Flash config pointer
 * @param  rCmd: Read RCV register cmd
 * @param  wCmd: Write RCV register cmd
 * @param  bitPos: RCV register bit pos
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_RCV_Enable_With_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t rCmd, uint8_t wCmd, uint8_t bitPos)
{
    __disable_irq();
    XIP_SFlash_RCV_Enable_Need_Lock(pFlashCfg, rCmd, wCmd, bitPos);
    __enable_irq();
    return 0;
}

/****************************************************************************//**
 * @brief  Read data from flash with lock
 *
 * @param  pFlashCfg:Flash config pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION XIP_SFlash_Init(SPI_Flash_Cfg_Type *pFlashCfg)
{
    uint32_t ret;

    if(pFlashCfg==NULL){
        /* Get flash config identify */
        XIP_SFlash_Opt_Enter(&aesEnable);
        ret=SF_Cfg_Flash_Identify_Ext(1,1,0,0,&flashCfg);
        XIP_SFlash_Opt_Exit(aesEnable);
        if((ret&BFLB_FLASH_ID_VALID_FLAG)==0){
            return ERROR;
        }
    }else{
        memcpy(&flashCfg,pFlashCfg,sizeof(flashCfg));
    }
    
    return SUCCESS;
}

/****************************************************************************//**
 * @brief  Read data from flash with lock
 *
 * @param  addr: flash read start address
 * @param  dst: data pointer to store data read from flash
 * @param  len: data length to read
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Read(uint32_t addr, uint8_t *dst, int len)
{
    __disable_irq();
    XIP_SFlash_Opt_Enter(&aesEnable);
    XIP_SFlash_Read_Need_Lock_Ext(&flashCfg, addr, dst, len);
    XIP_SFlash_Opt_Exit(aesEnable);
    __enable_irq();
    return 0;
}

/****************************************************************************//**
 * @brief  Program flash one region with lock
 *
 * @param  addr: Start address to be programed
 * @param  src: Data pointer to be programed
 * @param  len: Data length to be programed
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Write(uint32_t addr, uint8_t *src, int len)
{
    __disable_irq();
    XIP_SFlash_Opt_Enter(&aesEnable);
    XIP_SFlash_Write_Need_Lock_Ext(&flashCfg, addr, src, len);
    XIP_SFlash_Opt_Exit(aesEnable);
    __enable_irq();
    return 0;
}

/****************************************************************************//**
 * @brief  Erase flash one region with lock
 *
 * @param  addr: Start address to be erased
 * @param  len: Data length to be erased
 *
 * @return 0
 *
*******************************************************************************/
__WEAK
int ATTR_TCM_SECTION XIP_SFlash_Erase(uint32_t addr, int len)
{
    __disable_irq();
    XIP_SFlash_Opt_Enter(&aesEnable);
    XIP_SFlash_Erase_Need_Lock_Ext(&flashCfg, addr, addr + len - 1);
    XIP_SFlash_Opt_Exit(aesEnable);
    __enable_irq();
    return 0;
}
/*@} end of group XIP_SFLASH_EXT_Public_Functions */

/*@} end of group XIP_SFLASH_EXT */

/*@} end of group BL602_Peripheral_Driver */
