/**
  ******************************************************************************
  * @file    bl602_sflah_ext.h
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
#ifndef __BL602_SFLAH_EXT_H__
#define __BL602_SFLAH_EXT_H__

#include "bl602_sflash.h"
#include "bl602_common.h"

/** @addtogroup  BL602_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SFLAH_EXT
 *  @{
 */

/** @defgroup  SFLAH_EXT_Public_Types
 *  @{
 */

/**
 *  @brief Serial flash security register configuration
 */
typedef struct
{
    uint8_t eraseCmd;       /*!< Erase security register command */
    uint8_t programCmd;     /*!< Program security register command */
    uint8_t readCmd;        /*!< Read security register command */
    uint8_t enterSecOptCmd; /*!< Enter security register option mode command */
    uint8_t exitSecOptCmd;  /*!< Exit security register option mode command */
    uint8_t blockNum;       /*!< Security register block number */
    uint8_t *data;          /*!< Data pointer to be program/read */
    uint32_t addr;          /*!< Start address to be program/read */
    uint32_t len;           /*!< Data length to be program/read */
} SFlash_Sec_Reg_Cfg;

/*@} end of group SFLAH_EXT_Public_Types */

/** @defgroup  SFLAH_EXT_Public_Constants
 *  @{
 */

/*@} end of group SFLAH_EXT_Public_Constants */

/** @defgroup  SFLAH_EXT_Public_Macros
 *  @{
 */

/*@} end of group SFLAH_EXT_Public_Macros */

/** @defgroup  SFLAH_EXT_Public_Functions
 *  @{
 */
BL_Err_Type SFlash_Restore_From_Powerdown(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead);
BL_Err_Type SFlash_RCV_Enable(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t rCmd, uint8_t wCmd, uint8_t bitPos);
BL_Err_Type SFlash_Erase_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg);
BL_Err_Type SFlash_Program_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg,
                                             SFlash_Sec_Reg_Cfg *pSecRegCfg);
BL_Err_Type SFlash_Read_Security_Register(SFlash_Sec_Reg_Cfg *pSecRegCfg);
BL_Err_Type SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *pFlashCfg);

/*@} end of group SFLAH_EXT_Public_Functions */

/*@} end of group SFLAH_EXT */

/*@} end of group BL602_Peripheral_Driver */

#endif /* __BL602_SFLAH_EXT_H__ */
