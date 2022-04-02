/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_mag_f32.c
 * Description:  Floating-point complex magnitude
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

#include "dsp/complex_math_functions.h"

/**
  @ingroup groupCmplxMath
 */

/**
  @defgroup cmplx_mag Complex Magnitude

  Computes the magnitude of the elements of a complex data vector.

  The <code>pSrc</code> points to the source data and
  <code>pDst</code> points to the where the result should be written.
  <code>numSamples</code> specifies the number of complex samples
  in the input array and the data is stored in an interleaved fashion
  (real, imag, real, imag, ...).
  The input array has a total of <code>2*numSamples</code> values;
  the output array has a total of <code>numSamples</code> values.

  The underlying algorithm is used:

  <pre>
  for (n = 0; n < numSamples; n++) {
      pDst[n] = sqrt(pSrc[(2*n)+0]^2 + pSrc[(2*n)+1]^2);
  }
  </pre>

  There are separate functions for floating-point, Q15, and Q31 data types.
 */

/**
  @addtogroup cmplx_mag
  @{
 */

/**
  @brief         Floating-point complex magnitude.
  @param[in]     pSrc        points to input vector
  @param[out]    pDst        points to output vector
  @param[in]     numSamples  number of samples in each vector
  @return        none
 */


void riscv_cmplx_mag_f32(
  const float32_t * pSrc,
        float32_t * pDst,
        uint32_t numSamples)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = numSamples;                               /* Loop counter */
  size_t l;
  const float32_t * input = pSrc;
  float32_t * output = pDst;
  ptrdiff_t bstride = 8;
  vfloat32m8_t v_R,v_I;
  vfloat32m8_t vR2_m4, vI2_m4;
  vfloat32m8_t v_sum;
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_R = vlse32_v_f32m8(input, bstride, l);
    input++;
    v_I = vlse32_v_f32m8(input, bstride, l);
    input += (l*2-1);
    v_sum = vfadd_vv_f32m8(vfmul_vv_f32m8(v_R, v_R, l), vfmul_vv_f32m8(v_I, v_I, l), l);
    vse32_v_f32m8(output, vfsqrt_v_f32m8(v_sum, l), l);
    output += l;
  }

#else
  uint32_t blkCnt;                               /* loop counter */
  float32_t real, imag;                      /* Temporary variables to hold input values */


#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
    /* C[0] = sqrt(A[0] * A[0] + A[1] * A[1]) */

    real = *pSrc++;
    imag = *pSrc++;

    /* store result in destination buffer. */
    riscv_sqrt_f32((real * real) + (imag * imag), pDst++);

    real = *pSrc++;
    imag = *pSrc++;
    riscv_sqrt_f32((real * real) + (imag * imag), pDst++);

    real = *pSrc++;
    imag = *pSrc++;
    riscv_sqrt_f32((real * real) + (imag * imag), pDst++);

    real = *pSrc++;
    imag = *pSrc++;
    riscv_sqrt_f32((real * real) + (imag * imag), pDst++);

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining outputs */
  blkCnt = numSamples % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = numSamples;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C[0] = sqrt(A[0] * A[0] + A[1] * A[1]) */

    real = *pSrc++;
    imag = *pSrc++;

    /* store result in destination buffer. */
    riscv_sqrt_f32((real * real) + (imag * imag), pDst++);

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of cmplx_mag group
 */
