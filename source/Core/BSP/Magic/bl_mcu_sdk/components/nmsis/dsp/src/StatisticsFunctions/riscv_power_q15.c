/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_power_q15.c
 * Description:  Sum of the squares of the elements of a Q15 vector
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
  @brief         Sum of the squares of the elements of a Q15 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    sum of the squares value returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 64-bit internal accumulator.
                   The input is represented in 1.15 format.
                   Intermediate multiplication yields a 2.30 format, and this
                   result is added without saturation to a 64-bit accumulator in 34.30 format.
                   With 33 guard bits in the accumulator, there is no risk of overflow, and the
                   full precision of the intermediate multiplication is preserved.
                   Finally, the return result is in 34.30 format.
 */
void riscv_power_q15(
  const q15_t * pSrc,
        uint32_t blockSize,
        q63_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  const q15_t * input = pSrc;
  q63_t * output = pResult;
  vint16m4_t v_in;
  vint32m8_t v_in2;
  l = vsetvl_e64m1(1);
  vint64m1_t v_sum = vmv_s_x_i64m1(v_sum, 0, l);
  for (; (l = vsetvl_e16m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle16_v_i16m4(input, l);
    input += l;
    v_in2 = vwmul_vv_i32m8(v_in, v_in, l);
    v_sum = vwredsum_vs_i32m8_i64m1(v_sum, v_in2, v_sum, l);
  }
  l = vsetvl_e64m1(1);
  vse64_v_i64m1(output, v_sum, l);
#else
        uint32_t blkCnt;                               /* Loop counter */
        q63_t sum = 0;                                 /* Temporary result storage */
        q15_t in;                                      /* Temporary variable to store input value */

#if defined (RISCV_MATH_LOOPUNROLL) && defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
        q63_t in64;                                    /* Temporary variable to store packed input value */
#endif /* __RISCV_XLEN == 64 */
        q31_t in32;                                    /* Temporary variable to store packed input value */
#endif

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */

    /* Compute Power and store result in a temporary variable, sum. */
#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
    in64 = read_q15x4_ia ((q15_t **) &pSrc);
    sum = __RV_SMALDA(sum, in64, in64);
#else
    in32 = read_q15x2_ia ((q15_t **) &pSrc);
    sum = __RV_SMALDA(sum, in32, in32);

    in32 = read_q15x2_ia ((q15_t **) &pSrc);
    sum = __RV_SMALDA(sum, in32, in32);
#endif /* __RISCV_XLEN == 64 */
#else
    in = *pSrc++;
    sum += ((q31_t) in * in);

    in = *pSrc++;
    sum += ((q31_t) in * in);

    in = *pSrc++;
    sum += ((q31_t) in * in);

    in = *pSrc++;
    sum += ((q31_t) in * in);
#endif /* #if defined (RISCV_MATH_DSP) */

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
    sum += ((q31_t) in * in);

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store result in 34.30 format */
  *pResult = sum;
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of power group
 */
