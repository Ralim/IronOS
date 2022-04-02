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
 * Title:        riscv_nn_mat_mult_kernel_s8.c
 * Description:  Matrix-multiplication function for convolution
 *
 * $Date:        May 29, 2020
 * $Revision:    V.1.0.2
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"
#include "riscv_nnfunctions.h"

/*
   * Matrix-multiplication function for convolution with per-channel requantization.
   *
   * Refer header file for details.
   *
   */

q7_t *riscv_nn_mat_mult_kernel_s8(const q7_t *input_a,
                                const q7_t *input_b,
                                const uint16_t output_ch,
                                const int32_t *out_shift,
                                const int32_t *out_mult,
                                const int32_t out_offset,
                                const int16_t activation_min,
                                const int16_t activation_max,
                                const uint16_t num_col_a,
                                const int32_t *const output_bias,
                                q7_t *out_0)
{
#if defined(RISCV_VECTOR)
    /* set up the second output pointers */
    q7_t *out_1 = out_0 + output_ch;
    const int32_t *bias = output_bias;

    size_t l;
    uint32_t blkCnt;
    vint8m2_t va1m2,va2m2,vb1m2,vb2m2;
    vint32m1_t vch00m1,vch01m1,vch10m1,vch11m1;
    vint32m1_t vtemp;
    vint32m8_t vch00m8,vch01m8,vch10m8,vch11m8;
    // q31_t ch_0_out_0,ch_0_out_1;
    l = vsetvl_e32m1(1);
    vtemp = vsub_vv_i32m1(vtemp,vtemp, l);

    uint16_t row_count = output_ch/2;
    const q7_t *ip_a0 = input_a;
    /* compute the last odd numbered row if any */
    while (row_count)
    {
        /* setup pointers for B */
        const q7_t *ip_b0 = input_b;
        const q7_t *ip_b1 = ip_b0 + num_col_a;
        const q7_t *ip_a1 = ip_a0 + num_col_a;

        /* load the bias */
        q31_t ch_0_out_0 = *bias;
        q31_t ch_0_out_1 = *bias++;
        q31_t ch_1_out_0 = *bias;
        q31_t ch_1_out_1 = *bias++;

        uint16_t col_count = num_col_a;
        blkCnt = col_count;
        vch00m8 = vand_vx_i32m8(vch00m8, 0, l);
        vch01m8 = vand_vx_i32m8(vch01m8, 0, l);
        vch10m8 = vand_vx_i32m8(vch10m8, 0, l);
        vch11m8 = vand_vx_i32m8(vch11m8, 0, l);
        for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
            va1m2 = vle8_v_i8m2(ip_a0, l);
            va2m2 = vle8_v_i8m2(ip_a1, l);
            vb1m2 = vle8_v_i8m2(ip_b0, l);
            vb2m2 = vle8_v_i8m2(ip_b1, l);
            ip_a0 += l;
            ip_a1 += l;
            ip_b0 += l;
            ip_b1 += l;
            l = vsetvl_e8m2(blkCnt);
            ch_0_out_0 += vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(vtemp, vwmul_vv_i16m4(va1m2, vb1m2, l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            ch_0_out_1 += vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(vtemp, vwmul_vv_i16m4(va1m2, vb2m2, l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            ch_1_out_0 += vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(vtemp, vwmul_vv_i16m4(va2m2, vb1m2, l), vtemp, l));
            l = vsetvl_e8m2(blkCnt);
            ch_1_out_1 += vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(vtemp, vwmul_vv_i16m4(va2m2, vb2m2, l), vtemp, l));
        }

        ch_0_out_0 = riscv_nn_requantize(ch_0_out_0, *out_mult, *out_shift);
        ch_0_out_0 += out_offset;
        ch_0_out_0 = MAX(ch_0_out_0, activation_min);
        ch_0_out_0 = MIN(ch_0_out_0, activation_max);
        *out_0++ = (q7_t)ch_0_out_0;

        ch_0_out_1 = riscv_nn_requantize(ch_0_out_1, *out_mult, *out_shift);
        ch_0_out_1 += out_offset;
        ch_0_out_1 = MAX(ch_0_out_1, activation_min);
        ch_0_out_1 = MIN(ch_0_out_1, activation_max);
        *out_1++ = (q7_t)ch_0_out_1;
        out_mult++;
        out_shift++;

        ch_1_out_0 = riscv_nn_requantize(ch_1_out_0, *out_mult, *out_shift);
        ch_1_out_0 += out_offset;
        ch_1_out_0 = MAX(ch_1_out_0, activation_min);
        ch_1_out_0 = MIN(ch_1_out_0, activation_max);
        *out_0++ = (q7_t)ch_1_out_0;

        ch_1_out_1 = riscv_nn_requantize(ch_1_out_1, *out_mult, *out_shift);
        ch_1_out_1 += out_offset;
        ch_1_out_1 = MAX(ch_1_out_1, activation_min);
        ch_1_out_1 = MIN(ch_1_out_1, activation_max);
        *out_1++ = (q7_t)ch_1_out_1;
        out_mult++;
        out_shift++;

        row_count--;
        ip_a0 += num_col_a;
    }
    if (output_ch & 0x1)
    {
        const q7_t *ip_b0 = input_b;
        const q7_t *ip_b1 = ip_b0 + num_col_a;

        /* load the bias */
        q31_t ch_0_out_0 = *bias;
        q31_t ch_0_out_1 = *bias++;

        uint16_t col_count = num_col_a;
        // while (col_count)
        blkCnt = col_count;
        l = vsetvl_e8m2(blkCnt);
        vch00m8 = vand_vx_i32m8(vch00m8, 0, l);
        vch01m8 = vand_vx_i32m8(vch01m8, 0, l);
        for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
            va1m2 = vle8_v_i8m2(ip_a0, l);
            vb1m2 = vle8_v_i8m2(ip_b0, l);
            vb2m2 = vle8_v_i8m2(ip_b1, l);
            ip_a0 += l;
            ip_b0 += l;
            ip_b1 += l;
            vch00m8 = vwadd_wv_i32m8(vch00m8, vwmul_vv_i16m4(va1m2, vb1m2, l), l);
            vch01m8 = vwadd_wv_i32m8(vch01m8, vwmul_vv_i16m4(va1m2, vb2m2, l), l);
        }
        l = vsetvl_e32m8(col_count);
        vch00m1 = vredsum_vs_i32m8_i32m1(vtemp, vch00m8, vtemp, l);
        vch01m1 = vredsum_vs_i32m8_i32m1(vtemp, vch01m8, vtemp, l);

        l = vsetvl_e32m1(1);
        ch_0_out_0 += vmv_x_s_i32m1_i32(vch00m1);
        ch_0_out_1 += vmv_x_s_i32m1_i32(vch01m1);
        // printf("ch_0_out_0 is %d\n",ch_0_out_0);

        ch_0_out_0 = riscv_nn_requantize(ch_0_out_0, *out_mult, *out_shift);
        ch_0_out_0 += out_offset;
        ch_0_out_0 = MAX(ch_0_out_0, activation_min);
        ch_0_out_0 = MIN(ch_0_out_0, activation_max);
        *out_0++ = (q7_t)ch_0_out_0;

        ch_0_out_1 = riscv_nn_requantize(ch_0_out_1, *out_mult, *out_shift);
        ch_0_out_1 += out_offset;
        ch_0_out_1 = MAX(ch_0_out_1, activation_min);
        ch_0_out_1 = MIN(ch_0_out_1, activation_max);
        *out_1++ = (q7_t)ch_0_out_1;
        out_mult++;
        out_shift++;
    }

    out_0 += output_ch;

    /* return the new output pointer with offset */
    return out_0;


#else
#if   defined(RISCV_MATH_DSP)
   /* set up the second output pointers */
    q7_t *out_1 = out_0 + output_ch;
    const int32_t *bias = output_bias;

    uint16_t row_count = output_ch / 2;
    const q7_t *ip_a0 = input_a;
    /* this loop over rows in A */
    while (row_count)
    {
        /* setup pointers for B */
        const q7_t *ip_b0 = input_b;
        const q7_t *ip_b1 = ip_b0 + num_col_a;

        /* align the second pointer for A */
        const q7_t *ip_a1 = ip_a0 + num_col_a;

        /* Init accumulator with bias for channel N and N + 1 */
        q31_t ch_0_out_0 = *bias;
        q31_t ch_0_out_1 = *bias++;
        q31_t ch_1_out_0 = *bias;
        q31_t ch_1_out_1 = *bias++;

        uint16_t col_count = num_col_a / 4;
        /* accumulate over the vector */
        while (col_count)
        {
            q31_t     inB1 = *__SIMD32(ip_b0)++;
            q31_t     inB2 = *__SIMD32(ip_b1)++;

            q31_t     inA1 = *__SIMD32(ip_a0)++;
            q31_t     inA2 = *__SIMD32(ip_a1)++;

            ch_0_out_0 = __RV_SMAQA(ch_0_out_0, inA1, inB1);
            ch_0_out_1 = __RV_SMAQA(ch_0_out_1, inA1, inB2);
            ch_1_out_0 = __RV_SMAQA(ch_1_out_0, inA2, inB1);
            ch_1_out_1 = __RV_SMAQA(ch_1_out_1, inA2, inB2);

            // q31_t a01, a02, a11, a12;
            // q31_t b0 = riscv_nn_read_q15x2_ia(&ip_b0);
            // q31_t b1 = riscv_nn_read_q15x2_ia(&ip_b1);

            // ip_a0 = read_and_pad(ip_a0, &a01, &a02);
            // ip_a1 = read_and_pad(ip_a1, &a11, &a12);

            // ch_0_out_0 = __SMLAD(a01, b0, ch_0_out_0);
            // ch_0_out_1 = __SMLAD(a01, b1, ch_0_out_1);
            // ch_1_out_0 = __SMLAD(a11, b0, ch_1_out_0);
            // ch_1_out_1 = __SMLAD(a11, b1, ch_1_out_1);

            // b0 = riscv_nn_read_q15x2_ia(&ip_b0);
            // b1 = riscv_nn_read_q15x2_ia(&ip_b1);

            // ch_0_out_0 = __SMLAD(a02, b0, ch_0_out_0);
            // ch_0_out_1 = __SMLAD(a02, b1, ch_0_out_1);
            // ch_1_out_0 = __SMLAD(a12, b0, ch_1_out_0);
            // ch_1_out_1 = __SMLAD(a12, b1, ch_1_out_1);

            col_count--;
        } /* while over col_count */
        col_count = num_col_a & 0x3;
        while (col_count)
        {
            q7_t a0 = *ip_a0++;
            q7_t b0 = *ip_b0++;
            q7_t a1 = *ip_a1++;
            q7_t b1 = *ip_b1++;

            ch_0_out_0 += a0 * b0;
            ch_0_out_1 += a0 * b1;
            ch_1_out_0 += a1 * b0;
            ch_1_out_1 += a1 * b1;
            col_count--;
        } /* while over col_count */

        ch_0_out_0 = riscv_nn_requantize(ch_0_out_0, *out_mult, *out_shift);
        ch_0_out_0 += out_offset;
        ch_0_out_0 = MAX(ch_0_out_0, activation_min);
        ch_0_out_0 = MIN(ch_0_out_0, activation_max);
        *out_0++ = (q7_t)ch_0_out_0;

        ch_0_out_1 = riscv_nn_requantize(ch_0_out_1, *out_mult, *out_shift);
        ch_0_out_1 += out_offset;
        ch_0_out_1 = MAX(ch_0_out_1, activation_min);
        ch_0_out_1 = MIN(ch_0_out_1, activation_max);
        *out_1++ = (q7_t)ch_0_out_1;
        out_mult++;
        out_shift++;
        ch_1_out_0 = riscv_nn_requantize(ch_1_out_0, *out_mult, *out_shift);
        ch_1_out_0 += out_offset;
        ch_1_out_0 = MAX(ch_1_out_0, activation_min);
        ch_1_out_0 = MIN(ch_1_out_0, activation_max);
        *out_0++ = (q7_t)ch_1_out_0;

        ch_1_out_1 = riscv_nn_requantize(ch_1_out_1, *out_mult, *out_shift);
        ch_1_out_1 += out_offset;
        ch_1_out_1 = MAX(ch_1_out_1, activation_min);
        ch_1_out_1 = MIN(ch_1_out_1, activation_max);
        *out_1++ = (q7_t)ch_1_out_1;
        out_mult++;
        out_shift++;

        /* skip row */
        ip_a0 += num_col_a;
        row_count--;
    }

    /* compute the last odd numbered row if any */
    if (output_ch & 0x1)
    {
        /* setup pointers for B */
        const q7_t *ip_b0 = input_b;
        const q7_t *ip_b1 = ip_b0 + num_col_a;

        /* load the bias */
        q31_t ch_0_out_0 = *bias;
        q31_t ch_0_out_1 = *bias++;

        uint16_t col_count = num_col_a >> 2;
        while (col_count)
        {
            q31_t a01, a02;
            q31_t b0 = riscv_nn_read_q7x4_ia(&ip_b0);
            q31_t b1 = riscv_nn_read_q7x4_ia(&ip_b1);
            a01 = riscv_nn_read_q7x4_ia(&ip_a0);
            // ip_a0 = read_and_pad(ip_a0, &a01, &a02);

            ch_0_out_0 = __RV_SMAQA(ch_0_out_0, a01, b0);
            ch_0_out_1 = __RV_SMAQA(ch_0_out_1, a01, b1);

            // b0 = riscv_nn_read_q15x2_ia(&ip_b0);
            // b1 = riscv_nn_read_q15x2_ia(&ip_b1);
            // ch_0_out_0 = __SMLAD(a02, b0, ch_0_out_0);
            // ch_0_out_1 = __SMLAD(a02, b1, ch_0_out_1);

            col_count--;
        }
        col_count = num_col_a & 0x3;
        while (col_count)
        {
            q7_t a0 = *ip_a0++;
            q7_t b0 = *ip_b0++;
            q7_t b1 = *ip_b1++;

            ch_0_out_0 += a0 * b0;
            ch_0_out_1 += a0 * b1;
            col_count--;
        }
        ch_0_out_0 = riscv_nn_requantize(ch_0_out_0, *out_mult, *out_shift);
        ch_0_out_0 += out_offset;
        ch_0_out_0 = MAX(ch_0_out_0, activation_min);
        ch_0_out_0 = MIN(ch_0_out_0, activation_max);
        *out_0++ = (q7_t)ch_0_out_0;

        ch_0_out_1 = riscv_nn_requantize(ch_0_out_1, *out_mult, *out_shift);
        ch_0_out_1 += out_offset;
        ch_0_out_1 = MAX(ch_0_out_1, activation_min);
        ch_0_out_1 = MIN(ch_0_out_1, activation_max);
        *out_1++ = (q7_t)ch_0_out_1;
        out_mult++;
        out_shift++;
    }

    out_0 += output_ch;

    /* return the new output pointer with offset */
    return out_0;
#else
    (void)input_a;
    (void)input_b;
    (void)output_ch;
    (void)out_shift;
    (void)out_mult;
    (void)out_offset;
    (void)activation_min;
    (void)activation_max;
    (void)num_col_a;
    (void)output_bias;
    (void)out_0;
    /* To be completed */
    return NULL;
#endif
#endif  /* defined(RISCV_VECTOR) */
}
