/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_trans_q15.c
 * Description:  Q15 matrix transpose
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
  @addtogroup MatrixTrans
  @{
 */

/**
  @brief         Q15 matrix transpose.
  @param[in]     pSrc      points to input matrix
  @param[out]    pDst      points to output matrix
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed
 */
 
riscv_status riscv_mat_trans_q15(
  const riscv_matrix_instance_q15 * pSrc,
        riscv_matrix_instance_q15 * pDst)
{
        q15_t *pIn = pSrc->pData;                      /* input data matrix pointer */
        q15_t *pOut = pDst->pData;                     /* output data matrix pointer */
        uint16_t nRows = pSrc->numRows;                /* number of rows */
        uint16_t nCols = pSrc->numCols;                /* number of columns */
        uint32_t col, row = nRows, i = 0U;             /* Loop counters */
        riscv_status status;                             /* status of matrix transpose */

#if defined (RISCV_MATH_LOOPUNROLL)
#if __RISCV_XLEN == 64
        q63_t in64;                                      /* variable to hold temporary output  */
#else
        q31_t in;                                      /* variable to hold temporary output  */
#endif /* __RISCV_XLEN == 64 */
#endif

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((pSrc->numRows != pDst->numCols) ||
      (pSrc->numCols != pDst->numRows)   )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = nRows;
  size_t l,max_l;
  ptrdiff_t bstride = 2;  //  16bit/8bit = 2
  ptrdiff_t col_diff = bstride * nCols;  //Control the column width of the span
  uint16_t colnum;     //  How many rowumns are controlled
  vint16m8_t v_in;
  q15_t *pIn1;
  // max_l = vsetvl_e16m8(32);
    for(colnum = 0;colnum < nCols; colnum++)
    { 
      blkCnt = nRows;
      pIn1 = pIn;
      for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) 
      { 
        v_in = vlse16_v_i16m8(pIn, col_diff, l);
        vse16_v_i16m8 (pOut, v_in, l);
        // if(l == max_l)
        // {
        pIn = pIn+l*nCols;
        // }
        pOut = pOut+l;
      }
    pIn = pIn1;
    pIn = pIn+1;
    }
    /* Set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;
#else

  {
    /* Matrix transpose by exchanging the rows with columns */
    /* row loop */
    do
    {
      /* Pointer pOut is set to starting address of column being processed */
      pOut = pDst->pData + i;

#if defined (RISCV_MATH_LOOPUNROLL)

      /* Loop unrolling: Compute 4 outputs at a time */
      col = nCols >> 2U;

      while (col > 0U)        /* column loop */
      {
#if __RISCV_XLEN == 64
        in64 = read_q15x4_ia ((q15_t **) &pIn);

        *pOut = (q15_t) in64;
        /* Update pointer pOut to point to next row of transposed matrix */
        pOut += nRows;
        *pOut = (q15_t) ((in64 & (q31_t) 0xffff0000) >> 16);
        pOut += nRows;
        *pOut = (q15_t) (in64 >> 32);
        pOut += nRows;
        *pOut = (q15_t) (((in64 >> 32) & (q31_t) 0xffff0000) >> 16);
        pOut += nRows;

#else
        /* Read two elements from row */
        in = read_q15x2_ia ((q15_t **) &pIn);

        /* Unpack and store one element in  destination */
        *pOut = (q15_t) in;

        /* Update pointer pOut to point to next row of transposed matrix */
        pOut += nRows;

        /* Unpack and store second element in destination */
        *pOut = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

        /* Update  pointer pOut to point to next row of transposed matrix */
        pOut += nRows;

        /* Read two elements from row */
        in = read_q15x2_ia ((q15_t **) &pIn);

        /* Unpack and store one element in destination */
        *pOut = (q15_t) in;

        /* Update pointer pOut to point to next row of transposed matrix */
        pOut += nRows;

        /* Unpack and store second element in destination */
        *pOut = (q15_t) ((in & (q31_t) 0xffff0000) >> 16);

        /* Update pointer pOut to point to next row of transposed matrix */
        pOut += nRows;
#endif /* __RISCV_XLEN == 64 */
        /* Decrement column loop counter */
        col--;
      }

      /* Loop unrolling: Compute remaining outputs */
      col = nCols % 0x4U;

#else

      /* Initialize col with number of samples */
      col = nCols;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

      while (col > 0U)
      {
        /* Read and store input element in destination */
        *pOut = *pIn++;

        /* Update pointer pOut to point to next row of transposed matrix */
        pOut += nRows;

        /* Decrement column loop counter */
        col--;
      }

      i++;

      /* Decrement row loop counter */
      row--;

    } while (row > 0U);          /* row loop end */

    /* Set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;
  }

#endif /*defined(RISCV_VECTOR)*/

  /* Return to application */
  return (status);

}

/**
  @} end of MatrixTrans group
 */
