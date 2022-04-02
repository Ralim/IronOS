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
 * Title:        riscv_fully_connected_q7_opt.c
 * Description:  Q7 basic fully-connected layer function
 *
 * $Date:        09. October 2020
 * $Revision:    V.1.0.1
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
 * @addtogroup FC
 * @{
 */

/**
 * @brief Q7 opt fully-connected layer function
 * @param[in]       pV          pointer to input vector
 * @param[in]       pM          pointer to matrix weights
 * @param[in]       dim_vec     length of the vector
 * @param[in]       num_of_rows number of rows in weight matrix
 * @param[in]       bias_shift  amount of left-shift for bias
 * @param[in]       out_shift   amount of right-shift for output
 * @param[in]       bias        pointer to bias
 * @param[in,out]   pOut        pointer to output vector
 * @param[in,out]   vec_buffer  pointer to buffer space for input
 * @return     The function returns <code>RISCV_MATH_SUCCESS</code>
 *
 * @details
 *
 * <b>Buffer size:</b>
 *
 * vec_buffer size: dim_vec
 *
 * This opt function is designed to work with interleaved weight
 * matrix. The vector input is assumed in q7_t format, we call
 *  riscv_q7_to_q15_no_shift_shuffle function to expand into
 *  q15_t format with certain weight re-ordering, refer to the function
 *  comments for more details.
 *  Here we use only one pointer to read 4 rows in the weight
 *  matrix. So if the original q7_t matrix looks like this:
 *
 *  | a11 | a12 | a13 | a14 | a15 | a16 | a17 |
 *
 *  | a21 | a22 | a23 | a24 | a25 | a26 | a27 |
 *
 *  | a31 | a32 | a33 | a34 | a35 | a36 | a37 |
 *
 *  | a41 | a42 | a43 | a44 | a45 | a46 | a47 |
 *
 *  | a51 | a52 | a53 | a54 | a55 | a56 | a57 |
 *
 *  | a61 | a62 | a63 | a64 | a65 | a66 | a67 |
 *
 *
 *  We operates on multiple-of-4 rows, so the first four rows becomes
 *
 *  | a11 | a21 | a13 | a23 | a31 | a41 | a33 | a43 |
 *
 *  | a12 | a22 | a14 | a24 | a32 | a42 | a34 | a44 |
 *
 *  | a15 | a25 | a35 | a45 | a16 | a26 | a36 | a46 |
 *
 *  So within the kernel, we first read the re-ordered vector in as:
 *
 *  | b1  | b3  | and | b2  | b4  |
 *
 *  the four q31_t weights will look like
 *
 *  | a11 | a13 |, | a21 | a23 |, | a31 | a33 |, | a41 | a43 |
 *
 *  | a12 | a14 |, | a22 | a24 |, | a32 | a34 |, | a42 | a44 |
 *
 *  The column left over will be in-order.
 *  which is:
 *
 *  | a17 | a27 | a37 | a47 |
 *
 *  For the left-over rows, we do 1x1 computation, so the data remains
 *  as its original order.
 *
 *  So the stored weight matrix looks like this:
 *
 *  | a11 | a21 | a13 | a23 | a31 | a41 |
 *
 *  | a33 | a43 | a12 | a22 | a14 | a24 |
 *
 *  | a32 | a42 | a34 | a44 | a15 | a25 |
 *
 *  | a35 | a45 | a16 | a26 | a36 | a46 |
 *
 *  | a17 | a27 | a37 | a47 | a51 | a52 |
 *
 *  | a53 | a54 | a55 | a56 | a57 | a61 |
 *
 *  | a62 | a63 | a64 | a65 | a66 | a67 |
 *
 *
 */

riscv_status riscv_fully_connected_q7_opt(const q7_t *pV,
                                      const q7_t *pM,
                                      const uint16_t dim_vec,
                                      const uint16_t num_of_rows,
                                      const uint16_t bias_shift,
                                      const uint16_t out_shift,
                                      const q7_t *bias,
                                      q7_t *pOut,
                                      q15_t *vec_buffer)
{

#if defined(RISCV_MATH_DSP)
    /* Run the following code for RISC-V Core with DSP enabled */

    const q7_t *pB = pM;
    q7_t *pO = pOut;
    const q7_t *pBias = bias;
    q7_t    *pA;
    uint16_t  rowCnt = num_of_rows >> 2;
    //q7_t    *vec_buffer_q7 = (q7_t *)vec_buffer;

    riscv_q7_to_q7_reordered_no_shift(pV, (q7_t *)vec_buffer, dim_vec);

    while (rowCnt)
    {

        q31_t sum = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        q31_t sum2 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        q31_t sum3 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        q31_t sum4 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);

        uint16_t colCnt = dim_vec >> 2;

        pA = (q7_t *)vec_buffer;
        while (colCnt)
        {

            q31_t     inA1 = *__SIMD32(pA)++;
            q31_t     inB1 = *__SIMD32(pB)++;
            q31_t     inB2 = *__SIMD32(pB)++;
            q31_t     inB3 = *__SIMD32(pB)++;
            q31_t     inB4 = *__SIMD32(pB)++;

            q31_t     sumb1 = inB1 & 0x000000ff | (inB3 & 0x000000ff) << 8 |
                              inB1 & 0x00ff0000 | (inB3 & 0x00ff0000) << 8  ;

            q31_t     sumb2 = (inB1 & 0x0000ff00) >> 8 | (inB3 & 0x0000ff00) |
                              ((inB1 & 0xff000000) >> 8) & 0x00ff0000 | (inB3 & 0xff000000) ;

            q31_t     sumb3 = inB2 & 0x000000ff | (inB4 & 0x000000ff) << 8 |
                              inB2 & 0x00ff0000 | (inB4 & 0x00ff0000) << 8  ;

            q31_t     sumb4 = (inB2 & 0x0000ff00) >> 8 | (inB4 & 0x0000ff00) |
                              ((inB2 & 0xff000000) >> 8) & 0x00ff0000 | (inB4 & 0xff000000) ;

            sum  = __RV_SMAQA(sum , inA1, sumb1);
            sum2 = __RV_SMAQA(sum2, inA1, sumb2);
            sum3 = __RV_SMAQA(sum3, inA1, sumb3);
            sum4 = __RV_SMAQA(sum4, inA1, sumb4);

            colCnt--;
        }

        colCnt = dim_vec & 0x3;

        while (colCnt)
        {
            q7_t     inV = *pA++;
            q7_t      inM = *pB++;
            q7_t      inM2 = *pB++;
            q7_t      inM3 = *pB++;
            q7_t      inM4 = *pB++;

            sum += inV * inM;
            sum2 += inV * inM2;
            sum3 += inV * inM3;
            sum4 += inV * inM4;
            colCnt--;
        } /* while over colCnt */
        *pO++ = (q7_t)(__SSAT((sum >> out_shift), 8));
        *pO++ = (q7_t)(__SSAT((sum2 >> out_shift), 8));
        *pO++ = (q7_t)(__SSAT((sum3 >> out_shift), 8));
        *pO++ = (q7_t)(__SSAT((sum4 >> out_shift), 8));

        /* adjust the pointers and counters */
        rowCnt--;
    }

    /* left-over part of the rows */
    rowCnt = num_of_rows & 0x3;

    while (rowCnt)
    {
        q31_t     sum = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);

        pA = (q7_t *)vec_buffer;
#if __RISCV_XLEN == 64
        uint16_t  colCnt = dim_vec >> 3;
        q63_t sum64 = 0;
        while (colCnt)
        {

            q63_t     inA1 = *__SIMD64(pA)++;
            q63_t     inB1 = *__SIMD64(pB)++;

            sum64  = __RV_SMAQA(sum64, inA1, inB1);

            colCnt--;
        }
        sum = sum + (q31_t)(sum64 & 0xFFFFFFFF) + (q31_t)((sum64 & 0xFFFFFFFF00000000)>>32);
        /* left-over of the vector */
        colCnt = dim_vec & 0x7;

#else
        uint16_t  colCnt = dim_vec >> 2;
        while (colCnt)
        {

            q31_t     inA1 = *__SIMD32(pA)++;
            q31_t     inB1 = *__SIMD32(pB)++;

            sum  = __RV_SMAQA(sum, inA1, inB1);

            colCnt--;
        }

        /* left-over of the vector */
        colCnt = dim_vec & 0x3;
#endif /* __RISCV_XLEN == 64 */

        while (colCnt)
        {
            q15_t inV = *pA++;
            q7_t inM = *pB++;
            sum += inV * inM;
            colCnt--;
        }

        *pO++ = (q7_t)(__SSAT((sum >> out_shift), 8));

        rowCnt--;
    }

#else
    /* Run the following code as reference implementation for RISC-V Core without DSP */
    uint16_t rowCnt = num_of_rows >> 2;
    const q7_t *pB = pM;
    const q7_t *pA;
    q7_t *pO = pOut;
    const q7_t *pBias = bias;

#if defined(RISCV_VECTOR)
    size_t l;
    uint32_t blkCnt;
    ptrdiff_t bstridea;
    ptrdiff_t bstrideb;
    vint8m2_t v_a1,v_a2,v_a3,v_a4;
    vint8m2_t v_b1,v_b2,v_b3,v_b4;
    vint32m1_t vtemp;
    l = vsetvl_e32m1(1);
    vtemp = vsub_vv_i32m1(vtemp,vtemp, l);
#endif

    while (rowCnt)
    {
        q31_t sum = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        q31_t sum2 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        q31_t sum3 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        q31_t sum4 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);

        uint16_t colCnt = dim_vec >> 2;

        pA = pV;
#if defined(RISCV_VECTOR)
        bstridea = 4;
        bstrideb = 16;
        blkCnt = colCnt;
        for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
            v_a1 = vlse8_v_i8m2(pA++,bstridea, l);
            v_a3 = vlse8_v_i8m2(pA++,bstridea, l);
            v_a2 = vlse8_v_i8m2(pA++,bstridea, l);
            v_a4 = vlse8_v_i8m2(pA++,bstridea, l);
            v_b1 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b3 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b2 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b4 = vlse8_v_i8m2(pB++,bstrideb, l);

            l = vsetvl_e8m2(blkCnt);
            sum += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a1, v_b1, l),vwmul_vv_i16m4(v_a2, v_b2, l), l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            sum2 += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a1, v_b3, l),vwmul_vv_i16m4(v_a2, v_b4, l), l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            v_b1 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b3 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b2 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b4 = vlse8_v_i8m2(pB++,bstrideb, l);
            l = vsetvl_e8m2(blkCnt);
            sum3 += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a1, v_b1, l),vwmul_vv_i16m4(v_a2, v_b2, l), l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            sum4 += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a1, v_b3, l),vwmul_vv_i16m4(v_a2, v_b4, l), l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            v_b1 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b3 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b2 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b4 = vlse8_v_i8m2(pB++,bstrideb, l);
            l = vsetvl_e8m2(blkCnt);
            sum += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a3, v_b1, l),vwmul_vv_i16m4(v_a4, v_b2, l), l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            sum2 += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a3, v_b3, l),vwmul_vv_i16m4(v_a4, v_b4, l), l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            v_b1 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b3 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b2 = vlse8_v_i8m2(pB++,bstrideb, l);
            v_b4 = vlse8_v_i8m2(pB++,bstrideb, l);
            l = vsetvl_e8m2(blkCnt);
            sum3 += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a3, v_b1, l),vwmul_vv_i16m4(v_a4, v_b2, l), l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            sum4 += vmv_x_s_i32m1_i32(vredsum_vs_i32m8_i32m1(vtemp, vwadd_vv_i32m8(vwmul_vv_i16m4(v_a3, v_b3, l),vwmul_vv_i16m4(v_a4, v_b4, l), l), vtemp, l));
            pA += (l-1)*bstridea;
            pB += (l-1)*bstrideb;
        }
#else
        while (colCnt)
        {
            q7_t inA1 = *pA++;
            q7_t inA3 = *pA++;
            q7_t inA2 = *pA++;
            q7_t inA4 = *pA++;

            q7_t inB1 = *pB++;
            q7_t inB3 = *pB++;
            q7_t inB2 = *pB++;
            q7_t inB4 = *pB++;

            sum += inA1 * inB1 + inA2 * inB2;
            sum2 += inA1 * inB3 + inA2 * inB4;

            inB1 = *pB++;
            inB3 = *pB++;
            inB2 = *pB++;
            inB4 = *pB++;

            sum3 += inA1 * inB1 + inA2 * inB2;
            sum4 += inA1 * inB3 + inA2 * inB4;

            inB1 = *pB++;
            inB3 = *pB++;
            inB2 = *pB++;
            inB4 = *pB++;

            sum += inA3 * inB1 + inA4 * inB2;
            sum2 += inA3 * inB3 + inA4 * inB4;

            inB1 = *pB++;
            inB3 = *pB++;
            inB2 = *pB++;
            inB4 = *pB++;

            sum3 += inA3 * inB1 + inA4 * inB2;
            sum4 += inA3 * inB3 + inA4 * inB4;

            colCnt--;
        }
#endif
        colCnt = dim_vec & 0x3;
        while (colCnt)
        {
            q7_t inA = *pA++;
            q7_t inB = *pB++;
            sum += inA * inB;
            inB = *pB++;
            sum2 += inA * inB;
            inB = *pB++;
            sum3 += inA * inB;
            inB = *pB++;
            sum4 += inA * inB;

            colCnt--;
        }
        *pO++ = (q7_t)__SSAT((sum >> out_shift), 8);
        *pO++ = (q7_t)__SSAT((sum2 >> out_shift), 8);
        *pO++ = (q7_t)__SSAT((sum3 >> out_shift), 8);
        *pO++ = (q7_t)__SSAT((sum4 >> out_shift), 8);

        rowCnt--;
    }

    rowCnt = num_of_rows & 0x3;

    while (rowCnt)
    {
        int ip_out = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);

        int j;

        pA = pV;
        for (j = 0; j < dim_vec; j++)
        {
            q7_t inA = *pA++;
            q7_t inB = *pB++;
            ip_out += inA * inB;
        }
        *pO++ = (q7_t)__SSAT((ip_out >> out_shift), 8);

        rowCnt--;
    }

#endif /* RISCV_MATH_DSP */

    /* Return to RISCV_MATH_SUCCESS */
    return (RISCV_MATH_SUCCESS);
}

/**
 * @} end of FC group
 */
