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
 * Title:        riscv_nn_mat_mult_kernel_s8_s16_reordered.c
 * Description:  Matrix-multiplication function for convolution with reordered columns
 *
 * $Date:        February 27, 2020
 * $Revision:    V.1.0.2
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#include "ref_functions.h"
#include "riscv_nnsupportfunctions.h"

/*
 * Matrix-multiplication with re-ordered input and bias inputs for convolution with per-channel
 *        requantization. The re-ordering is a consequence of sign extension is done by the SXTB16 command.
 *
 * Refer header file for details. This function differs from riscv_nn_mat_mult_kernel_s8_s16(), in that it uses
 *        read_and_pad_reordered() instead of riscv_nn_mat_mult_kernel_s8_s16(). Investigating the cycles impact and
 *        unifying these two functions is a potential future improvement.
 *
 */

q7_t *riscv_nn_mat_mult_kernel_s8_s16_reordered_ref(const q7_t *input_a,
                                                  const q15_t *input_b,
                                                  const uint16_t output_ch,
                                                  const int32_t *out_shift,
                                                  const int32_t *out_mult,
                                                  const int32_t out_offset,
                                                  const int16_t activation_min,
                                                  const int16_t activation_max,
                                                  const uint16_t num_col_a,
                                                  const int32_t *const output_bias,
                                                  q7_t *out_0)
{
    (void)input_a;
    (void)input_b;
    (void)output_ch;
    (void)out_shift;
    (void)out_mult;
    (void)out_offset;
    (void)activation_min;
    (void)activation_max;
    (void)num_col_a;
    (void)output_bias;
    (void)out_0;
    /* To be completed */
    return NULL;
}
