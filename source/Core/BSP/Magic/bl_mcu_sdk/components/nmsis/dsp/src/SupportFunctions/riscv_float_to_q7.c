/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_float_to_q7.c
 * Description:  Converts the elements of the floating-point vector to Q7 vector
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

#include "dsp/support_functions.h"

/**
  @ingroup groupSupport
 */

/**
  @addtogroup float_to_x
  @{
 */

/**
 * @brief Converts the elements of the floating-point vector to Q7 vector.
 * @param[in]       *pSrc points to the floating-point input vector
 * @param[out]      *pDst points to the Q7 output vector
 * @param[in]       blockSize length of the input vector
 * @return none.
 *
 *\par Description:
 * \par
 * The equation used for the conversion process is:
 * <pre>
 * 	pDst[n] = (q7_t)(pSrc[n] * 128);   0 <= n < blockSize.
 * </pre>
 * \par Scaling and Overflow Behavior:
 * \par
 * The function uses saturating arithmetic.
 * Results outside of the allowable Q7 range [0x80 0x7F] will be saturated.
 * \note
 * In order to apply rounding, the library should be rebuilt with the ROUNDING macro
 * defined in the preprocessor section of project options.
 */
void riscv_float_to_q7(
  const float32_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                         /* Loop counter */
  const float32_t *pIn = pSrc;                         /* Source pointer */
  size_t l;
  vfloat32m8_t v_in;
  vint8m2_t v_out;
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle32_v_f32m8(pIn, l);
    pIn += l;
#ifdef RISCV_MATH_ROUNDING
    v_out = vnclip_wx_i8m2(vnclip_wx_i16m4(vfcvt_x_f_v_i32m8(vfmul_vf_f32m8(v_in, 128.0f, l), l), 0U, l), 0U, l);
#else
    v_out = vnclip_wx_i8m2(vnclip_wx_i16m4(vfcvt_rtz_x_f_v_i32m8(vfmul_vf_f32m8(v_in, 128.0f, l), l), 0U, l), 0U, l);
#endif
    vse8_v_i8m2 (pDst, v_out, l);
    pDst +=l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
  const float32_t *pIn = pSrc;                         /* Source pointer */

#ifdef RISCV_MATH_ROUNDING
        float32_t in;
#endif /* #ifdef RISCV_MATH_ROUNDING */

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A * 128 */

    /* Convert from float to q7 and store result in destination buffer */
#ifdef RISCV_MATH_ROUNDING

    in = (*pIn++ * 128);
    in += in > 0.0f ? 0.5f : -0.5f;
    *pDst++ = (q7_t) (__SSAT((q15_t) (in), 8));

    in = (*pIn++ * 128);
    in += in > 0.0f ? 0.5f : -0.5f;
    *pDst++ = (q7_t) (__SSAT((q15_t) (in), 8));

    in = (*pIn++ * 128);
    in += in > 0.0f ? 0.5f : -0.5f;
    *pDst++ = (q7_t) (__SSAT((q15_t) (in), 8));

    in = (*pIn++ * 128);
    in += in > 0.0f ? 0.5f : -0.5f;
    *pDst++ = (q7_t) (__SSAT((q15_t) (in), 8));

#else

    *pDst++ = __SSAT((q31_t) (*pIn++ * 128.0f), 8);
    *pDst++ = __SSAT((q31_t) (*pIn++ * 128.0f), 8);
    *pDst++ = __SSAT((q31_t) (*pIn++ * 128.0f), 8);
    *pDst++ = __SSAT((q31_t) (*pIn++ * 128.0f), 8);

#endif /* #ifdef RISCV_MATH_ROUNDING */

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
    /* C = A * 128 */

    /* Convert from float to q7 and store result in destination buffer */
#ifdef RISCV_MATH_ROUNDING

    in = (*pIn++ * 128);
    in += in > 0.0f ? 0.5f : -0.5f;
    *pDst++ = (q7_t) (__SSAT((q15_t) (in), 8));

#else

    *pDst++ = (q7_t) __SSAT((q31_t) (*pIn++ * 128.0f), 8);

#endif /* #ifdef RISCV_MATH_ROUNDING */

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of float_to_x group
 */
