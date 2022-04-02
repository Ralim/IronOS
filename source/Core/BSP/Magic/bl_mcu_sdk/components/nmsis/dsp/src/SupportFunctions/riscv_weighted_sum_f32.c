/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_weighted_sum_f32.c
 * Description:  Weighted Sum
 *
 * $Date:        23 April 2021
 * $Revision:    V1.9.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
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

#include <limits.h>
#include <math.h>

#include "dsp/support_functions.h"

/**
 * @addtogroup weightedsum
 * @{
 */


/**
 * @brief Weighted sum
 *
 *
 * @param[in]    *in           Array of input values.
 * @param[in]    *weigths      Weights
 * @param[in]    blockSize     Number of samples in the input array.
 * @return       Weighted sum
 *
 */

float32_t riscv_weighted_sum_f32(const float32_t *in, const float32_t *weigths, uint32_t blockSize)
{

    float32_t accum1, accum2;
    const float32_t *pIn, *pW;
    uint32_t blkCnt;

#if defined(RISCV_VECTOR)
    uint32_t blkCnt_v;
    size_t l;
    float32_t temp_max;
    vfloat32m8_t v_x, v_y;
    vfloat32m8_t v_a, v_b;
    vfloat32m1_t v_temp;
    l = vsetvl_e32m1(1);
    v_temp = vfsub_vv_f32m1(v_temp, v_temp, l);
#endif

    pIn = in;
    pW = weigths;

    accum1=0.0f;
    accum2=0.0f;

#if defined(RISCV_VECTOR)
    blkCnt_v = blockSize;
    l = vsetvl_e32m8(blkCnt_v);
    v_a = vfsub_vv_f32m8(v_a,v_a, l);
    v_b = vfsub_vv_f32m8(v_b,v_b, l);
    for (; (l = vsetvl_e32m8(blkCnt_v)) > 0; blkCnt_v -= l) {
        v_x = vle32_v_f32m8(pIn, l);
        v_y = vle32_v_f32m8(pW, l);
        v_a = vfmacc_vv_f32m8(v_a,v_x,v_y, l);
        v_b = vfadd_vv_f32m8(v_b,v_y, l);
        pIn += l;
        pW += l;
    }
    l = vsetvl_e32m8(blockSize);
    accum1 = vfmv_f_s_f32m1_f32 (vfredsum_vs_f32m8_f32m1(v_temp,v_a,v_temp, l));
    l = vsetvl_e32m8(blockSize);
    accum2 = vfmv_f_s_f32m1_f32 (vfredsum_vs_f32m8_f32m1(v_temp,v_b,v_temp, l));
#else
    blkCnt = blockSize;
    while(blkCnt > 0)
    {
        accum1 += *pIn++ * *pW;
        accum2 += *pW++;
        blkCnt--;
    }
#endif
    return(accum1 / accum2);
}

/**
 * @} end of weightedsum group
 */
