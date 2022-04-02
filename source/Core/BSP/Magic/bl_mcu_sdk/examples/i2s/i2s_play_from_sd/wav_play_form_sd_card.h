/**
 * @file wav_play_form_sd_card.h
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

#ifndef __MAV_PLAY_H__
#define __MAV_PLAY_H__

#include "hal_i2s.h"

//WAV文件块定义

/* RIFF (RIFF WAVE Chunk) */
typedef struct __attribute__((packed)) {
    uint32_t chunk_id;   /*chunk id:"RIFF",(0X46464952) */
    uint32_t chunk_size; /*file size -8 */
    uint32_t format;     /* "WAVE" (0X45564157) */
} chunk_riff_t;

/* fmt (Format Chunk)*/
typedef struct __attribute__((packed)) {
    uint32_t chunk_id;        /* chunk id:"fmt",(0X20746D66) */
    uint32_t chunk_size;      /* fmt size -8  = 20*/
    uint16_t audio_format;    /* 音频格式;0X10,表示线性 PCM; 0X11 表示 IMA ADPCM */
    uint16_t num_of_channels; /* 通道数量;1,表示单声道;2,表示双声道*/
    uint32_t sample_rate;     /* 采样率;0X1F40,表示 8Khz */
    uint32_t byte_rate;       /* 字节速率 */
    uint16_t block_align;     /*块对齐(字节)*/
    uint16_t bits_per_sample; /*单个采样数据大小;4 位 ADPCM,设置为 4*/
} chunk_format_t;

//fact (Fact Chunk)
typedef struct __attribute__((packed)) {
    uint32_t chunk_id;       //chunk id;这里固定为"fact",即 0X74636166;
    uint32_t chunk_size;     //子集合大小(不包括 ID 和 Size);这里为:4.
    uint32_t data_fact_size; //数据转换为 PCM 格式后的大小
} chunk_fact_t;

//data (Data Chunk)
typedef struct __attribute__((packed)) {
    uint32_t chunk_id;   //chunk id;这里固定为"data",即 0X61746164
    uint32_t chunk_size; //子集合大小(不包括 ID 和 Size);文件大小-60.
} chunk_data_t;

//
typedef enum {
    CHUNK_RIFF,
    CHUNK_FORMAT,
    CHUNK_FACT,
    CHUNK_DATA,
} mav_chunk_t;

//.wav information
typedef struct
{
    int chunk_riff_offset; //在数据里的位置偏移，-1表示没有此块
    chunk_riff_t chunk_riff;
    int chunk_format_offset;
    chunk_format_t chunk_format;
    int chunk_fact_offset;
    chunk_fact_t chunk_fact;
    int chunk_data_offset;
    chunk_data_t chunk_data;
} wav_information_t;

typedef enum {
    AUDIO_CMD_START,
    AUDIO_CMD_STOP,
    AUDIO_CMD_VOLUME,
} AUDIO_CMD_t;

//播放控制块
typedef struct audio_dev {
    uint8_t *buff[2];
    uint32_t buff_data_size[2]; //buff内数据长度
    uint32_t buff_size_max;     //buff的大小
    uint8_t buff_using;         //正在使用的buff

    uint8_t audio_state; //状态
    uint8_t audio_type;  //类型

    int (*audio_init)(struct audio_dev *audio_dev, const TCHAR *path);
    int (*audio_control)(struct audio_dev *audio_dev, AUDIO_CMD_t cmd, void *args);

    int (*audio_callback)(struct audio_dev *audio_dev); //中断回调函数，用来重新装载buff，

    struct device *device;              //i2s的device，可以考虑换成指向其他外设
    wav_information_t *wav_information; //wav信息结构体，可以考虑换成一个共用体指向其他音乐格式数据
} audio_dev_t;

int sd_wav_play_register(audio_dev_t *audio_dev);

#endif