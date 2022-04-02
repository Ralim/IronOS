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
 * $Date:        January 20, 2020
 * $Revision:    V.1.0.1
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"
#include "ref_functions.h"

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

riscv_status riscv_nn_mat_mul_core_1x_s8_ref(int32_t row_elements,
                                         const int8_t *row_base,
                                         const int8_t *col_base,
                                         int32_t *const sum_col,
                                         int32_t *const output)
{
    int32_t acc_n0 = 0;
    int32_t sum_tmp = 0;

    for (int i = 0; i < row_elements; i++)
    {
        sum_tmp += col_base[i];
        acc_n0 += row_base[i] * col_base[i];
    }

    *sum_col = sum_tmp;
    *output = acc_n0;
    return RISCV_MATH_SUCCESS;
}

/**
 * @} end of NNBasicMath group
 */
