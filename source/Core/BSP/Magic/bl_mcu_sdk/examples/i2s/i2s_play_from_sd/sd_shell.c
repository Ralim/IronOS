/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>

#include <ff.h>
#include "shell.h"
#include "bflb_platform.h"

#include "wav_play_form_sd_card.h"

#define BUF_CNT          128
#define MAX_PATH_LEN     255
#define MAX_FILENAME_LEN 128
#define MAX_INPUT_LEN    20

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

const char *fatfs_table[] = {
    "FR_OK：成功",                                           /* (0) Succeeded */
    "FR_DISK_ERR：底层硬件错误",                             /* (1) A hard error occurred in the low level disk I/O layer */
    "FR_INT_ERR：断言失败",                                  /* (2) Assertion failed */
    "FR_NOT_READY：物理驱动没有工作",                        /* (3) The physical drive cannot work */
    "FR_NO_FILE：文件不存在",                                /* (4) Could not find the file */
    "FR_NO_PATH：路径不存在",                                /* (5) Could not find the path */
    "FR_INVALID_NAME：无效文件名",                           /* (6) The path name format is invalid */
    "FR_DENIED：由于禁止访问或者目录已满访问被拒绝",         /* (7) Access denied due to prohibited access or directory full */
    "FR_EXIST：文件已经存在",                                /* (8) Access denied due to prohibited access */
    "FR_INVALID_OBJECT：文件或者目录对象无效",               /* (9) The file/directory object is invalid */
    "FR_WRITE_PROTECTED：物理驱动被写保护",                  /* (10) The physical drive is write protected */
    "FR_INVALID_DRIVE：逻辑驱动号无效",                      /* (11) The logical drive number is invalid */
    "FR_NOT_ENABLED：卷中无工作区",                          /* (12) The volume has no work area */
    "FR_NO_FILESYSTEM：没有有效的FAT卷",                     /* (13) There is no valid FAT volume */
    "FR_MKFS_ABORTED：由于参数错误f_mkfs()被终止",           /* (14) The f_mkfs() aborted due to any parameter error */
    "FR_TIMEOUT：在规定的时间内无法获得访问卷的许可",        /* (15) Could not get a grant to access the volume within defined period */
    "FR_LOCKED：由于文件共享策略操作被拒绝",                 /* (16) The operation is rejected according to the file sharing policy */
    "FR_NOT_ENOUGH_CORE：无法分配长文件名工作区",            /* (17) LFN working buffer could not be allocated */
    "FR_TOO_MANY_OPEN_FILES：当前打开的文件数大于_FS_SHARE", /* (18) Number of open files > _FS_SHARE */
    "FR_INVALID_PARAMETER：参数无效"                         /* (19) Given parameter is invalid */
};

/* Maintenance guarantees this begins with '/' and is NUL-terminated. */
static char path[MAX_PATH_LEN] = "sd:/";

static int cd(size_t argc, char **argv)
{
    int err;

    if (argc < 2) {
        strcpy(path, ".");
    } else {
        strcpy(path, argv[1]);
    }

    err = f_chdir(path);

    if (err) {
        MSG("%s %s\r\n", fatfs_table[err], path);
        return -1;
    }

    return 0;
}

static int ls(size_t argc, char **argv)
{
    FILINFO entry = { 0 };
    DIR dir;
    int err;
    uint8_t flag = 0;

    for (uint8_t i = 1; i < argc; i++) {
        //cmd
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-a") == 0) {
                flag |= 0x01;
            } else {
                MSG("参数错误 %s\r\n", argv[i]);
                return -1;
            }
        }
        //path
        else {
            if ((flag & 0x80) == 0) {
                strcpy(path, argv[i]);
                flag |= 0x80;
            } else {
                MSG("参数错误 %s\r\n", argv[i]);
                return -1;
            }
        }
    }

    if ((flag & 0x80) == 0) {
        strcpy(path, ".");
    }

    err = f_opendir(&dir, path);

    if (err) {
        MSG("%s %s\r\n", fatfs_table[err], path);
        return -1;
    }

    while (1) {
        err = f_readdir(&dir, &entry);

        if (err) {
            MSG("%s %s\r\n", fatfs_table[err], path);
            break;
        }

        /* Check for end of directory listing */
        if (entry.fname[0] == '\0') {
            break;
        }

        if ((entry.fattrib & AM_HID) && !(flag & 0x01)) {
            continue;
        }

        MSG("%s%s%s\r\n", entry.fname, (entry.fattrib & AM_DIR) ? "/" : "", (entry.fattrib & AM_HID) ? "(Hidden)" : "");
    }

    f_closedir(&dir);

    return 0;
}

static int pwd(size_t argc, char **argv)
{
    int err;
    err = f_getcwd(path, MAX_PATH_LEN);

    if (err) {
        MSG("%s\r\n", fatfs_table[err]);
        return -1;
    }

    MSG("%s\r\n", path);

    return 0;
}

static int truncc(size_t argc, char **argv)
{
    FIL file;
    int length = 0;
    int err;
    uint32_t temp;

    if (argc > 2) {
        strcpy(path, argv[1]);
        length = strtol(argv[2], NULL, 0);
    } else if (argc == 2) {
        strcpy(path, argv[1]);
        length = 0;
    } else if (argc < 2) {
        MSG("缺少参数\r\n");
        MSG("trunc <path> [length] \r\n");
        return -1;
    }

    err = f_open(&file, path, FA_READ | FA_WRITE);

    if (err) {
        MSG("%s %s\r\n", fatfs_table[err], path);
        return -1;
        ;
    }

    temp = f_tell(&file);
    f_lseek(&file, length);
    err = f_truncate(&file);
    f_lseek(&file, temp);

    if (err) {
        MSG("Failed to truncate %s (%d)\r\n", path, err);
        err = -1;
    }

    f_close(&file);

    return err;
}

static int mkdir(size_t argc, char **argv)
{
    int err;

    if (argc < 2) {
        strcpy(path, "new_dir");
    } else {
        strcpy(path, argv[1]);
    }

    err = f_mkdir(path);

    if (err) {
        MSG("%s %s\r\n", fatfs_table[err], path);
        err = -1;
    }

    return err;
}

static int rm(size_t argc, char **argv)
{
    int err;

    if (argc < 2) {
        MSG("缺少参数\r\n");
        MSG("rm <path> \r\n");
        return -1;
    } else {
        strcpy(path, argv[1]);
    }

    err = f_unlink(path);

    if (err) {
        MSG("%s %s\r\n", fatfs_table[err], path);
        err = -1;
    }

    return err;
}

/*
static int cmd_read( size_t argc, char **argv)
{
    FILINFO dirent;
    FIL file;
    int count;
    long int offset;
    int err;

    if(argc<2)
    {
        MSG("缺少参数\r\n");
        MSG("read <path> [count] [offset] \r\n");
        return -1;
    }

    strcpy(path, argv[1]);

    if (argc > 2) {
        count = strtol(argv[2], NULL, 0);
        if (count <= 0) {
            count = INT_MAX;
        }
    } else {
        count = INT_MAX;
    }

    if (argc > 3) {
        offset = strtol(argv[3], NULL, 0);
    } else {
        offset = 0;
    }

    err = f_stat(path, &dirent);
    if (err) {
        MSG("%s %s\r\n",fatfs_table[err], path);
        return -1;
    }

    if (dirent.fattrib & AM_DIR) {
        MSG( "无目标文件\r\n", path);
        return -1;
    }

    MSG( "File size: %d", dirent.fsize);

    err = f_open(&file, path,  FA_READ);
    if (err) {
        MSG("%s %s\r\n",fatfs_table[err], path);
        return -1;
    }

    if (offset > 0) {
        err = f_lseek(&file, offset);
        if (err) {
            MSG("%s %s\r\n",fatfs_table[err], path);
            f_close(&file);
            return -1;
        }
    }

    while (count > 0)
    {
        uint32_t read;
        uint8_t buf[16];
        int i;

        f_read(&file, buf, MIN(count, sizeof(buf)),&read);
        if (read <= 0) {
            break;
        }

        MSG( "%08X  ", offset);

        for (i = 0; i < read; i++) {
            MSG( "%02X ", buf[i]);
        }
        for (; i < sizeof(buf); i++) {
            MSG( "   ");
        }
        i = sizeof(buf) - i;
        MSG( "%*c", i*3, ' ');

        for (i = 0; i < read; i++) {
            MSG( "%c", buf[i] < 32 ||
                      buf[i] > 127 ? '.' : buf[i]);
        }

        MSG( "");

        offset += read;
        count -= read;
    }

    f_close(&file);

    return 0;
}

static int cmd_write( size_t argc, char **argv)
{
    uint8_t buf[BUF_CNT];
    uint8_t buf_len;
    uint32_t write_len;
    int arg_offset;
    FIL file;
    long int offset = -1;
    int err;

    //create_abs_path(argv[1], path, sizeof(path));

    if (!strcmp(argv[2], "-o")) {
        if (argc < 4) {
            MSG( "Missing argument");
            return -1;
        }

        offset = strtol(argv[3], NULL, 0);

        arg_offset = 4;
    } else {
        arg_offset = 2;
    }

    err = f_open(&file, path, FA_READ | FA_WRITE);
    if (err) {
        MSG( "Failed to open %s (%d)", path, err);
        return -1;
    }

    if (offset < 0) {
        err = f_lseek(&file, f_size(&file)); // set seek end
    } else {
        err = f_lseek(&file, offset);
    }
    if (err) {
        MSG( "Failed to seek %s (%d)", path, err);
        f_close(&file);
        return -1;
    }

    buf_len = 0U;
    while (arg_offset < argc) {
        buf[buf_len++] = strtol(argv[arg_offset++], NULL, 16);

        if ((buf_len == BUF_CNT) || (arg_offset == argc)) {
            err = f_write(&file, buf, buf_len, &write_len);
            if (err < 0) {
                MSG( "Failed to write %s (%d)",
                          path, err);
                f_close(&file);
                return -1;
            }

            buf_len = 0U;
        }
    }

    f_close(&file);

    return 0;
}
*/

extern audio_dev_t Audio_Dev;

int play(size_t argc, char **argv)
{
    FILINFO dirent;
    // FIL file;
    // DIR dp;
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
        if (Audio_Dev.audio_control(&Audio_Dev, AUDIO_CMD_START, NULL) != 0) {
            MSG("play start error\r\n");
        } else {
            MSG("play start \r\n");
        }
    }

    if (flag & 0x02) /* -p */
    {
        if (Audio_Dev.audio_control(&Audio_Dev, AUDIO_CMD_STOP, NULL) != 0) {
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
SHELL_CMD_EXPORT(cd, enter open directory)
SHELL_CMD_EXPORT(ls, list directory contents)
SHELL_CMD_EXPORT(pwd, get current path)
SHELL_CMD_EXPORT(truncc, truncate the file)
SHELL_CMD_EXPORT(mkdir, make new directory)
SHELL_CMD_EXPORT(rm, make new directory)
SHELL_CMD_EXPORT(play, play)
