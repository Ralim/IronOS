/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mean_q7.c
 * Description:  Mean value of a Q7 vector
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
  @addtogroup mean
  @{
 */

/**
  @brief         Mean value of a Q7 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    mean value returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 32-bit internal accumulator.
                   The input is represented in 1.7 format and is accumulated in a 32-bit
                   accumulator in 25.7 format.
                   There is no risk of internal overflow with this approach, and the
                   full precision of intermediate result is preserved.
                   Finally, the accumulator is truncated to yield a result of 1.7 format.
 */

void riscv_mean_q7(
  const q7_t * pSrc,
        uint32_t blockSize,
        q7_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  const q7_t * input = pSrc;
  q7_t * result = pResult;
  q15_t sum;
  vint8m8_t v_in;
  l = vsetvl_e16m1(1);
  vint16m1_t v_sum = vmv_s_x_i16m1(v_sum, 0, l);                /* init v_sum data */
  for (; (l = vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
    v_in = vle8_v_i8m8(input, l);
    input += l;
    v_sum = vwredsum_vs_i8m8_i16m1(v_sum, v_in ,v_sum, l);
  }
  l = vsetvl_e16m1(1);
  sum = vmv_x_s_i16m1_i16(v_sum);
  * result = (q7_t) (sum / (int32_t) blockSize);
#else
        uint32_t blkCnt;                               /* Loop counter */
        q31_t sum = 0;                                 /* Temporary result storage */

#if defined (RISCV_MATH_LOOPUNROLL)
#if __RISCV_XLEN == 64
        q63_t in64;
#endif /* __RISCV_XLEN == 64 */
        q31_t in;
#endif

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
#if __RISCV_XLEN == 64
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    in64 = read_q7x8_ia ((q7_t **) &pSrc);
    sum += (q31_t)((in64 << 56U) >> 56U);
    sum += (q31_t)((in64 << 48U) >> 56U);
    sum += (q31_t)((in64 << 40U) >> 56U);
    sum += (q31_t)((in64 << 32U) >> 56U);
    sum += (q31_t)((in64 << 24U) >> 56U);
    sum += (q31_t)((in64 << 16U) >> 56U);
    sum += (q31_t)((in64 <<  8U) >> 56U);
    sum += (q31_t)((in64       ) >> 56U);
#else
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    in = read_q7x4_ia ((q7_t **) &pSrc);
    sum += ((in << 24U) >> 24U);
    sum += ((in << 16U) >> 24U);
    sum += ((in <<  8U) >> 24U);
    sum +=  (in >> 24U);
#endif /* __RISCV_XLEN == 64 */

    /* Decrement the loop counter */
    blkCnt--;
  }
#if __RISCV_XLEN == 64
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x8U;
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
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    sum += *pSrc++;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) / blockSize  */
  /* Store result to destination */
  *pResult = (q7_t) (sum / (int32_t) blockSize);
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of mean group
 */
