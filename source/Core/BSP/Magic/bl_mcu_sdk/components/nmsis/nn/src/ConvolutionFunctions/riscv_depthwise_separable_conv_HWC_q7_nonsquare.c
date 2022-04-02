/*
 * Copyright (C) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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
 * $Date:        January 26, 2021
 * $Revision:    V.1.0.2
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnfunctions.h"
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
 * @param[in]       dim_im_in_x   input tensor dimension x
 * @param[in]       dim_im_in_y   input tensor dimension y
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
 * some constraints:
 *   ch_im_in is equal to ch_im_out
 *
 */

riscv_status riscv_depthwise_separable_conv_HWC_q7_nonsquare(const q7_t *Im_in,
                                                         const uint16_t dim_im_in_x,
                                                         const uint16_t dim_im_in_y,
                                                         const uint16_t ch_im_in,
                                                         const q7_t *wt,
                                                         const uint16_t ch_im_out,
                                                         const uint16_t dim_kernel_x,
                                                         const uint16_t dim_kernel_y,
                                                         const uint16_t padding_x,
                                                         const uint16_t padding_y,
                                                         const uint16_t stride_x,
                                                         const uint16_t stride_y,
                                                         const q7_t *bias,
                                                         const uint16_t bias_shift,
                                                         const uint16_t out_shift,
                                                         q7_t *Im_out,
                                                         const uint16_t dim_im_out_x,
                                                         const uint16_t dim_im_out_y,
                                                         q15_t *bufferA,
                                                         q7_t *bufferB)
{

    (void)bufferB;

#if defined (RISCV_MATH_DSP) || defined (RISCV_VECTOR)
    /* Run the following code for RISC-V Core with DSP enabled */

    /*
     * Implementation:
     * There are 3 nested loop here:
     * Inner loop: calculate each output value with MAC instruction over an accumulator
     * Mid   loop: loop over different output channel
     * Outer loop: loop over different output (x, y)
     *
     */

    int16_t i_out_y, i_out_x;
    int16_t i_ker_y, i_ker_x;
    q7_t *colBuffer = (q7_t *)bufferA;
    q7_t *pBuffer = colBuffer;
    const q7_t *pBias = bias;
    q7_t *pOut = Im_out;
    uint16_t rowCnt;
    uint16_t row_shift;

#if defined(RISCV_VECTOR)
    size_t l;
    uint32_t blkCnt;
    ptrdiff_t bstride;
    vint8m2_t vam2,vbm2;
    vint32m1_t vtemp;
    l = vsetvl_e32m1(1);
    vtemp = vsub_vv_i32m1(vtemp,vtemp, l);
#endif

    /* do some checking here, basically ch_im_in == ch_im_out */
    if (ch_im_in != ch_im_out)
    {
        return RISCV_MATH_SIZE_MISMATCH;
    }

    for (i_out_y = 0; i_out_y < dim_im_out_y; i_out_y++)
    {
        for (i_out_x = 0; i_out_x < dim_im_out_x; i_out_x++)
        {
            /* we first do im2col here */
            for (i_ker_y = i_out_y * stride_y - padding_y; i_ker_y < i_out_y * stride_y - padding_y + dim_kernel_y;
                 i_ker_y++)
            {
                for (i_ker_x = i_out_x * stride_x - padding_x; i_ker_x < i_out_x * stride_x - padding_x + dim_kernel_x;
                     i_ker_x++)
                {
                    if (i_ker_y < 0 || i_ker_y >= dim_im_in_y || i_ker_x < 0 || i_ker_x >= dim_im_in_x)
                    {
                        riscv_fill_q7(0, pBuffer, ch_im_in);
                        /* memset(pBuffer, 0, ch_im_in); */
                    } else
                    {
                        riscv_copy_q7((q7_t *) Im_in + (i_ker_y * dim_im_in_x + i_ker_x) * ch_im_in, pBuffer, ch_im_in);
                        /* memcpy(pBuffer, (q7_t *) Im_in + (i_ker_y * dim_im_in_x + i_ker_x) * ch_im_in, ch_im_in); */
                    }
                    pBuffer += ch_im_in;
                }
            }

            /* we will do the computation here for each channel */
            rowCnt = ch_im_out >> 2;
            row_shift = 0;
            pBias = bias;
#if defined(RISCV_VECTOR)
            rowCnt = ch_im_out;
            bstride = ch_im_in;
            while (rowCnt)
            {
                q7_t     *pB = colBuffer + row_shift;
                const q7_t *pA = wt + row_shift;
                q31_t     sum = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
                uint16_t  colCnt = (dim_kernel_x * dim_kernel_y);

                row_shift += 1;

                blkCnt = colCnt;
                for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
                    vam2 = vlse8_v_i8m2(pA,bstride, l);
                    vbm2 = vlse8_v_i8m2(pB,bstride, l);
                    pA += l*ch_im_in;
                    pB += l*ch_im_in;
                    l = vsetvl_e8m2(blkCnt);
                    sum += vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(vtemp, vwmul_vv_i16m4(vam2, vbm2, l), vtemp, l));
                }
                *pOut++ = (q7_t) __SSAT((sum >> out_shift), 8);
                rowCnt--;
            }
#else
            while (rowCnt)
            {
                q31_t     sum =  ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
                q31_t     sum2 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
                q31_t     sum3 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
                q31_t     sum4 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
#if defined (RISCV_MATH_DSP)
                uint16_t  colCnt = (dim_kernel_x * dim_kernel_y) >> 2;
                q7_t     *pB = colBuffer + row_shift;
                const q7_t *pA = wt + row_shift;
                row_shift += 4;

                while (colCnt)
                {

                    q31_t     A1, A2, A3 , A4 , B1, B2, B3, B4 ;
                    q31_t     opA1, opA2, opA3 , opA4 , opB1, opB2, opB3, opB4 ;

                    opB1 = *__SIMD32(pB);
                    pB += ch_im_in;
                    opB2 = *__SIMD32(pB);
                    pB += ch_im_in;
                    opB3 = *__SIMD32(pB);
                    pB += ch_im_in;
                    opB4 = *__SIMD32(pB);
                    pB += ch_im_in;

                    B1 = (opB1 & 0x000000ff) |
                         ((opB2 & 0x000000ff)<<8) |
                         ((opB3 & 0x000000ff)<<16) |
                         ((opB4 & 0x000000ff)<<24);

                    B2 = ((opB1 & 0x0000ff00)>>8) |
                         ((opB2 & 0x0000ff00)) |
                         ((opB3 & 0x0000ff00)<<8) |
                         ((opB4 & 0x0000ff00)<<16);

                    B3 = ((opB1 & 0x00ff0000)>>16) |
                         ((opB2 & 0x00ff0000)>>8) |
                         ((opB3 & 0x00ff0000)) |
                         ((opB4 & 0x00ff0000)<<8);

                    B4 = ((opB1 & 0xff000000)>>24) |
                         ((opB2 & 0xff000000)>>16) |
                         ((opB3 & 0xff000000)>>8) |
                         ((opB4 & 0xff000000));

                    opA1 = *__SIMD32(pA);
                    pA += ch_im_in;
                    opA2 = *__SIMD32(pA);
                    pA += ch_im_in;
                    opA3 = *__SIMD32(pA);
                    pA += ch_im_in;
                    opA4 = *__SIMD32(pA);
                    pA += ch_im_in;

                    A1 = (opA1 & 0x000000ff) |
                         ((opA2 & 0x000000ff)<<8) |
                         ((opA3 & 0x000000ff)<<16) |
                         ((opA4 & 0x000000ff)<<24);

                    A2 = ((opA1 & 0x0000ff00)>>8) |
                         ((opA2 & 0x0000ff00)) |
                         ((opA3 & 0x0000ff00)<<8) |
                         ((opA4 & 0x0000ff00)<<16);

                    A3 = ((opA1 & 0x00ff0000)>>16) |
                         ((opA2 & 0x00ff0000)>>8) |
                         ((opA3 & 0x00ff0000)) |
                         ((opA4 & 0x00ff0000)<<8);

                    A4 = ((opA1 & 0xff000000)>>24) |
                         ((opA2 & 0xff000000)>>16) |
                         ((opA3 & 0xff000000)>>8) |
                         ((opA4 & 0xff000000));

                    sum =  __RV_SMAQA(sum , A1, B1);
                    sum2 = __RV_SMAQA(sum2, A2, B2);
                    sum3 = __RV_SMAQA(sum3, A3, B3);
                    sum4 = __RV_SMAQA(sum4, A4, B4);
                    colCnt--;
                }


                colCnt = (dim_kernel_x * dim_kernel_y) & 0x3;
#else
                uint16_t  colCnt = (dim_kernel_x * dim_kernel_y);
                q7_t     *pB = colBuffer + row_shift;
                const q7_t *pA = wt + row_shift;
                row_shift += 4;
#endif /*ndef RISCV_VECTOR*/
                while (colCnt)
                {
                    union riscv_nnword inA, inB;
                    inA.word = riscv_nn_read_q7x4(pA);
                    pA += ch_im_in;
                    inB.word = riscv_nn_read_q7x4(pB);
                    pB += ch_im_in;
                    sum += inA.bytes[0] * inB.bytes[0];
                    sum2 += inA.bytes[1] * inB.bytes[1];
                    sum3 += inA.bytes[2] * inB.bytes[2];
                    sum4 += inA.bytes[3] * inB.bytes[3];
                    colCnt--;
                }

                *pOut++ = (q7_t)__SSAT((sum >> out_shift), 8);
                *pOut++ = (q7_t)__SSAT((sum2 >> out_shift), 8);
                *pOut++ = (q7_t)__SSAT((sum3 >> out_shift), 8);
                *pOut++ = (q7_t)__SSAT((sum4 >> out_shift), 8);

                rowCnt--;
            }

            rowCnt = ch_im_out & 0x3;
            while (rowCnt)
            {
                q7_t *pB = colBuffer + row_shift;
                const q7_t *pA = wt + row_shift;
                q31_t sum = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
                uint16_t colCnt = (dim_kernel_x * dim_kernel_y);

                row_shift += 1;

                while (colCnt)
                {
                    q7_t A1 = *pA;
                    q7_t B1 = *pB;
                    pA += ch_im_in;
                    pB += ch_im_in;
                    sum += A1 * B1;

                    colCnt--;
                }
                *pOut++ = (q7_t)__SSAT((sum >> out_shift), 8);
                rowCnt--;
            }
#endif
            // clear counter and pointers
            pBuffer = colBuffer;
        }
    }

#else
    (void)bufferA;

    /* Run the following code as reference implementation for RISC-V Core without DSP */
    int i_out_y, i_out_x, i_ch_out;
    int i_ker_y, i_ker_x;

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
                int conv_out = ((q31_t)(bias[i_ch_out]) << bias_shift) + NN_ROUND(out_shift);
                for (i_ker_y = 0; i_ker_y < dim_kernel_y; i_ker_y++)
                {
                    for (i_ker_x = 0; i_ker_x < dim_kernel_x; i_ker_x++)
                    {
                        int in_row = stride_y * i_out_y + i_ker_y - padding_y;
                        int in_col = stride_x * i_out_x + i_ker_x - padding_x;
                        if (in_row >= 0 && in_col >= 0 && in_row < dim_im_in_y && in_col < dim_im_in_x)
                        {
                            conv_out += Im_in[(in_row * dim_im_in_x + in_col) * ch_im_in + i_ch_out] *
                                wt[(i_ker_y * dim_kernel_x + i_ker_x) * ch_im_out + i_ch_out];
                        }
                    }
                }
                Im_out[(i_out_y * dim_im_out_x + i_out_x) * ch_im_out + i_ch_out] =
                    (q7_t)__SSAT((conv_out >> out_shift), 8);
            }
        }
    }

#endif /* RISCV_MATH_DSP */

    /* Return to application */
    return RISCV_MATH_SUCCESS;
}

/**
 * @} end of NNConv group
 */
