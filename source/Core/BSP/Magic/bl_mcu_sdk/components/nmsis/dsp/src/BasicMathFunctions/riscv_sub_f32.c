/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_sub_f32.c
 * Description:  Floating-point vector subtraction
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
  @defgroup BasicSub Vector Subtraction

  Element-by-element subtraction of two vectors.

  <pre>
      pDst[n] = pSrcA[n] - pSrcB[n],   0 <= n < blockSize.
  </pre>

  There are separate functions for floating-point, Q7, Q15, and Q31 data types.
 */

/**
  @addtogroup BasicSub
  @{
 */

/**
  @brief         Floating-point vector subtraction.
  @param[in]     pSrcA      points to the first input vector
  @param[in]     pSrcB      points to the second input vector
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none
 */

void riscv_sub_f32(
  const float32_t * pSrcA,
  const float32_t * pSrcB,
        float32_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  vfloat32m8_t vx, vy;
       
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle32_v_f32m8(pSrcA, l);
    pSrcA += l;
    vy = vle32_v_f32m8(pSrcB, l);
    vse32_v_f32m8 (pDst, vfsub_vv_f32m8(vx, vy, l), l);
    pSrcB += l;
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A - B */

    /* Subtract and store result in destination buffer. */
    *pDst++ = (*pSrcA++) - (*pSrcB++);

    *pDst++ = (*pSrcA++) - (*pSrcB++);

    *pDst++ = (*pSrcA++) - (*pSrcB++);

    *pDst++ = (*pSrcA++) - (*pSrcB++);

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
    *pDst++ = (*pSrcA++) - (*pSrcB++);

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /*defined(RISCV_VECTOR)*/
}

/**
  @} end of BasicSub group
 */
