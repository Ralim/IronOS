/**
  ******************************************************************************
  * @file    blsp_media_boot.c
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

#include "stdint.h"
#include "string.h"
#include "blsp_port.h"
#include "bflb_platform.h"
#include "blsp_bootinfo.h"
#include "blsp_common.h"
#include "blsp_boot_parser.h"
#include "blsp_media_boot.h"
#include "softcrc.h"
#include "bflb_eflash_loader_interface.h"
#include "hal_uart.h"

extern int main(void);
/****************************************************************************/ /**
 * @brief  Media boot calculate hash
 *
 * @param  startAddr: Start address to calculate
 * @param  totalLen: Data length to calculate
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
static int32_t blsp_mediaboot_cal_hash(uint32_t start_addr, uint32_t total_len)
{
    int32_t deal_len = 0;
    int32_t read_len = 0;
    uint32_t addr = start_addr;
    int32_t ret;
    uint32_t dump_cnt = 0;
    while (deal_len < total_len) {
        read_len = total_len - deal_len;

        if (read_len > BFLB_BOOT2_READBUF_SIZE) {
            read_len = BFLB_BOOT2_READBUF_SIZE;
        }

        ret = blsp_mediaboot_read(addr, g_boot2_read_buf, read_len);

        if (ret != BFLB_BOOT2_SUCCESS) {
            return ret;
        }

        if (dump_cnt == 0) {
            if (blsp_boot2_dump_critical_flag()) {
                blsp_dump_data(g_boot2_read_buf, read_len);
            }

            dump_cnt = 1;
        }

            
        /* Update hash*/
        sec_hash_update(&hash_handle, g_boot2_read_buf, read_len);

        addr += read_len;
        deal_len += read_len;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Media boot read signature
 *
 * @param  addr: Start address to read signature
 * @param  len: Signature length pointer
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
static int32_t blsp_mediaboot_read_signaure(uint32_t addr, uint32_t *len)
{
    int32_t ret = BFLB_BOOT2_SUCCESS;
    uint32_t sig_len = 0;
    uint8_t *ptmp;

    /* Read signature*/
    ret = blsp_mediaboot_read(addr, (uint8_t *)&sig_len, sizeof(sig_len));

    if(ret != BFLB_BOOT2_SUCCESS)
    {
        return ret;
    }

    addr += sizeof(sig_len);

    if(sig_len > HAL_BOOT2_SIGN_MAXSIZE)
    {
        return BFLB_BOOT2_IMG_SIGNATURE_LEN_ERROR;
    }

    /* Tail 4 bytes for crc */
    ptmp = ((uint8_t *)g_boot2_read_buf);
    ret = blsp_mediaboot_read(addr, (uint8_t *)(ptmp + sizeof(sig_len)), sig_len + 4);

    if(ret != BFLB_BOOT2_SUCCESS)
    {
        return ret;
    }

    ARCH_MemCpy_Fast(g_boot2_read_buf, &sig_len, sizeof(sig_len));
    addr += (sig_len + 4);
    *len = sig_len;

    return ret;
}
/****************************************************************************/ /**
 * @brief  Media boot parse one firmware
 *
 * @param  g_boot_img_cfg: Boot image config pointer to hold parsed information
 * @param  bootheaderAddr: Bootheader start address
 * @param  imgAddr: Image start address
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
static int32_t blsp_mediaboot_parse_one_group(boot2_image_config *boot_img_cfg, uint32_t boot_header_addr, uint32_t img_addr)
{
    uint32_t addr;
    int32_t ret;
    uint32_t sig_len=0;

    addr=boot_header_addr+hal_boot2_get_bootheader_offset();
    /* Read boot header*/
    MSG("R header from %08x\r\n", addr);
    ret = blsp_mediaboot_read(addr, g_boot2_read_buf, sizeof(struct  hal_bootheader_t));

    if (ret != BFLB_BOOT2_SUCCESS) {
        return ret;
    }

    if (blsp_boot2_dump_critical_flag()) {
        blsp_dump_data(g_boot2_read_buf, sizeof(struct  hal_bootheader_t));
    }

    addr += sizeof(struct  hal_bootheader_t);
    ret = hal_boot_parse_bootheader(boot_img_cfg, (uint8_t *)g_boot2_read_buf);

    if (ret != BFLB_BOOT2_SUCCESS) {
        return ret;
    }
    /* we init hash here since aes iv need to be caled as image hash */
    sec_hash_init(&hash_handle, SEC_HASH_SHA256);

    /* Due to OTA, the flash_offset is changed, so copy from partition info */
    boot_img_cfg->basic_cfg.group_image_offset = img_addr;

    /* If sign enable,get pk key and signature*/
    if(boot_img_cfg->basic_cfg.sign_type){
        /* Read public key */
        MSG("R PK\r\n");
        ret=blsp_mediaboot_read(addr,g_boot2_read_buf,sizeof(boot_pk_config));
        if(ret!=BFLB_BOOT2_SUCCESS){
            return ret;
        }
        if(blsp_boot2_dump_critical_flag()){
            blsp_dump_data(g_boot2_read_buf,sizeof(boot_pk_config));
        }
        addr+=sizeof(boot_pk_config);
        ret=blsp_boot_parse_pkey(boot_img_cfg,(uint8_t *)g_boot2_read_buf,1);
        if(ret!=BFLB_BOOT2_SUCCESS){
            return ret;
        }
        if(hal_boot2_get_grp_count()>1){
            /* Read public key 2*/
            MSG("R PK2\r\n");
            ret=blsp_mediaboot_read(addr,g_boot2_read_buf,sizeof(boot_pk_config));
            if(ret!=BFLB_BOOT2_SUCCESS){
                return ret;
            }
            if(blsp_boot2_dump_critical_flag()){
                blsp_dump_data(g_boot2_read_buf,sizeof(boot_pk_config));
            }
            addr+=sizeof(boot_pk_config);
            ret=blsp_boot_parse_pkey(boot_img_cfg,(uint8_t *)g_boot2_read_buf,0);
            if(ret!=BFLB_BOOT2_SUCCESS){
                return ret;
            }
        }
        /* Read signature*/
        MSG_DBG("R SIG1\r\n");
        blsp_mediaboot_read_signaure(addr,&sig_len);
        if(ret!=BFLB_BOOT2_SUCCESS){
            return ret;
        }
        if(blsp_boot2_dump_critical_flag()){
            blsp_dump_data(g_boot2_read_buf,sig_len);
        }
        /*len+data+crc*/
        addr+=sizeof(sig_len);
        addr+=(sig_len+4);
        ret=blsp_boot_parse_signature(boot_img_cfg,(uint8_t *)g_boot2_read_buf,1);
        if(ret!=BFLB_BOOT2_SUCCESS){
            return ret;
        }

        if(hal_boot2_get_grp_count()>1){
            /* Read signature2*/
            MSG_DBG("R SIG2\r\n");
            blsp_mediaboot_read_signaure(addr,&sig_len);
            if(ret!=BFLB_BOOT2_SUCCESS){
                return ret;
            }
            if(blsp_boot2_dump_critical_flag()){
                blsp_dump_data(g_boot2_read_buf,sig_len);
            }
            /*len+data+crc*/
            addr+=sizeof(sig_len);
            addr+=(sig_len+4);
            ret=blsp_boot_parse_signature(boot_img_cfg,(uint8_t *)g_boot2_read_buf,0);
            if(ret!=BFLB_BOOT2_SUCCESS){
                return ret;
            }
        }
    }

    /* If encrypt enable,get AES key*/
    if(boot_img_cfg->basic_cfg.encrypt_type){
        /* Read aes iv*/
        MSG_DBG("R IV\r\n");
        ret=blsp_mediaboot_read(addr,g_boot2_read_buf,sizeof(boot_aes_config));
        if(ret!=BFLB_BOOT2_SUCCESS){
            return ret;
        }
        if(blsp_boot2_dump_critical_flag()){
            blsp_dump_data(g_boot2_read_buf,sizeof(boot_aes_config));
        }
        addr+=sizeof(boot_aes_config);
        ret=blsp_boot_parse_aesiv(boot_img_cfg,(uint8_t *)g_boot2_read_buf);
        if(ret!=BFLB_BOOT2_SUCCESS){
            return ret;
        }
    }

    if (boot_img_cfg->basic_cfg.no_segment) {
        /* Flash image */
        if (!boot_img_cfg->basic_cfg.hash_ignore) {
            MSG("Cal hash addr 0x%08x,len %d\r\n",img_addr,boot_img_cfg->basic_cfg.img_len_cnt);
            ret = blsp_mediaboot_cal_hash(img_addr,
                                          boot_img_cfg->basic_cfg.img_len_cnt);

            if (ret != BFLB_BOOT2_SUCCESS) {
                MSG_ERR("Cal hash err\r\n");
                return ret;
            }

            ret = blsp_boot_parser_check_hash(boot_img_cfg);

            if (ret != BFLB_BOOT2_SUCCESS) {
                return ret;
            }
        }

        ret=blsp_boot_parser_check_signature(boot_img_cfg);
        if(ret!=BFLB_BOOT2_SUCCESS){
            return ret;
        }
        boot_img_cfg->img_valid = 1;
    } else {
        boot_img_cfg->img_valid = 0;
    }

    return ret;
}

/****************************************************************************/ /**
 * @brief  Media boot read data
 *
 * @param  addr: Start address to read
 * @param  data: Data buffer to hold read data
 * @param  len: Data length to read
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
int32_t ATTR_TCM_SECTION blsp_mediaboot_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    flash_read(BLSP_BOOT2_XIP_BASE + addr, data, len);

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Media boot main process
 *
 * @param  cpuBootheaderAddr[BLSP_BOOT2_CPU_GROUP_MAX]: CPU bootheader address list
 * @param  cpuRollBack[BLSP_BOOT2_CPU_GROUP_MAX]: CPU need roll back flag hold list
 * @param  rollBack: 1 for rollback when imge error occurs, 0 for not rollback when imge error occurs
 *
 * @return int32_t
 *
*******************************************************************************/
int32_t blsp_mediaboot_main(uint32_t group_boot_header_addr[BLSP_BOOT2_CPU_GROUP_MAX], uint8_t group_roll_back[BLSP_BOOT2_CPU_GROUP_MAX],uint8_t roll_back)
{
    int32_t ret;
    uint32_t i = 0,core;
    uint32_t valid_group_found = 0;
    uint32_t boot_header_addr[BLSP_BOOT2_CPU_GROUP_MAX];

    MSG("Media boot main\r\n");

    /* Reset some parameters*/
    for (i = 0; i < BLSP_BOOT2_CPU_GROUP_MAX; i++) {
        memset(&g_boot_img_cfg[i], 0, sizeof(g_boot_img_cfg[i]));
        boot_header_addr[i] = group_boot_header_addr[i];
        group_roll_back[i] = 0;
    }

    /* Try to boot from flash */
    for (i = 0; i < BLSP_BOOT2_CPU_GROUP_MAX; i++) {
        if (boot_header_addr[i] == 0) {
            MSG_ERR("Group %d not boot\r\n", i);
            continue;
        }

        ret = blsp_mediaboot_parse_one_group(&g_boot_img_cfg[i], boot_header_addr[i],
                                          boot_header_addr[i] + BFLB_FW_IMG_OFFSET_AFTER_HEADER);

        if (ret != BFLB_BOOT2_SUCCESS) {
            MSG_ERR("Group %d parse fail\r\n", i);
            group_roll_back[i] = 1;
        } else {
            valid_group_found++;
        }
    }

    /* roll_back == 1 means need to check rock back, and can not jump away */
    if ((valid_group_found != hal_boot2_get_grp_count()) && (1 == roll_back)) {
#if BLSP_BOOT2_CPU_GROUP_MAX>1
        if (boot_header_addr[1] == 0 && (valid_group_found == 1)) 
        {
            MSG("One group Only\r\n");
        } 
        else
#endif
        {
            MSG_ERR("Group roll back\r\n");
            return BFLB_BOOT2_IMG_Roll_Back;
        }
    }

    if (valid_group_found == 0) {
        MSG_ERR("no valid group found\r\n");
        return BFLB_BOOT2_IMG_ALL_INVALID_ERROR;
    }

    /* Get msp and pc value */
    for (i = 0; i < BLSP_BOOT2_CPU_GROUP_MAX; i++) {
        if (!g_boot_img_cfg[i].img_valid) {
            continue;
        }
        for(core = 0; core < BLSP_BOOT2_CPU_MAX; core++){
            if (g_boot_img_cfg[i].cpu_cfg[core].boot_entry == 0) {

#ifdef ARCH_RISCV
                g_boot_img_cfg[i].cpu_cfg[core].boot_entry = ((uint32_t )main) & 0xff000000;
#endif
            }
        }
    }
    for(i=0;i<BLSP_BOOT2_CPU_GROUP_MAX;i++){
        if(!g_boot_img_cfg[i].img_valid){
            continue;
        }
        for(core = 0; core < BLSP_BOOT2_CPU_MAX; core++){
            if(g_boot_img_cfg[i].cpu_cfg[core].config_enable){
                if(g_boot_img_cfg[i].cpu_cfg[core].halt_cpu == 0){
                    MSG("group[%d] offset %08x ,core[%d] offset %08x ",i,
                        g_boot_img_cfg[i].basic_cfg.group_image_offset,
                        core,
                        g_boot_img_cfg[i].cpu_cfg[core].image_address_offset);
                    MSG("bootentry %08x\r\n",g_boot_img_cfg[i].cpu_cfg[core].boot_entry);
                }
            }
        }
    }
#if BLSP_BOOT2_CPU_MAX>1
    MSG("group[%d],core[%d] halt cpu %d\r\n",0,1,g_boot_img_cfg[0].cpu_cfg[1].halt_cpu);
#endif
    blsp_boot2_show_timer();
#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH||BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM
    if (0 == bflb_eflash_loader_if_handshake_poll(0)) {
        bflb_eflash_loader_main();
    }
#endif
    
    /* Prepare jump to entry*/
    blsp_mediaboot_pre_jump();

    /* We should never get here unless something is wrong */
    return BFLB_BOOT2_FAIL;
}




