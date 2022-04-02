/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_sort_f32.c
 * Description:  Floating point sort
 *
 * $Date:        23 April 2021
 * $Revision:    V1.9.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
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

#include "riscv_sorting.h"

/**
  @ingroup groupSupport
 */

/**
  @addtogroup Sorting
  @{
 */


/**
 * @brief Generic sorting function
 *
 * @param[in]  S          points to an instance of the sorting structure.
 * @param[in]  pSrc       points to the block of input data.
 * @param[out] pDst       points to the block of output data.
 * @param[in]  blockSize  number of samples to process.
 */

void riscv_sort_f32(
  const riscv_sort_instance_f32 * S, 
        float32_t * pSrc, 
        float32_t * pDst, 
        uint32_t blockSize)
{
    switch(S->alg)
    {
        case RISCV_SORT_BITONIC:
        riscv_bitonic_sort_f32(S, pSrc, pDst, blockSize);
        break;

        case RISCV_SORT_BUBBLE:
        riscv_bubble_sort_f32(S, pSrc, pDst, blockSize);
        break;

        case RISCV_SORT_HEAP:
        riscv_heap_sort_f32(S, pSrc, pDst, blockSize);
        break;

        case RISCV_SORT_INSERTION:
        riscv_insertion_sort_f32(S, pSrc, pDst, blockSize);
        break;

        case RISCV_SORT_QUICK:
        riscv_quick_sort_f32(S, pSrc, pDst, blockSize);
        break;

        case RISCV_SORT_SELECTION:
        riscv_selection_sort_f32(S, pSrc, pDst, blockSize);
        break;
    }
}

/**
  @} end of Sorting group
 */
