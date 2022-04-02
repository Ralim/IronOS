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
 * Title:        riscv_nn_depthwise_conv_s8_core.c
 * Description:  Depthwise convolution on im2col buffers.
 *
 * $Date:        09. October 2020
 * $Revision:    V.1.0.4
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"

/*
 * Depthwise conv on an im2col buffer where the input channel equals
 * output channel.
 *
 * Refer header file for details.
 *
 */

q7_t *riscv_nn_depthwise_conv_s8_core(const q7_t *row,
                                    const q15_t *col,
                                    const uint16_t num_ch,
                                    const int32_t *out_shift,
                                    const int32_t *out_mult,
                                    const int32_t out_offset,
                                    const int32_t activation_min,
                                    const int32_t activation_max,
                                    const uint16_t kernel_size,
                                    const int32_t *const output_bias,
                                    q7_t *out)
{
    (void)row;
    (void)col;
    (void)num_ch;
    (void)out_shift;
    (void)out_mult;
    (void)out_offset;
    (void)activation_min;
    (void)activation_max;
    (void)kernel_size;
    (void)output_bias;
    (void)out;
    return NULL;
}
