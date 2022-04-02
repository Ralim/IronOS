
/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_jensenshannon_distance_f16.c
 * Description:  Jensen-Shannon distance between two vectors
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

#include "dsp/distance_functions_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)

#include <limits.h>
#include <math.h>

/**
  @ingroup FloatDist
 */

/**
  @defgroup JensenShannon Jensen-Shannon distance

  Jensen-Shannon distance
 */


/**
  @addtogroup JensenShannon
  @{
 */

/// @private
__STATIC_INLINE float16_t rel_entr(float16_t x, float16_t y)
{
    return (x * logf(x / y));
}




/**
 * @brief        Jensen-Shannon distance between two vectors
 *
 * This function is assuming that elements of second vector are > 0
 * and 0 only when the corresponding element of first vector is 0.
 * Otherwise the result of the computation does not make sense
 * and for speed reasons, the cases returning NaN or Infinity are not
 * managed.
 *
 * When the function is computing x log (x / y) with x == 0 and y == 0,
 * it will compute the right result (0) but a division by zero will occur
 * and should be ignored in client code.
 *
 * @param[in]    pA         First vector
 * @param[in]    pB         Second vector
 * @param[in]    blockSize  vector length
 * @return distance
 *
 */


float16_t riscv_jensenshannon_distance_f16(const float16_t *pA,const float16_t *pB, uint32_t blockSize)
{
    _Float16 left, right,sum, tmp;
    float16_t result;
    uint32_t i;

    left = 0.0f16; 
    right = 0.0f16;
    for(i=0; i < blockSize; i++)
    {
      tmp = ((_Float16)pA[i] + (_Float16)pB[i]) / 2.0f16;
      left  += (_Float16)rel_entr(pA[i], tmp);
      right += (_Float16)rel_entr(pB[i], tmp);
    }


    sum = left + right;
    riscv_sqrt_f16(sum/2.0f, &result);
    return(result);

}


/**
 * @} end of JensenShannon group
 */

#endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */ 

