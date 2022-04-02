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
 * Title:        riscv_fully_connected_q7.c
 * Description:  Q7 basic fully-connected layer function
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
 * @addtogroup FC
 * @{
 */

/**
 * @brief Q7 basic fully-connected layer function
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
 * This basic function is designed to work with regular weight
 * matrix without interleaving.
 *
 */

riscv_status riscv_fully_connected_q7(const q7_t *pV,
                                  const q7_t *pM,
                                  const uint16_t dim_vec,
                                  const uint16_t num_of_rows,
                                  const uint16_t bias_shift,
                                  const uint16_t out_shift,
                                  const q7_t *bias,
                                  q7_t *pOut,
                                  q15_t *vec_buffer)
{

#if defined (RISCV_MATH_DSP) || defined (RISCV_VECTOR)
    /* Run the following code for RISC-V Core with DSP enabled */

    const q7_t *pB = pM;
    const q7_t *pB2;
    q7_t *pO = pOut;
    const q7_t *pBias = bias;
    q7_t    *pA;
    uint16_t  rowCnt = num_of_rows >> 1;

    /* expand the vector into the buffer */
    riscv_q7_to_q7_reordered_no_shift(pV, (q7_t *)vec_buffer, dim_vec);

    while (rowCnt)
    {
        pA = (q7_t *)vec_buffer;
        pB2 = pB + dim_vec;
#if defined (RISCV_VECTOR)
        /* accumulate over the vector */
        size_t l;
        uint32_t blkCnt = dim_vec;
        q31_t sum,sum2;
        // q7_t temp[] = {0};
        l = vsetvl_e8m1(blkCnt);
        vint8m1_t va1m1,vb1m1,vb2m1;
        vint16m2_t vch00m2,vch01m2;
        vint64m1_t vch00m1,vch01m1;
        vint64m1_t vtemp00m1,vtemp01m1;
        vint32m4_t vch00m4,vch01m4;
        //initial array and temp sum to zero
        vch00m4 = vmv_v_x_i32m4(0, l);
        vch01m4 = vmv_v_x_i32m4(0, l);
        vch00m1 = vmv_v_x_i64m1(0, l);
        vch01m1 = vmv_v_x_i64m1(0, l);
        vtemp00m1 = vmv_v_x_i64m1(0, l);
        vtemp01m1 = vmv_v_x_i64m1(0, l);
        for (; (l = vsetvl_e8m1(blkCnt)) > 0; blkCnt -= l) {
            va1m1 = vle8_v_i8m1(pA , l);
            vb1m1 = vle8_v_i8m1(pB , l);
            vb2m1 = vle8_v_i8m1(pB2, l);
            pA  += l;
            pB  += l;
            pB2 += l;
            vch00m2= vwmul_vv_i16m2(va1m1, vb1m1, l);
            vch01m2= vwmul_vv_i16m2(va1m1, vb2m1, l);
            vch00m4 = vwadd_wv_i32m4(vch00m4, vch00m2, l);
            vch01m4 = vwadd_wv_i32m4(vch01m4, vch01m2, l);
        }
        //set vl to max vl
        l = vsetvl_e8m1(dim_vec);
        //calculate sum
        vch00m1 = vwredsum_vs_i32m4_i64m1(vtemp00m1, vch00m4, vtemp00m1, l);
        vch01m1 = vwredsum_vs_i32m4_i64m1(vtemp01m1, vch01m4, vtemp01m1, l);
        //set vl to 1
        l = vsetvl_e32m1(1);
        vch00m1 = vmin_vx_i64m1(vmax_vx_i64m1(vch00m1, 0xffffffff80000000ULL, l), 0x000000007fffffffULL, l);
        vch01m1 = vmin_vx_i64m1(vmax_vx_i64m1(vch01m1, 0xffffffff80000000ULL, l), 0x000000007fffffffULL, l);
        //Here we calculate sum of four vector
        //write result scalar back
        sum  = (q31_t)vmv_x_s_i64m1_i64(vch00m1);
        sum2 = (q31_t)vmv_x_s_i64m1_i64(vch01m1);
        /* init the sum with bias */
        sum  += ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        sum2 += ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
#else
        q31_t     sum =  ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
        q31_t     sum2 = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
#if __RISCV_XLEN == 64
        uint16_t  colCnt = dim_vec >> 3;
        q63_t sum64 = 0;
        q63_t sum642 = 0;
        while (colCnt)
        {
            q63_t     inB1 = *__SIMD64(pB)++;
            q63_t     inB2 = *__SIMD64(pB2)++;
            q63_t     inA1 = *__SIMD64(pA)++;

            sum64  = __RV_SMAQA(sum64 , inA1, inB1);
            sum642 = __RV_SMAQA(sum642, inA1, inB2);

            colCnt--;
        }
        sum = sum + (q31_t)(sum64 & 0xFFFFFFFF) + (q31_t)((sum64 & 0xFFFFFFFF00000000)>>32);
        sum2 = sum2 + (q31_t)(sum642 & 0xFFFFFFFF) + (q31_t)((sum642 & 0xFFFFFFFF00000000)>>32);
        colCnt = dim_vec & 0x7;
#else
        uint16_t  colCnt = dim_vec >> 2;
        while (colCnt)
        {
            /*
            q31_t     inV, inM11, inM12, inM21, inM22;
            pB = read_and_pad_reordered(pB, &inM11, &inM12);
            pB2 = read_and_pad_reordered(pB2, &inM21, &inM22);

            inV = riscv_nn_read_q15x2_ia(&pA);

            sum = __SMLAD(inV, inM11, sum);
            sum2 = __SMLAD(inV, inM21, sum2);

            inV = riscv_nn_read_q15x2_ia(&pA);

            sum = __SMLAD(inV, inM12, sum);
            sum2 = __SMLAD(inV, inM22, sum2);
            */
            q31_t     inB1 = *__SIMD32(pB)++;
            q31_t     inB2 = *__SIMD32(pB2)++;

            q31_t     inA1 = *__SIMD32(pA)++;

            sum  = __RV_SMAQA(sum , inA1, inB1);
            sum2 = __RV_SMAQA(sum2, inA1, inB2);

            colCnt--;
        }
        colCnt = dim_vec & 0x3;
#endif /* __RISCV_XLEN == 64 */
        while (colCnt)
        {
            q7_t inV = *pA++;
            q15_t inM = *pB++;
            q15_t inM2 = *pB2++;

            sum += inV * inM;
            sum2 += inV * inM2;
            colCnt--;
        }                       /* while over colCnt */
#endif /* defined (RISCV_VECTOR) */
        *pO++ = (q7_t) (__SSAT((sum >> out_shift), 8));
        *pO++ = (q7_t) (__SSAT((sum2 >> out_shift), 8));

        /* adjust the pointers and counters */
        pB += dim_vec;
        rowCnt--;
    }

    /* left-over part of the rows */
    rowCnt = num_of_rows & 0x1;

    while (rowCnt)
    {
        uint16_t  colCnt = dim_vec >> 2;

        pA = (q7_t *)vec_buffer;
#if defined (RISCV_VECTOR)
        /* accumulate over the vector */
        size_t l;
        uint32_t blkCnt = dim_vec;
        q31_t sum;
        // q7_t temp[] = {0};
        l = vsetvl_e8m1(blkCnt);
        vint8m1_t va1m1,vb1m1;
        vint16m2_t vch00m2;
        vint64m1_t vch00m1;
        vint64m1_t vtemp00m1;
        vint32m4_t vch00m4;
        //initial array and temp sum to zero
        vch00m4 = vmv_v_x_i32m4(0, l);
        vch00m1 = vmv_v_x_i64m1(0, l);
        vtemp00m1 = vmv_v_x_i64m1(0, l);
        for (; (l = vsetvl_e8m1(blkCnt)) > 0; blkCnt -= l) {
            va1m1 = vle8_v_i8m1(pA , l);
            vb1m1 = vle8_v_i8m1(pB , l);
            pA  += l;
            pB  += l;
            pB2 += l;
            vch00m2= vwmul_vv_i16m2(va1m1, vb1m1, l);
            vch00m4 = vwadd_wv_i32m4(vch00m4, vch00m2, l);
        }
        //set vl to max vl
        l = vsetvl_e8m1(dim_vec);
        //calculate sum
        vch00m1 = vwredsum_vs_i32m4_i64m1(vtemp00m1, vch00m4, vtemp00m1, l);
        //set vl to 1
        l = vsetvl_e32m1(1);
        vch00m1 = vmin_vx_i64m1(vmax_vx_i64m1(vch00m1, 0xffffffff80000000ULL, l), 0x000000007fffffffULL, l);
        //Here we calculate sum of four vector
        //write result scalar back
        sum  = (q31_t)vmv_x_s_i64m1_i64(vch00m1);
       /* init the sum with bias */
        sum += ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);
#else
        q31_t     sum = ((q31_t)(*pBias++) << bias_shift) + NN_ROUND(out_shift);

        while (colCnt)
        {
            /*
            q31_t     inV1, inV2, inM11, inM12;

            pB = read_and_pad_reordered(pB, &inM11, &inM12);

            inV1 = riscv_nn_read_q15x2_ia(&pA);
            sum = __SMLAD(inV1, inM11, sum);

            inV2 = riscv_nn_read_q15x2_ia(&pA);
            sum = __SMLAD(inV2, inM12, sum);
            */

            q31_t     inB1 = *__SIMD32(pB)++;
            q31_t     inA1 = *__SIMD32(pA)++;
            sum  = __RV_SMAQA(sum, inA1, inB1);

            colCnt--;
        }

        /* left-over of the vector */
        colCnt = dim_vec & 0x3;
        while (colCnt)
        {
            q7_t inV = *pA++;
            q15_t inM = *pB++;
            sum += inV * inM;
            colCnt--;
        }
#endif /* defined (RISCV_VECTOR) */
        *pO++ = (q7_t) (__SSAT((sum >> out_shift), 8));

        rowCnt--;
    }

#else
    (void)vec_buffer;
    int i, j;

    /* Run the following code as reference implementation for RISC-V Core without DSP */
    for (i = 0; i < num_of_rows; i++)
    {
        int ip_out = ((q31_t)(bias[i]) << bias_shift) + NN_ROUND(out_shift);
        for (j = 0; j < dim_vec; j++)
        {
            ip_out += pV[j] * pM[i * dim_vec + j];
        }
        pOut[i] = (q7_t)__SSAT((ip_out >> out_shift), 8);
    }

#endif /* RISCV_MATH_DSP */

    /* Return to RISCV_MATH_SUCCESS */
    return (RISCV_MATH_SUCCESS);
}

/**
 * @} end of FC group
 */
