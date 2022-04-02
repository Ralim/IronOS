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
 * Note:
 *
 * This Case is currently based on BL706_AVB development board with ili9341 controller screen.
 * If you use other hardware platforms, please modify the pin Settings by yourself.
 *
 * This routine audio playback requires transcoding the audio into PCM encoding and exporting it as an audio array.
 * See fhm_onechannel_16k_20.h for audio array examples , and i2s/script for transcoding scripts.
 *
 * Your should Short connect func1 and func2 jump cap on 706_AVB Board,and Plug the Audio Codec Board(ES8388) whit speaker into the HD11 interface , and connect SD Card with SPI ports.
 *
 * You need to fill in the correct audio file path which in SD Card file systems in audio_init(), please refer to line 58 of this code, this version only support Engilsh path.
 *
 * This case use shell module to complete human-computer interaction.
 */
#include "bflb_platform.h"
#include "ff.h"
#include "fatfs_posix_api.h"
#include "wav_play_from_sd_card.h"

#ifdef SUPPORT_SHELL
#include "hal_uart.h"
#include "shell.h"
#endif

FATFS Fs_1;
audio_dev_t Audio_Dev;
void fatfs_sd_driver_register(void);

#ifdef SUPPORT_SHELL
void shell_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint8_t data;
    if (state == UART_EVENT_RX_FIFO) {
        for (size_t i = 0; i < size; i++) {
            data = *(uint8_t *)(args + i);
            shell_handler(data);
        }
    }
}
#endif

int main(void)
{
    FRESULT res;

    bflb_platform_init(0);

    fatfs_sd_driver_register();
    sd_wav_play_register(&Audio_Dev);

#ifdef SUPPORT_SHELL
    shell_init();
    struct device *uart = device_find("debug_log");
    if (uart) {
        device_set_callback(uart, shell_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT));
    }
#endif

    res = f_mount(&Fs_1, "sd:", 1);
    f_chdrive("sd:");
    f_chdir("./wav_test");

    if (res == FR_OK) {
        if (Audio_Dev.audio_init(&Audio_Dev, "sd:/wav_test/test9_44100_stereo_s32(24bit).wav") == 0) {
            MSG("Audio Init success\r\n");
        } else {
            MSG("Audio Init error\r\n");
        }
    } else {
        MSG("fatfs open ERROR\r\n");
    }

    Audio_Dev.audio_control(&Audio_Dev, AUDIO_CMD_PLAY_START, NULL);

    /* start/stop test */
    /*
    bflb_platform_delay_ms(10000);
    Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_PLAY_STOP,NULL);
    bflb_platform_delay_ms(5000);
    Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_PLAY_START,NULL);
    bflb_platform_delay_ms(5000);
    */

    /* volume test */
    /*
    for(uint32_t vol=0; vol<80; vol++)
    {
        Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_VOLUME,(void *)vol);
        bflb_platform_delay_ms(100);
    }
    for(uint32_t vol=80; vol>0; vol--)
    {
        Audio_Dev.audio_control(&Audio_Dev,AUDIO_CMD_VOLUME,(void *)vol);
        bflb_platform_delay_ms(100);
    }
    */

    Audio_Dev.audio_control(&Audio_Dev, AUDIO_CMD_VOLUME, (void *)30);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
