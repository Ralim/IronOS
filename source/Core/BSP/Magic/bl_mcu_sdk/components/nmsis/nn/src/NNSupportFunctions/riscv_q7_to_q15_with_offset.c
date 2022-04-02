/*
 * Copyright (C) 2010-2020 Arm Limited or its affiliates. All rights reserved.
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in_q7x4 compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in_q7x4 writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_q7_to_q15_with_offset.c
 * Description:  Converts the elements of the Q7 vector to Q15 vector with an added offset
 *
 * $Date:        March 3, 2020
 * $Revision:    V.2.0.2
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup nndata_convert
 * @{
 */

void riscv_q7_to_q15_with_offset(const q7_t *src, q15_t *dst, uint32_t block_size, q15_t offset)
{
    int block_cnt;
#if defined (RISCV_VECTOR)
    uint32_t blkCnt = block_size;                              /* Loop counter */
    size_t l;
    q15_t *pCnt = dst;
    const q7_t *pV = src;
         
    for (; (l = vsetvl_e8m4(blkCnt)) > 0; blkCnt -= l) {
        vse16_v_i16m8 (pCnt, vadd_vx_i16m8(vwadd_vx_i16m8(vle8_v_i8m4(pV, l), 0, l), offset, l), l);
        pV += l;
        pCnt += l;
    }
#else
#if   defined(RISCV_MATH_DSP)
    /* Run the below code for cores that support SIMD instructions  */
    q31_t in_q7x4;
    q31_t in_q15x2_1;
    q31_t in_q15x2_2;

    /*loop unrolling */
    block_cnt = block_size >> 2;

    /* First part of the processing with loop unrolling.  Compute 4 outputs at a time. */
    const q31_t offset_q15x2 = __RV_PKBB16(offset, offset);
    while (block_cnt > 0)
    {
        in_q7x4 = riscv_nn_read_q7x4_ia(&src);

        in_q15x2_1 = __RV_ADD16(offset_q15x2, __RV_SUNPKD810(in_q7x4));
        in_q15x2_2 = __RV_ADD16(offset_q15x2, __RV_SUNPKD832(in_q7x4));

        riscv_nn_write_q15x2_ia(&dst, in_q15x2_1);
        riscv_nn_write_q15x2_ia(&dst, in_q15x2_2);

        block_cnt--;
    }
    /* Handle left over samples */
    block_cnt = block_size % 0x4;

#else
    /* Run the below code for RISC-V Core without DSP */
    /* Loop over block_size number of values */
    block_cnt = block_size;
#endif

    while (block_cnt > 0)
    {
        *dst++ = (q15_t)*src++ + offset;

        /* Decrement the loop counter */
        block_cnt--;
    }
#endif /*defined (RISCV_VECTOR)*/
}
/**
 * @} end of nndata_convert group
 */
