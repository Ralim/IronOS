/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_fir_decimate_q31.c
 * Description:  Q31 FIR Decimator
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
  @addtogroup FIR_decimate
  @{
 */

/**
  @brief         Processing function for the Q31 FIR decimator.
  @param[in]     S          points to an instance of the Q31 FIR decimator structure
  @param[in]     pSrc       points to the block of input data
  @param[out]    pDst       points to the block of output data
  @param[in]     blockSize  number of samples to process
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using an internal 64-bit accumulator.
                   The accumulator has a 2.62 format and maintains full precision of the intermediate multiplication results but provides only a single guard bit.
                   Thus, if the accumulator result overflows it wraps around rather than clip.
                   In order to avoid overflows completely the input signal must be scaled down by log2(numTaps) bits (where log2 is read as log to the base 2).
                   After all multiply-accumulates are performed, the 2.62 accumulator is truncated to 1.32 format and then saturated to 1.31 format.

 @remark
                   Refer to \ref riscv_fir_decimate_fast_q31() for a faster but less precise implementation of this function.
 */

void riscv_fir_decimate_q31(
  const riscv_fir_decimate_instance_q31 * S,
  const q31_t * pSrc,
        q31_t * pDst,
        uint32_t blockSize)
{
        q31_t *pState = S->pState;                     /* State pointer */
  const q31_t *pCoeffs = S->pCoeffs;                   /* Coefficient pointer */
        q31_t *pStateCur;                              /* Points to the current sample of the state */
        q31_t *px0;                                    /* Temporary pointer for state buffer */
  const q31_t *pb;                                     /* Temporary pointer for coefficient buffer */
        q31_t x0, c0;                                  /* Temporary variables to hold state and coefficient values */
        q63_t acc0;                                    /* Accumulator */
        uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
        uint32_t i, tapCnt, blkCnt, outBlockSize = blockSize / S->M;  /* Loop counters */

#if defined (RISCV_MATH_LOOPUNROLL)
        q31_t *px1, *px2, *px3;
        q31_t x1, x2, x3;
        q63_t acc1, acc2, acc3;
#if __RISCV_XLEN == 64
        q63_t x064, c064, x164, x264, x364; 
#endif /* __RISCV_XLEN == 64 */
#endif

  /* S->pState buffer contains previous frame (numTaps - 1) samples */
  /* pStateCur points to the location where the new input data should be written */
  pStateCur = S->pState + (numTaps - 1U);

#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

    /* Loop unrolling: Compute 4 samples at a time */
  blkCnt = outBlockSize >> 2U;

  /* Samples loop unrolled by 4 */
  while (blkCnt > 0U)
  {
    /* Copy 4 * decimation factor number of new input samples into the state buffer */
    i = S->M * 4;

    do
    {
      *pStateCur++ = *pSrc++;

    } while (--i);

    /* Set accumulators to zero */
    acc0 = 0;
    acc1 = 0;
    acc2 = 0;
    acc3 = 0;

    /* Initialize state pointer for all the samples */
    px0 = pState;
    px1 = pState + S->M;
    px2 = pState + 2 * S->M;
    px3 = pState + 3 * S->M;

    /* Initialize coeff pointer */
    pb = pCoeffs;

    /* Loop unrolling: Compute 4 taps at a time */
    tapCnt = numTaps >> 2U;

    while (tapCnt > 0U)
    {
#if __RISCV_XLEN == 64
      c064 = read_q31x2_ia((q31_t **)&pb);
      x064 = read_q31x2_ia((q31_t **)&px0);
      x164 = read_q31x2_ia((q31_t **)&px1);
      x264 = read_q31x2_ia((q31_t **)&px2);
      x364 = read_q31x2_ia((q31_t **)&px3);

      acc0 = __RV_KMADA32(acc0, x064, c064);
      acc1 = __RV_KMADA32(acc1, x164, c064);
      acc2 = __RV_KMADA32(acc2, x264, c064);
      acc3 = __RV_KMADA32(acc3, x364, c064);

      c064 = read_q31x2_ia((q31_t **)&pb);
      x064 = read_q31x2_ia((q31_t **)&px0);
      x164 = read_q31x2_ia((q31_t **)&px1);
      x264 = read_q31x2_ia((q31_t **)&px2);
      x364 = read_q31x2_ia((q31_t **)&px3);

      acc0 = __RV_KMADA32(acc0, x064, c064);
      acc1 = __RV_KMADA32(acc1, x164, c064);
      acc2 = __RV_KMADA32(acc2, x264, c064);
      acc3 = __RV_KMADA32(acc3, x364, c064);

#else
      /* Read the b[numTaps-1] coefficient */
      c0 = *(pb++);

      /* Read x[n-numTaps-1] sample for acc0 */
      x0 = *(px0++);
      /* Read x[n-numTaps-1] sample for acc1 */
      x1 = *(px1++);
      /* Read x[n-numTaps-1] sample for acc2 */
      x2 = *(px2++);
      /* Read x[n-numTaps-1] sample for acc3 */
      x3 = *(px3++);

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;
      acc1 += (q63_t) x1 * c0;
      acc2 += (q63_t) x2 * c0;
      acc3 += (q63_t) x3 * c0;

      /* Read the b[numTaps-2] coefficient */
      c0 = *(pb++);

      /* Read x[n-numTaps-2] sample for acc0, acc1, acc2, acc3 */
      x0 = *(px0++);
      x1 = *(px1++);
      x2 = *(px2++);
      x3 = *(px3++);

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;
      acc1 += (q63_t) x1 * c0;
      acc2 += (q63_t) x2 * c0;
      acc3 += (q63_t) x3 * c0;

      /* Read the b[numTaps-3] coefficient */
      c0 = *(pb++);

      /* Read x[n-numTaps-3] sample acc0, acc1, acc2, acc3 */
      x0 = *(px0++);
      x1 = *(px1++);
      x2 = *(px2++);
      x3 = *(px3++);

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;
      acc1 += (q63_t) x1 * c0;
      acc2 += (q63_t) x2 * c0;
      acc3 += (q63_t) x3 * c0;

      /* Read the b[numTaps-4] coefficient */
      c0 = *(pb++);

      /* Read x[n-numTaps-4] sample acc0, acc1, acc2, acc3 */
      x0 = *(px0++);
      x1 = *(px1++);
      x2 = *(px2++);
      x3 = *(px3++);

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;
      acc1 += (q63_t) x1 * c0;
      acc2 += (q63_t) x2 * c0;
      acc3 += (q63_t) x3 * c0;
#endif /* __RISCV_XLEN == 64 */

      /* Decrement loop counter */
      tapCnt--;
    }

    /* Loop unrolling: Compute remaining taps */
    tapCnt = numTaps % 0x4U;

    while (tapCnt > 0U)
    {
      /* Read coefficients */
      c0 = *(pb++);

      /* Fetch state variables for acc0, acc1, acc2, acc3 */
      x0 = *(px0++);
      x1 = *(px1++);
      x2 = *(px2++);
      x3 = *(px3++);

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;
      acc1 += (q63_t) x1 * c0;
      acc2 += (q63_t) x2 * c0;
      acc3 += (q63_t) x3 * c0;

      /* Decrement loop counter */
      tapCnt--;
    }

    /* Advance the state pointer by the decimation factor
     * to process the next group of decimation factor number samples */
    pState = pState + S->M * 4;

    /* The result is in the accumulator, store in the destination buffer. */
    *pDst++ = (q31_t) (acc0 >> 31);
    *pDst++ = (q31_t) (acc1 >> 31);
    *pDst++ = (q31_t) (acc2 >> 31);
    *pDst++ = (q31_t) (acc3 >> 31);

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining samples */
  blkCnt = outBlockSize % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = outBlockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
//It turns out if add these part, it will take more cycles
#if defined (RISCV_VECTOR)
  uint32_t blkCnti = S->M;                              /* Loop counter */
  size_t l;
       
  for (; (l = vsetvl_e32m8(blkCnti)) > 0; blkCnti -= l) {
    vse32_v_i32m8 (pStateCur, vle32_v_i32m8(pSrc, l), l);
    pSrc += l;
    pStateCur += l;
  }
#else
    /* Copy decimation factor number of new input samples into the state buffer */
    i = S->M;

    do
    {
      *pStateCur++ = *pSrc++;

    } while (--i);
#endif /*defined (RISCV_VECTOR)*/
    /* Set accumulator to zero */
    acc0 = 0;

    /* Initialize state pointer */
    px0 = pState;

    /* Initialize coeff pointer */
    pb = pCoeffs;

#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

    /* Loop unrolling: Compute 4 taps at a time */
    tapCnt = numTaps >> 2U;

    while (tapCnt > 0U)
    {
      /* Read the b[numTaps-1] coefficient */
      c0 = *pb++;

      /* Read x[n-numTaps-1] sample */
      x0 = *px0++;

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;

      /* Read the b[numTaps-2] coefficient */
      c0 = *pb++;

      /* Read x[n-numTaps-2] sample */
      x0 = *px0++;

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;

      /* Read the b[numTaps-3] coefficient */
      c0 = *pb++;

      /* Read x[n-numTaps-3] sample */
      x0 = *px0++;

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;

      /* Read the b[numTaps-4] coefficient */
      c0 = *pb++;

      /* Read x[n-numTaps-4] sample */
      x0 = *px0++;

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;

      /* Decrement loop counter */
      tapCnt--;
    }

    /* Loop unrolling: Compute remaining taps */
    tapCnt = numTaps % 0x4U;

#else

    /* Initialize tapCnt with number of taps */
    tapCnt = numTaps;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */
#if defined (RISCV_VECTOR)
    uint32_t blkCntb;
    // size_t l;
    vint32m4_t va1m4,va2m4;
    vint64m8_t vch00m8;
    vint64m1_t vch00m1;
    vint64m1_t vtemp00m1;
    vint64m8_t vach00m8;
        blkCntb = numTaps;                               /* Loop counter */
        // q7_t temp[] = {0};
        l = vsetvl_e32m4(blkCntb);
        //initial array to zero
        vach00m8 = vmv_v_x_i64m8(0, l);
        l = vsetvl_e64m4(1);
        vtemp00m1 = vmv_v_x_i64m1(0, l);
        for (; (l = vsetvl_e32m4(blkCntb)) > 0; blkCntb -= l) {
            va1m4 = vle32_v_i32m4(pb, l);
            va2m4 = vle32_v_i32m4(px0, l);
            pb += l;
            px0 += l;
            vch00m8 = vwmul_vv_i64m8(va1m4, va2m4, l);
            vach00m8 = vadd_vv_i64m8(vach00m8, vch00m8, l);
        }
        //Here we calculate sum of four vector
        //set vl to max vl
        l = vsetvl_e32m4(numTaps);
        //calculate sum
        vch00m1 = vredsum_vs_i64m8_i64m1(vtemp00m1, vach00m8, vtemp00m1, l);
        //set vl to 1
        l = vsetvl_e64m1(1);
        //wrfte result scalar back
        acc0  += (q63_t)vmv_x_s_i64m1_i64(vch00m1);
#else
    while (tapCnt > 0U)
    {
      /* Read coefficients */
      c0 = *pb++;

      /* Fetch 1 state variable */
      x0 = *px0++;

      /* Perform the multiply-accumulate */
      acc0 += (q63_t) x0 * c0;

      /* Decrement loop counter */
      tapCnt--;
    }
#endif /*defined (RISCV_VECTOR)*/
    /* Advance the state pointer by the decimation factor
     * to process the next group of decimation factor number samples */
    pState = pState + S->M;

    /* The result is in the accumulator, store in the destination buffer. */
    *pDst++ = (q31_t) (acc0 >> 31);

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Processing is complete.
     Now copy the last numTaps - 1 samples to the satrt of the state buffer.
     This prepares the state buffer for the next function call. */

  /* Points to the start of the state buffer */
  pStateCur = S->pState;

#if defined (RISCV_MATH_LOOPUNROLL) && !defined (RISCV_VECTOR)

  /* Loop unrolling: Compute 4 taps at a time */
  tapCnt = (numTaps - 1U) >> 2U;

  /* Copy data */
  while (tapCnt > 0U)
  {
#if __RISCV_XLEN == 64
    write_q31x2_ia((q31_t **)&pStateCur,read_q31x2_ia((q31_t **)&pState));
    write_q31x2_ia((q31_t **)&pStateCur,read_q31x2_ia((q31_t **)&pState));
#else
    *pStateCur++ = *pState++;
    *pStateCur++ = *pState++;
    *pStateCur++ = *pState++;
    *pStateCur++ = *pState++;
#endif /* __RISCV_XLEN == 64 */

    /* Decrement loop counter */
    tapCnt--;
  }

  /* Loop unrolling: Compute remaining taps */
  tapCnt = (numTaps - 1U) % 0x04U;

#else

  /* Initialize tapCnt with number of taps */
  tapCnt = (numTaps - 1U);

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */
#if defined (RISCV_VECTOR)
  uint32_t blkCnti = tapCnt;                              /* Loop counter */
  size_t l;
       
  for (; (l = vsetvl_e32m8(blkCnti)) > 0; blkCnti -= l) {
    vse32_v_i32m8 (pStateCur, vle32_v_i32m8(pState, l), l);
    pState += l;
    pStateCur += l;
  }
#else
  /* Copy data */
  while (tapCnt > 0U)
  {
    *pStateCur++ = *pState++;

    /* Decrement loop counter */
    tapCnt--;
  }
#endif /*defined (RISCV_VECTOR)*/
}
/**
  @} end of FIR_decimate group
 */
