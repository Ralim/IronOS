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
 * Title:        riscv_elementwise_add_s8
 * Description:  Element wise add
 *
 * $Date:        July 31, 2020
 * $Revision:    V.2.5.1
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "ref_functions.h"
#include "riscv_nnsupportfunctions.h"

/**
 * @note The *_no_sat API does not mean that the input not saturated, Since
 *       __MULT is a positive integer, it is saturated. The API definition
 *       has more info about it.
 */
#define SAT_INPUT(__INPUT, __MULT, __SHIFT)                 \
  __INPUT = riscv_nn_doubling_high_mult_no_sat(__INPUT, __MULT); \
  __INPUT = riscv_nn_divide_by_power_of_two(__INPUT, -__SHIFT);

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup BasicMath
 * @{
 */

/*
 * s8 element wise add
 *
 * Refer header file for details.
 *
 */

/* Note: __SHIFT is expected to be <=0 */


riscv_status
riscv_elementwise_add_s8_ref(const int8_t *input_1_vect,
                           const int8_t *input_2_vect,
                           const int32_t input_1_offset,
                           const int32_t input_1_mult,
                           const int32_t input_1_shift,
                           const int32_t input_2_offset,
                           const int32_t input_2_mult,
                           const int32_t input_2_shift,
                           const int32_t left_shift,
                           int8_t *output,
                           const int32_t out_offset,
                           const int32_t out_mult,
                           const int32_t out_shift,
                           const int32_t out_activation_min,
                           const int32_t out_activation_max,
                           const uint32_t block_size)
{
  uint32_t loop_count;
  int32_t input_1;
  int32_t input_2;
  int32_t sum;

  loop_count = block_size;

  while (loop_count > 0U)
  {
    /* C = A + B */

    input_1 = (*input_1_vect++ + input_1_offset) << left_shift;
    input_2 = (*input_2_vect++ + input_2_offset) << left_shift;

    input_1 = riscv_nn_doubling_high_mult(input_1, input_1_mult);
    input_1 = riscv_nn_divide_by_power_of_two(input_1, -input_1_shift);

    input_2 = riscv_nn_doubling_high_mult(input_2, input_2_mult);
    input_2 = riscv_nn_divide_by_power_of_two(input_2, -input_2_shift);

    sum = input_1 + input_2;
    SAT_INPUT(sum, out_mult, out_shift);
    sum += out_offset;

    sum = MAX(sum, out_activation_min);
    sum = MIN(sum, out_activation_max);

    *output++ = (q7_t)sum;

    /* Decrement loop counter */
    loop_count--;
  }

  return (RISCV_MATH_SUCCESS);
}

/**
 * @} end of BasicMath group
 */
