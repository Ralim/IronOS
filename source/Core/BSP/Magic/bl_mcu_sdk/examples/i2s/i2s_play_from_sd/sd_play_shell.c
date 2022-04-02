/**
 * @file fatfs_posix_api.c
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

#ifdef SUPPORT_SHELL

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ff.h>
#include "shell.h"
#include "bflb_platform.h"

#include "wav_play_from_sd_card.h"

#define BUF_CNT          128
#define MAX_PATH_LEN     255
#define MAX_FILENAME_LEN 128
#define MAX_INPUT_LEN    20

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

extern audio_dev_t Audio_Dev;

int cmd_play(size_t argc, char **argv)
{
    char path[SHELL_CONSOLEBUF_SIZE];
    FILINFO dirent;

    uint8_t flag = 0;
    uint32_t vol = 0;

    strcpy(path, ".");

    if (argc == 1) {
        MSG("缺少参数\r\n");
        MSG("play [-o path] [-s] [-p] [-v volume] \r\n");
    }

    for (uint8_t i = 1; i < argc; i++) {
        //cmd
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-s") == 0) {
                flag |= 0x01;
            } else if (strcmp(argv[i], "-p") == 0) {
                flag |= 0x02;
            } else if (strcmp(argv[i], "-o") == 0) {
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    strcpy(path, argv[i + 1]);
                    i++;
                }

                flag |= 0x04;
            } else if (strcmp(argv[i], "-v") == 0) {
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    vol = atoi(argv[i + 1]);
                    i++;
                }

                flag |= 0x08;
            } else {
                MSG("参数错误 %s\r\n", argv[i]);
                return -1;
            }
        } else {
            MSG("参数错误 %s\r\n", argv[i]);
            return -1;
        }
    }

    if (flag & 0x04) /* -o */
    {
        f_stat(path, &dirent);

        if (f_stat(path, &dirent) != FR_OK || (dirent.fattrib & AM_DIR)) {
            MSG("-o:无目标文件\r\n", path);
            return -1;
        }

        if (Audio_Dev.audio_init(&Audio_Dev, path) == 0) {
            MSG("open success\r\n");
        } else {
            MSG("open error\r\n");
        }
    }

    if (flag & 0x01) /* -s */
    {
        if (Audio_Dev.audio_control(&Audio_Dev, AUDIO_CMD_PLAY_START, NULL) != 0) {
            MSG("play start error\r\n");
        } else {
            MSG("play start \r\n");
        }
    }

    if (flag & 0x02) /* -p */
    {
        if (Audio_Dev.audio_control(&Audio_Dev, AUDIO_CMD_PLAY_STOP, NULL) != 0) {
            MSG("play stop error\r\n");
        } else {
            MSG("play stop \r\n");
        }
    }

    if (flag & 0x08) /* -v */
    {
        if (Audio_Dev.audio_control(&Audio_Dev, AUDIO_CMD_VOLUME, (void *)vol) != 0) {
            MSG("play set voice volume error\r\n");
        } else {
            MSG("set voice volume:%d \r\n", vol);
        }
    }

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_play, play, play music)
#endif
