/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_scale_q31.c
 * Description:  Multiplies a Q31 matrix by a scalar
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

#include "dsp/matrix_functions.h"

/**
  @ingroup groupMatrix
 */

/**
  @addtogroup MatrixScale
  @{
 */

/**
  @brief         Q31 matrix scaling.
  @param[in]     pSrc        points to input matrix
  @param[in]     scaleFract  fractional portion of the scale factor
  @param[in]     shift       number of bits to shift the result by
  @param[out]    pDst        points to output matrix structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed

  @par           Scaling and Overflow Behavior
                   The input data <code>*pSrc</code> and <code>scaleFract</code> are in 1.31 format.
                   These are multiplied to yield a 2.62 intermediate result which is shifted with saturation to 1.31 format.
 */
riscv_status riscv_mat_scale_q31(
  const riscv_matrix_instance_q31 * pSrc,
        q31_t                     scaleFract,
        int32_t                   shift,
        riscv_matrix_instance_q31 * pDst)
{
  q31_t *pIn = pSrc->pData;                      /* Input data matrix pointer */
  q31_t *pOut = pDst->pData;                     /* Output data matrix pointer */
  uint32_t numSamples;                           /* Total number of elements in the matrix */
  uint32_t blkCnt;                               /* Loop counter */
  riscv_status status;                             /* Status of matrix scaling */
  int32_t kShift = shift + 1;                    /* Shift to apply after scaling */
  q31_t in, out;                                 /* Temporary variabels */

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((pSrc->numRows != pDst->numRows) ||
      (pSrc->numCols != pDst->numCols)   )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */
#if defined(RISCV_VECTOR)
    /* Total number of samples in input matrix */
  numSamples = (uint32_t) pSrc->numRows * pSrc->numCols;
  blkCnt = numSamples;
  size_t l;
  vint32m4_t v_in, v_out;
    for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) {
    v_in = vle32_v_i32m4(pIn, l);
    pIn += l;
    v_in = vnsra_wx_i32m4(vwmul_vx_i64m8(v_in, scaleFract, l),32U, l);
    v_out = vsll_vx_i32m4(v_in, (uint8_t)kShift, l);
    vse32_v_i32m4 (pOut, v_out, l);
    pOut += l;
  }
      /* Set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;
#else

  {
    /* Total number of samples in input matrix */
    numSamples = (uint32_t) pSrc->numRows * pSrc->numCols;

#if defined (RISCV_MATH_LOOPUNROLL)

    /* Loop unrolling: Compute 4 outputs at a time */
    blkCnt = numSamples >> 2U;

    while (blkCnt > 0U)
    {
      /* C(m,n) = A(m,n) * k */

      /* Scale, saturate and store result in destination buffer. */
      in = *pIn++;                                 /* read four inputs from source */
      in = ((q63_t) in * scaleFract) >> 32;        /* multiply input with scaler value */
      out = in << kShift;                          /* apply shifting */
      if (in != (out >> kShift))                   /* saturate the results. */
        out = 0x7FFFFFFF ^ (in >> 31);
      *pOut++ = out;                               /* Store result destination */

      in = *pIn++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
        out = 0x7FFFFFFF ^ (in >> 31);
      *pOut++ = out;

      in = *pIn++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
        out = 0x7FFFFFFF ^ (in >> 31);
      *pOut++ = out;

      in = *pIn++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
        out = 0x7FFFFFFF ^ (in >> 31);
      *pOut++ = out;

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
      /* C(m,n) = A(m,n) * k */

      /* Scale, saturate and store result in destination buffer. */
      in = *pIn++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
        out = 0x7FFFFFFF ^ (in >> 31);
      *pOut++ = out;

      /* Decrement loop counter */
      blkCnt--;
    }

    /* Set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;
  }

  /* Return to application */
  return (status);
#endif /*defined(RISCV_VECTOR)*/
}

/**
  @} end of MatrixScale group
 */
