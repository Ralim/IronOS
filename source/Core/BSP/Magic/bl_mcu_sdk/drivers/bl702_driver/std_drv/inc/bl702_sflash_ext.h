/**
  ******************************************************************************
  * @file    bl702_sflah_ext.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
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
#ifndef __BL702_SFLAH_EXT_H__
#define __BL702_SFLAH_EXT_H__

#include "bl702_sflash.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SFLAH_EXT
 *  @{
 */

/** @defgroup  SFLAH_EXT_Public_Types
 *  @{
 */

/**
 *  @brief Serail flash protect KH25V40 type definition
 */
typedef enum {
    SFLASH_KH25V40_PROTECT_NONE,          /*!< SFlash no protect KH25V40 */
    SFLASH_KH25V40_PROTECT_448KTO512K,    /*!< SFlash protect KH25V40 448K to 512K */
    SFLASH_KH25V40_PROTECT_384KTO512K,    /*!< SFlash protect KH25V40 384K to 512K */
    SFLASH_KH25V40_PROTECT_256KTO512K,    /*!< SFlash protect KH25V40 256K to 512K */
    SFLASH_KH25V40_PROTECT_ALL,           /*!< SFlash protect KH25V40 0K to 512K */
} SFlash_Protect_Kh25v40_Type;

/*@} end of group SFLAH_EXT_Public_Types */

/** @defgroup  SFLAH_EXT_Public_Constants
 *  @{
 */

/** @defgroup  SFLASH_PROTECT_KH25V40_TYPE
 *  @{
 */
#define IS_SFLASH_PROTECT_KH25V40_TYPE(type) (((type) == SFLASH_KH25V40_PROTECT_NONE) || \
                                              ((type) == SFLASH_KH25V40_PROTECT_448KTO512K) ||  \
                                              ((type) == SFLASH_KH25V40_PROTECT_384KTO512K) ||  \
                                              ((type) == SFLASH_KH25V40_PROTECT_256KTO512K) || \
                                              ((type) == SFLASH_KH25V40_PROTECT_ALL))

/*@} end of group SFLAH_EXT_Public_Constants */

/** @defgroup  SFLAH_EXT_Public_Macros
 *  @{
 */

/*@} end of group SFLAH_EXT_Public_Macros */

/** @defgroup  SFLAH_EXT_Public_Functions
 *  @{
 */
BL_Err_Type SFlash_KH25V40_Write_Protect(SPI_Flash_Cfg_Type *flashCfg, SFlash_Protect_Kh25v40_Type protect);
BL_Err_Type SFlash_Read_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue,
                                     uint8_t regLen);
BL_Err_Type SFlash_Write_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue,
                                      uint8_t regLen);
BL_Err_Type SFlash_Clear_Status_Register(SPI_Flash_Cfg_Type *pFlashCfg);

/*@} end of group SFLAH_EXT_Public_Functions */

/*@} end of group SFLAH_EXT */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_SFLAH_EXT_H__ */
