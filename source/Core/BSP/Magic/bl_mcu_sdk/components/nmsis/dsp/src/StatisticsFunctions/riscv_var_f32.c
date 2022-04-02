/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_var_f32.c
 * Description:  Variance of the elements of a floating-point vector
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
  @defgroup variance  Variance

  Calculates the variance of the elements in the input vector.
  The underlying algorithm used is the direct method sometimes referred to as the two-pass method:

  <pre>
      Result = sum(element - meanOfElements)^2) / numElement - 1

      meanOfElements = ( pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + ... + pSrc[blockSize-1] ) / blockSize
  </pre>

  There are separate functions for floating point, Q31, and Q15 data types.
 */

/**
  @addtogroup variance
  @{
 */

/**
  @brief         Variance of the elements of a floating-point vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    variance value returned here
  @return        none
 */
void riscv_var_f32(
  const float32_t * pSrc,
        uint32_t blockSize,
        float32_t * pResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                   /* Loop counter */
  float32_t sum = 0.0f;                          /* Temporary result storage */
  float32_t fSum = 0.0f;
  float32_t fMean, fValue;
  size_t l;
  const float32_t * input = pSrc;
  float32_t * output = pResult;
  vfloat32m8_t v_in;                               /* Temporary variable to store input value */ 
  vfloat32m8_t v_fValue;
  l = vsetvl_e32m1(1);
  vfloat32m1_t v_fSum = vfmv_s_f_f32m1(v_fSum, 0.0f, l);
  vfloat32m1_t v_sum = vfmv_s_f_f32m1(v_sum, 0.0f, l);
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle32_v_f32m8(input, l);
    input += l;
    v_sum = vfredsum_vs_f32m8_f32m1(v_sum, v_in, v_sum, l);
  }
  l = vsetvl_e32m1(1);
  sum = vfmv_f_s_f32m1_f32(v_sum);
  /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) / blockSize  */
  fMean = sum / (float32_t) blockSize;
  input = pSrc;
  for (blkCnt = blockSize; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle32_v_f32m8(input, l);
    input += l;
    v_fValue = vfsub_vf_f32m8(v_in, fMean, l);
    v_fSum = vfredsum_vs_f32m8_f32m1(v_fSum, vfmul_vv_f32m8(v_fValue, v_fValue, l), v_fSum, l);
  }
  l = vsetvl_e32m1(1);
  fSum = vfmv_f_s_f32m1_f32(v_fSum);
   /* Variance */
  *output = fSum / (float32_t)(blockSize - 1.0f);
#else
        uint32_t blkCnt;                               /* Loop counter */
        float32_t sum = 0.0f;                          /* Temporary result storage */
        float32_t fSum = 0.0f;
        float32_t fMean, fValue;
  const float32_t * pInput = pSrc;

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
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */

    sum += *pInput++;
    sum += *pInput++;
    sum += *pInput++;
    sum += *pInput++;


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
    /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) */

    sum += *pInput++;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* C = (A[0] + A[1] + A[2] + ... + A[blockSize-1]) / blockSize  */
  fMean = sum / (float32_t) blockSize;

  pInput = pSrc;

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    fValue = *pInput++ - fMean;
    fSum += fValue * fValue;

    fValue = *pInput++ - fMean;
    fSum += fValue * fValue;

    fValue = *pInput++ - fMean;
    fSum += fValue * fValue;

    fValue = *pInput++ - fMean;
    fSum += fValue * fValue;

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
    fValue = *pInput++ - fMean;
    fSum += fValue * fValue;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Variance */
  *pResult = fSum / (float32_t)(blockSize - 1.0f);
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of variance group
 */
