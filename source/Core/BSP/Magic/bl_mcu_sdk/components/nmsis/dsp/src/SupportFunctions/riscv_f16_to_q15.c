/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_float_to_q15.c
 * Description:  Converts the elements of the floating-point vector to Q15 vector
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

#include "dsp/support_functions_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)


/**
  @ingroup groupSupport
 */

/**
  @addtogroup f16_to_x
  @{
 */

/**
  @brief         Converts the elements of the f16 vector to Q15 vector.
  @param[in]     pSrc       points to the f16 input vector
  @param[out]    pDst       points to the Q15 output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Details
                   The equation used for the conversion process is:
  <pre>
      pDst[n] = (q15_t)(pSrc[n] * 32768);   0 <= n < blockSize.
  </pre>

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q15 range [0x8000 0x7FFF] are saturated.

  @note
                   In order to apply rounding in scalar version, the library should be rebuilt with the ROUNDING macro
                   defined in the preprocessor section of project options.
 */


void riscv_f16_to_q15(
  const float16_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize)
{
    const float16_t *pIn = pSrc;      /* Src pointer */
    uint32_t  blkCnt;           /* loop counter */
#ifdef RISCV_MATH_ROUNDING
    float16_t in;
#endif                          /*      #ifdef RISCV_MATH_ROUNDING        */

    /*
     * Loop over blockSize number of values
     */
    blkCnt = blockSize;

    while (blkCnt > 0U)
    {

#ifdef RISCV_MATH_ROUNDING

        /*
         * C = A * 65536
         */
        /*
         * convert from float to Q31 and then store the results in the destination buffer
         */
        in = *pIn++;
        in = (in * 32768.0);
        in += in > 0.0 ? 0.5 : -0.5;
        *pDst++ = clip_q31_to_q15((q31_t) (in));

#else

        /*
         * C = A * 32768
         */
        /*
         * convert from float to Q31 and then store the results in the destination buffer
         */
        *pDst++ = clip_q31_to_q15((q31_t) (*pIn++ * 32768.0));

#endif                          /*      #ifdef RISCV_MATH_ROUNDING        */

        /*
         * Decrement the loop counter
         */
        blkCnt--;
    }

}

/**
  @} end of f16_to_x group
 */

#endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */ 

