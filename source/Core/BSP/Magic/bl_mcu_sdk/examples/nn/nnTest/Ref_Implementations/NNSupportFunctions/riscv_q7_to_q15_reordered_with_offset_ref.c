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
 * Title:        riscv_q7_to_q15_reordered_with_offset.c
 * Description:  Converts the elements of the Q7 vector to a reordered Q15 vector with an added offset. The re-ordering
 *               is a signature of sign extension intrinsic(DSP extension).
 *
 * $Date:        May 29, 2020
 * $Revision:    V.2.0.3
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"
#include "ref_functions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup nndata_convert
 * @{
 */

/**
 * @brief Converts the elements of the Q7 vector to a reordered Q15 vector with an added offset.
 *
 * @note  Refer header file for details.
 *
 */

void riscv_q7_to_q15_reordered_with_offset_ref(const q7_t *src, q15_t *dst, uint32_t block_size, q15_t offset)
{

    (void)src;
    (void)dst;
    (void)block_size;
    (void)offset;
    /* Not available */
}

/**
 * @} end of nndata_convert group
 */
