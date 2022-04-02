/**
 * @file bl_romfs.c
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

#include <string.h>
#include "drv_mmheap.h"
#include "bl_romfs.h"


static uint32_t romfs_endaddr(void);
static int dirent_type(void *addr);
static uint32_t romfs_endaddr(void);
static uint32_t dirent_hardfh(void *addr);
static uint32_t dirent_childaddr(void *addr);
static uint32_t dirent_size(void *addr);

static char *romfs_root = NULL; /* The mount point of the physical addr */

static int is_path_ch(char ch)
{
    if (((ch >= 'a') && (ch <= 'z')) ||
        ((ch >= 'A') && (ch <= 'Z')) ||
        ((ch >= '0') && (ch <= '9')) ||
        (ch == '/') ||
        (ch == '.') ||
        (ch == '_') ||
        (ch == '-')) {
        return 1;
    }
    return 0;
}

static int filter_format(const char *path, uint32_t size)
{
    int res;
    int i;
    int i_old;

    /* sure mountpoint */
    res = strncmp(path, ROMFS_MOUNTPOINT, strlen(ROMFS_MOUNTPOINT));
    if (res) {
        ROMFS_ERROR("ERROR: format is error.\r\n");
        return -1;
    }

    /* sure '/' format, sure every ch */
    for (i = 0; i < size; i++) {
        /* sure every ch */
        if (0 == is_path_ch(path[i])) {
            ROMFS_ERROR("ERROR: is_path_ch. i = %d\r\n", i);
            return -2;
        }

        /* sure '/' */
        if ('/' != path[i]) {
            continue;
        }
        if (i != 0) {
            if (i == i_old) {
                ROMFS_ERROR("ERROR: format error.\r\n");
                return -3;
            } else {
                i_old = i;
            }
        } else {
            i_old = i;
        }
    }

    return 0;
}

int romfs_mount(void)
{
    /* get romfs infomation */

    romfs_root = (void *)ROMFS_ROOT_ADDRESS;

    ROMFS_DEBUG("romfs: set romfs_addr:%8X\r\n", romfs_root);

    if (strncmp(romfs_root, "-rom1fs-", 8)) {
        ROMFS_ERROR("ERROR: no find romfs\r\n");
        return -1;
    } else {
        ROMFS_DEBUG("romfs: found romfs\r\n");
    }

    ROMFS_DEBUG("romfs: romfs size:%d*1024Byte\r\n", dirent_size(romfs_root) >> 10);

    return 0;
}

static int dirent_type(void *addr)
{
    if (0 == ((U32HTONL(*((uint32_t *)addr))) & 0x00000007)) {
        return ROMFH_HRD;
    } else if (2 == ((U32HTONL(*((uint32_t *)addr))) & 0x00000007)) {
        return ROMFH_REG;
    } else if (1 == ((U32HTONL(*((uint32_t *)addr))) & 0x00000007)) {
        return ROMFH_DIR;
    }

    return ROMFH_UNKNOW;
}

static uint32_t romfs_endaddr(void)
{
    return ((uint32_t)romfs_root + U32HTONL(*((uint32_t *)romfs_root + 2)));
}

static uint32_t dirent_hardfh(void *addr)
{
    return U32HTONL(*((uint32_t *)addr)) & 0xFFFFFFF0;
}

static uint32_t dirent_childaddr(void *addr)
{
    return U32HTONL(*((uint32_t *)addr + 1)) & 0xFFFFFFF0;
}

static uint32_t dirent_size(void *addr)
{
    return U32HTONL(*((uint32_t *)addr + 2));
}

static int file_info(char *path, char **p_addr_start_input, char **p_addr_end_input)
{
    char *addr_start = *p_addr_start_input;
    char *addr_end = *p_addr_end_input;
    ROMFS_ASSERT(path && addr_start_input && addr_end_input);

    ROMFS_DEBUG("romfs: file info path = %s\r\n", path);

    /* check arg */
    if (ROMFS_MAX_NAME_LEN < strlen(path)) {
        return -1;
    }

    /* /romfs */
    ROMFS_DEBUG("romfs: addr_start = %p\r\n", addr_start);
    if (addr_start == romfs_root) {
        addr_start = (char *)(romfs_root + ALIGNUP16(strlen(romfs_root + 16) + 1) + 16 + 64);
    }

    ROMFS_DEBUG("romfs: addr_start = %p, addr_end = %p, path = %s\r\n", addr_start, addr_end, path);
    while (1) {
        if (ROMFH_DIR == dirent_type(addr_start)) {
            if (0 == memcmp(path, addr_start + 16, strlen(path))) {
                if (addr_start[16 + strlen(path)] == 0) {
                    if (0 == dirent_hardfh(addr_start)) {
                        break; // the dir is the last dirent
                    }
                    addr_end = romfs_root + dirent_hardfh(addr_start);
                    ROMFS_DEBUG("romfs: update addr_end = %p\r\n", addr_end);
                    break;
                }
            }
        } else if (ROMFH_REG == dirent_type(addr_start)) {
            if (0 == memcmp(path, addr_start + 16, strlen(path))) {
                if (addr_start[16 + strlen(path)] == 0) {
                    addr_end = romfs_root + dirent_hardfh(addr_start);
                    break;
                }
            }
        } else if (ROMFH_HRD != dirent_type(addr_start)) {
            ROMFS_ERROR("ERROR: addr_start = %p, dirent_type(addr_start) = %d\r\n", addr_start, dirent_type(addr_start));
            // log_buf(addr_start, 8);
            ROMFS_ERROR("ERROR: unknow the dirent_type.\r\n");
            return -1;
        }

        ROMFS_DEBUG("romfs: addr_start = %p, off = 0x%08lx\r\n", addr_start, dirent_hardfh(addr_start));
        addr_start = romfs_root + dirent_hardfh(addr_start);
        if (addr_start >= addr_end) {
            ROMFS_WARN("WARN: start >= end, not found path = %s, addr_start = %p, addr_end = %p\r\n", path, addr_start, addr_end);
            return -1;
        }
    }

    ROMFS_DEBUG("romfs: update addr_start = %p, addr_end = %p\r\n", addr_start, addr_end);
    /* update out */
    *p_addr_start_input = addr_start;
    *p_addr_end_input = addr_end;
    return 0;
}

/*
 * input : path
 * output: p_addr_start_input, p_addr_end_input
 * return: 0 success, other error
 */
uint32_t dirent_file(char *path, void **p_addr_start_input, void **p_addr_end_input)
{
    char *addr_start;
    char *addr_end;

    char *p_name = NULL;
    char name[ROMFS_MAX_NAME_LEN + 1];
    char *p_ret = NULL;
    char need_enter_child = 0;

    ROMFS_ASSERT(path && addr_start_input && addr_end_input);

    /* check arg */
    if (strlen(path) < strlen(ROMFS_MOUNTPOINT)) {
        return -1;
    }

    ROMFS_DEBUG("romfs: dirent_file path = %s\r\n", path);

    /* rm root_mountpoint and'/', /romfs/ */
    if (0 != memcmp(path, ROMFS_MOUNTPOINT, strlen(ROMFS_MOUNTPOINT))) {
        ROMFS_ERROR("ERROR: not support path.\r\n");
        return -1;
    }
    p_name = path + strlen(ROMFS_MOUNTPOINT);
    if ((*p_name != '/') && (*p_name != '\0')) {
        ROMFS_ERROR("ERROR: not support path.\r\n");
        return -1;
    }
    if (*p_name == '/') {
        p_name += 1;
    }

    /* search every one */
    addr_start = romfs_root;
    addr_end = (char *)romfs_endaddr();
    ROMFS_DEBUG("romfs: romfs start_addr:%p, end_addr:%p, p_name = %s\r\n", addr_start, addr_end, p_name);

    while (1) {
        if (0 == *p_name) {
            break;
        }
        p_ret = strchr(p_name, '/');

        if (1 == need_enter_child) {
            if (addr_start == (romfs_root + dirent_childaddr(addr_start))) {
                return -2;
            }
            addr_start = romfs_root + dirent_childaddr(addr_start);
            need_enter_child = 0;
        }

        if (NULL == p_ret) {
            /* last name, use it find, update addr_start_end and return */
            ROMFS_DEBUG("romfs: last name.\r\n");
            if (strlen(p_name) > ROMFS_MAX_NAME_LEN) {
                ROMFS_ERROR("ERROR: name too long!\r\n");
                return -1;
            }
            if (0 != file_info(p_name, (char **)&addr_start, (char **)&addr_end)) {
                ROMFS_WARN("WARN: file info error, p_name = %s, addr_start = %p, addr_end = %p\r\n", p_name, addr_start, addr_end);
                return -1;
            }

            break;
        } else {
            memset(name, 0, sizeof(name));
            memcpy(name, p_name, (p_ret - p_name));
            ROMFS_DEBUG("romfs: mid name.\r\n");
            /* mid name, use it find, update addr_start_end and continue */
            if (0 != file_info(name, (char **)&addr_start, (char **)&addr_end)) {
                ROMFS_ERROR("ERROR: file info error.\r\n");
                return -1;
            }

            need_enter_child = 1;
            p_name = p_ret + 1;
            continue;
        }
    }

    ROMFS_DEBUG("romfs: dirent_file start = %p, end = %p\r\n", addr_start, addr_end);
    /* update out arg, and return */
    *p_addr_start_input = addr_start;
    *p_addr_end_input = addr_end;

    return 0;
}

int romfs_open(romfs_file_t *fp, const char *path, int flags)
{
    char *start_addr;
    char *end_addr;

    ROMFS_DEBUG("romfs: romfs open.\r\n");

    /* sure romfs_root is valid */
    if (romfs_root == NULL) {
        ROMFS_ERROR("ERROR: romfs_root is null.\r\n");
        return -1;
    }

    /* sure format is valid */
    if (0 != filter_format(path, strlen(path))) {
        ROMFS_ERROR("ERROR: path format is error.\r\n");
        return -1;
    }

    /* jump to the back of volume name, get addr_max */
    if (0 != dirent_file((char *)path, (void **)&start_addr, (void **)&end_addr)) {
        return -2;
    }

    fp->f_arg = start_addr;
    fp->offset = 0;

    return 0;
}

int romfs_close(romfs_file_t *fp)
{
    ROMFS_DEBUG("romfs: romfs close.\r\n");
    /* update romfs_file_t *fp */
    fp->f_arg = NULL;
    fp->offset = 0;
    return -1;
}

int romfs_size(romfs_file_t *fp)
{
    if(fp == NULL){
        return -1;
    }

    return dirent_size(fp->f_arg);
}

size_t romfs_read(romfs_file_t *fp, char *buf, size_t length)
{
    char *payload_buf;
    uint32_t payload_size;
    int len;

    /* init payload_buf and payload_size */
    payload_buf = ((char *)fp->f_arg) + ALIGNUP16(strlen(((char *)fp->f_arg) + 16) + 1) + 16;
    payload_size = dirent_size(fp->f_arg);

    /* check arg */
    if (fp->offset >= payload_size) {
        //ROMFS_WARN("WARN: offset >= payload_size\r\n");
        return 0;
    }

    /* memcpy data */
    if ((fp->offset + length) < payload_size) {
        len = length;
        memcpy(buf, payload_buf + fp->offset, len);
        fp->offset += len;
    } else {
        len = payload_size - fp->offset;
        memcpy(buf, payload_buf + fp->offset, len);
        fp->offset = payload_size;
    }

    return len;
}

// int romfs_ioctl(romfs_file_t *fp, int cmd, unsigned long arg)
// {
//     int ret = -1;
//     romfs_filebuf_t *file_buf = (romfs_filebuf_t *)arg;

//     if ((NULL == fp) || (NULL == file_buf)) {
//         return -2;
//     }
//     switch (cmd) {
//         case (IOCTL_ROMFS_GET_FILEBUF): {
//             ROMFS_DEBUG("romfs: IOCTL_ROMFS_GET_FILEBUF.\r\n");
//             file_buf->buf = ((char *)fp->f_arg) + ALIGNUP16(strlen(((char *)fp->f_arg) + 16) + 1) + 16;
//             file_buf->bufsize = dirent_size(fp->f_arg);
//             return 0;
//         } break;
//         default: {
//             ret = -3;
//         }
//     }

//     return ret;
// }

size_t romfs_lseek(romfs_file_t *fp, int off, romfs_whence_t whence)
{
    uint32_t payload_size;
    size_t tmp;

    if (NULL == fp) {
        return -1;
    }

    payload_size = dirent_size(fp->f_arg);

    if (whence == ROMFS_SEEK_SET) {
        if (off < 0) {
            ROMFS_ERROR("ERROR: not support whence.\r\n");
            return -2;
        }
        tmp = off;
    } else if (whence == ROMFS_SEEK_END) {
        if (off > 0) {
            ROMFS_ERROR("ERROR: not support whence.\r\n");
            return -3;
        }
        tmp = off + payload_size;
    } else if (whence == ROMFS_SEEK_CUR) {
        tmp = off + fp->offset;
    } else {
        ROMFS_ERROR("ERROR: not support whence.\r\n");
        return -4;
    }

    if ((tmp < 0) || (tmp > payload_size)) {
        ROMFS_ERROR("ERROR: not support whence.\r\n");
        return -5;
    }

    fp->offset = tmp;

    return fp->offset;
}

int romfs_stat(const char *path, romfs_stat_t *st)
{
    char *start_addr = 0;
    char *end_addr = 0;
    int res;

    ROMFS_DEBUG("romfs: romfs_stat path = %s\r\n", path);
    res = dirent_file((char *)path, (void **)&start_addr, (void **)&end_addr);

    if (res != 0) {
        ROMFS_WARN("WARN: dirent_file res = %d\r\n", res);
        return -1;
    }

    if (start_addr == romfs_root) {
        st->st_size = 0;
    } else {
        if (ROMFH_DIR == dirent_type(start_addr)) {
            st->st_size = 0;
            st->st_mode = ROMFS_S_IFDIR;
            ROMFS_DEBUG("romfs: st_size set 0");
        } else if (ROMFH_REG == dirent_type(start_addr)) {
            st->st_size = dirent_size(start_addr);
            ROMFS_DEBUG("romfs: st_size set %ld\r\n", st->st_size);
            st->st_mode = ROMFS_S_IFREG;
        } else {
            ROMFS_WARN("WARN: dirent_type err.\r\n");
            return -2;
        }
    }

    return 0;
}

int romfs_opendir(romfs_dir_t *dp,const char *path)
{
    char *start_addr;
    char *end_addr;
    int res;

    ROMFS_DEBUG("romfs: path = %s\r\n", path);

    /* sure romfs_root is valid */
    if (romfs_root == NULL) {
        ROMFS_ERROR("ERROR: romfs_root is null.\r\n");
        return -1;
    }

    // dp = (romfs_dir_t *)malloc(sizeof(romfs_dir_t) + ROMFS_MAX_NAME_LEN + 1);
    if (NULL == dp) {
        return -2;
    }
    memset(dp, 0, sizeof(romfs_dir_t));

    res = dirent_file((char *)path, (void **)&start_addr, (void **)&end_addr);
    ROMFS_DEBUG("romfs: open dir path = %s, start = %p, end = %p\r\n", path, start_addr, end_addr);
    if (0 == res) {
        /* need add update dir_addr and current_addr */
        if (start_addr == romfs_root) {
            dp->dir_start_addr = (char *)(romfs_root + ALIGNUP16(strlen(romfs_root + 16) + 1) + 16 + 64);
        } else {
            if (0 == dirent_childaddr(start_addr)) {
                return -3;
            } else {
                dp->dir_start_addr = (char *)(romfs_root + dirent_childaddr(start_addr));
            }
        }
        dp->dir_end_addr = end_addr;
        dp->dir_cur_addr = NULL;
        return 0;
    }

    /* open err */
    return -4;
}

romfs_dirent_t *romfs_readdir(romfs_dir_t *dir)
{
    if (!dir) {
        return NULL;
    }

    while (1) {
        /* current is NULL */
        if (NULL == dir->dir_cur_addr) {
            dir->dir_cur_addr = dir->dir_start_addr;
        } else {
            if (dir->dir_cur_addr >= dir->dir_end_addr) {
                return NULL;
            } else {
                while (1) {
                    if ((dir->dir_cur_addr >= dir->dir_end_addr) || (dir->dir_cur_addr < dir->dir_start_addr)) {
                        ROMFS_DEBUG("romfs: cur_addr = %p\r\n", dir->dir_cur_addr);
                        return NULL;
                    }
                    //ROMFS_ERROR("ERROR: cur_addr = %p\r\n", dir->dir_cur_addr);
                    if (0 == dirent_hardfh(dir->dir_cur_addr)) {
                        ROMFS_DEBUG("romfs: cur_addr = %p\r\n", dir->dir_cur_addr);
                        break;
                    }
                    ROMFS_DEBUG("romfs: cur_addr = %p\r\n", dir->dir_cur_addr);
                    if (NULL == dir->dir_cur_addr) {
                        return NULL;
                    }
                    if ((ROMFH_DIR == dirent_type(dir->dir_cur_addr)) ||
                        (ROMFH_REG == dirent_type(dir->dir_cur_addr))) {
                        ROMFS_DEBUG("romfs: cur_addr = %p\r\n", dir->dir_cur_addr);
                        break;
                    }
                    dir->dir_cur_addr = romfs_root + dirent_hardfh(dir->dir_cur_addr);
                }
            }
        }

        ROMFS_DEBUG("romfs: name = %s\r\n", (char *)(dir->dir_cur_addr + 16));
        strncpy(dir->cur_dirent.d_name, dir->dir_cur_addr + 16, ROMFS_MAX_NAME_LEN);
        dir->cur_dirent.d_name[ROMFS_MAX_NAME_LEN] = '\0';
        ROMFS_DEBUG("romfs: name = %s\r\n", dir->cur_dirent.d_name);

        if (0 == dirent_hardfh(dir->dir_cur_addr)) {
            dir->dir_cur_addr = dir->dir_end_addr;
        } else {
            dir->dir_cur_addr = romfs_root + dirent_hardfh(dir->dir_cur_addr);
        }

        /* rm . and .. dir */
        if (((dir->cur_dirent.d_name[0] == '.') && (dir->cur_dirent.d_name[1] == '.') && (dir->cur_dirent.d_name[2] == '\0')) ||
            ((dir->cur_dirent.d_name[0] == '.') && (dir->cur_dirent.d_name[1] == '\0'))) {
            ROMFS_DEBUG("romfs: ......name = %s\r\n", dir->cur_dirent.d_name);
            continue;
        } else {
            break;
        }
    }

    if(ROMFH_DIR == dirent_type(dir->dir_cur_addr)){
        dir->cur_dirent.d_type = ROMFH_DIR;
    }else if(ROMFH_REG == dirent_type(dir->dir_cur_addr)){
        dir->cur_dirent.d_type = ROMFH_REG;
    }

    return &(dir->cur_dirent);
}

int romfs_closedir(romfs_dir_t *dir)
{
    if (!dir) {
        return -1;
    }

    return 0;
}

// static const fs_ops_t romfs_ops = {
//     .open = &romfs_open,
//     .close = &romfs_close,
//     .read = &romfs_read,
//     .write = NULL,
//     .access = NULL,
//     .lseek = &romfs_lseek,
//     .stat = &romfs_stat,
//     .unlink = NULL,
//     .opendir = &romfs_opendir,
//     .readdir = &romfs_readdir,
//     .closedir = &romfs_closedir,
//     .telldir = NULL,
//     .seekdir = NULL,
//     .ioctl = &romfs_ioctl
// };
