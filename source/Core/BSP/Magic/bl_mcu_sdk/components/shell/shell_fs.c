#if defined(SHELL_USING_FS)
#include "shell.h"
#include "ff.h"
#include "fatfs_posix_api.h"

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

static char path[SHELL_CONSOLEBUF_SIZE] = { 0 };

#ifdef DFS_USING_WORKDIR
extern char working_directory[];
#endif

int cmd_ls(int argc, char **argv)
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
SHELL_CMD_EXPORT_ALIAS(cmd_ls, ls, List information about the FILEs.);

int cmd_rm(int argc, char **argv)
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
SHELL_CMD_EXPORT_ALIAS(cmd_rm, rm, Remove(unlink) the FILE(s).);

int cmd_cd(int argc, char **argv)
{
    int err;

    if (argc < 2) {
        strcpy(path, ".");
    } else {
        strcpy(path, argv[1]);
    }

    for (uint16_t i = strlen(path) - 1; i > 0; i--) {
        if (path[i] == '/')
            path[i] = '\0';
        else
            break;
    }

    err = f_chdir(path);

    if (err) {
        MSG("%s %s\r\n", fatfs_table[err], path);
        return -1;
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cd, cd, Change the shell working directory.);

int cmd_pwd(int argc, char **argv)
{
    int err;
    err = f_getcwd(path, SHELL_CONSOLEBUF_SIZE);

    if (err) {
        MSG("%s\r\n", fatfs_table[err]);
        return -1;
    }

    MSG("%s\r\n", path);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pwd, pwd, Print the name of the current working directory.);

int cmd_mkdir(int argc, char **argv)
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
SHELL_CMD_EXPORT_ALIAS(cmd_mkdir, mkdir, Create the DIRECTORY.);

#endif