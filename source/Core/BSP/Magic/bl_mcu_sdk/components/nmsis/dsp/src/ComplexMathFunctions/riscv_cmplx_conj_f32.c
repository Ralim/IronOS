/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_conj_f32.c
 * Description:  Floating-point complex conjugate
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
  @defgroup cmplx_conj Complex Conjugate

  Conjugates the elements of a complex data vector.

  The <code>pSrc</code> points to the source data and
  <code>pDst</code> points to the destination data where the result should be written.
  <code>numSamples</code> specifies the number of complex samples
  and the data in each array is stored in an interleaved fashion
  (real, imag, real, imag, ...).
  Each array has a total of <code>2*numSamples</code> values.

  The underlying algorithm is used:
  <pre>
  for (n = 0; n < numSamples; n++) {
      pDst[(2*n)  ] =  pSrc[(2*n)  ];    // real part
      pDst[(2*n)+1] = -pSrc[(2*n)+1];    // imag part
  }
  </pre>

  There are separate functions for floating-point, Q15, and Q31 data types.
 */

/**
  @addtogroup cmplx_conj
  @{
 */

/**
  @brief         Floating-point complex conjugate.
  @param[in]     pSrc        points to the input vector
  @param[out]    pDst        points to the output vector
  @param[in]     numSamples  number of samples in each vector
  @return        none
 */

void riscv_cmplx_conj_f32(
  const float32_t * pSrc,
        float32_t * pDst,
        uint32_t numSamples)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = numSamples;                               /* Loop counter */
  size_t l;
  l = vsetvl_e32m8(blkCnt);
  const float32_t * input = pSrc;
  float32_t * output = pDst;
  ptrdiff_t bstride = 8;
  vfloat32m8_t v_R,v_I;
  vfloat32m8_t v0;
  v0 = vfsub_vv_f32m8(v0, v0, l);                   /* vector 0 */
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_R = vlse32_v_f32m8(input, bstride, l);
    input++;
    vsse32_v_f32m8 (output, bstride, v_R, l);
    output++;
    v_I = vfsub_vv_f32m8(v0, vlse32_v_f32m8(input, bstride, l), l);
    input += (l*2-1);
    vsse32_v_f32m8 (output, bstride, v_I, l);
    output += (l*2-1);
  }
  
#else
        uint32_t blkCnt;                               /* Loop counter */

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
    /* C[0] + jC[1] = A[0]+ j(-1)A[1] */

    /* Calculate Complex Conjugate and store result in destination buffer. */
    *pDst++ =  *pSrc++;
    *pDst++ = -*pSrc++;

    *pDst++ =  *pSrc++;
    *pDst++ = -*pSrc++;

    *pDst++ =  *pSrc++;
    *pDst++ = -*pSrc++;

    *pDst++ =  *pSrc++;
    *pDst++ = -*pSrc++;

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
    /* C[0] + jC[1] = A[0]+ j(-1)A[1] */

    /* Calculate Complex Conjugate and store result in destination buffer. */
    *pDst++ =  *pSrc++;
    *pDst++ = -*pSrc++;

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of cmplx_conj group
 */
