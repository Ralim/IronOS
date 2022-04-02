/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_lms_norm_q15.c
 * Description:  Processing function for Q15 normalized LMS filter
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
  @addtogroup LMS_NORM
  @{
 */

/**
  @brief         Processing function for Q15 normalized LMS filter.
  @param[in]     S         points to an instance of the Q15 normalized LMS filter structure
  @param[in]     pSrc      points to the block of input data
  @param[in]     pRef      points to the block of reference data
  @param[out]    pOut      points to the block of output data
  @param[out]    pErr      points to the block of error data
  @param[in]     blockSize number of samples to process
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using a 64-bit internal accumulator.
                   Both coefficients and state variables are represented in 1.15 format and
                   multiplications yield a 2.30 result. The 2.30 intermediate results are
                   accumulated in a 64-bit accumulator in 34.30 format.
                   There is no risk of internal overflow with this approach and the full
                   precision of intermediate multiplications is preserved. After all additions
                   have been performed, the accumulator is truncated to 34.15 format by
                   discarding low 15 bits. Lastly, the accumulator is saturated to yield a
                   result in 1.15 format.
 @par
  	               In this filter, filter coefficients are updated for each sample and the
                   updation of filter cofficients are saturted.
 */

void riscv_lms_norm_q15(
        riscv_lms_norm_instance_q15 * S,
  const q15_t * pSrc,
        q15_t * pRef,
        q15_t * pOut,
        q15_t * pErr,
        uint32_t blockSize)
{
        q15_t *pState = S->pState;                     /* State pointer */
        q15_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
        q15_t *pStateCurnt;                            /* Points to the current sample of the state */
        q15_t *px, *pb;                                /* Temporary pointers for state and coefficient buffers */
        q15_t mu = S->mu;                              /* Adaptive factor */
        uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
        uint32_t tapCnt, blkCnt;                       /* Loop counters */
        q63_t acc;                                     /* Accumulator */
        q31_t energy;                                  /* Energy of the input */
        q15_t e = 0, d = 0;                            /* Error, reference data sample */
        q15_t w = 0, in;                               /* Weight factor and state */
        q15_t x0;                                      /* Temporary variable to hold input sample */
        q15_t errorXmu, oneByEnergy;                   /* Temporary variables to store error and mu product and reciprocal of energy */
        q15_t postShift;                               /* Post shift to be applied to weight after reciprocal calculation */
        q31_t coef;                                    /* Temporary variable for coefficient */
        q31_t acc_l, acc_h;                            /* Temporary input */
        int32_t lShift = (15 - (int32_t) S->postShift);       /*  Post shift  */
        int32_t uShift = (32 - lShift);

  energy = S->energy;
  x0 = S->x0;

  /* S->pState points to buffer which contains previous frame (numTaps - 1) samples */
  /* pStateCurnt points to the location where the new input data should be written */
  pStateCurnt = &(S->pState[(numTaps - 1U)]);

  /* initialise loop count */
  blkCnt = blockSize;

  while (blkCnt > 0U)
  {
    /* Copy the new input sample into the state buffer */
    *pStateCurnt++ = *pSrc;

    /* Initialize pState pointer */
    px = pState;

    /* Initialize coefficient pointer */
    pb = pCoeffs;

    /* Read the sample from input buffer */
    in = *pSrc++;

    /* Update the energy calculation */
    energy -= (((q31_t) x0 * (x0)) >> 15);
    energy += (((q31_t) in * (in)) >> 15);

    /* Set the accumulator to zero */
    acc = 0;

#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

    /* Loop unrolling: Compute 4 taps at a time. */
    tapCnt = numTaps >> 2U;

    while (tapCnt > 0U)
    {
#if __RISCV_XLEN == 64
      acc = __RV_SMALDA(acc, read_q15x4_ia (&px), read_q15x4_ia (&pb));
#else
      /* Perform the multiply-accumulate */
      /* acc +=  b[N] * x[n-N] + b[N-1] * x[n-N-1] */
      acc = __RV_SMALDA(acc, read_q15x2_ia (&px), read_q15x2_ia (&pb));
      acc = __RV_SMALDA(acc, read_q15x2_ia (&px), read_q15x2_ia (&pb));
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
    vint16m4_t vx, vy;
    vint64m1_t temp00m1,temp01m1,accm1;
    l = vsetvl_e64m1(1);
    temp00m1 = vmv_v_x_i64m1(0, l);
    temp01m1 = vmv_v_x_i64m1(0, l);
    for (; (l = vsetvl_e16m4(vblkCnt)) > 0; vblkCnt -= l) {
      vx = vle16_v_i16m4(px, l);
      px += l;
      vy = vle16_v_i16m4(pb, l);
      pb += l;
      acc += vmv_x_s_i64m1_i64(vwredsum_vs_i32m8_i64m1 ( temp00m1,vwmul_vv_i32m8(vx, vy, l), temp01m1, l));
    }
#else
    while (tapCnt > 0U)
    {
      /* Perform the multiply-accumulate */
      acc += (q63_t) (((q31_t) (*px++) * (*pb++)));

      /* Decrement the loop counter */
      tapCnt--;
    }
#endif /* defined (RISCV_VECTOR) */
    /* Calc lower part of acc */
    acc_l = acc & 0xffffffff;

    /* Calc upper part of acc */
    acc_h = (acc >> 32) & 0xffffffff;

    /* Apply shift for lower part of acc and upper part of acc */
    acc = (uint32_t) acc_l >> lShift | acc_h << uShift;

    /* Converting the result to 1.15 format and saturate the output */
    acc = __SSAT(acc, 16U);

    /* Store the result from accumulator into the destination buffer. */
    *pOut++ = (q15_t) acc;

    /* Compute and store error */
    d = *pRef++;
    e = d - (q15_t) acc;
    *pErr++ = e;

    /* Calculation of 1/energy */
    postShift = riscv_recip_q15((q15_t) energy + DELTA_Q15, &oneByEnergy, S->recipTable);

    /* Calculation of e * mu value */
    errorXmu = (q15_t) (((q31_t) e * mu) >> 15);

    /* Calculation of (e * mu) * (1/energy) value */
    acc = (((q31_t) errorXmu * oneByEnergy) >> (15 - postShift));

    /* Weighting factor for the normalized version */
    w = (q15_t) __SSAT((q31_t) acc, 16);

    /* Initialize pState pointer */
    px = pState;

    /* Initialize coefficient pointer */
    pb = pCoeffs;

#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

    /* Loop unrolling: Compute 4 taps at a time. */
    tapCnt = numTaps >> 2U;

    /* Update filter coefficients */
    while (tapCnt > 0U)
    {
      coef = (q31_t) *pb + (((q31_t) w * (*px++)) >> 15);
      *pb++ = (q15_t) __SSAT(coef, 16);

      coef = (q31_t) *pb + (((q31_t) w * (*px++)) >> 15);
      *pb++ = (q15_t) __SSAT(coef, 16);

      coef = (q31_t) *pb + (((q31_t) w * (*px++)) >> 15);
      *pb++ = (q15_t) __SSAT(coef, 16);

      coef = (q31_t) *pb + (((q31_t) w * (*px++)) >> 15);
      *pb++ = (q15_t) __SSAT(coef, 16);

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
    vblkCnt = numTaps;
    for (; (l = vsetvl_e16m4(vblkCnt)) > 0; vblkCnt -= l) {
      vx = vle16_v_i16m4(px, l);
      px += l;
      vy = vle16_v_i16m4(pb, l);
      vse16_v_i16m4(pb,vnclip_wx_i16m4(vwadd_wv_i32m8(vsra_vx_i32m8(vwmul_vx_i32m8(vx, w, l),15, l),vy, l),0, l), l);
      pb += l;
    }
#else
    while (tapCnt > 0U)
    {
      /* Perform the multiply-accumulate */
      coef = (q31_t) *pb + (((q31_t) w * (*px++)) >> 15);
      *pb++ = (q15_t) __SSAT(coef, 16);

      /* Decrement loop counter */
      tapCnt--;
    }
#endif /* defined (RISCV_VECTOR) */
    x0 = *pState;

    /* Advance state pointer by 1 for the next sample */
    pState = pState + 1;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Save energy and x0 values for the next frame */
  S->energy = (q15_t) energy;
  S->x0 = x0;

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
    write_q15x2_ia (&pStateCurnt, read_q15x2_ia (&pState));
    write_q15x2_ia (&pStateCurnt, read_q15x2_ia (&pState));

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
    for (; (l = vsetvl_e16m4(vblkCnt)) > 0; vblkCnt -= l) {
      vse16_v_i16m4(pStateCurnt,vle16_v_i16m4(pState, l), l);
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
  @} end of LMS_NORM group
 */
