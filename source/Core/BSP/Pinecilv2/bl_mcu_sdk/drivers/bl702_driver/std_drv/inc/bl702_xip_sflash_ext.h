/**
  ******************************************************************************
  * @file    bl702_xip_sflash_ext.h
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
#ifndef __BL702_XIP_SFLASH_EXT_H__
#define __BL702_XIP_SFLASH_EXT_H__

#include "bl702_common.h"
#include "bl702_sflash.h"
#include "bl702_xip_sflash.h"
#include "bl702_sflash.h"
#include "bl702_sflash_ext.h"
#include "bl702_sf_cfg.h"
#include "bl702_sf_cfg_ext.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  XIP_SFLASH
 *  @{
 */

/** @defgroup  XIP_SFLASH_EXT_Public_Types
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Public_Types */

/** @defgroup  XIP_SFLASH_EXT_Public_Constants
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Public_Constants */

/** @defgroup  XIP_SFLASH_EXT_Public_Macros
 *  @{
 */

/*@} end of group XIP_SFLASH_EXT_Public_Macros */

/** @defgroup  XIP_SFLASH_EXT_Public_Functions
 *  @{
 */

BL_Err_Type XIP_SFlash_KH25V40_Write_Protect_Need_Lock(SPI_Flash_Cfg_Type *flashCfg, SFlash_Protect_Kh25v40_Type protect);
BL_Err_Type XIP_SFlash_Clear_Status_Register_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg);

/*@} end of group XIP_SFLASH_EXT_Public_Functions */

/*@} end of group XIP_SFLASH */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_XIP_SFLASH_EXT_H__ */
