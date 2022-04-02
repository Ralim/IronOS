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
 * Title:        riscv_convolve_1x1_s8_fast.c
 * Description:  Fast q7 version of 1x1 convolution (non-square shape)
 *
 * $Date:        09. October 2020
 * $Revision:    V.2.0.3
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnfunctions.h"
#include "riscv_nnsupportfunctions.h"

#define DIM_KER_X (1U)
#define DIM_KER_Y (1U)

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup NNConv
 * @{
 */

/*
 * Fast s8 version for 1x1 convolution (non-square shape)
 *
 * Refer header file for details.
 *
 */

riscv_status riscv_convolve_1x1_s8_fast(const nmsis_nn_context *ctx,
                                    const nmsis_nn_conv_params *conv_params,
                                    const nmsis_nn_per_channel_quant_params *quant_params,
                                    const nmsis_nn_dims *input_dims,
                                    const q7_t *input_data,
                                    const nmsis_nn_dims *filter_dims,
                                    const q7_t *filter_data,
                                    const nmsis_nn_dims *bias_dims,
                                    const int32_t *bias_data,
                                    const nmsis_nn_dims *output_dims,
                                    q7_t *output_data)
{
    if (input_dims->c % 4 != 0 || conv_params->padding.w != 0 || conv_params->padding.h != 0 ||
        conv_params->stride.w != 1 || conv_params->stride.h != 1)
    {
        return RISCV_MATH_SIZE_MISMATCH;
    }

    (void)ctx;
    (void)filter_dims;
    (void)bias_dims;

    /* Run the following code as reference implementation for RISC-V processors with or without DSP extension */

    const int32_t lhs_rows = input_dims->w * input_dims->h * input_dims->n;
    const int32_t rhs_rows = output_dims->c;
    const int32_t rhs_cols = input_dims->c;

    riscv_nn_mat_mult_nt_t_s8(input_data,
                            filter_data,
                            bias_data,
                            output_data,
                            quant_params->multiplier,
                            quant_params->shift,
                            lhs_rows,
                            rhs_rows,
                            rhs_cols,
                            conv_params->input_offset,
                            conv_params->output_offset,
                            conv_params->activation.min,
                            conv_params->activation.max);


    /* Return to application */
    return RISCV_MATH_SUCCESS;
}

int32_t riscv_convolve_1x1_s8_fast_get_buffer_size(const nmsis_nn_dims *input_dims)
{
    (void)input_dims;
    return 0;
}

/**
 * @} end of NNConv group
 */
