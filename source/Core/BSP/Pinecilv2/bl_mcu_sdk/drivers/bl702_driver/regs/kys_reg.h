/**
  ******************************************************************************
  * @file    kys_reg.h
  * @version V1.2
  * @date    2020-03-30
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
#ifndef __KYS_REG_H__
#define __KYS_REG_H__

#include "bl702.h"

/* 0x0 : ks_ctrl */
#define KYS_KS_CTRL_OFFSET (0x0)
#define KYS_KS_EN          KYS_KS_EN
#define KYS_KS_EN_POS      (0U)
#define KYS_KS_EN_LEN      (1U)
#define KYS_KS_EN_MSK      (((1U << KYS_KS_EN_LEN) - 1) << KYS_KS_EN_POS)
#define KYS_KS_EN_UMSK     (~(((1U << KYS_KS_EN_LEN) - 1) << KYS_KS_EN_POS))
#define KYS_GHOST_EN       KYS_GHOST_EN
#define KYS_GHOST_EN_POS   (2U)
#define KYS_GHOST_EN_LEN   (1U)
#define KYS_GHOST_EN_MSK   (((1U << KYS_GHOST_EN_LEN) - 1) << KYS_GHOST_EN_POS)
#define KYS_GHOST_EN_UMSK  (~(((1U << KYS_GHOST_EN_LEN) - 1) << KYS_GHOST_EN_POS))
#define KYS_DEG_EN         KYS_DEG_EN
#define KYS_DEG_EN_POS     (3U)
#define KYS_DEG_EN_LEN     (1U)
#define KYS_DEG_EN_MSK     (((1U << KYS_DEG_EN_LEN) - 1) << KYS_DEG_EN_POS)
#define KYS_DEG_EN_UMSK    (~(((1U << KYS_DEG_EN_LEN) - 1) << KYS_DEG_EN_POS))
#define KYS_DEG_CNT        KYS_DEG_CNT
#define KYS_DEG_CNT_POS    (4U)
#define KYS_DEG_CNT_LEN    (4U)
#define KYS_DEG_CNT_MSK    (((1U << KYS_DEG_CNT_LEN) - 1) << KYS_DEG_CNT_POS)
#define KYS_DEG_CNT_UMSK   (~(((1U << KYS_DEG_CNT_LEN) - 1) << KYS_DEG_CNT_POS))
#define KYS_RC_EXT         KYS_RC_EXT
#define KYS_RC_EXT_POS     (8U)
#define KYS_RC_EXT_LEN     (2U)
#define KYS_RC_EXT_MSK     (((1U << KYS_RC_EXT_LEN) - 1) << KYS_RC_EXT_POS)
#define KYS_RC_EXT_UMSK    (~(((1U << KYS_RC_EXT_LEN) - 1) << KYS_RC_EXT_POS))
#define KYS_ROW_NUM        KYS_ROW_NUM
#define KYS_ROW_NUM_POS    (16U)
#define KYS_ROW_NUM_LEN    (3U)
#define KYS_ROW_NUM_MSK    (((1U << KYS_ROW_NUM_LEN) - 1) << KYS_ROW_NUM_POS)
#define KYS_ROW_NUM_UMSK   (~(((1U << KYS_ROW_NUM_LEN) - 1) << KYS_ROW_NUM_POS))
#define KYS_COL_NUM        KYS_COL_NUM
#define KYS_COL_NUM_POS    (20U)
#define KYS_COL_NUM_LEN    (5U)
#define KYS_COL_NUM_MSK    (((1U << KYS_COL_NUM_LEN) - 1) << KYS_COL_NUM_POS)
#define KYS_COL_NUM_UMSK   (~(((1U << KYS_COL_NUM_LEN) - 1) << KYS_COL_NUM_POS))

/* 0x10 : ks_int_en */
#define KYS_KS_INT_EN_OFFSET (0x10)
#define KYS_KS_INT_EN        KYS_KS_INT_EN
#define KYS_KS_INT_EN_POS    (0U)
#define KYS_KS_INT_EN_LEN    (1U)
#define KYS_KS_INT_EN_MSK    (((1U << KYS_KS_INT_EN_LEN) - 1) << KYS_KS_INT_EN_POS)
#define KYS_KS_INT_EN_UMSK   (~(((1U << KYS_KS_INT_EN_LEN) - 1) << KYS_KS_INT_EN_POS))

/* 0x14 : ks_int_sts */
#define KYS_KS_INT_STS_OFFSET  (0x14)
#define KYS_KEYCODE_VALID      KYS_KEYCODE_VALID
#define KYS_KEYCODE_VALID_POS  (0U)
#define KYS_KEYCODE_VALID_LEN  (4U)
#define KYS_KEYCODE_VALID_MSK  (((1U << KYS_KEYCODE_VALID_LEN) - 1) << KYS_KEYCODE_VALID_POS)
#define KYS_KEYCODE_VALID_UMSK (~(((1U << KYS_KEYCODE_VALID_LEN) - 1) << KYS_KEYCODE_VALID_POS))

/* 0x18 : keycode_clr */
#define KYS_KEYCODE_CLR_OFFSET (0x18)
#define KYS_KEYCODE_CLR        KYS_KEYCODE_CLR
#define KYS_KEYCODE_CLR_POS    (0U)
#define KYS_KEYCODE_CLR_LEN    (4U)
#define KYS_KEYCODE_CLR_MSK    (((1U << KYS_KEYCODE_CLR_LEN) - 1) << KYS_KEYCODE_CLR_POS)
#define KYS_KEYCODE_CLR_UMSK   (~(((1U << KYS_KEYCODE_CLR_LEN) - 1) << KYS_KEYCODE_CLR_POS))

/* 0x1C : keycode_value */
#define KYS_KEYCODE_VALUE_OFFSET (0x1C)
#define KYS_KEYCODE0             KYS_KEYCODE0
#define KYS_KEYCODE0_POS         (0U)
#define KYS_KEYCODE0_LEN         (8U)
#define KYS_KEYCODE0_MSK         (((1U << KYS_KEYCODE0_LEN) - 1) << KYS_KEYCODE0_POS)
#define KYS_KEYCODE0_UMSK        (~(((1U << KYS_KEYCODE0_LEN) - 1) << KYS_KEYCODE0_POS))
#define KYS_KEYCODE1             KYS_KEYCODE1
#define KYS_KEYCODE1_POS         (8U)
#define KYS_KEYCODE1_LEN         (8U)
#define KYS_KEYCODE1_MSK         (((1U << KYS_KEYCODE1_LEN) - 1) << KYS_KEYCODE1_POS)
#define KYS_KEYCODE1_UMSK        (~(((1U << KYS_KEYCODE1_LEN) - 1) << KYS_KEYCODE1_POS))
#define KYS_KEYCODE2             KYS_KEYCODE2
#define KYS_KEYCODE2_POS         (16U)
#define KYS_KEYCODE2_LEN         (8U)
#define KYS_KEYCODE2_MSK         (((1U << KYS_KEYCODE2_LEN) - 1) << KYS_KEYCODE2_POS)
#define KYS_KEYCODE2_UMSK        (~(((1U << KYS_KEYCODE2_LEN) - 1) << KYS_KEYCODE2_POS))
#define KYS_KEYCODE3             KYS_KEYCODE3
#define KYS_KEYCODE3_POS         (24U)
#define KYS_KEYCODE3_LEN         (8U)
#define KYS_KEYCODE3_MSK         (((1U << KYS_KEYCODE3_LEN) - 1) << KYS_KEYCODE3_POS)
#define KYS_KEYCODE3_UMSK        (~(((1U << KYS_KEYCODE3_LEN) - 1) << KYS_KEYCODE3_POS))

struct kys_reg {
    /* 0x0 : ks_ctrl */
    union {
        struct
        {
            uint32_t ks_en          : 1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1     : 1; /* [    1],       rsvd,        0x0 */
            uint32_t ghost_en       : 1; /* [    2],        r/w,        0x0 */
            uint32_t deg_en         : 1; /* [    3],        r/w,        0x0 */
            uint32_t deg_cnt        : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t rc_ext         : 2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_15 : 6; /* [15:10],       rsvd,        0x0 */
            uint32_t row_num        : 3; /* [18:16],        r/w,        0x7 */
            uint32_t reserved_19    : 1; /* [   19],       rsvd,        0x0 */
            uint32_t col_num        : 5; /* [24:20],        r/w,       0x13 */
            uint32_t reserved_25_31 : 7; /* [31:25],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ks_ctrl;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[12];

    /* 0x10 : ks_int_en */
    union {
        struct
        {
            uint32_t ks_int_en     : 1;  /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ks_int_en;

    /* 0x14 : ks_int_sts */
    union {
        struct
        {
            uint32_t keycode_valid : 4;  /* [ 3: 0],          r,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ks_int_sts;

    /* 0x18 : keycode_clr */
    union {
        struct
        {
            uint32_t keycode_clr   : 4;  /* [ 3: 0],        w1c,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } keycode_clr;

    /* 0x1C : keycode_value */
    union {
        struct
        {
            uint32_t keycode0 : 8; /* [ 7: 0],          r,       0xff */
            uint32_t keycode1 : 8; /* [15: 8],          r,       0xff */
            uint32_t keycode2 : 8; /* [23:16],          r,       0xff */
            uint32_t keycode3 : 8; /* [31:24],          r,       0xff */
        } BF;
        uint32_t WORD;
    } keycode_value;
};

typedef volatile struct kys_reg kys_reg_t;

#endif /* __KYS_REG_H__ */
