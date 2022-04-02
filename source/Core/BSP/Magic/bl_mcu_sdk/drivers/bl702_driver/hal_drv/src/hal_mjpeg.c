/**
 * @file hal_mjpeg.c
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
#include "bl702_mjpeg.h"
#include "bl702_glb.h"
#include "hal_mjpeg.h"

void mjpeg_init(mjpeg_device_t *mjpeg_cfg)
{
    MJPEG_Packet_Type packetCfg = { 0 };

    MJPEG_CFG_Type mjpegCfg = {
        .burst = MJPEG_BURST_INCR16,
        .quality = mjpeg_cfg->quality,
        .yuv = mjpeg_cfg->yuv_format,
        .waitCount = 0x400,
        .bufferMjpeg = mjpeg_cfg->write_buffer_addr,
        .sizeMjpeg = mjpeg_cfg->write_buffer_size,
        .bufferCamYY = mjpeg_cfg->read_buffer_addr,
        .sizeCamYY = mjpeg_cfg->read_buffer_size,
        .bufferCamUV = 0,
        .sizeCamUV = 0,
        .resolutionX = mjpeg_cfg->resolution_x,
        .resolutionY = mjpeg_cfg->resolution_y,
        .bitOrderEnable = ENABLE,
        .evenOrderEnable = ENABLE,
        .swapModeEnable = DISABLE,
        .overStopEnable = ENABLE,
        .reflectDmy = DISABLE,
        .verticalDmy = DISABLE,
        .horizationalDmy = DISABLE,
    };

    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_MJPEG);

    uint32_t tmpVal;

    /* Disable mjpeg module */
    tmpVal = BL_RD_REG(MJPEG_BASE, MJPEG_CONTROL_1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, MJPEG_REG_MJPEG_ENABLE);
    BL_WR_REG(MJPEG_BASE, MJPEG_CONTROL_1, tmpVal);

    MJPEG_Init(&mjpegCfg);

    if (mjpeg_cfg->packet_cut_mode & MJPEG_PACKET_ADD_DEFAULT) {
        packetCfg.packetEnable = ENABLE;
        packetCfg.packetHead = mjpeg_cfg->packet_head_length;
        packetCfg.packetBody = mjpeg_cfg->packet_body_length;
        packetCfg.packetTail = mjpeg_cfg->packet_tail_length;
    }

    if (mjpeg_cfg->packet_cut_mode & MJPEG_PACKET_ADD_FRAME_HEAD) {
        packetCfg.frameHead = mjpeg_cfg->frame_head_length;
    }

    if (mjpeg_cfg->packet_cut_mode & MJPEG_PACKET_ADD_FRAME_TAIL) {
        packetCfg.frameTail = ENABLE;
    }

    if (mjpeg_cfg->packet_cut_mode & MJPEG_PACKET_ADD_END_TAIL) {
        packetCfg.endToTail = ENABLE;
    }

    MJPEG_Packet_Config(&packetCfg);

    if (mjpeg_cfg->yuv_format == MJPEG_YUV_FORMAT_YUV422_INTERLEAVE) {
        MJPEG_Set_YUYV_Order_Interleave(1, 0, 3, 2);
    }
}

void mjpeg_start(void)
{
    uint32_t tmpVal;

    /* Enable mjpeg module */
    tmpVal = BL_RD_REG(MJPEG_BASE, MJPEG_CONTROL_1);
    tmpVal = BL_SET_REG_BIT(tmpVal, MJPEG_REG_MJPEG_ENABLE);
    BL_WR_REG(MJPEG_BASE, MJPEG_CONTROL_1, tmpVal);
}

void mjpeg_stop(void)
{
    uint32_t tmpVal;

    /* Disable mjpeg module */
    tmpVal = BL_RD_REG(MJPEG_BASE, MJPEG_CONTROL_1);
    tmpVal = BL_CLR_REG_BIT(tmpVal, MJPEG_REG_MJPEG_ENABLE);
    BL_WR_REG(MJPEG_BASE, MJPEG_CONTROL_1, tmpVal);
}

uint8_t mjpeg_get_one_frame(uint8_t **pic, uint32_t *len, uint8_t *q)
{
    MJPEG_Frame_Info info;
    arch_memset(&info, 0, sizeof(info));

    MJPEG_Get_Frame_Info(&info);

    if (info.validFrames == 0) {
        return ERROR;
    }

    *pic = (uint8_t *)(info.curFrameAddr);
    *len = info.curFrameBytes;
    *q = info.curFrameQ;

    return SUCCESS;
}

void mjpeg_drop_one_frame(void)
{
    MJPEG_Pop_Frame();
}