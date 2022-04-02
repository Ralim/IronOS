
#include "hal_boot2.h"
#include "hal_gpio.h"
#include "hal_flash.h"
#include "bl602_glb.h"
#include "bl602_ef_ctrl.h"
#include "bl602_hbn.h"
#include "bl602_xip_sflash.h"
#include "bl602_sf_cfg.h"
#include "bl602_sf_cfg_ext.h"
#include "bl602_glb.h"
#include "bl602_xip_sflash_ext.h"
#include "tzc_sec_reg.h"
#include "softcrc.h"
#include "hal_sec_hash.h"

/**
 * @brief boot2 custom
 *
 * @param None
 * @return uint32
 */
uint32_t hal_boot2_custom(void)
{
    uint32_t sw_cfg, flash_pin_cfg;
    EF_Ctrl_Read_Sw_Usage(0, (uint32_t *)&sw_cfg);
    /* flash_pin_cfg
     * 0:internal flash with io switch,
     * 1:internal flash no io switch,
     * 2:GPIO 17-22
     * 3:GPIO 0-2 & 20-22
    */
    flash_pin_cfg = ((sw_cfg>>16)&0x3);
    if((flash_pin_cfg == 0)||(flash_pin_cfg == 1)){
        gpio_set_mode(GPIO_PIN_20, GPIO_OUTPUT_MODE);
        gpio_write(GPIO_PIN_20, 0);
    }
    
    
    return 0;
}

/**
 * @brief get efuse Boot2 config
 *
 * @param g_efuse_cfg
 * @param
 * @param
 * @return None
 */
void hal_boot2_get_efuse_cfg(boot2_efuse_hw_config *efuse_cfg)
{
    uint32_t tmp;
    uint32_t rootClk;
    uint8_t hdiv = 0, bdiv = 0;

    /* save bclk fclk div and root clock sel */
    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
    rootClk = BL_RD_REG(HBN_BASE, HBN_GLB);

    /* change root clock to rc32m for efuse operation */
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);

    /* Get sign and aes type*/
    EF_Ctrl_Read_Secure_Boot((EF_Ctrl_Sign_Type *)efuse_cfg->sign, (EF_Ctrl_SF_AES_Type *)efuse_cfg->encrypted);
    /* Get hash:aes key slot 0 and slot1*/
    EF_Ctrl_Read_AES_Key(0, (uint32_t *)efuse_cfg->pk_hash_cpu0, 8);
    EF_Ctrl_Read_Chip_ID(efuse_cfg->chip_id);
    /* Get HBN check sign config */
    EF_Ctrl_Read_Sw_Usage(0, &tmp);
    efuse_cfg->hbn_check_sign = (tmp >> 22) & 0x01;

    /* restore bclk fclk div and root clock sel */
    GLB_Set_System_CLK_Div(hdiv, bdiv);
    BL_WR_REG(HBN_BASE, HBN_GLB,rootClk);
    __NOP();__NOP();__NOP();__NOP();
}

/**
 * @brief reset sec eng clock
 *
 * @return
 */
void hal_boot2_reset_sec_eng(void)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_SEC);
}

/**
 * @brief system soft reset
 *
 * @return
 */
void hal_boot2_sw_system_reset(void)
{
    GLB_SW_System_Reset();
}


/**
 * @brief
 *
 * @param flag
 * @param
 * @param
 * @return
 */
void hal_boot2_set_psmode_status(uint32_t flag)
{
    HBN_Set_Status_Flag(flag);
}

/**
 * @brief
 *
 * @param
 * @param
 * @param
 * @return flag
 */
uint32_t hal_boot2_get_psmode_status(void)
{
    return HBN_Get_Status_Flag();
}

/**
 * @brief
 *
 * @param
 * @param
 * @param
 * @return user define flag
 */
uint32_t hal_boot2_get_user_fw(void)
{
    return BL_RD_WORD(HBN_BASE + HBN_RSV0_OFFSET);
}

/**
 * @brief clr user define flag
 *
 * @param
 * @param
 * @param
 * @return
 */
void hal_boot2_clr_user_fw(void)
{
    uint32_t *p = (uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET);
    *p = 0;
}

/**
 * @brief hal_boot2_sboot_finish
 *
 * @return 
 */
void ATTR_TCM_SECTION hal_boot2_sboot_finish(void) 
{
    uint32_t tmp_val;

    tmp_val = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL);

    tmp_val = BL_SET_REG_BITS_VAL(tmp_val, TZC_SEC_TZC_SBOOT_DONE, 0xf);

    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL, tmp_val);
}
/**
 * @brief hal_boot2_uart_gpio_init
 *
 * @return 
 */
void hal_boot2_uart_gpio_init(void)
{
    GLB_GPIO_Type gpios[]={GPIO_PIN_7,GPIO_PIN_16};

    GLB_GPIO_Func_Init(GPIO_FUN_UART,gpios,2);

    GLB_UART_Fun_Sel((GPIO_PIN_7 % 8), GLB_UART_SIG_FUN_UART0_RXD);
    GLB_UART_Fun_Sel((GPIO_PIN_16 % 8), GLB_UART_SIG_FUN_UART0_TXD);  
}

/**
 * @brief hal_boot2_debug_uart_gpio_init
 *
 * @return 
 */
void hal_boot2_debug_uart_gpio_init(void)
{
    GLB_GPIO_Type gpios[]={GPIO_PIN_8};

    GLB_GPIO_Func_Init(GPIO_FUN_UART,gpios,1);

    GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_GPIO8_GPIO15);
    GLB_UART_Fun_Sel(GLB_UART_SIG_4, GLB_UART_SIG_FUN_UART1_TXD);
}
/**
 * @brief hal_boot2_debug_uart_gpio_deinit
 *
 * @return 
 */
void hal_boot2_debug_uart_gpio_deinit(void)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_UART0);
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_UART1);
    GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_NONE);
}

/****************************************************************************/ /**
 * @brief  Check if the input public key is the same as  burned in the efuse
 *
 * @param  g_boot_img_cfg: Boot image config pointer
 * @param  data: Image data pointer
 *
 * @return boot_error_code type
 *
*******************************************************************************/
int32_t hal_boot_parse_bootheader(boot2_image_config *boot_img_cfg, uint8_t *data)
{
    struct  hal_bootheader_t *header = (struct  hal_bootheader_t *)data;
    uint32_t crc;
    uint32_t crc_pass = 0;
    uint32_t i = 0;
    uint32_t *phash = (uint32_t *)header->hash;
    int32_t ret;

    if (header->bootCfg.bval.crcIgnore == 1 && header->crc32 == HAL_BOOT2_DEADBEEF_VAL) {
        //MSG("Crc ignored\r\n");
        crc_pass = 1;
    } else {
        crc = BFLB_Soft_CRC32((uint8_t *)header, sizeof(struct  hal_bootheader_t) - sizeof(header->crc32));

        if (header->crc32 == crc) {
            crc_pass = 1;
        }
    }

    if (crc_pass) {
        if (header->bootCfg.bval.notLoadInBoot) {
            return 0x0202;
        }

        /* Get which CPU's img it is*/
        for (i = 0; i < HAL_BOOT2_CPU_MAX; i++) {
            if (0 == memcmp((void *)&header->magicCode, HAL_BOOT2_CPU0_MAGIC,
                            sizeof(header->magicCode))) {
                break;
            } else if (0 == memcmp((void *)&header->magicCode, HAL_BOOT2_CPU1_MAGIC,
                                   sizeof(header->magicCode))) {
                break;
            }
        }

        if (i == HAL_BOOT2_CPU_MAX) {
            /* No cpu img magic match */
            //MSG_ERR("Magic code error\r\n");
            return 0x0203;
        }

        boot_img_cfg->cpu_type = i;
        boot_img_cfg->entry_point = 0;
        /* Set image valid 0 as default */
        boot_img_cfg->img_valid = 0;

        /* Deal with pll config */

        /* Encrypt and sign */
        boot_img_cfg->encrypt_type = header->bootCfg.bval.encrypt_type;
        boot_img_cfg->sign_type = header->bootCfg.bval.sign;
        boot_img_cfg->key_sel = header->bootCfg.bval.key_sel;

        /* Xip relative */
        boot_img_cfg->no_segment = header->bootCfg.bval.no_segment;
        boot_img_cfg->cache_enable = header->bootCfg.bval.cache_enable;
        boot_img_cfg->aes_region_lock = header->bootCfg.bval.aes_region_lock;
        boot_img_cfg->halt_cpu1 = header->bootCfg.bval.halt_cpu1;
        boot_img_cfg->cache_way_disable = header->bootCfg.bval.cache_way_disable;
        boot_img_cfg->hash_ignore = header->bootCfg.bval.hash_ignore;
        /* Firmware len*/
        boot_img_cfg->img_segment_info.img_len = header->img_segment_info.img_len;

        /* Boot entry and flash offset */
        boot_img_cfg->entry_point = header->bootEntry;
        boot_img_cfg->img_start.flash_offset = header->img_start.flash_offset;

        //MSG("sign %d,encrypt:%d\r\n", boot_img_cfg->sign_type,boot_img_cfg->encrypt_type);

        /* Check encrypt and sign match*/
        if (g_efuse_cfg.encrypted[i] != 0) {
            if (boot_img_cfg->encrypt_type == 0) {
                //MSG_ERR("Encrypt not fit\r\n");
                return 0x0205;
            }
        }

        if (g_efuse_cfg.sign[i] ^ boot_img_cfg->sign_type) {
            //MSG_ERR("sign not fit\r\n");
            boot_img_cfg->sign_type = g_efuse_cfg.sign[i];
            return 0x0206;
        }

        if (g_ps_mode == 1 && (!g_efuse_cfg.hbn_check_sign)) {
            /* In HBN Mode, if user select to ignore hash and sign*/
            boot_img_cfg->hash_ignore = 1;
        } else if ((boot_img_cfg->hash_ignore == 1 && *phash != HAL_BOOT2_DEADBEEF_VAL) ||
                   g_efuse_cfg.sign[i] != 0) {
            /* If signed or user not really want to ignore, hash can't be ignored*/
            boot_img_cfg->hash_ignore = 0;
        }

        if (g_user_hash_ignored) {
            boot_img_cfg->hash_ignore = 1;
        }

        ARCH_MemCpy_Fast(boot_img_cfg->img_hash, header->hash, sizeof(header->hash));

        if (boot_img_cfg->img_segment_info.img_len == 0) {
            return 0x0207;
        }

        /* Start hash here*/
        //Sec_Eng_SHA256_Init(&g_sha_ctx, SEC_ENG_SHA_ID0, SEC_ENG_SHA256, g_sha_tmp_buf, g_padding);
        //Sec_Eng_SHA_Start(SEC_ENG_SHA_ID0);
        device_unregister("dev_check_hash");
        sec_hash_sha256_register(0,"dev_check_hash");
        dev_check_hash = device_find("dev_check_hash");
        if(dev_check_hash){
            ret = device_open(dev_check_hash, 0);
            if(ret){
                //MSG_ERR("hash dev open err\r\n");
                return 0xffff;
            }
        }else{
            //MSG_ERR("hash dev find err\r\n");
            return 0xffff;
        }

    } else {
        //MSG_ERR("bootheader crc error\r\n");
        //blsp_dump_data((uint8_t *)&crc, 4);
        return 0x0204;
    }

    return 0;
}

void hal_boot2_clean_cache(void)
{
    
}
