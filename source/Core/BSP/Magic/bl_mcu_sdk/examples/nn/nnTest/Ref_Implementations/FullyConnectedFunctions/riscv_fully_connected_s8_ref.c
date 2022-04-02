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
 * Title:        riscv_fully_connected_s8
 * Description:  Fully connected function compatible with TF Lite.
 *
 * $Date:        May 2, 2020
 * $Revision:    V.2.0.0
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
 * @addtogroup FC
 * @{
 */

/*
   * S8 basic fully-connected and matrix multiplication layer function for TensorFlow Lite
   *
   * Refer header file for details.
   *
   */

riscv_status
riscv_fully_connected_s8_ref(const nmsis_nn_context *ctx,
                           const nmsis_nn_fc_params *fc_params,
                           const nmsis_nn_per_tensor_quant_params *quant_params,
                           const nmsis_nn_dims *input_dims,
                           const q7_t *input,
                           const nmsis_nn_dims *filter_dims,
                           const q7_t *kernel,
                           const nmsis_nn_dims *bias_dims,
                           const int32_t *bias,
                           const nmsis_nn_dims *output_dims,
                           q7_t *output)
{
    (void)bias_dims;
    (void)ctx;
    int32_t batch_cnt = input_dims->n;

    while (batch_cnt)
    {
        riscv_nn_vec_mat_mult_t_s8_ref(input,
                                     kernel,
                                     bias,
                                     output,
                                     fc_params->input_offset,
                                     fc_params->filter_offset,
                                     fc_params->output_offset,
                                     quant_params->multiplier,
                                     quant_params->shift,
                                     filter_dims->n, /* col_dim or accum_depth */
                                     output_dims->c, /* row_dim or output_depth */
                                     fc_params->activation.min,
                                     fc_params->activation.max);
        input += filter_dims->n;
        output += output_dims->c;
        batch_cnt--;
    }
    return (RISCV_MATH_SUCCESS);
}

int32_t riscv_fully_connected_s8_get_buffer_size_ref(const nmsis_nn_dims *filter_dims)
{
    (void)filter_dims;
    return 0;
}

/**
 * @} end of FC group
 */
