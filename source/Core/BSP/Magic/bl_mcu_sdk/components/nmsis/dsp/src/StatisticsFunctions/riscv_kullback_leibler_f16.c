/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_logsumexp_f16.c
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

#include "dsp/statistics_functions_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)

#include <limits.h>
#include <math.h>

/**
  @ingroup groupStats
 */

/**
  @defgroup Kullback-Leibler Kullback-Leibler divergence

  Computes the Kullback-Leibler divergence between two distributions

 */


/**
 * @addtogroup Kullback-Leibler
 * @{
 */


/**
 * @brief Kullback-Leibler
 *
 * Distribution A may contain 0 with Neon version.
 * Result will be right but some exception flags will be set.
 *
 * Distribution B must not contain 0 probability.
 *
 * @param[in]  *pSrcA         points to an array of input values for probaility distribution A.
 * @param[in]  *pSrcB         points to an array of input values for probaility distribution B.
 * @param[in]  blockSize      number of samples in the input array.
 * @return Kullback-Leibler divergence D(A || B)
 *
 */

float16_t riscv_kullback_leibler_f16(const float16_t * pSrcA,const float16_t * pSrcB,uint32_t blockSize)
{
    const float16_t *pInA, *pInB;
    uint32_t blkCnt;
    _Float16 accum, pA,pB;
 
    pInA = pSrcA;
    pInB = pSrcB;
    blkCnt = blockSize;

    accum = 0.0f;

    while(blkCnt > 0)
    {
       pA = *pInA++;
       pB = *pInB++;
       accum += pA * logf(pB / pA);
       
       blkCnt--;
    
    }

    return(-accum);
}

/**
 * @} end of Kullback-Leibler group
 */

#endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */ 

