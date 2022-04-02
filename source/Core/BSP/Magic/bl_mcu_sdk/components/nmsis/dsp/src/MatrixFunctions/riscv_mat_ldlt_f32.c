/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_ldl_f32.c
 * Description:  Floating-point LDL decomposition
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






/// @private
#define SWAP_ROWS_F32(A,i,j)     \
  for(int w=0;w < n; w++)    \
  {                          \
     float32_t tmp;          \
     tmp = A[i*n + w];       \
     A[i*n + w] = A[j*n + w];\
     A[j*n + w] = tmp;       \
  }

/// @private
#define SWAP_COLS_F32(A,i,j)     \
  for(int w=0;w < n; w++)    \
  {                          \
     float32_t tmp;          \
     tmp = A[w*n + i];       \
     A[w*n + i] = A[w*n + j];\
     A[w*n + j] = tmp;       \
  }

/**
  @ingroup groupMatrix
 */

/**
  @addtogroup MatrixChol
  @{
 */
  
/**
   * @brief Floating-point LDL^t decomposition of positive semi-definite matrix.
   * @param[in]  pSrc   points to the instance of the input floating-point matrix structure.
   * @param[out] pl   points to the instance of the output floating-point triangular matrix structure.
   * @param[out] pd   points to the instance of the output floating-point diagonal matrix structure.
   * @param[out] pp   points to the instance of the output floating-point permutation vector.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed
                   - \ref RISCV_MATH_DECOMPOSITION_FAILURE      : Input matrix cannot be decomposed
   * @par
   *  Computes the LDL^t decomposition of a matrix A such that P A P^t = L D L^t.
   */
riscv_status riscv_mat_ldlt_f32(
  const riscv_matrix_instance_f32 * pSrc,
  riscv_matrix_instance_f32 * pl,
  riscv_matrix_instance_f32 * pd,
  uint16_t * pp)
{

  riscv_status status;                             /* status of matrix inverse */
 

#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((pSrc->numRows != pSrc->numCols) ||
      (pl->numRows != pl->numCols) ||
      (pd->numRows != pd->numCols) ||
      (pl->numRows != pd->numRows)   )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */

  {

    const int n=pSrc->numRows;
    int fullRank = 1, diag,k;
    float32_t *pA;

    memcpy(pl->pData,pSrc->pData,n*n*sizeof(float32_t));
    pA = pl->pData;

    for(int k=0;k < n; k++)
    {
      pp[k] = k;
    }


    for(k=0;k < n; k++)
    {
        /* Find pivot */
        float32_t m=F32_MIN,a;
        int j=k; 


        for(int r=k;r<n;r++)
        {
           if (pA[r*n+r] > m)
           {
             m = pA[r*n+r];
             j = r;
           }
        }

        if(j != k)
        {
          SWAP_ROWS_F32(pA,k,j);
          SWAP_COLS_F32(pA,k,j);
        }


        pp[k] = j;

        a = pA[k*n+k];

        if (fabs(a) < 1.0e-8)
        {

            fullRank = 0;
            break;
        }

        for(int w=k+1;w<n;w++)
        {
          for(int x=k+1;x<n;x++)
          {
             pA[w*n+x] = pA[w*n+x] - pA[w*n+k] * pA[x*n+k] / a;
          }
        }

        for(int w=k+1;w<n;w++)
        {
               pA[w*n+k] = pA[w*n+k] / a;
        }

        

    }



    diag=k;
    if (!fullRank)
    {
      diag--;
      for(int row=0; row < n;row++)
      {
        for(int col=k; col < n;col++)
        {
           pl->pData[row*n+col]=0.0;
        }
      }
    }

    for(int row=0; row < n;row++)
    {
       for(int col=row+1; col < n;col++)
       {
         pl->pData[row*n+col] = 0.0;
       }
    }

    for(int d=0; d < diag;d++)
    {
      pd->pData[d*n+d] = pl->pData[d*n+d];
      pl->pData[d*n+d] = 1.0;
    }
  
    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}

/**
  @} end of MatrixChol group
 */
