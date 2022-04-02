/**
 * *****************************************************************************
 * @file hal_boot2_custom.h
 * @version 0.1
 * @date 2021-07-17
 * @brief
 * *****************************************************************************
 * @attention
 *
 *  <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *    3. Neither the name of Bouffalo Lab nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 */
#ifndef __HAL_BOOT2_H__
#define __HAL_BOOT2_H__

#include "hal_common.h"
#include "bl602_sflash.h"
#include "bl602_glb.h"

#define BL_TCM_BASE           BL602_TCM_BASE
#define BL_SYS_CLK_PLL        GLB_SYS_CLK_PLL160M
#define BL_SFLASH_CLK         GLB_SFLASH_CLK_80M
#define HAL_PLL_CFG_MAGICCODE "PCFG"

#define HAL_EFUSE_CPU_MAX                     2
#define HAL_EFUSE_PK_HASH_SIZE                256 / 8
#define HAL_BOOT2_SUPPORT_DECOMPRESS          1 /* 1 support decompress, 0 not support */
#define HAL_BOOT2_SUPPORT_USB_IAP             0 /* 1 support decompress, 0 not support */
#define HAL_BOOT2_SUPPORT_EFLASH_LOADER_RAM   1 /* 1 support decompress, 0 not support */
#define HAL_BOOT2_SUPPORT_EFLASH_LOADER_FLASH 0 /* 1 support decompress, 0 not support */

#define HAL_BOOT2_FW_IMG_OFFSET_AFTER_HEADER    4*1024

#define HAL_BOOT2_HASH_SIZE                   256 / 8
#define HAL_BOOT2_ECC_KEYXSIZE                256 / 8
#define HAL_BOOT2_ECC_KEYYSIZE                256 / 8
#define HAL_BOOT2_SIGN_MAXSIZE                2048 / 8
#define HAL_BOOT2_DEADBEEF_VAL                0xdeadbeef
#define HAL_BOOT2_CPU_MAX                     1

#define HAL_BOOT2_CPU0_MAGIC        "BFNP"
#define HAL_BOOT2_CPU1_MAGIC        "BFAP"



typedef struct
{
    uint8_t encrypted[HAL_EFUSE_CPU_MAX];
    uint8_t sign[HAL_EFUSE_CPU_MAX];
    uint8_t hbn_check_sign;
    uint8_t rsvd[3];
    uint8_t chip_id[8];
    uint8_t pk_hash_cpu0[HAL_EFUSE_PK_HASH_SIZE];
    uint8_t pk_hash_cpu1[HAL_EFUSE_PK_HASH_SIZE];
} boot2_efuse_hw_config;

typedef struct
{
    uint8_t xtal_type;
    uint8_t pll_clk;
    uint8_t hclk_div;
    uint8_t bclk_div;

    uint8_t flash_clk_type;
    uint8_t flash_clk_div;
    uint8_t rsvd[2];
} hal_sys_clk_config;

typedef struct
{
    uint32_t magicCode; /*'PCFG'*/
    hal_sys_clk_config cfg;
    uint32_t crc32;
} hal_pll_config;

typedef struct
{
    uint32_t magicCode; /*'FCFG'*/
    SPI_Flash_Cfg_Type cfg;
    uint32_t crc32;
} hal_flash_config;

struct  hal_bootheader_t
{
    uint32_t magicCode; /*'BFXP'*/
    uint32_t rivison;
    hal_flash_config flash_cfg;
    hal_pll_config clk_cfg;
    __PACKED_UNION
    {
        __PACKED_STRUCT
        {
            uint32_t sign              : 2;  /* [1: 0]      for sign*/
            uint32_t encrypt_type      : 2;  /* [3: 2]      for encrypt */
            uint32_t key_sel           : 2;  /* [5: 4]      for key sel in boot interface*/
            uint32_t rsvd6_7           : 2;  /* [7: 6]      for encrypt*/
            uint32_t no_segment        : 1;  /* [8]         no segment info */
            uint32_t cache_enable      : 1;  /* [9]         for cache */
            uint32_t notLoadInBoot     : 1;  /* [10]        not load this img in bootrom */
            uint32_t aes_region_lock   : 1;  /* [11]        aes region lock */
            uint32_t cache_way_disable : 4;  /* [15: 12]    cache way disable info*/
            uint32_t crcIgnore         : 1;  /* [16]        ignore crc */
            uint32_t hash_ignore       : 1;  /* [17]        hash crc */
            uint32_t halt_cpu1         : 1;  /* [18]        halt ap */
            uint32_t rsvd19_31         : 13; /* [31:19]     rsvd */
        }
        bval;
        uint32_t wval;
    }
    bootCfg;

    __PACKED_UNION
    {
        uint32_t segment_cnt;
        uint32_t img_len;
    }
    img_segment_info;

    uint32_t bootEntry; /* entry point of the image*/
    __PACKED_UNION
    {
        uint32_t ram_addr;
        uint32_t flash_offset;
    }
    img_start;

    uint8_t hash[HAL_BOOT2_HASH_SIZE]; /*hash of the image*/

    uint32_t rsv1;
    uint32_t rsv2;
    uint32_t crc32;
} ;

typedef struct
{
    uint8_t encrypt_type;
    uint8_t sign_type;
    uint8_t key_sel;
    uint8_t img_valid;

    uint8_t no_segment;
    uint8_t cache_enable;
    uint8_t cache_way_disable;
    uint8_t hash_ignore;

    uint8_t aes_region_lock;
    uint8_t halt_cpu1;
    uint8_t cpu_type;
    uint8_t r[1];

    __PACKED_UNION
    {
        uint32_t segment_cnt;
        uint32_t img_len;
    }
    img_segment_info;

    uint32_t msp_val;
    uint32_t entry_point;
    __PACKED_UNION
    {
        uint32_t ram_addr;
        uint32_t flash_offset;
    }
    img_start;
    uint32_t sig_len;
    uint32_t sig_len2;

    uint32_t deal_len;
    uint32_t max_input_len;

    uint8_t img_hash[HAL_BOOT2_HASH_SIZE]; //hash of the whole (all)images
    uint8_t aes_iv[16 + 4];                 //iv in boot header

    uint8_t eckye_x[HAL_BOOT2_ECC_KEYXSIZE];  //ec key in boot header
    uint8_t eckey_y[HAL_BOOT2_ECC_KEYYSIZE];  //ec key in boot header
    uint8_t eckey_x2[HAL_BOOT2_ECC_KEYXSIZE]; //ec key in boot header
    uint8_t eckey_y2[HAL_BOOT2_ECC_KEYYSIZE]; //ec key in boot header

    uint8_t signature[HAL_BOOT2_SIGN_MAXSIZE];  //signature in boot header
    uint8_t signature2[HAL_BOOT2_SIGN_MAXSIZE]; //signature in boot header

} boot2_image_config;


extern boot2_efuse_hw_config g_efuse_cfg;
extern uint8_t g_ps_mode;
extern uint32_t g_user_hash_ignored;
extern struct device *dev_check_hash;


uint32_t hal_boot2_custom(void);
void hal_boot2_reset_sec_eng(void);
void hal_boot2_sw_system_reset(void);
void hal_boot2_set_psmode_status(uint32_t flag);
uint32_t hal_boot2_get_psmode_status(void);
uint32_t hal_boot2_get_user_fw(void);
void hal_boot2_clr_user_fw(void);
void hal_boot2_get_efuse_cfg(boot2_efuse_hw_config *efuse_cfg);
int32_t hal_boot2_get_clk_cfg(hal_pll_config *cfg);
void hal_boot2_sboot_finish(void);
void hal_boot2_uart_gpio_init(void);
void hal_boot2_debug_uart_gpio_init(void);
void hal_boot2_debug_uart_gpio_deinit(void);
int32_t hal_boot_parse_bootheader(boot2_image_config *boot_img_cfg, uint8_t *data);
void hal_boot2_clean_cache(void);

#endif