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
 * Title:        riscv_nn_accumulate_q7_to_q15.c
 * Description:  Accumulate q7 vector into q15 one.
 *
 * $Date:        May 29, 2020
 * $Revision:    V.1.0.1
 *
 * pSrc Processor:  RISC-V CPUs
 *
 * -------------------------------------------------------------------- */
#include "riscv_math.h"
#include "ref_functions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup NNBasicMath
 * @{
 */

void riscv_nn_accumulate_q7_to_q15_ref(q15_t *pDst, const q7_t *pSrc, uint32_t length)
{
    q15_t *pCnt = pDst;
    const q7_t *pV = pSrc;
    q31_t v1, v2, vo1, vo2;
    int32_t cnt = length >> 2;
    q31_t in;

    while (cnt > 0l)
    {
        q31_t value = riscv_nn_read_q7x4_ia(&pV);
        v1 = __SXTB16(__ROR((uint32_t)value, 8));
        v2 = __SXTB16(value);
        vo2 = (q31_t)__PKHTB(v1, v2, 16);
        vo1 = (q31_t)__PKHBT(v2, v1, 16);

        in = riscv_nn_read_q15x2(pCnt);
        write_q15x2_ia(&pCnt, __QADD16(vo1, in));

        in = riscv_nn_read_q15x2(pCnt);
        write_q15x2_ia(&pCnt, __QADD16(vo2, in));

        cnt--;
    }
    cnt = length & 0x3;
    while (cnt > 0l)
    {
        *pCnt++ += *pV++;
        cnt--;
    }
}

/**
 * @} end of NNBasicMath group
 */