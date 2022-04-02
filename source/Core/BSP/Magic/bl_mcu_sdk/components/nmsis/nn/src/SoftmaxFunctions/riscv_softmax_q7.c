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
 * Title:        riscv_softmax_q7.c
 * Description:  Q7 softmax function
 *
 * $Date:        09. October 2020
 * $Revision:    V.1.0.2
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnfunctions.h"

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup Softmax
 * @{
 */

/**
 * @brief Q7 softmax function
 * @param[in]       vec_in      pointer to input vector
 * @param[in]       dim_vec     input vector dimention
 * @param[out]      p_out       pointer to output vector
 *
 * @details
 *
 *  Here, instead of typical natural logarithm e based softmax, we use
 *  2-based softmax here, i.e.,:
 *
 *  y_i = 2^(x_i) / sum(2^x_j)
 *
 *  The relative output will be different here.
 *  But mathematically, the gradient will be the same
 *  with a log(2) scaling factor.
 *
 */

void riscv_softmax_q7(const q7_t *vec_in, const uint16_t dim_vec, q7_t *p_out)
{
    q31_t sum;
    int16_t i;
    uint8_t shift;
    q15_t base;
    base = -128;
#if defined(RISCV_VECTOR)
    size_t l;
    q15_t temp_max;
    uint32_t blkCnt;
    ptrdiff_t bstride;
    vint8m8_t v_x;
    vint8m1_t vtemp;
    l = vsetvl_e8m1(1);
    vtemp = vmv_v_x_i8m1(base, l);
    blkCnt = dim_vec;
    // base = vec_in[0];
    i = 0;
    for (; (l = vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
        v_x = vle8_v_i8m8(vec_in+i, l);
        i += l;
        temp_max = vmv_x_s_i8m1_i8(vredmax_vs_i8m8_i8m1(vtemp, v_x, vtemp, l));
        if(temp_max > base) 
            base = temp_max;
    }
#else
    /* We first search for the maximum */
    for (i = 0; i < dim_vec; i++)
    {
        if (vec_in[i] > base)
        {
            base = vec_in[i];
        }
    }
#endif

    /*
     * So the base is set to max-8, meaning
     * that we ignore really small values.
     * anyway, they will be 0 after shrinking to q7_t.
     */
    base = base - (1 << 3);

    sum = 0;

    for (i = 0; i < dim_vec; i++)
    {
        shift = (uint8_t)__USAT(vec_in[i] - base, 3);
        sum += 0x1 << shift;
    }

    /* This is effectively (0x1 << 20) / sum */
    int output_base = (1 << 20) / sum;

    for (i = 0; i < dim_vec; i++)
    {

        /* Here minimum value of 13+base-vec_in[i] will be 5 */
        shift = (uint8_t)__USAT(13 + base - vec_in[i], 5);
        p_out[i] = (q7_t)__SSAT((output_base >> shift), 8);
    }
}

/**
 * @} end of Softmax group
 */