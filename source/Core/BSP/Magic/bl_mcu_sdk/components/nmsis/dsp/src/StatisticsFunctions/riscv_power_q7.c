/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_power_q7.c
 * Description:  Sum of the squares of the elements of a Q7 vector
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
  @brief         Sum of the squares of the elements of a Q7 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    sum of the squares value returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 32-bit internal accumulator.
                   The input is represented in 1.7 format.
                   Intermediate multiplication yields a 2.14 format, and this
                   result is added without saturation to an accumulator in 18.14 format.
                   With 17 guard bits in the accumulator, there is no risk of overflow, and the
                   full precision of the intermediate multiplication is preserved.
                   Finally, the return result is in 18.14 format.
 */
void riscv_power_q7(
  const q7_t * pSrc,
        uint32_t blockSize,
        q31_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  const q7_t * input = pSrc;
  q31_t * output = pResult;
  vint8m4_t v_in;
  vint16m8_t v_in2;
  l = vsetvl_e32m1(1);
  vint32m1_t v_sum = vmv_s_x_i32m1(v_sum, 0, l);
  for (; (l = vsetvl_e8m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle8_v_i8m4(input, l);
    input += l;
    v_in2 = vwmul_vv_i16m8(v_in, v_in, l);
    v_sum = vwredsum_vs_i16m8_i32m1(v_sum, v_in2, v_sum, l);
  }
  l = vsetvl_e32m1(1);
  vse32_v_i32m1(output, v_sum, l);
#else
        uint32_t blkCnt;                               /* Loop counter */
        q31_t sum = 0;                                 /* Temporary result storage */
        q7_t in;                                       /* Temporary variable to store input value */

#if defined (RISCV_MATH_LOOPUNROLL) && defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
        q63_t in64,sum64 = 0;                                    /* Temporary variable to store packed input value */
#else
        q31_t in32;                                    /* Temporary variable to store packed input value */
#endif /* __RISCV_XLEN == 64 */
#endif

#if defined (RISCV_MATH_LOOPUNROLL)
#if __RISCV_XLEN == 64
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 3U;
#else
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;
#endif /* __RISCV_XLEN == 64 */

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */

    /* Compute Power and store result in a temporary variable, sum. */
#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
    in64 = read_q7x8_ia ((q7_t **) &pSrc);
    sum64 = __RV_SMAQA(sum64, in64, in64);
#else
    in32 = read_q7x4_ia ((q7_t **) &pSrc);
    sum = __RV_SMAQA(sum, in32, in32);
#endif /* __RISCV_XLEN == 64 */
#else
    in = *pSrc++;
    sum += ((q15_t) in * in);

    in = *pSrc++;
    sum += ((q15_t) in * in);

    in = *pSrc++;
    sum += ((q15_t) in * in);

    in = *pSrc++;
    sum += ((q15_t) in * in);
#endif /* #if defined (RISCV_MATH_DSP) */

    /* Decrement loop counter */
    blkCnt--;
  }
#if __RISCV_XLEN == 64
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x8U;
  sum = (q31_t) ((sum64 >> 32U) + ((sum64 << 32U) >> 32U));
#else
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;
#endif /* __RISCV_XLEN == 64 */

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */

    /* Compute Power and store result in a temporary variable, sum. */
    in = *pSrc++;
    sum += ((q15_t) in * in);

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store result in 18.14 format */
  *pResult = sum;
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of power group
 */
