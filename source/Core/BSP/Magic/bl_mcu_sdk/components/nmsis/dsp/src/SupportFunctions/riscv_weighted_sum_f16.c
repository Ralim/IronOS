/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_weighted_sum_f16.c
 * Description:  Weighted Sum
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

#include <limits.h>
#include <math.h>

#include "dsp/support_functions_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)

/**
  @ingroup groupSupport
 */

/**
  @defgroup weightedsum Weighted Sum

  Weighted sum of values
 */


/**
 * @addtogroup weightedsum
 * @{
 */


/**
 * @brief Weighted sum
 *
 *
 * @param[in]    *in           Array of input values.
 * @param[in]    *weigths      Weights
 * @param[in]    blockSize     Number of samples in the input array.
 * @return       Weighted sum
 *
 */


float16_t riscv_weighted_sum_f16(const float16_t *in, const float16_t *weigths, uint32_t blockSize)
{

    _Float16 accum1, accum2;
    const float16_t *pIn, *pW;
    uint32_t blkCnt;


    pIn = in;
    pW = weigths;

    accum1=0.0f16;
    accum2=0.0f16;

    blkCnt = blockSize;
    while(blkCnt > 0)
    {
        accum1 += (_Float16)*pIn++ * (_Float16)*pW;
        accum2 += (_Float16)*pW++;
        blkCnt--;
    }

    return(accum1 / accum2);
}

/**
 * @} end of weightedsum group
 */

#endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */ 

