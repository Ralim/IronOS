/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_shift_q31.c
 * Description:  Shifts the elements of a Q31 vector by a specified number of bits
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
  @defgroup BasicShift Vector Shift

  Shifts the elements of a fixed-point vector by a specified number of bits.
  There are separate functions for Q7, Q15, and Q31 data types.
  The underlying algorithm used is:

  <pre>
      pDst[n] = pSrc[n] << shift,   0 <= n < blockSize.
  </pre>

  If <code>shift</code> is positive then the elements of the vector are shifted to the left.
  If <code>shift</code> is negative then the elements of the vector are shifted to the right.

  The functions support in-place computation allowing the source and destination
  pointers to reference the same memory buffer.
 */

/**
  @addtogroup BasicShift
  @{
 */

/**
  @brief         Shifts the elements of a Q31 vector a specified number of bits.
  @param[in]     pSrc       points to the input vector
  @param[in]     shiftBits  number of bits to shift.  A positive value shifts left; a negative value shifts right.
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in the vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q31 range [0x80000000 0x7FFFFFFF] are saturated.
 */

void riscv_shift_q31(
  const q31_t * pSrc,
        int8_t shiftBits,
        q31_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  uint8_t sign = (shiftBits & 0x80);
  size_t l;
  vint32m4_t vx;
       
  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) {
    vx = vle32_v_i32m4(pSrc, l);
    pSrc += l;
    /* If the shift value is positive then do right shift else left shift */
    if (sign == 0U)
    {
      vse32_v_i32m4 (pDst, vnclip_wx_i32m4(vsll_vx_i64m8(vwadd_vx_i64m8(vx,0, l), shiftBits, l),0, l), l);
      pDst += l;
    }
    else
    {
      vse32_v_i32m4 (pDst, vsra_vx_i32m4(vx, -shiftBits, l), l);
      pDst += l;
    }
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
        uint8_t sign = (shiftBits & 0x80);             /* Sign of shiftBits */

#if defined (RISCV_MATH_LOOPUNROLL)

  q31_t in, out;                                 /* Temporary variables */

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;
#if __RISCV_XLEN == 64
    while (blkCnt > 0U)
    {
	    write_q31x2_ia(&pDst, __RV_KSLRA32(read_q31x2_ia((q31_t **)&pSrc), shiftBits));
	    write_q31x2_ia(&pDst, __RV_KSLRA32(read_q31x2_ia((q31_t **)&pSrc), shiftBits));
      blkCnt--;
    }
#else
  /* If the shift value is positive then do right shift else left shift */
    while (blkCnt > 0U)
    {
	    *pDst++ = __RV_KSLRAW(*pSrc++, shiftBits);
	    *pDst++ = __RV_KSLRAW(*pSrc++, shiftBits);
	    *pDst++ = __RV_KSLRAW(*pSrc++, shiftBits);
	    *pDst++ = __RV_KSLRAW(*pSrc++, shiftBits);
      blkCnt--;
    }
#endif /* __RISCV_XLEN == 64 */
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  /* If the shift value is positive then do right shift else left shift */
  if (sign == 0U)
  {
    while (blkCnt > 0U)
    {
      /* C = A << shiftBits */

      /* Shift input and store result in destination buffer. */
      *pDst++ = clip_q63_to_q31((q63_t) *pSrc++ << shiftBits);

      /* Decrement loop counter */
      blkCnt--;
    }
  }
  else
  {
    while (blkCnt > 0U)
    {
      /* C = A >> shiftBits */

      /* Shift input and store result in destination buffer. */
      *pDst++ = (*pSrc++ >> -shiftBits);

      /* Decrement loop counter */
      blkCnt--;
    }
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicShift group
 */
