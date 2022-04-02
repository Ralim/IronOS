/*
 * Copyright (C) 2021 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        riscv_nn_vec_mat_mult_t_svdf_s8
 * Description:  s8 vector by matrix (transposed) multiplication with
 *               s16 output. Targetted at SVDF operator.
 *
 * $Date:        15. April 2021
 * $Revision:    V.1.0.0
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup NNBasicMath
 * @{
 */

/*
 * s8 vector(lhs) by matrix (transposed) multiplication
 *
 * Refer header file for details.
 *
 */
riscv_status riscv_nn_vec_mat_mult_t_svdf_s8(const q7_t *lhs,
                                         const q7_t *rhs,
                                         q15_t *dst,
                                         const int32_t lhs_offset,
                                         const int32_t rhs_offset,
                                         const int32_t dst_offset,
                                         const int32_t dst_multiplier,
                                         const int32_t dst_shift,
                                         const int32_t rhs_cols,
                                         const int32_t rhs_rows,
                                         const int32_t activation_min,
                                         const int32_t activation_max)
{
    (void)rhs_offset;
    if (rhs_cols < 0 || (Q31_MAX - rhs_cols) < 16 || dst_offset < 0)
    {
        return RISCV_MATH_ARGUMENT_ERROR;
    }

    (void)rhs_offset;
#if   defined(RISCV_MATH_DSP) && !defined(RISCV_VECTOR)
    int32_t row_loop_cnt = rhs_rows / 2;

    const int16_t lhs_offset_s16 = lhs_offset;
    const int16_t rhs_offset_s16 = rhs_offset;

    const uint32_t lhs_offset_s16x2 = __PKHBT(lhs_offset_s16, lhs_offset_s16, 16);
    const uint32_t rhs_offset_s16x2 = __PKHBT(rhs_offset_s16, rhs_offset_s16, 16);
    for (int32_t i = 0; i < row_loop_cnt; i++)
    {
        int32_t acc_0 = 0;
        int32_t acc_1 = 0;

        const int32_t col_loop_cnt = rhs_cols / 4;
        const int8_t *lhs_vec = lhs;
        const int8_t *rhs_0 = rhs;
        const int8_t *rhs_1 = rhs + rhs_cols;
        rhs += 2 * rhs_cols;
        for (int j = col_loop_cnt; j != 0; j--)
        {
            int32_t vec_0 = riscv_nn_read_q7x4_ia(&lhs_vec);
            int32_t vec_1 = __SXTAB16_RORn(lhs_offset_s16x2, (uint32_t)vec_0, 8);
            vec_0 = __SXTAB16(lhs_offset_s16x2, vec_0);
            int32_t ker_0 = riscv_nn_read_q7x4_ia(&rhs_0);
            int32_t ker_1 = __SXTAB16_RORn(rhs_offset_s16x2, (uint32_t)ker_0, 8);
            ker_0 = __SXTAB16(rhs_offset_s16x2, ker_0);
            acc_0 = __SMLAD(ker_1, vec_1, acc_0);
            acc_0 = __SMLAD(ker_0, vec_0, acc_0);
            ker_0 = riscv_nn_read_q7x4_ia(&rhs_1);
            ker_1 = __SXTAB16_RORn(rhs_offset_s16x2, (uint32_t)ker_0, 8);
            ker_0 = __SXTAB16(rhs_offset_s16x2, ker_0);
            acc_1 = __SMLAD(ker_1, vec_1, acc_1);
            acc_1 = __SMLAD(ker_0, vec_0, acc_1);
        }
        for (int k = col_loop_cnt * 4; k < rhs_cols; k++)
        {
            const int32_t lhs_temp = (*lhs_vec + lhs_offset);
            lhs_vec++;
            acc_0 += lhs_temp * (*rhs_0 + rhs_offset);
            rhs_0++;
            acc_1 += lhs_temp * (*rhs_1 + rhs_offset);
            rhs_1++;
        }
        acc_0 = riscv_nn_requantize(acc_0, dst_multiplier, dst_shift);
        acc_1 = riscv_nn_requantize(acc_1, dst_multiplier, dst_shift);

        // Clamp the result
        acc_0 = MAX(acc_0, activation_min);
        acc_0 = MIN(acc_0, activation_max);
        acc_1 = MAX(acc_1, activation_min);
        acc_1 = MIN(acc_1, activation_max);
        *dst = (q15_t)acc_0;
        *(dst + dst_offset) = (q15_t)acc_1;
        dst += 2 * dst_offset;
    }
    if (rhs_rows & 0x1)
    {
        int32_t acc_0 = 0;
        const int32_t col_loop_cnt = rhs_cols / 4;
        const int8_t *lhs_vec = lhs;
        const int8_t *rhs_0 = rhs;
        for (int i = col_loop_cnt; i != 0; i--)
        {
            int32_t vec_0 = riscv_nn_read_q7x4_ia(&lhs_vec);
            int32_t vec_1 = __SXTAB16(lhs_offset_s16x2, __ROR((uint32_t)vec_0, 8));
            vec_0 = __SXTAB16(lhs_offset_s16x2, vec_0);
            int32_t ker_0 = riscv_nn_read_q7x4_ia(&rhs_0);
            int32_t ker_1 = __SXTAB16(rhs_offset_s16x2, __ROR((uint32_t)ker_0, 8));
            ker_0 = __SXTAB16(rhs_offset_s16x2, ker_0);
            acc_0 = __SMLAD(ker_1, vec_1, acc_0);
            acc_0 = __SMLAD(ker_0, vec_0, acc_0);
        }
        for (int j = col_loop_cnt * 4; j < rhs_cols; j++)
        {
            const int32_t lhs_temp = (*lhs_vec + lhs_offset);
            lhs_vec++;
            acc_0 += lhs_temp * (*rhs_0 + rhs_offset);
            rhs_0++;
        }
        acc_0 = riscv_nn_requantize(acc_0, dst_multiplier, dst_shift);

        // Clamp the result
        acc_0 = MAX(acc_0, activation_min);
        acc_0 = MIN(acc_0, activation_max);
        *dst = (q15_t)acc_0;
        dst += dst_offset;
    }

#else

#if defined(RISCV_VECTOR)
    uint32_t blkCnt;                               /* Loop counter */
    size_t l;
    vint8m2_t v_rhs0, v_rhs1, v_rhs2;
    vint8m2_t v_lhs;
    vint16m4_t v_a, v_b, v_c;
    vint32m1_t v_temp;
    const q7_t *pRHS0;
    const q7_t *pRHS1;
    const q7_t *pRHS2;
    const q7_t *pLHS;
    ptrdiff_t bstride;
    l = vsetvl_e32m1(1);
    v_temp = vsub_vv_i32m1(v_temp, v_temp, l);
#endif
    int32_t row_loop_cnt = rhs_rows / 3;

    for (int i_row_loop_cnt = 0; i_row_loop_cnt < row_loop_cnt; i_row_loop_cnt++)
    {
        const q7_t *lhs_ptr = lhs;
        const q7_t *rhs_ptr_0 = &rhs[0];
        const q7_t *rhs_ptr_1 = &rhs[rhs_cols];
        const q7_t *rhs_ptr_2 = &rhs[rhs_cols * 2];
#if defined(RISCV_VECTOR)
        blkCnt = rhs_cols;
        pRHS0 = rhs_ptr_0;
        pRHS1 = rhs_ptr_1;
        pRHS2 = rhs_ptr_2;
        pLHS = lhs_ptr;
        l = vsetvl_e32m8(blkCnt);
        v_a = vsub_vv_i16m4(v_a,v_a, l);
        v_b = vsub_vv_i16m4(v_b,v_b, l);
        v_c = vsub_vv_i16m4(v_c,v_c, l);
        for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
            v_rhs0 = vle8_v_i8m2(pRHS0, l);
            v_rhs1 = vle8_v_i8m2(pRHS1, l);
            v_rhs2 = vle8_v_i8m2(pRHS2, l);
            v_lhs = vle8_v_i8m2(pLHS, l);
            v_lhs = vadd_vx_i8m2(v_lhs,lhs_offset, l);
            v_a = vwmacc_vv_i16m4(v_a,v_lhs,v_rhs0, l);
            v_b = vwmacc_vv_i16m4(v_b,v_lhs,v_rhs1, l);
            v_c = vwmacc_vv_i16m4(v_c,v_lhs,v_rhs2, l);

            pRHS0 += l;
            pRHS1 += l;
            pRHS2 += l;
            pLHS += l;
        }
        l = vsetvl_e8m2(rhs_cols);
        q31_t res00 = vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(v_temp,v_a,v_temp, l));
        l = vsetvl_e8m2(rhs_cols);
        q31_t res01 = vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(v_temp,v_b,v_temp, l));
        l = vsetvl_e8m2(rhs_cols);
        q31_t res02 = vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(v_temp,v_c,v_temp, l));
#else
        q31_t res00 = 0;
        q31_t res01 = 0;
        q31_t res02 = 0;
        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            const q31_t rhs_value0 = (int8_t)*rhs_ptr_0;
            const q31_t rhs_value1 = (int8_t)*rhs_ptr_1;
            const q31_t rhs_value2 = (int8_t)*rhs_ptr_2;
            const q31_t lhs_value = (int8_t)*lhs_ptr + lhs_offset;

            res00 += lhs_value * rhs_value0;
            res01 += lhs_value * rhs_value1;
            res02 += lhs_value * rhs_value2;

            ++rhs_ptr_0;
            ++rhs_ptr_1;
            ++rhs_ptr_2;
            ++lhs_ptr;
        }
#endif
        // Quantize down
        res00 = riscv_nn_requantize(res00, dst_multiplier, dst_shift);
        res01 = riscv_nn_requantize(res01, dst_multiplier, dst_shift);
        res02 = riscv_nn_requantize(res02, dst_multiplier, dst_shift);

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);
        res01 = MAX(res01, activation_min);
        res01 = MIN(res01, activation_max);
        res02 = MAX(res02, activation_min);
        res02 = MIN(res02, activation_max);

        *dst = (q15_t)res00;
        *(dst + dst_offset) = (q15_t)res01;
        *(dst + 2 * dst_offset) = (q15_t)res02;
        dst += 3 * dst_offset;
        rhs += 3 * rhs_cols;
    }

    const int loop_cnt = rhs_rows % 3;

    for (int i_loop_cnt = 0; i_loop_cnt < loop_cnt; i_loop_cnt++)
    {
        const q7_t *lhs_ptr = &lhs[0];
        const q7_t *rhs_ptr = &rhs[0];

        q31_t res00 = 0;

#if defined(RISCV_VECTOR)
        blkCnt = rhs_cols;
        pRHS0 = rhs_ptr;
        pLHS = lhs_ptr;
        l = vsetvl_e32m8(blkCnt);
        v_a = vsub_vv_i16m4(v_a,v_a, l);
        for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
            v_rhs0 = vle8_v_i8m2(pRHS0, l);
            v_lhs = vle8_v_i8m2(pLHS, l);
            v_lhs = vadd_vx_i8m2(v_lhs,lhs_offset, l);
            v_a = vwmacc_vv_i16m4(v_a,v_lhs,v_rhs0, l);

            pRHS0 += l;
            pLHS += l;
        }
        l = vsetvl_e8m2(rhs_cols);
        res00 = vmv_x_s_i32m1_i32(vwredsum_vs_i16m4_i32m1(v_temp,v_a,v_temp, l));
#else
        for (int32_t rhs_cols_idx = 0; rhs_cols_idx < rhs_cols; ++rhs_cols_idx)
        {
            q31_t rhs_value0 = (int8_t)rhs_ptr[0] + rhs_offset;
            q31_t lhs_value = (int8_t)lhs_ptr[0] + lhs_offset;

            res00 += lhs_value * rhs_value0;

            ++rhs_ptr;
            ++lhs_ptr;
        }
#endif

        // Quantize down
        res00 = riscv_nn_requantize(res00, dst_multiplier, dst_shift);

        // Clamp the result
        res00 = MAX(res00, activation_min);
        res00 = MIN(res00, activation_max);

        *dst = (q15_t)res00;
        dst += dst_offset;
        rhs += rhs_cols;
    }
#endif

    return RISCV_MATH_SUCCESS;
}

/**
 * @} end of NNBasicMath group
 */
