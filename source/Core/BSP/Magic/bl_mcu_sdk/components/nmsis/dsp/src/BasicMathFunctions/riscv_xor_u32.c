/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_xor_u32.c
 * Description:  uint32_t bitwise exclusive OR
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
  @addtogroup Xor
  @{
 */

/**
  @brief         Compute the logical bitwise XOR of two fixed-point vectors.
  @param[in]     pSrcA      points to input vector A
  @param[in]     pSrcB      points to input vector B
  @param[out]    pDst       points to output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none
 */

void riscv_xor_u32(
    const uint32_t * pSrcA,
    const uint32_t * pSrcB,
          uint32_t * pDst,
          uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  vuint32m8_t vx, vy;
       
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle32_v_u32m8(pSrcA, l);
    pSrcA += l;
    vy = vle32_v_u32m8(pSrcB, l);
    vse32_v_u32m8 (pDst, vxor_vv_u32m8(vx, vy, l), l);
    pSrcB += l;
    pDst += l;
  }
#else
    uint32_t blkCnt;      /* Loop counter */

#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)

    uint64_t * pSrcA_temp = pSrcA;
    uint64_t * pSrcB_temp = pSrcB;
    uint64_t * pDst_temp = pDst;
    if(blkCnt = blockSize >> 1)
    {
        while (blkCnt > 0U)
        {
            *pDst_temp++ = (*pSrcA_temp++)^(*pSrcB_temp++);

            /* Decrement the loop counter */
            blkCnt--;
        }
    }
    if(blkCnt = blockSize%2)
    {
        pSrcA = (uint8_t * )(pSrcA_temp-1);
        pSrcB = (uint8_t * )(pSrcB_temp-1);
    }

#else
    /* Initialize blkCnt with number of samples */
    blkCnt = blockSize;
#endif

    while (blkCnt > 0U)
    {
        *pDst++ = (*pSrcA++)^(*pSrcB++);

        /* Decrement the loop counter */
        blkCnt--;
    }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of Xor group
 */
