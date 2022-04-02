/**
 * @file bsp_spi_sd.h
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

#ifndef __SPI_SD_H__
#define __SPI_SD_H__

#define SPI_PIN_CS GPIO_PIN_10

//SD传输数据结束后是否释放总线宏定义
#define NO_RELEASE 0
#define RELEASE    1
// SD卡类型定义
#define SD_TYPE_MMC  0
#define SD_TYPE_V1   1
#define SD_TYPE_V2   2
#define SD_TYPE_V2HC 4

#define CMD0  0  // Resets the SD memory card.
#define CMD1  1  // Sends host capacity support information and activates the card's initialization process.
#define CMD8  8  // Sends SD Memory Card interface condition, which includes host supply voltage information and asks the card whether card supports voltage.
#define CMD9  9  // Addressed card sends its card specific data (CSD) on the CMD line.
#define CMD10 10 // Addressed card sends its card identification (CID) on the CMD line.
#define CMD12 12 // Forces the card to stop transmission.
#define CMD16 16 // Sets the block length (in bytes for SDSC) for all following block commands (read, write, lock).
// Default block length is fixed to 512 Bytes. Not effective  for SDHS and SDXC
#define CMD17  17 // Reads single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of fixed 512 bytes in case of SDHC and SDXC
#define CMD18  18 // Continuously transfers data blocks from card to host until interrupted by STOP_TRANSMISSION command
#define ACMD23 23 // Specify block count for CMD18 and CMD25
#define CMD24  24 // Writes single block of size selected by SET_BLOCKLEN in case of SDSC, and a block of fixed 512 bytes in case of SDHC and SDXC.
#define CMD25  25 // Continuously writes blocks of data until a STOP_TRANSMISSION follows.
#define ACMD41 41 //
#define CMD55  55 // Indicates to the card that the next command is an application specific command rather than a standard command.
#define CMD58  58 // get OCR
#define CMD59  59 // enable/disable crc

//数据写入回应字意义
#define MSD_DATA_OK          0x05
#define MSD_DATA_CRC_ERROR   0x0B
#define MSD_DATA_WRITE_ERROR 0x0D
#define MSD_DATA_OTHER_ERROR 0xFF
//SD卡回应标记字
#define MSD_RESPONSE_NO_ERROR    0x00 // Card state is ready
#define MSD_IN_IDLE_STATE        0x01 // Card is in identification state
#define MSD_ERASE_RESET          0x02 // Card is in standby state
#define MSD_ILLEGAL_COMMAND      0x04 // Card is in transfer state
#define MSD_COM_CRC_ERROR        0x08 // Card is sending an operation
#define MSD_ERASE_SEQUENCE_ERROR 0x10 // Card is receiving operation information
#define MSD_ADDRESS_ERROR        0x20 // Card is in programming state
#define MSD_PARAMETER_ERROR      0x40 // Card is disconnected
#define MSD_RESPONSE_FAILURE     0xFF // Card is in error state

typedef struct
{
    uint8_t CSD[16];             /*!< SD card specific data table           */
    uint8_t CSDStruct;           /*!< CSD structure                         */
    uint8_t SysSpecVersion;      /*!< System specification version          */
    uint8_t Reserved1;           /*!< Reserved                              */
    uint8_t TAAC;                /*!< Data read access time 1               */
    uint8_t NSAC;                /*!< Data read access time 2 in CLK cycles */
    uint8_t MaxBusClkFrec;       /*!< Max. bus clock frequency              */
    uint16_t CardComdClasses;    /*!< Card command classes                  */
    uint8_t RdBlockLen;          /*!< Max. read data block length           */
    uint8_t PartBlockRead;       /*!< Partial blocks for read allowed       */
    uint8_t WrBlockMisalign;     /*!< Write block misalignment              */
    uint8_t RdBlockMisalign;     /*!< Read block misalignment               */
    uint8_t DSRImpl;             /*!< DSR implemented                       */
    uint8_t Reserved2;           /*!< Reserved                              */
    uint32_t DeviceSize;         /*!< Device Size                           */
    uint8_t MaxRdCurrentVDDMin;  /*!< Max. read current @ VDD min           */
    uint8_t MaxRdCurrentVDDMax;  /*!< Max. read current @ VDD max           */
    uint8_t MaxWrCurrentVDDMin;  /*!< Max. write current @ VDD min          */
    uint8_t MaxWrCurrentVDDMax;  /*!< Max. write current @ VDD max          */
    uint8_t DeviceSizeMul;       /*!< Device size multiplier                */
    uint8_t EraseGrSize;         /*!< Erase group size                      */
    uint8_t EraseGrMul;          /*!< Erase group size multiplier           */
    uint8_t WrProtectGrSize;     /*!< Write protect group size              */
    uint8_t WrProtectGrEnable;   /*!< Write protect group enable            */
    uint8_t ManDeflECC;          /*!< Manufacturer default ECC              */
    uint8_t WrSpeedFact;         /*!< Write speed factor                    */
    uint8_t MaxWrBlockLen;       /*!< Max. write data block length          */
    uint8_t WriteBlockPaPartial; /*!< Partial blocks for write allowed      */
    uint8_t Reserved3;           /*!< Reserved                              */
    uint8_t ContentProtectAppli; /*!< Content protection application        */
    uint8_t FileFormatGrouop;    /*!< File format group                     */
    uint8_t CopyFlag;            /*!< Copy flag (OTP)                       */
    uint8_t PermWrProtect;       /*!< Permanent write protection            */
    uint8_t TempWrProtect;       /*!< Temporary write protection            */
    uint8_t FileFormat;          /*!< File format                           */
    uint8_t ECC;                 /*!< ECC code                              */
    uint8_t CSD_CRC;             /*!< CSD CRC                               */
    uint8_t Reserved4;           /*!< Always 1                              */

} SD_CSDTypedef;

typedef struct
{
    uint8_t CID[16];        /*!< SD card identification number table            */
    uint8_t ManufacturerID; /*!< Manufacturer ID       */
    uint16_t OEM_AppliID;   /*!< OEM/Application ID    */
    uint32_t ProdName1;     /*!< Product Name part1    */
    uint8_t ProdName2;      /*!< Product Name part2    */
    uint8_t ProdRev;        /*!< Product Revision      */
    uint32_t ProdSN;        /*!< Product Serial Number */
    uint8_t Reserved1;      /*!< Reserved1             */
    uint16_t ManufactDate;  /*!< Manufacturing Date    */
    uint8_t CID_CRC;        /*!< CID CRC               */
    uint8_t Reserved2;      /*!< Always 1              */

} SD_CIDTypedef;

typedef struct SD_CardInfoTypedef {
    SD_CSDTypedef SD_csd;   /*!< SD card specific data register         */
    SD_CIDTypedef SD_cid;   /*!< SD card identification number register */
    uint64_t CardCapacity;  /*!< Card capacity                          */
    uint32_t CardBlockSize; /*!< Card block size                        */
    uint16_t RCA;           /*!< SD relative card address               */
    uint8_t CardType;       /*!< SD card type                           */
} SD_CardInfoTypedef;

extern SD_CardInfoTypedef SD_CardInfo;

uint8_t SD_Init(SD_CardInfoTypedef *pCardInfo);
uint8_t SD_Get_CardInfo(SD_CardInfoTypedef *pCardInfo);

uint8_t SD_ReadBlock(uint32_t sector, uint8_t *buffer, uint32_t num);
uint8_t SD_WriteBlock(uint32_t sector, uint8_t *buffer, uint32_t num);

#endif