/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cfft_q15.c
 * Description:  Combined Radix Decimation in Q15 Frequency CFFT processing function
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

#include "dsp/transform_functions.h"


extern void riscv_radix4_butterfly_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint32_t twidCoefModifier);

extern void riscv_radix4_butterfly_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint32_t twidCoefModifier);

extern void riscv_bitreversal_16(
        uint16_t * pSrc,
  const uint16_t bitRevLen,
  const uint16_t * pBitRevTable);

void riscv_cfft_radix4by2_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef);

void riscv_cfft_radix4by2_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef);

/**
  @ingroup groupTransforms
 */

/**
  @addtogroup ComplexFFT
  @{
 */

/**
  @brief         Processing function for Q15 complex FFT.
  @param[in]     S               points to an instance of Q15 CFFT structure
  @param[in,out] p1              points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place
  @param[in]     ifftFlag       flag that selects transform direction
                   - value = 0: forward transform
                   - value = 1: inverse transform
  @param[in]     bitReverseFlag flag that enables / disables bit reversal of output
                   - value = 0: disables bit reversal of output
                   - value = 1: enables bit reversal of output
  @return        none
 */

void riscv_cfft_q15(
  const riscv_cfft_instance_q15 * S,
        q15_t * p1,
        uint8_t ifftFlag,
        uint8_t bitReverseFlag)
{
  uint32_t L = S->fftLen;

  if (ifftFlag == 1U)
  {
     switch (L)
     {
     case 16:
     case 64:
     case 256:
     case 1024:
     case 4096:
       riscv_radix4_butterfly_inverse_q15 ( p1, L, (q15_t*)S->pTwiddle, 1 );
       break;

     case 32:
     case 128:
     case 512:
     case 2048:
       riscv_cfft_radix4by2_inverse_q15 ( p1, L, S->pTwiddle );
       break;
     }
  }
  else
  {
     switch (L)
     {
     case 16:
     case 64:
     case 256:
     case 1024:
     case 4096:
       riscv_radix4_butterfly_q15  ( p1, L, (q15_t*)S->pTwiddle, 1 );
       break;

     case 32:
     case 128:
     case 512:
     case 2048:
       riscv_cfft_radix4by2_q15  ( p1, L, S->pTwiddle );
       break;
     }
  }

  if ( bitReverseFlag )
    riscv_bitreversal_16 ((uint16_t*) p1, S->bitRevLength, S->pBitRevTable);
}

/**
  @} end of ComplexFFT group
 */

void riscv_cfft_radix4by2_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef)
{
        uint32_t i;
        uint32_t n2;
        q15_t p0, p1, p2, p3;
#if defined (RISCV_MATH_DSP)
        q31_t T, S, R;
        q31_t coeff, out1, out2;
  const q15_t *pC = pCoef;
        q15_t *pSi = pSrc;
        q15_t *pSl = pSrc + fftLen;
#if __RISCV_XLEN == 64
        q15_t xt, yt, cosVal, sinVal;
        uint32_t n264;
        q63_t T64, S64, R64;
        q63_t coeff64, out164, out264;
        uint32_t l;
#endif /* __RISCV_XLEN == 64 */
#else
        uint32_t l;
        q15_t xt, yt, cosVal, sinVal;
#endif

  n2 = fftLen >> 1U;

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
  n264 = n2 >> 1U;
  for (i = n264; i > 0; i--)
  {
      coeff64 = read_q15x4_ia ((q15_t **) &pC);

      T64 = read_q15x4 (pSi);
      T64 = __RV_RADD16(T64, 0); /* this is just a SIMD arithmetic shift right by 1 */

      S64 = read_q15x4 (pSl);
      S64 = __RV_RADD16(S64, 0); /* this is just a SIMD arithmetic shift right by 1 */

      R64 = __RV_KSUB16(T64, S64);

      write_q15x4_ia (&pSi, __RV_RADD16(T64, S64));

      out164 = __RV_KMDA(coeff64, R64) >> 16U;
      out264 = __RV_SMXDS(R64, coeff64);

      write_q15x4_ia (&pSl, (q63_t) (((out264) & 0xFFFF0000FFFF0000) | (out164 & 0x0000FFFF0000FFFF)));
  }

#else
  for (i = n2; i > 0; i--)
  {
      coeff = read_q15x2_ia ((q15_t **) &pC);

      T = read_q15x2 (pSi);
      T = __RV_RADD16(T, 0); /* this is just a SIMD arithmetic shift right by 1 */

      S = read_q15x2 (pSl);
      S = __RV_RADD16(S, 0); /* this is just a SIMD arithmetic shift right by 1 */

      R = __RV_KSUB16(T, S);

      write_q15x2_ia (&pSi, __RV_RADD16(T, S));

      out1 = __RV_KMDA(coeff, R) >> 16U;
      out2 = __RV_SMXDS(R, coeff);

      write_q15x2_ia (&pSl, (q31_t)__PKHBT( out1, out2, 0 ) );
  }
#endif /* __RISCV_XLEN == 64 */

#else /* #if defined (RISCV_MATH_DSP) */

  for (i = 0; i < n2; i++)
  {
     cosVal = pCoef[2 * i];
     sinVal = pCoef[2 * i + 1];

     l = i + n2;

     xt =           (pSrc[2 * i] >> 1U) - (pSrc[2 * l] >> 1U);
     pSrc[2 * i] = ((pSrc[2 * i] >> 1U) + (pSrc[2 * l] >> 1U)) >> 1U;

     yt =               (pSrc[2 * i + 1] >> 1U) - (pSrc[2 * l + 1] >> 1U);
     pSrc[2 * i + 1] = ((pSrc[2 * l + 1] >> 1U) + (pSrc[2 * i + 1] >> 1U)) >> 1U;

     pSrc[2 * l]     = (((int16_t) (((q31_t) xt * cosVal) >> 16U)) +
                        ((int16_t) (((q31_t) yt * sinVal) >> 16U))  );

     pSrc[2 * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16U)) -
                        ((int16_t) (((q31_t) xt * sinVal) >> 16U))   );
  }

#endif /* #if defined (RISCV_MATH_DSP) */

  /* first col */
  riscv_radix4_butterfly_q15( pSrc,          n2, (q15_t*)pCoef, 2U);

  /* second col */
  riscv_radix4_butterfly_q15( pSrc + fftLen, n2, (q15_t*)pCoef, 2U);

  n2 = fftLen >> 1U;
  for (i = 0; i < n2; i++)
  {
     p0 = pSrc[4 * i + 0];
     p1 = pSrc[4 * i + 1];
     p2 = pSrc[4 * i + 2];
     p3 = pSrc[4 * i + 3];

     p0 <<= 1U;
     p1 <<= 1U;
     p2 <<= 1U;
     p3 <<= 1U;

     pSrc[4 * i + 0] = p0;
     pSrc[4 * i + 1] = p1;
     pSrc[4 * i + 2] = p2;
     pSrc[4 * i + 3] = p3;
  }

}

void riscv_cfft_radix4by2_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef)
{
        uint32_t i;
        uint32_t n2;
        q15_t p0, p1, p2, p3;
#if defined (RISCV_MATH_DSP)
        q31_t T, S, R;
        q31_t coeff, out1, out2;
  const q15_t *pC = pCoef;
        q15_t *pSi = pSrc;
        q15_t *pSl = pSrc + fftLen;
#if __RISCV_XLEN == 64
        q15_t xt, yt, cosVal, sinVal;
        uint32_t n264;
        q63_t T64, S64, R64;
        q63_t coeff64, out164, out264;
        uint32_t l;
#endif /* __RISCV_XLEN == 64 */
#else
        uint32_t l;
        q15_t xt, yt, cosVal, sinVal;
#endif

  n2 = fftLen >> 1U;

#if defined (RISCV_MATH_DSP)
#if __RISCV_XLEN == 64
  n264 = n2 >> 1U;
  for (i = n264; i > 0; i--)
  {
      coeff64 = read_q15x4_ia ((q15_t **) &pC);

      T64 = read_q15x4 (pSi);
      T64 = __RV_RADD16(T64, 0); /* this is just a SIMD arithmetic shift right by 1 */

      S64 = read_q15x4 (pSl);
      S64 = __RV_RADD16(S64, 0); /* this is just a SIMD arithmetic shift right by 1 */

      R64 = __RV_KSUB16(T64, S64);

      write_q15x4_ia (&pSi, __RV_RADD16(T64, S64));

      out1 = __SMUAD(coeff, R) >> 16U;
      out2 = __SMUSDX(coeff, R);

      write_q15x4_ia (&pSl, (q63_t) ((out264) & 0xFFFF0000FFFF0000) | (out164 & 0x0000FFFF0000FFFF));
  }
  n264 = n2 % 2;
  if (1 == n264)
  {
      coeff = read_q15x2_ia ((q15_t **) &pC);

      T = read_q15x2 (pSi);
      T = __RV_RADD16(T, 0); /* this is just a SIMD arithmetic shift right by 1 */

      S = read_q15x2 (pSl);
      S = __RV_RADD16(S, 0); /* this is just a SIMD arithmetic shift right by 1 */

      R = __RV_KSUB16(T, S);

      write_q15x2_ia (&pSi, __RV_RADD16(T, S));

      out1 = __SMUAD(coeff, R) >> 16U;
      out2 = __SMUSDX(coeff, R);

      write_q15x2_ia (&pSl, (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));
  }

#else

  for (i = n2; i > 0; i--)
  {
     coeff = read_q15x2_ia ((q15_t **) &pC);

     T = read_q15x2 (pSi);
     T = __RV_RADD16(T, 0); /* this is just a SIMD arithmetic shift right by 1 */

     S = read_q15x2 (pSl);
     S = __RV_RADD16(S, 0); /* this is just a SIMD arithmetic shift right by 1 */

     R = __RV_KSUB16(T, S);

     write_q15x2_ia (&pSi, __RV_RADD16(T, S));

     out1 = __SMUSD(coeff, R) >> 16U;
     out2 = __SMUADX(coeff, R);

     write_q15x2_ia (&pSl, (q31_t)__PKHBT( out1, out2, 0 ));
  }
#endif
#else /* #if defined (RISCV_MATH_DSP) */

  for (i = 0; i < n2; i++)
  {
     cosVal = pCoef[2 * i];
     sinVal = pCoef[2 * i + 1];

     l = i + n2;

     xt =           (pSrc[2 * i] >> 1U) - (pSrc[2 * l] >> 1U);
     pSrc[2 * i] = ((pSrc[2 * i] >> 1U) + (pSrc[2 * l] >> 1U)) >> 1U;

     yt =               (pSrc[2 * i + 1] >> 1U) - (pSrc[2 * l + 1] >> 1U);
     pSrc[2 * i + 1] = ((pSrc[2 * l + 1] >> 1U) + (pSrc[2 * i + 1] >> 1U)) >> 1U;

     pSrc[2 * l]      = (((int16_t) (((q31_t) xt * cosVal) >> 16U)) -
                         ((int16_t) (((q31_t) yt * sinVal) >> 16U))  );

     pSrc[2 * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16U)) +
                        ((int16_t) (((q31_t) xt * sinVal) >> 16U))  );
  }

#endif /* #if defined (RISCV_MATH_DSP) */

  /* first col */
  riscv_radix4_butterfly_inverse_q15( pSrc,          n2, (q15_t*)pCoef, 2U);

  /* second col */
  riscv_radix4_butterfly_inverse_q15( pSrc + fftLen, n2, (q15_t*)pCoef, 2U);

  n2 = fftLen >> 1U;
  for (i = 0; i < n2; i++)
  {
     p0 = pSrc[4 * i + 0];
     p1 = pSrc[4 * i + 1];
     p2 = pSrc[4 * i + 2];
     p3 = pSrc[4 * i + 3];

     p0 <<= 1U;
     p1 <<= 1U;
     p2 <<= 1U;
     p3 <<= 1U;

     pSrc[4 * i + 0] = p0;
     pSrc[4 * i + 1] = p1;
     pSrc[4 * i + 2] = p2;
     pSrc[4 * i + 3] = p3;
  }
}

