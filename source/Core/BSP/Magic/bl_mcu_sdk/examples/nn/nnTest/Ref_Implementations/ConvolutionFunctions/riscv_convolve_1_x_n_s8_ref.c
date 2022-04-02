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
 * Title:        riscv_convolve_1_x_n_s8.c
 * Description:  s8 version of 1xN convolution using symmetric quantization.
 *
 * $Date:        July 27, 2020
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
   * 1xN s8 convolution function.
   *
   * Refer header file for details.
   *
   */

riscv_status riscv_convolve_1_x_n_s8_ref(const nmsis_nn_context* ctx,
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
    (void)bias_dims;
    riscv_status status = RISCV_MATH_SUCCESS;
    if (output_dims->w % 4 != 0)
    {
        status = RISCV_MATH_SIZE_MISMATCH;
        goto out;
    }

    status = riscv_convolve_s8_ref(ctx,
                                 conv_params,
                                 quant_params,
                                 input_dims,
                                 input_data,
                                 filter_dims,
                                 filter_data,
                                 bias_dims,
                                 bias_data,
                                 output_dims,
                                 output_data);

out:
    /* Return to application */
    return status;
}

int32_t riscv_convolve_1_x_n_s8_get_buffer_size_ref(const nmsis_nn_dims* input_dims,
                                                  const nmsis_nn_dims* filter_dims)
{
    (void)input_dims;
    (void)filter_dims;
    return 0;
}

/**
 * @} end of NNConv group
 */
