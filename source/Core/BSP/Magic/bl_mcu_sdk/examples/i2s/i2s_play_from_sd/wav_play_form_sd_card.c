/**
 * @file wav_play_form_sd_card.c
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

#include "bsp_es8388.h"
#include "hal_i2s.h"
#include "hal_dma.h"
#include "ff.h"
#include "wav_play_form_sd_card.h"

static int wav_data_parser(uint8_t buff[1024], wav_information_t *wav_information);
static uint32_t pcm_24bit_to_32bit(uint8_t *buff, uint32_t data_size);
static int sd_wav_play_init(audio_dev_t *audio_dev, const TCHAR *path);
static int sd_wav_play_control(struct audio_dev *audio_dev, AUDIO_CMD_t cmd, void *args);
static int sd_wav_play_callback(audio_dev_t *audio_dev);

static FIL Wav_Fp;

static uint8_t Data_Buff0[4 * 1024] __attribute__((section(".system_ram"), aligned(4)));
static uint8_t Data_Buff1[4 * 1024] __attribute__((section(".system_ram"), aligned(4)));

static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_SLAVE,                    /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_3DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};

static wav_information_t Wav_Information;
static audio_dev_t *p_Audio_Dev = NULL;

static void dma_ch2_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (p_Audio_Dev && p_Audio_Dev->audio_callback) {
        p_Audio_Dev->audio_callback(p_Audio_Dev);
    }
}

/* get File pointer from top of file*/
static int wav_data_parser(uint8_t buff[1024], wav_information_t *wav_information)
{
    uint32_t offset = 0;
    uint32_t chunk_id;

    /* RIFF WAVE Chunk */
    chunk_id = ((chunk_riff_t *)&buff[offset])->chunk_id;

    if (chunk_id == 0x46464952) {
        wav_information->chunk_riff_offset = offset;
        wav_information->chunk_riff = *((chunk_riff_t *)&buff[offset]);
        offset += sizeof(chunk_riff_t);
    } else {
        wav_information->chunk_riff_offset = -1;
        return 1;
    }

    /* Format Chunk */
    chunk_id = ((chunk_format_t *)&buff[offset])->chunk_id;

    if (chunk_id == 0x20746D66 && offset < 1000) /* fmt */
    {
        wav_information->chunk_format_offset = offset;
        wav_information->chunk_format = *((chunk_format_t *)&buff[offset]);
        offset += ((chunk_format_t *)&buff[offset])->chunk_size + 8;
    } else {
        wav_information->chunk_format_offset = -1;
        return 1;
    }

    /* Fact/list Chunk */
    chunk_id = ((chunk_fact_t *)&buff[offset])->chunk_id;

    if ((chunk_id == 0X74636166 || chunk_id == 0X5453494C) && offset < 1000) /*fact or list*/
    {
        wav_information->chunk_fact_offset = offset;
        wav_information->chunk_fact = *((chunk_fact_t *)&buff[offset]);
        offset += ((chunk_fact_t *)&buff[offset])->chunk_size + 8;
    } else {
        wav_information->chunk_fact_offset = -1;
    }

    /* Data Chunk */
    chunk_id = ((chunk_data_t *)&buff[offset])->chunk_id;

    if (chunk_id == 0X61746164 && offset < 1000) {
        wav_information->chunk_data_offset = offset;
        wav_information->chunk_data = *((chunk_data_t *)&buff[offset]);
    } else {
        wav_information->chunk_data_offset = -1;
        return 1;
    }

    return 0;
}

static uint32_t pcm_24bit_to_32bit(uint8_t *buff, uint32_t data_size)
{
    /* buff大小应该在data_size的三分之四倍以上 */
    for (uint16_t i = data_size / 3; i > 0; i--) {
        buff[i * 4 - 1] = buff[i * 3 - 1];
        buff[i * 4 - 2] = buff[i * 3 - 2];
        buff[i * 4 - 3] = buff[i * 3 - 3];
        buff[i * 4 - 4] = 0;
    }

    return data_size / 3 * 4;
}

static int sd_wav_play_init(audio_dev_t *audio_dev, const TCHAR *path)
{
    UINT num;
    int res;
    uint8_t buff[1024];

    audio_dev->buff_using = 1;
    audio_dev->device = (struct device *)NULL;
    audio_dev->audio_state = 0;

    f_close(&Wav_Fp);
    res = f_open(&Wav_Fp, path, FA_READ);

    res |= f_lseek(&Wav_Fp, 0);
    res |= f_read(&Wav_Fp, buff, sizeof(buff), &num);

    if (res != FR_OK) {
        MSG("wav file open error\r\n");
        return 1;
    }

    /* Parse the WAV file */
    res = wav_data_parser(buff, &Wav_Information);

    if (res == 0) {
        audio_dev->wav_information = &Wav_Information;
        /*
        MSG("RIFF_offset:  %d\r\n", Wav_Information.chunk_riff_offset);
        MSG("format_offset:%d\r\n", Wav_Information.chunk_format_offset);
        MSG("fact_offset:  %d\r\n", Wav_Information.chunk_fact_offset);
        MSG("data_offset:  %d\r\n", Wav_Information.chunk_data_offset);
        */
    } else {
        MSG("wav file parse error\r\n");
        return 1;
    }

    audio_dev->device = device_find("I2S");

    if (audio_dev->device) {
        device_close(audio_dev->device);
    } else {
        i2s_register(I2S0_INDEX, "I2S");
        audio_dev->device = device_find("I2S");
    }

    struct device *dma_ch2 = device_find("i2s_ch2");

    if (dma_ch2) {
        device_close(dma_ch2);
    } else {
        dma_register(DMA0_CH2_INDEX, "i2s_ch2");
        dma_ch2 = device_find("i2s_ch2");
    }

    if ((audio_dev->device) && dma_ch2) {
        /* I2S Config */
        ((i2s_device_t *)(audio_dev->device))->interface_mode = I2S_MODE_LEFT;
        ((i2s_device_t *)(audio_dev->device))->sampl_freq_hz = audio_dev->wav_information->chunk_format.sample_rate;
        ((i2s_device_t *)(audio_dev->device))->channel_num = audio_dev->wav_information->chunk_format.num_of_channels;
        uint8_t pcm_w = audio_dev->wav_information->chunk_format.bits_per_sample / 8;

        if (pcm_w <= 2) {
            ((i2s_device_t *)(audio_dev->device))->frame_size = I2S_FRAME_LEN_16;
        } else {
            ((i2s_device_t *)(audio_dev->device))->frame_size = I2S_FRAME_LEN_32;
        }

        ((i2s_device_t *)(audio_dev->device))->data_size = ((i2s_device_t *)(audio_dev->device))->frame_size;
        ((i2s_device_t *)(audio_dev->device))->fifo_threshold = 8;
        res = device_open((audio_dev->device), DEVICE_OFLAG_DMA_TX);

        /* ES8388 Config */
        switch (((i2s_device_t *)(audio_dev->device))->data_size) {
            case I2S_FRAME_LEN_16:
                ES8388Cfg.data_width = ES8388_DATA_LEN_16;
                break;

            case I2S_FRAME_LEN_24:
                ES8388Cfg.data_width = ES8388_DATA_LEN_24;
                break;

            case I2S_FRAME_LEN_32:
                ES8388Cfg.data_width = ES8388_DATA_LEN_32;
                break;

            default:
                return 1;
                break;
        }

        ES8388_Init(&ES8388Cfg);
        ES8388_Set_Voice_Volume(20);

        MSG("sampl_freq_hz : %d\r\n", ((i2s_device_t *)(audio_dev->device))->sampl_freq_hz);
        MSG("channel_num   : %d\r\n", ((i2s_device_t *)(audio_dev->device))->channel_num);
        MSG("data_size     : %d\r\n", ((i2s_device_t *)(audio_dev->device))->data_size);

        /* DMA Config */
        ((dma_device_t *)dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        ((dma_device_t *)dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        ((dma_device_t *)dma_ch2)->src_req = (uint32_t)NULL;
        ((dma_device_t *)dma_ch2)->dst_req = DMA_REQUEST_I2S_TX;

        switch (((i2s_device_t *)(audio_dev->device))->data_size * ((i2s_device_t *)(audio_dev->device))->channel_num) {
            case I2S_DATA_LEN_8:
                ((dma_device_t *)dma_ch2)->src_width = DMA_TRANSFER_WIDTH_8BIT;
                ((dma_device_t *)dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
                break;

            case I2S_DATA_LEN_16:
                ((dma_device_t *)dma_ch2)->src_width = DMA_TRANSFER_WIDTH_16BIT;
                ((dma_device_t *)dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
                break;

            default:
                ((dma_device_t *)dma_ch2)->src_width = DMA_TRANSFER_WIDTH_32BIT;
                ((dma_device_t *)dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
                break;
        }

        device_open(dma_ch2, 0);
        device_set_callback(dma_ch2, dma_ch2_irq_callback);
        device_control(dma_ch2, DEVICE_CTRL_SET_INT, NULL);
        device_control((audio_dev->device), DEVICE_CTRL_I2S_ATTACH_TX_DMA, (void *)dma_ch2);

        /* Fill data frist*/
        f_lseek(&Wav_Fp, audio_dev->wav_information->chunk_data_offset + sizeof(chunk_data_t));

        if (audio_dev->wav_information->chunk_format.bits_per_sample / 8 == 3) {
            f_read(&Wav_Fp, audio_dev->buff[0], audio_dev->buff_size_max / 4 * 3, &num);
            audio_dev->buff_data_size[0] = pcm_24bit_to_32bit(audio_dev->buff[0], num);
            f_read(&Wav_Fp, audio_dev->buff[1], audio_dev->buff_size_max / 4 * 3, &num);
            audio_dev->buff_data_size[1] = pcm_24bit_to_32bit(audio_dev->buff[1], num);
        } else {
            f_read(&Wav_Fp, audio_dev->buff[0], audio_dev->buff_size_max, &num);
            audio_dev->buff_data_size[0] = num;
            f_read(&Wav_Fp, audio_dev->buff[1], audio_dev->buff_size_max, &num);
            audio_dev->buff_data_size[1] = num;
        }

        audio_dev->audio_state = 1;
    } else {
        return 1;
    }

    return 0;
}

static int sd_wav_play_control(struct audio_dev *audio_dev, AUDIO_CMD_t cmd, void *args)
{
    int res = -1;

    switch (cmd) {
        case AUDIO_CMD_START:
            if (audio_dev->audio_state) {
                res = device_write(audio_dev->device, 0, audio_dev->buff[!audio_dev->buff_using], audio_dev->buff_data_size[!audio_dev->buff_using]);
                audio_dev->audio_state = 2;
                res = 0;
            }

            break;

        case AUDIO_CMD_STOP:
            if (audio_dev->audio_state) {
                audio_dev->audio_state = 1;
                res = 0;
            }

            break;

        case AUDIO_CMD_VOLUME:
            if (audio_dev->audio_state) {
                res = ES8388_Set_Voice_Volume((uint32_t)args);
                res = 0;
            }

            break;

        default:
            break;
    }

    return res;
}

static int sd_wav_play_callback(audio_dev_t *audio_dev)
{
    uint32_t num = 0;
    uint8_t buff_using = audio_dev->buff_using;

    if (audio_dev->buff_data_size[!buff_using]) {
        /* start i2s-dma */
        if (audio_dev->audio_state == 2 && audio_dev->buff[!buff_using]) {
            device_write(audio_dev->device, 0, audio_dev->buff[!buff_using], audio_dev->buff_data_size[!buff_using]);
            audio_dev->buff_using = !buff_using;
        }

        /* fill data */
        if (audio_dev->wav_information->chunk_format.bits_per_sample / 8 == 3) {
            f_read(&Wav_Fp, audio_dev->buff[buff_using], audio_dev->buff_size_max / 4 * 3, &num);
            num = pcm_24bit_to_32bit(audio_dev->buff[buff_using], num);
        } else {
            f_read(&Wav_Fp, audio_dev->buff[buff_using], audio_dev->buff_size_max, &num);
        }

        audio_dev->buff_data_size[buff_using] = num;
    }

    /* play end, refill the buff for next time*/
    if (num == 0 && audio_dev->buff_data_size[!buff_using] == 0) {
        f_lseek(&Wav_Fp, audio_dev->wav_information->chunk_data_offset + sizeof(chunk_data_t));

        if (audio_dev->wav_information->chunk_format.bits_per_sample / 8 == 3) {
            f_read(&Wav_Fp, audio_dev->buff[0], audio_dev->buff_size_max / 4 * 3, &num);
            audio_dev->buff_data_size[0] = pcm_24bit_to_32bit(audio_dev->buff[0], num);
            f_read(&Wav_Fp, audio_dev->buff[1], audio_dev->buff_size_max / 4 * 3, &num);
            audio_dev->buff_data_size[1] = pcm_24bit_to_32bit(audio_dev->buff[1], num);
        } else {
            f_read(&Wav_Fp, audio_dev->buff[0], audio_dev->buff_size_max, &num);
            audio_dev->buff_data_size[0] = num;
            f_read(&Wav_Fp, audio_dev->buff[1], audio_dev->buff_size_max, &num);
            audio_dev->buff_data_size[1] = num;
        }

        audio_dev->audio_state = 1;
        MSG("End of the play\r\n");
    }

    return 0;
}

int sd_wav_play_register(audio_dev_t *audio_dev)
{
    p_Audio_Dev = audio_dev;
    audio_dev->audio_init = sd_wav_play_init,
    audio_dev->audio_control = sd_wav_play_control,
    audio_dev->audio_callback = sd_wav_play_callback,

    audio_dev->buff[0] = Data_Buff0;
    audio_dev->buff[1] = Data_Buff1;
    audio_dev->buff_data_size[0] = 0;
    audio_dev->buff_data_size[1] = 0;
    audio_dev->audio_state = 0;
    audio_dev->buff_size_max = sizeof(Data_Buff1);
    return 0;
}
