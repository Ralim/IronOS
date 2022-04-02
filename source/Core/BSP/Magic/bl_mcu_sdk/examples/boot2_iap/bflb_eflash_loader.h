/**
  ******************************************************************************
  * @file    blsp_eflash_loader.h
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
#ifndef __BFLB_EFLASH_LOADER_H__
#define __BFLB_EFLASH_LOADER_H__

#include "bflb_eflash_loader_interface.h"
#include "bflb_eflash_loader_cmds.h"
#include "bflb_eflash_loader_uart.h"
#include "bflb_eflash_loader_usb.h"

/*error code definition*/
typedef enum tag_eflash_loader_error_code_t {
    BFLB_EFLASH_LOADER_SUCCESS = 0x00,

    /*flash*/
    BFLB_EFLASH_LOADER_FLASH_INIT_ERROR = 0x0001,
    BFLB_EFLASH_LOADER_FLASH_ERASE_PARA_ERROR = 0x0002,
    BFLB_EFLASH_LOADER_FLASH_ERASE_ERROR = 0x0003,
    BFLB_EFLASH_LOADER_FLASH_WRITE_PARA_ERROR = 0x0004,
    BFLB_EFLASH_LOADER_FLASH_WRITE_ADDR_ERROR = 0x0005,
    BFLB_EFLASH_LOADER_FLASH_WRITE_ERROR = 0x0006,
    BFLB_EFLASH_LOADER_FLASH_BOOT_PARA_ERROR = 0x0007,
    BFLB_EFLASH_LOADER_FLASH_SET_PARA_ERROR = 0x0008,
    BFLB_EFLASH_LOADER_FLASH_READ_STATUS_REG_ERROR = 0x0009,
    BFLB_EFLASH_LOADER_FLASH_WRITE_STATUS_REG_ERROR = 0x000A,
    BFLB_EFLASH_LOADER_FLASH_DECOMPRESS_WRITE_ERROR = 0x000B,
    BFLB_EFLASH_LOADER_FLASH_WRITE_XZ_ERROR = 0x000C,

    /*cmd*/
    BFLB_EFLASH_LOADER_CMD_ID_ERROR = 0x0101,
    BFLB_EFLASH_LOADER_CMD_LEN_ERROR = 0x0102,
    BFLB_EFLASH_LOADER_CMD_CRC_ERROR = 0x0103,
    BFLB_EFLASH_LOADER_CMD_SEQ_ERROR = 0x0104,

    /*image*/
    BFLB_EFLASH_LOADER_IMG_BOOTHEADER_LEN_ERROR = 0x0201,
    BFLB_EFLASH_LOADER_IMG_BOOTHEADER_NOT_LOAD_ERROR = 0x0202,
    BFLB_EFLASH_LOADER_IMG_BOOTHEADER_MAGIC_ERROR = 0x0203,
    BFLB_EFLASH_LOADER_IMG_BOOTHEADER_CRC_ERROR = 0x0204,
    BFLB_EFLASH_LOADER_IMG_BOOTHEADER_ENCRYPT_NOTFIT = 0x0205,
    BFLB_EFLASH_LOADER_IMG_BOOTHEADER_SIGN_NOTFIT = 0x0206,
    BFLB_EFLASH_LOADER_IMG_SEGMENT_CNT_ERROR = 0x0207,
    BFLB_EFLASH_LOADER_IMG_AES_IV_LEN_ERROR = 0x0208,
    BFLB_EFLASH_LOADER_IMG_AES_IV_CRC_ERROR = 0x0209,
    BFLB_EFLASH_LOADER_IMG_PK_LEN_ERROR = 0x020a,
    BFLB_EFLASH_LOADER_IMG_PK_CRC_ERROR = 0x020b,
    BFLB_EFLASH_LOADER_IMG_PK_HASH_ERROR = 0x020c,
    BFLB_EFLASH_LOADER_IMG_SIGNATURE_LEN_ERROR = 0x020d,
    BFLB_EFLASH_LOADER_IMG_SIGNATURE_CRC_ERROR = 0x020e,
    BFLB_EFLASH_LOADER_IMG_SECTIONHEADER_LEN_ERROR = 0x020f,
    BFLB_EFLASH_LOADER_IMG_SECTIONHEADER_CRC_ERROR = 0x0210,
    BFLB_EFLASH_LOADER_IMG_SECTIONHEADER_DST_ERROR = 0x0211,
    BFLB_EFLASH_LOADER_IMG_SECTIONDATA_LEN_ERROR = 0x0212,
    BFLB_EFLASH_LOADER_IMG_SECTIONDATA_DEC_ERROR = 0x0213,
    BFLB_EFLASH_LOADER_IMG_SECTIONDATA_TLEN_ERROR = 0x0214,
    BFLB_EFLASH_LOADER_IMG_SECTIONDATA_CRC_ERROR = 0x0215,
    BFLB_EFLASH_LOADER_IMG_HALFBAKED_ERROR = 0x0216,
    BFLB_EFLASH_LOADER_IMG_HASH_ERROR = 0x0217,
    BFLB_EFLASH_LOADER_IMG_SIGN_PARSE_ERROR = 0x0218,
    BFLB_EFLASH_LOADER_IMG_SIGN_ERROR = 0x0219,
    BFLB_EFLASH_LOADER_IMG_DEC_ERROR = 0x021a,
    BFLB_EFLASH_LOADER_IMG_ALL_INVALID_ERROR = 0x021b,

    /*IF*/
    BFLB_EFLASH_LOADER_IF_RATE_LEN_ERROR = 0x0301,
    BFLB_EFLASH_LOADER_IF_RATE_PARA_ERROR = 0x0302,
    BFLB_EFLASH_LOADER_IF_PASSWORDERROR = 0x0303,
    BFLB_EFLASH_LOADER_IF_PASSWORDCLOSE = 0x0304,

    /*efuse*/
    BFLB_EFLASH_LOADER_EFUSE_WRITE_PARA_ERROR = 0x0401,
    BFLB_EFLASH_LOADER_EFUSE_WRITE_ADDR_ERROR = 0x0402,
    BFLB_EFLASH_LOADER_EFUSE_WRITE_ERROR = 0x0403,
    BFLB_EFLASH_LOADER_EFUSE_READ_PARA_ERROR = 0x0404,
    BFLB_EFLASH_LOADER_EFUSE_READ_ADDR_ERROR = 0x0405,
    BFLB_EFLASH_LOADER_EFUSE_READ_ERROR = 0x0406,
    BFLB_EFLASH_LOADER_EFUSE_READ_MAC_ERROR = 0x0407,

    /*MISC*/
    BFLB_EFLASH_LOADER_PLL_ERROR = 0xfffc,
    BFLB_EFLASH_LOADER_INVASION_ERROR = 0xfffd,
    BFLB_EFLASH_LOADER_POLLING = 0xfffe,
    BFLB_EFLASH_LOADER_FAIL = 0xffff,

} eflash_loader_error_code_t;

/** private definition**/
#define EFLASH_LOADER_CMD_DISABLE   0x000
#define EFLASH_LOADER_CMD_ENABLE    0x001
#define BFLB_EFLASH_LOADER_CMD_ACK  0x00004B4F
#define BFLB_EFLASH_LOADER_CMD_NACK 0x00004C46

#define BFLB_EFLASH_LOADER_EFUSE0_SIZE 128

/*max data payload in command*/
#define BFLB_EFLASH_LOADER_READBUF_SIZE     (1024 * 16 + 8)
#define BFLB_EFLASH_LOADER_WRITEBUF_SIZE    BFLB_EFLASH_LOADER_READBUF_SIZE
#define BFLB_EFLASH_LOADER_CMD_DATA_MAX_LEN (BFLB_EFLASH_LOADER_READBUF_SIZE - 0x04) //cmd+rsvd+len(2bytes)

#define MAXOF(a, b)          ((a) > (b) ? (a) : (b))
#define OFFSET(TYPE, MEMBER) ((uint32_t)(&(((TYPE *)0)->MEMBER)))




uint8_t bootrom_read_boot_mode(void);
void bflb_eflash_loader_init_uart_gpio(uint8_t eflash_loader_uart_pin_select);
void bflb_eflash_loader_deinit_uart_gpio_do(uint8_t eflash_loader_uart_pin_select);
void bflb_eflash_loader_deinit_uart_gpio(uint8_t eflash_loader_uart_pin_select);
void bflb_eflash_loader_init_flash_gpio(uint8_t flash_cfg);

#endif
