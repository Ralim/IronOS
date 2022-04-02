/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_mult_q7.c
 * Description:  Q7 vector multiplication
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
  @addtogroup BasicMult
  @{
 */

/**
  @brief         Q7 vector multiplication
  @param[in]     pSrcA      points to the first input vector
  @param[in]     pSrcB      points to the second input vector
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   Results outside of the allowable Q7 range [0x80 0x7F] are saturated.
 */
void riscv_mult_q7(
  const q7_t * pSrcA,
  const q7_t * pSrcB,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  vint8m8_t vx, vy;
       
  for (; (l = vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle8_v_i8m8(pSrcA, l);
    pSrcA += l;
    vy = vle8_v_i8m8(pSrcB, l);
    vse8_v_i8m8 (pDst, vsmul_vv_i8m8(vx, vy, l), l);
    pSrcB += l;
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */

#if defined (RISCV_MATH_LOOPUNROLL)

#if defined (RISCV_MATH_DSP)
  q7_t out1, out2, out3, out4;                   /* Temporary output variables */
#endif


#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
  /* Loop unrolling: Compute 8 outputs at a time */
  blkCnt = blockSize >> 3U;
#else
	/* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;
#endif /* defined (RISCV_DSP64) || (__RISCV_XLEN == 64) */

  while (blkCnt > 0U)
  {
    /* C = A * B */

#if defined (RISCV_MATH_DSP)
    /* Multiply inputs and store results in temporary variables */
    //out1 = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    //out2 = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    //out3 = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    //out4 = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);

    ///* Pack and store result in destination buffer (in single write) */
    //write_q7x4_ia (&pDst, __PACKq7(out1, out2, out3, out4));
	//write_q7x4_ia (&pDst, __KHM8 (read_q7x4_ia ((q7_t **) &pSrcA), read_q7x4_ia ((q7_t **) &pSrcB)));
#if __RISCV_XLEN == 64
	write_q7x8_ia (&pDst, __RV_KHM8 (read_q7x8_ia ((q7_t **) &pSrcA), read_q7x8_ia ((q7_t **) &pSrcB)));
#else
#ifdef RISCV_DSP64
	write_q7x8_ia (&pDst, __RV_DKHM8 (read_q7x8_ia ((q7_t **) &pSrcA), read_q7x8_ia ((q7_t **) &pSrcB)));
#else
	write_q7x4_ia (&pDst, __RV_KHM8 (read_q7x4_ia ((q7_t **) &pSrcA), read_q7x4_ia ((q7_t **) &pSrcB)));
#endif
#endif /* __RISCV_XLEN == 64 */
#else
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);
#endif
#endif

    /* Decrement loop counter */
    blkCnt--;
  }

#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x8U;
#else
	blkCnt = blockSize % 0x4U;
#endif

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = A * B */

    /* Multiply input and store result in destination buffer. */
    *pDst++ = (q7_t) __SSAT((((q15_t) (*pSrcA++) * (*pSrcB++)) >> 7), 8);

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicMult group
 */
