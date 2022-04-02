/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_negate_q7.c
 * Description:  Negates Q7 vectors
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
  @addtogroup BasicNegate
  @{
 */

/**
  @brief         Negates the elements of a Q7 vector.
  @param[in]     pSrc       points to the input vector.
  @param[out]    pDst       points to the output vector.
  @param[in]     blockSize   number of samples in each vector.
  @return        none

  @par           Scaling and Overflow Behavior
                   The function uses saturating arithmetic.
                   The Q7 value -1 (0x80) is saturated to the maximum allowable positive value 0x7F.
 */
void riscv_negate_q7(
  const q7_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  size_t l;
  l = vsetvl_e8m8(blkCnt);
  vint8m8_t vx,vy = vmv_s_x_i8m8(vy, 0, l);
       
  for (; (l = vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle8_v_i8m8(pSrc, l);
    pSrc += l;
    vse8_v_i8m8 (pDst, vssub_vv_i8m8(vy ,vx, l), l);
    pDst += l;
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
        q7_t in;                                       /* Temporary input variable */

#if defined (RISCV_MATH_LOOPUNROLL)

#if defined (RISCV_MATH_DSP)
#ifdef RISCV_DSP64
  q63_t in1;                                    /* Temporary input variable */
#else
  q31_t in1;
#endif
#endif // RISCV_MATH_DSP

#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
  /* Loop unrolling: Compute 8 outputs at a time */
  blkCnt = blockSize >> 3U;
#else
	/* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;
#endif // RISCV_DSP64

  while (blkCnt > 0U)
  {
    /* C = -A */

#if defined (RISCV_MATH_DSP)
    /* Negate and store result in destination buffer (4 samples at a time). */
#if __RISCV_XLEN == 64
    in1 = read_q7x8_ia ((q7_t **) &pSrc);
    write_q7x8_ia (&pDst, __RV_KSUB8(0, in1));
#else
#ifdef RISCV_DSP64
    in1 = read_q7x8_ia ((q7_t **) &pSrc);
    write_q7x8_ia (&pDst, __DQSUB8(0, in1));
#else
    in1 = read_q7x4_ia ((q7_t **) &pSrc);
    write_q7x4_ia (&pDst, __RV_KSUB8(0, in1));

#endif
#endif /* __RISCV_XLEN == 64 */
#else
    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;

    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;

    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;

    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;

#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;

    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;

    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;

    in = *pSrc++;
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;
#endif // RISCV_DSP64

#endif

    /* Decrement loop counter */
    blkCnt--;
  }

#if defined (RISCV_DSP64) || (__RISCV_XLEN == 64)
  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x8U;
#else
	/* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;
#endif

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    /* C = -A */

    /* Negate and store result in destination buffer. */
    in = *pSrc++;

#if defined (RISCV_MATH_DSP)
    *pDst++ = (q7_t) __QSUB8(0, in);
#else
    *pDst++ = (in == (q7_t) 0x80) ? (q7_t) 0x7f : -in;
#endif

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicNegate group
 */
