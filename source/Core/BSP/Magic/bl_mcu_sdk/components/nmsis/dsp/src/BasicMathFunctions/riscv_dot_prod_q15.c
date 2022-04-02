/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_dot_prod_q15.c
 * Description:  Q15 dot product
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

#include "dsp/basic_math_functions.h"

/**
  @ingroup groupMath
 */

/**
  @addtogroup BasicDotProd
  @{
 */

/**
  @brief         Dot product of Q15 vectors.
  @param[in]     pSrcA      points to the first input vector
  @param[in]     pSrcB      points to the second input vector
  @param[in]     blockSize  number of samples in each vector
  @param[out]    result     output result returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The intermediate multiplications are in 1.15 x 1.15 = 2.30 format and these
                   results are added to a 64-bit accumulator in 34.30 format.
                   Nonsaturating additions are used and given that there are 33 guard bits in the accumulator
                   there is no risk of overflow.
                   The return result is in 34.30 format.
 */
void riscv_dot_prod_q15(
  const q15_t * pSrcA,
  const q15_t * pSrcB,
        uint32_t blockSize,
        q63_t * result)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  const q15_t * inputA = pSrcA;
  const q15_t * inputB = pSrcB;
  q63_t * output = result;
  vint16m4_t v_inA;
  vint16m4_t v_inB;
  l = vsetvl_e64m1(1);
  vint64m1_t v_sum = vmv_s_x_i64m1(v_sum, 0, l);
  for (; (l = vsetvl_e16m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_inA = vle16_v_i16m4(inputA, l);
    v_inB = vle16_v_i16m4(inputB, l);
    inputA += l;
    inputB += l;
    v_sum = vwredsum_vs_i32m8_i64m1(v_sum, vwmul_vv_i32m8(v_inA, v_inB, l), v_sum, l);
  }
  l = vsetvl_e64m1(1);
  vse64_v_i64m1(output, v_sum, l);
#else
        uint32_t blkCnt;                               /* Loop counter */
        volatile q63_t sum = 0;                                 /* Temporary return variable */

#if defined (RISCV_MATH_LOOPUNROLL)
#if __RISCV_XLEN == 64
  /* Loop unrolling: Compute 8 outputs at a time */
  blkCnt = blockSize >> 3U;
#else
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;
#endif /* __RISCV_XLEN == 64 */
  while (blkCnt > 0U)
  {
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
	sum = __RV_SMALDA(sum, read_q15x4_ia ((q15_t **) &pSrcA), read_q15x4_ia ((q15_t **) &pSrcB));
	sum = __RV_SMALDA(sum, read_q15x4_ia ((q15_t **) &pSrcA), read_q15x4_ia ((q15_t **) &pSrcB));
#else
    /* Calculate dot product and store result in a temporary buffer. */
    //sum = __SMLALD(read_q15x2_ia ((q15_t **) &pSrcA), read_q15x2_ia ((q15_t **) &pSrcB), sum);
    //sum = __SMLALD(read_q15x2_ia ((q15_t **) &pSrcA), read_q15x2_ia ((q15_t **) &pSrcB), sum);
	sum = __RV_SMALDA(sum, read_q15x2_ia ((q15_t **) &pSrcA), read_q15x2_ia ((q15_t **) &pSrcB));
	sum = __RV_SMALDA(sum, read_q15x2_ia ((q15_t **) &pSrcA), read_q15x2_ia ((q15_t **) &pSrcB));
#endif /* __RISCV_XLEN == 64 */
#else
    sum += (q63_t)((q31_t) *pSrcA++ * *pSrcB++);
    sum += (q63_t)((q31_t) *pSrcA++ * *pSrcB++);
    sum += (q63_t)((q31_t) *pSrcA++ * *pSrcB++);
    sum += (q63_t)((q31_t) *pSrcA++ * *pSrcB++);
#endif

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */

    /* Calculate dot product and store result in a temporary buffer. */
#if defined (RISCV_MATH_DSP)
    sum  = __SMLALD((*pSrcA++) & 0xffff, (*pSrcB++) & 0xffff, sum);
#else
    sum += (q63_t)((q31_t) *pSrcA++ * *pSrcB++);
#endif

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store result in destination buffer in 34.30 format */
  *result = sum;
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicDotProd group
 */
