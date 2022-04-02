/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_dot_prod_q31.c
 * Description:  Q31 complex dot product
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
  @brief         Q31 complex dot product.
  @param[in]     pSrcA       points to the first input vector
  @param[in]     pSrcB       points to the second input vector
  @param[in]     numSamples  number of samples in each vector
  @param[out]    realResult  real part of the result returned here
  @param[out]    imagResult  imaginary part of the result returned here
  @return        none

  @par           Scaling and Overflow Behavior
                   The function is implemented using an internal 64-bit accumulator.
                   The intermediate 1.31 by 1.31 multiplications are performed with 64-bit precision and then shifted to 16.48 format.
                   The internal real and imaginary accumulators are in 16.48 format and provide 15 guard bits.
                   Additions are nonsaturating and no overflow will occur as long as <code>numSamples</code> is less than 32768.
                   The return results <code>realResult</code> and <code>imagResult</code> are in 16.48 format.
                   Input down scaling is not required.
 */

void riscv_cmplx_dot_prod_q31(
  const q31_t * pSrcA,
  const q31_t * pSrcB,
        uint32_t numSamples,
        q63_t * realResult,
        q63_t * imagResult)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = numSamples;                               /* Loop counter */
  size_t l;
  const q31_t * inputA = pSrcA;
  const q31_t * inputB = pSrcB;
  ptrdiff_t bstride = 8;
  vint32m4_t v_R1, v_R2, v_I1, v_I2;
  vint64m8_t v_RR, v_II, v_RI, v_IR;
  vint64m1_t v_dst;                      /* I don't know what the effect is  */ 
  vint64m1_t v_real ;
  vint64m1_t v_imag ;    /* Initialize accumulated value */
  l = vsetvl_e64m1(1);
  v_real = vmv_s_x_i64m1(v_real, 0, l);
  v_imag = vmv_s_x_i64m1(v_imag, 0, l);
  v_dst = vmv_s_x_i64m1(v_dst, 0, l);

  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) 
  {
    v_R1 = vlse32_v_i32m4(inputA, bstride, l);
    v_R2 = vlse32_v_i32m4(inputB, bstride, l);
    inputA++; inputB++;                  /* Point to the first complex pointer */
    v_I1 = vlse32_v_i32m4(inputA, bstride, l);
    v_I2 = vlse32_v_i32m4(inputB, bstride, l);
    v_RR = vsra_vx_i64m8(vwmul_vv_i64m8(v_R1, v_R2, l), 14, l);
    v_II = vsra_vx_i64m8(vwmul_vv_i64m8(v_I1, v_I2, l), 14, l);
    v_RI = vsra_vx_i64m8(vwmul_vv_i64m8(v_R1, v_I2, l), 14, l);
    v_IR = vsra_vx_i64m8(vwmul_vv_i64m8(v_I1, v_R2, l), 14, l);
    v_real = vredsum_vs_i64m8_i64m1(v_dst, vssub_vv_i64m8(v_RR, v_II, l), v_real, l);
    v_imag = vredsum_vs_i64m8_i64m1(v_dst, vsadd_vv_i64m8(v_RI, v_IR, l), v_imag, l);
    inputA += (l*2-1); inputB += (l*2-1);
  }
  vsetvl_e32m4(1);
  *realResult = vmv_x_s_i64m1_i64(v_real);
  *imagResult = vmv_x_s_i64m1_i64(v_imag);
#else
        uint32_t blkCnt;                               /* Loop counter */
        q63_t real_sum = 0, imag_sum = 0;              /* Temporary result variables */
#if __RISCV_XLEN == 64
        q63_t RESA,RESB;
#endif /* __RISCV_XLEN == 64 */
        q31_t a0,b0,c0,d0;

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
#if __RISCV_XLEN == 64
    RESA = read_q31x2_ia((q31_t **) &pSrcA);
    RESB = read_q31x2_ia((q31_t **) &pSrcB);
    real_sum += (__RV_SMBB32(RESA, RESB) >> 14);
    real_sum -= (__RV_SMTT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESB, RESA) >> 14);

    RESA = read_q31x2_ia((q31_t **) &pSrcA);
    RESB = read_q31x2_ia((q31_t **) &pSrcB);
    real_sum += (__RV_SMBB32(RESA, RESB) >> 14);
    real_sum -= (__RV_SMTT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESB, RESA) >> 14);

    RESA = read_q31x2_ia((q31_t **) &pSrcA);
    RESB = read_q31x2_ia((q31_t **) &pSrcB);
    real_sum += (__RV_SMBB32(RESA, RESB) >> 14);
    real_sum -= (__RV_SMTT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESB, RESA) >> 14);

    RESA = read_q31x2_ia((q31_t **) &pSrcA);
    RESB = read_q31x2_ia((q31_t **) &pSrcB);
    real_sum += (__RV_SMBB32(RESA, RESB) >> 14);
    real_sum -= (__RV_SMTT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESA, RESB) >> 14);
    imag_sum += (__RV_SMBT32(RESB, RESA) >> 14);
#else
    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

    real_sum += ((q63_t)a0 * c0) >> 14;
    imag_sum += ((q63_t)a0 * d0) >> 14;
    real_sum -= ((q63_t)b0 * d0) >> 14;
    imag_sum += ((q63_t)b0 * c0) >> 14;

    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

    real_sum += ((q63_t)a0 * c0) >> 14;
    imag_sum += ((q63_t)a0 * d0) >> 14;
    real_sum -= ((q63_t)b0 * d0) >> 14;
    imag_sum += ((q63_t)b0 * c0) >> 14;

    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

    real_sum += ((q63_t)a0 * c0) >> 14;
    imag_sum += ((q63_t)a0 * d0) >> 14;
    real_sum -= ((q63_t)b0 * d0) >> 14;
    imag_sum += ((q63_t)b0 * c0) >> 14;

    a0 = *pSrcA++;
    b0 = *pSrcA++;
    c0 = *pSrcB++;
    d0 = *pSrcB++;

    real_sum += ((q63_t)a0 * c0) >> 14;
    imag_sum += ((q63_t)a0 * d0) >> 14;
    real_sum -= ((q63_t)b0 * d0) >> 14;
    imag_sum += ((q63_t)b0 * c0) >> 14;
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

    real_sum += ((q63_t)a0 * c0) >> 14;
    imag_sum += ((q63_t)a0 * d0) >> 14;
    real_sum -= ((q63_t)b0 * d0) >> 14;
    imag_sum += ((q63_t)b0 * c0) >> 14;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Store real and imaginary result in 16.48 format  */
  *realResult = real_sum;
  *imagResult = imag_sum;
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of cmplx_dot_prod group
 */
