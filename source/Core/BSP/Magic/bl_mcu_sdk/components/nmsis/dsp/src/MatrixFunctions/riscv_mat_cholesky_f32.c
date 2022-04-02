/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_cholesky_f32.c
 * Description:  Floating-point Cholesky decomposition
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
  @defgroup MatrixChol Cholesky and LDLT decompositions

  Computes the Cholesky or LDL^t decomposition of a matrix.


  If the input matrix does not have a decomposition, then the 
  algorithm terminates and returns error status RISCV_MATH_DECOMPOSITION_FAILURE.
 */

/**
  @addtogroup MatrixChol
  @{
 */

/**
   * @brief Floating-point Cholesky decomposition of positive-definite matrix.
   * @param[in]  pSrc   points to the instance of the input floating-point matrix structure.
   * @param[out] pDst   points to the instance of the output floating-point matrix structure.
   * @return The function returns RISCV_MATH_SIZE_MISMATCH, if the dimensions do not match.
   * @return        execution status
                   - \ref RISCV_MATH_SUCCESS       : Operation successful
                   - \ref RISCV_MATH_SIZE_MISMATCH : Matrix size check failed
                   - \ref RISCV_MATH_DECOMPOSITION_FAILURE      : Input matrix cannot be decomposed
   * @par
   * If the matrix is ill conditioned or only semi-definite, then it is better using the LDL^t decomposition.
   * The decomposition of A is returning a lower triangular matrix U such that A = U U^t
   */

riscv_status riscv_mat_cholesky_f32(
  const riscv_matrix_instance_f32 * pSrc,
        riscv_matrix_instance_f32 * pDst)
{

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
    int i,j,k;
    int n = pSrc->numRows;
    float32_t invSqrtVj;
    float32_t *pA,*pG;

#if defined(RISCV_VECTOR)
    uint32_t blkCnt;                               /* Loop counter */
    size_t l;
    vfloat32m8_t v_x, v_y;
    vfloat32m8_t v_a;
    vfloat32m1_t v_temp;
    float32_t *pGX;
    float32_t *pGY;
    ptrdiff_t bstride = 8;
    l = vsetvl_e32m1(1);
    v_temp = vfsub_vv_f32m1(v_temp, v_temp, l);
#endif

    pA = pSrc->pData;
    pG = pDst->pData;
    

    for(i=0 ; i < n ; i++)
    {
       for(j=i ; j < n ; j++)
       {
#if defined(RISCV_VECTOR)
            if(i==0){
                pG[j * n + i] = pA[j * n + i];
            }
            else{
                blkCnt = i;
                pGX = pG + i * n;
                pGY = pG + j * n;
                l = vsetvl_e32m8(blkCnt);
                v_a = vfsub_vv_f32m8(v_a,v_a, l);
                for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
                    v_x = vle32_v_f32m8(pGX, l);
                    v_y = vle32_v_f32m8(pGY, l);
                    v_a = vfmacc_vv_f32m8(v_a,v_x,v_y, l);
                    pGX += l;
                    pGY += l;
                }
                l = vsetvl_e32m8(i);
                pG[j * n + i] = pA[j * n + i] - vfmv_f_s_f32m1_f32(vfredsum_vs_f32m8_f32m1(v_temp,v_a,v_temp, l));
            }
#else
            pG[j * n + i] = pA[j * n + i];

            for(k=0; k < i ; k++)
            {
                pG[j * n + i] = pG[j * n + i] - pG[i * n + k] * pG[j * n + k];
            }
#endif
       }

       if (pG[i * n + i] <= 0.0f)
       {
         return(RISCV_MATH_DECOMPOSITION_FAILURE);
       }

       invSqrtVj = 1.0f/sqrtf(pG[i * n + i]);
       for(j=i ; j < n ; j++)
       {
         pG[j * n + i] = pG[j * n + i] * invSqrtVj ;
       }
    }

    status = RISCV_MATH_SUCCESS;

  }

  
  /* Return to application */
  return (status);
}

/**
  @} end of MatrixChol group
 */
