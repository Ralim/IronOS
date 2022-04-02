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
 * Title:        riscv_depthwise_separable_conv_HWC_q7_nonsquare.c
 * Description:  Q7 depthwise separable convolution function (non-square shape)
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
 * @brief Q7 depthwise separable convolution function (non-square shape)
 * @param[in]       Im_in         pointer to input tensor
 * @param[in]       dim_im_in_x   input tensor dimention x
 * @param[in]       dim_im_in_y   input tensor dimention y
 * @param[in]       ch_im_in      number of input tensor channels
 * @param[in]       wt            pointer to kernel weights
 * @param[in]       ch_im_out     number of filters, i.e., output tensor channels
 * @param[in]       dim_kernel_x  filter kernel size x
 * @param[in]       dim_kernel_y  filter kernel size y
 * @param[in]       padding_x     padding sizes x
 * @param[in]       padding_y     padding sizes y
 * @param[in]       stride_x      convolution stride x
 * @param[in]       stride_y      convolution stride y
 * @param[in]       bias          pointer to bias
 * @param[in]       bias_shift    amount of left-shift for bias
 * @param[in]       out_shift     amount of right-shift for output
 * @param[in,out]   Im_out        pointer to output tensor
 * @param[in]       dim_im_out_x  output tensor dimension x
 * @param[in]       dim_im_out_y  output tensor dimension y
 * @param[in,out]   bufferA       pointer to buffer space for input
 * @param[in,out]   bufferB       pointer to buffer space for output
 * @return     The function returns either
 * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
 *
 * This function is the version with full list of optimization tricks, but with
 * some contraints:
 *   ch_im_in is equal to ch_im_out
 *
 */

riscv_status riscv_depthwise_separable_conv_HWC_q7_nonsquare_ref(const q7_t * Im_in,
                                                             const uint16_t dim_im_in_x,
                                                             const uint16_t dim_im_in_y,
                                                             const uint16_t ch_im_in,
                                                             const q7_t * wt,
                                                             const uint16_t ch_im_out,
                                                             const uint16_t dim_kernel_x,
                                                             const uint16_t dim_kernel_y,
                                                             const uint16_t padding_x,
                                                             const uint16_t padding_y,
                                                             const uint16_t stride_x,
                                                             const uint16_t stride_y,
                                                             const q7_t * bias,
                                                             const uint16_t bias_shift,
                                                             const uint16_t out_shift,
                                                             q7_t * Im_out,
                                                             const uint16_t dim_im_out_x,
                                                             const uint16_t dim_im_out_y,
                                                             q15_t * bufferA,
                                                             q7_t * bufferB)
{

    (void)bufferB;

    /* Run the following code as reference implementation for RISC-V Core without DSP */
    int       i_out_y, i_out_x, i_ch_out;
    int       i_ker_y, i_ker_x;

    /* do some checking here, basically ch_im_in == ch_im_out */
    if (ch_im_in != ch_im_out)
    {
        return RISCV_MATH_SIZE_MISMATCH;
    }

    for (i_out_y = 0; i_out_y < dim_im_out_y; i_out_y++)
    {
        for (i_out_x = 0; i_out_x < dim_im_out_x; i_out_x++)
        {
            for (i_ch_out = 0; i_ch_out < ch_im_out; i_ch_out++)
            {
                // for each output
                int       conv_out = ((q31_t)(bias[i_ch_out]) << bias_shift) + NN_ROUND(out_shift);
                for (i_ker_y = 0; i_ker_y < dim_kernel_y; i_ker_y++)
                {
                    for (i_ker_x = 0; i_ker_x < dim_kernel_x; i_ker_x++)
                    {
                        int       in_row = stride_y * i_out_y + i_ker_y - padding_y;
                        int       in_col = stride_x * i_out_x + i_ker_x - padding_x;
                        if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
                        {
                            conv_out += Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in + i_ch_out] *
                                wt[(i_ker_y * dim_kernel_x + i_ker_x) * ch_im_out + i_ch_out];
                        }
                    }
                }
                Im_out[(i_out_y * dim_im_out_x + i_out_x) * ch_im_out + i_ch_out] =
                    (q7_t) __SSAT((conv_out >> out_shift), 8);
            }
        }
    }
    /* Return to application */
    return RISCV_MATH_SUCCESS;

}

/**
 * @} end of NNConv group
 */
