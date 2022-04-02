/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_dot_prod_q15.c
 * Description:  Processing function for the Q15 Complex Dot product
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

#include "dsp/complex_math_functions.h"

/**
  @ingroup groupCmplxMath
 */

/**
  @addtogroup cmplx_dot_prod
  @{
 */

/**
  @brief         Q15 complex dot product.
  @param[in]     pSrcA       points to the first input vector
  @param[in]     pSrcB       points to the second input vector
  @param[in]     numSamples  number of samples in each vector
  @param[out]    realResult  real part of the result returned here
  @param[out]    imagResult  imaginary part of the result returned her
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using an internal 64-bit accumulator.
                   The intermediate 1.15 by 1.15 multiplications are performed with full precision and yield a 2.30 result.
                   These are accumulated in a 64-bit accumulator with 34.30 precision.
                   As a final step, the accumulators are converted to 8.24 format.
                   The return results <code>realResult</code> and <code>imagResult</code> are in 8.24 format.
 */

void riscv_cmplx_dot_prod_q15(
  const q15_t * pSrcA,
  const q15_t * pSrcB,
        uint32_t numSamples,
        q31_t * realResult,
        q31_t * imagResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = numSamples;                               /* Loop counter */
  size_t l;
  const q15_t * inputA = pSrcA;
  const q15_t * inputB = pSrcB;
  ptrdiff_t bstride = 4;
  vint16m4_t v_R1, v_R2, v_I1, v_I2;
  vint32m8_t v_RR, v_II, v_RI, v_IR;
  // vint32m1_t v_dst;                      /* I don't know what the effect is  */ 
  l = vsetvl_e64m1(1);
  vint64m1_t v_real = vmv_s_x_i64m1(v_real, 0, l);
  vint64m1_t v_dst = vmv_s_x_i64m1(v_dst, 0, l);
  vint64m1_t v_imag = vmv_s_x_i64m1(v_imag, 0, l);
  /* Initialize accumulated value */
  for (; (l = vsetvl_e16m8(blkCnt)) > 0; blkCnt -= l) 
  {
    v_R1 = vlse16_v_i16m4(inputA, bstride, l);
    v_R2 = vlse16_v_i16m4(inputB, bstride, l);
    inputA++; inputB++;                  /* Point to the first complex pointer */
    v_I1 = vlse16_v_i16m4(inputA, bstride, l);
    v_I2 = vlse16_v_i16m4(inputB, bstride, l);
    v_RR = vwmul_vv_i32m8(v_R1, v_R2, l);
    v_II = vwmul_vv_i32m8(v_I1, v_I2, l);
    v_RI = vwmul_vv_i32m8(v_R1, v_I2, l);
    v_IR = vwmul_vv_i32m8(v_I1, v_R2, l);
    v_real = vwredsum_vs_i32m8_i64m1(v_dst, vssub_vv_i32m8(v_RR, v_II, l), v_real, l);
    v_imag = vwredsum_vs_i32m8_i64m1(v_dst, vsadd_vv_i32m8(v_RI, v_IR, l), v_imag, l);
    inputA += (l*2-1); inputB += (l*2-1);
  }
  vsetvl_e64m1(1);
  *realResult = (q31_t)(vmv_x_s_i64m1_i64(v_real)>> 6);
  *imagResult = (q31_t)(vmv_x_s_i64m1_i64(v_imag)>> 6);
#else
        uint32_t blkCnt;                               /* Loop counter */
        q63_t real_sum = 0, imag_sum = 0;              /* Temporary result variables */
#if __RISCV_XLEN == 64
        q31_t RESA,RESB;
#endif /* __RISCV_XLEN == 64 */
        q15_t a0,b0,c0,d0;

#if defined (RISCV_MATH_LOOPUNROLL)
  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
#if __RISCV_XLEN == 64
    RESA = read_q15x2_ia((q15_t **) &pSrcA);
    RESB = read_q15x2_ia((q15_t **) &pSrcB);
    real_sum += __RV_SMALDRS(0,RESA,RESB);
    imag_sum += __RV_SMALXDA(0,RESA,RESB);

    RESA = read_q15x2_ia((q15_t **) &pSrcA);
    RESB = read_q15x2_ia((q15_t **) &pSrcB);
    real_sum += __RV_SMALDRS(0,RESA,RESB);
    imag_sum += __RV_SMALXDA(0,RESA,RESB);

    RESA = read_q15x2_ia((q15_t **) &pSrcA);
    RESB = read_q15x2_ia((q15_t **) &pSrcB);
    real_sum += __RV_SMALDRS(0,RESA,RESB);
    imag_sum += __RV_SMALXDA(0,RESA,RESB);

    RESA = read_q15x2_ia((q15_t **) &pSrcA);
    RESB = read_q15x2_ia((q15_t **) &pSrcB);
    real_sum += __RV_SMALDRS(0,RESA,RESB);
    imag_sum += __RV_SMALXDA(0,RESA,RESB);
#else
    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

#if defined (RISCV_MATH_DSP)
	real_sum = __SMALBB(real_sum, a0, c0);
	imag_sum = __SMALBB(imag_sum, a0, d0);
	real_sum = __SMSLDA(real_sum, b0, d0);
	imag_sum = __SMALBB(imag_sum, b0, c0);
#else
	real_sum += (q31_t)a0 * c0;
    imag_sum += (q31_t)a0 * d0;
    real_sum -= (q31_t)b0 * d0;
    imag_sum += (q31_t)b0 * c0;
#endif

    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

#if defined (RISCV_MATH_DSP)
	real_sum = __SMALBB(real_sum, a0, c0);
	imag_sum = __SMALBB(imag_sum, a0, d0);
	real_sum = __SMSLDA(real_sum, b0, d0);
	imag_sum = __SMALBB(imag_sum, b0, c0);
#else
    real_sum += (q31_t)a0 * c0;
    imag_sum += (q31_t)a0 * d0;
    real_sum -= (q31_t)b0 * d0;
    imag_sum += (q31_t)b0 * c0;
#endif

    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

#if defined (RISCV_MATH_DSP)
	real_sum = __SMALBB(real_sum, a0, c0);
	imag_sum = __SMALBB(imag_sum, a0, d0);
	real_sum = __SMSLDA(real_sum, b0, d0);
	imag_sum = __SMALBB(imag_sum, b0, c0);
#else
    real_sum += (q31_t)a0 * c0;
    imag_sum += (q31_t)a0 * d0;
    real_sum -= (q31_t)b0 * d0;
    imag_sum += (q31_t)b0 * c0;
#endif

    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

#if defined(RISCV_MATH_DSP)
	real_sum = __SMALBB(real_sum, a0, c0);
	imag_sum = __SMALBB(imag_sum, a0, d0);
	real_sum = __SMSLDA(real_sum, b0, d0);
	imag_sum = __SMALBB(imag_sum, b0, c0);
#else
    real_sum += (q31_t)a0 * c0;
    imag_sum += (q31_t)a0 * d0;
    real_sum -= (q31_t)b0 * d0;
    imag_sum += (q31_t)b0 * c0;
#endif
#endif /* __RISCV_XLEN == 64 */
    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining outputs */
  blkCnt = numSamples % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = numSamples;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  while (blkCnt > 0U)
  {
    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

// #if defined(RISCV_MATH_DSP)
// 	real_sum = __SMALBB(real_sum, a0, c0);
// 	imag_sum = __SMALBB(imag_sum, a0, d0);
// 	real_sum = __SMSLDA(real_sum, b0, d0);
// 	imag_sum = __SMALBB(imag_sum, b0, c0);
// #else
    real_sum += (q31_t)a0 * c0;
    imag_sum += (q31_t)a0 * d0;
    real_sum -= (q31_t)b0 * d0;
    imag_sum += (q31_t)b0 * c0;
// #endif

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store real and imaginary result in 8.24 format  */
  /* Convert real data in 34.30 to 8.24 by 6 right shifts */
  *realResult = (q31_t) (real_sum >> 6);
  /* Convert imaginary data in 34.30 to 8.24 by 6 right shifts */
  *imagResult = (q31_t) (imag_sum >> 6);
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of cmplx_dot_prod group
 */
