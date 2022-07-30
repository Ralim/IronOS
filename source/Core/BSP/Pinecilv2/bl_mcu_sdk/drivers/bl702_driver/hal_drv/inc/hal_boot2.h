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

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "bl702_sflash.h"
#include "bl702_glb.h"

#define BL_TCM_BASE           BL702_TCM_BASE
#define BL_SYS_CLK_PLL        GLB_SYS_CLK_DLL144M
#define BL_SFLASH_CLK         GLB_SFLASH_CLK_72M
#define HAL_PLL_CFG_MAGICCODE "PCFG"

#define HAL_BOOT2_PK_HASH_SIZE                256 / 8
#define HAL_BOOT2_IMG_HASH_SIZE               256 / 8
#define HAL_BOOT2_ECC_KEYXSIZE                256 / 8
#define HAL_BOOT2_ECC_KEYYSIZE                256 / 8
#define HAL_BOOT2_SIGN_MAXSIZE                (2048 / 8)
#define HAL_BOOT2_DEADBEEF_VAL                0xdeadbeef
#define HAL_BOOT2_CPU0_MAGIC                  "BFNP"
#define HAL_BOOT2_CPU1_MAGIC                  "BFAP"
#define HAL_BOOT2_CP_FLAG                     0x02
#define HAL_BOOT2_MP_FLAG                     0x01
#define HAL_BOOT2_SP_FLAG                     0x00

#define HAL_BOOT2_SUPPORT_DECOMPRESS          1 /* 1 support decompress, 0 not support */
#define HAL_BOOT2_SUPPORT_USB_IAP             0 /* 1 support decompress, 0 not support */
#define HAL_BOOT2_SUPPORT_EFLASH_LOADER_RAM   1 /* 1 support decompress, 0 not support */
#define HAL_BOOT2_SUPPORT_EFLASH_LOADER_FLASH 0 /* 1 support decompress, 0 not support */

#define HAL_BOOT2_CPU_GROUP_MAX               1
#define HAL_BOOT2_CPU_MAX                     1
#define HAL_BOOT2_RAM_IMG_COUNT_MAX           0

#define HAL_BOOT2_FW_IMG_OFFSET_AFTER_HEADER  4*1024  
typedef struct
{
    uint8_t encrypted[HAL_BOOT2_CPU_GROUP_MAX];
    uint8_t sign[HAL_BOOT2_CPU_GROUP_MAX];
    uint8_t hbn_check_sign;
    uint8_t rsvd[1];
    uint8_t chip_id[8];
    uint8_t pk_hash_cpu0[HAL_BOOT2_PK_HASH_SIZE];
    uint8_t pk_hash_cpu1[HAL_BOOT2_PK_HASH_SIZE];
    uint8_t uart_download_cfg;
    uint8_t sf_pin_cfg;
    uint8_t keep_dbg_port_closed;
    uint8_t boot_pin_cfg;
} boot2_efuse_hw_config;

struct __attribute__((packed, aligned(4))) hal_flash_config 
{
    uint32_t magicCode; /*'FCFG'*/
    SPI_Flash_Cfg_Type cfg;
    uint32_t crc32;
} ;

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


struct __attribute__((packed, aligned(4))) hal_basic_cfg_t {
    uint32_t sign_type          : 2; /* [1: 0]   for sign */
    uint32_t encrypt_type       : 2; /* [3: 2]   for encrypt */
    uint32_t key_sel            : 2; /* [5: 4]   key slot */
    uint32_t xts_mode           : 1; /* [6]      for xts mode */
    uint32_t aes_region_lock    : 1; /* [7]      rsvd */
    uint32_t no_segment         : 1; /* [8]      no segment info */
    uint32_t boot2_enable       : 1; /* [9]      boot2 enable */
    uint32_t boot2_rollback     : 1; /* [10]     boot2 rollback */
    uint32_t cpu_master_id      : 4; /* [14: 11] master id */
    uint32_t notload_in_bootrom : 1; /* [15]     notload in bootrom */
    uint32_t crc_ignore         : 1; /* [16]     ignore crc */
    uint32_t hash_ignore        : 1; /* [17]     hash ignore */
    uint32_t power_on_mm        : 1; /* [18]     power on mm */
    uint32_t em_sel             : 3; /* [21: 19] em_sel */
    uint32_t cmds_en            : 1; /* [22]     command spliter enable */
    uint32_t cmds_wrap_mode     : 2; /* [24: 23] cmds wrap mode */
    uint32_t cmds_wrap_len      : 4; /* [28: 25] cmds wrap len */
    uint32_t icache_invalid     : 1; /* [29] icache invalid */
    uint32_t dcache_invalid     : 1; /* [30] dcache invalid */
    uint32_t fpga_halt_release  : 1; /* [31] FPGA halt release function */

    uint32_t group_image_offset;     /* flash controller offset */
    uint32_t aes_region_len;         /* aes region length */

    uint32_t img_len_cnt;            /* image length or segment count */
    uint32_t hash[8];                /* hash of the image */
};

struct __attribute__((packed, aligned(4))) hal_cpu_cfg_t {
    uint8_t config_enable;          /* coinfig this cpu */
    uint8_t halt_cpu;               /* halt this cpu */
    uint8_t cache_enable  : 1;      /* cache setting */
    uint8_t cache_wa      : 1;      /* cache setting */
    uint8_t cache_wb      : 1;      /* cache setting */
    uint8_t cache_wt      : 1;      /* cache setting */
    uint8_t cache_way_dis : 4;      /* cache setting */
    uint8_t rsvd;

    uint32_t image_address_offset;  /* image address on flash */
    uint32_t boot_entry;            /* entry point of the m0 image */
    uint32_t msp_val;               /* msp value */
};


struct  hal_bootheader_t
{
    uint32_t magicCode; /*'BFXP'*/
    uint32_t rivison;
    struct hal_flash_config flash_cfg;
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

    uint8_t hash[HAL_BOOT2_IMG_HASH_SIZE]; /*hash of the image*/

    uint32_t rsv1;
    uint32_t rsv2;
    uint32_t crc32;
} ;

typedef struct
{
    uint8_t img_valid;
    uint8_t pk_src;
    uint8_t rsvd[2];

    struct hal_basic_cfg_t basic_cfg;

    struct hal_cpu_cfg_t cpu_cfg[HAL_BOOT2_CPU_MAX];

    uint8_t aes_iv[16 + 4];                         //iv in boot header

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
#if HAL_BOOT2_SUPPORT_USB_IAP
void hal_boot2_debug_usb_port_init(void);
#endif

void hal_boot2_debug_uart_gpio_deinit(void);
int32_t hal_boot_parse_bootheader(boot2_image_config *boot_img_cfg, uint8_t *data);
void hal_boot2_clean_cache(void);
BL_Err_Type hal_boot2_set_cache(uint8_t cont_read, boot2_image_config *boot_img_cfg);
void hal_boot2_get_ram_img_cnt(char* img_name[],uint32_t *ram_img_cnt );
void hal_boot2_get_img_info(uint8_t *data, uint32_t *image_offset, uint32_t *img_len,uint8_t **hash);
void hal_boot2_release_cpu(uint32_t core, uint32_t boot_addr);
uint32_t hal_boot2_get_xip_addr(uint32_t flash_addr);
uint32_t hal_boot2_get_grp_count(void);
uint32_t hal_boot2_get_cpu_count(void);
uint32_t hal_boot2_get_feature_flag(void);
uint32_t hal_boot2_get_bootheader_offset(void);
#ifdef __cplusplus
}
#endif
#endif