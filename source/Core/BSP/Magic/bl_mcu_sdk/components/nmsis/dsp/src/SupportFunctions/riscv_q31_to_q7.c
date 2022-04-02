/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_q31_to_q7.c
 * Description:  Converts the elements of the Q31 vector to Q7 vector
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
  @brief         Converts the elements of the Q31 vector to Q7 vector.
  @param[in]     pSrc       points to the Q31 input vector
  @param[out]    pDst       points to the Q7 output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Details
                   The equation used for the conversion process is:
  <pre>
      pDst[n] = (q7_t) pSrc[n] >> 24;   0 <= n < blockSize.
  </pre>
 */
void riscv_q31_to_q7(
  const q31_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                         /* Loop counter */
  const q31_t *pIn = pSrc;                             /* Source pointer */
  size_t l;
  vint32m8_t v_in;
  vint8m2_t v_out;
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle32_v_i32m8(pIn, l);
    pIn += l;
    v_out = vnclip_wx_i8m2(vnclip_wx_i16m4(v_in, 24U, l), 0U, l);
    vse8_v_i8m2 (pDst, v_out, l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
  const q31_t *pIn = pSrc;                             /* Source pointer */

#if defined (RISCV_MATH_LOOPUNROLL)

  q7_t out1, out2, out3, out4;

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = (q7_t) (A >> 24) */

    /* Convert from q31 to q7 and store result in destination buffer */

    out1 = (q7_t) (*pIn++ >> 24);
    out2 = (q7_t) (*pIn++ >> 24);
    out3 = (q7_t) (*pIn++ >> 24);
    out4 = (q7_t) (*pIn++ >> 24);
    write_q7x4_ia (&pDst, __PACKq7(out1, out2, out3, out4));

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
    /* C = (q7_t) (A >> 24) */

    /* Convert from q31 to q7 and store result in destination buffer */
    *pDst++ = (q7_t) (*pIn++ >> 24);

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of q31_to_x group
 */
