/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_negate_q15.c
 * Description:  Negates Q15 vectors
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

#include "dsp/basic_math_functions.h"

/**
  @ingroup groupMath
 */

/**
  @addtogroup BasicNegate
  @{
 */

/**
  @brief         Negates the elements of a Q15 vector.
  @param[in]     pSrc       points to the input vector.
  @param[out]    pDst       points to the output vector.
  @param[in]     blockSize  number of samples in each vector.
  @return        none

  @par           Conditions for optimum performance
                   Input and output buffers should be aligned by 32-bit
  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   The Q15 value -1 (0x8000) is saturated to the maximum allowable positive value 0x7FFF.
 */
void riscv_negate_q15(
  const q15_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l = vsetvl_e16m8(blkCnt);
  vint16m8_t vx, vy = vmv_s_x_i16m8(vy, 0, l);
       
  for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle16_v_i16m8(pSrc, l);
    pSrc += l;
    vse16_v_i16m8 (pDst, vssub_vv_i16m8(vy ,vx, l), l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
        q15_t in;                                      /* Temporary input variable */

#if defined (RISCV_MATH_LOOPUNROLL)

#if defined (RISCV_MATH_DSP)
  q31_t in1;                                    /* Temporary input variables */
#endif

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = -A */

#if defined (RISCV_MATH_DSP)
    /* Negate and store result in destination buffer (2 samples at a time). */
    //in1 = read_q15x2_ia ((q15_t **) &pSrc);
    //write_q15x2_ia (&pDst, __QSUB16(0, in1));

    //in1 = read_q15x2_ia ((q15_t **) &pSrc);
    //write_q15x2_ia (&pDst, __QSUB16(0, in1));
#if __RISCV_XLEN == 64
    write_q15x4_ia(&pDst, __RV_KSUB16(0, read_q15x4_ia((q15_t **)&pSrc)));
#else
#ifdef RISCV_DSP64
    write_q15x4_ia(&pDst, __DKSUB16(0, read_q15x4_ia((q15_t **)&pSrc)));
#else
	  write_q15x2_ia(&pDst, __RV_KSUB16(0, read_q15x2_ia((q15_t **)&pSrc)));
	  write_q15x2_ia(&pDst, __RV_KSUB16(0, read_q15x2_ia((q15_t **)&pSrc)));
#endif
#endif /* __RISCV_XLEN == 64 */
#else
    in = *pSrc++;
    *pDst++ = (in == (q15_t) 0x8000) ? (q15_t) 0x7fff : -in;

    in = *pSrc++;
    *pDst++ = (in == (q15_t) 0x8000) ? (q15_t) 0x7fff : -in;

    in = *pSrc++;
    *pDst++ = (in == (q15_t) 0x8000) ? (q15_t) 0x7fff : -in;

    in = *pSrc++;
    *pDst++ = (in == (q15_t) 0x8000) ? (q15_t) 0x7fff : -in;
#endif

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
    /* C = -A */

    /* Negate and store result in destination buffer. */
    in = *pSrc++;
    *pDst++ = (in == (q15_t) 0x8000) ? (q15_t) 0x7fff : -in;

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicNegate group
 */
