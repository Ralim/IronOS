/**
  ******************************************************************************
  * @file    pdm_reg.h
  * @version V1.2
  * @date    2020-02-13
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __PDM_REG_H__
#define __PDM_REG_H__

#include "bl702.h"

/* 0x0 : pdm_datapath_config */
#define PDM_DATAPATH_CONFIG_OFFSET (0x0)
#define PDM_EN                     PDM_EN
#define PDM_EN_POS                 (0U)
#define PDM_EN_LEN                 (1U)
#define PDM_EN_MSK                 (((1U << PDM_EN_LEN) - 1) << PDM_EN_POS)
#define PDM_EN_UMSK                (~(((1U << PDM_EN_LEN) - 1) << PDM_EN_POS))
#define PDM_RX_SEL_128FS           PDM_RX_SEL_128FS
#define PDM_RX_SEL_128FS_POS       (2U)
#define PDM_RX_SEL_128FS_LEN       (1U)
#define PDM_RX_SEL_128FS_MSK       (((1U << PDM_RX_SEL_128FS_LEN) - 1) << PDM_RX_SEL_128FS_POS)
#define PDM_RX_SEL_128FS_UMSK      (~(((1U << PDM_RX_SEL_128FS_LEN) - 1) << PDM_RX_SEL_128FS_POS))
#define PDM_TX_SEL_128FS           PDM_TX_SEL_128FS
#define PDM_TX_SEL_128FS_POS       (3U)
#define PDM_TX_SEL_128FS_LEN       (1U)
#define PDM_TX_SEL_128FS_MSK       (((1U << PDM_TX_SEL_128FS_LEN) - 1) << PDM_TX_SEL_128FS_POS)
#define PDM_TX_SEL_128FS_UMSK      (~(((1U << PDM_TX_SEL_128FS_LEN) - 1) << PDM_TX_SEL_128FS_POS))
#define PDM_DC_MUL                 PDM_DC_MUL
#define PDM_DC_MUL_POS             (4U)
#define PDM_DC_MUL_LEN             (8U)
#define PDM_DC_MUL_MSK             (((1U << PDM_DC_MUL_LEN) - 1) << PDM_DC_MUL_POS)
#define PDM_DC_MUL_UMSK            (~(((1U << PDM_DC_MUL_LEN) - 1) << PDM_DC_MUL_POS))
#define PDM_SCALE_SEL              PDM_SCALE_SEL
#define PDM_SCALE_SEL_POS          (12U)
#define PDM_SCALE_SEL_LEN          (3U)
#define PDM_SCALE_SEL_MSK          (((1U << PDM_SCALE_SEL_LEN) - 1) << PDM_SCALE_SEL_POS)
#define PDM_SCALE_SEL_UMSK         (~(((1U << PDM_SCALE_SEL_LEN) - 1) << PDM_SCALE_SEL_POS))
#define PDM_DITHER_SEL             PDM_DITHER_SEL
#define PDM_DITHER_SEL_POS         (16U)
#define PDM_DITHER_SEL_LEN         (2U)
#define PDM_DITHER_SEL_MSK         (((1U << PDM_DITHER_SEL_LEN) - 1) << PDM_DITHER_SEL_POS)
#define PDM_DITHER_SEL_UMSK        (~(((1U << PDM_DITHER_SEL_LEN) - 1) << PDM_DITHER_SEL_POS))
#define PDM_FORCE_LR               PDM_FORCE_LR
#define PDM_FORCE_LR_POS           (20U)
#define PDM_FORCE_LR_LEN           (1U)
#define PDM_FORCE_LR_MSK           (((1U << PDM_FORCE_LR_LEN) - 1) << PDM_FORCE_LR_POS)
#define PDM_FORCE_LR_UMSK          (~(((1U << PDM_FORCE_LR_LEN) - 1) << PDM_FORCE_LR_POS))
#define PDM_FORCE_SEL              PDM_FORCE_SEL
#define PDM_FORCE_SEL_POS          (21U)
#define PDM_FORCE_SEL_LEN          (1U)
#define PDM_FORCE_SEL_MSK          (((1U << PDM_FORCE_SEL_LEN) - 1) << PDM_FORCE_SEL_POS)
#define PDM_FORCE_SEL_UMSK         (~(((1U << PDM_FORCE_SEL_LEN) - 1) << PDM_FORCE_SEL_POS))
#define PDM_DSD_SWAP               PDM_DSD_SWAP
#define PDM_DSD_SWAP_POS           (22U)
#define PDM_DSD_SWAP_LEN           (1U)
#define PDM_DSD_SWAP_MSK           (((1U << PDM_DSD_SWAP_LEN) - 1) << PDM_DSD_SWAP_POS)
#define PDM_DSD_SWAP_UMSK          (~(((1U << PDM_DSD_SWAP_LEN) - 1) << PDM_DSD_SWAP_POS))
#define PDM_OUT_DAT_DLY            PDM_OUT_DAT_DLY
#define PDM_OUT_DAT_DLY_POS        (24U)
#define PDM_OUT_DAT_DLY_LEN        (2U)
#define PDM_OUT_DAT_DLY_MSK        (((1U << PDM_OUT_DAT_DLY_LEN) - 1) << PDM_OUT_DAT_DLY_POS)
#define PDM_OUT_DAT_DLY_UMSK       (~(((1U << PDM_OUT_DAT_DLY_LEN) - 1) << PDM_OUT_DAT_DLY_POS))
#define PDM_OUT_SEL_DLY            PDM_OUT_SEL_DLY
#define PDM_OUT_SEL_DLY_POS        (26U)
#define PDM_OUT_SEL_DLY_LEN        (2U)
#define PDM_OUT_SEL_DLY_MSK        (((1U << PDM_OUT_SEL_DLY_LEN) - 1) << PDM_OUT_SEL_DLY_POS)
#define PDM_OUT_SEL_DLY_UMSK       (~(((1U << PDM_OUT_SEL_DLY_LEN) - 1) << PDM_OUT_SEL_DLY_POS))
#define PDM_OUT_SEL_INV            PDM_OUT_SEL_INV
#define PDM_OUT_SEL_INV_POS        (28U)
#define PDM_OUT_SEL_INV_LEN        (1U)
#define PDM_OUT_SEL_INV_MSK        (((1U << PDM_OUT_SEL_INV_LEN) - 1) << PDM_OUT_SEL_INV_POS)
#define PDM_OUT_SEL_INV_UMSK       (~(((1U << PDM_OUT_SEL_INV_LEN) - 1) << PDM_OUT_SEL_INV_POS))

/* 0x4 : pdm_dma_config */
#define PDM_DMA_CONFIG_OFFSET  (0x4)
#define PDM_DMA_RX_EN          PDM_DMA_RX_EN
#define PDM_DMA_RX_EN_POS      (0U)
#define PDM_DMA_RX_EN_LEN      (1U)
#define PDM_DMA_RX_EN_MSK      (((1U << PDM_DMA_RX_EN_LEN) - 1) << PDM_DMA_RX_EN_POS)
#define PDM_DMA_RX_EN_UMSK     (~(((1U << PDM_DMA_RX_EN_LEN) - 1) << PDM_DMA_RX_EN_POS))
#define PDM_RX_FORMAT          PDM_RX_FORMAT
#define PDM_RX_FORMAT_POS      (4U)
#define PDM_RX_FORMAT_LEN      (3U)
#define PDM_RX_FORMAT_MSK      (((1U << PDM_RX_FORMAT_LEN) - 1) << PDM_RX_FORMAT_POS)
#define PDM_RX_FORMAT_UMSK     (~(((1U << PDM_RX_FORMAT_LEN) - 1) << PDM_RX_FORMAT_POS))
#define PDM_DMA_TX_EN          PDM_DMA_TX_EN
#define PDM_DMA_TX_EN_POS      (8U)
#define PDM_DMA_TX_EN_LEN      (1U)
#define PDM_DMA_TX_EN_MSK      (((1U << PDM_DMA_TX_EN_LEN) - 1) << PDM_DMA_TX_EN_POS)
#define PDM_DMA_TX_EN_UMSK     (~(((1U << PDM_DMA_TX_EN_LEN) - 1) << PDM_DMA_TX_EN_POS))
#define PDM_TX_FORMAT          PDM_TX_FORMAT
#define PDM_TX_FORMAT_POS      (12U)
#define PDM_TX_FORMAT_LEN      (3U)
#define PDM_TX_FORMAT_MSK      (((1U << PDM_TX_FORMAT_LEN) - 1) << PDM_TX_FORMAT_POS)
#define PDM_TX_FORMAT_UMSK     (~(((1U << PDM_TX_FORMAT_LEN) - 1) << PDM_TX_FORMAT_POS))
#define PDM_TX_DATA_SHIFT      PDM_TX_DATA_SHIFT
#define PDM_TX_DATA_SHIFT_POS  (16U)
#define PDM_TX_DATA_SHIFT_LEN  (5U)
#define PDM_TX_DATA_SHIFT_MSK  (((1U << PDM_TX_DATA_SHIFT_LEN) - 1) << PDM_TX_DATA_SHIFT_POS)
#define PDM_TX_DATA_SHIFT_UMSK (~(((1U << PDM_TX_DATA_SHIFT_LEN) - 1) << PDM_TX_DATA_SHIFT_POS))

/* 0x8 : pdm_dma_wdata2 */
#define PDM_DMA_WDATA2_OFFSET (0x8)
#define PDM_DMA_WDATA2        PDM_DMA_WDATA2
#define PDM_DMA_WDATA2_POS    (0U)
#define PDM_DMA_WDATA2_LEN    (32U)
#define PDM_DMA_WDATA2_MSK    (((1U << PDM_DMA_WDATA2_LEN) - 1) << PDM_DMA_WDATA2_POS)
#define PDM_DMA_WDATA2_UMSK   (~(((1U << PDM_DMA_WDATA2_LEN) - 1) << PDM_DMA_WDATA2_POS))

/* 0x10 : pdm_dma_wdata */
#define PDM_DMA_WDATA_OFFSET (0x10)
#define PDM_DMA_WDATA        PDM_DMA_WDATA
#define PDM_DMA_WDATA_POS    (0U)
#define PDM_DMA_WDATA_LEN    (32U)
#define PDM_DMA_WDATA_MSK    (((1U << PDM_DMA_WDATA_LEN) - 1) << PDM_DMA_WDATA_POS)
#define PDM_DMA_WDATA_UMSK   (~(((1U << PDM_DMA_WDATA_LEN) - 1) << PDM_DMA_WDATA_POS))

/* 0x14 : pdm_dma_rdata */
#define PDM_DMA_RDATA_OFFSET (0x14)
#define PDM_DMA_RDATA        PDM_DMA_RDATA
#define PDM_DMA_RDATA_POS    (0U)
#define PDM_DMA_RDATA_LEN    (32U)
#define PDM_DMA_RDATA_MSK    (((1U << PDM_DMA_RDATA_LEN) - 1) << PDM_DMA_RDATA_POS)
#define PDM_DMA_RDATA_UMSK   (~(((1U << PDM_DMA_RDATA_LEN) - 1) << PDM_DMA_RDATA_POS))

/* 0x18 : pdm_tx_fifo_status */
#define PDM_TX_FIFO_STATUS_OFFSET (0x18)
#define PDM_TX_FIFO_EMPTY         PDM_TX_FIFO_EMPTY
#define PDM_TX_FIFO_EMPTY_POS     (0U)
#define PDM_TX_FIFO_EMPTY_LEN     (1U)
#define PDM_TX_FIFO_EMPTY_MSK     (((1U << PDM_TX_FIFO_EMPTY_LEN) - 1) << PDM_TX_FIFO_EMPTY_POS)
#define PDM_TX_FIFO_EMPTY_UMSK    (~(((1U << PDM_TX_FIFO_EMPTY_LEN) - 1) << PDM_TX_FIFO_EMPTY_POS))
#define PDM_TX_FIFO_FULL          PDM_TX_FIFO_FULL
#define PDM_TX_FIFO_FULL_POS      (1U)
#define PDM_TX_FIFO_FULL_LEN      (1U)
#define PDM_TX_FIFO_FULL_MSK      (((1U << PDM_TX_FIFO_FULL_LEN) - 1) << PDM_TX_FIFO_FULL_POS)
#define PDM_TX_FIFO_FULL_UMSK     (~(((1U << PDM_TX_FIFO_FULL_LEN) - 1) << PDM_TX_FIFO_FULL_POS))
#define PDM_TX_CS                 PDM_TX_CS
#define PDM_TX_CS_POS             (2U)
#define PDM_TX_CS_LEN             (2U)
#define PDM_TX_CS_MSK             (((1U << PDM_TX_CS_LEN) - 1) << PDM_TX_CS_POS)
#define PDM_TX_CS_UMSK            (~(((1U << PDM_TX_CS_LEN) - 1) << PDM_TX_CS_POS))
#define PDM_TXFIFORDPTR           PDM_TXFIFORDPTR
#define PDM_TXFIFORDPTR_POS       (4U)
#define PDM_TXFIFORDPTR_LEN       (3U)
#define PDM_TXFIFORDPTR_MSK       (((1U << PDM_TXFIFORDPTR_LEN) - 1) << PDM_TXFIFORDPTR_POS)
#define PDM_TXFIFORDPTR_UMSK      (~(((1U << PDM_TXFIFORDPTR_LEN) - 1) << PDM_TXFIFORDPTR_POS))
#define PDM_TXFIFOWRPTR           PDM_TXFIFOWRPTR
#define PDM_TXFIFOWRPTR_POS       (8U)
#define PDM_TXFIFOWRPTR_LEN       (2U)
#define PDM_TXFIFOWRPTR_MSK       (((1U << PDM_TXFIFOWRPTR_LEN) - 1) << PDM_TXFIFOWRPTR_POS)
#define PDM_TXFIFOWRPTR_UMSK      (~(((1U << PDM_TXFIFOWRPTR_LEN) - 1) << PDM_TXFIFOWRPTR_POS))
#define PDM_TX2_FIFO_EMPTY        PDM_TX2_FIFO_EMPTY
#define PDM_TX2_FIFO_EMPTY_POS    (16U)
#define PDM_TX2_FIFO_EMPTY_LEN    (1U)
#define PDM_TX2_FIFO_EMPTY_MSK    (((1U << PDM_TX2_FIFO_EMPTY_LEN) - 1) << PDM_TX2_FIFO_EMPTY_POS)
#define PDM_TX2_FIFO_EMPTY_UMSK   (~(((1U << PDM_TX2_FIFO_EMPTY_LEN) - 1) << PDM_TX2_FIFO_EMPTY_POS))
#define PDM_TX2_FIFO_FULL         PDM_TX2_FIFO_FULL
#define PDM_TX2_FIFO_FULL_POS     (17U)
#define PDM_TX2_FIFO_FULL_LEN     (1U)
#define PDM_TX2_FIFO_FULL_MSK     (((1U << PDM_TX2_FIFO_FULL_LEN) - 1) << PDM_TX2_FIFO_FULL_POS)
#define PDM_TX2_FIFO_FULL_UMSK    (~(((1U << PDM_TX2_FIFO_FULL_LEN) - 1) << PDM_TX2_FIFO_FULL_POS))
#define PDM_TX2_CS                PDM_TX2_CS
#define PDM_TX2_CS_POS            (18U)
#define PDM_TX2_CS_LEN            (2U)
#define PDM_TX2_CS_MSK            (((1U << PDM_TX2_CS_LEN) - 1) << PDM_TX2_CS_POS)
#define PDM_TX2_CS_UMSK           (~(((1U << PDM_TX2_CS_LEN) - 1) << PDM_TX2_CS_POS))
#define PDM_TX2FIFORDPTR          PDM_TX2FIFORDPTR
#define PDM_TX2FIFORDPTR_POS      (20U)
#define PDM_TX2FIFORDPTR_LEN      (3U)
#define PDM_TX2FIFORDPTR_MSK      (((1U << PDM_TX2FIFORDPTR_LEN) - 1) << PDM_TX2FIFORDPTR_POS)
#define PDM_TX2FIFORDPTR_UMSK     (~(((1U << PDM_TX2FIFORDPTR_LEN) - 1) << PDM_TX2FIFORDPTR_POS))
#define PDM_TX2FIFOWRPTR          PDM_TX2FIFOWRPTR
#define PDM_TX2FIFOWRPTR_POS      (24U)
#define PDM_TX2FIFOWRPTR_LEN      (2U)
#define PDM_TX2FIFOWRPTR_MSK      (((1U << PDM_TX2FIFOWRPTR_LEN) - 1) << PDM_TX2FIFOWRPTR_POS)
#define PDM_TX2FIFOWRPTR_UMSK     (~(((1U << PDM_TX2FIFOWRPTR_LEN) - 1) << PDM_TX2FIFOWRPTR_POS))

/* 0x1C : pdm_rx_fifo_status */
#define PDM_RX_FIFO_STATUS_OFFSET (0x1C)
#define PDM_RX_FIFO_EMPTY         PDM_RX_FIFO_EMPTY
#define PDM_RX_FIFO_EMPTY_POS     (0U)
#define PDM_RX_FIFO_EMPTY_LEN     (1U)
#define PDM_RX_FIFO_EMPTY_MSK     (((1U << PDM_RX_FIFO_EMPTY_LEN) - 1) << PDM_RX_FIFO_EMPTY_POS)
#define PDM_RX_FIFO_EMPTY_UMSK    (~(((1U << PDM_RX_FIFO_EMPTY_LEN) - 1) << PDM_RX_FIFO_EMPTY_POS))
#define PDM_RX_FIFO_FULL          PDM_RX_FIFO_FULL
#define PDM_RX_FIFO_FULL_POS      (1U)
#define PDM_RX_FIFO_FULL_LEN      (1U)
#define PDM_RX_FIFO_FULL_MSK      (((1U << PDM_RX_FIFO_FULL_LEN) - 1) << PDM_RX_FIFO_FULL_POS)
#define PDM_RX_FIFO_FULL_UMSK     (~(((1U << PDM_RX_FIFO_FULL_LEN) - 1) << PDM_RX_FIFO_FULL_POS))
#define PDM_RX_CS                 PDM_RX_CS
#define PDM_RX_CS_POS             (2U)
#define PDM_RX_CS_LEN             (2U)
#define PDM_RX_CS_MSK             (((1U << PDM_RX_CS_LEN) - 1) << PDM_RX_CS_POS)
#define PDM_RX_CS_UMSK            (~(((1U << PDM_RX_CS_LEN) - 1) << PDM_RX_CS_POS))
#define PDM_RXFIFORDPTR           PDM_RXFIFORDPTR
#define PDM_RXFIFORDPTR_POS       (4U)
#define PDM_RXFIFORDPTR_LEN       (2U)
#define PDM_RXFIFORDPTR_MSK       (((1U << PDM_RXFIFORDPTR_LEN) - 1) << PDM_RXFIFORDPTR_POS)
#define PDM_RXFIFORDPTR_UMSK      (~(((1U << PDM_RXFIFORDPTR_LEN) - 1) << PDM_RXFIFORDPTR_POS))
#define PDM_RXFIFOWRPTR           PDM_RXFIFOWRPTR
#define PDM_RXFIFOWRPTR_POS       (8U)
#define PDM_RXFIFOWRPTR_LEN       (3U)
#define PDM_RXFIFOWRPTR_MSK       (((1U << PDM_RXFIFOWRPTR_LEN) - 1) << PDM_RXFIFOWRPTR_POS)
#define PDM_RXFIFOWRPTR_UMSK      (~(((1U << PDM_RXFIFOWRPTR_LEN) - 1) << PDM_RXFIFOWRPTR_POS))

struct pdm_reg {
    /* 0x0 : pdm_datapath_config */
    union {
        struct
        {
            uint32_t pdm_en         : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1     : 1; /* [    1],       rsvd,        0x0 */
            uint32_t rx_sel_128fs   : 1; /* [    2],        r/w,        0x0 */
            uint32_t tx_sel_128fs   : 1; /* [    3],        r/w,        0x0 */
            uint32_t dc_mul         : 8; /* [11: 4],        r/w,       0x64 */
            uint32_t scale_sel      : 3; /* [14:12],        r/w,        0x5 */
            uint32_t reserved_15    : 1; /* [   15],       rsvd,        0x0 */
            uint32_t dither_sel     : 2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19 : 2; /* [19:18],       rsvd,        0x0 */
            uint32_t force_lr       : 1; /* [   20],        r/w,        0x0 */
            uint32_t force_sel      : 1; /* [   21],        r/w,        0x0 */
            uint32_t dsd_swap       : 1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23    : 1; /* [   23],       rsvd,        0x0 */
            uint32_t out_dat_dly    : 2; /* [25:24],        r/w,        0x0 */
            uint32_t out_sel_dly    : 2; /* [27:26],        r/w,        0x0 */
            uint32_t out_sel_inv    : 1; /* [   28],        r/w,        0x0 */
            uint32_t rsvd_31_29     : 3; /* [31:29],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pdm_datapath_config;

    /* 0x4 : pdm_dma_config */
    union {
        struct
        {
            uint32_t pdm_dma_rx_en  : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3   : 3;  /* [ 3: 1],       rsvd,        0x0 */
            uint32_t rx_format      : 3;  /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7     : 1;  /* [    7],       rsvd,        0x0 */
            uint32_t pdm_dma_tx_en  : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11  : 3;  /* [11: 9],       rsvd,        0x0 */
            uint32_t tx_format      : 3;  /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15    : 1;  /* [   15],       rsvd,        0x0 */
            uint32_t tx_data_shift  : 5;  /* [20:16],        r/w,        0x0 */
            uint32_t reserved_21_31 : 11; /* [31:21],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pdm_dma_config;

    /* 0x8 : pdm_dma_wdata2 */
    union {
        struct
        {
            uint32_t pdm_dma_wdata2 : 32; /* [31: 0],          w,          x */
        } BF;
        uint32_t WORD;
    } pdm_dma_wdata2;

    /* 0xc  reserved */
    uint8_t RESERVED0xc[4];

    /* 0x10 : pdm_dma_wdata */
    union {
        struct
        {
            uint32_t pdm_dma_wdata : 32; /* [31: 0],          w,          x */
        } BF;
        uint32_t WORD;
    } pdm_dma_wdata;

    /* 0x14 : pdm_dma_rdata */
    union {
        struct
        {
            uint32_t pdm_dma_rdata : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } pdm_dma_rdata;

    /* 0x18 : pdm_tx_fifo_status */
    union {
        struct
        {
            uint32_t tx_fifo_empty  : 1; /* [    0],          r,        0x0 */
            uint32_t tx_fifo_full   : 1; /* [    1],          r,        0x0 */
            uint32_t tx_cs          : 2; /* [ 3: 2],          r,        0x0 */
            uint32_t TxFifoRdPtr    : 3; /* [ 6: 4],          r,        0x4 */
            uint32_t reserved_7     : 1; /* [    7],       rsvd,        0x0 */
            uint32_t TxFifoWrPtr    : 2; /* [ 9: 8],          r,        0x0 */
            uint32_t reserved_10_15 : 6; /* [15:10],       rsvd,        0x0 */
            uint32_t tx2_fifo_empty : 1; /* [   16],          r,        0x0 */
            uint32_t tx2_fifo_full  : 1; /* [   17],          r,        0x0 */
            uint32_t tx2_cs         : 2; /* [19:18],          r,        0x0 */
            uint32_t Tx2FifoRdPtr   : 3; /* [22:20],          r,        0x4 */
            uint32_t reserved_23    : 1; /* [   23],       rsvd,        0x0 */
            uint32_t Tx2FifoWrPtr   : 2; /* [25:24],          r,        0x0 */
            uint32_t reserved_26_31 : 6; /* [31:26],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pdm_tx_fifo_status;

    /* 0x1C : pdm_rx_fifo_status */
    union {
        struct
        {
            uint32_t rx_fifo_empty  : 1;  /* [    0],          r,        0x1 */
            uint32_t rx_fifo_full   : 1;  /* [    1],          r,        0x0 */
            uint32_t rx_cs          : 2;  /* [ 3: 2],          r,        0x0 */
            uint32_t RxFifoRdPtr    : 2;  /* [ 5: 4],          r,        0x3 */
            uint32_t reserved_6_7   : 2;  /* [ 7: 6],       rsvd,        0x0 */
            uint32_t RxFifoWrPtr    : 3;  /* [10: 8],          r,        0x0 */
            uint32_t reserved_11_31 : 21; /* [31:11],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pdm_rx_fifo_status;
};

typedef volatile struct pdm_reg pdm_reg_t;

#endif /* __PDM_REG_H__ */
