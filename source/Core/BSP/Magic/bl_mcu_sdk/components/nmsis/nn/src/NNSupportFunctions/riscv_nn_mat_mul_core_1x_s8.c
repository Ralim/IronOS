/*
 * Copyright (C) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_nn_mat_mul_core_1x_s8.c
 * Description:  General Matrix-multiplication function
 *
 * $Date:        09. October 2020
 * $Revision:    V.1.0.2
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup NNBasicMath
 * @{
 */

/*
 * s8 matrix multiplication to process 1 row
 *
 * Refer header file for details.
 *
 */

riscv_status riscv_nn_mat_mul_core_1x_s8(int32_t row_elements,
                                     const int8_t *row_base,
                                     const int8_t *col_base,
                                     int32_t *const sum_col,
                                     int32_t *const output)
{
    int32_t acc_n0 = 0;
    int32_t sum_tmp = 0;

#if defined (RISCV_VECTOR)
    uint32_t blkCnt = row_elements;                              /* Loop counter */
    size_t l;
    const q7_t *pCol = col_base;
    const q7_t *pRow = row_base;
    vint8m2_t v_col,v_row;
    vint32m1_t temp;
    l = vsetvl_e8m2(1);
    temp = vsub_vv_i32m1(temp,temp, l);
    for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
      v_col = vle8_v_i8m2(pCol, l);
      v_row = vle8_v_i8m2(pRow, l);
      pCol += l;
      pRow += l;
      sum_tmp += vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(temp,vwadd_vx_i16m4(v_col,0, l),temp, l));
      l = vsetvl_e8m2(blkCnt);
      acc_n0 += vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(temp,vwadd_vv_i16m4(v_col,v_row, l),temp, l));
    }
#else
    for (int i = 0; i < row_elements; i++)
    {
        sum_tmp += col_base[i];
        acc_n0 += row_base[i] * col_base[i];
    }
#endif

    *sum_col = sum_tmp;
    *output = acc_n0;
    return RISCV_MATH_SUCCESS;
}

/**
 * @} end of NNBasicMath group
 */
