/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_var_q31.c
 * Description:  Variance of an array of Q31 type
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
  @brief         Variance of the elements of a Q31 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    variance value returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using an internal 64-bit accumulator.
                   The input is represented in 1.31 format, which is then downshifted by 8 bits
                   which yields 1.23, and intermediate multiplication yields a 2.46 format.
                   The accumulator maintains full precision of the intermediate multiplication results,
                   and as a consequence has only 16 guard bits.
                   There is no saturation on intermediate additions.
                   If the accumulator overflows it wraps around and distorts the result.
                   In order to avoid overflows completely the input signal must be scaled down by
                   log2(blockSize)-8 bits, as a total of blockSize additions are performed internally.
                   After division, internal variables should be Q18.46
                   Finally, the 18.46 accumulator is right shifted by 15 bits to yield a 1.31 format value.
 */
void riscv_var_q31(
  const q31_t * pSrc,
        uint32_t blockSize,
        q31_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                   /* Loop counter */
  q63_t sum = 0;                                 /* Temporary result storage */
  q63_t meanOfSquares, squareOfMean;             /* Square of mean and mean of square */
  q63_t sumOfSquares = 0;                        /* Sum of squares */
  size_t l;
  const q31_t * input = pSrc;
  q31_t * output = pResult;
  vint32m4_t v_in;                               /* Temporary variable to store input value */  
  vint64m8_t v_in2;
  l = vsetvl_e64m1(1);
  vint64m1_t v_sumOfSquares = vmv_s_x_i64m1(v_sumOfSquares, 0, l);
  vint64m1_t v_sum = vmv_s_x_i64m1(v_sum, 0, l);
  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vsra_vx_i32m4(vle32_v_i32m4(input, l), 8U, l);
    input += l;
    v_in2 = vwmul_vv_i64m8(v_in, v_in, l);
    v_sum = vwredsum_vs_i32m4_i64m1(v_sum, v_in, v_sum, l);
    v_sumOfSquares = vredsum_vs_i64m8_i64m1(v_sumOfSquares, v_in2, v_sumOfSquares, l);
  }
  l = vsetvl_e64m1(1);
  sum = vmv_x_s_i64m1_i64(v_sum);
  sumOfSquares = vmv_x_s_i64m1_i64(v_sumOfSquares);
  /* Compute Mean of squares and store result in a temporary variable, meanOfSquares. */
  meanOfSquares = (sumOfSquares / (q63_t)(blockSize - 1U));

  /* Compute square of mean */
  squareOfMean = ( sum * sum / (q63_t)(blockSize * (blockSize - 1U)));

  /* Compute variance and store result in destination */
  *output = (meanOfSquares - squareOfMean) >> 15U;
#else
        uint32_t blkCnt;                               /* Loop counter */
        q63_t sum = 0;                                 /* Temporary result storage */
        q63_t meanOfSquares, squareOfMean;             /* Square of mean and mean of square */
        q63_t sumOfSquares = 0;                        /* Sum of squares */
        q31_t in;                                      /* Temporary variable to store input value */
#if __RISCV_XLEN == 64
        q63_t in64A,in64B;                                      /* Temporary variable to store input value */
#endif /* __RISCV_XLEN == 64 */
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
#if __RISCV_XLEN == 64
    in64A = read_q31x2_ia ((q31_t **) &pSrc);
    in64A = (((int64_t)(in64A << 32)) >> 40) | ((((int64_t) in64A) >> 40) << 32);
    sumOfSquares += ((q63_t)__RV_SMBB32(in64A, in64A) >> 32);
    sumOfSquares += __RV_SMTT32(in64A, in64A);
    in64B = read_q31x2_ia ((q31_t **) &pSrc);
    in64B = (((int64_t)(in64B << 32)) >> 40) | ((((int64_t) in64B) >> 40) << 32);
    sumOfSquares += ((q63_t)__RV_SMBB32(in64B, in64B) >> 32);
    sumOfSquares += __RV_SMTT32(in64B, in64B);

    in64A = __RV_ADD32(in64A,in64B);
    sum += (((int64_t)__RV_CRAS32(in64A,in64A)) >> 32);

#else
    in = *pSrc++ >> 8U;
    /* Compute sum of squares and store result in a temporary variable, sumOfSquares. */
    sumOfSquares += ((q63_t) (in) * (in));
    /* Compute sum and store result in a temporary variable, sum. */
    sum += in;

    in = *pSrc++ >> 8U;
    sumOfSquares += ((q63_t) (in) * (in));
    sum += in;

    in = *pSrc++ >> 8U;
    sumOfSquares += ((q63_t) (in) * (in));
    sum += in;

    in = *pSrc++ >> 8U;
    sumOfSquares += ((q63_t) (in) * (in));
    sum += in;
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
    /* C = A[0] + A[1] + ... + A[blockSize-1] */

    in = *pSrc++ >> 8U;
    /* Compute sum of squares and store result in a temporary variable, sumOfSquares. */
    sumOfSquares += ((q63_t) (in) * (in));
    /* Compute sum and store result in a temporary variable, sum. */
    sum += in;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Compute Mean of squares and store result in a temporary variable, meanOfSquares. */
  meanOfSquares = (sumOfSquares / (q63_t)(blockSize - 1U));

  /* Compute square of mean */
  squareOfMean = ( sum * sum / (q63_t)(blockSize * (blockSize - 1U)));

  /* Compute variance and store result in destination */
  *pResult = (meanOfSquares - squareOfMean) >> 15U;
#endif /* defined(RISCV_VECTOR) */
}
/**
  @} end of variance group
 */
