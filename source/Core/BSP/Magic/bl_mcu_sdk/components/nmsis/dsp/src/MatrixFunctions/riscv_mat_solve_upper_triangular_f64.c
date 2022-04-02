/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mat_solve_upper_triangular_f64.c
 * Description:  Solve linear system UT X = A with UT upper triangular matrix
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
  @addtogroup MatrixInv
  @{
 */

/**
   * @brief Solve UT . X = A where UT is an upper triangular matrix
   * @param[in]  ut  The upper triangular matrix
   * @param[in]  a  The matrix a
   * @param[out] dst The solution X of UT . X = A
   * @return The function returns RISCV_MATH_SINGULAR, if the system can't be solved.
  */
  riscv_status riscv_mat_solve_upper_triangular_f64(
  const riscv_matrix_instance_f64 * ut,
  const riscv_matrix_instance_f64 * a,
  riscv_matrix_instance_f64 * dst)
  {
riscv_status status;                             /* status of matrix inverse */
#if defined(RISCV_VECTOR) && (defined(__riscv_flen) && (__riscv_flen == 64))
    uint32_t blkCnt;                               /* Loop counter */
    size_t l;
    vfloat64m8_t v_x, v_y;
    vfloat64m8_t v_a;
    vfloat64m1_t v_temp;
    float64_t *pVut_row;
    float64_t *pX_row;
    ptrdiff_t bstride;
    l = vsetvl_e64m1(1);
    v_temp = vfsub_vv_f64m1(v_temp, v_temp, l);
#endif
#ifdef RISCV_MATH_MATRIX_CHECK

  /* Check for matrix mismatch condition */
  if ((ut->numRows != ut->numCols) ||
      (a->numRows != a->numCols) ||
      (ut->numRows != a->numRows)   )
  {
    /* Set status as RISCV_MATH_SIZE_MISMATCH */
    status = RISCV_MATH_SIZE_MISMATCH;
  }
  else

#endif /* #ifdef RISCV_MATH_MATRIX_CHECK */

  {

    int i,j,k,n;

    n = dst->numRows;

    float64_t *pX = dst->pData;
    float64_t *pUT = ut->pData;
    float64_t *pA = a->pData;

    float64_t *ut_row;
    float64_t *a_col;

    for(j=0; j < n; j ++)
    {
       a_col = &pA[j];

       for(i=n-1; i >= 0 ; i--)
       {
            ut_row = &pUT[n*i];

            float64_t tmp=a_col[i * n];
#if defined(RISCV_VECTOR) && (defined(__riscv_flen) && (__riscv_flen == 64))
            blkCnt = n-i-1;
            pVut_row = ut_row + i+1;
            pX_row = pX + n*(i+1)+j;
            l = vsetvl_e64m8(blkCnt);
            v_a = vfsub_vv_f64m8(v_a,v_a, l);
            bstride = 8*n;
            for (; (l = vsetvl_e64m8(blkCnt)) > 0; blkCnt -= l) {
                v_x = vle64_v_f64m8(pVut_row, l);
                v_y = vlse64_v_f64m8(pX_row,bstride, l);
                v_a = vfmacc_vv_f64m8(v_a,v_x,v_y, l);
                pVut_row += l;
                pX_row += l*n;
            }
            l = vsetvl_e64m8(n-i-1);
            tmp -= vfmv_f_s_f64m1_f64(vfredsum_vs_f64m8_f64m1(v_temp,v_a,v_temp, l));
#else
            for(k=n-1; k > i; k--)
            {
                tmp -= ut_row[k] * pX[n*k+j];
            }
#endif

            if (ut_row[i]==0.0f)
            {
              return(RISCV_MATH_SINGULAR);
            }
            tmp = tmp / ut_row[i];
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
