/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_offset_q15.c
 * Description:  Q15 vector offset
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
  @addtogroup BasicOffset
  @{
 */

/**
  @brief         Adds a constant offset to a Q15 vector.
  @param[in]     pSrc       points to the input vector
  @param[in]     offset     is the offset to be added
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q15 range [0x8000 0x7FFF] are saturated.
 */
void riscv_offset_q15(
  const q15_t * pSrc,
        q15_t offset,
        q15_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  vint16m8_t vx;
       
  for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle16_v_i16m8(pSrc, l);
    pSrc += l;
    vse16_v_i16m8 (pDst, vsadd_vx_i16m8(vx, offset, l), l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */

#if defined (RISCV_MATH_LOOPUNROLL)

#if defined (RISCV_MATH_DSP)

 #if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
	q31_t offset_packed[2];                           /* Offset packed to 32 bit */
	q63_t offset_all;
  /* Offset is packed to 64 bit in order to use SIMD64 for addition */
  offset_packed[0] = __RV_PKBB16(offset, offset);
  offset_packed[1] = __RV_PKBB16(offset, offset);
  offset_all = *((q63_t *)offset_packed);
 #else
  q31_t offset_all;
  /* Offset is packed to 32 bit in order to use SIMD32 for addition */
  offset_all = __RV_PKBB16(offset, offset);
 #endif

#endif

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    /* C = A + offset */

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
	write_q15x4_ia (&pDst, __RV_KADD32(read_q15x4_ia ((q15_t **) &pSrc), offset_all));
#else
 #ifdef RISCV_DSP64
	write_q15x4_ia (&pDst, __DKADD16(read_q15x4_ia ((q15_t **) &pSrc), offset_all));
 #else
    /* Add offset and store result in destination buffer (2 samples at a time). */
    write_q15x2_ia (&pDst, __RV_KADD16(read_q15x2_ia ((q15_t **) &pSrc), offset_all));
    write_q15x2_ia (&pDst, __RV_KADD16(read_q15x2_ia ((q15_t **) &pSrc), offset_all));
 #endif
#endif /* __RISCV_XLEN == 64 */
#else
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrc++ + offset), 16);
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrc++ + offset), 16);
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrc++ + offset), 16);
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrc++ + offset), 16);
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
    /* C = A + offset */

    /* Add offset and store result in destination buffer. */
#if defined (RISCV_MATH_DSP)
    *pDst++ = (q15_t) __QADD16(*pSrc++, offset);
#else
    *pDst++ = (q15_t) __SSAT(((q31_t) *pSrc++ + offset), 16);
#endif

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicOffset group
 */
