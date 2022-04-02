/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        SupportFunctions.c
 * Description:  Combination of all support function source files.
 *
 * $Date:        16. March 2020
 * $Revision:    V1.1.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2019-2020 ARM Limited or its affiliates. All rights reserved.
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

#include "riscv_barycenter_f32.c"
#include "riscv_bitonic_sort_f32.c"
#include "riscv_bubble_sort_f32.c"
#include "riscv_copy_f32.c"
#include "riscv_copy_q15.c"
#include "riscv_copy_q31.c"
#include "riscv_copy_q7.c"
#include "riscv_fill_f32.c"
#include "riscv_fill_q15.c"
#include "riscv_fill_q31.c"
#include "riscv_fill_q7.c"
#include "riscv_heap_sort_f32.c"
#include "riscv_insertion_sort_f32.c"
#include "riscv_merge_sort_f32.c"
#include "riscv_merge_sort_init_f32.c"
#include "riscv_quick_sort_f32.c"
#include "riscv_selection_sort_f32.c"
#include "riscv_sort_f32.c"
#include "riscv_sort_init_f32.c"
#include "riscv_weighted_sum_f32.c"

#include "riscv_float_to_q15.c"
#include "riscv_float_to_q31.c"
#include "riscv_float_to_q7.c"
#include "riscv_q15_to_float.c"
#include "riscv_q15_to_q31.c"
#include "riscv_q15_to_q7.c"
#include "riscv_q31_to_float.c"
#include "riscv_q31_to_q15.c"
#include "riscv_q31_to_q7.c"
#include "riscv_q7_to_float.c"
#include "riscv_q7_to_q15.c"
#include "riscv_q7_to_q31.c"
