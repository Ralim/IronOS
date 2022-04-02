/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cmplx_mag_squared_q15.c
 * Description:  Q15 complex magnitude squared
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
  @addtogroup cmplx_mag_squared
  @{
 */

/**
  @brief         Q15 complex magnitude squared.
  @param[in]     pSrc        points to input vector
  @param[out]    pDst        points to output vector
  @param[in]     numSamples  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The function implements 1.15 by 1.15 multiplications and finally output is converted into 3.13 format.
 */

void riscv_cmplx_mag_squared_q15(
  const q15_t * pSrc,
        q15_t * pDst,
        uint32_t numSamples)
{
#if defined(RISCV_VECTOR)
  uint32_t blkCnt = numSamples;                               /* Loop counter */
  size_t l;
  const q15_t * input = pSrc;
  q15_t * output = pDst;
  ptrdiff_t bstride = 4;
  vint16m2_t v_R,v_I;
  vint32m4_t vR2_m4, vI2_m4;
  vint64m8_t vsum_m8;
  vint16m2_t v_summ2;
  for (; (l = vsetvl_e16m2(blkCnt)) > 0; blkCnt -= l) 
  {
    v_R = vlse16_v_i16m2(input, bstride, l);
    input++;
    v_I = vlse16_v_i16m2(input, bstride, l);
    input += (l*2-1);
    vR2_m4 = vwmul_vv_i32m4(v_R, v_R, l);
    vI2_m4 = vwmul_vv_i32m4(v_I, v_I, l);
    vsum_m8 = vwadd_vv_i64m8 (vR2_m4, vI2_m4, l);
    //v_summ2 = vnclip_wx_i16m2(vnsra_wx_i32m4(vsum_m8, 17), 0); 
    v_summ2 = vnclip_wx_i16m2(vnclip_wx_i32m4(vsum_m8, 17, l), 0, l);
    vse16_v_i16m2 (output, v_summ2, l);
    output += l;
  }

#else
        uint32_t blkCnt;                               /* Loop counter */

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
        q63_t in64;
        q63_t acc064;     
        q15_t real, imag;                              /* Temporary input variables */
        q31_t acc0,acc1;    
#else
        q31_t in;
        q31_t acc0;    
        q15_t real, imag;                              /* Temporary input variables */
        q31_t acc1;                                 /* Accumulators */
#endif /* __RISCV_XLEN == 64 */
#else
        q15_t real, imag;                              /* Temporary input variables */
        q31_t acc0,acc1;                              /* Accumulators */
#endif

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
    /* C[0] = (A[0] * A[0] + A[1] * A[1]) */

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
    in64 = read_q15x4_ia ((q15_t **) &pSrc);
    acc064 = __RV_KMDA(in64, in64);
    *pDst++ = (q15_t) ((q31_t)acc064 >> 17);
    *pDst++ = (q15_t) (acc064 >> 49);

    in64 = read_q15x4_ia ((q15_t **) &pSrc);
    acc064 = __RV_KMDA(in64, in64);
    *pDst++ = (q15_t) ((q31_t)acc064 >> 17);
    *pDst++ = (q15_t) (acc064 >> 49);
    
#else
    in = read_q15x2_ia ((q15_t **) &pSrc);
    acc0 = __RV_KMDA(in, in);
    /* store result in 3.13 format in destination buffer. */
    *pDst++ = (q15_t) (acc0 >> 17);

    in = read_q15x2_ia ((q15_t **) &pSrc);
    acc0 = __RV_KMDA(in, in);
    *pDst++ = (q15_t) (acc0 >> 17);

    in = read_q15x2_ia ((q15_t **) &pSrc);
    acc0 = __RV_KMDA(in, in);
    *pDst++ = (q15_t) (acc0 >> 17);

    in = read_q15x2_ia ((q15_t **) &pSrc);
    acc0 = __RV_KMDA(in, in);
    *pDst++ = (q15_t) (acc0 >> 17);
#endif /* __RISCV_XLEN == 64 */
#else
    real = *pSrc++;
    imag = *pSrc++;
    acc0 = ((q31_t) real * real);
    acc1 = ((q31_t) imag * imag);
    /* store result in 3.13 format in destination buffer. */
    *pDst++ = (q15_t) (((q63_t) acc0 + acc1) >> 17);

    real = *pSrc++;
    imag = *pSrc++;
    acc0 = ((q31_t) real * real);
    acc1 = ((q31_t) imag * imag);
    *pDst++ = (q15_t) (((q63_t) acc0 + acc1) >> 17);

    real = *pSrc++;
    imag = *pSrc++;
    acc0 = ((q31_t) real * real);
    acc1 = ((q31_t) imag * imag);
    *pDst++ = (q15_t) (((q63_t) acc0 + acc1) >> 17);

    real = *pSrc++;
    imag = *pSrc++;
    acc0 = ((q31_t) real * real);
    acc1 = ((q31_t) imag * imag);
    *pDst++ = (q15_t) (((q63_t) acc0 + acc1) >> 17);
#endif /* #if defined (RISCV_MATH_DSP) */

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
    /* C[0] = (A[0] * A[0] + A[1] * A[1]) */

// #if defined (RISCV_MATH_DSP)
//     in = read_q15x2_ia ((q15_t **) &pSrc);
//     acc0 = __SMUAD(in, in);

//     /* store result in 3.13 format in destination buffer. */
//     *pDst++ = (q15_t) (acc0 >> 17);
// #else
    real = *pSrc++;
    imag = *pSrc++;
    acc0 = ((q31_t) real * real);
    acc1 = ((q31_t) imag * imag);

    /* store result in 3.13 format in destination buffer. */
    *pDst++ = (q15_t) (((q63_t) acc0 + acc1) >> 17);
// #endif

    /* Decrement loop counter */
    blkCnt--;
  }
#endif /* defined(RISCV_VECTOR) */
}


/**
  @} end of cmplx_mag_squared group
 */
