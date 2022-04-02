/**
 * @file lv_port_fs_templ.c
 *
 */

/*Copy this file as "lv_port_fs.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs.h"
#include "stdio.h"
#include "string.h"
#include "bflb_platform.h"


#if LV_USING_FATFS
#include "ff.h"
#endif

#if LV_USING_ROMFS
#include "bl_romfs.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Create a type to store the required data about your file.
 * If you are using a File System library
 * it already should have a File type.
 * For example FatFS has `file_t `. In this case use `typedef file_t  file_t`*/
typedef uint32_t file_t; /* 没有实际作用, 使用时是通过指针强转的 */

/*Similarly to `file_t` create a type for directory reading too */
typedef uint32_t dir_t; /* 没有实际作用, 使用时是通过指针强转的 */

#if (LV_USING_FATFS)

FATFS FS_OBJ_SD;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static lv_fs_res_t fs_open(lv_fs_drv_t *drv, void *file_p, const char *path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos);
static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p);
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);
static lv_fs_res_t fs_remove(lv_fs_drv_t *drv, const char *path);
static lv_fs_res_t fs_trunc(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t fs_rename(lv_fs_drv_t *drv, const char *oldname, const char *newname);
static lv_fs_res_t fs_free(lv_fs_drv_t *drv, uint32_t *total_p, uint32_t *free_p);
static lv_fs_res_t fs_dir_open(lv_fs_drv_t *drv, void *rddir_p, const char *path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p);

extern void fatfs_sd_driver_register(void);

#endif

#if (LV_USING_ROMFS)
static lv_fs_res_t lvport_romfs_open(lv_fs_drv_t *drv, void *file_p, const char *path, lv_fs_mode_t mode);
static lv_fs_res_t lvport_romfs_close(lv_fs_drv_t *drv, void *file_p);
static lv_fs_res_t lvport_romfs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p);
static lv_fs_res_t lvport_romfs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);
static lv_fs_res_t lvport_romfs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos);
static lv_fs_res_t lvport_romfs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);
static lv_fs_res_t lvport_romfs_dir_close(struct _lv_fs_drv_t *drv, void *rddir_p);
static lv_fs_res_t lvport_romfs_dir_open(struct _lv_fs_drv_t *drv, void *rddir_p, const char *path);
static lv_fs_res_t lvport_romfs_dir_read(struct _lv_fs_drv_t *drv, void *rddir_p, char *fn);

#endif

/**********************
 *  STATIC VARIABLES
 **********************/
typedef uint8_t lv_fs_res_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void)
{
/* fatfs init */
#if (LV_USING_FATFS)
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /* Add a simple drive to open images */
    lv_fs_drv_t fs_drv_s;
    lv_fs_drv_init(&fs_drv_s);

    /*Set up fields...*/
    fs_drv_s.file_size = sizeof(FIL);
    fs_drv_s.letter = 'S';
    fs_drv_s.open_cb = fs_open;
    fs_drv_s.close_cb = fs_close;
    fs_drv_s.read_cb = fs_read;
    fs_drv_s.write_cb = fs_write;
    fs_drv_s.seek_cb = fs_seek;
    fs_drv_s.tell_cb = fs_tell;
    fs_drv_s.free_space_cb = fs_free;
    fs_drv_s.size_cb = fs_size;
    fs_drv_s.remove_cb = fs_remove;
    fs_drv_s.rename_cb = fs_rename;
    fs_drv_s.trunc_cb = fs_trunc;

    fs_drv_s.rddir_size = sizeof(DIR);
    fs_drv_s.dir_close_cb = fs_dir_close;
    fs_drv_s.dir_open_cb = fs_dir_open;
    fs_drv_s.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv_s);
#endif

/* romfs init */
#if (LV_USING_ROMFS)

    romfs_mount();

    /* Add a simple drive to open images */
    lv_fs_drv_t fs_drv_r;
    lv_fs_drv_init(&fs_drv_r);

    /*Set up fields...*/
    fs_drv_r.file_size = sizeof(romfs_file_t);
    fs_drv_r.letter = 'R';
    fs_drv_r.open_cb = lvport_romfs_open;
    fs_drv_r.close_cb = lvport_romfs_close;
    fs_drv_r.read_cb = lvport_romfs_read;
    fs_drv_r.write_cb = NULL;
    fs_drv_r.seek_cb = lvport_romfs_seek;
    fs_drv_r.tell_cb = lvport_romfs_tell;
    fs_drv_r.free_space_cb = NULL;
    fs_drv_r.size_cb = lvport_romfs_size;
    fs_drv_r.remove_cb = NULL;
    fs_drv_r.rename_cb = NULL;
    fs_drv_r.trunc_cb = NULL;

    fs_drv_r.rddir_size = sizeof(romfs_dir_t);
    fs_drv_r.dir_close_cb = lvport_romfs_dir_close;
    fs_drv_r.dir_open_cb = lvport_romfs_dir_open;
    fs_drv_r.dir_read_cb = lvport_romfs_dir_read;

    lv_fs_drv_register(&fs_drv_r);
#endif
}

#if (LV_USING_FATFS)
/**********************
 *   STATIC FUNCTIONS
 **********************/
lv_fs_res_t res_fatfs_to_lv(FRESULT res)
{
    if (res == FR_OK) {
        return LV_FS_RES_OK;
    }

    switch (res) {
        case (FR_DISK_ERR):
            res = LV_FS_RES_HW_ERR;
            break;

        case (FR_NO_FILE):
            res = LV_FS_RES_NOT_EX;
            break;

        case (FR_NO_PATH):
            res = LV_FS_RES_NOT_EX;
            break;

        case (FR_NOT_ENOUGH_CORE):
            res = LV_FS_RES_OUT_OF_MEM;
            break;

        case (FR_LOCKED):
            res = LV_FS_RES_LOCKED;

        case (FR_TOO_MANY_OPEN_FILES):
            res = LV_FS_RES_LOCKED;
            break;

        case (FR_NO_FILESYSTEM):
            res = LV_FS_RES_FS_ERR;
            break;

        case (FR_WRITE_PROTECTED):
            res = LV_FS_RES_DENIED;
            break;

        case (FR_TIMEOUT):
            res = LV_FS_RES_TOUT;
            break;

        default:
            res = LV_FS_RES_UNKNOWN;
    }

    return res;
}

/* Initialize your Storage device and File system. */
static void fs_init(void)
{
    /*E.g. for FatFS initialize the SD card and FatFS itself*/

    /*You code here*/
    fatfs_sd_driver_register();
    f_mount(&FS_OBJ_SD, "sd:", 1);
}

/**
 * Open a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_open(lv_fs_drv_t *drv, void *file_p, const char *path, lv_fs_mode_t mode)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    BYTE fatfs_mode;
    char *path_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_buf = (char *)malloc(sizeof(char) * (strlen(path) + 5));
            sprintf(path_buf, "SD:/%s", path);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    switch (mode) {
        case (LV_FS_MODE_RD):
            fatfs_mode = FA_READ;
            break;

        case (LV_FS_MODE_WR):
            fatfs_mode = FA_WRITE;
            break;

        case (LV_FS_MODE_WR | LV_FS_MODE_RD):
            fatfs_mode = FA_WRITE | FA_READ;
            break;

        default:
            fatfs_mode = LV_FS_MODE_RD;
            break;
    }

    res = f_open((FIL *)file_p, path_buf, fatfs_mode);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_buf);
    return res;
}

/**
 * Close an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_close((FIL *)file_p);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Read data from an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_read((FIL *)file_p, buf, btr, (UINT *)br);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Write into a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_write((FIL *)file_p, buf, btw, (UINT *)bw);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_lseek((FIL *)file_p, pos);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Give the size of a file bytes
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param size pointer to a variable to store the size
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p)
{
    //lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    *size_p = f_size((FIL *)file_p);
    return LV_FS_RES_OK;
}
/**
 * Give the position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    //lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    *pos_p = f_tell((FIL *)file_p);
    return LV_FS_RES_OK;
}

/**
 * Delete a file
 * @param drv pointer to a driver where this function belongs
 * @param path path of the file to delete
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_remove(lv_fs_drv_t *drv, const char *path)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    char *path_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_buf = (char *)malloc(sizeof(char) * (strlen(path) + 5));
            sprintf(path_buf, "SD:/%s", path);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_unlink(path_buf);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_buf);
    return res;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_fs_open )
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_trunc(lv_fs_drv_t *drv, void *file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = f_truncate((FIL *)file_p);
    res = res_fatfs_to_lv(res);

    return res;
}

/**
 * Rename a file
 * @param drv pointer to a driver where this function belongs
 * @param oldname path to the file
 * @param newname path with the new name
 * @return LV_FS_RES_OK or any error from 'fs_res_t'
 */
static lv_fs_res_t fs_rename(lv_fs_drv_t *drv, const char *oldname, const char *newname)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    char *path_old_buf = NULL;
    char *path_new_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_old_buf = (char *)malloc(sizeof(char) * (strlen(oldname) + 5));
            path_new_buf = (char *)malloc(sizeof(char) * (strlen(newname) + 5));
            sprintf(path_old_buf, "SD:/%s", oldname);
            sprintf(path_new_buf, "SD:/%s", newname);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_rename(path_old_buf, path_new_buf);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_old_buf);
    lv_mem_free(path_new_buf);
    return res;
}

/**
 * Get the free and total size of a driver in kB
 * @param drv pointer to a driver where this function belongs
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_free(lv_fs_drv_t *drv, uint32_t *total_p, uint32_t *free_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    char *path = NULL;
    FATFS *fs_obj;

    /* Add your code here*/
    switch (drv->letter) {
        case 'S':
            path = "SD:";
            fs_obj = &FS_OBJ_SD;
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_getfree(path, free_p, &fs_obj);
    *free_p = (fs_obj->csize) * (*free_p) / 1024;
    *total_p = (fs_obj->csize) * (fs_obj->n_fatent - 2) / 1024;

    res = res_fatfs_to_lv(res);
    return res;
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv pointer to a driver where this function belongs
 * @param rddir_p pointer to a 'lv_fs_dir_t' variable
 * @param path path to a directory
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_open(lv_fs_drv_t *drv, void *rddir_p, const char *path)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    char *path_buf = NULL;

    switch (drv->letter) {
        case 'S':
            path_buf = (char *)malloc(sizeof(char) * (strlen(path) + 5));
            sprintf(path_buf, "SD:/%s", path);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = f_opendir(rddir_p, path_buf);
    res = res_fatfs_to_lv(res);

    lv_mem_free(path_buf);
    return res;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv pointer to a driver where this function belongs
 * @param rddir_p pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    FILINFO entry;

    /* Add your code here*/
    res = f_readdir(rddir_p, &entry);
    res = res_fatfs_to_lv(res);

    if (res == LV_FS_RES_OK) {
        sprintf(fn, "%s%s", (entry.fattrib & AM_DIR) ? "/" : "", entry.fname);
    }

    return res;
}

/**
 * Close the directory reading
 * @param drv pointer to a driver where this function belongs
 * @param rddir_p pointer to an initialized 'lv_fs_dir_t' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    f_closedir((DIR *)rddir_p);
    res = res_fatfs_to_lv(res);
    return res;
}
#endif

#if (LV_USING_ROMFS)

static lv_fs_res_t lvport_romfs_open(lv_fs_drv_t *drv, void *file_p, const char *path, lv_fs_mode_t mode)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    char *path_buf = NULL;

    switch (drv->letter) {
        case 'R':
            path_buf = (char *)malloc(sizeof(char) * (strlen(path) + 8));
            sprintf(path_buf, "/romfs/%s", path);
            break;

        default:
            return LV_FS_RES_NOT_EX;
            break;
    }

    res = romfs_open((romfs_file_t *)file_p, path_buf, mode);

    lv_mem_free(path_buf);
    return res;
}

static lv_fs_res_t lvport_romfs_close(lv_fs_drv_t *drv, void *file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = romfs_close((romfs_file_t *)file_p);

    return res;
}

static lv_fs_res_t lvport_romfs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    uint32_t num = romfs_read((romfs_file_t *)file_p, buf, btr);
    if (num > 0) {
        *br = num;
        res = 0;
    }else{
        res = num;
    }

    return res;
}

static lv_fs_res_t lvport_romfs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/
    res = romfs_lseek((romfs_file_t *)file_p, pos, ROMFS_SEEK_SET);
    if (res > 0) {
        res = 0;
    }

    return res;
}

static lv_fs_res_t lvport_romfs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p)
{
    if(file_p == NULL){
        return -1;
    }

    *pos_p = ((romfs_file_t *)file_p)->offset;

    return 0;
}

static lv_fs_res_t lvport_romfs_dir_close(struct _lv_fs_drv_t *drv, void *rddir_p)
{
    return romfs_closedir((romfs_dir_t *)rddir_p);
}

static lv_fs_res_t lvport_romfs_dir_open(struct _lv_fs_drv_t *drv, void *rddir_p,const char *path)
{
    char *path_buf = NULL;

    if(rddir_p == NULL){
        return LV_FS_RES_UNKNOWN;
    }

    path_buf = (char *)malloc(sizeof(char) * (strlen(path) + 8));
    sprintf(path_buf, "/romfs/%s", path);

    return romfs_opendir((romfs_dir_t *)rddir_p,(const char *)path_buf);
}

static lv_fs_res_t lvport_romfs_dir_read(struct _lv_fs_drv_t *drv, void *rddir_p, char *fn)
{
    if(rddir_p == NULL){
        return LV_FS_RES_UNKNOWN;
    }

    romfs_dirent_t *romfs_dirent = romfs_readdir((romfs_dir_t *)rddir_p);
    if(romfs_dirent == NULL){
        return LV_FS_RES_UNKNOWN;
    }
    if(fn != NULL){
        memcpy((void*)fn,(const void*)romfs_dirent,sizeof(romfs_dirent_t));
    }
    return LV_FS_RES_OK;
}

static lv_fs_res_t lvport_romfs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p)
{
    /* Add your code here*/
    int res = romfs_size((romfs_file_t *)file_p);
    if(res<0){
        return LV_FS_RES_UNKNOWN;
    }
    *size_p = res;
    return LV_FS_RES_OK;
}

#endif

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
