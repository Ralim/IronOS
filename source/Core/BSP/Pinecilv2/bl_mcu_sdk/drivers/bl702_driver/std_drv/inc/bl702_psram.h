/**
  ******************************************************************************
  * @file    bl702_psram.h
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
#ifndef __BL702_PSRAM_H__
#define __BL702_PSRAM_H__

#include "sf_ctrl_reg.h"
#include "bl702_common.h"
#include "bl702_sf_ctrl.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PSRAM
 *  @{
 */

/** @defgroup  PSRAM_Public_Types
 *  @{
 */

/**
 *  @brief Psram drive strength type definition
 */
typedef enum {
    PSRAM_DRIVE_STRENGTH_50_OHMS,  /*!< Drive strength 50 ohms(default) */
    PSRAM_DRIVE_STRENGTH_100_OHMS, /*!< Drive strength 100 ohms */
    PSRAM_DRIVE_STRENGTH_200_OHMS, /*!< Drive strength 200 ohms */
} PSRAM_Drive_Strength;

/**
 *  @brief Psram burst length size type definition
 */
typedef enum {
    PSRAM_BURST_LENGTH_16_BYTES,  /*!< Burst Length 16 bytes */
    PSRAM_BURST_LENGTH_32_BYTES,  /*!< Burst Length 32 bytes */
    PSRAM_BURST_LENGTH_64_BYTES,  /*!< Burst Length 64 bytes */
    PSRAM_BURST_LENGTH_512_BYTES, /*!< Burst Length 512 bytes(default) */
} PSRAM_Burst_Length;

/**
 *  @brief Psram ctrl mode type definition
 */
typedef enum {
    PSRAM_SPI_CTRL_MODE, /*!< Psram SPI ctrl mode */
    PSRAM_QPI_CTRL_MODE, /*!< Psram QPI ctrl mode */
} PSRAM_Ctrl_Mode;

/**
 *  @brief Psram ctrl configuration structure type definition
 */
typedef struct
{
    uint8_t readIdCmd;                  /*!< Read ID command */
    uint8_t readIdDmyClk;               /*!< Read ID command dummy clock */
    uint8_t burstToggleCmd;             /*!< Burst toggle length command */
    uint8_t resetEnableCmd;             /*!< Psram reset enable command */
    uint8_t resetCmd;                   /*!< Psram reset command */
    uint8_t enterQuadModeCmd;           /*!< Psram enter quad mode command */
    uint8_t exitQuadModeCmd;            /*!< Psram exit quad mode command */
    uint8_t readRegCmd;                 /*!< Read register command */
    uint8_t readRegDmyClk;              /*!< Read register command dummy clock */
    uint8_t writeRegCmd;                /*!< Write register command */
    uint8_t readCmd;                    /*!< Psram read command */
    uint8_t readDmyClk;                 /*!< Psram read command dummy clock */
    uint8_t fReadCmd;                   /*!< Psram fast read command */
    uint8_t fReadDmyClk;                /*!< Psram fast read command dummy clock */
    uint8_t fReadQuadCmd;               /*!< Psram fast read quad command */
    uint8_t fReadQuadDmyClk;            /*!< Psram fast read quad command dummy clock */
    uint8_t writeCmd;                   /*!< Psram write command */
    uint8_t quadWriteCmd;               /*!< Psram quad write command */
    uint16_t pageSize;                  /*!< Psram page size */
    PSRAM_Ctrl_Mode ctrlMode;           /*!< Psram ctrl mode */
    PSRAM_Drive_Strength driveStrength; /*!< Psram drive strength */
    PSRAM_Burst_Length burstLength;     /*!< Psram burst length size */
} SPI_Psram_Cfg_Type;

/*@} end of group PSRAM_Public_Types */

/** @defgroup  PSRAM_Public_Constants
 *  @{
 */

/** @defgroup  PSRAM_DRIVE_STRENGTH
 *  @{
 */
#define IS_PSRAM_DRIVE_STRENGTH(type) (((type) == PSRAM_DRIVE_STRENGTH_50_OHMS) ||  \
                                       ((type) == PSRAM_DRIVE_STRENGTH_100_OHMS) || \
                                       ((type) == PSRAM_DRIVE_STRENGTH_200_OHMS))

/** @defgroup  PSRAM_BURST_LENGTH
 *  @{
 */
#define IS_PSRAM_BURST_LENGTH(type) (((type) == PSRAM_BURST_LENGTH_16_BYTES) || \
                                     ((type) == PSRAM_BURST_LENGTH_32_BYTES) || \
                                     ((type) == PSRAM_BURST_LENGTH_64_BYTES) || \
                                     ((type) == PSRAM_BURST_LENGTH_512_BYTES))

/** @defgroup  PSRAM_CTRL_MODE
 *  @{
 */
#define IS_PSRAM_CTRL_MODE(type) (((type) == PSRAM_SPI_CTRL_MODE) || \
                                  ((type) == PSRAM_QPI_CTRL_MODE))

/*@} end of group PSRAM_Public_Constants */

/** @defgroup  PSRAM_Public_Macros
 *  @{
 */

/*@} end of group PSRAM_Public_Macros */

/** @defgroup  PSRAM_Public_Functions
 *  @{
 */

/**
 *  @brief PSRAM Functions
 */
void Psram_Init(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg);
void Psram_ReadReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue);
void Psram_WriteReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue);
BL_Err_Type Psram_SetDriveStrength(SPI_Psram_Cfg_Type *psramCfg);
BL_Err_Type Psram_SetBurstWrap(SPI_Psram_Cfg_Type *psramCfg);
void Psram_ReadId(SPI_Psram_Cfg_Type *psramCfg, uint8_t *data);
BL_Err_Type Psram_EnterQuadMode(SPI_Psram_Cfg_Type *psramCfg);
BL_Err_Type Psram_ExitQuadMode(SPI_Psram_Cfg_Type *psramCfg);
BL_Err_Type Psram_ToggleBurstLength(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode);
BL_Err_Type Psram_SoftwareReset(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode);
BL_Err_Type Psram_Set_IDbus_Cfg(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint32_t len);
BL_Err_Type Psram_Cache_Write_Set(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, BL_Fun_Type wtEn,
                                  BL_Fun_Type wbEn, BL_Fun_Type waEn);
BL_Err_Type Psram_Write(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len);
BL_Err_Type Psram_Read(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len);

/*@} end of group PSRAM_Public_Functions */

/*@} end of group PSRAM */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_PSRAM_H__ */
