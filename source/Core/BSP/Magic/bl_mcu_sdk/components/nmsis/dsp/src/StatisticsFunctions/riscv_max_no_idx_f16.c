/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_max_no_idx_f16.c
 * Description:  Maximum value of a floating-point vector without returning the index
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


/**
  @ingroup groupStats
 */


/**
  @addtogroup Max
  @{
 */

/**
  @brief         Maximum value of a floating-point vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     blockSize  number of samples in input vector
  @param[out]    pResult    maximum value returned here
  @return        none
 */


void riscv_max_no_idx_f16(
    const float16_t *pSrc,
    uint32_t   blockSize,
    float16_t *pResult)
{
   float16_t   maxValue = F16_MIN;
   float16_t   newVal;

   while (blockSize > 0U)
   {
       newVal = *pSrc++;
   
       /* compare for the maximum value */
       if (maxValue < newVal)
       {
           /* Update the maximum value and it's index */
           maxValue = newVal;
       }
   
       blockSize --;
   }
    
   *pResult = maxValue;
}


/**
  @} end of Max group
 */

#endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */ 

