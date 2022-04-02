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
#include "data_protocol.h"
#include "wav_play_from_isp.h"

extern isp_obj_t isp_obj_uart;

static int wav_data_parser(uint8_t *buff, uint16_t max_size, wav_information_t *wav_information);
static uint32_t pcm_24bit_to_32bit(uint8_t *buff, uint32_t data_size);
static int isp_wav_play_init(struct audio_dev *audio_dev, uint8_t mode, uint8_t *buff);
static int isp_wav_play_control(struct audio_dev *audio_dev, AUDIO_CMD_t cmd, void *args);
static int isp_wav_play_callback(audio_dev_t *audio_dev);

uint8_t audio_buff[2][4 * 1024] __attribute__((section(".system_ram"), aligned(4)));

static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_SLAVE,                    /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_9DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};

static wav_information_t Wav_Information;
static record_config_t record_config;
static audio_dev_t *p_Audio_Dev = NULL;

static void dma_ch2_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (p_Audio_Dev && p_Audio_Dev->audio_callback) {
        p_Audio_Dev->audio_callback(p_Audio_Dev);
    }
}

static void dma_ch3_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (p_Audio_Dev && p_Audio_Dev->audio_callback) {
        p_Audio_Dev->audio_callback(p_Audio_Dev);
    }
}

/* get File pointer from top of file*/
static int wav_data_parser(uint8_t *buff, uint16_t max_size, wav_information_t *wav_information)
{
    uint32_t offset = 0;
    uint32_t chunk_id;

    if (buff == NULL)
        return 1;

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

    if (chunk_id == 0x20746D66 && offset < max_size) /* fmt */
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

    if ((chunk_id == 0X74636166 || chunk_id == 0X5453494C) && offset < max_size) /*fact or list*/
    {
        wav_information->chunk_fact_offset = offset;
        wav_information->chunk_fact = *((chunk_fact_t *)&buff[offset]);
        offset += ((chunk_fact_t *)&buff[offset])->chunk_size + 8;
    } else {
        wav_information->chunk_fact_offset = -1;
    }

    /* Data Chunk */
    chunk_id = ((chunk_data_t *)&buff[offset])->chunk_id;

    if (chunk_id == 0X61746164 && offset < max_size) {
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

static int isp_wav_play_init(struct audio_dev *audio_dev, uint8_t mode, uint8_t *buff)
{
    int res;

    struct device *dma_ch2 = device_find("i2s_ch2");
    struct device *dma_ch3 = device_find("i2s_ch3");

    audio_dev->device = device_find("I2S");

    if (audio_dev->device) {
        device_close(audio_dev->device);
    } else {
        i2s_register(I2S0_INDEX, "I2S");
        audio_dev->device = device_find("I2S");
    }

    if (mode == 1) {
        goto record_conf;
    }

    /* play config */
    if (dma_ch2) {
        device_close(dma_ch2);
    } else {
        dma_register(DMA0_CH2_INDEX, "i2s_ch2");
        dma_ch2 = device_find("i2s_ch2");
    }

    /* Parse the WAV file */
    res = wav_data_parser(buff, 1000, &Wav_Information);

    if (!res) {
        audio_dev->wav_information = &Wav_Information;
    } else {
        /* MSG("wav file parse error\r\n"); */
        return 1;
    }

    if ((audio_dev->device) && dma_ch2) {
        /* I2S Config */
        I2S_DEV(audio_dev->device)->interface_mode = I2S_MODE_LEFT;
        I2S_DEV(audio_dev->device)->sampl_freq_hz = audio_dev->wav_information->chunk_format.sample_rate;
        I2S_DEV(audio_dev->device)->channel_num = audio_dev->wav_information->chunk_format.num_of_channels;
        uint8_t pcm_w = audio_dev->wav_information->chunk_format.bits_per_sample / 8;

        if (pcm_w <= 2) {
            I2S_DEV(audio_dev->device)->frame_size = I2S_FRAME_LEN_16;
        } else {
            I2S_DEV(audio_dev->device)->frame_size = I2S_FRAME_LEN_32;
        }

        I2S_DEV(audio_dev->device)->data_size = I2S_DEV(audio_dev->device)->frame_size;
        I2S_DEV(audio_dev->device)->fifo_threshold = 3;
        res = device_open((audio_dev->device), DEVICE_OFLAG_DMA_TX);

        /* ES8388 Config */
        switch (I2S_DEV(audio_dev->device)->data_size) {
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
        ES8388_Set_Voice_Volume(50);

        /* DMA Config */
        DMA_DEV(dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch2)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2)->dst_req = DMA_REQUEST_I2S_TX;
        DMA_DEV(dma_ch2)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        switch (I2S_DEV(audio_dev->device)->data_size * I2S_DEV(audio_dev->device)->channel_num) {
            case 1:
                DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_8BIT;
                DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
                break;

            case 2:
                DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_16BIT;
                DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
                break;

            default:
                DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_32BIT;
                DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
                break;
        }

        device_open(dma_ch2, 0);
        device_set_callback(dma_ch2, dma_ch2_irq_callback);
        device_control(dma_ch2, DEVICE_CTRL_SET_INT, NULL);
        device_control((audio_dev->device), DEVICE_CTRL_ATTACH_TX_DMA, (void *)dma_ch2);

        /* Delete the information data*/
        uint32_t data_size = audio_dev->buff_data_size[!audio_dev->buff_using];
        data_size = data_size - audio_dev->wav_information->chunk_data_offset;
        audio_dev->buff_data_size[!audio_dev->buff_using] = data_size;

        uint8_t *p_dst = audio_dev->buff[!audio_dev->buff_using];
        uint8_t *p_src = &(audio_dev->buff[!audio_dev->buff_using][audio_dev->wav_information->chunk_data_offset]);

        memcpy(p_dst, p_src, data_size);

        if (audio_dev->wav_information->chunk_format.bits_per_sample / 8 == 3) {
            audio_dev->buff_data_size[!audio_dev->buff_using] = pcm_24bit_to_32bit(audio_dev->buff[!audio_dev->buff_using], data_size);

        } else {
        }

    } else {
        return 1;
    }

    return 0;

/* record config */
record_conf:

    if (dma_ch3) {
        device_close(dma_ch3);
    } else {
        dma_register(DMA0_CH3_INDEX, "i2s_ch3");
        dma_ch3 = device_find("i2s_ch3");
    }

    if ((audio_dev->device) && dma_ch3) {
        I2S_DEV(audio_dev->device)->iis_mode = I2S_MODE_MASTER;
        I2S_DEV(audio_dev->device)->interface_mode = I2S_MODE_LEFT;
        I2S_DEV(audio_dev->device)->channel_num = audio_dev->record_config->channel_num;
        I2S_DEV(audio_dev->device)->data_size = audio_dev->record_config->data_size;

        if (I2S_DEV(audio_dev->device)->data_size <= 2) {
            I2S_DEV(audio_dev->device)->frame_size = I2S_FRAME_LEN_16;
        } else {
            I2S_DEV(audio_dev->device)->frame_size = I2S_FRAME_LEN_32;
        }
        I2S_DEV(audio_dev->device)->fifo_threshold = 4;

        switch (audio_dev->record_config->sampl_freq) {
            case SAMPL_FREQ_8000:
                I2S_DEV(audio_dev->device)->sampl_freq_hz = 8000;
                break;
            case SAMPL_FREQ_16000:
                I2S_DEV(audio_dev->device)->sampl_freq_hz = 16000;
                break;
            // case SAMPL_FREQ_32000:
            //     I2S_DEV(audio_dev->device)->sampl_freq_hz = 32000;
            //     break;
            case SAMPL_FREQ_44100:
                I2S_DEV(audio_dev->device)->sampl_freq_hz = 44100;
                break;
            case SAMPL_FREQ_48000:
                I2S_DEV(audio_dev->device)->sampl_freq_hz = 48000;
                break;
            default:
                break;
        }
        device_open(audio_dev->device, DEVICE_OFLAG_DMA_RX);

        /* ES8388 Config */
        switch (I2S_DEV(audio_dev->device)->data_size) {
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

        /* DMA Config */
        DMA_DEV(dma_ch3)->direction = DMA_PERIPH_TO_MEMORY;
        DMA_DEV(dma_ch3)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch3)->src_req = DMA_REQUEST_I2S_RX;
        DMA_DEV(dma_ch3)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch3)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch3)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch3)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch3)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        switch (I2S_DEV(audio_dev->device)->data_size * I2S_DEV(audio_dev->device)->channel_num) {
            case 1:
                DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_8BIT;
                DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
                break;

            case 2:
                DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_16BIT;
                DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
                break;

            default:
                DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_32BIT;
                DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
                break;
        }

        device_open(dma_ch3, 0);
        device_set_callback(dma_ch3, dma_ch3_irq_callback);
        device_control(dma_ch3, DEVICE_CTRL_SET_INT, NULL);
        device_control((audio_dev->device), DEVICE_CTRL_ATTACH_RX_DMA, (void *)dma_ch3);
    }
    return 0;
}

static int isp_wav_play_control(struct audio_dev *audio_dev, AUDIO_CMD_t cmd, void *args)
{
    static uint32_t volume = 50;
    int res = -1;

    switch (cmd) {
        case AUDIO_CMD_PLAY_START:
            if (audio_dev->audio_state) {
                audio_dev->buff_using = !audio_dev->buff_using;
                res = device_write(audio_dev->device, 0, audio_dev->buff[audio_dev->buff_using], audio_dev->buff_data_size[audio_dev->buff_using]);
                audio_dev->audio_state = 4;
                res = 0;
            }
            break;

        case AUDIO_CMD_PLAY_STOP:
            if (audio_dev->audio_state) {
                audio_dev->audio_state = 1;
                res = 0;
            }
            break;

        case AUDIO_CMD_RECORD_START:
            if (audio_dev->audio_state >= 10) {
                audio_dev->buff_using = !audio_dev->buff_using;
                res = device_read(audio_dev->device, 0, audio_dev->buff[audio_dev->buff_using], audio_dev->buff_size_max);
                audio_dev->audio_state = 12;
            }

            break;

        case AUDIO_CMD_RECORD_STOP:
            if (audio_dev->audio_state > 10) {
                audio_dev->audio_state = 10;
                res = 0;
            }
            break;

        case AUDIO_CMD_SET_VOLUME:
            if (audio_dev->audio_state) {
                res = ES8388_Set_Voice_Volume((uint32_t)args);
                res = 0;
                volume = (uint32_t)args;
            }
            break;

        case AUDIO_CMD_GET_VOLUME:
            if (audio_dev->audio_state) {
                // res = ES8388_Set_Voice_Volume((uint32_t)args);
                res = (int)volume;
            }
            break;

        default:
            break;
    }

    return res;
}

static int isp_wav_play_callback(audio_dev_t *audio_dev)
{
    if (audio_dev->audio_state < 10) { /* play*/
        if (audio_dev->buff_data_size[!audio_dev->buff_using] > 0 && isp_obj_uart.status.isp_state_mode == RECEIVE_WAIT_ACK) {
            /* data ready, switch buff */
            audio_dev->buff_using = !audio_dev->buff_using;
            device_write(audio_dev->device, 0, audio_dev->buff[audio_dev->buff_using], audio_dev->buff_data_size[audio_dev->buff_using]);
            audio_dev->buff_data_size[audio_dev->buff_using] = 0;

            audio_dev->buff_data_size[!audio_dev->buff_using] = isp_obj_uart.length;
            isp_obj_uart.file_data = audio_dev->buff[!audio_dev->buff_using];

            isp_obj_uart.status.isp_state_mode = NO_TASK;
            isp_uart_send_ack(&isp_obj_uart, REPLY_SUCCES);

        } else {
            /* Data not ready, wait data receive*/
            audio_dev->audio_state = 3;
        }

    } else { /* record */
        audio_dev->buff_data_size[audio_dev->buff_using] = audio_dev->buff_size_max;

        if (audio_dev->audio_state == 12) {
            if (isp_obj_uart.status.isp_state_mode == NO_TASK) {
                /* Continue to the recording */
                audio_dev->buff_using = !audio_dev->buff_using;
                device_read(audio_dev->device, 0, audio_dev->buff[audio_dev->buff_using], audio_dev->buff_size_max);
                /* send data to PC */
                isp_obj_uart.file_data = audio_dev->buff[!audio_dev->buff_using];
                isp_obj_uart.length = audio_dev->buff_data_size[!audio_dev->buff_using];
                isp_obj_uart.cmd_id = FILE_DATA;
                isp_obj_uart.file_type = 0x58;
                isp_uart_send_data(&isp_obj_uart);
                audio_dev->buff_data_size[!audio_dev->buff_using] = 0;
            } else {
                /* Data not ready, wait data transmit*/
                audio_dev->audio_state = 11;
            }
        }
    }
    return 0;
}

int isp_wav_play_register(audio_dev_t *audio_dev)
{
    p_Audio_Dev = audio_dev;
    audio_dev->audio_init = isp_wav_play_init,
    audio_dev->audio_control = isp_wav_play_control,
    audio_dev->audio_callback = isp_wav_play_callback,

    audio_dev->buff[0] = audio_buff[0];
    audio_dev->buff[1] = audio_buff[1];
    audio_dev->buff_data_size[0] = 0;
    audio_dev->buff_data_size[1] = 0;
    audio_dev->audio_state = 0;
    audio_dev->buff_size_max = sizeof(audio_buff) / 2;

    record_config.data_size = 2;
    record_config.channel_num = 1;
    record_config.sampl_freq = SAMPL_FREQ_16000;
    audio_dev->record_config = &record_config;
    return 0;
}
