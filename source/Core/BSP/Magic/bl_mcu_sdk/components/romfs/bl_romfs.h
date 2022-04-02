/**
 * @file bl_romfs.h
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
 */

#ifndef __BL_ROMFS_H__
#define __BL_ROMFS_H__

#include "bflb_platform.h"

#define ROMFS_DEBUG(a, ...) //bflb_platform_printf(a, ##__VA_ARGS__)
#define ROMFS_WARN(a, ...)  bflb_platform_printf(a, ##__VA_ARGS__)
#define ROMFS_ERROR(a, ...) bflb_platform_printf(a, ##__VA_ARGS__)
#define ROMFS_ASSERT(EXPR)

#define ALIGNUP16(x) (((x) + 15) & ~15)

#define U32MK_HL(a, b, c, d) (((uint32_t)((a)&0xff) << 24) | \
                              ((uint32_t)((b)&0xff) << 16) | \
                              ((uint32_t)((c)&0xff) << 8) |  \
                              (uint32_t)((d)&0xff))
#define U32HTONL(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                     (((x) & (uint32_t)0x0000ff00UL) << 8) |  \
                     (((x) & (uint32_t)0x00ff0000UL) >> 8) |  \
                     (((x) & (uint32_t)0xff000000UL) >> 24))
#define U32NTOHL(x) U32HTONL(x)

#define ROMFS_MOUNTPOINT   "/romfs" /* must '/' */
#define ROMFS_MAX_NAME_LEN (64)

#define ROMFH_HRD    0
#define ROMFH_DIR    1
#define ROMFH_REG    2
#define ROMFH_UNKNOW 3

#define ROMFS_S_IFDIR 0x0040000
#define ROMFS_S_IFREG 0x0100000

/* romfs address,plase Change the address for your romfs-image programming*/
#define ROMFS_ROOT_ADDRESS (0x23000000 - 0x2000 + 0x80000)

struct bl_mtd_handle_t {
    char name[16];
    int id;
    unsigned int offset;
    unsigned int size;
    void *xip_addr;
};

typedef enum {
    ROMFS_SEEK_SET,
    ROMFS_SEEK_CUR,
    ROMFS_SEEK_END,
} romfs_whence_t;

typedef struct {
    uint32_t st_mode;
    uint32_t st_size;
} romfs_stat_t;

typedef struct {
    void *f_arg;   /* f_arg for file */
    size_t offset; /* offset for file */
    int fd;        /* file fd */
} romfs_file_t;

/* readdir 返回的路径结构体，被包含在 romfs_dir_t 中*/
typedef struct {
    int d_ino;      /* file number */
    uint8_t d_type; /* type of file */
    char d_name[ROMFS_MAX_NAME_LEN + 1];  /* file name */
} romfs_dirent_t;

/* opendir 得到的目录结构体 */
typedef struct {
    char *dir_start_addr;
    char *dir_end_addr;
    char *dir_cur_addr;
    romfs_dirent_t cur_dirent;
} romfs_dir_t;

int romfs_mount(void);
int romfs_open(romfs_file_t *fp, const char *path, int flags);
int romfs_close(romfs_file_t *fp);
int romfs_size(romfs_file_t *fp);
size_t romfs_read(romfs_file_t *fp, char *buf, size_t length);
size_t romfs_lseek(romfs_file_t *fp, int off, romfs_whence_t whence);
int romfs_stat(const char *path, romfs_stat_t *st);
int romfs_opendir(romfs_dir_t *dp,const char *path);
romfs_dirent_t *romfs_readdir(romfs_dir_t *dir);
int romfs_closedir(romfs_dir_t *dir);

#endif
