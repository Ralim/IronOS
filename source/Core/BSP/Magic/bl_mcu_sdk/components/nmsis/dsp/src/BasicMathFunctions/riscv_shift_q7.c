/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_shift_q7.c
 * Description:  Processing function for the Q7 Shifting
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
  @addtogroup BasicShift
  @{
 */

/**
  @brief         Shifts the elements of a Q7 vector a specified number of bits
  @param[in]     pSrc       points to the input vector
  @param[in]     shiftBits  number of bits to shift.  A positive value shifts left; a negative value shifts right.
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           onditions for optimum performance
                   Input and output buffers should be aligned by 32-bit
  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q7 range [0x80 0x7F] are saturated.
 */

void riscv_shift_q7(
  const q7_t * pSrc,
        int8_t shiftBits,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  uint8_t sign = (shiftBits & 0x80);
  size_t l;
  vint8m4_t vx;
       
  for (; (l = vsetvl_e8m4(blkCnt)) > 0; blkCnt -= l) {
    vx = vle8_v_i8m4(pSrc, l);
    pSrc += l;
    /* If the shift value is positive then do right shift else left shift */
    if (sign == 0U)
    {
      vse8_v_i8m4 (pDst, vnclip_wx_i8m4(vsll_vx_i16m8(vwadd_vx_i16m8(vx,0, l), shiftBits, l),0, l), l);
      pDst += l;
    }
    else
    {
      vse8_v_i8m4 (pDst, vsra_vx_i8m4(vx, -shiftBits, l), l);
      pDst += l;
    }
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
        uint8_t sign = (shiftBits & 0x80);             /* Sign of shiftBits */

#if defined (RISCV_MATH_LOOPUNROLL)

//#if defined (RISCV_MATH_DSP)
//  q7_t in1,  in2,  in3,  in4;                    /* Temporary input variables */
//#endif
#if __RISCV_XLEN == 64
  /* Loop unrolling: Compute 8 outputs at a time */
  blkCnt = blockSize >> 3U;

  while(blkCnt > 0U) {
	  write_q7x8_ia(&pDst, __RV_KSLRA8(read_q7x8_ia((q7_t **)&pSrc), shiftBits));
	  blkCnt--;
  }
#else
#ifdef RISCV_DSP64
  /* Loop unrolling: Compute 8 outputs at a time */
  blkCnt = blockSize >> 3U;

  while(blkCnt > 0U) {
	  write_q7x8_ia(&pDst, __RV_DKSLRA8(read_q7x8_ia((q7_t **)&pSrc), shiftBits));
	  blkCnt--;
  }
#else
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while(blkCnt > 0U) {
	  write_q7x4_ia(&pDst, __RV_KSLRA8(read_q7x4_ia((q7_t **)&pSrc), shiftBits));
	  blkCnt--;
  }
#endif
#endif /* __RISCV_XLEN == 64 */
//  /* If the shift value is positive then do right shift else left shift */
//  if (sign == 0U)
//  {
//    while (blkCnt > 0U)
//    {
//      /* C = A << shiftBits */
//
//#if defined (RISCV_MATH_DSP)
//      /* Read 4 inputs */
//      //in1 = *pSrc++;
//      //in2 = *pSrc++;
//      //in3 = *pSrc++;
//      //in4 = *pSrc++;
//
//    /* Pack and store result in destination buffer (in single write) */
//      //write_q7x4_ia (&pDst, __PACKq7(__SSAT((in1 << shiftBits), 8),
//      //                               __SSAT((in2 << shiftBits), 8),
//      //                               __SSAT((in3 << shiftBits), 8),
//      //                               __SSAT((in4 << shiftBits), 8) ));
//      write_q7x4_ia(&pDst, __KSLRA8(read_q7x4_ia((q7_t **)&pSrc), shiftBits));
//#else
//      *pDst++ = (q7_t) __SSAT(((q15_t) *pSrc++ << shiftBits), 8);
//      *pDst++ = (q7_t) __SSAT(((q15_t) *pSrc++ << shiftBits), 8);
//      *pDst++ = (q7_t) __SSAT(((q15_t) *pSrc++ << shiftBits), 8);
//      *pDst++ = (q7_t) __SSAT(((q15_t) *pSrc++ << shiftBits), 8);
//#endif
//
//      /* Decrement loop counter */
//      blkCnt--;
//    }
//  }
//  else
//  {
//    while (blkCnt > 0U)
//    {
//      /* C = A >> shiftBits */
//
//#if defined (RISCV_MATH_DSP)
//      /* Read 4 inputs */
//      in1 = *pSrc++;
//      in2 = *pSrc++;
//      in3 = *pSrc++;
//      in4 = *pSrc++;
//
//    /* Pack and store result in destination buffer (in single write) */
//      write_q7x4_ia (&pDst, __PACKq7((in1 >> -shiftBits),
//                                     (in2 >> -shiftBits),
//                                     (in3 >> -shiftBits),
//                                     (in4 >> -shiftBits) ));
//#else
//      *pDst++ = (*pSrc++ >> -shiftBits);
//      *pDst++ = (*pSrc++ >> -shiftBits);
//      *pDst++ = (*pSrc++ >> -shiftBits);
//      *pDst++ = (*pSrc++ >> -shiftBits);
//#endif
//
//      /* Decrement loop counter */
//      blkCnt--;
//    }
//  }

  /* Loop unrolling: Compute remaining outputs */
#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
  blkCnt = blockSize % 0x8U;
#else
  blkCnt = blockSize % 0x4U;
#endif

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
      *pDst++ = (q7_t) __SSAT(((q15_t) *pSrc++ << shiftBits), 8);

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
