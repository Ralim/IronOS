/**
  ******************************************************************************
  * @file    cci_reg.h
  * @version V1.2
  * @date    2019-11-12
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
#ifndef __CCI_REG_H__
#define __CCI_REG_H__

#include "bl602.h"

/* 0x0 : cci_cfg */
#define CCI_CFG_OFFSET              (0x0)
#define CCI_EN                      CCI_EN
#define CCI_EN_POS                  (0U)
#define CCI_EN_LEN                  (1U)
#define CCI_EN_MSK                  (((1U << CCI_EN_LEN) - 1) << CCI_EN_POS)
#define CCI_EN_UMSK                 (~(((1U << CCI_EN_LEN) - 1) << CCI_EN_POS))
#define CCI_SLV_SEL_CCI2            CCI_SLV_SEL_CCI2
#define CCI_SLV_SEL_CCI2_POS        (1U)
#define CCI_SLV_SEL_CCI2_LEN        (1U)
#define CCI_SLV_SEL_CCI2_MSK        (((1U << CCI_SLV_SEL_CCI2_LEN) - 1) << CCI_SLV_SEL_CCI2_POS)
#define CCI_SLV_SEL_CCI2_UMSK       (~(((1U << CCI_SLV_SEL_CCI2_LEN) - 1) << CCI_SLV_SEL_CCI2_POS))
#define CCI_MAS_SEL_CCI2            CCI_MAS_SEL_CCI2
#define CCI_MAS_SEL_CCI2_POS        (2U)
#define CCI_MAS_SEL_CCI2_LEN        (1U)
#define CCI_MAS_SEL_CCI2_MSK        (((1U << CCI_MAS_SEL_CCI2_LEN) - 1) << CCI_MAS_SEL_CCI2_POS)
#define CCI_MAS_SEL_CCI2_UMSK       (~(((1U << CCI_MAS_SEL_CCI2_LEN) - 1) << CCI_MAS_SEL_CCI2_POS))
#define CCI_MAS_HW_MODE             CCI_MAS_HW_MODE
#define CCI_MAS_HW_MODE_POS         (3U)
#define CCI_MAS_HW_MODE_LEN         (1U)
#define CCI_MAS_HW_MODE_MSK         (((1U << CCI_MAS_HW_MODE_LEN) - 1) << CCI_MAS_HW_MODE_POS)
#define CCI_MAS_HW_MODE_UMSK        (~(((1U << CCI_MAS_HW_MODE_LEN) - 1) << CCI_MAS_HW_MODE_POS))
#define CCI_REG_M_CCI_SCLK_EN       CCI_REG_M_CCI_SCLK_EN
#define CCI_REG_M_CCI_SCLK_EN_POS   (4U)
#define CCI_REG_M_CCI_SCLK_EN_LEN   (1U)
#define CCI_REG_M_CCI_SCLK_EN_MSK   (((1U << CCI_REG_M_CCI_SCLK_EN_LEN) - 1) << CCI_REG_M_CCI_SCLK_EN_POS)
#define CCI_REG_M_CCI_SCLK_EN_UMSK  (~(((1U << CCI_REG_M_CCI_SCLK_EN_LEN) - 1) << CCI_REG_M_CCI_SCLK_EN_POS))
#define CCI_REG_DIV_M_CCI_SCLK      CCI_REG_DIV_M_CCI_SCLK
#define CCI_REG_DIV_M_CCI_SCLK_POS  (5U)
#define CCI_REG_DIV_M_CCI_SCLK_LEN  (2U)
#define CCI_REG_DIV_M_CCI_SCLK_MSK  (((1U << CCI_REG_DIV_M_CCI_SCLK_LEN) - 1) << CCI_REG_DIV_M_CCI_SCLK_POS)
#define CCI_REG_DIV_M_CCI_SCLK_UMSK (~(((1U << CCI_REG_DIV_M_CCI_SCLK_LEN) - 1) << CCI_REG_DIV_M_CCI_SCLK_POS))
#define CCI_CFG_CCI1_PRE_READ       CCI_CFG_CCI1_PRE_READ
#define CCI_CFG_CCI1_PRE_READ_POS   (7U)
#define CCI_CFG_CCI1_PRE_READ_LEN   (1U)
#define CCI_CFG_CCI1_PRE_READ_MSK   (((1U << CCI_CFG_CCI1_PRE_READ_LEN) - 1) << CCI_CFG_CCI1_PRE_READ_POS)
#define CCI_CFG_CCI1_PRE_READ_UMSK  (~(((1U << CCI_CFG_CCI1_PRE_READ_LEN) - 1) << CCI_CFG_CCI1_PRE_READ_POS))
#define CCI_REG_SCCI_CLK_INV        CCI_REG_SCCI_CLK_INV
#define CCI_REG_SCCI_CLK_INV_POS    (8U)
#define CCI_REG_SCCI_CLK_INV_LEN    (1U)
#define CCI_REG_SCCI_CLK_INV_MSK    (((1U << CCI_REG_SCCI_CLK_INV_LEN) - 1) << CCI_REG_SCCI_CLK_INV_POS)
#define CCI_REG_SCCI_CLK_INV_UMSK   (~(((1U << CCI_REG_SCCI_CLK_INV_LEN) - 1) << CCI_REG_SCCI_CLK_INV_POS))
#define CCI_REG_MCCI_CLK_INV        CCI_REG_MCCI_CLK_INV
#define CCI_REG_MCCI_CLK_INV_POS    (9U)
#define CCI_REG_MCCI_CLK_INV_LEN    (1U)
#define CCI_REG_MCCI_CLK_INV_MSK    (((1U << CCI_REG_MCCI_CLK_INV_LEN) - 1) << CCI_REG_MCCI_CLK_INV_POS)
#define CCI_REG_MCCI_CLK_INV_UMSK   (~(((1U << CCI_REG_MCCI_CLK_INV_LEN) - 1) << CCI_REG_MCCI_CLK_INV_POS))

/* 0x4 : cci_addr */
#define CCI_ADDR_OFFSET       (0x4)
#define CCI_APB_CCI_ADDR      CCI_APB_CCI_ADDR
#define CCI_APB_CCI_ADDR_POS  (0U)
#define CCI_APB_CCI_ADDR_LEN  (32U)
#define CCI_APB_CCI_ADDR_MSK  (((1U << CCI_APB_CCI_ADDR_LEN) - 1) << CCI_APB_CCI_ADDR_POS)
#define CCI_APB_CCI_ADDR_UMSK (~(((1U << CCI_APB_CCI_ADDR_LEN) - 1) << CCI_APB_CCI_ADDR_POS))

/* 0x8 : cci_wdata */
#define CCI_WDATA_OFFSET       (0x8)
#define CCI_APB_CCI_WDATA      CCI_APB_CCI_WDATA
#define CCI_APB_CCI_WDATA_POS  (0U)
#define CCI_APB_CCI_WDATA_LEN  (32U)
#define CCI_APB_CCI_WDATA_MSK  (((1U << CCI_APB_CCI_WDATA_LEN) - 1) << CCI_APB_CCI_WDATA_POS)
#define CCI_APB_CCI_WDATA_UMSK (~(((1U << CCI_APB_CCI_WDATA_LEN) - 1) << CCI_APB_CCI_WDATA_POS))

/* 0xC : cci_rdata */
#define CCI_RDATA_OFFSET       (0xC)
#define CCI_APB_CCI_RDATA      CCI_APB_CCI_RDATA
#define CCI_APB_CCI_RDATA_POS  (0U)
#define CCI_APB_CCI_RDATA_LEN  (32U)
#define CCI_APB_CCI_RDATA_MSK  (((1U << CCI_APB_CCI_RDATA_LEN) - 1) << CCI_APB_CCI_RDATA_POS)
#define CCI_APB_CCI_RDATA_UMSK (~(((1U << CCI_APB_CCI_RDATA_LEN) - 1) << CCI_APB_CCI_RDATA_POS))

/* 0x10 : cci_ctl */
#define CCI_CTL_OFFSET      (0x10)
#define CCI_WRITE_FLAG      CCI_WRITE_FLAG
#define CCI_WRITE_FLAG_POS  (0U)
#define CCI_WRITE_FLAG_LEN  (1U)
#define CCI_WRITE_FLAG_MSK  (((1U << CCI_WRITE_FLAG_LEN) - 1) << CCI_WRITE_FLAG_POS)
#define CCI_WRITE_FLAG_UMSK (~(((1U << CCI_WRITE_FLAG_LEN) - 1) << CCI_WRITE_FLAG_POS))
#define CCI_READ_FLAG       CCI_READ_FLAG
#define CCI_READ_FLAG_POS   (1U)
#define CCI_READ_FLAG_LEN   (1U)
#define CCI_READ_FLAG_MSK   (((1U << CCI_READ_FLAG_LEN) - 1) << CCI_READ_FLAG_POS)
#define CCI_READ_FLAG_UMSK  (~(((1U << CCI_READ_FLAG_LEN) - 1) << CCI_READ_FLAG_POS))
#define CCI_AHB_STATE       CCI_AHB_STATE
#define CCI_AHB_STATE_POS   (2U)
#define CCI_AHB_STATE_LEN   (2U)
#define CCI_AHB_STATE_MSK   (((1U << CCI_AHB_STATE_LEN) - 1) << CCI_AHB_STATE_POS)
#define CCI_AHB_STATE_UMSK  (~(((1U << CCI_AHB_STATE_LEN) - 1) << CCI_AHB_STATE_POS))

struct cci_reg {
    /* 0x0 : cci_cfg */
    union {
        struct
        {
            uint32_t cci_en             : 1;  /* [    0],        r/w,        0x1 */
            uint32_t cci_slv_sel_cci2   : 1;  /* [    1],        r/w,        0x0 */
            uint32_t cci_mas_sel_cci2   : 1;  /* [    2],        r/w,        0x0 */
            uint32_t cci_mas_hw_mode    : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reg_m_cci_sclk_en  : 1;  /* [    4],        r/w,        0x0 */
            uint32_t reg_div_m_cci_sclk : 2;  /* [ 6: 5],        r/w,        0x1 */
            uint32_t cfg_cci1_pre_read  : 1;  /* [    7],        r/w,        0x0 */
            uint32_t reg_scci_clk_inv   : 1;  /* [    8],        r/w,        0x0 */
            uint32_t reg_mcci_clk_inv   : 1;  /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_31     : 22; /* [31:10],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } cci_cfg;

    /* 0x4 : cci_addr */
    union {
        struct
        {
            uint32_t apb_cci_addr : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } cci_addr;

    /* 0x8 : cci_wdata */
    union {
        struct
        {
            uint32_t apb_cci_wdata : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } cci_wdata;

    /* 0xC : cci_rdata */
    union {
        struct
        {
            uint32_t apb_cci_rdata : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } cci_rdata;

    /* 0x10 : cci_ctl */
    union {
        struct
        {
            uint32_t cci_write_flag : 1;  /* [    0],          r,        0x0 */
            uint32_t cci_read_flag  : 1;  /* [    1],          r,        0x0 */
            uint32_t ahb_state      : 2;  /* [ 3: 2],          r,        0x0 */
            uint32_t reserved_4_31  : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } cci_ctl;
};

typedef volatile struct cci_reg cci_reg_t;

#endif /* __CCI_REG_H__ */
