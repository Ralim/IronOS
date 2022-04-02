/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_dot_prod_q31.c
 * Description:  Q31 dot product
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
  @brief         Dot product of Q31 vectors.
  @param[in]     pSrcA      points to the first input vector.
  @param[in]     pSrcB      points to the second input vector.
  @param[in]     blockSize  number of samples in each vector.
  @param[out]    result     output result returned here.
  @return        none

  @par           Scaling and Overflow Behavior
                   The intermediate multiplications are in 1.31 x 1.31 = 2.62 format and these
                   are truncated to 2.48 format by discarding the lower 14 bits.
                   The 2.48 result is then added without saturation to a 64-bit accumulator in 16.48 format.
                   There are 15 guard bits in the accumulator and there is no risk of overflow as long as
                   the length of the vectors is less than 2^16 elements.
                   The return result is in 16.48 format.
 */

void riscv_dot_prod_q31(
  const q31_t * pSrcA,
  const q31_t * pSrcB,
        uint32_t blockSize,
        q63_t * result)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  const q31_t * inputA = pSrcA;
  const q31_t * inputB = pSrcB;
  q63_t * output = result;
  vint32m4_t v_inA;
  vint32m4_t v_inB;
  l = vsetvl_e64m1(1);
  vint64m1_t v_sum = vmv_s_x_i64m1(v_sum, 0, l);
  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_inA = vle32_v_i32m4(inputA, l);
    v_inB = vle32_v_i32m4(inputB, l);
    inputA += l;
    inputB += l;
    v_sum = vredsum_vs_i64m8_i64m1(v_sum, vsra_vx_i64m8(vwmul_vv_i64m8(v_inA, v_inB, l), 14, l), v_sum, l);
  }
  l = vsetvl_e64m1(1);
  vse64_v_i64m1(output, v_sum, l);
#else
        uint32_t blkCnt;                               /* Loop counter */
        q63_t sum = 0;                                 /* Temporary return variable */

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A[0]* B[0] + A[1]* B[1] + A[2]* B[2] + .....+ A[blockSize-1]* B[blockSize-1] */

    /* Calculate dot product and store result in a temporary buffer. */
#if defined(RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
#else
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
	sum += (__RV_MULSR64(*pSrcA++, *pSrcB++) >> 14);
#endif /* __RISCV_XLEN == 64 */
#else
    sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
    sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
    sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
    sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
#endif
	  //sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
	  //sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
	  //sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
	  //sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;

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
#if defined(RISCV_MATH_DSP)
	sum += (__MULSR64(*pSrcA++, *pSrcB++) >> 14);
#else
    sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;
#endif
	  //sum += ((q63_t) *pSrcA++ * *pSrcB++) >> 14U;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store result in destination buffer in 16.48 format */
  *result = sum;
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicDotProd group
 */
