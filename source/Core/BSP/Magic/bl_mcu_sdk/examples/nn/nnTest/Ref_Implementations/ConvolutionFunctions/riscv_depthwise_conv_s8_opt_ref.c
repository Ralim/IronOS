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
 * Title:        riscv_depthwise_conv_s8_opt.c
 * Description:  Optimized s8 depthwise separable convolution function for
 *               channel multiplier of 1.
 *
 * $Date:        May 29, 2020
 * $Revision:    V.2.0.1
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "ref_functions.h"
#include "riscv_nnsupportfunctions.h"

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup NNConv
 * @{
 */

/*
   * Optimized s8 depthwise convolution function with constraint that in_channel equals out_channel
   *
   *  Refer prototype header file for details.
   *
   */

riscv_status riscv_depthwise_conv_s8_opt_ref(const nmsis_nn_context *ctx,
                                         const nmsis_nn_dw_conv_params *dw_conv_params,
                                         const nmsis_nn_per_channel_quant_params *quant_params,
                                         const nmsis_nn_dims *input_dims,
                                         const q7_t *input,
                                         const nmsis_nn_dims *filter_dims,
                                         const q7_t *kernel,
                                         const nmsis_nn_dims *bias_dims,
                                         const int32_t *bias,
                                         const nmsis_nn_dims *output_dims,
                                         q7_t *output)
{
    const int32_t input_x = input_dims->w;
    const int32_t input_y = input_dims->h;
    const int32_t input_ch = input_dims->c;
    const int32_t output_ch = output_dims->c;
    const int32_t kernel_x = filter_dims->w;
    const int32_t kernel_y = filter_dims->h;
    const int32_t pad_x = dw_conv_params->padding.w;
    const int32_t pad_y = dw_conv_params->padding.h;
    const int32_t stride_x = dw_conv_params->stride.w;
    const int32_t stride_y = dw_conv_params->stride.h;
    const int32_t *output_shift = quant_params->shift;
    const int32_t *output_mult = quant_params->multiplier;
    const int32_t output_x = output_dims->w;
    const int32_t output_y = output_dims->h;
    const int32_t output_offset = dw_conv_params->output_offset;
    const int32_t input_offset = dw_conv_params->input_offset;
    const int32_t output_activation_min = dw_conv_params->activation.min;
    const int32_t output_activation_max = dw_conv_params->activation.max;
    q15_t *buffer_a = (q15_t *)ctx->buf;

    /* Check input constraints input_ch == output_ch */
    if (input_ch != output_ch)
    {
        return RISCV_MATH_SIZE_MISMATCH;
    }
    /* Run the following code as reference implementation for RISC-V Core without DSP */
    return riscv_depthwise_conv_s8_ref(ctx,
                                     dw_conv_params,
                                     quant_params,
                                     input_dims,
                                     input,
                                     filter_dims,
                                     kernel,
                                     bias_dims,
                                     bias,
                                     output_dims,
                                     output);

    /* Return to application */
    return RISCV_MATH_SUCCESS;
}

int32_t riscv_depthwise_conv_s8_opt_get_buffer_size_ref(const nmsis_nn_dims *input_dims,
                                                      const nmsis_nn_dims *filter_dims)
{
    (void)input_dims;
    (void)filter_dims;
    return 0;
}

/**
 * @} end of NNConv group
 */
