/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_mult_q31.c
 * Description:  Q31 matrix multiplication
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
  @addtogroup MatrixMult
  @{
 */

/**
  @brief         Q31 matrix multiplication.
  @param[in]     pSrcA      points to the first input matrix structure
  @param[in]     pSrcB      points to the second input matrix structure
  @param[out]    pDst       points to output matrix structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed

  @par           Scaling and Overflow Behavior
                   The function is implemented using an internal 64-bit accumulator.
                   The accumulator has a 2.62 format and maintains full precision of the intermediate
                   multiplication results but provides only a single guard bit. There is no saturation
                   on intermediate additions. Thus, if the accumulator overflows it wraps around and
                   distorts the result. The input signals should be scaled down to avoid intermediate
                   overflows. The input is thus scaled down by log2(numColsA) bits
                   to avoid overflows, as a total of numColsA additions are performed internally.
                   The 2.62 accumulator is right shifted by 31 bits and saturated to 1.31 format to yield the final result.
  @remark
                   Refer to \ref riscv_mat_mult_fast_q31() for a faster but less precise implementation of this function.
 */
riscv_status riscv_mat_mult_q31(
  const riscv_matrix_instance_q31 * pSrcA,
  const riscv_matrix_instance_q31 * pSrcB,
        riscv_matrix_instance_q31 * pDst)
{
  q31_t *pIn1 = pSrcA->pData;                    /* Input data matrix pointer A */
  q31_t *pIn2 = pSrcB->pData;                    /* Input data matrix pointer B */
  q31_t *pInA = pSrcA->pData;                    /* Input data matrix pointer A */
  q31_t *pInB = pSrcB->pData;                    /* Input data matrix pointer B */
  q31_t *pOut = pDst->pData;                     /* Output data matrix pointer */
  q31_t *px;                                     /* Temporary output data matrix pointer */
  q63_t sum;                                     /* Accumulator */
  uint16_t numRowsA = pSrcA->numRows;            /* Number of rows of input matrix A */
  uint16_t numColsB = pSrcB->numCols;            /* Number of columns of input matrix B */
  uint16_t numColsA = pSrcA->numCols;            /* Number of columns of input matrix A */
  uint32_t col, i = 0U, row = numRowsA, colCnt;  /* Loop counters */
  riscv_status status;                             /* Status of matrix multiplication */
#if __RISCV_XLEN == 64
  q63_t temp164, temp264;                                     /* Accumulator */
#endif /* __RISCV_XLEN == 64 */
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
  uint16_t blkCnt = numColsA;  //number of matrix columns  numColsA = numrowB
  size_t l,max_l;              // max_l is the maximum column elements at a time
  ptrdiff_t bstride = 4;       //  32bit/8bit = 4
  ptrdiff_t col_diff = bstride * numColsB;  //Control the column width of the span
  uint16_t colnum,rownum;      //  How many rowumns and rownum are controlled
  vint32m4_t v_inA,v_inB;
  vint64m8_t vmul;
  l = vsetvl_e32m1(1);
  vint64m1_t vsum = vmv_s_x_i64m1(vsum, 0, l);
  // max_l = vsetvl_e32m4(32);
  px = pOut;
for(rownum = 0;rownum < numRowsA; rownum++)
  {
    pIn1 = pInA;       //backup pointer position
    for(colnum = 0;colnum < numColsB; colnum++)
    { 
      blkCnt = numColsA;
      pIn2 = pInB;     //backup pointer position
      sum = 0; 
      l = vsetvl_e32m1(1);
      vsum = vmv_s_x_i64m1(vsum, 0, l);
      for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l)   //Multiply a row by a column
      { 
        v_inA = vle32_v_i32m4(pInA, l);
        v_inB = vlse32_v_i32m4(pInB, col_diff, l);
        /* c(m,n) = a(1,1) * b(1,1) + a(1,2) * b(2,1) + .... + a(m,p) * b(p,n) */
        /* Perform multiply-accumulates */
        vmul = vwmul_vv_i64m8(v_inA, v_inB, l);
        vsum = vredsum_vs_i64m8_i64m1(vsum, vmul, vsum, l);
        //vsum = vmin_vx_i64m1(vmax_vx_i64m1(vredsum_vs_i64m8_i64m1(vsum, vmul, vsum) , 0xffffffff80000000),0x7fffffff);
        //sum = vmv_x_s_i64m1_i64 (vsum);
        // if(l == max_l)
        // {
        pInA = pInA+l;    //Pointer to the first element of the next line
        pInB = pInB+l*numColsB;
        // }
      }
      sum = vmv_x_s_i64m1_i64 (vsum);
      *px = (q31_t) (sum >> 31);
      px++;
      pInA = pIn1; 
      pInB = pIn2;pInB = pInB+1;    //Pointer to the first element of the next column for matrix BS
    //printf("px=%d\n",px);
    }
    pInB = pSrcB->pData;
    pInA = pIn1;pInA = pInA+numColsA;    //Pointer to the first element of the next row for matrix A 
  }
  /* Set status as RISCV_MATH_SUCCESS */
  status = RISCV_MATH_SUCCESS;
#else
  {
    /* The following loop performs the dot-product of each row in pSrcA with each column in pSrcB */
    /* row loop */
    do
    {
      /* Output pointer is set to starting address of row being processed */
      px = pOut + i;

      /* For every row wise process, column loop counter is to be initiated */
      col = numColsB;

      /* For every row wise process, pIn2 pointer is set to starting address of pSrcB data */
      pIn2 = pSrcB->pData;

      /* column loop */
      do
      {
        /* Set the variable sum, that acts as accumulator, to zero */
        sum = 0;

        /* Initialize pointer pIn1 to point to starting address of column being processed */
        pIn1 = pInA;

#if defined (RISCV_MATH_LOOPUNROLL)

        /* Loop unrolling: Compute 4 MACs at a time. */
        colCnt = numColsA >> 2U;

        /* matrix multiplication */
        while (colCnt > 0U)
        {
          /* c(m,n) = a(1,1) * b(1,1) + a(1,2) * b(2,1) + .... + a(m,p) * b(p,n) */
#if __RISCV_XLEN == 64
	        temp164 = read_q31x2_ia ((q31_t **) &pIn1);
          temp264 = ((q63_t) *pIn2) | ((q63_t) *pIn2 << 32);
          sum = __RV_KMADA32(sum, temp164, temp264);
#else
          /* Perform the multiply-accumulates */
          sum += (q63_t) *pIn1++ * *pIn2;
          pIn2 += numColsB;

          sum += (q63_t) *pIn1++ * *pIn2;
          pIn2 += numColsB;

          sum += (q63_t) *pIn1++ * *pIn2;
          pIn2 += numColsB;

          sum += (q63_t) *pIn1++ * *pIn2;
          pIn2 += numColsB;
#endif /* __RISCV_XLEN == 64 */

          /* Decrement loop counter */
          colCnt--;
        }

        /* Loop unrolling: Compute remaining MACs */
        colCnt = numColsA % 0x4U;

#else

        /* Initialize cntCnt with number of columns */
        colCnt = numColsA;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

        while (colCnt > 0U)
        {
          /* c(m,n) = a(1,1) * b(1,1) + a(1,2) * b(2,1) + .... + a(m,p) * b(p,n) */

          /* Perform the multiply-accumulates */
          sum += (q63_t) *pIn1++ * *pIn2;
          pIn2 += numColsB;

          /* Decrement loop counter */
          colCnt--;
        }

        /* Convert result from 2.62 to 1.31 format and store in destination buffer */
        *px++ = (q31_t) (sum >> 31);

        /* Decrement column loop counter */
        col--;

        /* Update pointer pIn2 to point to starting address of next column */
        pIn2 = pInB + (numColsB - col);

      } while (col > 0U);

      /* Update pointer pInA to point to starting address of next row */
      i = i + numColsB;
      pInA = pInA + numColsA;

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
