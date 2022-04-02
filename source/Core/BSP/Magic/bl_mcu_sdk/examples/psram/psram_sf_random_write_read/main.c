/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bflb_platform.h"
#include "bsp_sf_psram.h"
#include "bl702_l1c.h"
#include "bl702_sec_eng.h"

#include "stdlib.h"

uint32_t readVal = 0;
uint32_t expectedVal = 0;
uint8_t accessNum = 0;
uint8_t checkType = 0;
uint8_t psram_id[8] = { 0 };

BL_Err_Type ATTR_TCM_SECTION Psram_Random_Write_Read(void)
{
    uint32_t cnt = 0, i = 0;
    uint32_t tmp = 0;
    uint8_t data[32];
    uint32_t currentAddr = 0;
    uint8_t *p1 = (uint8_t *)BL702_PSRAM_XIP_BASE;
    uint8_t p1_val = 0;
    uint16_t *p2 = (uint16_t *)BL702_PSRAM_XIP_BASE;
    uint16_t p2_val = 0;
    uint32_t *p4 = (uint32_t *)BL702_PSRAM_XIP_BASE;
    uint32_t p4_val = 0;
    uint8_t access_type = 0;

    i = 0;
    cnt = 0;
    Sec_Eng_Trng_Enable();
    Sec_Eng_Trng_Read(data);
    tmp = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24);
    srand(tmp);
    *p1 = 0;
    *p2 = 0;
    *p4 = 0;
    access_type = 0;

    while (1) {
        i = rand();
        access_type = (i / PSRAM_SIZE) % 3;
        currentAddr = i % (PSRAM_SIZE - 128);
        currentAddr += 64;

        if (access_type == 0) {
            p1_val = i;
            *(p1 + currentAddr) = p1_val;
            *(p1 + currentAddr - 64) = p1_val + 1;
            *(p1 + currentAddr + 64) = p1_val + 2;
        } else if (access_type == 1) {
            p2_val = i;
            *(p2 + currentAddr / 2) = p2_val;
            *(p2 + (currentAddr - 64) / 2) = p2_val + 1;
            *(p2 + (currentAddr + 64) / 2) = p2_val + 2;
        } else if (access_type == 2) {
            p4_val = i;
            *(p4 + currentAddr / 4) = p4_val;
            *(p4 + (currentAddr - 64) / 4) = p4_val + 1;
            *(p4 + (currentAddr + 64) / 4) = p4_val + 2;
        }

        /* Check value */
        accessNum = access_type + 1;

        if (access_type == 0) {
            if (*(p1 + currentAddr) != (p1_val & 0xff)) {
                readVal = *(p1 + currentAddr);
                expectedVal = p1_val;
                checkType = 0;
                return ERROR;
            }

            if (*(p1 + currentAddr - 64) != ((p1_val + 1) & 0xff)) {
                readVal = *(p1 + currentAddr - 64);
                expectedVal = p1_val + 1;
                checkType = 1;
                return ERROR;
            }

            if (*(p1 + currentAddr + 64) != ((p1_val + 2) & 0xff)) {
                readVal = *(p1 + currentAddr + 64);
                expectedVal = p1_val + 2;
                checkType = 2;
                return ERROR;
            }
        } else if (access_type == 1) {
            if (*(p2 + currentAddr / 2) != (p2_val & 0xffff)) {
                readVal = *(p2 + currentAddr / 2);
                expectedVal = p2_val;
                checkType = 0;
                return ERROR;
            }

            if (*(p2 + (currentAddr - 64) / 2) != ((p2_val + 1) & 0xffff)) {
                readVal = *(p2 + (currentAddr - 64) / 2);
                expectedVal = p2_val + 1;
                checkType = 1;
                return ERROR;
            }

            if (*(p2 + (currentAddr + 64) / 2) != ((p2_val + 2) & 0xffff)) {
                readVal = *(p2 + (currentAddr + 64) / 2);
                expectedVal = p2_val + 2;
                checkType = 2;
                return ERROR;
            }
        } else if (access_type == 2) {
            if (*(p4 + currentAddr / 4) != p4_val) {
                readVal = *(p4 + currentAddr / 4);
                expectedVal = p4_val;
                checkType = 0;
                return ERROR;
            }

            if (*(p4 + (currentAddr - 64) / 4) != p4_val + 1) {
                readVal = *(p4 + (currentAddr - 64) / 4);
                expectedVal = p4_val + 1;
                checkType = 1;
                return ERROR;
            }

            if (*(p4 + (currentAddr + 64) / 4) != p4_val + 2) {
                readVal = *(p4 + (currentAddr + 64) / 4);
                expectedVal = p4_val + 2;
                checkType = 2;
                return ERROR;
            }
        }

        cnt++;

        if (cnt >= 1 * 1024 * 1024) {
            break;
        }
    }

    return SUCCESS;
}

int main(void)
{
    uint32_t hitCountLow, hitCountHigh;
    uint32_t missCount;
    BL_Err_Type stat;
    bflb_platform_init(0);

    MSG("PSRAM write read case \r\n");

    bsp_sf_psram_init(1);
    bsp_sf_psram_read_id(psram_id);
    MSG("BSP SF PSRAM ID: %02X %02X %02X %02X %02X %02X %02X %02X. \n",
        psram_id[0], psram_id[1], psram_id[2], psram_id[3], psram_id[4], psram_id[5], psram_id[6], psram_id[7]);

    while (1) {
        stat = Psram_Random_Write_Read();

        if (stat == ERROR) {
            BL_CASE_FAIL;
            while (1) {
            }
        }
        BL_CASE_SUCCESS;
        L1C_Cache_Hit_Count_Get(&hitCountLow, &hitCountHigh);
        missCount = L1C_Cache_Miss_Count_Get();
        MSG("Hit count low=%08x, high=%08x, Miss count=%08x\r\n", hitCountLow, hitCountHigh, missCount);
        MSG("System time%dms\r\n", bflb_platform_get_time_ms());
    }
}
