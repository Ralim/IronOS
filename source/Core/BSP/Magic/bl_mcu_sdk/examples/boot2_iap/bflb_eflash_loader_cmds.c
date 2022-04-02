/**
  ******************************************************************************
  * @file    blsp_eflash_loader_cmds.c
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
#include "bflb_eflash_loader.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "softcrc.h"
#include "bflb_platform.h"
#include "partition.h"
#include "hal_flash.h"
#include "hal_sec_hash.h"
#include "blsp_media_boot.h"

#define BFLB_EFLASH_LOADER_CHECK_LEN   2048
#define BFLB_EFLASH_MAX_SIZE           2 * 1024 * 1024

extern struct device *download_uart;

#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM
static struct image_cfg_t image_cfg;
static struct bootrom_img_ctrl_t img_ctrl;
static struct segment_header_t segment_hdr;
uint32_t eflash_loader_cmd_ack_buf[16];
/*for bl602*/
static int32_t bflb_bootrom_cmd_get_bootinfo(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_bootrom_cmd_load_bootheader(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_bootrom_cmd_load_segheader(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_bootrom_cmd_load_segdata(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_bootrom_cmd_check_img(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_bootrom_cmd_run(uint16_t cmd, uint8_t *data, uint16_t len);
static void bflb_bootrom_cmd_ack(uint32_t result);
#endif


#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH
static uint32_t g_eflash_loader_error = 0;
/* for bl702 */
static int32_t bflb_eflash_loader_cmd_read_jedec_id(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_reset(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_erase_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_write_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_read_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_xip_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_write_flash_check(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_set_flash_para(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_xip_read_flash_start(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_xip_read_flash_finish(uint16_t cmd, uint8_t *data, uint16_t len);
#endif

#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH||BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM    
static const struct eflash_loader_cmd_cfg_t eflash_loader_cmds[] = {
#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM    
    /*for bl602*/
    { BFLB_EFLASH_LOADER_CMD_GET_BOOTINFO, EFLASH_LOADER_CMD_ENABLE, bflb_bootrom_cmd_get_bootinfo },
    { BFLB_EFLASH_LOADER_CMD_LOAD_BOOTHEADER, EFLASH_LOADER_CMD_ENABLE, bflb_bootrom_cmd_load_bootheader },
    { BFLB_EFLASH_LOADER_CMD_LOAD_SEGHEADER, EFLASH_LOADER_CMD_ENABLE, bflb_bootrom_cmd_load_segheader },
    { BFLB_EFLASH_LOADER_CMD_LOAD_SEGDATA, EFLASH_LOADER_CMD_ENABLE, bflb_bootrom_cmd_load_segdata },
    { BFLB_EFLASH_LOADER_CMD_CHECK_IMG, EFLASH_LOADER_CMD_ENABLE, bflb_bootrom_cmd_check_img },
    { BFLB_EFLASH_LOADER_CMD_RUN, EFLASH_LOADER_CMD_ENABLE, bflb_bootrom_cmd_run },
#endif

#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH
    /* for bl702 */
    { BFLB_EFLASH_LOADER_CMD_RESET, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_reset },
    { BFLB_EFLASH_LOADER_CMD_FLASH_ERASE, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_erase_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_WRITE, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_write_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_READ, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_read_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_WRITE_CHECK, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_write_flash_check },
    { BFLB_EFLASH_LOADER_CMD_FLASH_SET_PARA, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_set_flash_para },
    { BFLB_EFLASH_LOADER_CMD_FLASH_READSHA, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_readSha_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_XIP_READSHA, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_xip_readSha_flash },
    { BFLB_EFLASH_LOADER_CMD_XIP_READ_START, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_xip_read_flash_start },
    { BFLB_EFLASH_LOADER_CMD_XIP_READ_FINISH, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_xip_read_flash_finish },
    { BFLB_EFLASH_LOADER_CMD_FLASH_READ_JEDECID, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_read_jedec_id },
#endif        
};
#endif

#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM 
/* ack host with command process result */
static void bflb_bootrom_cmd_ack(uint32_t result)
{
    if (result == 0) {
        /*OK*/
        eflash_loader_cmd_ack_buf[0] = BFLB_BOOTROM_CMD_ACK;
        bflb_eflash_loader_if_write((uint32_t *)eflash_loader_cmd_ack_buf, 2);
        return;
    } else {
        /* FL+Error code(2bytes) */
        eflash_loader_cmd_ack_buf[0] = BFLB_BOOTROM_CMD_NACK | ((result << 16) & 0xffff0000);
        bflb_eflash_loader_if_write(eflash_loader_cmd_ack_buf, 4);
    }
}

/* for bl602 eflash loader */
static int32_t bflb_bootrom_cmd_get_bootinfo(uint16_t cmd, uint8_t *data, uint16_t len)
{
    /*OK(2)+len(2)+bootrom version(4)+OTP(16)*/
    uint8_t *bootinfo = (uint8_t *)eflash_loader_cmd_ack_buf;
    uint8_t otp_cfg[16] = { 0 };

    MSG("get bootinfo\r\n");

    eflash_loader_cmd_ack_buf[0] = BFLB_BOOTROM_CMD_ACK;
    bootinfo[2] = 0x14;
    bootinfo[3] = 00;
    *((uint32_t *)(bootinfo + 4)) = BFLB_BOOTROM_VERSION;
    memcpy(bootinfo + 8, &otp_cfg, 16);

    bflb_eflash_loader_if_write(eflash_loader_cmd_ack_buf, bootinfo[2] + 4);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_bootrom_parse_bootheader(uint8_t *data)
{
    struct bootheader_t *header = (struct bootheader_t *)data;
    uint32_t crc;
    uint32_t crcpass = 0;

    if (header->bootcfg.bval.crc_ignore == 1 && header->crc32 == BOOTROM_DEADBEEF_VAL) {
        crcpass = 1;
    } else {
        crc = BFLB_Soft_CRC32((uint8_t *)header, sizeof(struct bootheader_t) - sizeof(header->crc32));
        if (header->crc32 == crc) {
            crcpass = 1;
        }
    }

    if (crcpass) {
        if (header->bootcfg.bval.notload_in_bootrom) {
            return BFLB_EFLASH_LOADER_IMG_BOOTHEADER_NOT_LOAD_ERROR;
        }

        /* for boot2, one CPU only */
        /*get which CPU's img it is*/

        if (0 == memcmp((void *)&header->magiccode, "BFNP", sizeof(header->magiccode))) {
            //current_cpu=i;
            img_ctrl.pkhash_src = 0; //boot_cpu_cfg[i].pkhash_src;
        } else {
            return BFLB_EFLASH_LOADER_IMG_BOOTHEADER_MAGIC_ERROR;
        }

        image_cfg.entrypoint = 0;
        /* set image valid 0 as default */
        image_cfg.img_valid = 0;

        /* deal with pll config */
        //bflb_bootrom_clk_set_from_user(&header->clkCfg,1);

        /* encrypt and sign */
        image_cfg.encrypt_type = header->bootcfg.bval.encrypt_type;
        image_cfg.sign_type = header->bootcfg.bval.sign;
        image_cfg.key_sel = header->bootcfg.bval.key_sel;

        /* xip relative */
        image_cfg.no_segment = header->bootcfg.bval.no_segment;
        image_cfg.cache_select = header->bootcfg.bval.cache_select;
        image_cfg.aes_region_lock = header->bootcfg.bval.aes_region_lock;
        image_cfg.halt_ap = header->bootcfg.bval.halt_ap;
        image_cfg.cache_way_disable = header->bootcfg.bval.cache_way_disable;

        image_cfg.hash_ignore = header->bootcfg.bval.hash_ignore;
        /* firmware len*/
        image_cfg.img_segment_info.segment_cnt = header->img_segment_info.segment_cnt;

        /*boot entry and flash offset */
        image_cfg.entrypoint = header->bootentry;
        image_cfg.img_start.flashoffset = header->img_start.flashoffset;

        if (image_cfg.img_segment_info.segment_cnt == 0) {
            return BFLB_EFLASH_LOADER_IMG_SEGMENT_CNT_ERROR;
        }
        /*clear segment_cnt*/
        img_ctrl.segment_cnt = 0;

    } else {
        return BFLB_EFLASH_LOADER_IMG_BOOTHEADER_CRC_ERROR;
    }

    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_bootrom_cmd_load_bootheader(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    MSG("load bootheader\r\n");

    if (len != sizeof(struct bootheader_t)) {
        ret = BFLB_EFLASH_LOADER_IMG_BOOTHEADER_LEN_ERROR;
    } else {
        ret = bflb_bootrom_parse_bootheader(data);

        if (BFLB_EFLASH_LOADER_SUCCESS == ret) {
            if (image_cfg.sign_type) {
                img_ctrl.state = BOOTROM_IMG_PK;
            } else if (image_cfg.encrypt_type) {
                img_ctrl.state = BOOTROM_IMG_AESIV;
            } else {
                img_ctrl.state = BOOTROM_IMG_SEGHEADER;
            }
        }
    }

    bflb_bootrom_cmd_ack(ret);

    return ret;
}

static int32_t bflb_bootrom_is_boot_dst_valid(uint32_t addr, uint32_t len)
{
    return 1;
}

int32_t bflb_bootrom_parse_seg_header(uint8_t *data)
{
    struct segment_header_t *hdr;

    hdr = (struct segment_header_t *)data;

    if (hdr->crc32 == BFLB_Soft_CRC32(hdr, sizeof(struct segment_header_t) - 4)) {
        memcpy(&segment_hdr, hdr, sizeof(struct segment_header_t));
        if (bflb_bootrom_is_boot_dst_valid(segment_hdr.destaddr, segment_hdr.len) == 1) {
            return BFLB_EFLASH_LOADER_SUCCESS;
        } else {
            return BFLB_EFLASH_LOADER_IMG_SECTIONHEADER_DST_ERROR;
        }
    } else {
        return BFLB_EFLASH_LOADER_IMG_SECTIONHEADER_CRC_ERROR;
    }
}

static int32_t bflb_bootrom_cmd_load_segheader(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
    uint8_t *segdatainfo = (uint8_t *)eflash_loader_cmd_ack_buf;

    MSG("load SegHdr\r\n");

    if (img_ctrl.state != BOOTROM_IMG_SEGHEADER) {
        ret = BFLB_EFLASH_LOADER_CMD_SEQ_ERROR;
    } else {
        if (len != sizeof(struct segment_header_t)) {
            ret = BFLB_EFLASH_LOADER_IMG_SECTIONHEADER_LEN_ERROR;
        } else {
            memset(&segment_hdr, 0, sizeof(struct segment_header_t));
            img_ctrl.segdata_recv_len = 0;
            ret = bflb_bootrom_parse_seg_header(data);
            if (ret == BFLB_EFLASH_LOADER_SUCCESS) {
                //bflb_bootrom_printe("dest=%08x,len=%d\r\n",segment_hdr.destaddr,segment_hdr.len);
                img_ctrl.state = BOOTROM_IMG_SEGDATA;
            }
        }
    }

    /* if segheader is encrypted, then ack segment len*/
    if (ret == BFLB_EFLASH_LOADER_SUCCESS) {
        /*ack segdata len*/
        eflash_loader_cmd_ack_buf[0] = BFLB_BOOTROM_CMD_ACK;
        segdatainfo[2] = sizeof(segment_hdr);
        segdatainfo[3] = 0x00;
        memcpy(&segdatainfo[4], (void *)&segment_hdr, sizeof(segment_hdr));
        bflb_eflash_loader_if_write(eflash_loader_cmd_ack_buf, segdatainfo[2] + 4);
    } else {
        bflb_bootrom_cmd_ack(ret);
    }
    return ret;
}

static int32_t bflb_bootrom_cmd_load_segdata(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    MSG("load SegData\r\n");

    if (img_ctrl.state != BOOTROM_IMG_SEGDATA) {
        ret = BFLB_EFLASH_LOADER_CMD_SEQ_ERROR;
        goto finished;
    }

    if (image_cfg.encrypt_type && len % 16 != 0) {
        ret = BFLB_EFLASH_LOADER_IMG_SECTIONDATA_LEN_ERROR;
        //bflb_bootrom_printe("len error,len=%d\r\n",len);
        goto finished;
    }

    if (img_ctrl.segdata_recv_len + len > segment_hdr.len) {
        ret = BFLB_EFLASH_LOADER_IMG_SECTIONDATA_TLEN_ERROR;
        //bflb_bootrom_printe("tlen error,receive=%d,indicator=%d\r\n",
        //img_ctrl.segdata_recv_len+len,segment_hdr.len);
        goto finished;
    }

    /*no encryption,copy directlly */
    ARCH_MemCpy_Fast((void *)segment_hdr.destaddr, data, len);
    MSG("segment_hdr.destaddr 0x%08x, len %d,data %02x %02x %02x %02x \r\n", segment_hdr.destaddr, len, data[0], data[1], data[2], data[3]);
    img_ctrl.segdata_recv_len += len;
    segment_hdr.destaddr += len;

    if (img_ctrl.segdata_recv_len >= segment_hdr.len) {
        /*finish loading one segment*/
        img_ctrl.segment_cnt++;
        if (img_ctrl.segment_cnt == image_cfg.img_segment_info.segment_cnt) {
            img_ctrl.state = BOOTROM_IMG_CHECK;
        } else {
            img_ctrl.state = BOOTROM_IMG_SEGHEADER;
        }
    }

finished:
    bflb_bootrom_cmd_ack(ret);

    return ret;
}

static int32_t bflb_bootrom_cmd_check_img(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    MSG("check img\r\n");

    if (img_ctrl.state != BOOTROM_IMG_CHECK) {
        ret = BFLB_EFLASH_LOADER_IMG_HALFBAKED_ERROR;
        bflb_bootrom_cmd_ack(ret);
        return ret;
    }

    /*default,set state to BOOTROM_IMG_BOOTHEADER*/
    img_ctrl.state = BOOTROM_IMG_BOOTHEADER;

    /*finally, check hash and signature*/
    ret = BFLB_EFLASH_LOADER_SUCCESS; //bflb_bootrom_check_hash();
    if (ret == BFLB_EFLASH_LOADER_SUCCESS) {
        ret = BFLB_EFLASH_LOADER_SUCCESS; //bflb_bootrom_check_signature();
        bflb_bootrom_cmd_ack(ret);
        if (ret == BFLB_EFLASH_LOADER_SUCCESS) {
            img_ctrl.state = BOOTROM_IMG_RUN;
            image_cfg.img_valid = 1;
        }
    } else {
        bflb_bootrom_cmd_ack(ret);
    }

    return ret;
}

static void bflb_eflash_loader_exit_delay_us(uint32_t cnt)
{
    volatile uint32_t i = (32 / 5) * cnt;
    while (i--)
        ;
}

static void bflb_eflash_loader_jump_entry(void)
{
    pentry_t pentry = 0;

    /* deal NP's entry point */
    if (image_cfg.img_valid) {
        pentry = (pentry_t)image_cfg.entrypoint;
        MSG("image_cfg.entrypoint 0x%08x\r\n", image_cfg.entrypoint);
        if (pentry != NULL) {
            pentry();
        }
    }

    /*if cann't jump(open jtag only?),stay here */
    while (1) {
        /*use soft delay only */
        bflb_eflash_loader_exit_delay_us(100);
    }
}

static int32_t bflb_bootrom_cmd_run(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    MSG("run\r\n");

    if (img_ctrl.state != BOOTROM_IMG_RUN) {
        ret = BFLB_EFLASH_LOADER_CMD_SEQ_ERROR;
        bflb_bootrom_cmd_ack(ret);
        return ret;
    }

    bflb_bootrom_cmd_ack(ret);
    //bflb_eflash_loader_usart_if_wait_tx_idle(BFLB_BOOTROM_IF_TX_IDLE_TIMEOUT);
    bflb_platform_delay_ms(BFLB_BOOTROM_IF_TX_IDLE_TIMEOUT);

    /* get msp and pc value */

    MSG("image_cfg.img_valid %d\r\n", image_cfg.img_valid);

    if (image_cfg.img_valid) {
        if (image_cfg.entrypoint == 0) {
            if (image_cfg.img_start.ramaddr != 0) {
                image_cfg.entrypoint = image_cfg.img_start.ramaddr;
            } else {
                image_cfg.entrypoint = BL_TCM_BASE;
            }
        }
    }
    MSG("jump\r\n");

    /*jump to entry */
    bflb_eflash_loader_jump_entry();
    return ret;
}

#endif




#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH
/* ack host with command process result */
static void bflb_eflash_loader_cmd_ack(uint32_t result)
{
    if (result == 0) {
        /*OK*/
        g_eflash_loader_cmd_ack_buf[0] = BFLB_EFLASH_LOADER_CMD_ACK;
        bflb_eflash_loader_if_write((uint32_t *)g_eflash_loader_cmd_ack_buf, 2);
        return;
    } else {
        /* FL+Error code(2bytes) */
        g_eflash_loader_cmd_ack_buf[0] = BFLB_EFLASH_LOADER_CMD_NACK | ((result << 16) & 0xffff0000);
        bflb_eflash_loader_if_write(g_eflash_loader_cmd_ack_buf, 4);
    }
}

static int32_t bflb_eflash_loader_cmd_read_jedec_id(uint16_t cmd, uint8_t *data, uint16_t len)
{
    uint32_t ackdata[2];
    uint8_t *tmp_buf;
    MSG("JID\n");
    ackdata[0] = BFLB_EFLASH_LOADER_CMD_ACK;
    tmp_buf = (uint8_t *)ackdata;
    /*ack read jedec ID */
    tmp_buf[2] = 4;
    tmp_buf[3] = 0;
    flash_read_jedec_id((uint8_t *)&ackdata[1]);
    ackdata[1] &= 0x00ffffff;
    ackdata[1] |= 0x80000000;
    bflb_eflash_loader_if_write((uint32_t *)ackdata, 4 + 4);
    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_reset(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    pt_table_set_iap_para(&p_iap_param);
    pt_table_dump();
    MSG("RST\n");

    bflb_eflash_loader_cmd_ack(ret);
    bflb_eflash_loader_if_wait_tx_idle(BFLB_EFLASH_LOADER_IF_TX_IDLE_TIMEOUT);

    /* add for bl702, will impact on boot pin read */
    hal_boot2_set_psmode_status(0x594c440B);

    /* FPGA POR RST NOT work,so add system reset */
    bflb_platform_delay_us(10);
    hal_boot2_sw_system_reset();

    return ret;
}

static int32_t bflb_eflash_loader_cmd_erase_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
    uint32_t startaddr, endaddr;

    MSG("E\n");

    if (len != 8) {
        ret = BFLB_EFLASH_LOADER_FLASH_ERASE_PARA_ERROR;
    } else {
        /*clean write error, since write usually behand erase*/
        g_eflash_loader_error = BFLB_EFLASH_LOADER_SUCCESS;

        memcpy(&startaddr, data, 4);
        memcpy(&endaddr, data + 4, 4);

        p_iap_param.iap_img_len = endaddr - startaddr + 1;

        MSG("from%08xto%08x\n", p_iap_param.iap_start_addr, p_iap_param.iap_start_addr + p_iap_param.iap_img_len - 1);

        if (SUCCESS != flash_erase(p_iap_param.iap_start_addr, p_iap_param.iap_img_len)) {
            MSG("fail\n");
            ret = BFLB_EFLASH_LOADER_FLASH_ERASE_ERROR;
        }
    }

    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t ATTR_TCM_SECTION bflb_eflash_loader_cmd_write_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
    uint32_t write_len;

    MSG("W\n");

    if (len <= 4) {
        ret = BFLB_EFLASH_LOADER_FLASH_WRITE_PARA_ERROR;
    } else {
        //memcpy(&startaddr,data,4);
        write_len = len - 4;
        MSG("to%08x,%d\n", p_iap_param.iap_write_addr, write_len);

        if (p_iap_param.iap_write_addr < 0xffffffff) {
            if (SUCCESS != flash_write(p_iap_param.iap_write_addr, data + 4, write_len)) {
                /*error , response again with error */
                MSG("fail\r\n");
                ret = BFLB_EFLASH_LOADER_FLASH_WRITE_ERROR;
                g_eflash_loader_error = ret;
            } else {
                bflb_eflash_loader_cmd_ack(ret);
                p_iap_param.iap_write_addr += write_len;
                //bflb_eflash_loader_printe("Write suss\r\n");
                return BFLB_EFLASH_LOADER_SUCCESS;
            }
        } else {
            ret = BFLB_EFLASH_LOADER_FLASH_WRITE_ADDR_ERROR;
        }
        MSG("%d\n", bflb_platform_get_time_us());
    }

    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t bflb_eflash_loader_cmd_read_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_xip_read_flash_start(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t bflb_eflash_loader_cmd_xip_read_flash_finish(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    MSG("exit\n");
    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t bflb_eflash_loader_cmd_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_xip_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
    uint32_t startaddr, read_len;
    //SEC_Eng_SHA256_Ctx sha_ctx;
    //SEC_ENG_SHA_ID_Type shaId = SEC_ENG_SHA_ID0;
    uint16_t sha_len = 32;
    uint8_t ackdata[32+4];
    MSG("XRSha\n");
    uint8_t *g_sha_in_buf = vmalloc(BFLB_EFLASH_LOADER_READBUF_SIZE);
    if (len != 8) {
        ret = BFLB_EFLASH_LOADER_FLASH_WRITE_PARA_ERROR;
        bflb_eflash_loader_cmd_ack(ret);
    } else {
        startaddr = p_iap_param.iap_start_addr;
        read_len = p_iap_param.iap_img_len;
        MSG("from%08x,%d\n", startaddr, read_len);
        //MSG("!!!Be careful that SHA input data should locate at OCRAM \n");
        /* Init SHA and input SHA temp buffer for scattered data and g_padding buffer */
        //Sec_Eng_SHA256_Init(&sha_ctx, shaId, SEC_ENG_SHA256, g_sha_tmp_buf, g_padding);
        //Sec_Eng_SHA_Start(shaId);
        sec_hash_init(&hash_handle, SEC_HASH_SHA256);

        while (read_len > 0) {
            if (read_len > BFLB_EFLASH_LOADER_READBUF_SIZE) {
                blsp_mediaboot_read(startaddr, g_sha_in_buf, BFLB_EFLASH_LOADER_READBUF_SIZE);
                /*cal sha here*/
                //Sec_Eng_SHA256_Update(&sha_ctx, shaId, (uint8_t *)g_sha_in_buf, BFLB_EFLASH_LOADER_READBUF_SIZE);
                //device_write(dev_check_hash, 0, g_sha_in_buf, BFLB_EFLASH_LOADER_READBUF_SIZE);
                sec_hash_update(&hash_handle, g_sha_in_buf, BFLB_EFLASH_LOADER_READBUF_SIZE);
                read_len -= BFLB_EFLASH_LOADER_READBUF_SIZE;
                startaddr += BFLB_EFLASH_LOADER_READBUF_SIZE;
            } else {
                blsp_mediaboot_read(startaddr, g_sha_in_buf, read_len);
                /*cal sha here*/
                //Sec_Eng_SHA256_Update(&sha_ctx, shaId, (uint8_t *)g_sha_in_buf, read_len);
                //device_write(dev_check_hash, 0, g_sha_in_buf, read_len);
                sec_hash_update(&hash_handle, g_sha_in_buf, read_len);
                read_len -= read_len;
                startaddr += read_len;
            }
        }

        //Sec_Eng_SHA256_Finish(&sha_ctx, shaId, &ackdata[4]);
        //device_read(dev_check_hash, 0, &ackdata[4], 0);
        //device_close(dev_check_hash);
        sec_hash_finish(&hash_handle, (uint8_t *)&ackdata[4]);

        for (sha_len = 0; sha_len < 32; sha_len++) {
            MSG("\r\n");
            MSG("%02x ", ackdata[4 + sha_len]);
        }

        sha_len = 32;
        /*ack read data */
        ackdata[0] = BFLB_EFLASH_LOADER_CMD_ACK & 0xff;
        ackdata[1] = (BFLB_EFLASH_LOADER_CMD_ACK >> 8) & 0xff;
        ackdata[2] = sha_len & 0xff;
        ackdata[3] = (sha_len >> 8) & 0xff;
        bflb_eflash_loader_if_write((uint32_t *)ackdata, sha_len + 4);
    }

    return ret;
}

static int32_t bflb_eflash_loader_cmd_write_flash_check(uint16_t cmd, uint8_t *data, uint16_t len)
{
    MSG("WC\n");

    bflb_eflash_loader_cmd_ack(g_eflash_loader_error);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_set_flash_para(uint16_t cmd, uint8_t *data, uint16_t len)
{
    bflb_eflash_loader_cmd_ack(BFLB_EFLASH_LOADER_SUCCESS);
    return BFLB_EFLASH_LOADER_SUCCESS;
}
#endif

int32_t bflb_eflash_loader_cmd_process(uint8_t cmdid, uint8_t *data, uint16_t len)
{
    
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH||BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM    
    int i = 0;
    for (i = 0; i < sizeof(eflash_loader_cmds) / sizeof(eflash_loader_cmds[0]); i++) {
        if (eflash_loader_cmds[i].cmd == cmdid) {
            if (EFLASH_LOADER_CMD_ENABLE == eflash_loader_cmds[i].enabled && NULL != eflash_loader_cmds[i].cmd_process) {
                ret = eflash_loader_cmds[i].cmd_process(cmdid, data, len);
            } else {
                return BFLB_EFLASH_LOADER_CMD_ID_ERROR;
            }

            break;
        }
    }
#endif    

    return ret;
}
