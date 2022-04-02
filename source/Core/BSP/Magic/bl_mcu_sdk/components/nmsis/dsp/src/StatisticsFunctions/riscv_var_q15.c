/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_var_q15.c
 * Description:  Variance of an array of Q15 type
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
  @addtogroup variance
  @{
 */

/**
  @brief         Variance of the elements of a Q15 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    variance value returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 64-bit internal accumulator.
                   The input is represented in 1.15 format.
                   Intermediate multiplication yields a 2.30 format, and this
                   result is added without saturation to a 64-bit accumulator in 34.30 format.
                   With 33 guard bits in the accumulator, there is no risk of overflow, and the
                   full precision of the intermediate multiplication is preserved.
                   Finally, the 34.30 result is truncated to 34.15 format by discarding the lower
                   15 bits, and then saturated to yield a result in 1.15 format.
 */
void riscv_var_q15(
  const q15_t * pSrc,
        uint32_t blockSize,
        q15_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                   /* Loop counter */
  q31_t sum = 0;                                 /* Accumulator */
  q31_t meanOfSquares, squareOfMean;             /* Square of mean and mean of square */
  q63_t sumOfSquares = 0;                        /* Sum of squares */
  size_t l;
  const q15_t * input = pSrc;
  q15_t * output = pResult;
  vint16m4_t v_in;                               /* Temporary variable to store input value */  
  vint32m8_t v_in2;
  l = vsetvl_e64m1(1);
  vint64m1_t v_sumOfSquares = vmv_s_x_i64m1(v_sumOfSquares, 0, l);
  vint32m1_t v_sum = vmv_s_x_i32m1(v_sum, 0, l);
  for (; (l = vsetvl_e16m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle16_v_i16m4(input, l);
    input += l;
    v_in2 = vwmul_vv_i32m8(v_in, v_in, l);
    v_sum = vwredsum_vs_i16m4_i32m1(v_sum, v_in, v_sum, l);
    v_sumOfSquares = vwredsum_vs_i32m8_i64m1(v_sumOfSquares, v_in2, v_sumOfSquares, l);
  }
  l = vsetvl_e64m1(1);
  sum = vmv_x_s_i32m1_i32(v_sum);
  sumOfSquares = vmv_x_s_i64m1_i64(v_sumOfSquares);
  /* Compute Mean of squares and store result in a temporary variable, meanOfSquares. */
  meanOfSquares = (q31_t) (sumOfSquares / (q63_t)(blockSize - 1U));

  /* Compute square of mean */
  squareOfMean = (q31_t) ((q63_t) sum * sum / (q63_t)(blockSize * (blockSize - 1U)));

  /* mean of squares minus the square of mean. */
  *pResult = (meanOfSquares - squareOfMean) >> 15U;
#else
        uint32_t blkCnt;                               /* Loop counter */
        q31_t sum = 0;                                 /* Accumulator */
        q31_t meanOfSquares, squareOfMean;             /* Square of mean and mean of square */
        q63_t sumOfSquares = 0;                        /* Sum of squares */
        q15_t in;                                      /* Temporary variable to store input value */

#if defined (RISCV_MATH_LOOPUNROLL) && defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
        q63_t in64,sum64;
#endif /* __RISCV_XLEN == 64 */
        q31_t in32;                                    /* Temporary variable to store input value */
#endif

  if (blockSize <= 1U)
  {
    *pResult = 0;
    return;
  }

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A[0] * A[0] + A[1] * A[1] + ... + A[blockSize-1] * A[blockSize-1] */
    /* C = A[0] + A[1] + ... + A[blockSize-1] */

    /* Compute sum of squares and store result in a temporary variable, sumOfSquares. */
    /* Compute sum and store result in a temporary variable, sum. */
#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
    in64 = read_q15x4_ia ((q15_t **) &pSrc);
    sumOfSquares = __RV_SMALDA(sumOfSquares, in64, in64);
    sum += ((q31_t)((in64 << 48U) >> 48U));
    sum += ((q31_t)((in64 << 32U) >> 48U));
    sum += ((q31_t)((in64 << 16U) >> 48U));
    sum += ((q31_t)((in64       ) >> 48U));
#else
    in32 = read_q15x2_ia ((q15_t **) &pSrc);
    sumOfSquares = __SMLALD(in32, in32, sumOfSquares);
    sum += ((in32 << 16U) >> 16U);
    sum +=  (in32 >> 16U);

    in32 = read_q15x2_ia ((q15_t **) &pSrc);
    sumOfSquares = __SMLALD(in32, in32, sumOfSquares);
    sum += ((in32 << 16U) >> 16U);
    sum +=  (in32 >> 16U);
#endif /* __RISCV_XLEN == 64 */
#else
    in = *pSrc++;
    sumOfSquares += (in * in);
    sum += in;

    in = *pSrc++;
    sumOfSquares += (in * in);
    sum += in;

    in = *pSrc++;
    sumOfSquares += (in * in);
    sum += in;

    in = *pSrc++;
    sumOfSquares += (in * in);
    sum += in;
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
    /* C = A[0] + A[1] + ... + A[blockSize-1] */

    in = *pSrc++;
    /* Compute sum of squares and store result in a temporary variable, sumOfSquares. */
#if defined (RISCV_MATH_DSP)
    sumOfSquares = __SMLALD(in, in, sumOfSquares);
#else
    sumOfSquares += (in * in);
#endif /* #if defined (RISCV_MATH_DSP) */
    /* Compute sum and store result in a temporary variable, sum. */
    sum += in;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Compute Mean of squares and store result in a temporary variable, meanOfSquares. */
  meanOfSquares = (q31_t) (sumOfSquares / (q63_t)(blockSize - 1U));

  /* Compute square of mean */
  squareOfMean = (q31_t) ((q63_t) sum * sum / (q63_t)(blockSize * (blockSize - 1U)));

  /* mean of squares minus the square of mean. */
  *pResult = (meanOfSquares - squareOfMean) >> 15U;
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of variance group
 */
