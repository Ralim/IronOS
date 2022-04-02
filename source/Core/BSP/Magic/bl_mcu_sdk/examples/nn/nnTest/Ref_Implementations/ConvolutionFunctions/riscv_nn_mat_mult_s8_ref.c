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
 * Title:        riscv_nn_mat_mult_s8.c
 * Description:  General Matrix-multiplication function
 *
 * $Date:        July 27, 2020
 * $Revision:    V.2.0.4
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"
#include "ref_functions.h"

/*
   * s8 General matrix multiplication function with per-channel requantization for upto 4 column batches.
   *
   * Refer header file for details.
   *
   */

q7_t *riscv_nn_mat_mult_s8_ref(const q7_t *input_row,
                             const q7_t *input_col,
                             const uint16_t output_ch,
                             const uint16_t col_batches,
                             const int32_t *output_shift,
                             const int32_t *output_mult,
                             const int32_t out_offset,
                             const int32_t col_offset,
                             const int32_t row_offset,
                             const int16_t activation_min,
                             const int16_t activation_max,
                             const uint16_t row_len,
                             const int32_t *const bias,
                             q7_t *out)
{
    (void)input_row;
    (void)input_col;
    (void)output_ch;
    (void)col_batches;
    (void)output_shift;
    (void)output_mult;
    (void)out_offset;
    (void)col_offset;
    (void)row_offset;
    (void)activation_min;
    (void)activation_max;
    (void)row_len;
    (void)bias;
    (void)out;
    return NULL;
}
