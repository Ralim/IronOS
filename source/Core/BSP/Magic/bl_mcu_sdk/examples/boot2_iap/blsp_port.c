/**
  ******************************************************************************
  * @file    blsp_port.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case c file
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

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "blsp_port.h"
#include "bflb_platform.h"
#include "blsp_bootinfo.h"
#include "blsp_common.h"
#include "blsp_media_boot.h"
#include "tzc_sec_reg.h"
#include "hal_boot2.h"

extern uint32_t __boot2_pass_param_addr;
static uint32_t start_time=0;
/****************************************************************************/ /**
* @brief  Boot2 show timer for cal boot time
*
* @param  None
*
* @return None
*
*******************************************************************************/
void blsp_boot2_start_timer(void)
{
    start_time=(uint32_t)bflb_platform_get_time_ms();
}

/****************************************************************************/ /**
* @brief  Boot2 show timer for cal boot time
*
* @param  None
*
* @return None
*
*******************************************************************************/
void blsp_boot2_show_timer(void)
{
    MSG("Counter value=%d\n", (unsigned int)bflb_platform_get_time_ms()-start_time);
}

/****************************************************************************/ /**
 * @brief  Read power save mode
 *
 * @param  None
 *
 * @return BFLB_PSM_ACTIVE or BFLB_PSM_HBN
 *
*******************************************************************************/
uint8_t blsp_read_power_save_mode(void)
{
    if (hal_boot2_get_psmode_status() == HBN_STATUS_WAKEUP_FLAG) {
        return BFLB_PSM_HBN;
    } else {
        return BFLB_PSM_ACTIVE;
    }
}

/****************************************************************************/ /**
 * @brief  Boot2 Pass parameter to FW
 *
 * @param  data: Data pointer to pass
 * @param  len: Data length
 *
 * @return None
 *
*******************************************************************************/
void blsp_boot2_pass_parameter(void *data, uint32_t len)
{
    static uint8_t *p_parameter = NULL;
    
    if (len == 0) {
        //GLB_Set_EM_Sel(0); //system init has done
        //p_parameter = (uint8_t *)(0x42020000 + 60 * 1024);
        //p_parameter = (uint8_t *)(0x42030000+103*1024);
        p_parameter = (uint8_t *)&__boot2_pass_param_addr;
        return;
    }

    MSG("pass param addr %08x,len %d\r\n", p_parameter,len);
    ARCH_MemCpy_Fast(p_parameter, data, len);
    p_parameter += len;
}

/****************************************************************************/ /**
 * @brief  Release other CPU
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION blsp_boot2_releae_other_cpu(void)
{
#if BLSP_BOOT2_RAM_IMG_COUNT_MAX>0
    uint32_t i=0;
    pentry_t pentry;
#endif
#if BLSP_BOOT2_CPU_MAX>1
    uint32_t group,core;
    /* deal Other CPUs' entry point and release*/
    for(group = 0; group < BLSP_BOOT2_CPU_GROUP_MAX; group++){
        for(core = 1; core < BLSP_BOOT2_CPU_MAX; core++){
            if(g_boot_img_cfg[group].cpu_cfg[core].config_enable){
                if(g_boot_img_cfg[group].cpu_cfg[core].halt_cpu == 0){
                    hal_boot2_release_cpu(core, g_boot_img_cfg[group].cpu_cfg[core].boot_entry);
                }
            }
        }
    }
#endif
#if BLSP_BOOT2_RAM_IMG_COUNT_MAX>0
    /* deal with ram image */
    for(i=0;i<BLSP_BOOT2_RAM_IMG_COUNT_MAX;i++){
        if(ram_img_config[i].valid && ram_img_config[i].core != 0 && 
           ram_img_config[i].core != 0xf){
            hal_boot2_release_cpu(ram_img_config[i].core,ram_img_config[i].boot_addr);
        }
    }
    for(i=0;i<BLSP_BOOT2_RAM_IMG_COUNT_MAX;i++){
        if(ram_img_config[i].valid&&ram_img_config[i].core==0){
            pentry=(pentry_t)ram_img_config[i].boot_addr;
            cpu_global_irq_disable();
            hal_boot2_clean_cache();
            pentry();
        }
    }
#endif
}

/****************************************************************************/ /**
 * @brief  Set encryption config
 *
 * @param  index: Region index
 * @param  g_boot_img_cfg: Boot image config pointer to hold parsed information
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
int32_t ATTR_TCM_SECTION blsp_boot2_set_encrypt(uint8_t index, boot2_image_config *g_boot_img_cfg)
{
    uint32_t aes_enabled = 0;
    uint32_t len = 0;

    if(g_boot_img_cfg->img_valid==0){
        return BFLB_BOOT2_SUCCESS;
    }
    /* FIXME:,1:lock, should be 1??*/
    if (g_boot_img_cfg->basic_cfg.encrypt_type != 0) {
        len = g_boot_img_cfg->basic_cfg.img_len_cnt;

        if (len != 0) {
            SF_Ctrl_AES_Set_Key_BE(index, NULL, (SF_Ctrl_AES_Key_Type)(g_boot_img_cfg->basic_cfg.encrypt_type - 1));
            SF_Ctrl_AES_Set_IV_BE(index, g_boot_img_cfg->aes_iv, g_boot_img_cfg->basic_cfg.group_image_offset);

            SF_Ctrl_AES_Set_Region(index, 1 /*enable this region*/, 1 /*hardware key*/,
                                   g_boot_img_cfg->basic_cfg.group_image_offset,
                                   g_boot_img_cfg->basic_cfg.group_image_offset + len - 1,
                                   g_boot_img_cfg->basic_cfg.aes_region_lock /*lock*/);
            aes_enabled = 1;
        }
    }

    if (aes_enabled) {
        SF_Ctrl_AES_Enable_BE();
        SF_Ctrl_AES_Enable();
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get dump critical data flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_dump_critical_flag(void)
{
    return 0;
}




