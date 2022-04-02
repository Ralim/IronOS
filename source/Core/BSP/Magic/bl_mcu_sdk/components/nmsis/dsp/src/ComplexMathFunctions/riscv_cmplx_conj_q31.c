/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_conj_q31.c
 * Description:  Q31 complex conjugate
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
  @addtogroup cmplx_conj
  @{
 */

/**
  @brief         Q31 complex conjugate.
  @param[in]     pSrc        points to the input vector
  @param[out]    pDst        points to the output vector
  @param[in]     numSamples  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   The Q31 value -1 (0x80000000) is saturated to the maximum allowable positive value 0x7FFFFFFF.
 */


void riscv_cmplx_conj_q31(
  const q31_t * pSrc,
        q31_t * pDst,
        uint32_t numSamples)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = numSamples;                               /* Loop counter */
  size_t l;
  l = vsetvl_e32m8(blkCnt);
  const q31_t * input = pSrc;
  q31_t * output = pDst;
  ptrdiff_t bstride = 8;
  vint32m8_t v_R,v_I;
  vint32m8_t v0;
  v0 = vxor_vv_i32m8(v0, v0, l);                   /* vector 0 */
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_R = vlse32_v_i32m8(input, bstride, l);
    input++;
    vsse32_v_i32m8 (output, bstride, v_R, l);
    output++;
    v_I = vsub_vv_i32m8(v0, vlse32_v_i32m8(input, bstride, l), l);
    input += (l*2-1);
    vsse32_v_i32m8 (output, bstride, v_I, l);
    output += (l*2-1);
  }
  
#else
        uint32_t blkCnt;                               /* Loop counter */
        q31_t in;                                      /* Temporary input variable */

#if defined (RISCV_MATH_LOOPUNROLL)
#if __RISCV_XLEN == 64
q63_t in64;
#endif /* __RISCV_XLEN == 64 */
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
    /* C[0] + jC[1] = A[0]+ j(-1)A[1] */
#if __RISCV_XLEN == 64
    in64 = read_q31x2_ia ((q31_t **) &pSrc);
    write_q15x4_ia (&pDst, __RV_STSA32(0,in64));
    in64 = read_q31x2_ia ((q31_t **) &pSrc);
    write_q15x4_ia (&pDst, __RV_STSA32(0,in64));
    in64 = read_q31x2_ia ((q31_t **) &pSrc);
    write_q15x4_ia (&pDst, __RV_STSA32(0,in64));
    in64 = read_q31x2_ia ((q31_t **) &pSrc);
    write_q15x4_ia (&pDst, __RV_STSA32(0,in64));
#else
    /* Calculate Complex Conjugate and store result in destination buffer. */
    *pDst++ =  *pSrc++;
    in = *pSrc++;
#if defined (RISCV_MATH_DSP)
    *pDst++ = __QSUB(0, in);
#else
    *pDst++ = (in == INT32_MIN) ? INT32_MAX : -in;
#endif

    *pDst++ =  *pSrc++;
    in =  *pSrc++;
#if defined (RISCV_MATH_DSP)
    *pDst++ = __QSUB(0, in);
#else
    *pDst++ = (in == INT32_MIN) ? INT32_MAX : -in;
#endif

    *pDst++ =  *pSrc++;
    in = *pSrc++;
#if defined (RISCV_MATH_DSP)
    *pDst++ = __QSUB(0, in);
#else
    *pDst++ = (in == INT32_MIN) ? INT32_MAX : -in;
#endif

    *pDst++ =  *pSrc++;
    in = *pSrc++;
#if defined (RISCV_MATH_DSP)
    *pDst++ = __QSUB(0, in);
#else
    *pDst++ = (in == INT32_MIN) ? INT32_MAX : -in;
#endif
#endif /* __RISCV_XLEN == 64 */
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
    in = *pSrc++;
#if defined (RISCV_MATH_DSP)
    *pDst++ = __QSUB(0, in);
#else
    *pDst++ = (in == INT32_MIN) ? INT32_MAX : -in;
#endif

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of cmplx_conj group
 */
