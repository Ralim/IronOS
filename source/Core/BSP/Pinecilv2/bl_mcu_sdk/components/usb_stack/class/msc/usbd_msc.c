/**
 * @file usbd_msc.c
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
#include "usbd_core.h"
#include "usbd_scsi.h"
#include "usbd_msc.h"

/* max USB packet size */
#ifndef CONFIG_USB_HS
#define MASS_STORAGE_BULK_EP_MPS 64
#else
#define MASS_STORAGE_BULK_EP_MPS 512
#endif

#define MSD_OUT_EP_IDX 0
#define MSD_IN_EP_IDX  1

/* Describe EndPoints configuration */
static usbd_endpoint_t mass_ep_data[2];

/* MSC Bulk-only Stage */
enum Stage {
    MSC_READ_CBW = 0, /* Command Block Wrapper */
    MSC_DATA_OUT = 1, /* Data Out Phase */
    MSC_DATA_IN = 2,  /* Data In Phase */
    MSC_SEND_CSW = 3, /* Command Status Wrapper */
    MSC_WAIT_CSW = 4, /* Command Status Wrapper */
};

/** MSC Bulk-Only Command Block Wrapper (CBW) */
struct CBW {
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataLength;
    uint8_t bmFlags;
    uint8_t bLUN;
    uint8_t bCBLength;
    uint8_t CB[16];
} __packed;

/** MSC Bulk-Only Command Status Wrapper (CSW) */
struct CSW {
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataResidue;
    uint8_t bStatus;
} __packed;

/* Device data structure */
struct usbd_msc_cfg_private {
    /* state of the bulk-only state machine */
    enum Stage stage;
    struct CBW cbw;
    struct CSW csw;

    uint8_t sKey; /* Sense key */
    uint8_t ASC;  /* Additional Sense Code */
    uint8_t ASQ;  /* Additional Sense Qualifier */
    uint8_t max_lun;
    uint16_t scsi_blk_size;
    uint32_t scsi_blk_nbr;

    uint32_t scsi_blk_addr;
    uint32_t scsi_blk_len;
    uint8_t *block_buffer;

} usbd_msc_cfg;

/*memory OK (after a usbd_msc_memory_verify)*/
static bool memOK;

static void usbd_msc_reset(void)
{
    usbd_msc_cfg.stage = MSC_READ_CBW;
    usbd_msc_cfg.scsi_blk_addr = 0U;
    usbd_msc_cfg.scsi_blk_len = 0U;
    usbd_msc_cfg.max_lun = 0;
    usbd_msc_cfg.sKey = 0;
    usbd_msc_cfg.ASC = 0;
    usbd_msc_cfg.ASQ = 0;

    (void)memset((void *)&usbd_msc_cfg.cbw, 0, sizeof(struct CBW));
    (void)memset((void *)&usbd_msc_cfg.csw, 0, sizeof(struct CSW));

    usbd_msc_get_cap(0, &usbd_msc_cfg.scsi_blk_nbr, &usbd_msc_cfg.scsi_blk_size);

    if (usbd_msc_cfg.block_buffer) {
        free(usbd_msc_cfg.block_buffer);
    }
    usbd_msc_cfg.block_buffer = malloc(usbd_msc_cfg.scsi_blk_size * sizeof(uint8_t));
    memset(usbd_msc_cfg.block_buffer, 0, usbd_msc_cfg.scsi_blk_size * sizeof(uint8_t));
}

/**
 * @brief Handler called for Class requests not handled by the USB stack.
 *
 * @param setup    Information about the request to execute.
 * @param len       Size of the buffer.
 * @param data      Buffer containing the request result.
 *
 * @return  0 on success, negative errno code on fail.
 */
static int msc_storage_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("MSC Class request: "
                 "bRequest 0x%02x\r\n",
                 setup->bRequest);

    switch (setup->bRequest) {
        case MSC_REQUEST_RESET:
            USBD_LOG_DBG("MSC_REQUEST_RESET\r\n");

            if (setup->wLength) {
                USBD_LOG_WRN("Invalid length\r\n");
                return -1;
            }

            usbd_msc_reset();
            break;

        case MSC_REQUEST_GET_MAX_LUN:
            USBD_LOG_DBG("MSC_REQUEST_GET_MAX_LUN\r\n");

            if (setup->wLength != 1) {
                USBD_LOG_WRN("Invalid length\r\n");
                return -1;
            }

            *data = (uint8_t *)(&usbd_msc_cfg.max_lun);
            *len = 1;
            break;

        default:
            USBD_LOG_WRN("Unhandled MSC Class bRequest 0x%02x\r\n", setup->bRequest);
            return -1;
    }

    return 0;
}

static void usbd_msc_bot_abort(void)
{
    if ((usbd_msc_cfg.cbw.bmFlags == 0) && (usbd_msc_cfg.cbw.dDataLength != 0)) {
        usbd_ep_set_stall(mass_ep_data[MSD_OUT_EP_IDX].ep_addr);
    }
    usbd_ep_set_stall(mass_ep_data[MSD_IN_EP_IDX].ep_addr);
}

static void sendCSW(uint8_t CSW_Status)
{
    usbd_msc_cfg.csw.dSignature = MSC_CSW_Signature;
    usbd_msc_cfg.csw.bStatus = CSW_Status;

    /* updating the State Machine , so that we wait CSW when this
	 * transfer is complete, ie when we get a bulk in callback
	 */
    usbd_msc_cfg.stage = MSC_WAIT_CSW;

    if (usbd_ep_write(mass_ep_data[MSD_IN_EP_IDX].ep_addr, (uint8_t *)&usbd_msc_cfg.csw,
                      sizeof(struct CSW), NULL) != 0) {
        USBD_LOG_ERR("usb write failure\r\n");
    }
}

static void sendLastData(uint8_t *buffer, uint8_t size)
{
    size = MIN(size, usbd_msc_cfg.cbw.dDataLength);

    /* updating the State Machine , so that we send CSW when this
	 * transfer is complete, ie when we get a bulk in callback
	 */
    usbd_msc_cfg.stage = MSC_SEND_CSW;

    if (usbd_ep_write(mass_ep_data[MSD_IN_EP_IDX].ep_addr, buffer, size, NULL) != 0) {
        USBD_LOG_ERR("USB write failed\r\n");
    }

    usbd_msc_cfg.csw.dDataResidue -= size;
    usbd_msc_cfg.csw.bStatus = CSW_STATUS_CMD_PASSED;
}

/**
 * @brief SCSI COMMAND
 */
static bool SCSI_testUnitReady(uint8_t **data, uint32_t *len);
static bool SCSI_requestSense(uint8_t **data, uint32_t *len);
static bool SCSI_inquiry(uint8_t **data, uint32_t *len);
static bool SCSI_startStopUnit(uint8_t **data, uint32_t *len);
static bool SCSI_preventAllowMediaRemoval(uint8_t **data, uint32_t *len);
static bool SCSI_modeSense6(uint8_t **data, uint32_t *len);
static bool SCSI_modeSense10(uint8_t **data, uint32_t *len);
static bool SCSI_readFormatCapacity(uint8_t **data, uint32_t *len);
static bool SCSI_readCapacity10(uint8_t **data, uint32_t *len);
static bool SCSI_read10(uint8_t **data, uint32_t *len);
static bool SCSI_read12(uint8_t **data, uint32_t *len);
static bool SCSI_write10(uint8_t **data, uint32_t *len);
static bool SCSI_write12(uint8_t **data, uint32_t *len);
static bool SCSI_verify10(uint8_t **data, uint32_t *len);

/**
* @brief  SCSI_SenseCode
*         Load the last error code in the error list
* @param  sKey: Sense Key
* @param  ASC: Additional Sense Code
* @retval none

*/
static void SCSI_SenseCode(uint8_t sKey, uint8_t ASC)
{
    usbd_msc_cfg.sKey = sKey;
    usbd_msc_cfg.ASC = ASC;
}

static bool SCSI_processRead(void)
{
    uint32_t transfer_len;

    USBD_LOG_DBG("read addr:%d\r\n", usbd_msc_cfg.scsi_blk_addr);

    transfer_len = MIN(usbd_msc_cfg.scsi_blk_len, MASS_STORAGE_BULK_EP_MPS);

    /* we read an entire block */
    if (!(usbd_msc_cfg.scsi_blk_addr % usbd_msc_cfg.scsi_blk_size)) {
        if (usbd_msc_sector_read((usbd_msc_cfg.scsi_blk_addr / usbd_msc_cfg.scsi_blk_size), usbd_msc_cfg.block_buffer, usbd_msc_cfg.scsi_blk_size) != 0) {
            SCSI_SenseCode(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_UNRECOVERED_READ_ERROR);
            return false;
        }
    }

    usbd_ep_write(mass_ep_data[MSD_IN_EP_IDX].ep_addr,
                  &usbd_msc_cfg.block_buffer[usbd_msc_cfg.scsi_blk_addr % usbd_msc_cfg.scsi_blk_size], transfer_len, NULL);

    usbd_msc_cfg.scsi_blk_addr += transfer_len;
    usbd_msc_cfg.scsi_blk_len -= transfer_len;
    usbd_msc_cfg.csw.dDataResidue -= transfer_len;

    if (usbd_msc_cfg.scsi_blk_len == 0) {
        usbd_msc_cfg.stage = MSC_SEND_CSW;
    }

    return true;
}

static bool SCSI_processWrite(uint8_t *buf, uint16_t len)
{
    USBD_LOG_DBG("write addr:%d\r\n", usbd_msc_cfg.scsi_blk_addr);

    /* we fill an array in RAM of 1 block before writing it in memory */
    for (int i = 0; i < len; i++) {
        usbd_msc_cfg.block_buffer[usbd_msc_cfg.scsi_blk_addr % usbd_msc_cfg.scsi_blk_size + i] = buf[i];
    }

    /* if the array is filled, write it in memory */
    if ((usbd_msc_cfg.scsi_blk_addr % usbd_msc_cfg.scsi_blk_size) + len >= usbd_msc_cfg.scsi_blk_size) {
        if (usbd_msc_sector_write((usbd_msc_cfg.scsi_blk_addr / usbd_msc_cfg.scsi_blk_size), usbd_msc_cfg.block_buffer, usbd_msc_cfg.scsi_blk_size) != 0) {
            SCSI_SenseCode(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_WRITE_FAULT);
            return false;
        }
    }

    usbd_msc_cfg.scsi_blk_addr += len;
    usbd_msc_cfg.scsi_blk_len -= len;
    usbd_msc_cfg.csw.dDataResidue -= len;

    if (usbd_msc_cfg.scsi_blk_len == 0) {
        sendCSW(CSW_STATUS_CMD_PASSED);
    }

    return true;
}

static bool SCSI_processVerify(uint8_t *buf, uint16_t len)
{
    USBD_LOG_DBG("verify addr:%d\r\n", usbd_msc_cfg.scsi_blk_addr);

    /* we read an entire block */
    if (!(usbd_msc_cfg.scsi_blk_addr % usbd_msc_cfg.scsi_blk_size)) {
        if (usbd_msc_sector_read((usbd_msc_cfg.scsi_blk_addr / usbd_msc_cfg.scsi_blk_size), usbd_msc_cfg.block_buffer, usbd_msc_cfg.scsi_blk_size) != 0) {
            SCSI_SenseCode(SCSI_SENSE_HARDWARE_ERROR, SCSI_ASC_UNRECOVERED_READ_ERROR);
            return false;
        }
    }

    /* info are in RAM -> no need to re-read memory */
    for (uint16_t i = 0U; i < len; i++) {
        if (usbd_msc_cfg.block_buffer[usbd_msc_cfg.scsi_blk_addr % usbd_msc_cfg.scsi_blk_size + i] != buf[i]) {
            USBD_LOG_DBG("Mismatch sector %d offset %d",
                         usbd_msc_cfg.scsi_blk_addr / usbd_msc_cfg.scsi_blk_size, i);
            memOK = false;
            break;
        }
    }

    usbd_msc_cfg.scsi_blk_addr += len;
    usbd_msc_cfg.scsi_blk_len -= len;
    usbd_msc_cfg.csw.dDataResidue -= len;

    if (usbd_msc_cfg.scsi_blk_len == 0) {
        sendCSW(CSW_STATUS_CMD_PASSED);
    }

    return true;
}

static bool SCSI_CBWDecode(uint8_t *buf, uint16_t size)
{
    uint8_t send_buffer[64];
    uint8_t *buf2send = send_buffer;
    uint32_t len2send = 0;
    bool ret = false;

    if (size != sizeof(struct CBW)) {
        USBD_LOG_ERR("size != sizeof(cbw)\r\n");
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    memcpy((uint8_t *)&usbd_msc_cfg.cbw, buf, size);

    usbd_msc_cfg.csw.dTag = usbd_msc_cfg.cbw.dTag;
    usbd_msc_cfg.csw.dDataResidue = usbd_msc_cfg.cbw.dDataLength;

    if ((usbd_msc_cfg.cbw.bLUN > 1) || (usbd_msc_cfg.cbw.dSignature != MSC_CBW_Signature) || (usbd_msc_cfg.cbw.bCBLength < 1) || (usbd_msc_cfg.cbw.bCBLength > 16)) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    } else {
        switch (usbd_msc_cfg.cbw.CB[0]) {
            case SCSI_TEST_UNIT_READY:
                ret = SCSI_testUnitReady(&buf2send, &len2send);
                break;
            case SCSI_REQUEST_SENSE:
                ret = SCSI_requestSense(&buf2send, &len2send);
                break;
            case SCSI_INQUIRY:
                ret = SCSI_inquiry(&buf2send, &len2send);
                break;
            case SCSI_START_STOP_UNIT:
                ret = SCSI_startStopUnit(&buf2send, &len2send);
                break;
            case SCSI_PREVENT_ALLOW_MEDIA_REMOVAL:
                ret = SCSI_preventAllowMediaRemoval(&buf2send, &len2send);
                break;
            case SCSI_MODE_SENSE6:
                ret = SCSI_modeSense6(&buf2send, &len2send);
                break;
            case SCSI_MODE_SENSE10:
                ret = SCSI_modeSense10(&buf2send, &len2send);
                break;
            case SCSI_READ_FORMAT_CAPACITIES:
                ret = SCSI_readFormatCapacity(&buf2send, &len2send);
                break;
            case SCSI_READ_CAPACITY10:
                ret = SCSI_readCapacity10(&buf2send, &len2send);
                break;
            case SCSI_READ10:
                ret = SCSI_read10(NULL, 0);
                break;
            case SCSI_READ12:
                ret = SCSI_read12(NULL, 0);
                break;
            case SCSI_WRITE10:
                ret = SCSI_write10(NULL, 0);
                break;
            case SCSI_WRITE12:
                ret = SCSI_write12(NULL, 0);
                break;
            case SCSI_VERIFY10:
                ret = SCSI_verify10(NULL, 0);
                break;

            default:
                SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
                USBD_LOG_WRN("unsupported cmd:0x%02x\r\n", usbd_msc_cfg.cbw.CB[0]);
                ret = false;
                break;
        }
    }
    if (ret) {
        if (usbd_msc_cfg.stage == MSC_READ_CBW) {
            if (len2send) {
                sendLastData(buf2send, len2send);
            } else {
                sendCSW(CSW_STATUS_CMD_PASSED);
            }
        }
    }
    return ret;
}

static bool SCSI_testUnitReady(uint8_t **data, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength != 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }
    *data = NULL;
    *len = 0;
    return true;
}

static bool SCSI_requestSense(uint8_t **data, uint32_t *len)
{
    uint8_t data_len = REQUEST_SENSE_DATA_LEN;
    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    if (usbd_msc_cfg.cbw.CB[4] < REQUEST_SENSE_DATA_LEN) {
        data_len = usbd_msc_cfg.cbw.CB[4];
    }

    uint8_t request_sense[REQUEST_SENSE_DATA_LEN] = {
        0x70,
        0x00,
        0x00, /* Sense Key */
        0x00,
        0x00,
        0x00,
        0x00,
        REQUEST_SENSE_DATA_LEN - 8,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00, /* Additional Sense Code */
        0x00, /* Additional Sense Request */
        0x00,
        0x00,
        0x00,
        0x00,
    };

    request_sense[2] = usbd_msc_cfg.sKey;
    request_sense[12] = usbd_msc_cfg.ASC;
    request_sense[13] = usbd_msc_cfg.ASQ;
#if 0
    request_sense[ 2] = 0x06;           /* UNIT ATTENTION */
    request_sense[12] = 0x28;           /* Additional Sense Code: Not ready to ready transition */
    request_sense[13] = 0x00;           /* Additional Sense Code Qualifier */
#endif
#if 0
    request_sense[ 2] = 0x02;           /* NOT READY */
    request_sense[12] = 0x3A;           /* Additional Sense Code: Medium not present */
    request_sense[13] = 0x00;           /* Additional Sense Code Qualifier */
#endif
#if 0
    request_sense[ 2] = 0x05;         /* ILLEGAL REQUEST */
    request_sense[12] = 0x20;         /* Additional Sense Code: Invalid command */
    request_sense[13] = 0x00;         /* Additional Sense Code Qualifier */
#endif
#if 0
    request_sense[ 2] = 0x00;         /* NO SENSE */
    request_sense[12] = 0x00;         /* Additional Sense Code: No additional code */
    request_sense[13] = 0x00;         /* Additional Sense Code Qualifier */
#endif

    memcpy(*data, (uint8_t *)&request_sense, data_len);
    *len = data_len;
    return true;
}

static bool SCSI_inquiry(uint8_t **data, uint32_t *len)
{
    uint8_t data_len = STANDARD_INQUIRY_DATA_LEN;

    uint8_t inquiry00[6] = {
        0x00,
        0x00,
        0x00,
        (0x06 - 4U),
        0x00,
        0x80
    };

    /* USB Mass storage VPD Page 0x80 Inquiry Data for Unit Serial Number */
    uint8_t inquiry80[8] = {
        0x00,
        0x80,
        0x00,
        0x08,
        0x20, /* Put Product Serial number */
        0x20,
        0x20,
        0x20
    };

    uint8_t inquiry[STANDARD_INQUIRY_DATA_LEN] = {
        /* 36 */

        /* LUN 0 */
        0x00,
        0x80,
        0x02,
        0x02,
        (STANDARD_INQUIRY_DATA_LEN - 5),
        0x00,
        0x00,
        0x00,
        'B', 'o', 'u', 'f', 'f', 'a', 'l', 'o', /* Manufacturer : 8 bytes */
        'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        '0', '.', '0', '1' /* Version      : 4 Bytes */
    };

    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    if ((usbd_msc_cfg.cbw.CB[1] & 0x01U) != 0U) { /* Evpd is set */
        if (usbd_msc_cfg.cbw.CB[2] == 0U) {       /* Request for Supported Vital Product Data Pages*/
            data_len = 0x06;
            memcpy(*data, (uint8_t *)&inquiry00, data_len);
        } else if (usbd_msc_cfg.cbw.CB[2] == 0x80U) { /* Request for VPD page 0x80 Unit Serial Number */
            data_len = 0x08;
            memcpy(*data, (uint8_t *)&inquiry80, data_len);
        } else { /* Request Not supported */
            SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELED_IN_COMMAND);
            return false;
        }
    } else {
        if (usbd_msc_cfg.cbw.CB[4] < STANDARD_INQUIRY_DATA_LEN) {
            data_len = usbd_msc_cfg.cbw.CB[4];
        }
        memcpy(*data, (uint8_t *)&inquiry, data_len);
    }

    *len = data_len;
    return true;
}

static bool SCSI_startStopUnit(uint8_t **data, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength != 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    if ((usbd_msc_cfg.cbw.CB[4] & 0x3U) == 0x1U) /* START=1 */
    {
        //SCSI_MEDIUM_UNLOCKED;
    } else if ((usbd_msc_cfg.cbw.CB[4] & 0x3U) == 0x2U) /* START=0 and LOEJ Load Eject=1 */
    {
        //SCSI_MEDIUM_EJECTED;
    } else if ((usbd_msc_cfg.cbw.CB[4] & 0x3U) == 0x3U) /* START=1 and LOEJ Load Eject=1 */
    {
        //SCSI_MEDIUM_UNLOCKED;
    } else {
    }

    *data = NULL;
    *len = 0;
    return true;
}

static bool SCSI_preventAllowMediaRemoval(uint8_t **data, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength != 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }
    if (usbd_msc_cfg.cbw.CB[4] == 0U) {
        //SCSI_MEDIUM_UNLOCKED;
    } else {
        //SCSI_MEDIUM_LOCKED;
    }
    *data = NULL;
    *len = 0;
    return true;
}

static bool SCSI_modeSense6(uint8_t **data, uint32_t *len)
{
    uint8_t data_len = 4;
    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }
    if (usbd_msc_cfg.cbw.CB[4] < MODE_SENSE6_DATA_LEN) {
        data_len = usbd_msc_cfg.cbw.CB[4];
    }

    uint8_t sense6[] = { 0x03, 0x00, 0x00, 0x00 };

    memcpy(*data, (uint8_t *)&sense6, data_len);
    *len = data_len;
    return true;
}

static bool SCSI_modeSense10(uint8_t **data, uint32_t *len)
{
    uint8_t data_len = MODE_SENSE10_DATA_LEN;
    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }
    if (usbd_msc_cfg.cbw.CB[8] < MODE_SENSE10_DATA_LEN) {
        data_len = usbd_msc_cfg.cbw.CB[8];
    }

    uint8_t sense10[MODE_SENSE10_DATA_LEN] = {
        0x00,
        0x26,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x08,
        0x12,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };
    memcpy(*data, (uint8_t *)&sense10, data_len);
    *len = data_len;
    return true;
}

static bool SCSI_readFormatCapacity(uint8_t **data, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }
    uint8_t capacity[READ_FORMAT_CAPACITY_DATA_LEN] = {
        0x00,
        0x00,
        0x00,
        0x08, /* Capacity List Length */
        (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 24) & 0xff),
        (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 16) & 0xff),
        (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 8) & 0xff),
        (uint8_t)((usbd_msc_cfg.scsi_blk_nbr >> 0) & 0xff),

        0x02, /* Descriptor Code: Formatted Media */
        0x00,
        (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 8) & 0xff),
        (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 0) & 0xff),
    };

    memcpy(*data, (uint8_t *)&capacity, READ_FORMAT_CAPACITY_DATA_LEN);
    *len = READ_FORMAT_CAPACITY_DATA_LEN;
    return true;
}

static bool SCSI_readCapacity10(uint8_t **data, uint32_t *len)
{
    if (usbd_msc_cfg.cbw.dDataLength == 0U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    uint8_t capacity[READ_CAPACITY10_DATA_LEN] = {
        (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 24) & 0xff),
        (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 16) & 0xff),
        (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 8) & 0xff),
        (uint8_t)(((usbd_msc_cfg.scsi_blk_nbr - 1) >> 0) & 0xff),

        (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 24) & 0xff),
        (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 16) & 0xff),
        (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 8) & 0xff),
        (uint8_t)((usbd_msc_cfg.scsi_blk_size >> 0) & 0xff),
    };

    memcpy(*data, (uint8_t *)&capacity, READ_CAPACITY10_DATA_LEN);
    *len = READ_CAPACITY10_DATA_LEN;
    return true;
}

static bool SCSI_read10(uint8_t **data, uint32_t *len)
{
    /* Logical Block Address of First Block */
    uint32_t lba = 0;
    uint32_t blk_num = 0;
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x80U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    lba = GET_BE32(&usbd_msc_cfg.cbw.CB[2]);
    USBD_LOG_DBG("lba: 0x%x\r\n", lba);

    usbd_msc_cfg.scsi_blk_addr = lba * usbd_msc_cfg.scsi_blk_size;

    /* Number of Blocks to transfer */
    blk_num = GET_BE16(&usbd_msc_cfg.cbw.CB[7]);

    USBD_LOG_DBG("num (block) : 0x%x\r\n", blk_num);
    usbd_msc_cfg.scsi_blk_len = blk_num * usbd_msc_cfg.scsi_blk_size;

    if ((lba + blk_num) > usbd_msc_cfg.scsi_blk_nbr) {
        USBD_LOG_ERR("LBA out of range\r\n");
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_ADDRESS_OUT_OF_RANGE);
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != usbd_msc_cfg.scsi_blk_len) {
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_IN;
    return SCSI_processRead();
}

static bool SCSI_read12(uint8_t **data, uint32_t *len)
{
    /* Logical Block Address of First Block */
    uint32_t lba = 0;
    uint32_t blk_num = 0;
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x80U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    lba = GET_BE32(&usbd_msc_cfg.cbw.CB[2]);
    USBD_LOG_DBG("lba: 0x%x\r\n", lba);

    usbd_msc_cfg.scsi_blk_addr = lba * usbd_msc_cfg.scsi_blk_size;

    /* Number of Blocks to transfer */
    blk_num = GET_BE32(&usbd_msc_cfg.cbw.CB[6]);

    USBD_LOG_DBG("num (block) : 0x%x\r\n", blk_num);
    usbd_msc_cfg.scsi_blk_len = blk_num * usbd_msc_cfg.scsi_blk_size;

    if ((lba + blk_num) > usbd_msc_cfg.scsi_blk_nbr) {
        USBD_LOG_ERR("LBA out of range\r\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != usbd_msc_cfg.scsi_blk_len) {
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_IN;
    return SCSI_processRead();
}

static bool SCSI_write10(uint8_t **data, uint32_t *len)
{
    /* Logical Block Address of First Block */
    uint32_t lba = 0;
    uint32_t blk_num = 0;
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x00U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    lba = GET_BE32(&usbd_msc_cfg.cbw.CB[2]);
    USBD_LOG_DBG("lba: 0x%x\r\n", lba);

    usbd_msc_cfg.scsi_blk_addr = lba * usbd_msc_cfg.scsi_blk_size;

    /* Number of Blocks to transfer */
    blk_num = GET_BE16(&usbd_msc_cfg.cbw.CB[7]);

    USBD_LOG_DBG("num (block) : 0x%x\r\n", blk_num);
    usbd_msc_cfg.scsi_blk_len = blk_num * usbd_msc_cfg.scsi_blk_size;

    if ((lba + blk_num) > usbd_msc_cfg.scsi_blk_nbr) {
        USBD_LOG_ERR("LBA out of range\r\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != usbd_msc_cfg.scsi_blk_len) {
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_OUT;
    return true;
}

static bool SCSI_write12(uint8_t **data, uint32_t *len)
{
    /* Logical Block Address of First Block */
    uint32_t lba = 0;
    uint32_t blk_num = 0;
    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x00U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    lba = GET_BE32(&usbd_msc_cfg.cbw.CB[2]);
    USBD_LOG_DBG("lba: 0x%x\r\n", lba);

    usbd_msc_cfg.scsi_blk_addr = lba * usbd_msc_cfg.scsi_blk_size;

    /* Number of Blocks to transfer */
    blk_num = GET_BE32(&usbd_msc_cfg.cbw.CB[6]);

    USBD_LOG_DBG("num (block) : 0x%x\r\n", blk_num);
    usbd_msc_cfg.scsi_blk_len = blk_num * usbd_msc_cfg.scsi_blk_size;

    if ((lba + blk_num) > usbd_msc_cfg.scsi_blk_nbr) {
        USBD_LOG_ERR("LBA out of range\r\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != usbd_msc_cfg.scsi_blk_len) {
        return false;
    }
    usbd_msc_cfg.stage = MSC_DATA_OUT;
    return true;
}

static bool SCSI_verify10(uint8_t **data, uint32_t *len)
{
    /* Logical Block Address of First Block */
    uint32_t lba = 0;
    uint32_t blk_num = 0;

    if ((usbd_msc_cfg.cbw.CB[1] & 0x02U) == 0x00U) {
        return true;
    }

    if (((usbd_msc_cfg.cbw.bmFlags & 0x80U) != 0x00U) || (usbd_msc_cfg.cbw.dDataLength == 0U)) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_CDB);
        return false;
    }

    if ((usbd_msc_cfg.cbw.CB[1] & 0x02U) == 0x02U) {
        SCSI_SenseCode(SCSI_SENSE_ILLEGAL_REQUEST, SCSI_ASC_INVALID_FIELED_IN_COMMAND);
        return false; /* Error, Verify Mode Not supported*/
    }

    lba = GET_BE32(&usbd_msc_cfg.cbw.CB[2]);
    USBD_LOG_DBG("lba: 0x%x\r\n", lba);

    usbd_msc_cfg.scsi_blk_addr = lba * usbd_msc_cfg.scsi_blk_size;

    /* Number of Blocks to transfer */
    blk_num = GET_BE16(&usbd_msc_cfg.cbw.CB[7]);

    USBD_LOG_DBG("num (block) : 0x%x\r\n", blk_num);
    usbd_msc_cfg.scsi_blk_len = blk_num * usbd_msc_cfg.scsi_blk_size;

    if ((lba + blk_num) > usbd_msc_cfg.scsi_blk_nbr) {
        USBD_LOG_ERR("LBA out of range\r\n");
        return false;
    }

    if (usbd_msc_cfg.cbw.dDataLength != usbd_msc_cfg.scsi_blk_len) {
        return false;
    }

    memOK = true;
    usbd_msc_cfg.stage = MSC_DATA_OUT;
    return true;
}

static void mass_storage_bulk_out(uint8_t ep)
{
    uint8_t out_buf[MASS_STORAGE_BULK_EP_MPS];
    uint32_t bytes_read;

    usbd_ep_read(ep, out_buf, MASS_STORAGE_BULK_EP_MPS,
                 &bytes_read);

    switch (usbd_msc_cfg.stage) {
        case MSC_READ_CBW:
            if (SCSI_CBWDecode(out_buf, bytes_read) == false) {
                USBD_LOG_ERR("Command:0x%02x decode err\r\n", usbd_msc_cfg.cbw.CB[0]);
                usbd_msc_bot_abort();
                return;
            }
            break;
        /* last command is write10 or write12,and has caculated blk_addr and blk_len,so the device start reading data from host*/
        case MSC_DATA_OUT:
            switch (usbd_msc_cfg.cbw.CB[0]) {
                case SCSI_WRITE10:
                case SCSI_WRITE12:
                    if (SCSI_processWrite(out_buf, bytes_read) == false) {
                        sendCSW(CSW_STATUS_CMD_FAILED); /* send fail status to host,and the host will retry*/
                        //return;
                    }
                    break;
                case SCSI_VERIFY10:
                    if (SCSI_processVerify(out_buf, bytes_read) == false) {
                        sendCSW(CSW_STATUS_CMD_FAILED); /* send fail status to host,and the host will retry*/
                        //return;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    /*set ep ack to recv next data*/
    usbd_ep_read(ep, NULL, 0, NULL);
}

/**
 * @brief EP Bulk IN handler, used to send data to the Host
 *
 * @param ep        Endpoint address.
 * @param ep_status Endpoint status code.
 *
 * @return  N/A.
 */
static void mass_storage_bulk_in(uint8_t ep)
{
    switch (usbd_msc_cfg.stage) {
        /* last command is read10 or read12,and has caculated blk_addr and blk_len,so the device has to send remain data to host*/
        case MSC_DATA_IN:
            switch (usbd_msc_cfg.cbw.CB[0]) {
                case SCSI_READ10:
                case SCSI_READ12:
                    if (SCSI_processRead() == false) {
                        sendCSW(CSW_STATUS_CMD_FAILED); /* send fail status to host,and the host will retry*/
                        return;
                    }
                    break;
                default:
                    break;
            }

            break;

        /*the device has to send a CSW*/
        case MSC_SEND_CSW:
            sendCSW(CSW_STATUS_CMD_PASSED);
            break;

        /*the host has received the CSW*/
        case MSC_WAIT_CSW:
            usbd_msc_cfg.stage = MSC_READ_CBW;
            break;

        default:
            break;
    }
}

void msc_storage_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
        case USB_EVENT_RESET:
            usbd_msc_reset();
            break;

        default:
            break;
    }
}

static usbd_class_t msc_class;

static usbd_interface_t msc_intf = {
    .class_handler = msc_storage_class_request_handler,
    .vendor_handler = NULL,
    .notify_handler = msc_storage_notify_handler,
};

void usbd_msc_class_init(uint8_t out_ep, uint8_t in_ep)
{
    msc_class.name = "usbd_msc";

    usbd_class_register(&msc_class);
    usbd_class_add_interface(&msc_class, &msc_intf);

    mass_ep_data[0].ep_addr = out_ep;
    mass_ep_data[0].ep_cb = mass_storage_bulk_out;
    mass_ep_data[1].ep_addr = in_ep;
    mass_ep_data[1].ep_cb = mass_storage_bulk_in;

    usbd_interface_add_endpoint(&msc_intf, &mass_ep_data[0]);
    usbd_interface_add_endpoint(&msc_intf, &mass_ep_data[1]);
}