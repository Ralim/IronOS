/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_logsumexp_f32.c
 * Description:  LogSumExp
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

#include "dsp/statistics_functions.h"
#include <limits.h>
#include <math.h>


/**
 * @addtogroup Entropy
 * @{
 */


/**
 * @brief Entropy
 *
 * @param[in]  pSrcA        Array of input values.
 * @param[in]  blockSize    Number of samples in the input array.
 * @return     Entropy      -Sum(p ln p)
 *
 */

float32_t riscv_entropy_f32(const float32_t * pSrcA,uint32_t blockSize)
{
    const float32_t *pIn;
    uint32_t blkCnt;
    float32_t accum, p;
 
    pIn = pSrcA;
    blkCnt = blockSize;

    accum = 0.0f;

    while(blkCnt > 0)
    {
       p = *pIn++;
       accum += p * logf(p);
       
       blkCnt--;
    
    }

    return(-accum);
}

/**
 * @} end of Entropy group
 */
