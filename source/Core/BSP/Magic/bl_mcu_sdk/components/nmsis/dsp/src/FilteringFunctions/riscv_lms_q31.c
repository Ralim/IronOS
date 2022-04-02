/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_lms_q31.c
 * Description:  Processing function for the Q31 LMS filter
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

#include "dsp/filtering_functions.h"

/**
  @ingroup groupFilters
 */

/**
  @addtogroup LMS
  @{
 */

/**
  @brief         Processing function for Q31 LMS filter.
  @param[in]     S         points to an instance of the Q31 LMS filter structure.
  @param[in]     pSrc      points to the block of input data.
  @param[in]     pRef      points to the block of reference data.
  @param[out]    pOut      points to the block of output data.
  @param[out]    pErr      points to the block of error data.
  @param[in]     blockSize number of samples to process.
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using an internal 64-bit accumulator.
                   The accumulator has a 2.62 format and maintains full precision of the intermediate
                   multiplication results but provides only a single guard bit.
                   Thus, if the accumulator result overflows it wraps around rather than clips.
                   In order to avoid overflows completely the input signal must be scaled down by
                   log2(numTaps) bits.
                   The reference signal should not be scaled down.
                   After all multiply-accumulates are performed, the 2.62 accumulator is shifted
                   and saturated to 1.31 format to yield the final result.
                   The output signal and error signal are in 1.31 format.
 @par
                   In this filter, filter coefficients are updated for each sample and
                   the updation of filter cofficients are saturted.
 */

void riscv_lms_q31(
  const riscv_lms_instance_q31 * S,
  const q31_t * pSrc,
        q31_t * pRef,
        q31_t * pOut,
        q31_t * pErr,
        uint32_t blockSize)
{       
        q31_t *pState = S->pState;                     /* State pointer */
        q31_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
        q31_t *pStateCurnt;                            /* Points to the current sample of the state */
        q31_t *px, *pb;                                /* Temporary pointers for state and coefficient buffers */
        q31_t mu = S->mu;                              /* Adaptive factor */
        uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
        uint32_t tapCnt, blkCnt;                       /* Loop counters */
        q63_t acc;                                     /* Accumulator */
        q31_t e = 0;                                   /* Error of data sample */
        q31_t alpha;                                   /* Intermediate constant for taps update */
        q31_t coef;                                    /* Temporary variable for coef */
        q31_t acc_l, acc_h;                            /* Temporary input */
        uint32_t uShift = ((uint32_t) S->postShift + 1U);
        uint32_t lShift = 32U - uShift;                /*  Shift to be applied to the output */
#if __RISCV_XLEN == 64
        q63_t acc064, acc164, acc264;
#endif /* __RISCV_XLEN == 64 */
  /* S->pState points to buffer which contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1U)]);

  /* initialise loop count */
  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* Copy the new input sample into the state buffer */
    *pStateCurnt++ = *pSrc++;

    /* Initialize pState pointer */
    px = pState;

    /* Initialize coefficient pointer */
    pb = pCoeffs;

    /* Set the accumulator to zero */
    acc = 0;

#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

    /* Loop unrolling: Compute 4 taps at a time. */
    tapCnt = numTaps >> 2U;

    while (tapCnt > 0U)
    {
#if __RISCV_XLEN == 64
      acc064 = read_q31x2_ia(&px);
      acc164 = read_q31x2_ia(&pb);
      acc = __RV_KMADA32(acc, acc064, acc164);
      acc064 = read_q31x2_ia(&px);
      acc164 = read_q31x2_ia(&pb);
      acc = __RV_KMADA32(acc, acc064, acc164);
#else
      /* Perform the multiply-accumulate */
      /* acc +=  b[N] * x[n-N] */
      acc += ((q63_t) (*px++)) * (*pb++);

      /* acc +=  b[N-1] * x[n-N-1] */
      acc += ((q63_t) (*px++)) * (*pb++);

      /* acc +=  b[N-2] * x[n-N-2] */
      acc += ((q63_t) (*px++)) * (*pb++);

      /* acc +=  b[N-3] * x[n-N-3] */
      acc += ((q63_t) (*px++)) * (*pb++);
#endif /* __RISCV_XLEN == 64 */
      /* Decrement loop counter */
      tapCnt--;
    }

    /* Loop unrolling: Compute remaining taps */
    tapCnt = numTaps % 0x4U;

#else

    /* Initialize tapCnt with number of samples */
    tapCnt = numTaps;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */
#if defined (RISCV_VECTOR)
    uint32_t vblkCnt = numTaps;
    size_t l;
    vint32m4_t vx, vy;
    vint64m1_t temp00m1,temp01m1,accm1;
    l = vsetvl_e64m1(1);
    temp00m1 = vmv_v_x_i64m1(0, l);
    temp01m1 = vmv_v_x_i64m1(0, l);
    for (; (l = vsetvl_e32m4(vblkCnt)) > 0; vblkCnt -= l) {
      vx = vle32_v_i32m4(px, l);
      px += l;
      vy = vle32_v_i32m4(pb, l);
      pb += l;
      accm1 = vredsum_vs_i64m8_i64m1 ( temp00m1,vwmul_vv_i64m8(vx, vy, l), temp01m1, l);
      acc += vmv_x_s_i64m1_i64(accm1);
    }
#else
    while (tapCnt > 0U)
    {
      /* Perform the multiply-accumulate */
      acc += ((q63_t) (*px++)) * (*pb++);

      /* Decrement the loop counter */
      tapCnt--;
    }
#endif /* defined (RISCV_VECTOR) */
    /* Converting the result to 1.31 format */
    /* Calc lower part of acc */
    acc_l = acc & 0xffffffff;

    /* Calc upper part of acc */
    acc_h = (acc >> 32) & 0xffffffff;

    acc = (uint32_t) acc_l >> lShift | acc_h << uShift;

    /* Store the result from accumulator into the destination buffer. */
    *pOut++ = (q31_t) acc;

    /* Compute and store error */
    e = *pRef++ - (q31_t) acc;
    *pErr++ = e;

    /* Compute alpha i.e. intermediate constant for taps update */
    alpha = (q31_t) (((q63_t) e * mu) >> 31);

    /* Initialize pState pointer */
    /* Advance state pointer by 1 for the next sample */
    px = pState++;

    /* Initialize coefficient pointer */
    pb = pCoeffs;

#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

    /* Loop unrolling: Compute 4 taps at a time. */
    tapCnt = numTaps >> 2U;

    /* Update filter coefficients */
    while (tapCnt > 0U)
    {
      /* Perform the multiply-accumulate */

      /* coef is in 2.30 format */
      coef = (q31_t) (((q63_t) alpha * (*px++)) >> (32));
      /* get coef in 1.31 format by left shifting */
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1U));
      /* update coefficient buffer to next coefficient */
      pb++;

      coef = (q31_t) (((q63_t) alpha * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1U));
      pb++;

      coef = (q31_t) (((q63_t) alpha * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1U));
      pb++;

      coef = (q31_t) (((q63_t) alpha * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1U));
      pb++;

      /* Decrement loop counter */
      tapCnt--;
    }

    /* Loop unrolling: Compute remaining taps */
    tapCnt = numTaps % 0x4U;

#else

    /* Initialize tapCnt with number of samples */
    tapCnt = numTaps;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */
#if defined (RISCV_VECTOR)
    vblkCnt = blockSize;
    for (; (l = vsetvl_e32m4(vblkCnt)) > 0; vblkCnt -= l) {
      vx = vle32_v_i32m4(px, l);
      px += l;
      vse32_v_i32m4 (pb,vnclip_wx_i32m4(vwadd_vv_i64m8(vsll_vx_i32m4(vnclip_wx_i32m4(vwmul_vx_i64m8(vx, alpha, l),32, l),1, l), vle32_v_i32m4(pb, l), l),0, l), l);
      pb += l;
    }
#else
    while (tapCnt > 0U)
    {
      /* Perform the multiply-accumulate */
      coef = (q31_t) (((q63_t) alpha * (*px++)) >> (32));
      *pb = clip_q63_to_q31((q63_t) * pb + (coef << 1U));
      pb++;

      /* Decrement loop counter */
      tapCnt--;
    }
#endif /* defined (RISCV_VECTOR) */
    /* Decrement loop counter */
    blkCnt--;
  }

  /* Processing is complete.
     Now copy the last numTaps - 1 samples to the start of the state buffer.
     This prepares the state buffer for the next function call. */

  /* Points to the start of the pState buffer */
  pStateCurnt = S->pState;

  /* copy data */
#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

  /* Loop unrolling: Compute 4 taps at a time. */
  tapCnt = (numTaps - 1U) >> 2U;

  while (tapCnt > 0U)
  {
    *pStateCurnt++ = *pState++;
    *pStateCurnt++ = *pState++;
    *pStateCurnt++ = *pState++;
    *pStateCurnt++ = *pState++;

    /* Decrement loop counter */
    tapCnt--;
  }

  /* Loop unrolling: Compute remaining taps */
  tapCnt = (numTaps - 1U) % 0x4U;

#else

  /* Initialize tapCnt with number of samples */
  tapCnt = (numTaps - 1U);

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */
#if defined (RISCV_VECTOR)
    uint32_t vblkCnt = (numTaps - 1U);
    size_t l;
    for (; (l = vsetvl_e32m4(vblkCnt)) > 0; vblkCnt -= l) {
      vse32_v_i32m4(pStateCurnt,vle32_v_i32m4(pState, l), l);
      pState += l;
      pStateCurnt += l;
    }
#else
  while (tapCnt > 0U)
  {
    *pStateCurnt++ = *pState++;

    /* Decrement loop counter */
    tapCnt--;
  }
#endif /* defined (RISCV_VECTOR) */
}

/**
  @} end of LMS group
 */
