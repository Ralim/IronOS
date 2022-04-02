/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_q7_to_q15.c
 * Description:  Converts the elements of the Q7 vector to Q15 vector
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
  @addtogroup q7_to_x
  @{
 */

/**
  @brief         Converts the elements of the Q7 vector to Q15 vector.
  @param[in]     pSrc       points to the Q7 input vector
  @param[out]    pDst       points to the Q15 output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Details
                   The equation used for the conversion process is:
  <pre>
      pDst[n] = (q15_t) pSrc[n] << 8;   0 <= n < blockSize.
  </pre>
 */

void riscv_q7_to_q15(
  const q7_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                         /* Loop counter */
  const q7_t *pIn = pSrc;                             /* Source pointer */
  size_t l;
  vint8m4_t v_in;
  vint16m8_t v_out;
  for (; (l = vsetvl_e8m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle8_v_i8m4(pIn, l);
    pIn += l;
    v_out = vsll_vx_i16m8(vsext_vf2_i16m8(v_in, l), 8U, l);
    vse16_v_i16m8 (pDst, v_out, l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
  const q7_t *pIn = pSrc;                              /* Source pointer */

#if defined (RISCV_MATH_LOOPUNROLL) && defined (RISCV_MATH_DSP)
        q31_t in;
        q31_t in1, in2;
        q31_t out1, out2;
#endif

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = (q15_t) A << 8 */

    /* Convert from q7 to q15 and store result in destination buffer */
#if defined (RISCV_MATH_DSP)

    in = read_q7x4_ia ((q7_t **) &pIn);

    /* rotatate in by 8 and extend two q7_t values to q15_t values */
    in1 = __SXTB16(__ROR(in, 8));

    /* extend remainig two q7_t values to q15_t values */
    in2 = __SXTB16(in);

    in1 = in1 << 8U;
    in2 = in2 << 8U;

    in1 = in1 & 0xFF00FF00;
    in2 = in2 & 0xFF00FF00;

    out2 = __PKHTB(in1, in2, 16);
    out1 = __PKHBT(in2, in1, 16);

    write_q15x2_ia (&pDst, out1);
    write_q15x2_ia (&pDst, out2);

#else

    *pDst++ = (q15_t) *pIn++ << 8;
    *pDst++ = (q15_t) *pIn++ << 8;
    *pDst++ = (q15_t) *pIn++ << 8;
    *pDst++ = (q15_t) *pIn++ << 8;

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
    /* C = (q15_t) A << 8 */

    /* Convert from q7 to q15 and store result in destination buffer */
    *pDst++ = (q15_t) * pIn++ << 8;

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of q7_to_x group
 */
