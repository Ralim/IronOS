/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "stddef.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM 0 /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC 1 /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB 2 /* Example: Map USB MSD to physical drive 2 */

FATFS_DiskioDriverTypeDef pDiskioDriver = {
    .RAM_disk_status = NULL,
    .MMC_disk_status = NULL,
    .USB_disk_status = NULL,
    .RAM_disk_initialize = NULL,
    .MMC_disk_initialize = NULL,
    .USB_disk_initialize = NULL,
    .RAM_disk_read = NULL,
    .MMC_disk_read = NULL,
    .USB_disk_read = NULL,
    .RAM_disk_write = NULL,
    .MMC_disk_write = NULL,
    .USB_disk_write = NULL,
    .RAM_disk_ioctl = NULL,
    .MMC_disk_ioctl = NULL,
    .USB_disk_ioctl = NULL,
    .Translate_Result_Code = NULL,
};

/*-----------------------------------------------------------------------*/
/* init driver callback                                                  */
/*-----------------------------------------------------------------------*/
void disk_driver_callback_init(FATFS_DiskioDriverTypeDef *pNewDriver)
{
    if (pNewDriver->RAM_disk_status)
        pDiskioDriver.RAM_disk_status = pNewDriver->RAM_disk_status;
    if (pNewDriver->MMC_disk_status)
        pDiskioDriver.MMC_disk_status = pNewDriver->MMC_disk_status;
    if (pNewDriver->USB_disk_status)
        pDiskioDriver.USB_disk_status = pNewDriver->USB_disk_status;
    if (pNewDriver->RAM_disk_initialize)
        pDiskioDriver.RAM_disk_initialize = pNewDriver->RAM_disk_initialize;
    if (pNewDriver->MMC_disk_initialize)
        pDiskioDriver.MMC_disk_initialize = pNewDriver->MMC_disk_initialize;
    if (pNewDriver->USB_disk_initialize)
        pDiskioDriver.USB_disk_initialize = pNewDriver->USB_disk_initialize;
    if (pNewDriver->RAM_disk_read)
        pDiskioDriver.RAM_disk_read = pNewDriver->RAM_disk_read;
    if (pNewDriver->MMC_disk_read)
        pDiskioDriver.MMC_disk_read = pNewDriver->MMC_disk_read;
    if (pNewDriver->USB_disk_read)
        pDiskioDriver.USB_disk_read = pNewDriver->USB_disk_read;
    if (pNewDriver->RAM_disk_write)
        pDiskioDriver.RAM_disk_write = pNewDriver->RAM_disk_write;
    if (pNewDriver->MMC_disk_write)
        pDiskioDriver.MMC_disk_write = pNewDriver->MMC_disk_write;
    if (pNewDriver->USB_disk_write)
        pDiskioDriver.USB_disk_write = pNewDriver->USB_disk_write;
    if (pNewDriver->RAM_disk_ioctl)
        pDiskioDriver.RAM_disk_ioctl = pNewDriver->RAM_disk_ioctl;
    if (pNewDriver->MMC_disk_ioctl)
        pDiskioDriver.MMC_disk_ioctl = pNewDriver->MMC_disk_ioctl;
    if (pNewDriver->USB_disk_ioctl)
        pDiskioDriver.USB_disk_ioctl = pNewDriver->USB_disk_ioctl;
    if (pNewDriver->Translate_Result_Code)
        pDiskioDriver.Translate_Result_Code = pNewDriver->Translate_Result_Code;
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    switch (pdrv) {
        case DEV_RAM:
            if (pDiskioDriver.RAM_disk_status) {
                result = pDiskioDriver.RAM_disk_status();
            }

            break;

        case DEV_MMC:
            if (pDiskioDriver.MMC_disk_status) {
                result = pDiskioDriver.MMC_disk_status();
            }

            break;

        case DEV_USB:
            if (pDiskioDriver.USB_disk_status) {
                result = pDiskioDriver.USB_disk_status();
            }

            break;

        default:
            return STA_NOINIT;
    }

    /* translate the reslut code here */
    if (pDiskioDriver.Translate_Result_Code) {
        stat = pDiskioDriver.Translate_Result_Code(result);
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    switch (pdrv) {
        case DEV_RAM:
            if (pDiskioDriver.RAM_disk_initialize) {
                result = pDiskioDriver.RAM_disk_initialize();
            }

            break;

        case DEV_MMC:
            if (pDiskioDriver.MMC_disk_initialize) {
                result = pDiskioDriver.MMC_disk_initialize();
            }

            break;

        case DEV_USB:
            if (pDiskioDriver.USB_disk_initialize) {
                result = pDiskioDriver.USB_disk_initialize();
            }

            break;

        default:
            return STA_NOINIT;
    }

    /* translate the reslut code here */
    if (pDiskioDriver.Translate_Result_Code) {
        stat = pDiskioDriver.Translate_Result_Code(result);
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    switch (pdrv) {
        case DEV_RAM:
            if (pDiskioDriver.RAM_disk_read) {
                result = pDiskioDriver.RAM_disk_read(buff, sector, count);
            }

            break;

        case DEV_MMC:
            if (pDiskioDriver.MMC_disk_read) {
                result = pDiskioDriver.MMC_disk_read(buff, sector, count);
            }

            break;

        case DEV_USB:
            if (pDiskioDriver.USB_disk_read) {
                result = pDiskioDriver.USB_disk_read(buff, sector, count);
            }

            break;

        default:
            return RES_PARERR;
    }

    /* translate the reslut code here */
    if (pDiskioDriver.Translate_Result_Code) {
        stat = pDiskioDriver.Translate_Result_Code(result);
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    LBA_t sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to write */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    switch (pdrv) {
        case DEV_RAM:
            if (pDiskioDriver.RAM_disk_write) {
                result = pDiskioDriver.RAM_disk_write(buff, sector, count);
            }

            break;

        case DEV_MMC:
            if (pDiskioDriver.MMC_disk_write) {
                result = pDiskioDriver.MMC_disk_write(buff, sector, count);
            }

            break;

        case DEV_USB:
            if (pDiskioDriver.USB_disk_write) {
                result = pDiskioDriver.USB_disk_write(buff, sector, count);
            }

            break;

        default:
            return RES_PARERR;
    }

    /* translate the reslut code here */
    if (pDiskioDriver.Translate_Result_Code) {
        stat = pDiskioDriver.Translate_Result_Code(result);
    }

    return stat;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
)
{
    DSTATUS stat = STA_NOINIT;
    int result = 0;

    switch (pdrv) {
        case DEV_RAM:
            if (pDiskioDriver.RAM_disk_ioctl) {
                result = pDiskioDriver.RAM_disk_ioctl(cmd, buff);
            }

            break;

        case DEV_MMC:
            if (pDiskioDriver.MMC_disk_ioctl) {
                result = pDiskioDriver.MMC_disk_ioctl(cmd, buff);
            }

            break;

        case DEV_USB:
            if (pDiskioDriver.USB_disk_ioctl) {
                result = pDiskioDriver.USB_disk_ioctl(cmd, buff);
            }

            break;

        default:
            return RES_PARERR;
    }

    /* translate the reslut code here */
    if (pDiskioDriver.Translate_Result_Code) {
        stat = pDiskioDriver.Translate_Result_Code(result);
    }

    return stat;
}
