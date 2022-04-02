/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_q31_to_q15.c
 * Description:  Converts the elements of the Q31 vector to Q15 vector
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
  @addtogroup q31_to_x
  @{
 */

/**
  @brief         Converts the elements of the Q31 vector to Q15 vector.
  @param[in]     pSrc       points to the Q31 input vector
  @param[out]    pDst       points to the Q15 output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Details
                   The equation used for the conversion process is:
  <pre>
      pDst[n] = (q15_t) pSrc[n] >> 16;   0 <= n < blockSize.
  </pre>
 */
void riscv_q31_to_q15(
  const q31_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                         /* Loop counter */
  const q31_t *pIn = pSrc;                             /* Source pointer */
  size_t l;
  vint32m8_t v_in;
  vint16m4_t v_out;
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle32_v_i32m8(pIn, l);
    pIn += l;
    v_out = vnclip_wx_i16m4(v_in, 16U, l);
    vse16_v_i16m4 (pDst, v_out, l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
  const q31_t *pIn = pSrc;                             /* Source pointer */

#if defined (RISCV_MATH_LOOPUNROLL) && defined (RISCV_MATH_DSP)
        q31_t in1, in2, in3, in4;
        q31_t out1, out2;
#endif

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = (q15_t) (A >> 16) */

    /* Convert from q31 to q15 and store result in destination buffer */
#if defined (RISCV_MATH_DSP)

    in1 = *pIn++;
    in2 = *pIn++;
    in3 = *pIn++;
    in4 = *pIn++;

    /* pack two higher 16-bit values from two 32-bit values */
    out1 = __PKHTB(in2, in1, 16);
    out2 = __PKHTB(in4, in3, 16);

    write_q15x2_ia (&pDst, out1);
    write_q15x2_ia (&pDst, out2);

#else

    *pDst++ = (q15_t) (*pIn++ >> 16);
    *pDst++ = (q15_t) (*pIn++ >> 16);
    *pDst++ = (q15_t) (*pIn++ >> 16);
    *pDst++ = (q15_t) (*pIn++ >> 16);

#endif /* #if defined (RISCV_MATH_DSP) */

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
    /* C = (q15_t) (A >> 16) */

    /* Convert from q31 to q15 and store result in destination buffer */
    *pDst++ = (q15_t) (*pIn++ >> 16);

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of q31_to_x group
 */
