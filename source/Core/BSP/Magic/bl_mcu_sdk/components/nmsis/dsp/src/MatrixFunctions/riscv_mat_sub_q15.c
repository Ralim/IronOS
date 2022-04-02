/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_sub_q15.c
 * Description:  Q15 Matrix subtraction
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
  @addtogroup MatrixSub
  @{
 */

/**
  @brief         Q15 matrix subtraction.
  @param[in]     pSrcA      points to the first input matrix structure
  @param[in]     pSrcB      points to the second input matrix structure
  @param[out]    pDst       points to output matrix structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q15 range [0x8000 0x7FFF] are saturated.
 */
riscv_status riscv_mat_sub_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
        riscv_matrix_instance_q15 * pDst)
{
        q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A */
        q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B */
        q15_t *pOut = pDst->pData;                     /* output data matrix pointer */

        uint32_t numSamples;                           /* total number of elements in the matrix */
        uint32_t blkCnt;                               /* loop counters  */
        riscv_status status;                             /* status of matrix subtraction  */

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((pSrcA->numRows != pSrcB->numRows) ||
      (pSrcA->numCols != pSrcB->numCols) ||
      (pSrcA->numRows != pDst->numRows)  ||
      (pSrcA->numCols != pDst->numCols)    )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else
#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */
#if defined(RISCV_VECTOR)
    /* Total number of samples in input matrix */
  numSamples = (uint32_t) pSrcA->numRows * pSrcA->numCols;
  blkCnt = numSamples;
  size_t l;
  vint16m8_t vx, vy;
       
  for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle16_v_i16m8(pInA, l);
    pInA += l;
    vy = vle16_v_i16m8(pInB, l);
    vse16_v_i16m8 (pOut, vssub_vv_i16m8(vx, vy, l), l);
    pInB += l;
    pOut += l;
  }
#else
      /* Set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;

  {
    /* Total number of samples in input matrix */
    numSamples = (uint32_t) pSrcA->numRows * pSrcA->numCols;

#if defined (RISCV_MATH_LOOPUNROLL)

    /* Loop unrolling: Compute 4 outputs at a time */
    blkCnt = numSamples >> 2U;

    while (blkCnt > 0U)
    {
      /* C(m,n) = A(m,n) - B(m,n) */

      /* Subtract, Saturate and store result in destination buffer. */
#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
      write_q15x4_ia (&pOut, __RV_KSUB16(read_q15x4_ia((q15_t**)&pInA), read_q15x4_ia((q15_t **)&pInB)));
#else
      write_q15x2_ia (&pOut, __QSUB16(read_q15x2_ia ((q15_t **) &pInA), read_q15x2_ia ((q15_t **) &pInB)));
      write_q15x2_ia (&pOut, __QSUB16(read_q15x2_ia ((q15_t **) &pInA), read_q15x2_ia ((q15_t **) &pInB)));
#endif /* __RISCV_XLEN == 64 */
#else
      *pOut++ = (q15_t) __SSAT(((q31_t) * pInA++ - *pInB++), 16);
      *pOut++ = (q15_t) __SSAT(((q31_t) * pInA++ - *pInB++), 16);
      *pOut++ = (q15_t) __SSAT(((q31_t) * pInA++ - *pInB++), 16);
      *pOut++ = (q15_t) __SSAT(((q31_t) * pInA++ - *pInB++), 16);
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
      /* C(m,n) = A(m,n) - B(m,n) */

      /* Subtract and store result in destination buffer. */
#if defined (RISCV_MATH_DSP)
      *pOut++ = (q15_t) __QSUB16(*pInA++, *pInB++);
#else
      *pOut++ = (q15_t) __SSAT(((q31_t) * pInA++ - *pInB++), 16);
#endif

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
  @} end of MatrixSub group
 */
