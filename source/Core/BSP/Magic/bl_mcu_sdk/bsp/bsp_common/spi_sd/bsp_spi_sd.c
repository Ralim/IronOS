/**
 * @file bsp_spi_sd.c
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
#include "bflb_platform.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "bsp_spi_sd.h"

static struct device *spi0;
static struct device *dma_ch3;
static struct device *dma_ch4;

SD_CardInfoTypedef SD_CardInfo = { 0 };

uint8_t SD_SPI_Init(void)
{
    gpio_set_mode(SPI_PIN_CS, GPIO_OUTPUT_MODE);
    gpio_write(SPI_PIN_CS, 1);

    spi0 = device_find("spi0");

    if (spi0) {
        device_close(spi0);
    } else {
        spi_register(SPI0_INDEX, "spi0");
        spi0 = device_find("spi0");
    }

    if (spi0) {
        device_open(spi0, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
    }

    dma_ch3 = device_find("dma0_ch3");

    if (dma_ch3) {
        device_close(dma_ch3);
    } else {
        dma_register(DMA0_CH3_INDEX, "dma0_ch3");
        dma_ch3 = device_find("dma0_ch3");
    }

    if (dma_ch3) {
        DMA_DEV(dma_ch3)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch3)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch3)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch3)->dst_req = DMA_REQUEST_SPI0_TX;
        DMA_DEV(dma_ch3)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch3)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch3)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch3)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch3, 0);
        device_control(dma_ch3, DEVICE_CTRL_CLR_INT, NULL);
    }

    dma_ch4 = device_find("dma0_ch4");

    if (dma_ch4) {
        device_close(dma_ch4);
    } else {
        dma_register(DMA0_CH4_INDEX, "dma0_ch4");
        dma_ch4 = device_find("dma0_ch4");
    }

    if (dma_ch4) {
        DMA_DEV(dma_ch4)->direction = DMA_PERIPH_TO_MEMORY;
        DMA_DEV(dma_ch4)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch4)->src_req = DMA_REQUEST_SPI0_RX;
        DMA_DEV(dma_ch4)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch4)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch4)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch4)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch4)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch4)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch4)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch4, 0);
        device_control(dma_ch4, DEVICE_CTRL_CLR_INT, NULL);
    }

    return SUCCESS;
}

BL_Err_Type SPI_ReadWriteByte(uint8_t *txBuff, uint8_t *rxBuff, uint32_t length)
{
    while (dma_channel_check_busy(dma_ch3) || dma_channel_check_busy(dma_ch4))
        ;

    if (length < 500) {
        spi_transmit_receive(spi0, txBuff, rxBuff, length, SPI_DATASIZE_8BIT);
    } else {
        device_control(spi0, DEVICE_CTRL_TX_DMA_RESUME, NULL);
        device_control(spi0, DEVICE_CTRL_RX_DMA_RESUME, NULL);

        dma_reload(dma_ch4, (uint32_t)DMA_ADDR_SPI_RDR, (uint32_t)rxBuff, length);
        dma_reload(dma_ch3, (uint32_t)txBuff, (uint32_t)DMA_ADDR_SPI_TDR, length);
        dma_channel_start(dma_ch4);
        dma_channel_start(dma_ch3);

        while (dma_channel_check_busy(dma_ch3) || dma_channel_check_busy(dma_ch4))
            ;

        device_control(spi0, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
        device_control(spi0, DEVICE_CTRL_RX_DMA_SUSPEND, NULL);
    }

    return SUCCESS;
}
void SPI_CS_WriteBit(uint8_t bit)
{
    gpio_write(SPI_PIN_CS, bit);
}
/****************************************************************************
 * @brief  SPI_SetSpeed
 *
 *
*******************************************************************************/
static void SD_SPI_SetSpeed(uint8_t mode)
{
    switch (mode) {
        case (0):
            device_control(spi0, DEVICE_CTRL_SPI_CONFIG_CLOCK, (void *)(300 * 1000));
            break;

        case (1):
            device_control(spi0, DEVICE_CTRL_SPI_CONFIG_CLOCK, (void *)(18 * 1000 * 1000));
            break;

        case (2):
            device_control(spi0, DEVICE_CTRL_SPI_CONFIG_CLOCK, (void *)(40 * 1000 * 1000));
            break;

        default:
            break;
    }
}

/****************************************************************************
 * @brief  SPI_CS_WriteBit
 *
 *
*******************************************************************************/
// static void SPI_CS_WriteBit(uint8_t bit)
// {
//     GLB_GPIO_Write(SPI_PIN_CS, bit);
// }

/****************************************************************************/ /**
 * @brief  SD_GetResponse
 *
 *
*******************************************************************************/
static BL_Err_Type SD_GetResponse(uint8_t Response)
{
    uint8_t rx;
    uint8_t t_0xFF = 0xFF;
    uint16_t Count = 0xFFFE;

    do {
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);
        Count--;
    } while (rx != Response && Count);

    if (Count == 0) {
        return ERROR;
    } else {
        return SUCCESS;
    }
}

/****************************************************************************
 * @brief  SD_SendCommand
 *
 *
*******************************************************************************/
static uint8_t SD_SendCommand(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t rx;
    uint8_t t_0xFF = 0xFF;
    uint8_t rxbuff[6];
    uint8_t txbuff[6];
    uint8_t retry = 0;

    SPI_CS_WriteBit(1);
    SPI_ReadWriteByte(&t_0xFF, &rx, 1);

    SPI_CS_WriteBit(0);
    SD_GetResponse(0xFF);

    txbuff[0] = cmd | 0x40;
    txbuff[1] = (uint8_t)(arg >> 24);
    txbuff[2] = (uint8_t)(arg >> 16);
    txbuff[3] = (uint8_t)(arg >> 8);
    txbuff[4] = (uint8_t)(arg);
    txbuff[5] = (uint8_t)(crc);

    SPI_ReadWriteByte(txbuff, rxbuff, 6);

    SPI_ReadWriteByte(&t_0xFF, &rx, 1);

    do {
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);
        retry++;
    } while (rx == 0xFF && retry < 20);

    //MSG("CMD%d :%d num:%d\r\n",cmd,rx,retry);
    SPI_CS_WriteBit(1);
    SPI_ReadWriteByte(&t_0xFF, &retry, 1);

    return rx;
}

/****************************************************************************
 * @brief  SD_SendCommand_NoDeassert
 *
 *
*******************************************************************************/
static uint8_t SD_SendCommand_NoDeassert(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t rx;
    uint8_t t_0xFF = 0xFF;
    uint8_t rxbuff[6];
    uint8_t txbuff[6];
    uint8_t retry = 0;

    SPI_CS_WriteBit(1);
    SPI_ReadWriteByte(&t_0xFF, &rx, 1);

    SPI_CS_WriteBit(0);
    SD_GetResponse(0xFF);

    txbuff[0] = cmd | 0x40;
    txbuff[1] = (uint8_t)(arg >> 24);
    txbuff[2] = (uint8_t)(arg >> 16);
    txbuff[3] = (uint8_t)(arg >> 8);
    txbuff[4] = (uint8_t)(arg);
    txbuff[5] = (uint8_t)(crc);

    SPI_ReadWriteByte(txbuff, rxbuff, 6);

    SPI_ReadWriteByte(&t_0xFF, &rx, 1);

    do {
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);
        retry++;
    } while (rx == 0xFF && retry < 20);

    //MSG("CMD%d :%d num:%d\r\n",cmd,rx,retry);

    return rx;
}

/****************************************************************************
 * @brief  SD_Idle_Sta
 *
 *
*******************************************************************************/
static BL_Err_Type SD_Idle_Sta(void)
{
    uint16_t i;
    uint8_t rx = 0;
    uint8_t t_0xFF = 0xFF;
    uint8_t retry = 0;

    SPI_CS_WriteBit(1);

    for (i = 0; i < 10; i++) {
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);
    }

    do {
        rx = SD_SendCommand(CMD0, 0, 0x95);
        retry++;
    } while (rx != MSD_IN_IDLE_STATE && retry < 100);

    if (retry >= 100) {
        //MSG("SD IDLE err:%d\r\n",rx);
        return ERROR;
    } else {
        //MSG("SD IDLE success\r\n");
        return SUCCESS;
    }
}

/****************************************************************************/ /**
 * @brief  SD_ReceiveData
 *
 *
 *
*******************************************************************************/
uint8_t SD_ReceiveData(uint8_t *data, uint16_t len, uint8_t release)
{
    uint8_t rx;
    uint8_t t_0xFF = 0xFF;
    uint16_t i;

    SPI_CS_WriteBit(0);

    /* get token */
    if (SD_GetResponse(0xFE)) {
        SPI_CS_WriteBit(1);
        MSG("sd receive err: no token\r\n");
        return ERROR;
    } else {
        //MSG("sd get token\r\n");
    }

    for (i = 0; i < len; i++) {
        SPI_ReadWriteByte(&t_0xFF, data + i, 1);
    }

    /* two dummy CRC */
    SPI_ReadWriteByte(&t_0xFF, &rx, 1);
    SPI_ReadWriteByte(&t_0xFF, &rx, 1);

    /* */
    if (release) {
        SPI_CS_WriteBit(1);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  SD_SendBlock
 *
 *
 *
*******************************************************************************/
uint8_t SD_SendBlock(uint8_t *buf, uint8_t cmd)
{
    uint8_t rx;
    uint8_t rxbuff[512];
    uint8_t t_0xFF = 0xFF;
    uint16_t i;

    /* Waiti for free */
    if (SD_GetResponse(0xFF)) {
        SPI_CS_WriteBit(1);
        return ERROR;
    }

    /* send token */
    SPI_ReadWriteByte(&cmd, &rx, 1);

    if (cmd != 0xFD) {
        /* send block data */
        SPI_ReadWriteByte(buf, rxbuff, 512);
        /* two dummy CRC */
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);
        /* 8 clock */
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);

        if ((rx & 0x1F) != 0x05) {
            //MSG("sd send err:%d\r\n",rx,1);
            SPI_CS_WriteBit(1);
            return rx;
        }

        i = 0;

        do {
            SPI_ReadWriteByte(&t_0xFF, &rx, 1);
            i++;
        } while (rx != 0xFF && i <= 0xFFFE);
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  SD_Get_CardInfo
 *
 *
*******************************************************************************/
uint8_t SD_Get_CardInfo(SD_CardInfoTypedef *pCardInfo)
{
    uint8_t rx, t_0xFF;
    uint32_t tmp = 0;
    /* get cid */
    rx = SD_SendCommand(CMD10, 0, 0xFF);

    if (rx) {
        return 1;
    }

    SD_ReceiveData(pCardInfo->SD_cid.CID, 16, RELEASE);
    SPI_ReadWriteByte(&t_0xFF, &rx, 1);
    /* get csd */
    rx = SD_SendCommand(CMD9, 0, 0xFF);

    if (rx) {
        return 1;
    }

    SD_ReceiveData(pCardInfo->SD_csd.CSD, 16, RELEASE);

    /* Byte 0 */
    tmp = pCardInfo->SD_csd.CSD[0];
    pCardInfo->SD_csd.CSDStruct = (uint8_t)((tmp & 0xC0) >> 6);
    pCardInfo->SD_csd.SysSpecVersion = (uint8_t)((tmp & 0x3C) >> 2);
    pCardInfo->SD_csd.Reserved1 = tmp & 0x03;
    /* Byte 1 */
    tmp = pCardInfo->SD_csd.CSD[1];
    pCardInfo->SD_csd.TAAC = (uint8_t)tmp;
    /* Byte 2 */
    tmp = pCardInfo->SD_csd.CSD[2];
    pCardInfo->SD_csd.NSAC = (uint8_t)tmp;
    /* Byte 3 */
    tmp = pCardInfo->SD_csd.CSD[3];
    pCardInfo->SD_csd.MaxBusClkFrec = (uint8_t)tmp;
    /* Byte 4 */
    tmp = pCardInfo->SD_csd.CSD[4];
    pCardInfo->SD_csd.CardComdClasses = (uint16_t)(tmp << 4);
    /* Byte 5 */
    tmp = pCardInfo->SD_csd.CSD[5];
    pCardInfo->SD_csd.CardComdClasses |= (uint16_t)((tmp & 0xF0) >> 4);
    pCardInfo->SD_csd.RdBlockLen = (uint8_t)(tmp & 0x0F);
    /* Byte 6 */
    tmp = pCardInfo->SD_csd.CSD[6];
    pCardInfo->SD_csd.PartBlockRead = (uint8_t)((tmp & 0x80) >> 7);
    pCardInfo->SD_csd.WrBlockMisalign = (uint8_t)((tmp & 0x40) >> 6);
    pCardInfo->SD_csd.RdBlockMisalign = (uint8_t)((tmp & 0x20) >> 5);
    pCardInfo->SD_csd.DSRImpl = (uint8_t)((tmp & 0x10) >> 4);
    pCardInfo->SD_csd.Reserved2 = 0; /*!< Reserved */

    if ((pCardInfo->CardType == SD_TYPE_V1) || (pCardInfo->CardType == SD_TYPE_V2)) {
        pCardInfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;
        /* Byte 7 */
        tmp = pCardInfo->SD_csd.CSD[7];
        pCardInfo->SD_csd.DeviceSize |= (tmp) << 2;
        /* Byte 8 */
        tmp = pCardInfo->SD_csd.CSD[8];
        pCardInfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;
        pCardInfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
        pCardInfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);
        /* Byte 9 */
        tmp = pCardInfo->SD_csd.CSD[9];
        pCardInfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
        pCardInfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
        pCardInfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1;
        /* Byte 10 */
        tmp = pCardInfo->SD_csd.CSD[10];
        pCardInfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;

        pCardInfo->CardCapacity = (pCardInfo->SD_csd.DeviceSize + 1);
        pCardInfo->CardCapacity *= (1 << (pCardInfo->SD_csd.DeviceSizeMul + 2));
        pCardInfo->CardBlockSize = 1 << (pCardInfo->SD_csd.RdBlockLen);
        pCardInfo->CardCapacity *= pCardInfo->CardBlockSize;
    } else if (pCardInfo->CardType == SD_TYPE_V2HC) {
        /* Byte 7 */
        tmp = pCardInfo->SD_csd.CSD[7];
        pCardInfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;
        /* Byte 8 */
        tmp = pCardInfo->SD_csd.CSD[8];
        pCardInfo->SD_csd.DeviceSize |= (tmp << 8);
        /* Byte 9 */
        tmp = pCardInfo->SD_csd.CSD[9];
        pCardInfo->SD_csd.DeviceSize |= (tmp);
        /* Byte 10 */
        tmp = pCardInfo->SD_csd.CSD[10];

        pCardInfo->CardCapacity = (uint64_t)((((uint64_t)pCardInfo->SD_csd.DeviceSize + 1)) * 512 * 1024);
        pCardInfo->CardBlockSize = 512;
    } else {
        /* Not supported card type */
    }

    pCardInfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
    pCardInfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;
    /* Byte 11 */
    tmp = pCardInfo->SD_csd.CSD[11];
    pCardInfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
    pCardInfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);
    /* Byte 12 */
    tmp = pCardInfo->SD_csd.CSD[12];
    pCardInfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
    pCardInfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
    pCardInfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
    pCardInfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;
    /* Byte 13 */
    tmp = pCardInfo->SD_csd.CSD[13];
    pCardInfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
    pCardInfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
    pCardInfo->SD_csd.Reserved3 = 0;
    pCardInfo->SD_csd.ContentProtectAppli = (tmp & 0x01);
    /* Byte 14 */
    tmp = pCardInfo->SD_csd.CSD[14];
    pCardInfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
    pCardInfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
    pCardInfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
    pCardInfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
    pCardInfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
    pCardInfo->SD_csd.ECC = (tmp & 0x03);
    /* Byte 15 */
    tmp = pCardInfo->SD_csd.CSD[15];
    pCardInfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
    pCardInfo->SD_csd.Reserved4 = 1;

    /* Byte 0 */
    tmp = pCardInfo->SD_cid.CID[0];
    pCardInfo->SD_cid.ManufacturerID = tmp;
    /* Byte 1 */
    tmp = pCardInfo->SD_cid.CID[1];
    pCardInfo->SD_cid.OEM_AppliID = tmp << 8;
    /* Byte 2 */
    tmp = pCardInfo->SD_cid.CID[2];
    pCardInfo->SD_cid.OEM_AppliID |= tmp;
    /* Byte 3 */
    tmp = pCardInfo->SD_cid.CID[3];
    pCardInfo->SD_cid.ProdName1 = tmp << 24;
    /* Byte 4 */
    tmp = pCardInfo->SD_cid.CID[4];
    pCardInfo->SD_cid.ProdName1 |= tmp << 16;
    /* Byte 5 */
    tmp = pCardInfo->SD_cid.CID[5];
    pCardInfo->SD_cid.ProdName1 |= tmp << 8;
    /* Byte 6 */
    tmp = pCardInfo->SD_cid.CID[6];
    pCardInfo->SD_cid.ProdName1 |= tmp;
    /* Byte 7 */
    tmp = pCardInfo->SD_cid.CID[7];
    pCardInfo->SD_cid.ProdName2 = tmp;
    /* Byte 8 */
    tmp = pCardInfo->SD_cid.CID[8];
    pCardInfo->SD_cid.ProdRev = tmp;
    /* Byte 9 */
    tmp = pCardInfo->SD_cid.CID[9];
    pCardInfo->SD_cid.ProdSN = tmp << 24;
    /* Byte 10 */
    tmp = pCardInfo->SD_cid.CID[10];
    pCardInfo->SD_cid.ProdSN |= tmp << 16;
    /* Byte 11 */
    tmp = pCardInfo->SD_cid.CID[11];
    pCardInfo->SD_cid.ProdSN |= tmp << 8;
    /* Byte 12 */
    tmp = pCardInfo->SD_cid.CID[12];
    pCardInfo->SD_cid.ProdSN |= tmp;
    /* Byte 13 */
    tmp = pCardInfo->SD_cid.CID[13];
    pCardInfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
    pCardInfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;
    /* Byte 14 */
    tmp = pCardInfo->SD_cid.CID[14];
    pCardInfo->SD_cid.ManufactDate |= tmp;
    /* Byte 15 */
    tmp = pCardInfo->SD_cid.CID[16];
    pCardInfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
    pCardInfo->SD_cid.Reserved2 = 1;
    return 0;
}

/****************************************************************************/ /**
 * @brief  SD_ReadSingleBlock
 *
 *
*******************************************************************************/
uint8_t SD_ReadBlock(uint32_t sector, uint8_t *buffer, uint32_t num)
{
    uint8_t rx;
    uint8_t t_0xFF;
    SPI_CS_WriteBit(0);

    /* sector to byte */
    if (SD_CardInfo.CardType != SD_TYPE_V2HC) {
        sector = sector << 9;
    }

    if (num == 1) {
        rx = SD_SendCommand(CMD17, sector, 0);

        if (rx) {
            MSG("SD CMD17 err:%02X\r\n", rx);
            return rx;
        }

        rx = SD_ReceiveData(buffer, 512, RELEASE);

        if (rx != 0) {
            MSG("SD sing read err:%02X\r\n", rx);
            return rx;
        }
    } else {
        rx = SD_SendCommand(CMD18, sector, 0);

        if (rx) {
            MSG("SD CMD18 err:%02X\r\n", rx);
            return rx;
        }

        while (num--) {
            rx = SD_ReceiveData(buffer, 512, NO_RELEASE);

            if (rx != 0) {
                MSG("SD read err:%02X\r\n", rx);
                return rx;
            }

            buffer += 512;
        }

        rx = SD_SendCommand(CMD12, 0, 0);
        SPI_CS_WriteBit(1);
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);
    }

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  SD_WriteSingleBlock
 *
 *
*******************************************************************************/
uint8_t SD_WriteBlock(uint32_t sector, uint8_t *data, uint32_t num)
{
    uint8_t rx;
    uint8_t t_0xFF;

    /* sector to byte */
    if (SD_CardInfo.CardType != SD_TYPE_V2HC) {
        sector = sector << 9;
    }

    if (SD_GetResponse(0xFF)) {
        return 1;
    }

    if (num == 1) {
        rx = SD_SendCommand(CMD24, sector, 1);

        if (rx) {
            MSG("SD CMD24 err:%d\r\n", rx);
            return rx;
        }

        SPI_CS_WriteBit(0);
        rx = SD_SendBlock(data, 0xFE);

        if (rx) {
            MSG("write err\r\n");
            SPI_CS_WriteBit(0);
            return rx;
        }
    } else if (num > 1) {
        if (SD_CardInfo.CardType != SD_TYPE_MMC) {
            rx = SD_SendCommand(CMD55, 0, 1);

            if (rx == 0) {
                rx = SD_SendCommand(ACMD23, num, 1);
            }

            if (rx != 0) {
                MSG("SD ACMD23 err:%d\r\n", rx);
                return rx;
            }
        }

        rx = SD_SendCommand(CMD25, sector, 1);

        if (rx != 0) {
            MSG("SD ACMD25 err:%d\r\n", rx);
            return rx;
        }

        SPI_CS_WriteBit(0);

        do {
            rx = SD_SendBlock(data, 0xFC);
            data += 512;
        } while (--num && rx == 0);

        rx = SD_SendBlock(data, 0xFD);
    }

    SPI_CS_WriteBit(1);
    SPI_ReadWriteByte(&t_0xFF, &rx, 1);
    return 0;
}
/****************************************************************************/ /**
 * @brief  SD init
 *
 *
 *
 *
*******************************************************************************/
uint8_t SD_Init(SD_CardInfoTypedef *pCardInfo)
{
    uint8_t rx;
    uint8_t t_0xFF = 0xFF;
    //uint8_t txbuff[6];
    uint8_t rxbuff[6];
    uint16_t retry = 0, i;

    SD_SPI_Init();

    SPI_CS_WriteBit(1);
    /* low speed */
    SD_SPI_SetSpeed(0);

    if (SD_Idle_Sta()) {
        MSG("SD IDLE err:%d\r\n", rx);
        return ERROR;
    } else {
        MSG("SD IDLE success\r\n");
    }

    /* send CMD8 */
    rx = SD_SendCommand_NoDeassert(CMD8, 0x1aa, 0x87);

    /* sd card v1.0 */
    if (rx == 0x05) {
        pCardInfo->CardType = SD_TYPE_V1;
        SPI_CS_WriteBit(1);
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);

        /* send CMD55+CMD41  sd init */
        do {
            rx = SD_SendCommand(CMD55, 0, 0);

            if (rx != 0x01) {
                MSG("sdv1 CMD55 err:%d\r\n", rx);
                return rx;
            }

            rx = SD_SendCommand(ACMD41, 0, 0);
            retry++;

        } while (rx != 0x00 && retry < 100);

        /* mmc card init */
        if (retry >= 100) {
            retry = 0;

            do {
                rx = SD_SendCommand(CMD1, 0, 0);
                retry++;
            } while (rx != 0x00 && retry < 100);

            if (retry >= 100) {
                MSG("sdv2 CMD1 timeout err:%d\r\n", rx);
                return ERROR;
            } else {
                pCardInfo->CardType = SD_TYPE_MMC;
            }
        }

        /* high speed */
        SD_SPI_SetSpeed(1);
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);

        /* set sector size */
        rx = SD_SendCommand(CMD16, 512, 0xff);

        if (rx != 0x00) {
            MSG("sdv1 CMD16 err:%d\r\n", rx);
            return rx;
        }
    }
    /* sd card v2.0 */
    else if (rx == 0x01) {
        SPI_ReadWriteByte(&t_0xFF, rxbuff + 0, 1);
        SPI_ReadWriteByte(&t_0xFF, rxbuff + 1, 1);
        SPI_ReadWriteByte(&t_0xFF, rxbuff + 2, 1);
        SPI_ReadWriteByte(&t_0xFF, rxbuff + 3, 1);

        SPI_CS_WriteBit(1);
        /* next 8 clocks */
        SPI_ReadWriteByte(&t_0xFF, &rx, 1);

        /* 2.7V - 3.6V */
        if (rxbuff[2] == 0x01 && rxbuff[3] == 0xAA) {
            retry = 0;

            do {
                rx = SD_SendCommand(CMD55, 0, 0);

                if (rx != 0x01) {
                    MSG("sdv2 CMD55 err:%d\r\n", rx);
                    return rx;
                }

                rx = SD_SendCommand(ACMD41, 0x40000000, 0);
                retry++;

                if (retry > 200) {
                    MSG("sdv2 CMD41 timeout err:%d\r\n", rx);
                    return rx;
                }
            } while (rx != 0);

            /* send CMD58 */
            rx = SD_SendCommand_NoDeassert(CMD58, 0, 0);

            if (rx != 0X00) {
                MSG("sdv2 CMD58 err:%d\r\n", rx);
                return rx;
            }

            /* get OCR */
            SPI_ReadWriteByte(&t_0xFF, rxbuff + 0, 1);
            SPI_ReadWriteByte(&t_0xFF, rxbuff + 1, 1);
            SPI_ReadWriteByte(&t_0xFF, rxbuff + 2, 1);
            SPI_ReadWriteByte(&t_0xFF, rxbuff + 3, 1);

            SPI_CS_WriteBit(1);
            SPI_ReadWriteByte(&t_0xFF, &rx, 1);

            /* bit32 CCS: 1-SDHC  0-SD2.0 */
            if (rxbuff[0] & 0x40) {
                pCardInfo->CardType = SD_TYPE_V2HC;
                MSG("sdv2hc success\r\n");
            } else {
                pCardInfo->CardType = SD_TYPE_V2;
                MSG("sdv2.0 success\r\n");
            }

            /* set sector size */
            rx = SD_SendCommand(CMD16, 512, 0xff);

            if (rx != 0x00) {
                MSG("sdv1 CMD16 err:%d\r\n", rx);
            }

            /* high speed */
            SD_SPI_SetSpeed(1);
            SPI_ReadWriteByte(&t_0xFF, &rx, 1);
        }
    } else {
        return 1;
    }

    MSG("sd init success\r\n");

    SD_Get_CardInfo(pCardInfo);

    MSG(" sd csd:");

    for (i = 0; i < 4; i++) {
        MSG("%X ", pCardInfo->SD_csd.CSD[i]);
    }

    MSG("end\r\n sd cid:");

    for (i = 0; i < 4; i++) {
        MSG("%X ", pCardInfo->SD_cid.CID[i]);
    }

    MSG("end\r\n");
    MSG("SDHC CardBlockSize:%d Byte\r\n", (pCardInfo->CardBlockSize));
    MSG("SDHC CardCapacity:%lld MByte\r\n", (SD_CardInfo.CardCapacity / 1024 / 1024));

    return 0;
}
