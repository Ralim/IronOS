/**
  ******************************************************************************
  * @file    blsp_bootinfo.h
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
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
#ifndef __BLSP_BOOTINFO_H__
#define __BLSP_BOOTINFO_H__

#include "stdint.h"
#include "blsp_port.h"
#include "hal_flash.h"
#include "hal_boot2.h"
#include "hal_sec_hash.h"


#define BFLB_BOOT2_FLASH_CFG_MAGIC   "FCFG"
#define BFLB_BOOT2_PLL_CFG_MAGICCODE "PCFG"
#define BFLB_BOOT2_FLASH_TZC_MAGIC   "TCFG"


#define BFLB_BOOT2_READBUF_SIZE         4 * 1024
#define BFLB_FW_IMG_OFFSET_AFTER_HEADER HAL_BOOT2_FW_IMG_OFFSET_AFTER_HEADER


/* Image owner type */
#define BFLB_BOOT2_CPU_0   0
#define BFLB_BOOT2_CPU_1   1

/* Public key hash size */
#define BFLB_BOOT2_PK_HASH_SIZE HAL_BOOT2_PK_HASH_SIZE


/* Power save define */
#define BFLB_PSM_ACTIVE 0
#define BFLB_PSM_HBN    1



typedef enum {
    BFLB_BOOT2_SUCCESS = 0x00,

    /* Flash*/
    BFLB_BOOT2_FLASH_INIT_ERROR = 0x0001,
    BFLB_BOOT2_FLASH_ERASE_PARA_ERROR = 0x0002,
    BFLB_BOOT2_FLASH_ERASE_ERROR = 0x0003,
    BFLB_BOOT2_FLASH_WRITE_PARA_ERROR = 0x0004,
    BFLB_BOOT2_FLASH_WRITE_ADDR_ERROR = 0x0005,
    BFLB_BOOT2_FLASH_WRITE_ERROR = 0x0006,
    BFLB_BOOT2_FLASH_BOOT_PARA = 0x0007,
    BFLB_BOOT2_FLASH_READ_ERROR = 0x0008,

    /* Image*/
    BFLB_BOOT2_IMG_BOOTHEADER_LEN_ERROR = 0x0201,
    BFLB_BOOT2_IMG_BOOTHEADER_NOT_LOAD_ERROR = 0x0202,
    BFLB_BOOT2_IMG_BOOTHEADER_MAGIC_ERROR = 0x0203,
    BFLB_BOOT2_IMG_BOOTHEADER_CRC_ERROR = 0x0204,
    BFLB_BOOT2_IMG_BOOTHEADER_ENCRYPT_NOTFIT = 0x0205,
    BFLB_BOOT2_IMG_BOOTHEADER_SIGN_NOTFIT = 0x0206,
    BFLB_BOOT2_IMG_SEGMENT_CNT_ERROR = 0x0207,
    BFLB_BOOT2_IMG_AES_IV_LEN_ERROR = 0x0208,
    BFLB_BOOT2_IMG_AES_IV_CRC_ERROR = 0x0209,
    BFLB_BOOT2_IMG_PK_LEN_ERROR = 0x020a,
    BFLB_BOOT2_IMG_PK_CRC_ERROR = 0x020b,
    BFLB_BOOT2_IMG_PK_HASH_ERROR = 0x020c,
    BFLB_BOOT2_IMG_SIGNATURE_LEN_ERROR = 0x020d,
    BFLB_BOOT2_IMG_SIGNATURE_CRC_ERROR = 0x020e,
    BFLB_BOOT2_IMG_SECTIONHEADER_LEN_ERROR = 0x020f,
    BFLB_BOOT2_IMG_SECTIONHEADER_CRC_ERROR = 0x0210,
    BFLB_BOOT2_IMG_SECTIONHEADER_DST_ERROR = 0x0211,
    BFLB_BOOT2_IMG_SECTIONDATA_LEN_ERROR = 0x0212,
    BFLB_BOOT2_IMG_SECTIONDATA_DEC_ERROR = 0x0213,
    BFLB_BOOT2_IMG_SECTIONDATA_TLEN_ERROR = 0x0214,
    BFLB_BOOT2_IMG_SECTIONDATA_CRC_ERROR = 0x0215,
    BFLB_BOOT2_IMG_HALFBAKED_ERROR = 0x0216,
    BFLB_BOOT2_IMG_HASH_ERROR = 0x0217,
    BFLB_BOOT2_IMG_SIGN_PARSE_ERROR = 0x0218,
    BFLB_BOOT2_IMG_SIGN_ERROR = 0x0219,
    BFLB_BOOT2_IMG_DEC_ERROR = 0x021a,
    BFLB_BOOT2_IMG_ALL_INVALID_ERROR = 0x021b,
    BFLB_BOOT2_IMG_Roll_Back = 0x021c,

    /* MISC*/
    BFLB_BOOT2_MEM_ERROR = 0xfffb,
    BFLB_BOOT2_PLL_ERROR = 0xfffc,
    BFLB_BOOT2_INVASION_ERROR = 0xfffd,
    BFLB_BOOT2_POLLING = 0xfffe,
    BFLB_BOOT2_FAIL = 0xffff,

} boot_error_code;



typedef struct
{
    uint8_t aes_iv[16];
    uint32_t crc32;
} boot_aes_config;

typedef struct
{
    uint8_t eckye_x[HAL_BOOT2_ECC_KEYXSIZE]; //ec key in boot header
    uint8_t eckey_y[HAL_BOOT2_ECC_KEYYSIZE]; //ec key in boot header
    uint32_t crc32;
} boot_pk_config;

typedef struct
{
    uint32_t sig_len;
    uint8_t signature[HAL_BOOT2_SIGN_MAXSIZE];
    uint32_t crc32;
} boot_sign_config;



typedef struct
{
    uint32_t msp_store_addr;
    uint32_t pc_store_addr;
    uint32_t default_xip_addr;
} boot_cpu_config;

/**
 *  @brief ram image config type
 */
typedef struct
{
    uint8_t  valid;      /*!< valid or not */
    uint8_t  core;       /*!< which core to run */
    uint32_t boot_addr ; /*!< boot addr */
} ram_img_config_t;

typedef void (*pentry_t)(void);

extern ram_img_config_t ram_img_config[BLSP_BOOT2_RAM_IMG_COUNT_MAX];
extern boot2_image_config g_boot_img_cfg[BLSP_BOOT2_CPU_GROUP_MAX];
extern boot2_efuse_hw_config g_efuse_cfg;
extern uint8_t g_ps_mode;
//extern uint8_t g_cpu_count;
extern uint8_t *g_boot2_read_buf;
extern sec_hash_handle_t hash_handle;

#endif /* __BLSP_BOOTINFO_H__ */



