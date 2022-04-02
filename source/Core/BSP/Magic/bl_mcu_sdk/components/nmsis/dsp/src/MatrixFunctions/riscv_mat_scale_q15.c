/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_scale_q15.c
 * Description:  Multiplies a Q15 matrix by a scalar
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
  @brief         Q15 matrix scaling.
  @param[in]     pSrc        points to input matrix
  @param[in]     scaleFract  fractional portion of the scale factor
  @param[in]     shift       number of bits to shift the result by
  @param[out]    pDst        points to output matrix structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed

  @par           Scaling and Overflow Behavior
                   The input data <code>*pSrc</code> and <code>scaleFract</code> are in 1.15 format.
                   These are multiplied to yield a 2.30 intermediate result and this is shifted with saturation to 1.15 format.
 */
riscv_status riscv_mat_scale_q15(
  const riscv_matrix_instance_q15 * pSrc,
        q15_t                     scaleFract,
        int32_t                   shift,
        riscv_matrix_instance_q15 * pDst)
{
        q15_t *pIn = pSrc->pData;                      /* Input data matrix pointer */
        q15_t *pOut = pDst->pData;                     /* Output data matrix pointer */
        uint32_t numSamples;                           /* Total number of elements in the matrix */
        uint32_t blkCnt;                               /* Loop counter */
        riscv_status status;                             /* Status of matrix scaling */
        int32_t kShift = 15 - shift;                   /* Total shift to apply after scaling */

#if defined (RISCV_MATH_DSP) && (__RISCV_XLEN == 64)
        q63_t inA164;
#endif /* __RISCV_XLEN == 64 */
#if defined (RISCV_MATH_LOOPUNROLL) && defined (RISCV_MATH_DSP)
        q31_t inA1, inA2;
        q31_t out1, out2, out3, out4;                  /* Temporary output variables */
        q15_t in1, in2, in3, in4;                      /* Temporary input variables */
#endif

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
  vint16m4_t vx;  
  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) {
    vx = vle16_v_i16m4(pIn, l);
    pIn += l;
    //vse16_v_i16m4 (pOut, vnsra_wx_i16m4(vmin_vx_i32m8(vmax_vx_i32m8(vwmul_vx_i32m8(vx, scaleFract), 0xffff8000), 0x7fff), kShift));
    vse16_v_i16m4 (pOut, vnclip_wx_i16m4(vwmul_vx_i32m8(vx, scaleFract, l), kShift, l), l);
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

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
      inA164 = read_q15x4_ia ((q15_t **) &pIn);

      /* Scale inputs and store result in temporary variables
       * in single cycle by packing the outputs */
      out1 = (q31_t) ((q15_t) (inA164 >> 16) * scaleFract);
      out2 = (q31_t) ((q15_t) (inA164      ) * scaleFract);
      out3 = (q31_t) ((q15_t) (inA164 >> 48) * scaleFract);
      out4 = (q31_t) ((q15_t) (inA164 >> 32) * scaleFract);
#else
      /* read 2 times 2 samples at a time from source */
      inA1 = read_q15x2_ia ((q15_t **) &pIn);
      inA2 = read_q15x2_ia ((q15_t **) &pIn);

      /* Scale inputs and store result in temporary variables
       * in single cycle by packing the outputs */
      out1 = (q31_t) ((q15_t) (inA1 >> 16) * scaleFract);
      out2 = (q31_t) ((q15_t) (inA1      ) * scaleFract);
      out3 = (q31_t) ((q15_t) (inA2 >> 16) * scaleFract);
      out4 = (q31_t) ((q15_t) (inA2      ) * scaleFract);
#endif /* __RISCV_XLEN == 64 */

      /* apply shifting */
      out1 = out1 >> kShift;
      out2 = out2 >> kShift;
      out3 = out3 >> kShift;
      out4 = out4 >> kShift;

      /* saturate the output */
      in1 = (q15_t) (__SSAT(out1, 16));
      in2 = (q15_t) (__SSAT(out2, 16));
      in3 = (q15_t) (__SSAT(out3, 16));
      in4 = (q15_t) (__SSAT(out4, 16));

      /* store result to destination */
      write_q15x2_ia (&pOut, __PKHBT(in2, in1, 16));
      write_q15x2_ia (&pOut, __PKHBT(in4, in3, 16));

#else
      *pOut++ = (q15_t) (__SSAT(((q31_t) (*pIn++) * scaleFract) >> kShift, 16));
      *pOut++ = (q15_t) (__SSAT(((q31_t) (*pIn++) * scaleFract) >> kShift, 16));
      *pOut++ = (q15_t) (__SSAT(((q31_t) (*pIn++) * scaleFract) >> kShift, 16));
      *pOut++ = (q15_t) (__SSAT(((q31_t) (*pIn++) * scaleFract) >> kShift, 16));
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
      /* C(m,n) = A(m,n) * k */

      /* Scale, saturate and store result in destination buffer. */
      *pOut++ = (q15_t) (__SSAT(((q31_t) (*pIn++) * scaleFract) >> kShift, 16));

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
