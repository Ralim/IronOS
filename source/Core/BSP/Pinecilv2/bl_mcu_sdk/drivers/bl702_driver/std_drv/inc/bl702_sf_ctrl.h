/**
  ******************************************************************************
  * @file    bl702_sf_ctrl.h
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
#ifndef __BL702_SF_CTRL_H__
#define __BL702_SF_CTRL_H__

#include "sf_ctrl_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SF_CTRL
 *  @{
 */

/** @defgroup  SF_CTRL_Public_Types
 *  @{
 */

/**
 *  @brief Serial flash pad select type definition
 */
typedef enum {
    SF_CTRL_PAD_SEL_SF1,               /*!< SF Ctrl pad select sf1, flash use GPIO 17-22, no psram */
    SF_CTRL_PAD_SEL_SF2,               /*!< SF Ctrl pad select sf2, flash use GPIO 23-28, no psram, embedded flash */
    SF_CTRL_PAD_SEL_SF3,               /*!< SF Ctrl pad select sf3, flash use GPIO 32-37, no psram */
    SF_CTRL_PAD_SEL_DUAL_BANK_SF1_SF2, /*!< SF Ctrl pad select sf1 and sf2, flash use GPIO 17-22, psram use GPIO 23-28 */
    SF_CTRL_PAD_SEL_DUAL_BANK_SF2_SF3, /*!< SF Ctrl pad select sf2 and sf3, flash use GPIO 23-28, psram use GPIO 32-37 */
    SF_CTRL_PAD_SEL_DUAL_BANK_SF3_SF1, /*!< SF Ctrl pad select sf3 and sf1, flash use GPIO 32-37, psram use GPIO 17-22 */
    SF_CTRL_PAD_SEL_DUAL_CS_SF2,       /*!< SF Ctrl pad select sf2, flash/psram use GPIO 23-28, psram use GPIO 17 as CS2 */
    SF_CTRL_PAD_SEL_DUAL_CS_SF3,       /*!< SF Ctrl pad select sf3, flash/psram use GPIO 32-37, psram use GPIO 23 as CS2 */
} SF_Ctrl_Pad_Select;

/**
 *  @brief Serial flash system bus control type definition
 */
typedef enum {
    SF_CTRL_SEL_FLASH, /*!< SF Ctrl system bus control flash */
    SF_CTRL_SEL_PSRAM, /*!< SF Ctrl system bus control psram */
} SF_Ctrl_Select;

/**
 *  @brief Serail flash controller wrap mode len type definition
 */
typedef enum {
    SF_CTRL_WRAP_LEN_8,    /*!< SF Ctrl wrap length: 8 */
    SF_CTRL_WRAP_LEN_16,   /*!< SF Ctrl wrap length: 16 */
    SF_CTRL_WRAP_LEN_32,   /*!< SF Ctrl wrap length: 32 */
    SF_CTRL_WRAP_LEN_64,   /*!< SF Ctrl wrap length: 64 */
    SF_CTRL_WRAP_LEN_128,  /*!< SF Ctrl wrap length: 128 */
    SF_CTRL_WRAP_LEN_256,  /*!< SF Ctrl wrap length: 256 */
    SF_CTRL_WRAP_LEN_512,  /*!< SF Ctrl wrap length: 512 */
    SF_CTRL_WRAP_LEN_1024, /*!< SF Ctrl wrap length: 1024 */
    SF_CTRL_WRAP_LEN_2048, /*!< SF Ctrl wrap length: 2048 */
    SF_CTRL_WRAP_LEN_4096, /*!< SF Ctrl wrap length: 4096 */
} SF_Ctrl_Wrap_Len_Type;

/**
 *  @brief Serial flash controller owner type definition
 */
typedef enum {
    SF_CTRL_OWNER_SAHB, /*!< System AHB bus control serial flash controller */
    SF_CTRL_OWNER_IAHB, /*!< I-Code AHB bus control serial flash controller */
} SF_Ctrl_Owner_Type;

/**
 *  @brief Serial flash controller select clock type definition
 */
typedef enum {
    SF_CTRL_SAHB_CLOCK,  /*!< Serial flash controller select default sahb clock */
    SF_CTRL_FLASH_CLOCK, /*!< Serial flash controller select flash clock */
} SF_Ctrl_Sahb_Type;

/**
 *  @brief Serial flash controller owner type definition
 */
typedef enum {
    HIGH_SPEED_MODE_CLOCK,  /*!< Serial flash controller high speed mode clk_ahb>clk_sf */
    REMOVE_CLOCK_CONSTRAIN, /*!< Serial flash controller remove clock constrain */
} SF_Ctrl_Ahb2sif_Type;

/**
 *  @brief Read and write type definition
 */
typedef enum {
    SF_CTRL_READ,  /*!< Serail flash read command flag */
    SF_CTRL_WRITE, /*!< Serail flash write command flag */
} SF_Ctrl_RW_Type;

/**
 *  @brief Serail flash interface IO type definition
 */
typedef enum {
    SF_CTRL_NIO_MODE, /*!< Normal IO mode define */
    SF_CTRL_DO_MODE,  /*!< Dual Output mode define */
    SF_CTRL_QO_MODE,  /*!< Quad Output mode define */
    SF_CTRL_DIO_MODE, /*!< Dual IO mode define */
    SF_CTRL_QIO_MODE, /*!< Quad IO mode define */
} SF_Ctrl_IO_Type;

/**
 *  @brief Serail flash controller interface mode type definition
 */
typedef enum {
    SF_CTRL_SPI_MODE, /*!< SPI mode define */
    SF_CTRL_QPI_MODE, /*!< QPI mode define */
} SF_Ctrl_Mode_Type;

/**
 *  @brief Serail flash controller command mode type definition
 */
typedef enum {
    SF_CTRL_CMD_1_LINE,  /*!< Command in one line mode */
    SF_CTRL_CMD_4_LINES, /*!< Command in four lines mode */
} SF_Ctrl_Cmd_Mode_Type;

/**
 *  @brief Serail flash controller address mode type definition
 */
typedef enum {
    SF_CTRL_ADDR_1_LINE,  /*!< Address in one line mode */
    SF_CTRL_ADDR_2_LINES, /*!< Address in two lines mode */
    SF_CTRL_ADDR_4_LINES, /*!< Address in four lines mode */
} SF_Ctrl_Addr_Mode_Type;

/**
 *  @brief Serail flash controller dummy mode type definition
 */
typedef enum {
    SF_CTRL_DUMMY_1_LINE,  /*!< Dummy in one line mode */
    SF_CTRL_DUMMY_2_LINES, /*!< Dummy in two lines mode */
    SF_CTRL_DUMMY_4_LINES, /*!< Dummy in four lines mode */
} SF_Ctrl_Dmy_Mode_Type;

/**
 *  @brief Serail flash controller data mode type definition
 */
typedef enum {
    SF_CTRL_DATA_1_LINE,  /*!< Data in one line mode */
    SF_CTRL_DATA_2_LINES, /*!< Data in two lines mode */
    SF_CTRL_DATA_4_LINES, /*!< Data in four lines mode */
} SF_Ctrl_Data_Mode_Type;

/**
 *  @brief Serail flash controller AES type definition
 */
typedef enum {
    SF_CTRL_AES_128BITS,            /*!< Serail flash AES key 128 bits length */
    SF_CTRL_AES_256BITS,            /*!< Serail flash AES key 256 bits length */
    SF_CTRL_AES_192BITS,            /*!< Serail flash AES key 192 bits length */
    SF_CTRL_AES_128BITS_DOUBLE_KEY, /*!< Serail flash AES key 128 bits length double key */
} SF_Ctrl_AES_Key_Type;

/**
 *  @brief Serail flash controller configuration structure type definition
 */
typedef struct
{
    SF_Ctrl_Owner_Type owner;         /*!< Sflash interface bus owner */
    SF_Ctrl_Sahb_Type sahbClock;      /*!< Sflash clock sahb sram select */
    SF_Ctrl_Ahb2sif_Type ahb2sifMode; /*!< Sflash ahb2sif mode */
    uint8_t clkDelay;                 /*!< Clock count for read due to pad delay */
    uint8_t clkInvert;                /*!< Clock invert */
    uint8_t rxClkInvert;              /*!< RX clock invert */
    uint8_t doDelay;                  /*!< Data out delay */
    uint8_t diDelay;                  /*!< Data in delay */
    uint8_t oeDelay;                  /*!< Output enable delay */
} SF_Ctrl_Cfg_Type;

/**
 *  @brief SF Ctrl psram controller configuration structure type definition
 */
typedef struct
{
    SF_Ctrl_Owner_Type owner;        /*!< Psram interface bus owner */
    SF_Ctrl_Pad_Select padSel;       /*!< SF Ctrl pad select */
    SF_Ctrl_Select bankSel;          /*!< SF Ctrl bank select */
    BL_Fun_Type psramRxClkInvertSrc; /*!< Select psram rx clock invert source */
    BL_Fun_Type psramRxClkInvertSel; /*!< Select inveted psram rx clock */
    BL_Fun_Type psramDelaySrc;       /*!< Select psram read delay source */
    uint8_t psramClkDelay;           /*!< Psram read delay cycle = n + 1 */
} SF_Ctrl_Psram_Cfg;

/**
 *  @brief SF Ctrl cmds configuration structure type definition
 */
typedef struct
{
    BL_Fun_Type cmdsEn;            /*!< SF Ctrl cmds enable */
    BL_Fun_Type burstToggleEn;     /*!< SF Ctrl burst toggle mode enable */
    BL_Fun_Type wrapModeEn;        /*!< SF Ctrl wrap mode cmd enable */
    SF_Ctrl_Wrap_Len_Type wrapLen; /*!< SF Ctrl wrap length */
} SF_Ctrl_Cmds_Cfg;

/**
 *  @brief Serail flash command configuration structure type definition
 */
typedef struct
{
    uint8_t rwFlag;                  /*!< Read write flag */
    SF_Ctrl_Cmd_Mode_Type cmdMode;   /*!< Command mode */
    SF_Ctrl_Addr_Mode_Type addrMode; /*!< Address mode */
    uint8_t addrSize;                /*!< Address size */
    uint8_t dummyClks;               /*!< Dummy clocks */
    SF_Ctrl_Dmy_Mode_Type dummyMode; /*!< Dummy mode */
    SF_Ctrl_Data_Mode_Type dataMode; /*!< Data mode */
    uint8_t rsv[1];                  /*!<  */
    uint32_t nbData;                 /*!< Transfer number of bytes */
    uint32_t cmdBuf[2];              /*!< Command buffer */
} SF_Ctrl_Cmd_Cfg_Type;

/*@} end of group SF_CTRL_Public_Types */

/** @defgroup  SF_CTRL_Public_Constants
 *  @{
 */

/** @defgroup  SF_CTRL_PAD_SELECT
 *  @{
 */
#define IS_SF_CTRL_PAD_SELECT(type) (((type) == SF_CTRL_PAD_SEL_SF1) ||               \
                                     ((type) == SF_CTRL_PAD_SEL_SF2) ||               \
                                     ((type) == SF_CTRL_PAD_SEL_SF3) ||               \
                                     ((type) == SF_CTRL_PAD_SEL_DUAL_BANK_SF1_SF2) || \
                                     ((type) == SF_CTRL_PAD_SEL_DUAL_BANK_SF2_SF3) || \
                                     ((type) == SF_CTRL_PAD_SEL_DUAL_BANK_SF3_SF1) || \
                                     ((type) == SF_CTRL_PAD_SEL_DUAL_CS_SF2) ||       \
                                     ((type) == SF_CTRL_PAD_SEL_DUAL_CS_SF3))

/** @defgroup  SF_CTRL_SELECT
 *  @{
 */
#define IS_SF_CTRL_SELECT(type) (((type) == SF_CTRL_SEL_FLASH) || \
                                 ((type) == SF_CTRL_SEL_PSRAM))

/** @defgroup  SF_CTRL_WRAP_LEN_TYPE
 *  @{
 */
#define IS_SF_CTRL_WRAP_LEN_TYPE(type) (((type) == SF_CTRL_WRAP_LEN_8) ||    \
                                        ((type) == SF_CTRL_WRAP_LEN_16) ||   \
                                        ((type) == SF_CTRL_WRAP_LEN_32) ||   \
                                        ((type) == SF_CTRL_WRAP_LEN_64) ||   \
                                        ((type) == SF_CTRL_WRAP_LEN_128) ||  \
                                        ((type) == SF_CTRL_WRAP_LEN_256) ||  \
                                        ((type) == SF_CTRL_WRAP_LEN_512) ||  \
                                        ((type) == SF_CTRL_WRAP_LEN_1024) || \
                                        ((type) == SF_CTRL_WRAP_LEN_2048) || \
                                        ((type) == SF_CTRL_WRAP_LEN_4096))

/** @defgroup  SF_CTRL_OWNER_TYPE
 *  @{
 */
#define IS_SF_CTRL_OWNER_TYPE(type) (((type) == SF_CTRL_OWNER_SAHB) || \
                                     ((type) == SF_CTRL_OWNER_IAHB))

/** @defgroup  SF_CTRL_SAHB_TYPE
 *  @{
 */
#define IS_SF_CTRL_SAHB_TYPE(type) (((type) == SF_CTRL_SAHB_CLOCK) || \
                                    ((type) == SF_CTRL_FLASH_CLOCK))

/** @defgroup  SF_CTRL_AHB2SIF_TYPE
 *  @{
 */
#define IS_SF_CTRL_AHB2SIF_TYPE(type) (((type) == HIGH_SPEED_MODE_CLOCK) || \
                                       ((type) == REMOVE_CLOCK_CONSTRAIN))

/** @defgroup  SF_CTRL_RW_TYPE
 *  @{
 */
#define IS_SF_CTRL_RW_TYPE(type) (((type) == SF_CTRL_READ) || \
                                  ((type) == SF_CTRL_WRITE))

/** @defgroup  SF_CTRL_IO_TYPE
 *  @{
 */
#define IS_SF_CTRL_IO_TYPE(type) (((type) == SF_CTRL_NIO_MODE) || \
                                  ((type) == SF_CTRL_DO_MODE) ||  \
                                  ((type) == SF_CTRL_QO_MODE) ||  \
                                  ((type) == SF_CTRL_DIO_MODE) || \
                                  ((type) == SF_CTRL_QIO_MODE))

/** @defgroup  SF_CTRL_MODE_TYPE
 *  @{
 */
#define IS_SF_CTRL_MODE_TYPE(type) (((type) == SF_CTRL_SPI_MODE) || \
                                    ((type) == SF_CTRL_QPI_MODE))

/** @defgroup  SF_CTRL_CMD_MODE_TYPE
 *  @{
 */
#define IS_SF_CTRL_CMD_MODE_TYPE(type) (((type) == SF_CTRL_CMD_1_LINE) || \
                                        ((type) == SF_CTRL_CMD_4_LINES))

/** @defgroup  SF_CTRL_ADDR_MODE_TYPE
 *  @{
 */
#define IS_SF_CTRL_ADDR_MODE_TYPE(type) (((type) == SF_CTRL_ADDR_1_LINE) ||  \
                                         ((type) == SF_CTRL_ADDR_2_LINES) || \
                                         ((type) == SF_CTRL_ADDR_4_LINES))

/** @defgroup  SF_CTRL_DMY_MODE_TYPE
 *  @{
 */
#define IS_SF_CTRL_DMY_MODE_TYPE(type) (((type) == SF_CTRL_DUMMY_1_LINE) ||  \
                                        ((type) == SF_CTRL_DUMMY_2_LINES) || \
                                        ((type) == SF_CTRL_DUMMY_4_LINES))

/** @defgroup  SF_CTRL_DATA_MODE_TYPE
 *  @{
 */
#define IS_SF_CTRL_DATA_MODE_TYPE(type) (((type) == SF_CTRL_DATA_1_LINE) ||  \
                                         ((type) == SF_CTRL_DATA_2_LINES) || \
                                         ((type) == SF_CTRL_DATA_4_LINES))

/** @defgroup  SF_CTRL_AES_KEY_TYPE
 *  @{
 */
#define IS_SF_CTRL_AES_KEY_TYPE(type) (((type) == SF_CTRL_AES_128BITS) || \
                                       ((type) == SF_CTRL_AES_256BITS) || \
                                       ((type) == SF_CTRL_AES_192BITS) || \
                                       ((type) == SF_CTRL_AES_128BITS_DOUBLE_KEY))

/*@} end of group SF_CTRL_Public_Constants */

/** @defgroup  SF_CTRL_Public_Macros
 *  @{
 */
#define SF_CTRL_NO_ADDRESS  0xFFFFFFFF
#define FLASH_CTRL_BUF_SIZE 256

/*@} end of group SF_CTRL_Public_Macros */

/** @defgroup  SF_CTRL_Public_Functions
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
void SF_Ctrl_IRQHandler(void);
#endif
void SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg);
void SF_Ctrl_Psram_Init(SF_Ctrl_Psram_Cfg *psramCfg);
uint8_t SF_Ctrl_Get_Clock_Delay(void);
void SF_Ctrl_Set_Clock_Delay(uint8_t delay);
void SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg);
void SF_Ctrl_Select_Pad(SF_Ctrl_Pad_Select sel);
void SF_Ctrl_Select_Bank(SF_Ctrl_Select sel);
void SF_Ctrl_Set_Owner(SF_Ctrl_Owner_Type owner);
void SF_Ctrl_Disable(void);
void SF_Ctrl_AES_Enable_BE(void);
void SF_Ctrl_AES_Enable_LE(void);
void SF_Ctrl_AES_Set_Region(uint8_t region, uint8_t enable, uint8_t hwKey, uint32_t startAddr,
                            uint32_t endAddr,
                            uint8_t locked);
void SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType);
void SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType);
void SF_Ctrl_AES_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset);
void SF_Ctrl_AES_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset);
void SF_Ctrl_AES_Enable(void);
void SF_Ctrl_AES_Disable(void);
uint8_t SF_Ctrl_Is_AES_Enable(void);
void SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset);
uint32_t SF_Ctrl_Get_Flash_Image_Offset(void);
void SF_Ctrl_Select_Clock(SF_Ctrl_Sahb_Type sahbType);
void SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg);
void SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid);
void SF_Ctrl_Psram_Write_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid);
void SF_Ctrl_Psram_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid);
BL_Sts_Type SF_Ctrl_GetBusyState(void);

/*@} end of group SF_CTRL_Public_Functions */

/*@} end of group SF_CTRL */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_SF_CTRL_H__ */
