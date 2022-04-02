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
 * Title:        riscv_nn_mat_mul_core_4x_s8.c
 * Description:  General matrix multiplication function for MVE extension
 *
 * $Date:        09. October 2020
 * $Revision:    V.2.0.1
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
 * s8 matrix multiplication to process 4 rows and one column
 *
 * Refer header file for details.
 *
 */
riscv_status riscv_nn_mat_mul_core_4x_s8(const int32_t row_elements,
                                     const int32_t offset,
                                     const int8_t *row_base,
                                     const int8_t *col_base,
                                     int32_t *const sum_col,
                                     int32_t *const output)
{
    int32_t acc_n0 = 0;
    int32_t acc_n1 = 0;
    int32_t acc_n2 = 0;
    int32_t acc_n3 = 0;

    const int8_t *ip_row_0 = row_base;
    const int8_t *ip_row_1 = row_base + offset;
    const int8_t *ip_row_2 = row_base + (2 * offset);
    const int8_t *ip_row_3 = row_base + (3 * offset);
    int32_t sum_tmp = 0;

    for (int i = 0; i < row_elements; i++)
    {
        int32_t col = col_base[i];
        sum_tmp += col;
        acc_n0 += ip_row_0[i] * col;
        acc_n1 += ip_row_1[i] * col;
        acc_n2 += ip_row_2[i] * col;
        acc_n3 += ip_row_3[i] * col;
    }
    output[0] = acc_n0;
    output[1] = acc_n1;
    output[2] = acc_n2;
    output[3] = acc_n3;

    *sum_col = sum_tmp;

    return RISCV_MATH_SUCCESS;
}

/**
 * @} end of NNBasicMath group
 */
