/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_scale_f32.c
 * Description:  Multiplies a floating-point vector by a scalar
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
  @defgroup BasicScale Vector Scale

  Multiply a vector by a scalar value.  For floating-point data, the algorithm used is:

  <pre>
      pDst[n] = pSrc[n] * scale,   0 <= n < blockSize.
  </pre>

  In the fixed-point Q7, Q15, and Q31 functions, <code>scale</code> is represented by
  a fractional multiplication <code>scaleFract</code> and an arithmetic shift <code>shift</code>.
  The shift allows the gain of the scaling operation to exceed 1.0.
  The algorithm used with fixed-point data is:

  <pre>
      pDst[n] = (pSrc[n] * scaleFract) << shift,   0 <= n < blockSize.
  </pre>

  The overall scale factor applied to the fixed-point data is
  <pre>
      scale = scaleFract * 2^shift.
  </pre>

  The functions support in-place computation allowing the source and destination
  pointers to reference the same memory buffer.
 */

/**
  @addtogroup BasicScale
  @{
 */

/**
  @brief         Multiplies a floating-point vector by a scalar.
  @param[in]     pSrc       points to the input vector
  @param[in]     scale      scale factor to be applied
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none
 */

void riscv_scale_f32(
  const float32_t *pSrc,
        float32_t scale,
        float32_t *pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  vfloat32m8_t vx;
       
  for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle32_v_f32m8(pSrc, l);
    pSrc += l;
    vse32_v_f32m8 (pDst, vfmul_vf_f32m8(vx, scale, l), l);
    pDst += l;
  }
#else
  uint32_t blkCnt;                               /* Loop counter */
#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  while (blkCnt > 0U)
  {
    float32_t in1, in2, in3, in4;

    /* C = A * scale */

    /* Scale input and store result in destination buffer. */
    in1 = (*pSrc++) * scale;

    in2 = (*pSrc++) * scale;

    in3 = (*pSrc++) * scale;

    in4 = (*pSrc++) * scale;

    *pDst++ = in1;
    *pDst++ = in2;
    *pDst++ = in3;
    *pDst++ = in4;

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
    /* C = A * scale */

    /* Scale input and store result in destination buffer. */
    *pDst++ = (*pSrc++) * scale;

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /*defined(RISCV_VECTOR)*/
}

/**
  @} end of BasicScale group
 */
