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
 * Title:        riscv_convolve_s8.c
 * Description:  s8 version of convolution using symmetric quantization.
 *
 * $Date:        July 27, 2020
 * $Revision:    V.2.0.2
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
   * Basic s8 convolution function.
   *
   * Refer header file for details. Optimal use case for the DSP implementation is when input and output channels
   * are multiples of 4 or atleast greater than 4.
   *
   */

riscv_status riscv_convolve_s8_ref(const nmsis_nn_context* ctx,
                               const nmsis_nn_conv_params* conv_params,
                               const nmsis_nn_per_channel_quant_params* quant_params,
                               const nmsis_nn_dims* input_dims,
                               const q7_t *input_data,
                               const nmsis_nn_dims* filter_dims,
                               const q7_t *filter_data,
                               const nmsis_nn_dims* bias_dims,
                               const int32_t *bias_data,
                               const nmsis_nn_dims* output_dims,
                               q7_t *output_data)
{
    q15_t *buffer_a = (q15_t *)ctx->buf;

    const uint16_t input_batches = input_dims->n;
    const uint16_t input_x       = input_dims->w;
    const uint16_t input_y       = input_dims->h;
    const uint16_t input_ch      = input_dims->c;
    const uint16_t kernel_x      = filter_dims->w;
    const uint16_t kernel_y      = filter_dims->h;
    const uint16_t output_x      = output_dims->w;
    const uint16_t output_y      = output_dims->h;
    const uint16_t output_ch     = output_dims->c;

    const uint16_t pad_x         = conv_params->padding.w;
    const uint16_t pad_y         = conv_params->padding.h;
    const uint16_t stride_x      = conv_params->stride.w;
    const uint16_t stride_y      = conv_params->stride.h;

    const int32_t input_offset       = conv_params->input_offset;
    const int32_t out_offset         = conv_params->output_offset;
    const int32_t out_activation_min = conv_params->activation.min;
    const int32_t out_activation_max = conv_params->activation.max;
    int32_t *output_mult             = quant_params->multiplier;
    int32_t *output_shift            = quant_params->shift;

    int i_batch;
    for (i_batch = 0; i_batch < input_batches; i_batch++)
    {
        /* Run the following code as reference implementation for RISC-V Core without DSP */
        (void)buffer_a;
        int32_t i_out_ch, i_out_y, i_out_x, i_input_ch, i_ker_y, i_ker_x;
        int32_t conv_out;

        for (i_out_ch = 0; i_out_ch < output_ch; i_out_ch++)
        {
            for (i_out_y = 0; i_out_y < output_y; i_out_y++)
            {
                for (i_out_x = 0; i_out_x < output_x; i_out_x++)
                {
                    conv_out = 0;

                    const int32_t base_idx_y = stride_y * i_out_y - pad_y;
                    const int32_t base_idx_x = stride_x * i_out_x - pad_x;

                    const int32_t ker_y_start = MAX(0, -base_idx_y);
                    const int32_t ker_x_start = MAX(0, -base_idx_x);

                    const int32_t ker_y_end = MIN(kernel_y, input_y - base_idx_y);
                    const int32_t ker_x_end = MIN(kernel_x, input_x - base_idx_x);

                    for (i_ker_y = ker_y_start; i_ker_y < ker_y_end; i_ker_y++)
                    {
                        for (i_ker_x = ker_x_start; i_ker_x < ker_x_end; i_ker_x++)
                        {
                            const int32_t in_row = base_idx_y + i_ker_y;
                            const int32_t in_col = base_idx_x + i_ker_x;
                            for (i_input_ch = 0; i_input_ch < input_ch; i_input_ch++)
                            {
                                conv_out +=
                                    (input_data[(in_row * input_x + in_col) * input_ch + i_input_ch] + input_offset) *
                                    filter_data[i_out_ch * input_ch * kernel_y * kernel_x +
                                           (i_ker_y * kernel_x + i_ker_x) * input_ch + i_input_ch];
                            }
                        }
                    }
                    if (bias_data)
                    {
                        conv_out += bias_data[i_out_ch];
                    }
                    conv_out = riscv_nn_requantize(conv_out, output_mult[i_out_ch], output_shift[i_out_ch]);
                    conv_out += out_offset;
                    conv_out = MAX(conv_out, out_activation_min);
                    conv_out = MIN(conv_out, out_activation_max);
                    output_data[i_out_ch + (i_out_y * output_x + i_out_x) * output_ch] = (int8_t)conv_out;
                }
            }
        }
        /* Advance to the next batch */
        input_data += (input_x * input_y * input_ch);
        output_data += (output_x * output_y * output_ch);
    }

    /* Return to application */
    return RISCV_MATH_SUCCESS;
}

int32_t riscv_convolve_s8_get_buffer_size_ref(const nmsis_nn_dims* input_dims,
                                        const nmsis_nn_dims* filter_dims)
{
    (void)input_dims;
    (void)filter_dims;
    return 0;
}

/**
 * @} end of NNConv group
 */
