/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_solve_lower_triangular_f16.c
 * Description:  Solve linear system LT X = A with LT lower triangular matrix
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

#include "dsp/matrix_functions_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)
/**
  @ingroup groupMatrix
 */


/**
  @addtogroup MatrixInv
  @{
 */


   /**
   * @brief Solve LT . X = A where LT is a lower triangular matrix
   * @param[in]  lt  The lower triangular matrix
   * @param[in]  a  The matrix a
   * @param[out] dst The solution X of LT . X = A
   * @return The function returns RISCV_MATH_SINGULAR, if the system can't be solved.
   */

  riscv_status riscv_mat_solve_lower_triangular_f16(
  const riscv_matrix_instance_f16 * lt,
  const riscv_matrix_instance_f16 * a,
  riscv_matrix_instance_f16 * dst)
  {
  riscv_status status;                             /* status of matrix inverse */


#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((lt->numRows != lt->numCols) ||
      (a->numRows != a->numCols) ||
      (lt->numRows != a->numRows)   )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */

  {
    /* a1 b1 c1   x1 = a1
          b2 c2   x2   a2
             c3   x3   a3

    x3 = a3 / c3 
    x2 = (a2 - c2 x3) / b2

    */
    int i,j,k,n;

    n = dst->numRows;

    float16_t *pX = dst->pData;
    float16_t *pLT = lt->pData;
    float16_t *pA = a->pData;

    float16_t *lt_row;
    float16_t *a_col;

    for(j=0; j < n; j ++)
    {
       a_col = &pA[j];

       for(i=0; i < n ; i++)
       {
            lt_row = &pLT[n*i];

            float16_t tmp=a_col[i * n];
            
            for(k=0; k < i; k++)
            {
                tmp -= lt_row[k] * pX[n*k+j];
            }

            if (lt_row[i]==0.0f)
            {
              return(RISCV_MATH_SINGULAR);
            }
            tmp = tmp / lt_row[i];
            pX[i*n+j] = tmp;
       }

    }
    status = RISCV_MATH_SUCCESS;

  }

  /* Return to application */
  return (status);
}

/**
  @} end of MatrixInv group
 */
#endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */ 
