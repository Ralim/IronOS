/**
  ******************************************************************************
  * @file    blsp_port.h
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
#ifndef __BLSP_PORT_H__
#define __BLSP_PORT_H__

#include "stdint.h"
#include "misc.h"
#include "hal_flash.h"
#include "xz_config.h"



#define MFG_START_REQUEST_OFFSET                ((4 + 184) * 1024)
#define BLSP_BOOT2_XIP_BASE                     BL_FLASH_XIP_BASE
#define BLSP_BOOT2_ROLLBACK
#define BLSP_BOOT2_SUPPORT_DECOMPRESS           HAL_BOOT2_SUPPORT_DECOMPRESS
#define BLSP_BOOT2_SUPPORT_USB_IAP              HAL_BOOT2_SUPPORT_USB_IAP
#define BLSP_BOOT2_SUPPORT_EFLASH_LOADER_RAM    HAL_BOOT2_SUPPORT_EFLASH_LOADER_RAM     
#define BLSP_BOOT2_SUPPORT_EFLASH_LOADER_FLASH  HAL_BOOT2_SUPPORT_EFLASH_LOADER_FLASH   

#define BOOT2_MODE_RELEASE                      0x01
#define BOOT2_MODE_DEBUG                        0x02
#define BOOT2_MODE_DEEP_DEBUG                   0x04
#define BLSP_BOOT2_MODE                         BOOT2_MODE_DEBUG
#define BLSP_BOOT2_CPU_GROUP_MAX                HAL_BOOT2_CPU_GROUP_MAX
#define BLSP_BOOT2_CPU_MAX                      HAL_BOOT2_CPU_MAX
#define BLSP_BOOT2_RAM_IMG_COUNT_MAX            HAL_BOOT2_RAM_IMG_COUNT_MAX


uint32_t blsp_boot2_get_cpu_count(void);
uint8_t blsp_read_power_save_mode(void);
void blsp_boot2_pass_parameter(void *data, uint32_t len);
void blsp_boot2_releae_other_cpu(void);
void blsp_boot2_start_timer(void);
void blsp_boot2_show_timer(void);
void blsp_boot2_init_sec_eng_pka(void);


#endif /* __BLSP_PORT_H__ */



