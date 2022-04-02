/**
 * @file fatfs_posix_api.h
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

/*************************************************************************************************************
 * 文件名		:	ff_vfs.h
 * 功能			:	VFS虚拟文件系统支持
 * 作者			:	cp1300@139.com
 * 创建时间		:	2020-09-29
 * 最后修改时间	:	2020-09-29
 * 详细			:	兼容linux的vfs文件系统API支持，需要先初始化fatfs
*************************************************************************************************************/
#ifndef _FF_VFS_H_
#define _FF_VFS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "ff.h"
#include "stdio.h"

typedef int ff_dev_t; //32为int数据，高12位主设备号，低20位次设备号

typedef int ff_off_t; //文件偏移此处使用32bit格式，由于FATFS最大只支持4G大小文件,此处设置为只支持2GB文件大小

typedef uint32_t ff_time_t;

typedef struct {
    uint32_t dd_vfs_fd;
    uint32_t dd_rsv;
} ff_dir_t;

struct stat {
    ff_dev_t st_dev;
    uint32_t st_size;
    uint32_t st_mode;
    ff_time_t st_actime;
    ff_time_t st_modtime;
    uint64_t st_ino;
    uint32_t st_nlink;
    uint32_t st_blksize;
    uint32_t st_uid;
    uint32_t st_gid;
};

typedef struct {
    uint32_t d_ino; /* file number */
    uint8_t d_type; /* type of file */
    char d_name[];  /* file name */
} ff_dirent_t;

typedef const struct fs_ops fs_ops_t;

struct ff_utimbuf {
    ff_time_t actime;  /**< time of last access */
    ff_time_t modtime; /**< time of last modification */
};
//用于  utimes 修改文件的访问时间，未实现
struct ff_timeval {
    ff_time_t tv_sec;  /* seconds */
    ff_time_t tv_usec; /* microseconds */
};

/* for posix fcntl() and lockf() */
#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

//打开文件的参数flags定义
#define O_RDONLY   (1 << 0) //以只读方式打开文件
#define O_WRONLY   (1 << 1) //以只写方式打开
#define O_RDWR     (O_RDONLY | O_WRONLY) //以可读可写方式打开
#define O_ACCMODE  (O_RDWR)
#define O_CREAT    (1 << 8) //文件不存在时创建文件
#define O_EXCL     (1 << 9) //在打开文件时，如果文件存在而且同时指定的标志位O_CREAT，则返回-EEXIST；如果文件不存在，则创建文件
#define O_NOCTTY   (1 << 10)
#define O_TRUNC    (1 << 11) //打开文件时，如果文件的长度不为0，则将文件的长度截短为0，下次写入文件时，从文件开始出写入数据
#define O_APPEND   (1 << 12) //打开文件后，将文件的写入点设置为文件末尾，下次写入文件时，新写入的数据会追加到文件末尾
#define O_DSYNC    (1 << 13)
#define O_NONBLOCK (1 << 14)
#define O_SYNC     (1 << 15)

//文件偏移参数
#define VFS_SEEK_SET (0) //文件偏移设置为从文件开始处offset字节处
#define VFS_SEEK_CUR (1) //文件偏移设置为相对于当前文件位置的offset字节处
#define VFS_SEEK_END (2) //文件偏移摄者为文件末尾的offset字节处

//访问权限测试值
#define R_OK (1 << 2)
#define W_OK (1 << 1)
#define X_OK (1 << 0)

//文件特性，并未实现
struct flock {
    short l_type;
    short l_whence;
    short l_start;
    short l_len;
};
#define F_DUPFD 0 /* dup */
#define F_GETFD 1 /* get close_on_exec */
#define F_SETFD 2 /* set/clear close_on_exec */
#define F_GETFL 3 /* get file->f_flags */
#define F_SETFL 4 /* set file->f_flags */
#ifndef F_GETLK
#define F_GETLK  5
#define F_SETLK  6
#define F_SETLKW 7
#endif //F_GETLK

//文件权限模式，并未实现
typedef unsigned int ff_mode_t;

//用户与组id，并未实现
typedef unsigned int ff_uid_t;
typedef unsigned int ff_gid_t;

#define F_OK    0
#define EPERM   1 /* Operation not permitted */
#define ENOENT  2 /* No such file or directory */
#define ESRCH   3 /* No such process */
#define EINTR   4 /* Interrupted system call */
#define EIO     5 /* I/O error */
#define ENXIO   6 /* No such device or address */
#define E2BIG   7 /* Argument list too long */
#define ENOEXEC 8 /* Exec format error */
#define EBADF   9 /* Bad file number */
#define ECHILD  10 /* No child processes */
#define EAGAIN  11 /* Try again */
#define ENOMEM  12 /* Out of memory */
#define EACCES  13 /* Permission denied */
#define EFAULT  14 /* Bad address */
#define ENOTBLK 15 /* Block device required */
#define EBUSY   16 /* Device or resource busy */
#define EEXIST  17 /* File exists */
#define EXDEV   18 /* Cross-device link */
#define ENODEV  19 /* No such device */
#define ENOTDIR 20 /* Not a directory */
#define EISDIR  21 /* Is a directory */
#define EINVAL  22 /* Invalid argument */
#define ENFILE  23 /* File table overflow */
#define EMFILE  24 /* Too many open files */
#define ENOTTY  25 /* Not a typewriter */
#define ETXTBSY 26 /* Text file busy */
#define EFBIG   27 /* File too large */
#define ENOSPC  28 /* No space left on device */
#define ESPIPE  29 /* Illegal seek */
#define EROFS   30 /* Read-only file system */
#define EMLINK  31 /* Too many links */
#define EPIPE   32 /* Broken pipe */
#define EDOM    33 /* Math argument out of domain of func */
#define ERANGE  34 /* Math result not representable */

int ff_vfs_init(void);                                             //初始化VFS系统。系统启动流程中组件初始化函数
int ff_open(const char *path, int flags, int mode);                //打开path所描述的文件或者设备
int ff_close(int fd);                                              //关闭已打开的文件描述符fd
int ff_read(int fd, void *buf, size_t nbytes);                     //从打开的文件中读取若干字节到buffer
int ff_write(int fd, const void *buf, size_t nbytes);              //将buffer开始出的nbytes字节数据写入到已经打开的文件
ff_off_t ff_lseek(int fd, ff_off_t offset, int whence);            //重新设置已打开文件的偏移
int ff_sync(int fd);                                               //会将对文件系统元数据(metadata)的修改以及文件系统内部缓存中的数据写入底层文件系统中
int ff_stat(const char *path, struct stat *st);                    //读取文件的属性信息，如文件大小、文件模式（权限、文件/文件夹）
int ff_fstat(int fd, struct stat *st);                             //读取文件的属性信息，如文件大小、文件模式（权限、文件/文件夹）等信息到st指针指向的buffer
int ff_link(const char *oldpath, const char *newpath);             //为文件创建一个新的链接文件。如果目标文件存在，则不会覆盖
int ff_unlink(const char *path);                                   //从文件系统中删除文件名
int ff_remove(const char *path);                                   //从文件系统中删除文件名，它既可以删除文件，也可以删除目录。
int ff_rename(const char *oldpath, const char *newpath);           //重命名一个文件，可以将文件从一个文件夹下通过重命名移动到另外一个文件夹下
ff_dir_t *ff_opendir(const char *path);                            //打开目录名path对应的目录流，并返回目录流指针
int ff_closedir(ff_dir_t *dirp);                                   //关闭目录流。调用ff_closedir()后，目录流指针不再可用
ff_dirent_t *ff_readdir(ff_dir_t *dirp);                           //返回一个指向ff_dirent_t的指针，该指针关联的目录流dirp的成员指针指向下一个成员
int ff_mkdir(const char *path);                                    //创建名为path的目录。如果已经存在path目录，则创建失败
int ff_rmdir(const char *path);                                    //删除一个目录。删除目录时，目录必须为空。
void ff_rewinddir(ff_dir_t *dirp);                                 //将目录流dirp的位置重置为目录的开始处。
int ff_telldir(ff_dir_t *dirp);                                    //返回与目录流dirp相关联的目录流的当前位置。
void ff_seekdir(ff_dir_t *dir, long loc);                          //设置目录流的位置，下次调用ff_readdir()时目录流将从设置的位置开始读取目录
int ff_access(const char *path, int amode);                        //检查当前程序是否可以访问path文件
int ff_chdir(const char *path);                                    //改变当前程序的工作目录到path路径。
char *ff_getcwd(char *buf, size_t size);                           //返回当当前程序的绝对工作目录字符串指针，同时如果buf参数不为空，也将当前工作目录字符串复制到buf中
long ff_pathconf(const char *path, int name);                      //函数返回配置文件的限制值，是与文件或目录相关联的运行时限制。
long ff_fpathconf(int fd, int name);                               //返回配置文件的限制值
int ff_utime(const char *path, const struct ff_utimbuf *times);    //通过参数times的actime和modtime成员改变文件的访问时间和修改时间
int ff_ftruncate(int fd, ff_off_t length);                         //指定文件大小
int ff_fcntl(int fd, int cmd, struct flock *lock);                 //根据文件描述词来操作文件的特性
int ff_fchmod(int fd, ff_mode_t mode);                             //改变现有文件的访问权限
int ff_fchown(int fd, ff_uid_t owner, ff_gid_t group);             //更改文件的用户ID和组ID
ff_uid_t ff_geteuid(void);                                         //获取用户有效 UID 值
int ff_utimes(const char *path, const struct ff_timeval times[2]); //修改一个文件的访问时间和修改时间

#define S_ISREG(st_mode) 0 //是否是一个常规文件-未实现
#define S_ISDIR(st_mode) 0 //是否是一个目录-未实现

//标准VFS文件接口
#define open  ff_open //打开文件
#define close ff_close //关闭已打开的文件
#define read  ff_read //读取已打开的文件
#define write ff_write //写入已打开的文件
#define lseek ff_lseek //重新设置已打开文件的偏移
//#define sync		ff_sync								//同步文件，刷新缓存
#define fsync ff_sync //同步文件，刷新缓存
__inline int stat(const char *path, struct stat *st)
{
    return ff_stat(path, st);
} //获取文件信息,有重名的结构体，这个地方只能使用内联函数
#define fstat     ff_fstat //获取文件信息
#define access    ff_access //测试访问权限
#define getcwd    ff_getcwd //返回当当前程序的绝对工作目录
#define ftruncate ff_ftruncate //指定文件大小
#define fcntl     ff_fcntl //根据文件描述词来操作文件的特性
#define fchmod    ff_fchmod //改变现有文件的访问权限
#define unlink    ff_unlink //从文件系统中删除文件
#define mkdir     ff_mkdir //创建名为path的目录
#define rmdir     ff_rmdir //删除一个目录
//#define fchown		ff_fchown							//更改文件的用户ID和组ID
//#define geteuid		ff_geteuid							//获取用户有效 UID 值
#define utimes ff_utimes //修改一个文件的访问时间和修改时间

#ifdef __cplusplus
}
#endif

#endif //_FF_VFS_H_
