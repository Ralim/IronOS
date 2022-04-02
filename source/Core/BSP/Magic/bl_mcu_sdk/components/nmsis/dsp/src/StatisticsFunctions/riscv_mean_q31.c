/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mean_q31.c
 * Description:  Mean value of a Q31 vector
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
  @brief         Mean value of a Q31 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    mean value returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 64-bit internal accumulator.
                   The input is represented in 1.31 format and is accumulated in a 64-bit
                   accumulator in 33.31 format.
                   There is no risk of internal overflow with this approach, and the
                   full precision of intermediate result is preserved.
                   Finally, the accumulator is truncated to yield a result of 1.31 format.
 */
void riscv_mean_q31(
  const q31_t * pSrc,
        uint32_t blockSize,
        q31_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  const q31_t * input = pSrc;
  q31_t * result = pResult;
  q63_t sum;
  vint32m8_t v_in;
  l = vsetvl_e64m1(1);
  vint64m1_t v_sum = vmv_s_x_i64m1(v_sum, 0, l);                /* init v_sum data */
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
    v_in = vle32_v_i32m8(input, l);
    input += l;
    v_sum = vwredsum_vs_i32m8_i64m1(v_sum, v_in ,v_sum, l);
  }
  l = vsetvl_e64m1(1);
  sum = vmv_x_s_i64m1_i64(v_sum);
  * result = (q31_t) (sum / blockSize); 
#else
        uint32_t blkCnt;                               /* Loop counter */
        q63_t sum = 0;                                 /* Temporary result storage */
#if __RISCV_XLEN == 64
        q63_t valueA,valueB;                                 /* Temporary result storage */
#endif /* __RISCV_XLEN == 64 */
#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
#if __RISCV_XLEN == 64
    valueA = read_q31x2_ia ((q31_t **) &pSrc);
    valueB = read_q31x2_ia ((q31_t **) &pSrc);
    valueA = __RV_ADD32(valueA,valueB);
    sum += (((int64_t)__RV_CRAS32(valueA,valueA)) >> 32);
#else
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    sum += *pSrc++;

    sum += *pSrc++;

    sum += *pSrc++;

    sum += *pSrc++;
#endif /* __RISCV_XLEN == 64 */

    /* Decrement the loop counter */
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
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */
    sum += *pSrc++;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) / blockSize  */
  /* Store result to destination */
  *pResult = (q31_t) (sum / blockSize);
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of mean group
 */
