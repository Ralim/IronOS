/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_q15_to_float.c
 * Description:  Converts the elements of the Q15 vector to floating-point vector
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

#include "dsp/support_functions.h"

/**
  @ingroup groupSupport
 */

/**
 * @defgroup q15_to_x  Convert 16-bit Integer value
 */

/**
  @addtogroup q15_to_x
  @{
 */

/**
  @brief         Converts the elements of the Q15 vector to floating-point vector.
  @param[in]     pSrc       points to the Q15 input vector
  @param[out]    pDst       points to the floating-point output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Details
                   The equation used for the conversion process is:
  <pre>
      pDst[n] = (float32_t) pSrc[n] / 32768;   0 <= n < blockSize.
  </pre>
 */

void riscv_q15_to_float(
  const q15_t * pSrc,
        float32_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  const q15_t *pIn = pSrc;                                   /* Src pointer */
  uint32_t blkCnt = blockSize;                               /* loop counter */
  size_t l;
  vint16m4_t v_in;
  vfloat32m8_t v_out;
  for (; (l = vsetvl_e16m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle16_v_i16m4(pIn, l);
    pIn += l;
    v_out = vfdiv_vf_f32m8(vfwcvt_f_x_v_f32m8(v_in, l), 32768.0f, l);
    vse32_v_f32m8 (pDst, v_out, l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
  const q15_t *pIn = pSrc;                             /* Source pointer */

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = (float32_t) A / 32768 */

    /* Convert from q15 to float and store result in destination buffer */
    *pDst++ = ((float32_t) * pIn++ / 32768.0f);
    *pDst++ = ((float32_t) * pIn++ / 32768.0f);
    *pDst++ = ((float32_t) * pIn++ / 32768.0f);
    *pDst++ = ((float32_t) * pIn++ / 32768.0f);

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = (float32_t) A / 32768 */

    /* Convert from q15 to float and store result in destination buffer */
    *pDst++ = ((float32_t) *pIn++ / 32768.0f);

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of q15_to_x group
 */
