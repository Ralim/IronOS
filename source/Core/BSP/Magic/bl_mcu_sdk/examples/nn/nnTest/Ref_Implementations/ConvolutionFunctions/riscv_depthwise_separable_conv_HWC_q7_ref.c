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
 * Title:        riscv_depthwise_separable_conv_HWC_q7.c
 * Description:  Q7 depthwise separable convolution function
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
 * @addtogroup NNConv
 * @{
 */

/**
 * @brief Q7 depthwise separable convolution function
 * @param[in]       Im_in       pointer to input tensor
 * @param[in]       dim_im_in   input tensor dimention
 * @param[in]       ch_im_in    number of input tensor channels
 * @param[in]       wt          pointer to kernel weights
 * @param[in]       ch_im_out   number of filters, i.e., output tensor channels
 * @param[in]       dim_kernel  filter kernel size
 * @param[in]       padding     padding sizes
 * @param[in]       stride      convolution stride
 * @param[in]       bias        pointer to bias
 * @param[in]       bias_shift  amount of left-shift for bias
 * @param[in]       out_shift   amount of right-shift for output
 * @param[in,out]   Im_out      pointer to output tensor
 * @param[in]       dim_im_out  output tensor dimension
 * @param[in,out]   bufferA     pointer to buffer space for input
 * @param[in,out]   bufferB     pointer to buffer space for output
 * @return     The function returns either
 * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
 *
 * @details
 *
 * <b>Buffer size:</b>
 *
 * bufferA size: 2*ch_im_in*dim_kernel*dim_kernel
 *
 * bufferB size: 0
 *
 * <b>Input dimension constraints:</b>
 *
 * ch_im_in equals ch_im_out
 *
 * Implementation:
 * There are 3 nested loop here:
 * Inner loop: calculate each output value with MAC instruction over an accumulator
 * Mid   loop: loop over different output channel
 * Outer loop: loop over different output (x, y)
 */
//
//      REMOVED
//
// riscv_status riscv_depthwise_separable_conv_HWC_q7_ref(const q7_t * Im_in,
//                                                    const uint16_t dim_im_in,
//                                                    const uint16_t ch_im_in,
//                                                    const q7_t * wt,
//                                                    const uint16_t ch_im_out,
//                                                    const uint16_t dim_kernel,
//                                                    const uint16_t padding,
//                                                    const uint16_t stride,
//                                                    const q7_t * bias,
//                                                    const uint16_t bias_shift,
//                                                    const uint16_t out_shift,
//                                                    q7_t * Im_out,
//                                                    const uint16_t dim_im_out,
//                                                    q15_t * bufferA,
//                                                    q7_t * bufferB)
// {
//     (void)bufferB;
//     /* Run the following code as reference implementation for RISC-V Core without DSP */
//     int       i_out_y, i_out_x, i_ch_out, i_ker_x, i_ker_y;
//     int       conv_out;

//     /* do some checking here, basically ch_im_in == ch_im_out */
//     if (ch_im_in != ch_im_out)
//     {
//         return RISCV_MATH_SIZE_MISMATCH;
//     }

//     for (i_out_y = 0; i_out_y < dim_im_out; i_out_y++)
//     {
//         for (i_out_x = 0; i_out_x < dim_im_out; i_out_x++)
//         {
//             for (i_ch_out = 0; i_ch_out < ch_im_out; i_ch_out++)
//             {
//                 // for each output
//                 conv_out = ((q31_t)(bias[i_ch_out]) << bias_shift) + NN_ROUND(out_shift);
//                 for (i_ker_y = 0; i_ker_y < dim_kernel; i_ker_y++)
//                 {
//                     for (i_ker_x = 0; i_ker_x < dim_kernel; i_ker_x++)
//                     {
//                         int       in_row = stride * i_out_y + i_ker_y - padding;
//                         int       in_col = stride * i_out_x + i_ker_x - padding;
//                         if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in && in_col < dim_im_in)
//                         {
//                             conv_out +=
//                                 Im_in[(in_row *
//                                        dim_im_in +
//                                        in_col) *
//                                       ch_im_in +
//                                       i_ch_out] * wt[(i_ker_y * dim_kernel + i_ker_x) * ch_im_out + i_ch_out];
//                         }
//                     }
//                 }
//                 Im_out[(i_out_y * dim_im_out +
//                         i_out_x) * ch_im_out + i_ch_out] = (q7_t) __SSAT((conv_out >> out_shift), 8);
//             }
//         }
//     }
//     /* Return to application */
//     return RISCV_MATH_SUCCESS;

// }

/**
 * @} end of NNConv group
 */
