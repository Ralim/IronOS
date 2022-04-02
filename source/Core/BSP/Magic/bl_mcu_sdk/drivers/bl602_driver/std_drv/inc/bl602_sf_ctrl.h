/**
  ******************************************************************************
  * @file    bl602_sf_ctrl.h
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
#ifndef __BL602_SF_CTRL_H__
#define __BL602_SF_CTRL_H__

#include "sf_ctrl_reg.h"
#include "bl602_common.h"

/** @addtogroup  BL602_Peripheral_Driver
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
    SF_CTRL_EMBEDDED_SEL,             /*!< Embedded flash select */
    SF_CTRL_EXTERNAL_17TO22_SEL,      /*!< External flash select gpio 17-22 */
    SF_CTRL_EXTERNAL_0TO2_20TO22_SEL, /*!< External flash select gpio 0-2 and 20-22 */
} SF_Ctrl_Pad_Sel;

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

/** @defgroup  SF_CTRL_PAD_SEL
 *  @{
 */
#define IS_SF_CTRL_PAD_SEL(type) (((type) == SF_CTRL_EMBEDDED_SEL) ||        \
                                  ((type) == SF_CTRL_EXTERNAL_17TO22_SEL) || \
                                  ((type) == SF_CTRL_EXTERNAL_0TO2_20TO22_SEL))

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
void SF_Ctrl_Set_Owner(SF_Ctrl_Owner_Type owner);
void SF_Ctrl_Disable(void);
void SF_Ctrl_Select_Pad(SF_Ctrl_Pad_Sel sel);
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
void SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset);
uint32_t SF_Ctrl_Get_Flash_Image_Offset(void);
void SF_Ctrl_Select_Clock(SF_Ctrl_Sahb_Type sahbType);
void SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg);
void SF_Ctrl_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid);
void SF_Ctrl_Icache2_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid);
BL_Sts_Type SF_Ctrl_GetBusyState(void);
uint8_t SF_Ctrl_Is_AES_Enable(void);
uint8_t SF_Ctrl_Get_Clock_Delay(void);
void SF_Ctrl_Set_Clock_Delay(uint8_t delay);

/*@} end of group SF_CTRL_Public_Functions */

/*@} end of group SF_CTRL */

/*@} end of group BL602_Peripheral_Driver */

#endif /* __BL602_SF_CTRL_H__ */
