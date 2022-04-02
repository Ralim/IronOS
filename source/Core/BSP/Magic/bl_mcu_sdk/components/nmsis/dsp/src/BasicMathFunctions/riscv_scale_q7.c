/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_scale_q7.c
 * Description:  Multiplies a Q7 vector by a scalar
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
  @addtogroup BasicScale
  @{
 */

/**
  @brief         Multiplies a Q7 vector by a scalar.
  @param[in]     pSrc       points to the input vector
  @param[in]     scaleFract fractional portion of the scale value
  @param[in]     shift      number of bits to shift the result by
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The input data <code>*pSrc</code> and <code>scaleFract</code> are in 1.7 format.
                   These are multiplied to yield a 2.14 intermediate result and this is shifted with saturation to 1.7 format.
 */

void riscv_scale_q7(
  const q7_t * pSrc,
        q7_t scaleFract,
        int8_t shift,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  int8_t kShift = 7 - shift;                                 /* Shift to apply after scaling */
  size_t l;
  vint8m4_t vx;
       
  for (; (l = vsetvl_e8m4(blkCnt)) > 0; blkCnt -= l) {
    vx = vle8_v_i8m4(pSrc, l);
    pSrc += l;
    vse8_v_i8m4 (pDst, vnclip_wx_i8m4(vwmul_vx_i16m8(vx, scaleFract, l), kShift, l), l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
        int8_t kShift = 7 - shift;                     /* Shift to apply after scaling */

#if defined (RISCV_MATH_LOOPUNROLL)

#if defined (RISCV_MATH_DSP)
  q7_t in1,  in2,  in3,  in4;                    /* Temporary input variables */
  q7_t out1, out2, out3, out4;                   /* Temporary output variables */
#endif
// #if __RISCV_XLEN == 64
//   /* Loop unrolling: Compute 8 outputs at a time */
//   blkCnt = blockSize >> 3U;
// #else
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;
// #endif /* __RISCV_XLEN == 64 */

  while (blkCnt > 0U)
  {
    /* C = A * scale */

#if defined (RISCV_MATH_DSP)
// #if __RISCV_XLEN == 64
// 	q63_t scale_packed;                           /* scale packed to 32 bit */
// 	//q31_t scale_packed_32;
// 	q7_t scale_a[8];
// 	scale_a[0]	 = scaleFract;
// 	scale_a[1]	 = scaleFract;
// 	scale_a[2]	 = scaleFract;
// 	scale_a[3]	 = scaleFract;
// 	scale_a[4]	 = scaleFract;
// 	scale_a[5]	 = scaleFract;
// 	scale_a[6]	 = scaleFract;
// 	scale_a[7]	 = scaleFract;

// 	/* scale is packed to 64 bit in order to use SIMD64 for addition */
//     scale_packed = *((q63_t *)scale_a);
//   write_q7x8_ia (&pDst,__RV_KSLRA8((__RV_SMUL8(read_q7x8_ia ((q7_t **) &pSrc), scale_packed)),shift));
// #else
    /* Reading 4 inputs from memory */
    in1 = *pSrc++;
    in2 = *pSrc++;
    in3 = *pSrc++;
    in4 = *pSrc++;

    /* Scale inputs and store result in the temporary variable. */
    out1 = (q7_t) (__SSAT(((in1) * scaleFract) >> kShift, 8));
    out2 = (q7_t) (__SSAT(((in2) * scaleFract) >> kShift, 8));
    out3 = (q7_t) (__SSAT(((in3) * scaleFract) >> kShift, 8));
    out4 = (q7_t) (__SSAT(((in4) * scaleFract) >> kShift, 8));

    /* Pack and store result in destination buffer (in single write) */
    write_q7x4_ia (&pDst, __PACKq7(out1, out2, out3, out4));
// #endif /* __RISCV_XLEN == 64 */
#else
    *pDst++ = (q7_t) (__SSAT((((q15_t) *pSrc++ * scaleFract) >> kShift), 8));
    *pDst++ = (q7_t) (__SSAT((((q15_t) *pSrc++ * scaleFract) >> kShift), 8));
    *pDst++ = (q7_t) (__SSAT((((q15_t) *pSrc++ * scaleFract) >> kShift), 8));
    *pDst++ = (q7_t) (__SSAT((((q15_t) *pSrc++ * scaleFract) >> kShift), 8));
#endif

    /* Decrement loop counter */
    blkCnt--;
  }
// #if __RISCV_XLEN == 64
//   /* Loop unrolling: Compute remaining outputs */
//   blkCnt = blockSize % 0x8U;
// #else
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;
// #endif /* __RISCV_XLEN == 64 */
#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = A * scale */

    /* Scale input and store result in destination buffer. */
    *pDst++ = (q7_t) (__SSAT((((q15_t) *pSrc++ * scaleFract) >> kShift), 8));

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicScale group
 */
