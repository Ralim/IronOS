/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_copy_q7.c
 * Description:  Copies the elements of a Q7 vector
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
  @addtogroup copy
  @{
 */

/**
  @brief         Copies the elements of a Q7 vector.
  @param[in]     pSrc       points to input vector
  @param[out]    pDst       points to output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none
 */
void riscv_copy_q7(
  const q7_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  vint8m8_t v_copy;
       
  for (; (l = vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
    v_copy = vle8_v_i8m8(pSrc, l);
    pSrc += l;
    vse8_v_i8m8 (pDst, v_copy, l);
    pDst += l;
  }
#else
  uint32_t blkCnt;                               /* Loop counter */

#if defined (RISCV_MATH_LOOPUNROLL)
#if __RISCV_XLEN == 64
  /* Loop unrolling: Compute 8 outputs at a time */
  blkCnt = blockSize >> 3U;
#else
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;
#endif /* __RISCV_XLEN == 64 */

  while (blkCnt > 0U)
  {
    /* C = A */
#if __RISCV_XLEN == 64
    /* read 8 samples at a time */
    write_q7x8_ia (&pDst, read_q7x8_ia ((q7_t **) &pSrc));
#else
    /* read 4 samples at a time */
    write_q7x4_ia (&pDst, read_q7x4_ia ((q7_t **) &pSrc));
#endif /* __RISCV_XLEN == 64 */

    /* Decrement loop counter */
    blkCnt--;
  }
#if __RISCV_XLEN == 64
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x8U;
#else
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;
#endif /* __RISCV_XLEN == 64 */

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = A */

    /* Copy and store result in destination buffer */
    *pDst++ = *pSrc++;

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicCopy group
 */
