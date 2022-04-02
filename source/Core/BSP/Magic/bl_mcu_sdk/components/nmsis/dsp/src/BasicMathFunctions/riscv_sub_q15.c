/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_sub_q15.c
 * Description:  Q15 vector subtraction
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
  @addtogroup BasicSub
  @{
 */

/**
  @brief         Q15 vector subtraction.
  @param[in]     pSrcA      points to the first input vector
  @param[in]     pSrcB      points to the second input vector
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q15 range [0x8000 0x7FFF] are saturated.
 */

void riscv_sub_q15(
  const q15_t * pSrcA,
  const q15_t * pSrcB,
        q15_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  vint16m8_t vx, vy;
       
  for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle16_v_i16m8(pSrcA, l);
    pSrcA += l;
    vy = vle16_v_i16m8(pSrcB, l);
    vse16_v_i16m8 (pDst, vssub_vv_i16m8(vx, vy, l), l);
    pSrcB += l;
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */

#if defined (RISCV_MATH_LOOPUNROLL)

#if defined (RISCV_MATH_DSP)
  q31_t inA1, inA2;
  q31_t inB1, inB2;
#endif

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A - B */

#if defined (RISCV_MATH_DSP)
    /* read 2 times 2 samples at a time from sourceA */
    //inA1 = read_q15x2_ia ((q15_t **) &pSrcA);
    //inA2 = read_q15x2_ia ((q15_t **) &pSrcA);
    ///* read 2 times 2 samples at a time from sourceB */
    //inB1 = read_q15x2_ia ((q15_t **) &pSrcB);
    //inB2 = read_q15x2_ia ((q15_t **) &pSrcB);

    ///* Subtract and store 2 times 2 samples at a time */
    //write_q15x2_ia (&pDst, __QSUB16(inA1, inB1));
    //write_q15x2_ia (&pDst, __QSUB16(inA2, inB2));
#if __RISCV_XLEN == 64
    write_q15x4_ia(&pDst, __RV_KSUB16(read_q15x4_ia((q15_t**)&pSrcA), read_q15x4_ia((q15_t **)&pSrcB)));
#else
#ifdef RISCV_DSP64
    write_q15x4_ia(&pDst, __DKSUB16(read_q15x4_ia((q15_t**)&pSrcA), read_q15x4_ia((q15_t **)&pSrcB)));
#else
	  write_q15x2_ia(&pDst, __RV_KSUB16(read_q15x2_ia((q15_t**)&pSrcA), read_q15x2_ia((q15_t **)&pSrcB)));
    write_q15x2_ia(&pDst, __RV_KSUB16(read_q15x2_ia((q15_t**)&pSrcA), read_q15x2_ia((q15_t **)&pSrcB)));
#endif
#endif /* __RISCV_XLEN == 64 */
#else
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrcA++ - *pSrcB++), 16);
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrcA++ - *pSrcB++), 16);
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrcA++ - *pSrcB++), 16);
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrcA++ - *pSrcB++), 16);
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
    /* C = A - B */

    /* Subtract and store result in destination buffer. */
#if defined (RISCV_MATH_DSP)
    *pDst++ = (q15_t) __QSUB16(*pSrcA++, *pSrcB++);
#else
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrcA++ - *pSrcB++), 16);
#endif

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicSub group
 */
