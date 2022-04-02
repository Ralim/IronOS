/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_mult_real_q31.c
 * Description:  Q31 complex by real multiplication
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
  @addtogroup CmplxByRealMult
  @{
 */

/**
  @brief         Q31 complex-by-real multiplication.
  @param[in]     pSrcCmplx   points to complex input vector
  @param[in]     pSrcReal    points to real input vector
  @param[out]    pCmplxDst   points to complex output vector
  @param[in]     numSamples  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q31 range[0x80000000 0x7FFFFFFF] are saturated.
 */

void riscv_cmplx_mult_real_q31(
  const q31_t * pSrcCmplx,
  const q31_t * pSrcReal,
        q31_t * pCmplxDst,
        uint32_t numSamples)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = numSamples;                               /* Loop counter */
  size_t l;
  const q31_t * input_c = pSrcCmplx;         /* Complex pointer */
  const q31_t * input_r = pSrcReal;          /* Real pointer */
  q31_t * output = pCmplxDst;
  ptrdiff_t bstride = 8;
  vint32m4_t v_Rc, v_Ic ,v_Rr;
  vint32m4_t vR2_m4, vI2_m4;
  vint32m4_t v_sum;
  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_Rc = vlse32_v_i32m4(input_c, bstride, l);
    input_c++;
    v_Ic = vlse32_v_i32m4(input_c, bstride, l);
    v_Rr = vle32_v_i32m4(input_r, l);
    input_r += l;
    input_c += (l*2-1);
    vR2_m4 = vnclip_wx_i32m4(vwmul_vv_i64m8(v_Rc, v_Rr, l), 31, l);
    vI2_m4 = vnclip_wx_i32m4(vwmul_vv_i64m8(v_Ic, v_Rr, l), 31, l);
    vsse32_v_i32m4(output, bstride, vR2_m4, l);
    output++;
    vsse32_v_i32m4(output, bstride, vI2_m4, l);
    output += (l*2-1);
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
        q31_t in;                                      /* Temporary variable */

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
    /* C[2 * i    ] = A[2 * i    ] * B[i]. */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i]. */

    in = *pSrcReal++;
#if defined (RISCV_MATH_DSP)
    /* store saturated result in 1.31 format to destination buffer */
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
#else
    /* store result in destination buffer. */
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
#endif

    in = *pSrcReal++;
#if defined (RISCV_MATH_DSP)
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
#else
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
#endif

    in = *pSrcReal++;
#if defined (RISCV_MATH_DSP)
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
#else
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
#endif

    in = *pSrcReal++;
#if defined (RISCV_MATH_DSP)
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
#else
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
#endif

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
    /* C[2 * i    ] = A[2 * i    ] * B[i]. */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i]. */

    in = *pSrcReal++;
#if defined (RISCV_MATH_DSP)
    /* store saturated result in 1.31 format to destination buffer */
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
    //*pCmplxDst++ = (__SSAT((q31_t) (((q63_t) *pSrcCmplx++ * in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
	*pCmplxDst++ = (__SSAT((q31_t) (__MULSR64(*pSrcCmplx++, in) >> 32), 31) << 1);
#else
    /* store result in destination buffer. */
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
    *pCmplxDst++ = (q31_t) clip_q63_to_q31(((q63_t) *pSrcCmplx++ * in) >> 31);
#endif

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of CmplxByRealMult group
 */
