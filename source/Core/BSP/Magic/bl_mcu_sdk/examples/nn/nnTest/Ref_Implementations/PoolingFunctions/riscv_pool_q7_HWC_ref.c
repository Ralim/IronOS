/*
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        riscv_pool_q7_HWC.c
 * Description:  Pooling function implementations
 *
 * $Date:        17. January 2018
 * $Revision:    V.1.0.0
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

  /**
   * @brief Q7 max pooling function
   * @param[in, out]  Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     Not used
   * @param[in,out]   Im_out      pointer to output tensor
   *
   * @details
   *
   * The pooling function is implemented as split x-pooling then
   * y-pooling.
   *
   * This pooling function is input-destructive. Input data is undefined
   * after calling this function.
   *
   */

//      REMOVED

// void
// riscv_maxpool_q7_HWC_ref(q7_t * Im_in,
//                    const uint16_t dim_im_in,
//                    const uint16_t ch_im_in,
//                    const uint16_t dim_kernel,
//                    const uint16_t padding,
//                    const uint16_t stride, const uint16_t dim_im_out, q7_t * bufferA, q7_t * Im_out)
// {
//     (void)bufferA;
//     /* Run the following code as reference implementation for RISC-V Core without DSP */
//     int16_t   i_ch_in, i_x, i_y;
//     int16_t   k_x, k_y;

//     for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
//     {
//         for (i_y = 0; i_y < dim_im_out; i_y++)
//         {
//             for (i_x = 0; i_x < dim_im_out; i_x++)
//             {
//                 int       max = -129;
//                 for (k_y = i_y * stride - padding; k_y < i_y * stride - padding + dim_kernel; k_y++)
//                 {
//                     for (k_x = i_x * stride - padding; k_x < i_x * stride - padding + dim_kernel; k_x++)
//                     {
//                         if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in && k_x < dim_im_in)
//                         {
//                             if (Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in)] > max)
//                             {
//                                 max = Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in)];
//                             }
//                         }
//                     }
//                 }
//                 Im_out[i_ch_in + ch_im_in * (i_x + i_y * dim_im_out)] = max;
//             }
//         }
//     }
// }

  /**
   * @brief Q7 average pooling function
   * @param[in,out]   Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     pointer to buffer space for input
   * @param[in,out]   Im_out      pointer to output tensor
   *
   * @details
   *
   * <b>Buffer size:</b>
   *
   * bufferA size:  2*dim_im_out*ch_im_in
   *
   * The pooling function is implemented as split x-pooling then
   * y-pooling.
   *
   * This pooling function is input-destructive. Input data is undefined
   * after calling this function.
   *
   */

  //        REMOVED

// void
// riscv_avepool_q7_HWC_ref(q7_t * Im_in,
//                    const uint16_t dim_im_in,
//                    const uint16_t ch_im_in,
//                    const uint16_t dim_kernel,
//                    const uint16_t padding,
//                    const uint16_t stride, const uint16_t dim_im_out, q7_t * bufferA, q7_t * Im_out)
// {

//     /* Run the following code as reference implementation for RISC-V Core without DSP */

//     (void)bufferA;
//     int16_t   i_ch_in, i_x, i_y;
//     int16_t   k_x, k_y;

//     for (i_ch_in = 0; i_ch_in < ch_im_in; i_ch_in++)
//     {
//         for (i_y = 0; i_y < dim_im_out; i_y++)
//         {
//             for (i_x = 0; i_x < dim_im_out; i_x++)
//             {
//                 int       sum = 0;
//                 int       count = 0;
//                 for (k_y = i_y * stride - padding; k_y < i_y * stride - padding + dim_kernel; k_y++)
//                 {
//                     for (k_x = i_x * stride - padding; k_x < i_x * stride - padding + dim_kernel; k_x++)
//                     {
//                         if (k_y >= 0 && k_x >= 0 && k_y < dim_im_in && k_x < dim_im_in)
//                         {
//                             sum += Im_in[i_ch_in + ch_im_in * (k_x + k_y * dim_im_in)];
//                             count++;
//                         }
//                     }
//                 }
//                 Im_out[i_ch_in + ch_im_in * (i_x + i_y * dim_im_out)] = sum / count;
//             }
//         }
//     }
// }

/**
 * @} end of Pooling group
 */
