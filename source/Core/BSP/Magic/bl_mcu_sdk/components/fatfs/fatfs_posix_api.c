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

/**
 * @brief POSIX Common File System Interface,
 *        Based on the fatfs.
 *        Only part of the interface is implemented
 */

#include "fatfs_posix_api.h"
#include "ff.h"
#include "stdlib.h"
#include "string.h"

/*Memory request interface*/
static void *(*vfs_malloc)(size_t size) = malloc; //默认的内存申请接口
static void (*vfs_free)(void *ptr) = free;        //默认的内存释放接口

/**
 * @brief fs init
 *
 * @return  0 on successful, -1 on failed.
 */
int ff_vfs_init(void)
{
    return 0;
}

/**
 * @brief FATF error conversion,Used for compatible Linux system error codes
 *
 * @param Status
 * @return int
 */
int ff_vfs_error(FRESULT Status)
{
    switch (Status) {
        case FR_OK:
            return 0;
        case FR_DISK_ERR:
            return -EIO; /* (1) A hard error occurred in the low level disk I/O layer */
        case FR_INT_ERR:
            return -EPIPE; /* (2) Assertion failed */
        case FR_NOT_READY:
            return -EIO; /* (3) The physical drive cannot work */
        case FR_NO_FILE:
            return -ENOENT; /* (4) Could not find the file */
        case FR_NO_PATH:
            return -ENOENT; /* (5) Could not find the path */
        case FR_INVALID_NAME:
            return -ENOEXEC; /* (6) The path name format is invalid */
        case FR_DENIED:
            return -ENOSPC; /* (7) Access denied due to prohibited access or directory full */
        case FR_EXIST:
            return -EACCES; /* (8) Access denied due to prohibited access */
        case FR_INVALID_OBJECT:
            return -ENXIO; /* (9) The file/directory object is invalid */
        case FR_WRITE_PROTECTED:
            return -EROFS; /* (10) The physical drive is write protected */
        case FR_INVALID_DRIVE:
            return -ENXIO; /* (11) The logical drive number is invalid */
        case FR_NOT_ENABLED:
            return -ENXIO; /* (12) The volume has no work area */
        case FR_NO_FILESYSTEM:
            return -EPERM; /* (13) There is no valid FAT volume */
        case FR_MKFS_ABORTED:
            return -EPERM; /* (14) The f_mkfs() aborted due to any problem */
        case FR_TIMEOUT:
            return -EBUSY; /* (15) Could not get a grant to access the volume within defined period */
        case FR_LOCKED:
            return -EACCES; /* (16) The operation is rejected according to the file sharing policy */
        case FR_NOT_ENOUGH_CORE:
            return -ENOMEM; /* (17) LFN working buffer could not be allocated */
        case FR_TOO_MANY_OPEN_FILES:
            return -EMFILE; /* (18) Number of open files > FF_FS_LOCK */
        case FR_INVALID_PARAMETER:
            return -EINVAL; /* (19) Given parameter is invalid */
        default:
            return -EPERM;
    }
}

/**
 * @brief Open the file described in PATH
 *
 * @param path The file path
 * @param flags The file's parameters:
 *          O_RDONLY:   read-only
 *          O_WRONLY:   write-only
 *          O_RDWR:     readable and writable
 *          O_CREAT:    Create a file when it doesn't exist
 *          O_EXCL:     When the file is opened, returned -EEXIST if the file exists and the flag bit O_CREAT is also specified.
 *                      If the file does not exist, the file is created
 *          O_TRUNC:    When opening the file, if the length of the file is not 0, the length of the file will be truncated to 0,
 *                      and the next time the file is written, the data will be written from the file
 *          O_APPEND:   After opening the file, set the write point of the file to the end of the file. The next time the file is written,
 *                      the newly written data will be appended to the end of the file
 * @param mode The permissions, not currently supported, ignored
 * @return int Positive: File descriptor (actually a pointer); Negative: Error execution, return error code
 */
int ff_open(const char *path, int flags, int mode)
{
    uint8_t OpenMode = 0;
    FIL *pFile;
    FRESULT Status;

    pFile = (FIL *)vfs_malloc(sizeof(FIL));
    if (pFile == NULL) {
        return -ENOMEM;
    }

    if (flags & O_WRONLY) {
        OpenMode |= FA_WRITE;
    }
    if (flags & O_RDWR) {
        OpenMode |= FA_WRITE | FA_READ;
    }
    if ((OpenMode & (FA_WRITE | FA_READ)) == 0) /*If no read-write is specified, the default is read-only*/
    {
        OpenMode |= FA_READ;
    }
    if ((flags & O_CREAT) && (flags & O_EXCL)) {
        OpenMode |= FA_CREATE_NEW;
    } else if (flags & O_CREAT) {
        OpenMode |= FA_OPEN_ALWAYS;
    }
    if (flags & O_TRUNC) {
        OpenMode |= FA_CREATE_ALWAYS;
    }
    if (flags & O_APPEND) {
        OpenMode |= FA_OPEN_APPEND;
    }

    Status = f_open(pFile, path, OpenMode);
    if (Status != FR_OK) {
        vfs_free(pFile);
        pFile = NULL;

        if ((flags & O_CREAT) && (flags & O_EXCL) && (Status == FR_EXIST)) {
            return -EEXIST;
        }
        return ff_vfs_error(Status);
    }

    return (int)pFile;
}

/**
 * @brief Close the file
 *
 * @param fd The descriptor of the file
 * @return int 0: success; Negative: Error execution, return error code
 */
int ff_close(int fd)
{
    FRESULT Status;

    if (fd <= 0)
        return -ENXIO;
    Status = f_close((FIL *)fd);
    if (Status == FR_OK) {
        vfs_free((void *)fd);
    }

    return ff_vfs_error(Status);
}

/**
 * @brief Read file data
 *
 * @param fd    The descriptor of the file
 * @param buf   The cache of data read by the file
 * @param nbytes Number of bytes to read
 * @return int  Read successfully, return the number of bytes read;
 *              Returns 0 if the file offset reached the end of the file when it was read.
 *              Other error conditions, return the error code
 */
int ff_read(int fd, void *buf, size_t nbytes)
{
    FRESULT Status;
    UINT cnt;

    if (fd <= 0)
        return -ENXIO;
    Status = f_read((FIL *)fd, buf, nbytes, (UINT *)&cnt);
    if (Status != FR_OK) {
        return ff_vfs_error(Status);
    }
    return cnt;
}

/**
 * @brief Writes data to a file
 *
 * @param fd    The descriptor of the file
 * @param buf   The cache of data to write to the file
 * @param nbytes The length of the file written
 * @return int  When successfully written, the number of bytes written to the file is returned.
 *              The actual number of bytes written may be less than nbytes (insufficient file space).
 *              Returns an error code (negative) when writing fails
 */
int ff_write(int fd, const void *buf, size_t nbytes)
{
    FRESULT Status;
    UINT cnt;

    if (fd <= 0)
        return -ENXIO;
    Status = f_write((FIL *)fd, buf, nbytes, (UINT *)&cnt);
    if (Status != FR_OK) {
        return ff_vfs_error(Status);
    }
    return cnt;
}

/**
 * @brief Resets the offset of the open file
 *
 * @param fd        The descriptor of the file
 * @param offset    Offset from the third parameter whence
 * @param whence    File offset basics
 *                  VFS_SEEK_SET: Based on file headers
 *                  VFS_SEEK_CUR: Based on the current location
 *                  VFS_SEEK_END: Based on file end
 * @return ff_off_t    Non-negative: on success, returns the number of offset bytes relative to the start of the file;
 *                  Negative: Failure, return error code
 */
ff_off_t ff_lseek(int fd, ff_off_t offset, int whence)
{
    FRESULT Status;
    long long temp;

    if (fd <= 0)
        return -ENXIO;

    switch (whence) {
        case VFS_SEEK_SET: {
            Status = f_lseek((FIL *)fd, offset);
            if (Status == FR_OK) {
                return (ff_off_t)((FIL *)fd)->fptr;
            }
            return ff_vfs_error(Status);
        }
        case VFS_SEEK_CUR: {
            temp = (ff_off_t)((FIL *)fd)->fptr;
            temp += offset;
            if (temp >= 0xffffffff)
                temp = 0xffffffff - 1;
            Status = f_lseek((FIL *)fd, temp);
            if (Status == FR_OK) {
                return (ff_off_t)((FIL *)fd)->fptr;
            }
            return ff_vfs_error(Status);
        }
        case VFS_SEEK_END: {
            temp = (ff_off_t)((FIL *)fd)->obj.objsize;
            temp -= offset;
            if (temp <= 0)
                temp = 0;
            Status = f_lseek((FIL *)fd, temp);
            if (Status == FR_OK) {
                return (ff_off_t)((FIL *)fd)->fptr;
            }
            return ff_vfs_error(Status);
        }
        default:
            return -EINVAL;
    }
}

/**
 * @brief Update the cache to memory
 *
 * @param fd    The descriptor of the file
 * @return int  0: success;
 *              Negative: Data synchronization failure, return error code
 */
int ff_sync(int fd)
{
    FRESULT Status;

    if (fd <= 0)
        return -ENXIO;           //文件指针为空,直接返回
    Status = f_sync((FIL *)fd);  //存储文件
    return ff_vfs_error(Status); //错误代码转义
}

/**
 * @brief Get the attributes of the file(path)
 *
 * @param path  The file path
 * @param st    Pointer to file status information structure
 * @return int  0: success;
 *              Negative: Data synchronization failure, return error code
 */
int ff_stat(const char *path, struct stat *st)
{
    FRESULT Status;
    FILINFO *pInfo;

    if (path == NULL || st == NULL) {
        return -EINVAL;
    }
    pInfo = vfs_malloc(sizeof(FILINFO));
    if (pInfo == NULL) {
        return -ENOMEM;
    }
    Status = f_stat(path, pInfo);
    if (Status == FR_OK) {
        st->st_size = pInfo->fsize;
        st->st_mode = pInfo->fattrib;
        st->st_actime = 0;
        st->st_modtime = 0;
    }
    vfs_free(pInfo);
    return ff_vfs_error(Status);
}

/**
 * @brief Get the property information of the file
 *
 * @param fd    The descriptor of the file
 * @param st    Pointer to file status information structure
 * @return int  0: success;
 *              Negative: Data synchronization failure, return error code
 */
int ff_fstat(int fd, struct stat *st)
{
    if (fd <= 0)
        return -ENXIO;
    if (st == NULL)
        return -EINVAL;

    st->st_size = ((FIL *)fd)->obj.objsize;
    st->st_mode = 0777;
    st->st_actime = 0;
    st->st_modtime = 0;

    return 0;
}

/**
 * @brief Checks whether the current program has access to PATH
 *
 * @param path  The descriptor of the file
 * @param amode The permissions, not currently supported, ignored
 * @return int  0: success;
 *              Negative: Data synchronization failure, return error code
 */
int ff_access(const char *path, int amode)
{
    struct stat st;

    return ff_stat(path, &st);
}

/**
 * @brief Retutn the current working directory,
 *        If the buf parameter is not empty, the current working directory string is also copied into the buf
 *
 * @param buf       Path string buffer
 * @param size      Path buffer size
 * @return char*    Path string buffer
 */
char *ff_getcwd(char *buf, size_t size)
{
    FRESULT Status;

    if (buf == NULL || size == 0) {
        return NULL;
    }

    Status = f_getcwd(buf, size);
    if (Status == FR_OK) {
        return buf;
    } else {
    }

    return NULL;
}

/*************************************************************************************************************************
* 函数			:	int ff_ftruncate(int fd, ff_off_t  length)
* 功能			:	指定文件大小
* 参数			:	fd:已经打开的文件描述符，必须由写入权限；length：指定的文件大小
* 返回			:	0：成功；负数：返回错误码
* 依赖			:	FATFS
* 作者			:	cp1300@139.com
* 时间			:	2020-09-29
* 最后修改时间 	: 	2020-09-29
* 说明			:
*************************************************************************************************************************/
/**
 * @brief
 *
 * @param fd        Specify file size,Automatically truncate or fill
 * @param length    File descriptor that is already open. Must have write permission
 * @return int      0: success;
 *                  Negative: Data synchronization failure, return error code
 */
int ff_ftruncate(int fd, ff_off_t length)
{
    FRESULT Status;
    UINT cnt;
    uint32_t PackCnt;
    uint16_t EndPackSize;
    uint8_t *pBuff;
    uint32_t i;

    if (fd <= 0)
        return -ENXIO;
    if (length <= ((FIL *)fd)->obj.objsize) {
        Status = f_lseek((FIL *)fd, length);
        if (Status == FR_OK) {
            Status = f_truncate((FIL *)fd);
            if (Status == FR_OK) {
                return 0;
            } else {
            }
        } else {
        }
        return ff_vfs_error(Status);
    } else {
        Status = f_lseek((FIL *)fd, ((FIL *)fd)->obj.objsize);
        if (Status == FR_OK) {
            PackCnt = (length - ((FIL *)fd)->obj.objsize) / 512;
            EndPackSize = (length - ((FIL *)fd)->obj.objsize) % 512;
            pBuff = vfs_malloc(512);
            if (pBuff == NULL) {
                return -ENOMEM;
            }
            memset(pBuff, 0, 512);

            for (i = 0; i < PackCnt; i++) {
                Status = f_write((FIL *)fd, pBuff, 512, (UINT *)&cnt);
                if (Status != FR_OK) {
                    break;
                }
            }
            if ((Status == FR_OK) && EndPackSize) {
                Status = f_write((FIL *)fd, pBuff, EndPackSize, (UINT *)&cnt);
                if (Status != FR_OK) {
                }
            }
            vfs_free(pBuff);
        } else {
        }
    }

    return ff_vfs_error(Status);
}

/**
 * @brief
 *
 * @param fd
 * @param cmd
 * @param lock
 * @return int
 */
int ff_fcntl(int fd, int cmd, struct flock *lock)
{
    if (fd <= 0)
        return -ENXIO; //文件指针为空,直接返回

    return 0;
}

/**
 * @brief           Change access to existing files
 *
 * @param fd        The file descriptor
 * @param mode
 * @return int
 */
int ff_fchmod(int fd, ff_mode_t mode)
{
    if (fd <= 0)
        return -ENXIO; //文件指针为空,直接返回

    return 0;
}

/**
 * @brief   Delete the file
 *
 * @param path  The file path
 * @return int  0: success;
 *              Negative: Data synchronization failure, return error code
 */
int ff_unlink(const char *path)
{
    FRESULT Status;
    FILINFO *pInfo;

    if (path == NULL)
        return -ENXIO;
    pInfo = vfs_malloc(sizeof(FILINFO));
    if (pInfo == NULL) {
        return -ENOMEM;
    }

    Status = f_stat(path, pInfo);
    if (Status == FR_OK) {
        Status = f_unlink(path);
        if (Status != FR_OK) {
        }
    } else {
    }
    vfs_free(pInfo);

    return ff_vfs_error(Status);
}

/**
 * @brief Create a directory, If it already exists, the creation fails
 *
 * @param path  The directory path
 * @return int  0: success;
 *              Negative: Data synchronization failure, return error code
 */
int ff_mkdir(const char *path)
{
    FRESULT Status;

    if (path == NULL)
        return -ENXIO;
    Status = f_mkdir(path);

    return ff_vfs_error(Status);
}

/**
 * @brief   Delete a directory,When you delete a directory, the directory must be empty
 *
 * @param path  Directory path
 * @return int  0: success;
 *              Negative: Data synchronization failure, return error code
 */
int ff_rmdir(const char *path)
{
    FRESULT Status;
    FILINFO *pInfo;

    if (path == NULL)
        return -ENXIO;
    pInfo = vfs_malloc(sizeof(FILINFO));
    if (pInfo == NULL) {
        return -ENOMEM;
    }

    Status = f_stat(path, pInfo);
    if (Status == FR_INVALID_NAME) {
        Status = f_unlink(path);
        if (Status != FR_OK) {
        }
    } else {
        if (Status != FR_OK) {
        } else {
            vfs_free(pInfo);
            return -ENOTDIR;
        }
    }
    vfs_free(pInfo);

    return ff_vfs_error(Status);
}

/**
 * @brief
 *
 * @param fd
 * @param owner
 * @param group
 * @return int
 */

int ff_fchown(int fd, ff_uid_t owner, ff_gid_t group)
{
    return 0;
}

/**
 * @brief
 *
 * @return ff_uid_t
 */
ff_uid_t ff_geteuid(void)
{
    return 0;
}

/**
 * @brief
 *
 * @param path
 * @param times
 * @return int
 */
int ff_utimes(const char *path, const struct ff_timeval times[2])
{
    return 0;
}