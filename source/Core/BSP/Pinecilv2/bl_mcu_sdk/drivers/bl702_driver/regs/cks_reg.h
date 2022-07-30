/**
  ******************************************************************************
  * @file    cks_reg.h
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
#ifndef __CKS_REG_H__
#define __CKS_REG_H__

#include "bl702.h"

/* 0x0 : cks_config */
#define CKS_CONFIG_OFFSET         (0x0)
#define CKS_CR_CKS_CLR            CKS_CR_CKS_CLR
#define CKS_CR_CKS_CLR_POS        (0U)
#define CKS_CR_CKS_CLR_LEN        (1U)
#define CKS_CR_CKS_CLR_MSK        (((1U << CKS_CR_CKS_CLR_LEN) - 1) << CKS_CR_CKS_CLR_POS)
#define CKS_CR_CKS_CLR_UMSK       (~(((1U << CKS_CR_CKS_CLR_LEN) - 1) << CKS_CR_CKS_CLR_POS))
#define CKS_CR_CKS_BYTE_SWAP      CKS_CR_CKS_BYTE_SWAP
#define CKS_CR_CKS_BYTE_SWAP_POS  (1U)
#define CKS_CR_CKS_BYTE_SWAP_LEN  (1U)
#define CKS_CR_CKS_BYTE_SWAP_MSK  (((1U << CKS_CR_CKS_BYTE_SWAP_LEN) - 1) << CKS_CR_CKS_BYTE_SWAP_POS)
#define CKS_CR_CKS_BYTE_SWAP_UMSK (~(((1U << CKS_CR_CKS_BYTE_SWAP_LEN) - 1) << CKS_CR_CKS_BYTE_SWAP_POS))

/* 0x4 : data_in */
#define CKS_DATA_IN_OFFSET (0x4)
#define CKS_DATA_IN        CKS_DATA_IN
#define CKS_DATA_IN_POS    (0U)
#define CKS_DATA_IN_LEN    (8U)
#define CKS_DATA_IN_MSK    (((1U << CKS_DATA_IN_LEN) - 1) << CKS_DATA_IN_POS)
#define CKS_DATA_IN_UMSK   (~(((1U << CKS_DATA_IN_LEN) - 1) << CKS_DATA_IN_POS))

/* 0x8 : cks_out */
#define CKS_OUT_OFFSET (0x8)
#define CKS_OUT        CKS_OUT
#define CKS_OUT_POS    (0U)
#define CKS_OUT_LEN    (16U)
#define CKS_OUT_MSK    (((1U << CKS_OUT_LEN) - 1) << CKS_OUT_POS)
#define CKS_OUT_UMSK   (~(((1U << CKS_OUT_LEN) - 1) << CKS_OUT_POS))

struct cks_reg {
    /* 0x0 : cks_config */
    union {
        struct
        {
            uint32_t cr_cks_clr       : 1;  /* [    0],        w1c,        0x0 */
            uint32_t cr_cks_byte_swap : 1;  /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31    : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } cks_config;

    /* 0x4 : data_in */
    union {
        struct
        {
            uint32_t data_in       : 8;  /* [ 7: 0],          w,          x */
            uint32_t reserved_8_31 : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } data_in;

    /* 0x8 : cks_out */
    union {
        struct
        {
            uint32_t cks_out        : 16; /* [15: 0],          r,     0xffff */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } cks_out;
};

typedef volatile struct cks_reg cks_reg_t;

#endif /* __CKS_REG_H__ */
