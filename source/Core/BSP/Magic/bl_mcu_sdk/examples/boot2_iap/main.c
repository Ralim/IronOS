/**
  ******************************************************************************
  * @file    blsp_boot2.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case c file
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

#include "bflb_platform.h"
#include "bflb_eflash_loader_interface.h"
#include "blsp_port.h"
#include "blsp_bootinfo.h"
#include "blsp_media_boot.h"
#include "blsp_boot_decompress.h"
#include "blsp_common.h"
#include "blsp_version.h"
#include "partition.h"
#include "softcrc.h"
#include "hal_uart.h"
#include "hal_flash.h"
#include "hal_boot2.h"

uint8_t *g_boot2_read_buf;
boot2_image_config g_boot_img_cfg[BLSP_BOOT2_CPU_GROUP_MAX];
boot2_efuse_hw_config g_efuse_cfg;
uint8_t g_ps_mode = BFLB_PSM_ACTIVE;
uint32_t g_user_hash_ignored = 0;
uint8_t g_usb_init_flag = 0;
ATTR_NOCACHE_RAM_SECTION sec_hash_handle_t hash_handle ;
int blsp_do_ram_image_boot(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, 
                           pt_table_entry_config *pt_entry);
/****************************************************************************/ /**
 * @brief  Boot2 runs error call back function
 *
 * @param  log: Log to print
 *
 * @return None
 *
*******************************************************************************/
static void blsp_boot2_on_error(void *log)
{
    while (1) {
#if BLSP_BOOT2_SUPPORT_USB_IAP
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_USB);
        if (0 == g_usb_init_flag) {
            bflb_eflash_loader_if_init();
            g_usb_init_flag = 1;
        }

        if (0 == bflb_eflash_loader_if_handshake_poll(1)) {
            bflb_eflash_loader_main();
        }
#endif
#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH||BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_UART);
        if (0 == bflb_eflash_loader_if_handshake_poll(0)) {
            bflb_eflash_loader_main();
        }
#endif
        MSG_ERR("%s\r\n", (char *)log);
        arch_delay_ms(500);
    }
}

/****************************************************************************/ /**
 * @brief  Boot2 Dump partition entry
 *
 * @param  ptEntry: Partition entry pointer to dump
 *
 * @return None
 *
*******************************************************************************/
void blsp_dump_pt_entry(pt_table_entry_config *pt_entry)
{
    MSG("Name=%s\r\n", pt_entry->name);
    MSG("Age=%d\r\n", (unsigned int)pt_entry->age);
    MSG("active Index=%d\r\n", (unsigned int)pt_entry->active_index);
    MSG("active start_address=%08x\r\n", (unsigned int)pt_entry->start_address[pt_entry->active_index & 0x01]);
}

/****************************************************************************/ /**
 * @brief  Boot2 check XZ FW and do decompression
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return 1 for find XZ FW and decompress success, 0 for other cases
 *
*******************************************************************************/
#if BLSP_BOOT2_SUPPORT_DECOMPRESS
static int blsp_boot2_check_xz_fw(pt_table_id_type activeID, pt_table_stuff_config *ptStuff, pt_table_entry_config *ptEntry)
{
    uint8_t buf[6];

    if (BFLB_BOOT2_SUCCESS != blsp_mediaboot_read(ptEntry->start_address[ptEntry->active_index], buf, sizeof(buf))) {
        MSG_ERR("Read fw fail\r\n");
        return 0;
    }

    if (blsp_boot2_dump_critical_flag()) {
        blsp_dump_data(buf, sizeof(buf));
    }

    if (blsp_boot2_verify_xz_header(buf) == 1) {
        MSG("XZ image\r\n");

        if (BFLB_BOOT2_SUCCESS == blsp_boot2_update_fw(activeID, ptStuff, ptEntry)) {
            return 1;
        } else {
            MSG_ERR("Img decompress fail\r\n");
            /* Set flag to make it not boot */
            ptEntry->active_index = 0;
            ptEntry->start_address[0] = 0;
            return 0;
        }
    }

    return 0;
}
#endif

/****************************************************************************/ /**
 * @brief  Boot2 copy firmware from OTA region to normal region
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
static int blsp_boot2_do_fw_copy(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    uint8_t active_index = pt_entry->active_index;
    uint32_t src_address = pt_entry->start_address[active_index & 0x01];
    uint32_t dest_address = pt_entry->start_address[!(active_index & 0x01)];
    uint32_t dest_max_size = pt_entry->max_len[!(active_index & 0x01)];
    uint32_t total_len = pt_entry->len;
    uint32_t deal_len = 0;
    uint32_t cur_len = 0;

    MSG("OTA copy src address %08x, dest address %08x, total len %d\r\n", src_address, dest_address, total_len);

    if (SUCCESS != flash_erase(dest_address, dest_max_size)) {
        MSG_ERR("Erase flash fail");
        return BFLB_BOOT2_FLASH_ERASE_ERROR;
    }

    while (deal_len < total_len) {
        cur_len = total_len - deal_len;

        if (cur_len > BFLB_BOOT2_READBUF_SIZE) {
            cur_len = BFLB_BOOT2_READBUF_SIZE;
        }

        if (BFLB_BOOT2_SUCCESS != blsp_mediaboot_read(src_address, g_boot2_read_buf, cur_len)) {
            MSG_ERR("Read FW fail when copy\r\n");
            return BFLB_BOOT2_FLASH_READ_ERROR;
        }

        if (SUCCESS != flash_write(dest_address, g_boot2_read_buf, cur_len)) {
            MSG_ERR("Write flash fail");
            return BFLB_BOOT2_FLASH_WRITE_ERROR;
        }

        src_address += cur_len;
        dest_address += cur_len;
        deal_len += cur_len;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Boot2 deal with one firmware
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 * @param  fwName: Firmware name pointer
 * @param  type: Firmware name ID
 *
 * @return 0 for partition table changed,need re-parse,1 for partition table or entry parsed successfully
 *
*******************************************************************************/
static int blsp_boot2_deal_one_fw(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff,
                                  pt_table_entry_config *pt_entry, uint8_t *fw_name,
                                  pt_table_entry_type type)
{
    uint32_t ret;

    if (fw_name != NULL) {
        MSG("Get FW:%s\r\n", fw_name);
        ret = pt_table_get_active_entries_by_name(pt_stuff, fw_name, pt_entry);
    } else {
        MSG("Get FW ID:%d\r\n", type);
        ret = pt_table_get_active_entries_by_id(pt_stuff, type, pt_entry);
    }

    if (PT_ERROR_SUCCESS != ret) {
        MSG_ERR("Entry not found\r\n");
    } else {
        //blsp_dump_pt_entry(pt_entry);
        MSG("Check Img\r\n");
#if BLSP_BOOT2_SUPPORT_DECOMPRESS
        if (blsp_boot2_check_xz_fw(active_id, pt_stuff, pt_entry) == 1) {
            return 0;
        }
#endif
        /* Check if this partition need copy */
        if (pt_entry->active_index >= 2) {
            MSG("Do image copy\r\n");
            if (BFLB_BOOT2_SUCCESS == blsp_boot2_do_fw_copy(active_id, pt_stuff, pt_entry)) {
                pt_entry->active_index = !(pt_entry->active_index & 0x01);
                pt_entry->age++;
                ret = pt_table_update_entry((pt_table_id_type)(!active_id), pt_stuff, pt_entry);

                if (ret != PT_ERROR_SUCCESS) {
                    MSG_ERR("Update Partition table entry fail\r\n");
                    return BFLB_BOOT2_FAIL;
                } else {
                    MSG("Done\r\n");
                }

                return 0;
            }
        }
    }

    return 1;
}

/****************************************************************************/ /**
 * @brief  Boot2 Roll back pt entry
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return boot_error_code
 *
*******************************************************************************/
#ifdef BLSP_BOOT2_ROLLBACK
int32_t blsp_boot2_rollback_ptentry(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    int32_t ret;

    pt_entry->active_index = !(pt_entry->active_index & 0x01);
    pt_entry->age++;
    ret = pt_table_update_entry((pt_table_id_type)(!active_id), pt_stuff, pt_entry);

    if (ret != PT_ERROR_SUCCESS) {
        MSG_ERR("Update PT entry fail\r\n");
        return BFLB_BOOT2_FAIL;
    }

    return BFLB_BOOT2_SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Boot2 get mfg start up request
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return 0 for partition table changed,need re-parse,1 for partition table or entry parsed successfully
 *
*******************************************************************************/
static void blsp_boot2_get_mfg_startreq(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry, uint8_t *user_fw_name)
{
    uint32_t ret;
    uint32_t len = 0;
    uint8_t tmp[16 + 1] = { 0 };

    ret = pt_table_get_active_entries_by_name(pt_stuff, (uint8_t *)"mfg", pt_entry);

    if (PT_ERROR_SUCCESS == ret) {
        MSG("read mfg flag addr:%08x,", pt_entry->start_address[0] + MFG_START_REQUEST_OFFSET);
        flash_read(pt_entry->start_address[0] + MFG_START_REQUEST_OFFSET, tmp, sizeof(tmp) - 1);
        MSG("%s\r\n", tmp);
        if (tmp[0] == '0' || tmp[0] == '1') {
            len = strlen((char *)tmp);
            if (len < 9) {
                arch_memcpy(user_fw_name, tmp, len);
                MSG("%s", tmp);
            }
        }
    } else {
        MSG("MFG not found\r\n");
    }
}

/****************************************************************************/ /**
 * @brief  Boot2 main function
 *
 * @param  None
 *
 * @return Return value
 *
*******************************************************************************/
int main(void)
{
    uint32_t ret = 0, i = 0;
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;
    /* Init to zero incase only one cpu boot up*/
    pt_table_entry_config pt_entry[BLSP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint32_t boot_header_addr[BLSP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint8_t boot_need_rollback[BLSP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint8_t pt_parsed = 1;
    uint8_t user_fw_name[9] = { 0 };
    uint32_t user_fw;
#ifdef BLSP_BOOT2_ROLLBACK
    uint8_t roll_backed = 0;
#endif
    uint8_t mfg_mode_flag = 0;
    //boot_clk_config clk_cfg;
    uint8_t flash_cfg_buf[4 + sizeof(SPI_Flash_Cfg_Type) + 4] = { 0 };
    uint32_t crc;
    uint8_t *flash_cfg = NULL;
    uint32_t flash_cfg_len = 0;
    extern void system_mtimer_clock_init(void);
    extern void peripheral_clock_init(void);

    system_mtimer_clock_init();
    peripheral_clock_init();

    blsp_boot2_start_timer();
    hal_boot2_get_efuse_cfg(&g_efuse_cfg);
    
#if (BLSP_BOOT2_MODE == BOOT2_MODE_RELEASE)
    bflb_platform_print_set(1);
#endif

#if (BLSP_BOOT2_MODE == BOOT2_MODE_DEBUG)
    bflb_platform_print_set(0);
#endif

#if (BLSP_BOOT2_MODE == BOOT2_MODE_DEEP_DEBUG)
    bflb_platform_print_set(0);
    hal_boot2_debug_uart_gpio_init();
#endif

    bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_UART);
    bflb_eflash_loader_if_init();

    simple_malloc_init(g_malloc_buf, sizeof(g_malloc_buf));
    g_boot2_read_buf = vmalloc(BFLB_BOOT2_READBUF_SIZE);
    ret = hal_boot2_custom();
    MSG("custom %04x\r\n",ret);
    ret = flash_init();
    if(ret){
        while(1){
            MSG_ERR("flash cfg get err\r\n");
            arch_delay_ms(500);
        }
    }
    MSG("flash init ret %d\r\n",(int)ret);

    MSG("BLSP Boot2 start:%s,%s\r\n", __DATE__, __TIME__);
    MSG("Group=%d,CPU Count=%d\r\n", hal_boot2_get_grp_count(),hal_boot2_get_cpu_count());
#if BLSP_BOOT2_CPU_MAX>1
    if (hal_boot2_get_feature_flag() == HAL_BOOT2_CP_FLAG) {
        MSG("Boot As Co-CPU\r\n");
    }else{
        MSG("Boot As Normal\r\n");
    }
#endif
#ifdef BL_SDK_VER
    MSG("sdk:%s\r\n", BL_SDK_VER);
#else
    MSG("MCU SDK:%s\r\n", MCU_SDK_VERSION);
    MSG("BSP Driver:%s\r\n", BSP_DRIVER_VERSION);
    MSG("BSP Common:%s\r\n", BSP_COMMON_VERSION);
#endif

    /* Reset Sec_Eng for using */
    hal_boot2_reset_sec_eng();
    /* Get power save mode */
    g_ps_mode = blsp_read_power_save_mode();

    /* Get User specified FW */
    user_fw = hal_boot2_get_user_fw();
    arch_memcpy(user_fw_name, &user_fw, 4);
    MSG("user_fw %s\r\n", user_fw_name);

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(flash_erase, flash_write, flash_read);

#if BLSP_BOOT2_SUPPORT_USB_IAP
    if (memcmp(user_fw_name, (char *)"USB", 3) == 0) {
        hal_boot2_clr_user_fw();
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_USB);
        bflb_eflash_loader_if_init();
        g_usb_init_flag = 1;
        if (0 == bflb_eflash_loader_if_handshake_poll(user_fw_name[3])) {
            bflb_eflash_loader_main();
        }
    }
#endif

    //pt_table_dump();
#if BLSP_BOOT2_RAM_IMG_COUNT_MAX>0
    /* Deal with ram image: copy from flash to ram like */
    do {
        active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

        if (PT_TABLE_ID_INVALID == active_id) {
            blsp_boot2_on_error("No valid PT\r\n");
        }
        ret=blsp_do_ram_image_boot(active_id, &pt_table_stuff[active_id], &pt_entry[0]);
    } while (ret != 0);
#endif
    /* Deal with flash image */
    while (1) {
        mfg_mode_flag = 0;
        do {
            active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

            if (PT_TABLE_ID_INVALID == active_id) {
                blsp_boot2_on_error("No valid PT\r\n");
            }

            MSG("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

            blsp_boot2_get_mfg_startreq(active_id, &pt_table_stuff[active_id], &pt_entry[0], user_fw_name);

            /* Get entry and boot */
            if (user_fw_name[0] == '0') {
                g_user_hash_ignored = 1;
                pt_parsed = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], &user_fw_name[1], PT_ENTRY_FW_CPU0);
                if (pt_parsed == 0) {
                    continue;
                } else {
                    hal_boot2_clr_user_fw();
                }
                mfg_mode_flag = 1;
                user_fw_name[0] = 0;
            } else if (user_fw_name[0] == '1' && BLSP_BOOT2_CPU_GROUP_MAX > 1) {
                g_user_hash_ignored = 1;
                pt_parsed = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[1], &user_fw_name[1], PT_ENTRY_FW_CPU1);
                if (pt_parsed == 0) {
                    continue;
                } else {
                    hal_boot2_clr_user_fw();
                }
                mfg_mode_flag = 1;
                user_fw_name[0] = 0;
            } else {
                /* partition type 0 is group0*/
                pt_parsed = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], NULL, PT_ENTRY_FW_CPU0);
                if (pt_parsed == 0) {
                    continue;
                }
                /* partition type 1 is group1 */
                if (BLSP_BOOT2_CPU_GROUP_MAX > 1) {
                    pt_parsed = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[1], NULL, PT_ENTRY_FW_CPU1);
                    if (pt_parsed == 0) {
                        continue;
                    }
                }
            }
        } while (pt_parsed == 0);

        /* Pass data to App*/
        blsp_boot2_pass_parameter(NULL, 0);
        /* Pass active partition table ID */
        blsp_boot2_pass_parameter(&active_id, 4);
        /* Pass active partition table content: table header+ entries +crc32 */
        blsp_boot2_pass_parameter(&pt_table_stuff[active_id], sizeof(pt_table_config) + 4 +
                                                                  pt_table_stuff[active_id].pt_table.entryCnt * sizeof(pt_table_entry_config));

        /* Pass flash config */
        if (pt_entry[0].start_address[pt_entry[0].active_index] != 0) {
            //flash_read(BLSP_BOOT2_XIP_BASE + pt_entry[0].start_address[pt_entry[0].active_index] + 8, flash_cfg_buf, sizeof(flash_cfg_buf));
            /* Include magic and CRC32 */
            flash_get_cfg(&flash_cfg, &flash_cfg_len);
            arch_memcpy(flash_cfg_buf, "FCFG", 4);
            arch_memcpy(flash_cfg_buf + 4, flash_cfg, flash_cfg_len);
            crc = BFLB_Soft_CRC32(flash_cfg, flash_cfg_len);
            arch_memcpy(flash_cfg_buf + 4 + flash_cfg_len, &crc, sizeof(crc));
            blsp_boot2_pass_parameter(flash_cfg_buf, sizeof(flash_cfg_buf));
        }

        MSG("Boot start\r\n");

        for (i = 0; i < BLSP_BOOT2_CPU_GROUP_MAX; i++) {
            boot_header_addr[i] = pt_entry[i].start_address[pt_entry[i].active_index];
        }

#ifdef BLSP_BOOT2_ROLLBACK
        /* mfg mode do not need roll back */
        if (roll_backed == 0 && mfg_mode_flag == 0) {
            ret = blsp_mediaboot_main(boot_header_addr, boot_need_rollback, 1);
        } else {
            ret = blsp_mediaboot_main(boot_header_addr, boot_need_rollback, 0);
        }
#else
        ret = blsp_mediaboot_main(boot_header_addr, boot_need_rollback, 0);
#endif

        if (mfg_mode_flag == 1) {
            continue;
        }

#ifdef BLSP_BOOT2_ROLLBACK

        /* If rollback is done, we still fail, break */
        if (roll_backed) {
            break;
        }

        MSG("Boot return 0x%04x\r\nCheck Rollback\r\n", ret);

        for (i = 0; i < BLSP_BOOT2_CPU_GROUP_MAX; i++) {
            if (boot_need_rollback[i] != 0) {
                MSG("Rollback group %d\r\n", i);

                if (BFLB_BOOT2_SUCCESS == blsp_boot2_rollback_ptentry(active_id, &pt_table_stuff[active_id], &pt_entry[i])) {
                    roll_backed = 1;
                }
            }
        }

        /* If need no rollback, boot fail due to other reseaon instead of imgae issue,break */
        if (roll_backed == 0) {
            break;
        }

#else
        break;
#endif
    }

    /* We should never get here unless boot fail */
    MSG_ERR("Media boot return %d\r\n", ret);

    while (1) {
#if BLSP_BOOT2_SUPPORT_USB_IAP
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_USB);
        if (0 == g_usb_init_flag) {
            bflb_eflash_loader_if_init();
            g_usb_init_flag = 1;
        }

        if (0 == bflb_eflash_loader_if_handshake_poll(1)) {
            bflb_eflash_loader_main();
        }
#endif
#if BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH||BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_UART);
        if (0 == bflb_eflash_loader_if_handshake_poll(0)) {
            bflb_eflash_loader_main();
        }
#endif
        MSG_ERR("BLSP boot2 fail\r\n");
        arch_delay_ms(500);
    }
}

void bfl_main()
{
    main();
}
