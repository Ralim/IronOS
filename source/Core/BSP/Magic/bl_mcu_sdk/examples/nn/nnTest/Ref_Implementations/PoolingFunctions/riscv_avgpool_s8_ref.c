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
 * Title:        riscv_avgpool_s8.c
 * Description:  Pooling function implementations
 *
 * $Date:        May 29,2020
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
 * @addtogroup Pooling
 * @{
 */

/*
 * s8 average pooling function
 *
 * Refer to header file for details.
 *
 */

riscv_status riscv_avgpool_s8_ref(const nmsis_nn_context *ctx,
                          const nmsis_nn_pool_params *pool_params,
                          const nmsis_nn_dims *input_dims,
                          const q7_t *src,
                          const nmsis_nn_dims *filter_dims,
                          const nmsis_nn_dims *output_dims,
                          q7_t *dst)
{
  const int32_t dim_src_height = input_dims->h;
  const int32_t dim_src_width = input_dims->w;
  const int32_t dim_dst_height = output_dims->h;
  const int32_t dim_dst_width = output_dims->w;
  const int32_t stride_height = pool_params->stride.h;
  const int32_t stride_width = pool_params->stride.w;
  const int32_t dim_kernel_height = filter_dims->h;
  const int32_t dim_kernel_width = filter_dims->w;
  const int32_t padding_height = pool_params->padding.h;
  const int32_t padding_width = pool_params->padding.w;
  const int32_t act_min = pool_params->activation.min;
  const int32_t act_max = pool_params->activation.max;
  const int32_t ch_src = input_dims->c;
  q15_t *bufferA = (q15_t *)ctx->buf;

  /* Reference C code adapted from NMSIS-NN riscv_avepool_q7_HWC.
   */
  (void)bufferA;
  int16_t i_ch_in, i_x, i_y;
  int16_t k_x, k_y;

  for (i_y = 0; i_y < dim_dst_height; i_y++)
  {
    for (i_x = 0; i_x < dim_dst_width; i_x++)
    {
      for (i_ch_in = 0; i_ch_in < ch_src; i_ch_in++)
      {
        int sum = 0;
        int count = 0;
        for (k_y = i_y * stride_height - padding_height; k_y < i_y * stride_height - padding_height + dim_kernel_height; k_y++)
        {
          for (k_x = i_x * stride_width - padding_width; k_x < i_x * stride_width - padding_width + dim_kernel_width; k_x++)
          {
            if (k_y >= 0 && k_x >= 0 && k_y < dim_src_height && k_x < dim_src_width)
            {
              sum += src[i_ch_in + ch_src * (k_x + k_y * dim_src_width)];
              count++;
            }
          }
        }
        sum = sum > 0 ? (sum + count / 2) / count : (sum - count / 2) / count;
        sum = MAX(sum, act_min);
        sum = MIN(sum, act_max);

        dst[i_ch_in + ch_src * (i_x + i_y * dim_dst_width)] = sum;
      }
    }
  }

  return RISCV_MATH_SUCCESS;
}


int32_t riscv_avgpool_s8_get_buffer_size_ref(const int dim_dst_width,
                                       const int ch_src)
{
  (void)dim_dst_width;

  (void)ch_src;
  return 0;
}
/**
 * @} end of Pooling group
 */
