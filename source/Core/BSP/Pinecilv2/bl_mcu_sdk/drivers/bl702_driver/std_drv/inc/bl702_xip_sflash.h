/**
  ******************************************************************************
  * @file    bl702_xip_sflash.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
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
#ifndef __BL702_XIP_SFLASH_H__
#define __BL702_XIP_SFLASH_H__

#include "bl702_common.h"
#include "bl702_sflash.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  XIP_SFLASH
 *  @{
 */

/** @defgroup  XIP_SFLASH_Public_Types
 *  @{
 */

/*@} end of group XIP_SFLASH_Public_Types */

/** @defgroup  XIP_SFLASH_Public_Constants
 *  @{
 */

/*@} end of group XIP_SFLASH_Public_Constants */

/** @defgroup  XIP_SFLASH_Public_Macros
 *  @{
 */

/*@} end of group XIP_SFLASH_Public_Macros */

/** @defgroup  XIP_SFLASH_Public_Functions
 *  @{
 */
void XIP_SFlash_Opt_Enter(void);
void XIP_SFlash_Opt_Exit(void);
BL_Err_Type XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset);
BL_Err_Type XIP_SFlash_State_Restore(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t offset);
BL_Err_Type XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode,
                                       uint32_t startaddr, uint32_t endaddr);
BL_Err_Type XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr,
                                       uint8_t *data, uint32_t len);
BL_Err_Type XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr,
                                      uint8_t *data, uint32_t len);
BL_Err_Type XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode,
                                            uint8_t *data);
BL_Err_Type XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode,
                                             uint8_t *data);
BL_Err_Type XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode,
                                             uint8_t *data, uint8_t idLen);
BL_Err_Type XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len);

/*@} end of group XIP_SFLASH_Public_Functions */

/*@} end of group XIP_SFLASH */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_XIP_SFLASH_H__ */
