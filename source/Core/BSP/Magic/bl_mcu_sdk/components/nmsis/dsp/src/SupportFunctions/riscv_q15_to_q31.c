/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_q15_to_q31.c
 * Description:  Converts the elements of the Q15 vector to Q31 vector
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
  @brief         Converts the elements of the Q15 vector to Q31 vector.
  @param[in]     pSrc       points to the Q15 input vector
  @param[out]    pDst       points to the Q31 output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Details
                   The equation used for the conversion process is:
  <pre>
      pDst[n] = (q31_t) pSrc[n] << 16;   0 <= n < blockSize.
  </pre>
 */
void riscv_q15_to_q31(
  const q15_t * pSrc,
        q31_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                         /* Loop counter */
  const q15_t *pIn = pSrc;                             /* Source pointer */
  size_t l;
  vint16m4_t v_in;
  vint32m8_t v_out;
  for (; (l = vsetvl_e16m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_in = vle16_v_i16m4(pIn, l);
    pIn += l;
    v_out = vsll_vx_i32m8(vsext_vf2_i32m8(v_in, l), 16U, l);
    vse32_v_i32m8 (pDst, v_out, l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
  const q15_t *pIn = pSrc;                             /* Source pointer */

#if defined (RISCV_MATH_LOOPUNROLL)
        q31_t in1, in2;
        q31_t out1, out2, out3, out4;
#endif

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = (q31_t)A << 16 */

    /* Convert from q15 to q31 and store result in destination buffer */
    in1 = read_q15x2_ia ((q15_t **) &pIn);
    in2 = read_q15x2_ia ((q15_t **) &pIn);


    /* extract lower 16 bits to 32 bit result */
    out1 = in1 << 16U;
    /* extract upper 16 bits to 32 bit result */
    out2 = in1 & 0xFFFF0000;
    /* extract lower 16 bits to 32 bit result */
    out3 = in2 << 16U;
    /* extract upper 16 bits to 32 bit result */
    out4 = in2 & 0xFFFF0000;


    *pDst++ = out1;
    *pDst++ = out2;
    *pDst++ = out3;
    *pDst++ = out4;

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
    /* C = (q31_t) A << 16 */

    /* Convert from q15 to q31 and store result in destination buffer */
    *pDst++ = (q31_t) *pIn++ << 16;

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of q15_to_x group
 */
