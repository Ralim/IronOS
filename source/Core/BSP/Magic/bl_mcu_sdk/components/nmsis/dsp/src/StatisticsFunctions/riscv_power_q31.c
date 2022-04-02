/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_power_q31.c
 * Description:  Sum of the squares of the elements of a Q31 vector
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

#include "dsp/statistics_functions.h"

/**
  @ingroup groupStats
 */

/**
  @addtogroup power
  @{
 */

/**
  @brief         Sum of the squares of the elements of a Q31 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    sum of the squares value returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 64-bit internal accumulator.
                   The input is represented in 1.31 format.
                   Intermediate multiplication yields a 2.62 format, and this
                   result is truncated to 2.48 format by discarding the lower 14 bits.
                   The 2.48 result is then added without saturation to a 64-bit accumulator in 16.48 format.
                   With 15 guard bits in the accumulator, there is no risk of overflow, and the
                   full precision of the intermediate multiplication is preserved.
                   Finally, the return result is in 16.48 format.
 */
void riscv_power_q31(
  const q31_t * pSrc,
        uint32_t blockSize,
        q63_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  const q31_t * input = pSrc;
  q63_t * output = pResult;
  vint32m4_t v_in;
  vint64m8_t v_in2;
  l = vsetvl_e64m1(1);
  vint64m1_t v_sum = vmv_s_x_i64m1(v_sum, 0, l);
  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle32_v_i32m4(input, l);
    input += l;
    v_in2 = vsra_vx_i64m8(vwmul_vv_i64m8(v_in, v_in, l), 14, l);
    v_sum = vredsum_vs_i64m8_i64m1(v_sum, v_in2, v_sum, l);
  }
  l = vsetvl_e64m1(1);
  vse64_v_i64m1(output, v_sum, l);
#else
        uint32_t blkCnt;                               /* Loop counter */
        q63_t sum = 0;                                 /* Temporary result storage */
        q31_t in;                                      /* Temporary variable to store input value */
#if __RISCV_XLEN == 64
        q63_t in64;                                      /* Temporary variable to store input value */
#endif /* __RISCV_XLEN == 64 */
#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */
#if __RISCV_XLEN == 64
    in64 = read_q31x2_ia ((q31_t **) &pSrc);
    sum += __RV_SMBB32(in64, in64);
    sum += __RV_SMTT32(in64, in64);
    in64 = read_q31x2_ia ((q31_t **) &pSrc);
    sum += __RV_SMBB32(in64, in64);
    sum += __RV_SMTT32(in64, in64);
#else
    /* Compute Power then shift intermediate results by 14 bits to maintain 16.48 format and store result in a temporary variable sum, providing 15 guard bits. */
    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14U;

    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14U;

    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14U;

    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14U;
#endif /* __RISCV_XLEN == 64 */

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
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */

    /* Compute Power and store result in a temporary variable, sum. */
    in = *pSrc++;
    sum += ((q63_t) in * in) >> 14U;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store results in 16.48 format */
  *pResult = sum;
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of power group
 */
