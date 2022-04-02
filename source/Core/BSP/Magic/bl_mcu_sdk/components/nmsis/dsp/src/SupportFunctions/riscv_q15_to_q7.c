/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_q15_to_q7.c
 * Description:  Converts the elements of the Q15 vector to Q7 vector
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
  @addtogroup q15_to_x
  @{
 */

/**
  @brief         Converts the elements of the Q15 vector to Q7 vector.
  @param[in]     pSrc       points to the Q15 input vector
  @param[out]    pDst       points to the Q7 output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Details
                   The equation used for the conversion process is:
  <pre>
      pDst[n] = (q7_t) pSrc[n] >> 8;   0 <= n < blockSize.
  </pre>
 */
void riscv_q15_to_q7(
  const q15_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                         /* Loop counter */
  const q15_t *pIn = pSrc;                             /* Source pointer */
  size_t l;
  vint16m8_t v_in;
  vint8m4_t v_out;
  for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle16_v_i16m8(pIn, l);
    pIn += l;
    v_out = vnclip_wx_i8m4(v_in, 8U, l);
    vse8_v_i8m4 (pDst, v_out, l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
  const q15_t *pIn = pSrc;                             /* Source pointer */

#if defined (RISCV_MATH_LOOPUNROLL) && defined (RISCV_MATH_DSP)
        q31_t in1, in2;
        q31_t out1, out2;
#endif

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = (q7_t) A >> 8 */

    /* Convert from q15 to q7 and store result in destination buffer */
#if defined (RISCV_MATH_DSP)

    in1 = read_q15x2_ia ((q15_t **) &pIn);
    in2 = read_q15x2_ia ((q15_t **) &pIn);


    out1 = __PKHTB(in2, in1, 16);
    out2 = __PKHBT(in2, in1, 16);


    /* rotate packed value by 24 */
    out2 = ((uint32_t) out2 << 8) | ((uint32_t) out2 >> 24);

    /* anding with 0xff00ff00 to get two 8 bit values */
    out1 = out1 & 0xFF00FF00;
    /* anding with 0x00ff00ff to get two 8 bit values */
    out2 = out2 & 0x00FF00FF;

    /* oring two values(contains two 8 bit values) to get four packed 8 bit values */
    out1 = out1 | out2;

    /* store 4 samples at a time to destiantion buffer */
    write_q7x4_ia (&pDst, out1);

#else

    *pDst++ = (q7_t) (*pIn++ >> 8);
    *pDst++ = (q7_t) (*pIn++ >> 8);
    *pDst++ = (q7_t) (*pIn++ >> 8);
    *pDst++ = (q7_t) (*pIn++ >> 8);

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
    /* C = (q7_t) A >> 8 */

    /* Convert from q15 to q7 and store result in destination buffer */
    *pDst++ = (q7_t) (*pIn++ >> 8);

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of q15_to_x group
 */
