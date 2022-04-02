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
 * Title:        riscv_elementwise_mul_s8
 * Description:  Element wise multiplication
 *
 * $Date:        January 26, 2021
 * $Revision:    V.1.0.5
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
 * @addtogroup BasicMath
 * @{
 */

/**
 * @brief s8 element wise multiplication of two vectors
 *
 * @note   Refer header file for details.
 *
 */

riscv_status riscv_elementwise_mul_s8(const int8_t *input_1_vect,
                                  const int8_t *input_2_vect,
                                  const int32_t input_1_offset,
                                  const int32_t input_2_offset,
                                  int8_t *output,
                                  const int32_t out_offset,
                                  const int32_t out_mult,
                                  const int32_t out_shift,
                                  const int32_t out_activation_min,
                                  const int32_t out_activation_max,
                                  const uint32_t block_size)
{

    int32_t loop_count;
#if defined(RISCV_VECTOR) && (__XLEN != 32) && (__FLEN != 32)
    uint32_t blkCnt = block_size;
  size_t l;
  vint32m4_t input_1;
  vint32m4_t input_2;
  // vint64m8_t mul_res64;
  vint32m4_t mul_res32;
  int32_t mul_res;
       
  for (; (l = vsetvl_e8m1(blkCnt)) > 0; blkCnt -= l) {
    input_1 = vadd_vx_i32m4(vwadd_vx_i32m4(vwadd_vx_i16m2(vle8_v_i8m1(input_1_vect, l), 0, l), 0, l), input_1_offset, l);
    input_1_vect += l;
    input_2 = vadd_vx_i32m4(vwadd_vx_i32m4(vwadd_vx_i16m2(vle8_v_i8m1(input_2_vect, l), 0, l), 0, l), input_2_offset, l);
    input_2_vect += l;

    mul_res32 = vmul_vv_i32m4(input_1, input_2, l);
    for(size_t i =0;i<l;i++){
      mul_res = vmv_x_s_i32m4_i32(mul_res32);
      l = vsetvl_e32m4(l);
      mul_res32 = vslide1down_vx_i32m4(mul_res32,0, l);
      mul_res = riscv_nn_requantize(mul_res, out_mult, out_shift) + out_offset;

      mul_res = MAX(mul_res, out_activation_min);
      mul_res = MIN(mul_res, out_activation_max);

      *output++ = (q7_t)mul_res;
    }
    // mul_res32 = vadd_vx_i32m4(vdiv_vx_i32m4(vsmul_vx_i32m4(vmul_vx_i32m4(mul_res32,(1 << LEFT_SHIFT(out_shift)),l), out_mult, l), (1 << out_shift), l), out_offset, l);

    // mul_res32 = vmax_vx_i32m4(mul_res32, out_activation_min, l);
    // mul_res32 = vmin_vx_i32m4(mul_res32, out_activation_max, l);

    // vse8_v_i8m1(output, vnclip_wx_i8m1(vnclip_wx_i16m2(mul_res32, 0, l), 0, l), l);
    // output += l;
  }
#else
  int32_t input_1;
  int32_t input_2;
  int32_t mul_res;

#if defined(RISCV_MATH_DSP)
    int32_t a_1, b_1, a_2, b_2;

    int32_t offset_1_packed, offset_2_packed;

    int8_t r1, r2, r3, r4;

    offset_1_packed = (input_1_offset << 16U) | (input_1_offset & 0x0FFFFL);
    offset_2_packed = (input_2_offset << 16U) | (input_2_offset & 0x0FFFFL);

    loop_count = block_size >> 2;

    while (loop_count > 0)
    {
        /* 4 outputs are calculated in one loop. The order of calculation is follows the order of output sign extension
           intrinsic */
        input_1_vect = read_and_pad_reordered(input_1_vect, &b_1, &a_1);
        input_2_vect = read_and_pad_reordered(input_2_vect, &b_2, &a_2);

        a_1 = __SADD16(a_1, offset_1_packed);
        b_1 = __SADD16(b_1, offset_1_packed);

        a_2 = __SADD16(a_2, offset_2_packed);
        b_2 = __SADD16(b_2, offset_2_packed);

        /* Mul 1 */
        input_1 = (int16_t)(b_1 & 0x0FFFFL);
        input_2 = (int16_t)(b_2 & 0x0FFFFL);

        mul_res = input_1 * input_2;
        mul_res = riscv_nn_requantize(mul_res, out_mult, out_shift) + out_offset;

        mul_res = MAX(mul_res, out_activation_min);
        mul_res = MIN(mul_res, out_activation_max);
        r1 = (q7_t)mul_res;

        /* Mul 3 */
        input_1 = (int16_t)((b_1 >> 16U) & 0x0FFFFL);
        input_2 = (int16_t)((b_2 >> 16U) & 0x0FFFFL);

        mul_res = input_1 * input_2;
        mul_res = riscv_nn_requantize(mul_res, out_mult, out_shift) + out_offset;
        mul_res = MAX(mul_res, out_activation_min);
        mul_res = MIN(mul_res, out_activation_max);
        r3 = (q7_t)mul_res;

        /* Mul 2 */
        input_1 = (int16_t)(a_1 & 0x0FFFFL);
        input_2 = (int16_t)(a_2 & 0x0FFFFL);

        mul_res = input_1 * input_2;
        mul_res = riscv_nn_requantize(mul_res, out_mult, out_shift) + out_offset;
        mul_res = MAX(mul_res, out_activation_min);
        mul_res = MIN(mul_res, out_activation_max);
        r2 = (q7_t)mul_res;

        /* Mul 4 */
        input_1 = (int16_t)((a_1 >> 16U) & 0x0FFFFL);
        input_2 = (int16_t)((a_2 >> 16U) & 0x0FFFFL);

        mul_res = input_1 * input_2;
        mul_res = riscv_nn_requantize(mul_res, out_mult, out_shift) + out_offset;
        mul_res = MAX(mul_res, out_activation_min);
        mul_res = MIN(mul_res, out_activation_max);
        r4 = (q7_t)mul_res;

        write_q7x4_ia(&output, __PACKq7(r1, r2, r3, r4));

        loop_count--;
    }

    loop_count = block_size & 0x3;
#else
    loop_count = block_size;
#endif

    while (loop_count > 0)
    {
        /* C = A * B */

        input_1 = *input_1_vect++ + input_1_offset;
        input_2 = *input_2_vect++ + input_2_offset;

        mul_res = input_1 * input_2;
        mul_res = riscv_nn_requantize(mul_res, out_mult, out_shift) + out_offset;

        mul_res = MAX(mul_res, out_activation_min);
        mul_res = MIN(mul_res, out_activation_max);

        *output++ = (q7_t)mul_res;

        /* Decrement loop counter */
        loop_count--;
    }
#endif
  return RISCV_MATH_SUCCESS;
}

/**
 * @} end of BasicMath group
 */
