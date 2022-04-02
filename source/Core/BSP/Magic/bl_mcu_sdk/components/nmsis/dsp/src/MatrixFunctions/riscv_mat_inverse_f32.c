/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_inverse_f32.c
 * Description:  Floating-point matrix inverse
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
  @defgroup MatrixInv Matrix Inverse

  Computes the inverse of a matrix.

  The inverse is defined only if the input matrix is square and non-singular (the determinant is non-zero).
  The function checks that the input and output matrices are square and of the same size.

  Matrix inversion is numerically sensitive and the NMSIS DSP library only supports matrix
  inversion of floating-point matrices.

  @par Algorithm
  The Gauss-Jordan method is used to find the inverse.
  The algorithm performs a sequence of elementary row-operations until it
  reduces the input matrix to an identity matrix. Applying the same sequence
  of elementary row-operations to an identity matrix yields the inverse matrix.
  If the input matrix is singular, then the algorithm terminates and returns error status
  <code>RISCV_MATH_SINGULAR</code>.
  \image html MatrixInverse.png "Matrix Inverse of a 3 x 3 matrix using Gauss-Jordan Method"
 */

/**
  @addtogroup MatrixInv
  @{
 */

/**
  @brief         Floating-point matrix inverse.
  @param[in]     pSrc      points to input matrix structure. The source matrix is modified by the function.
  @param[out]    pDst      points to output matrix structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed
                   - \ref RISCV_MATH_SINGULAR      : Input matrix is found to be singular (non-invertible)
 */
riscv_status riscv_mat_inverse_f32(
  const riscv_matrix_instance_f32 * pSrc,
        riscv_matrix_instance_f32 * pDst)
{
  float32_t *pIn = pSrc->pData;                  /* input data matrix pointer */
  float32_t *pOut = pDst->pData;                 /* output data matrix pointer */
  float32_t *pInT1, *pInT2;                      /* Temporary input data matrix pointer */
  float32_t *pOutT1, *pOutT2;                    /* Temporary output data matrix pointer */
  float32_t *pPivotRowIn, *pPRT_in, *pPivotRowDst, *pPRT_pDst;  /* Temporary input and output data matrix pointer */
  uint32_t numRows = pSrc->numRows;              /* Number of rows in the matrix  */
  uint32_t numCols = pSrc->numCols;              /* Number of Cols in the matrix  */
#if defined (RISCV_VECTOR)
  uint32_t blkCnt;                               /* loop counters */
  size_t vl;
  vfloat32m8_t vx, vy;
#endif

#if defined (RISCV_MATH_DSP) && !defined (RISCV_VECTOR)

  float32_t Xchg, in = 0.0f, in1;                /* Temporary input values  */
  uint32_t i, rowCnt, flag = 0U, j, loopCnt, k,l;      /* loop counters */
  riscv_status status;                             /* status of matrix inverse */

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((pSrc->numRows != pSrc->numCols) ||
      (pDst->numRows != pDst->numCols) ||
      (pSrc->numRows != pDst->numRows)   )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */

  {

    /*--------------------------------------------------------------------------------------------------------------
     * Matrix Inverse can be solved using elementary row operations.
     *
     *  Gauss-Jordan Method:
     *
     *      1. First combine the identity matrix and the input matrix separated by a bar to form an
     *        augmented matrix as follows:
     *                      _                  _         _         _
     *                     |  a11  a12 | 1   0  |       |  X11 X12  |
     *                     |           |        |   =   |           |
     *                     |_ a21  a22 | 0   1 _|       |_ X21 X21 _|
     *
     *      2. In our implementation, pDst Matrix is used as identity matrix.
     *
     *      3. Begin with the first row. Let i = 1.
     *
     *      4. Check to see if the pivot for row i is zero.
     *         The pivot is the element of the main diagonal that is on the current row.
     *         For instance, if working with row i, then the pivot element is aii.
     *         If the pivot is zero, exchange that row with a row below it that does not
     *         contain a zero in column i. If this is not possible, then an inverse
     *         to that matrix does not exist.
     *
     *      5. Divide every element of row i by the pivot.
     *
     *      6. For every row below and  row i, replace that row with the sum of that row and
     *         a multiple of row i so that each new element in column i below row i is zero.
     *
     *      7. Move to the next row and column and repeat steps 2 through 5 until you have zeros
     *         for every element below and above the main diagonal.
     *
     *      8. Now an identical matrix is formed to the left of the bar(input matrix, pSrc).
     *         Therefore, the matrix to the right of the bar is our solution(pDst matrix, pDst).
     *----------------------------------------------------------------------------------------------------------------*/

    /* Working pointer for destination matrix */
    pOutT1 = pOut;

    /* Loop over the number of rows */
    rowCnt = numRows;

    /* Making the destination matrix as identity matrix */
    while (rowCnt > 0U)
    {
      /* Writing all zeroes in lower triangle of the destination matrix */
      j = numRows - rowCnt;
      while (j > 0U)
      {
        *pOutT1++ = 0.0f;
        j--;
      }

      /* Writing all ones in the diagonal of the destination matrix */
      *pOutT1++ = 1.0f;

      /* Writing all zeroes in upper triangle of the destination matrix */
      j = rowCnt - 1U;
      while (j > 0U)
      {
        *pOutT1++ = 0.0f;
        j--;
      }

      /* Decrement loop counter */
      rowCnt--;
    }

    /* Loop over the number of columns of the input matrix.
       All the elements in each column are processed by the row operations */
    loopCnt = numCols;

    /* Index modifier to navigate through the columns */
    l = 0U;

    while (loopCnt > 0U)
    {
      /* Check if the pivot element is zero..
       * If it is zero then interchange the row with non zero row below.
       * If there is no non zero element to replace in the rows below,
       * then the matrix is Singular. */

      /* Working pointer for the input matrix that points
       * to the pivot element of the particular row  */
      pInT1 = pIn + (l * numCols);

      /* Working pointer for the destination matrix that points
       * to the pivot element of the particular row  */
      pOutT1 = pOut + (l * numCols);

      /* Temporary variable to hold the pivot value */
      in = *pInT1;

      /* Check if the pivot element is zero */
      if (*pInT1 == 0.0f)
      {
        /* Loop over the number rows present below */

        for (i = 1U; i < numRows - l; i++)
        {
          /* Update the input and destination pointers */
          pInT2 = pInT1 + (numCols * i);
          pOutT2 = pOutT1 + (numCols * i);

          /* Check if there is a non zero pivot element to
           * replace in the rows below */
          if (*pInT2 != 0.0f)
          {
            /* Loop over number of columns
             * to the right of the pilot element */
            j = numCols - l;

            while (j > 0U)
            {
              /* Exchange the row elements of the input matrix */
              Xchg = *pInT2;
              *pInT2++ = *pInT1;
              *pInT1++ = Xchg;

              /* Decrement the loop counter */
              j--;
            }

            /* Loop over number of columns of the destination matrix */
            j = numCols;

            while (j > 0U)
            {
              /* Exchange the row elements of the destination matrix */
              Xchg = *pOutT2;
              *pOutT2++ = *pOutT1;
              *pOutT1++ = Xchg;

              /* Decrement loop counter */
              j--;
            }

            /* Flag to indicate whether exchange is done or not */
            flag = 1U;

            /* Break after exchange is done */
            break;
          }
          /* Decrement loop counter */
        }
      }

      /* Update the status if the matrix is singular */
      if ((flag != 1U) && (in == 0.0f))
      {
        return RISCV_MATH_SINGULAR;
      }

      /* Points to the pivot row of input and destination matrices */
      pPivotRowIn = pIn + (l * numCols);
      pPivotRowDst = pOut + (l * numCols);

      /* Temporary pointers to the pivot row pointers */
      pInT1 = pPivotRowIn;
      pInT2 = pPivotRowDst;

      /* Pivot element of the row */
      in = *pPivotRowIn;

      /* Loop over number of columns
       * to the right of the pilot element */
      j = (numCols - l);

      while (j > 0U)
      {
        /* Divide each element of the row of the input matrix
         * by the pivot element */
        in1 = *pInT1;
        *pInT1++ = in1 / in;

        /* Decrement the loop counter */
        j--;
      }

      /* Loop over number of columns of the destination matrix */
      j = numCols;

      while (j > 0U)
      {
        /* Divide each element of the row of the destination matrix
         * by the pivot element */
        in1 = *pInT2;
        *pInT2++ = in1 / in;

        /* Decrement the loop counter */
        j--;
      }

      /* Replace the rows with the sum of that row and a multiple of row i
       * so that each new element in column i above row i is zero.*/

      /* Temporary pointers for input and destination matrices */
      pInT1 = pIn;
      pInT2 = pOut;

      /* index used to check for pivot element */
      i = 0U;

      /* Loop over number of rows */
      /*  to be replaced by the sum of that row and a multiple of row i */
      k = numRows;

      while (k > 0U)
      {
        /* Check for the pivot element */
        if (i == l)
        {
          /* If the processing element is the pivot element,
             only the columns to the right are to be processed */
          pInT1 += numCols - l;

          pInT2 += numCols;
        }
        else
        {
          /* Element of the reference row */
          in = *pInT1;

          /* Working pointers for input and destination pivot rows */
          pPRT_in = pPivotRowIn;
          pPRT_pDst = pPivotRowDst;

          /* Loop over the number of columns to the right of the pivot element,
             to replace the elements in the input matrix */
          j = (numCols - l);

          while (j > 0U)
          {
            /* Replace the element by the sum of that row
               and a multiple of the reference row  */
            in1 = *pInT1;
            *pInT1++ = in1 - (in * *pPRT_in++);

            /* Decrement the loop counter */
            j--;
          }

          /* Loop over the number of columns to
             replace the elements in the destination matrix */
          j = numCols;

          while (j > 0U)
          {
            /* Replace the element by the sum of that row
               and a multiple of the reference row  */
            in1 = *pInT2;
            *pInT2++ = in1 - (in * *pPRT_pDst++);

            /* Decrement loop counter */
            j--;
          }

        }

        /* Increment temporary input pointer */
        pInT1 = pInT1 + l;

        /* Decrement loop counter */
        k--;

        /* Increment pivot index */
        i++;
      }

      /* Increment the input pointer */
      pIn++;

      /* Decrement the loop counter */
      loopCnt--;

      /* Increment the index modifier */
      l++;
    }


#else

  float32_t Xchg, in = 0.0f;                     /* Temporary input values  */
  uint32_t i, rowCnt, flag = 0U, j, loopCnt, l;      /* loop counters */
  riscv_status status;                             /* status of matrix inverse */

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((pSrc->numRows != pSrc->numCols) ||
      (pDst->numRows != pDst->numCols) ||
      (pSrc->numRows != pDst->numRows)   )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */

  {

    /*--------------------------------------------------------------------------------------------------------------
     * Matrix Inverse can be solved using elementary row operations.
     *
     *  Gauss-Jordan Method:
     *
     *      1. First combine the identity matrix and the input matrix separated by a bar to form an
     *        augmented matrix as follows:
     *                      _  _          _     _      _   _         _         _
     *                     |  |  a11  a12  | | | 1   0  |   |       |  X11 X12  |
     *                     |  |            | | |        |   |   =   |           |
     *                     |_ |_ a21  a22 _| | |_0   1 _|  _|       |_ X21 X21 _|
     *
     *      2. In our implementation, pDst Matrix is used as identity matrix.
     *
     *      3. Begin with the first row. Let i = 1.
     *
     *      4. Check to see if the pivot for row i is zero.
     *         The pivot is the element of the main diagonal that is on the current row.
     *         For instance, if working with row i, then the pivot element is aii.
     *         If the pivot is zero, exchange that row with a row below it that does not
     *         contain a zero in column i. If this is not possible, then an inverse
     *         to that matrix does not exist.
     *
     *      5. Divide every element of row i by the pivot.
     *
     *      6. For every row below and  row i, replace that row with the sum of that row and
     *         a multiple of row i so that each new element in column i below row i is zero.
     *
     *      7. Move to the next row and column and repeat steps 2 through 5 until you have zeros
     *         for every element below and above the main diagonal.
     *
     *      8. Now an identical matrix is formed to the left of the bar(input matrix, src).
     *         Therefore, the matrix to the right of the bar is our solution(dst matrix, dst).
     *----------------------------------------------------------------------------------------------------------------*/

    /* Working pointer for destination matrix */
    pOutT1 = pOut;

    /* Loop over the number of rows */
    rowCnt = numRows;

    /* Making the destination matrix as identity matrix */
#if defined (RISCV_VECTOR)
    while (rowCnt > 0U)
    {
      blkCnt = numRows - rowCnt;
      vl = vsetvl_e32m8(blkCnt);
      vx = vfmv_v_f_f32m8(0, vl);
      for (; (vl = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= vl) {
        vse32_v_f32m8 (pOutT1, vx, vl);
        pOutT1 += vl;
      }
      /* Writing all ones in the diagonal of the destination matrix */
        *pOutT1++ = 1.0f;
  
      blkCnt = rowCnt - 1U;
      vl = vsetvl_e32m8(blkCnt);
      vx = vfmv_v_f_f32m8(0, vl);
      for (; (vl = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= vl) {
        vse32_v_f32m8 (pOutT1, vx, vl);
        pOutT1 += vl;
      }
      rowCnt--;
    }
#else
    while (rowCnt > 0U)
    {
      /* Writing all zeroes in lower triangle of the destination matrix */
      j = numRows - rowCnt;
      while (j > 0U)
      {
        *pOutT1++ = 0.0f;
        j--;
      }

      /* Writing all ones in the diagonal of the destination matrix */
      *pOutT1++ = 1.0f;

      /* Writing all zeroes in upper triangle of the destination matrix */
      j = rowCnt - 1U;
      while (j > 0U)
      {
        *pOutT1++ = 0.0f;
        j--;
      }

      /* Decrement loop counter */
      rowCnt--;
    }
#endif

    /* Loop over the number of columns of the input matrix.
       All the elements in each column are processed by the row operations */
    loopCnt = numCols;

    /* Index modifier to navigate through the columns */
    l = 0U;

    while (loopCnt > 0U)
    {
      /* Check if the pivot element is zero..
       * If it is zero then interchange the row with non zero row below.
       * If there is no non zero element to replace in the rows below,
       * then the matrix is Singular. */

      /* Working pointer for the input matrix that points
       * to the pivot element of the particular row  */
      pInT1 = pIn + (l * numCols);

      /* Working pointer for the destination matrix that points
       * to the pivot element of the particular row  */
      pOutT1 = pOut + (l * numCols);

      /* Temporary variable to hold the pivot value */
      in = *pInT1;

      /* Check if the pivot element is zero */
      if (*pInT1 == 0.0f)
      {
        /* Loop over the number rows present below */
        for (i = 1U; i < numRows-l; i++)
        {
          /* Update the input and destination pointers */
          pInT2 = pInT1 + (numCols * i);
          pOutT2 = pOutT1 + (numCols * i);

          /* Check if there is a non zero pivot element to
           * replace in the rows below */
          if (*pInT2 != 0.0f)
          {
            /* Loop over number of columns
             * to the right of the pilot element */
            for (j = 0U; j < (numCols - l); j++)
            {
              /* Exchange the row elements of the input matrix */
              Xchg = *pInT2;
              *pInT2++ = *pInT1;
              *pInT1++ = Xchg;
            }

            for (j = 0U; j < numCols; j++)
            {
              Xchg = *pOutT2;
              *pOutT2++ = *pOutT1;
              *pOutT1++ = Xchg;
            }

            /* Flag to indicate whether exchange is done or not */
            flag = 1U;

            /* Break after exchange is done */
            break;
          }
        }
      }


      /* Update the status if the matrix is singular */
      if ((flag != 1U) && (in == 0.0f))
      {
        return RISCV_MATH_SINGULAR;
      }

      /* Points to the pivot row of input and destination matrices */
      pPivotRowIn = pIn + (l * numCols);
      pPivotRowDst = pOut + (l * numCols);

      /* Temporary pointers to the pivot row pointers */
      pInT1 = pPivotRowIn;
      pOutT1 = pPivotRowDst;

      /* Pivot element of the row */
      in = *(pIn + (l * numCols));

      /* Loop over number of columns
       * to the right of the pilot element */
      for (j = 0U; j < (numCols - l); j++)
      {
        /* Divide each element of the row of the input matrix
         * by the pivot element */
        *pInT1 = *pInT1 / in;
        pInT1++;
      }
      for (j = 0U; j < numCols; j++)
      {
        /* Divide each element of the row of the destination matrix
         * by the pivot element */
        *pOutT1 = *pOutT1 / in;
        pOutT1++;
      }

      /* Replace the rows with the sum of that row and a multiple of row i
       * so that each new element in column i above row i is zero.*/

      /* Temporary pointers for input and destination matrices */
      pInT1 = pIn;
      pOutT1 = pOut;

      for (i = 0U; i < numRows; i++)
      {
        /* Check for the pivot element */
        if (i == l)
        {
          /* If the processing element is the pivot element,
             only the columns to the right are to be processed */
          pInT1 += numCols - l;
          pOutT1 += numCols;
        }
        else
        {
          /* Element of the reference row */
          in = *pInT1;

          /* Working pointers for input and destination pivot rows */
          pPRT_in = pPivotRowIn;
          pPRT_pDst = pPivotRowDst;

          /* Loop over the number of columns to the right of the pivot element,
             to replace the elements in the input matrix */
#if defined (RISCV_VECTOR)
          /* Replace the element by the sum of that row
               and a multiple of the reference row  */
          blkCnt = numCols - l;
          for (; (vl = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= vl)
          {
            vx = vle32_v_f32m8(pInT1, vl);
            vy = vle32_v_f32m8(pPRT_in, vl);
            vse32_v_f32m8 (pInT1, vfsub_vv_f32m8(vx, vfmul_vf_f32m8(vy,in, vl), vl), vl);
            pInT1 += vl;
            pPRT_in += vl;
          }
          /* Loop over the number of columns to
             replace the elements in the destination matrix */
          blkCnt = numCols;
          for (; (vl = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= vl)
          {
            vx = vle32_v_f32m8(pOutT1, vl);
            vy = vle32_v_f32m8(pPRT_pDst, vl);
            vse32_v_f32m8 (pOutT1, vfsub_vv_f32m8(vx, vfmul_vf_f32m8(vy,in, vl), vl), vl);
            pOutT1 += vl;
            pPRT_pDst += vl;
          }
#else
          for (j = 0U; j < (numCols - l); j++)
          {
            /* Replace the element by the sum of that row
               and a multiple of the reference row  */
            *pInT1 = *pInT1 - (in * *pPRT_in++);
            pInT1++;
          }

          /* Loop over the number of columns to
             replace the elements in the destination matrix */
          for (j = 0U; j < numCols; j++)
          {
            /* Replace the element by the sum of that row
               and a multiple of the reference row  */
            *pOutT1 = *pOutT1 - (in * *pPRT_pDst++);
            pOutT1++;
          }
#endif

        }

        /* Increment temporary input pointer */
        pInT1 = pInT1 + l;
      }

      /* Increment the input pointer */
      pIn++;

      /* Decrement the loop counter */
      loopCnt--;

      /* Increment the index modifier */
      l++;
    }

#endif /* #if defined (RISCV_MATH_DSP) */

    /* Set status as RISCV_MATH_SUCCESS */
    status = RISCV_MATH_SUCCESS;

    if ((flag != 1U) && (in == 0.0f))
    {
      pIn = pSrc->pData;
      for (i = 0; i < numRows * numCols; i++)
      {
        if (pIn[i] != 0.0f)
            break;
      }

      if (i == numRows * numCols)
        status = RISCV_MATH_SINGULAR;
    }
  }

  /* Return to application */
  return (status);
}

/**
  @} end of MatrixInv group
 */
