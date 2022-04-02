/*
 * Copyright (C) 2010-2020 Arm Limited or its affiliates. All rights reserved.
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_nn_accumulate_q7_to_q7.c
 * Description:  Accumulate q7 vector into q7 one.
 *
 * $Date:        May 29, 2020
 * $Revision:    V.1.0.1
 *
 * pSrc Processor:  RISC-V CPUs
 *
 * -------------------------------------------------------------------- */
#include "riscv_nnsupportfunctions.h"
#include "riscv_nnfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup NNBasicMath
 * @{
 */

void riscv_nn_accumulate_q7_to_q7(q7_t *pDst, const q7_t *pSrc, uint32_t length)
{
    q7_t *pCnt = pDst;
    const q7_t *pV = pSrc;
#if defined(RISCV_VECTOR)
    uint32_t blkCnt = length;                               /* Loop counter */
    size_t l;
    vint8m8_t valuea,valueb;
         
    for (; (l = vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
        valuea = vle8_v_i8m8(pV, l);
        valueb = vle8_v_i8m8(pCnt, l);
        vse8_v_i8m8 (pCnt, vsadd_vv_i8m8(valuea, valueb, l), l);
        pV += l;
        pCnt += l;
    }
#else
    int32_t cnt = length >> 2;

    while (cnt > 0l)
    {
        q31_t valuea = riscv_nn_read_q7x4_ia(&pV);
        q31_t valueb = riscv_nn_read_q7x4(pCnt);

        write_q7x4_ia(&pCnt, __RV_KADD8(valuea, valueb));
        cnt--;
    }
    cnt = length & 0x3;
    while (cnt > 0l)
    {
        *pCnt++ += *pV++;
        cnt--;
    }
#endif /* defined(RISCV_VECTOR) */
}

/**
 * @} end of NNBasicMath group
 */