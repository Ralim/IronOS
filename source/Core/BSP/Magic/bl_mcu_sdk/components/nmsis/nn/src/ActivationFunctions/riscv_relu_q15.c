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
 * Title:        riscv_relu_q15.c
 * Description:  Q15 version of ReLU
 *
 * $Date:        09. October 2020
 * $Revision:    V.1.0.2
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnfunctions.h"
#include "riscv_nnsupportfunctions.h"

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup Acti
 * @{
 */

/**
 * @brief Q15 RELU function
 * @param[in,out]   data        pointer to input
 * @param[in]       size        number of elements
 *
 * @details
 *
 * Optimized relu with QSUB instructions.
 *
 */

void riscv_relu_q15(q15_t *data, uint16_t size)
{
#if defined(RISCV_VECTOR)
  uint16_t blkCnt = size;                               /* Loop counter */
  size_t l;
  vint16m8_t vx;
  int16_t vy = 0;
  for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle16_v_i16m8(data, l);
    // if data >= zero, return data, else return zero
    vse16_v_i16m8 (data, vmax_vx_i16m8(vx, vy, l), l);
    data += l;
  }
#else
#if defined(RISCV_MATH_DSP)
    /* Run the following code for M cores with DSP extension */

    q15_t    *input = data;
    q15_t    *output = data;
    q31_t     in;
    q31_t     buf;
    q31_t     mask;
    q31_t      zero;
#if __RISCV_XLEN == 64
    uint16_t  i = size >> 2;
    q63_t in64;
    while (i)
    {
        in64 = *__SIMD64(input)++;

        *__SIMD64(output)++ = __RV_SMAX16 (in64,zero);

        i--;
    }
    i = size & 0x3;
    while (i)
    {
        if (*input < 0)
        {
            *input = 0;
        }
        input++;
        i--;
    }
#else
    uint16_t  i = size >> 1;
    while (i)
    {


        // in = *__SIMD32(input)++;

        //*__SIMD32(output)++ = __SMAX8 (in,zero);

        in = *__SIMD32(input)++;

        /* extract the first bit */
        //buf = __ROR(in & 0x80008000, 15);

        /* if MSB=1, mask will be 0xFF, 0x0 otherwise */
       // mask = __QSUB16(0x00000000, buf);
        *__SIMD32(output)++ = __RV_SMAX16 (in,zero);
       // *__SIMD32(output)++ = in & (~mask);
        i--;
    }

    if (size & 0x1)
    {
        if (*input < 0)
        {
            *input = 0;
        }
        input++;
    }
#endif /* __RISCV_XLEN == 64 */
#else
    /* Run the following code as reference implementation for M cores without DSP extension */
    uint16_t i;

    for (i = 0; i < size; i++)
    {
        if (data[i] < 0)
            data[i] = 0;
    }

#endif /* RISCV_MATH_DSP */
#endif /* RISCV_VECTOR */
}

/**
 * @} end of Acti group
 */
