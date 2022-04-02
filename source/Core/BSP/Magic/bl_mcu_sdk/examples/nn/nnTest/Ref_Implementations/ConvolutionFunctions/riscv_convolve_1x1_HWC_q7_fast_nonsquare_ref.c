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
 * Title:        riscv_convolve_1x1_HWC_q7_fast_nonsquare.c
 * Description:  Fast Q7 version of 1x1 convolution (non-square shape)
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
 * @brief Fast Q7 version of 1x1 convolution (non-sqaure shape)
 * @param[in]       Im_in        pointer to input tensor
 * @param[in]       dim_im_in_x  input tensor dimention x
 * @param[in]       dim_im_in_y  input tensor dimention y
 * @param[in]       ch_im_in     number of input tensor channels
 * @param[in]       wt           pointer to kernel weights
 * @param[in]       ch_im_out    number of filters, i.e., output tensor channels
 * @param[in]       dim_kernel_x filter kernel size x
 * @param[in]       dim_kernel_y filter kernel size y
 * @param[in]       padding_x    padding size x
 * @param[in]       padding_y    padding size y
 * @param[in]       stride_x     convolution stride x
 * @param[in]       stride_y     convolution stride y
 * @param[in]       bias         pointer to bias
 * @param[in]       bias_shift   amount of left-shift for bias
 * @param[in]       out_shift    amount of right-shift for output
 * @param[in,out]   Im_out       pointer to output tensor
 * @param[in]       dim_im_out_x output tensor dimension x
 * @param[in]       dim_im_out_y output tensor dimension y
 * @param[in,out]   bufferA      pointer to buffer space for input
 * @param[in,out]   bufferB      pointer to buffer space for output
 * @return     The function returns either
 * <code>RISCV_MATH_SIZE_MISMATCH</code> or <code>RISCV_MATH_SUCCESS</code> based on the outcome of size checking.
 *
 * This function is optimized for convolution with 1x1 kernel size (i.e., dim_kernel_x=1
 * and dim_kernel_y=1). It can be used for the second half of MobileNets [1] after depthwise
 * separable convolution.
 *
 * This function is the version with full list of optimization tricks, but with
 * some contraints:
 *   ch_im_in is multiple of 4
 *   ch_im_out is multiple of 2
 *
 * [1] MobileNets: Efficient Convolutional Neural Networks for Mobile Vision Applications
 * https://arxiv.org/abs/1704.04861
 */

riscv_status riscv_convolve_1x1_HWC_q7_fast_nonsquare_ref(const q7_t * Im_in,
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

    int       i, j, k, l, m, n;
    int       conv_out;
    int       in_row, in_col;

    if (ch_im_in % 4 != 0 || ch_im_out % 2 != 0 || dim_kernel_x != 1 || dim_kernel_y != 1
        || padding_x != 0 || padding_y != 0 || stride_x != 1 || stride_y != 1)
    {
        /* check if the input dimension meets the constraints */
        return RISCV_MATH_SIZE_MISMATCH;
    }

    for (i = 0; i < ch_im_out; i++)
    {
        for (j = 0; j < dim_im_out_y; j++)
        {
            for (k = 0; k < dim_im_out_x; k++)
            {
                conv_out = ((q31_t)(bias[i]) << bias_shift) + NN_ROUND(out_shift);
                for (m = 0; m < dim_kernel_y; m++)
                {
                    for (n = 0; n < dim_kernel_x; n++)
                    {
                        // if-for implementation
                        in_row = stride_y * j + m - padding_y;
                        in_col = stride_x * k + n - padding_x;
                        if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
                        {
                            for (l = 0; l < ch_im_in; l++)
                            {
                                conv_out += Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in + l] *
                                    wt[i * ch_im_in * dim_kernel_y * dim_kernel_x + (m * dim_kernel_y + n) * ch_im_in + l];
                            }
                        }
                    }
                }
                Im_out[i + (j * dim_im_out_x + k) * ch_im_out] = (q7_t) __SSAT((conv_out >> out_shift), 8);
            }
        }
    }
    /* Return to application */
    return RISCV_MATH_SUCCESS;
}

/**
 * @} end of NNConv group
 */
