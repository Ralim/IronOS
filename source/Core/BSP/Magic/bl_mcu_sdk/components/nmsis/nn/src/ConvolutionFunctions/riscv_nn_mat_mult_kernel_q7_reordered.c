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
 * Title:        riscv_nn_mat_mult_kernel_q7_q15_reordered.c
 * Description:  Matrix-multiplication function for convolution with reordered columns
 *
 * $Date:        17. January 2018
 * $Revision:    V.1.0.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#include "riscv_nnfunctions.h"
#include "riscv_nnsupportfunctions.h"

  /**
   * @brief Matrix-multiplication function for convolution with reordered columns
   * @param[in]       pA          pointer to operand A
   * @param[in]       pInBuffer   pointer to operand B, always conssists of 2 vectors
   * @param[in]       ch_im_out   numRow of A
   * @param[in]       numCol_A    numCol of A
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in]       bias        the bias
   * @param[in,out]   pOut        pointer to output
   * @return     The function returns the incremented output pointer
   *
   * @details
   *
   * This function assumes that data in pInBuffer are reordered
   */

q7_t     *riscv_nn_mat_mult_kernel_q7_reordered(const q7_t * pA,
                                                  const q7_t * pInBuffer,
                                                  const uint16_t ch_im_out,
                                                  const uint16_t numCol_A,
                                                  const uint16_t bias_shift,
                                                  const uint16_t out_shift, 
                                                  const q7_t * bias, 
                                                  q7_t * pOut)
{


#if defined (RISCV_MATH_DSP) || defined(RISCV_VECTOR)
    /* set up the second output pointers */
    q7_t     *pOut2 = pOut + ch_im_out;
    int       i;

    /* this loop over rows in A */
    for (i = 0; i < ch_im_out; i += 2)
    {
        /* setup pointers for B */
        const q7_t *pB = pInBuffer;
        const q7_t *pB2 = pB + numCol_A;

        /* align the second pointer for A */
        const q7_t *pA2 = pA + numCol_A;

#if defined(RISCV_VECTOR) && !defined (RISCV_MATH_DSP)
        /* accumulate over the vector */
        size_t l;
        uint32_t blkCnt = numCol_A;
        q31_t sum,sum2,sum3,sum4;
        // q7_t temp[] = {0};
        l = vsetvl_e8m1(blkCnt);
        vint8m1_t va1m1,va2m1,vb1m1,vb2m1;
        vint16m2_t vch00m2,vch01m2,vch10m2,vch11m2;
        vint64m1_t vch00m1,vch01m1,vch10m1,vch11m1;
        vint64m1_t vtemp00m1,vtemp01m1,vtemp10m1,vtemp11m1;
        vint32m4_t vch00m4,vch01m4,vch10m4,vch11m4;
        //initial array and temp sum to zero
        vch00m4 = vmv_v_x_i32m4(0, l);
        vch01m4 = vmv_v_x_i32m4(0, l);
        vch10m4 = vmv_v_x_i32m4(0, l);
        vch11m4 = vmv_v_x_i32m4(0, l);
        vtemp00m1 = vmv_v_x_i64m1(0, l);
        for (; (l = vsetvl_e8m1(blkCnt)) > 0; blkCnt -= l) {
            va1m1 = vle8_v_i8m1(pA , l);
            va2m1 = vle8_v_i8m1(pA2, l);
            vb1m1 = vle8_v_i8m1(pB , l);
            vb2m1 = vle8_v_i8m1(pB2, l);
            pA  += l;
            pA2 += l;
            pB  += l;
            pB2 += l;
            vch00m2= vwmul_vv_i16m2(va1m1, vb1m1, l);
            vch01m2= vwmul_vv_i16m2(va1m1, vb2m1, l);
            vch10m2= vwmul_vv_i16m2(va2m1, vb1m1, l);
            vch11m2= vwmul_vv_i16m2(va2m1, vb2m1, l);
            vch00m4 = vwadd_wv_i32m4(vch00m4, vch00m2, l);
            vch01m4 = vwadd_wv_i32m4(vch01m4, vch01m2, l);
            vch10m4 = vwadd_wv_i32m4(vch10m4, vch10m2, l);
            vch11m4 = vwadd_wv_i32m4(vch11m4, vch11m2, l);
        }
        //set vl to max vl
        l = vsetvl_e8m1(numCol_A);
            vch00m1 = vwredsum_vs_i32m4_i64m1(vtemp00m1, vch00m4, vtemp00m1, l);
            vch01m1 = vwredsum_vs_i32m4_i64m1(vtemp00m1, vch01m4, vtemp00m1, l);
            vch10m1 = vwredsum_vs_i32m4_i64m1(vtemp00m1, vch10m4, vtemp00m1, l);
            vch11m1 = vwredsum_vs_i32m4_i64m1(vtemp00m1, vch11m4, vtemp00m1, l);
        //Here we calculate sum of four vector
        //write result scalar back
        sum  = (q31_t)vmv_x_s_i64m1_i64(vch00m1);
        sum2 = (q31_t)vmv_x_s_i64m1_i64(vch01m1);
        sum3 = (q31_t)vmv_x_s_i64m1_i64(vch10m1);
        sum4 = (q31_t)vmv_x_s_i64m1_i64(vch11m1);
        /* init the sum with bias */
        sum  += ((q31_t)(bias[i]) << bias_shift) + NN_ROUND(out_shift);
        sum2 += ((q31_t)(bias[i]) << bias_shift) + NN_ROUND(out_shift);
        sum3 += ((q31_t)(bias[i + 1]) << bias_shift) + NN_ROUND(out_shift);
        sum4 += ((q31_t)(bias[i + 1]) << bias_shift) + NN_ROUND(out_shift);
#else
        /* init the sum with bias */
        q31_t     sum =  ((q31_t)(bias[i]) << bias_shift) + NN_ROUND(out_shift);
        q31_t     sum2 = ((q31_t)(bias[i]) << bias_shift) + NN_ROUND(out_shift);
        q31_t     sum3 = ((q31_t)(bias[i + 1]) << bias_shift) + NN_ROUND(out_shift);
        q31_t     sum4 = ((q31_t)(bias[i + 1]) << bias_shift) + NN_ROUND(out_shift);
#if __RISCV_XLEN == 64
        uint16_t  colCnt = numCol_A >> 3;
        q63_t sum64 = 0, sum642 = 0, sum643 = 0, sum644 = 0;
        /* accumulate over the vector */
        while (colCnt)
        {
           
            q63_t     inB1 = *__SIMD64(pB)++;
            q63_t     inB2 = *__SIMD64(pB2)++;
            q63_t     inA1 = *__SIMD64(pA)++;
            q63_t     inA2 = *__SIMD64(pA2)++;

            sum64  = __RV_SMAQA(sum64 , inA1, inB1);
            sum642 = __RV_SMAQA(sum642, inA1, inB2);
            sum643 = __RV_SMAQA(sum643, inA2, inB1);
            sum644 = __RV_SMAQA(sum644, inA2, inB2);

            colCnt--;
        }                       /* while over colCnt */
        sum  = sum  + (q31_t)(sum64  & 0xFFFFFFFF) + (q31_t)((sum64  & 0xFFFFFFFF00000000)>>32);
        sum2 = sum2 + (q31_t)(sum642 & 0xFFFFFFFF) + (q31_t)((sum642 & 0xFFFFFFFF00000000)>>32);
        sum3 = sum3 + (q31_t)(sum643 & 0xFFFFFFFF) + (q31_t)((sum643 & 0xFFFFFFFF00000000)>>32);
        sum4 = sum4 + (q31_t)(sum644 & 0xFFFFFFFF) + (q31_t)((sum644 & 0xFFFFFFFF00000000)>>32);

        colCnt = numCol_A & 0x7;
#else
        uint16_t  colCnt = numCol_A >> 2;
        /* accumulate over the vector */
        while (colCnt)
        {
            /*
            q31_t     inA11, inA12, inA21, inA22;
            q31_t     inB1 = *__SIMD32(pB)++;
            q31_t     inB2 = *__SIMD32(pB2)++;

            pA = (q7_t *) read_and_pad_reordered((void *)pA, &inA11, &inA12);
            pA2 = (q7_t *) read_and_pad_reordered((void *)pA2, &inA21, &inA22);

            sum = __SMLAD(inA11, inB1, sum);
            sum2 = __SMLAD(inA11, inB2, sum2);
            sum3 = __SMLAD(inA21, inB1, sum3);
            sum4 = __SMLAD(inA21, inB2, sum4);

            inB1 = *__SIMD32(pB)++;
            inB2 = *__SIMD32(pB2)++;

            sum = __SMLAD(inA12, inB1, sum);
            sum2 = __SMLAD(inA12, inB2, sum2);
            sum3 = __SMLAD(inA22, inB1, sum3);
            sum4 = __SMLAD(inA22, inB2, sum4);
            */
           
            q31_t     inB1 = *__SIMD32(pB)++;
            q31_t     inB2 = *__SIMD32(pB2)++;

            q31_t     inA1 = *__SIMD32(pA)++;
            q31_t     inA2 = *__SIMD32(pA2)++;

            sum  = __RV_SMAQA(sum , inA1, inB1);
            sum2 = __RV_SMAQA(sum2, inA1, inB2);
            sum3 = __RV_SMAQA(sum3, inA2, inB1);
            sum4 = __RV_SMAQA(sum4, inA2, inB2);

            colCnt--;
        }                       /* while over colCnt */
        colCnt = numCol_A & 0x3;
#endif /* __RISCV_XLEN == 64 */
        while (colCnt)
        {
            q7_t      inA1 = *pA++;
            q7_t      inB1 = *pB++;
            q7_t      inA2 = *pA2++;
            q7_t      inB2 = *pB2++;

            sum += inA1 * inB1;
            sum2 += inA1 * inB2;
            sum3 += inA2 * inB1;
            sum4 += inA2 * inB2;
            colCnt--;
        }                       /* while over colCnt */
#endif /* defined(RISCV_VECTOR) */
        *pOut++ = (q7_t) __SSAT((sum >> out_shift), 8);
        *pOut++ = (q7_t) __SSAT((sum3 >> out_shift), 8);
        *pOut2++ = (q7_t) __SSAT((sum2 >> out_shift), 8);
        *pOut2++ = (q7_t) __SSAT((sum4 >> out_shift), 8);
        /* skip the row computed with A2 */
        pA += numCol_A;
    }                           /* for over ch_im_out */

    pOut += ch_im_out;

    /* return the new output pointer with offset */
    return pOut;
#else
    /* To be completed */
    return NULL;
#endif                          /* RISCV_MATH_DSP */
}
