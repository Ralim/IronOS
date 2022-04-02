/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_mat_mult_q15.c
 * Description:  Q15 complex matrix multiplication
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
  @addtogroup CmplxMatrixMult
  @{
 */

/**
  @brief         Q15 Complex matrix multiplication.
  @param[in]     pSrcA      points to first input complex matrix structure
  @param[in]     pSrcB      points to second input complex matrix structure
  @param[out]    pDst       points to output complex matrix structure
  @param[in]     pScratch   points to an array for storing intermediate results
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed

  @par           Conditions for optimum performance
                   Input, output and state buffers should be aligned by 32-bit

  @par           Scaling and Overflow Behavior
                   The function is implemented using an internal 64-bit accumulator. The inputs to the
                   multiplications are in 1.15 format and multiplications yield a 2.30 result.
                   The 2.30 intermediate results are accumulated in a 64-bit accumulator in 34.30 format.
                   This approach provides 33 guard bits and there is no risk of overflow. The 34.30 result is then
                   truncated to 34.15 format by discarding the low 15 bits and then saturated to 1.15 format.
 */
riscv_status riscv_mat_cmplx_mult_q15(
  const riscv_matrix_instance_q15 * pSrcA,
  const riscv_matrix_instance_q15 * pSrcB,
        riscv_matrix_instance_q15 * pDst,
        q15_t                   * pScratch)
{
        q15_t *pSrcBT = pScratch;                      /* input data matrix pointer for transpose */
        q15_t *pInA = pSrcA->pData;                    /* input data matrix pointer A of Q15 type */
        q15_t *pInB = pSrcB->pData;                    /* input data matrix pointer B of Q15 type */
        q15_t *px;                                     /* Temporary output data matrix pointer */
        uint16_t numRowsA = pSrcA->numRows;            /* number of rows of input matrix A */
        uint16_t numColsB = pSrcB->numCols;            /* number of columns of input matrix B */
        uint16_t numColsA = pSrcA->numCols;            /* number of columns of input matrix A */
        uint16_t numRowsB = pSrcB->numRows;            /* number of rows of input matrix A */
        q63_t sumReal, sumImag;                        /* accumulator */
        uint32_t col, i = 0U, row = numRowsB, colCnt;  /* Loop counters */
        riscv_status status;                             /* Status of matrix multiplication */

#if defined (RISCV_MATH_DSP) && !defined (RISCV_VECTOR)
#if __RISCV_XLEN == 64
        q63_t prod164, prod264, pSourceA64, pSourceB64;
                q15_t a, b, c, d;
#endif /* __RISCV_XLEN == 64 */
        q31_t prod1, prod2;
        q31_t pSourceA, pSourceB;
#else
        q15_t a, b, c, d;
#endif /* #if defined (RISCV_MATH_DSP) */

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((pSrcA->numCols != pSrcB->numRows) ||
      (pSrcA->numRows != pDst->numRows)  ||
      (pSrcB->numCols != pDst->numCols)    )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */

#if defined(RISCV_VECTOR)
  q15_t *pIn1 = pSrcA->pData;                    /* Input data matrix pointer A */
  q15_t *pIn2 = pSrcB->pData;                    /* Input data matrix pointer B */
  uint16_t blkCnt = numColsA;  //number of matrix columns  numColsA = numrowB
  size_t l,max_l;              // max_l is the maximum column elements at a time
  ptrdiff_t bstride = 2;       //  16bit/8bit = 2
  ptrdiff_t reim_diff = bstride * 2;
  ptrdiff_t col_diff = reim_diff * numColsB;  //Control the column width of the span
  uint16_t colnum,rownum;      //  How many rowumns and rownum are controlled
  vint16m4_t v_inAR, v_inBR, v_inAI, v_inBI;
  vint32m8_t v_RR, v_II, v_RI, v_IR;
  vint32m8_t vReal, vImag;
  l = vsetvl_e16m1(1);
  vint64m1_t vsumReal = vmv_s_x_i64m1(vsumReal, 0, l);
  vint64m1_t vsumImag = vmv_s_x_i64m1(vsumImag, 0, l);
  // max_l = vsetvl_e16m4(32);
  q15_t *pOut = pDst->pData;                     /* Output data matrix pointer */
  px = pOut;
for(rownum = 0;rownum < numRowsA; rownum++)
  {
    pIn1 = pInA;       //backup pointer position
    for(colnum = 0;colnum < numColsB; colnum++)
    { 
      blkCnt = numColsA;
      pIn2 = pInB;     //backup pointer position
      sumReal = 0; 
      sumImag = 0; 
      l = vsetvl_e16m1(1);
      vsumReal = vmv_s_x_i64m1(vsumReal, 0, l);
      vsumImag = vmv_s_x_i64m1(vsumImag, 0, l);
      for (; (l = vsetvl_e16m4(blkCnt)) > 0; blkCnt -= l)   //Multiply a row by a column
      { 
        v_inAR = vlse16_v_i16m4(pInA, reim_diff, l);
        v_inBR = vlse16_v_i16m4(pInB, col_diff, l);
        pInA++; pInB++;
        v_inAI = vlse16_v_i16m4(pInA, reim_diff, l);
        v_inBI = vlse16_v_i16m4(pInB, col_diff, l);
        /* c(m,n) = a(1,1) * b(1,1) + a(1,2) * b(2,1) + .... + a(m,p) * b(p,n) */
        /* Perform multiply-accumulates */
        v_RR = vwmul_vv_i32m8(v_inAR , v_inBR, l);
        v_II = vwmul_vv_i32m8(v_inAI , v_inBI, l);
        v_RI = vwmul_vv_i32m8(v_inAR , v_inBI, l);
        v_IR = vwmul_vv_i32m8(v_inAI , v_inBR, l);
        vReal = vssub_vv_i32m8(v_RR, v_II, l);
        vImag = vsadd_vv_i32m8(v_RI, v_IR, l);
        vsumReal = vwredsum_vs_i32m8_i64m1(vsumReal, vReal, vsumReal, l);
        vsumImag = vwredsum_vs_i32m8_i64m1(vsumImag, vImag, vsumImag, l);
        // if(l == max_l)
        // {
        pInA = pInA+l*2-1;    //Pointer to the first element of the next line
        pInB = pInB+l*numColsB*4-1;
        // }
      }
      sumReal = vmv_x_s_i64m1_i64 (vsumReal);
      sumImag = vmv_x_s_i64m1_i64 (vsumImag);
      *px = (q15_t) __SSAT((sumReal >> 15), 16);
      px++;
      *px = (q15_t) __SSAT((sumImag >> 15), 16);
      px++;
      pInA = pIn1; 
      pInB = pIn2;pInB = pInB+2;    //Pointer to the first element of the next column for matrix BS
    //printf("px=%d\n",px);
    }
    pInB = pSrcB->pData;
    pInA = pIn1;pInA = pInA+numColsA*2;    //Pointer to the first element of the next row for matrix A 
  }
  /* Set status as RISCV_MATH_SUCCESS */
  status = RISCV_MATH_SUCCESS;
#else

  {
    /* Matrix transpose */
    do
    {
      /* The pointer px is set to starting address of column being processed */
      px = pSrcBT + i;

#if defined (RISCV_MATH_LOOPUNROLL)

      /* Apply loop unrolling and exchange the columns with row elements */
      col = numColsB >> 2;

      /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
         a second loop below computes the remaining 1 to 3 samples. */
      while (col > 0U)
      {
        /* Read two elements from row */
        write_q15x2 (px, read_q15x2_ia (&pInB));

        /* Update pointer px to point to next row of transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from row */
        write_q15x2 (px, read_q15x2_ia (&pInB));

        /* Update pointer px to point to next row of transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from row */
        write_q15x2 (px, read_q15x2_ia (&pInB));

        /* Update pointer px to point to next row of transposed matrix */
        px += numRowsB * 2;

        /* Read two elements from row */
        write_q15x2 (px, read_q15x2_ia (&pInB));

        /* Update pointer px to point to next row of transposed matrix */
        px += numRowsB * 2;

        /* Decrement column loop counter */
        col--;
      }

      /* If the columns of pSrcB is not a multiple of 4, compute any remaining output samples here.
       ** No loop unrolling is used. */
      col = numColsB % 0x4U;

#else

        /* Initialize blkCnt with number of samples */
        col = numColsB;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

      while (col > 0U)
      {
        /* Read two elements from row */
        write_q15x2 (px, read_q15x2_ia (&pInB));

        /* Update pointer px to point to next row of transposed matrix */
        px += numRowsB * 2;

        /* Decrement column loop counter */
        col--;
      }

      i = i + 2U;

      /* Decrement row loop counter */
      row--;

    } while (row > 0U);

    /* Reset variables for usage in following multiplication process */
    row = numRowsA;
    i = 0U;
    px = pDst->pData;

    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */
    /* row loop */
    do
    {
      /* For every row wise process, column loop counter is to be initiated */
      col = numColsB;

      /* For every row wise process, pIn2 pointer is set to starting address of transposed pSrcB data */
      pInB = pSrcBT;

      /* column loop */
      do
      {
        /* Set variable sum, that acts as accumulator, to zero */
        sumReal = 0;
        sumImag = 0;

        /* Initiate pointer pInA to point to starting address of column being processed */
        pInA = pSrcA->pData + i * 2;

        /* Apply loop unrolling and compute 2 MACs simultaneously. */
        colCnt = numColsA >> 1U;

        /* matrix multiplication */
        while (colCnt > 0U)
        {
          /* c(m,n) = a(1,1) * b(1,1) + a(1,2) * b(2,1) + .... + a(m,p) * b(p,n) */

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
          pSourceA64 = read_q15x4_ia ((q15_t **) &pInA);
          pSourceB64 = read_q15x4_ia ((q15_t **) &pInB);

          prod164 = __RV_SMDRS(pSourceA64, pSourceB64);
          prod264 = __RV_KMXDA(pSourceA64, pSourceB64);

          sumReal += (q63_t) ((q31_t) prod164);
          sumReal += (q63_t) ((q31_t) (prod164 >> 32));
          sumImag += (q63_t) ((q31_t) prod264);
          sumImag += (q63_t) ((q31_t) (prod264 >> 32));
#else
          /* read real and imag values from pSrcA and pSrcB buffer */
          pSourceA = read_q15x2_ia ((q15_t **) &pInA);
          pSourceB = read_q15x2_ia ((q15_t **) &pInB);

          /* Multiply and Accumlates */
          prod1 = __RV_SMDRS(pSourceA, pSourceB);
          prod2 = __RV_KMXDA(pSourceA, pSourceB);
          sumReal += (q63_t) prod1;
          sumImag += (q63_t) prod2;

          /* read real and imag values from pSrcA and pSrcB buffer */
          pSourceA = read_q15x2_ia ((q15_t **) &pInA);
          pSourceB = read_q15x2_ia ((q15_t **) &pInB);

          /* Multiply and Accumlates */
          prod1 = __RV_SMDRS(pSourceA, pSourceB);
          prod2 = __RV_KMXDA(pSourceA, pSourceB);
          sumReal += (q63_t) prod1;
          sumImag += (q63_t) prod2;
#endif /* __RISCV_XLEN == 64 */
#else /* #if defined (RISCV_MATH_DSP) */

          /* read real and imag values from pSrcA buffer */
          a = *pInA;
          b = *(pInA + 1U);
          /* read real and imag values from pSrcB buffer */
          c = *pInB;
          d = *(pInB + 1U);

          /* Multiply and Accumlates */
          sumReal += (q31_t) a *c;
          sumImag += (q31_t) a *d;
          sumReal -= (q31_t) b *d;
          sumImag += (q31_t) b *c;

          /* read next real and imag values from pSrcA buffer */
          a = *(pInA + 2U);
          b = *(pInA + 3U);
          /* read next real and imag values from pSrcB buffer */
          c = *(pInB + 2U);
          d = *(pInB + 3U);

          /* update pointer */
          pInA += 4U;

          /* Multiply and Accumlates */
          sumReal += (q31_t) a * c;
          sumImag += (q31_t) a * d;
          sumReal -= (q31_t) b * d;
          sumImag += (q31_t) b * c;
          /* update pointer */
          pInB += 4U;

#endif /* #if defined (RISCV_MATH_DSP) */

          /* Decrement loop counter */
          colCnt--;
        }

        /* process odd column samples */
        if ((numColsA & 0x1U) > 0U)
        {
          /* c(m,n) = a(1,1) * b(1,1) + a(1,2) * b(2,1) + .... + a(m,p) * b(p,n) */

#if defined (RISCV_MATH_DSP) && (__RISCV_XLEN != 64)
          /* read real and imag values from pSrcA and pSrcB buffer */
          pSourceA = read_q15x2_ia ((q15_t **) &pInA);
          pSourceB = read_q15x2_ia ((q15_t **) &pInB);

          /* Multiply and Accumlates */
          prod1 = __RV_SMDRS(pSourceA, pSourceB);
          prod2 = __RV_KMXDA(pSourceA, pSourceB);
          sumReal += (q63_t) prod1;
          sumImag += (q63_t) prod2;

#else /* #if defined (RISCV_MATH_DSP) */

          /* read real and imag values from pSrcA and pSrcB buffer */
          a = *pInA++;
          b = *pInA++;
          c = *pInB++;
          d = *pInB++;

          /* Multiply and Accumlates */
          sumReal += (q31_t) a * c;
          sumImag += (q31_t) a * d;
          sumReal -= (q31_t) b * d;
          sumImag += (q31_t) b * c;

#endif /* #if defined (RISCV_MATH_DSP) */

        }

        /* Saturate and store result in destination buffer */
        *px++ = (q15_t) (__SSAT(sumReal >> 15, 16));
        *px++ = (q15_t) (__SSAT(sumImag >> 15, 16));

        /* Decrement column loop counter */
        col--;

      } while (col > 0U);

      i = i + numColsA;

      /* Decrement row loop counter */
      row--;

    } while (row > 0U);

    /* Set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;
  }

#endif /*defined(RISCV_VECTOR)*/
  /* Return to application */
  return (status);
}

/**
  @} end of MatrixMult group
 */
