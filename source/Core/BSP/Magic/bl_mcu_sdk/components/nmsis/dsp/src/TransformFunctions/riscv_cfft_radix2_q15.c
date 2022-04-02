/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cfft_radix2_q15.c
 * Description:  Radix-2 Decimation in Frequency CFFT & CIFFT Fixed point processing function
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

void riscv_radix2_butterfly_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint16_t twidCoefModifier);

void riscv_radix2_butterfly_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint16_t twidCoefModifier);

void riscv_bitreversal_q15(
        q15_t * pSrc,
        uint32_t fftLen,
        uint16_t bitRevFactor,
  const uint16_t * pBitRevTab);

/**
  @ingroup groupTransforms
 */

/**
  @addtogroup ComplexFFT
  @{
 */

/**
  @brief         Processing function for the fixed-point CFFT/CIFFT.
  @deprecated    Do not use this function. It has been superseded by \ref riscv_cfft_q15 and will be removed in the future.
  @param[in]     S    points to an instance of the fixed-point CFFT/CIFFT structure
  @param[in,out] pSrc points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place
  @return        none
 */

void riscv_cfft_radix2_q15(
  const riscv_cfft_radix2_instance_q15 * S,
        q15_t * pSrc)
{

  if (S->ifftFlag == 1U)
  {
    riscv_radix2_butterfly_inverse_q15 (pSrc, S->fftLen, S->pTwiddle, S->twidCoefModifier);
  }
  else
  {
    riscv_radix2_butterfly_q15 (pSrc, S->fftLen, S->pTwiddle, S->twidCoefModifier);
  }

  riscv_bitreversal_q15(pSrc, S->fftLen, S->bitRevFactor, S->pBitRevTable);
}

/**
  @} end of ComplexFFT group
 */

void riscv_radix2_butterfly_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint16_t twidCoefModifier)
{
#if defined (RISCV_MATH_DSP)

  uint32_t i, j, k, l;
  uint32_t n1, n2, ia;
  q15_t in;
  q31_t T, S, R;
  q31_t coeff, out1, out2;
#if __RISCV_XLEN == 64
  q63_t T64, S64, R64;
  q63_t coeff64, out164, out264;
#endif /* __RISCV_XLEN == 64 */
  //N = fftLen;
  n2 = fftLen;

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  // loop for groups
  for (i = 0; i < n2; i++)
  {
#if __RISCV_XLEN == 64
    coeff64 = (q63_t)(((uint64_t)(read_q15x2 ((q15_t *)pCoef + (ia * 2U)))) | (((uint64_t)(read_q15x2 ((q15_t *)pCoef + ((ia + twidCoefModifier) * 2U)))) << 32));

    ia = ia + twidCoefModifier;
    ia = ia + twidCoefModifier;

    l = i + n2;

    T64 = read_q15x4 (pSrc + (2 * i));
    T64 = __RV_RADD16(T64, 0);

    S64 = read_q15x4 (pSrc + (2 * l));
    S64 = __RV_RADD16(S64, 0);

    R64 = __RV_KSUB16(T64, S64);

    write_q15x4 (pSrc + (2 * i), __RV_RADD16(T64, S64));

    out164 = __RV_KMDA(coeff64, R64) >> 16;
    out264 = __RV_SMXDS(R64, coeff64);

    write_q15x4 (pSrc + (2U * l), (q63_t) ((out264) & 0xFFFF0000FFFF0000) | (out164 & 0x0000FFFF0000FFFF));
    i++;
#else
    coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

    ia = ia + twidCoefModifier;

    l = i + n2;

    T = read_q15x2 (pSrc + (2 * i));
    in = ((int16_t) (T & 0xFFFF)) >> 1;
    T = ((T >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    S = read_q15x2 (pSrc + (2 * l));
    in = ((int16_t) (S & 0xFFFF)) >> 1;
    S = ((S >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    R = __QSUB16(T, S);

    write_q15x2 (pSrc + (2 * i), __SHADD16(T, S));

    out1 = __RV_KMDA(coeff, R) >> 16;
    out2 = __RV_SMXDS(R, coeff);

    write_q15x2 (pSrc + (2U * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));

    coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

    ia = ia + twidCoefModifier;

    /* loop for butterfly */
    i++;
    l++;

    T = read_q15x2 (pSrc + (2 * i));
    in = ((int16_t) (T & 0xFFFF)) >> 1;
    T = ((T >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    S = read_q15x2 (pSrc + (2 * l));
    in = ((int16_t) (S & 0xFFFF)) >> 1;
    S = ((S >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    R = __QSUB16(T, S);

    write_q15x2 (pSrc + (2 * i), __SHADD16(T, S));

    out1 = __RV_KMDA(coeff, R) >> 16;
    out2 = __RV_SMXDS(R, coeff);

    write_q15x2 (pSrc + (2U * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));

#endif /* __RISCV_XLEN == 64 */
  } /* groups loop end */

  twidCoefModifier = twidCoefModifier << 1U;

  /* loop for stage */
  for (k = fftLen / 2; k > 2; k = k >> 1)
  {
    n1 = n2;
    n2 = n2 >> 1;
    ia = 0;

    /* loop for groups */
    for (j = 0; j < n2; j++)
    {
      coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

      ia = ia + twidCoefModifier;

      /* loop for butterfly */
      for (i = j; i < fftLen; i += n1)
      {
        l = i + n2;

        T = read_q15x2 (pSrc + (2 * i));

        S = read_q15x2 (pSrc + (2 * l));

        R = __QSUB16(T, S);

        write_q15x2 (pSrc + (2 * i), __SHADD16(T, S));

    out1 = __RV_KMDA(coeff, R) >> 16;
    out2 = __RV_SMXDS(R, coeff);

        write_q15x2 (pSrc + (2U * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));

        i += n1;

        l = i + n2;

        T = read_q15x2 (pSrc + (2 * i));

        S = read_q15x2 (pSrc + (2 * l));

        R = __QSUB16(T, S);

        write_q15x2 (pSrc + (2 * i), __SHADD16(T, S));

    out1 = __RV_KMDA(coeff, R) >> 16;
    out2 = __RV_SMXDS(R, coeff);

        write_q15x2 (pSrc + (2U * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));

      } /* butterfly loop end */

    } /* groups loop end */

    twidCoefModifier = twidCoefModifier << 1U;
  } /* stages loop end */

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

  ia = ia + twidCoefModifier;

  /* loop for butterfly */
  for (i = 0; i < fftLen; i += n1)
  {
    l = i + n2;

    T = read_q15x2 (pSrc + (2 * i));

    S = read_q15x2 (pSrc + (2 * l));

    R = __RV_KSUB16(T, S);

    write_q15x2 (pSrc + (2 * i), __RV_KADD16(T, S));

    write_q15x2 (pSrc + (2 * l), R);

    i += n1;
    l = i + n2;

    T = read_q15x2 (pSrc + (2 * i));

    S = read_q15x2 (pSrc + (2 * l));

    R = __RV_KSUB16(T, S);

    write_q15x2 (pSrc + (2 * i), __RV_KADD16(T, S));

    write_q15x2 (pSrc + (2 * l), R);

  } /* groups loop end */


#else /* #if defined (RISCV_MATH_DSP) */

  uint32_t i, j, k, l;
  uint32_t n1, n2, ia;
  q15_t xt, yt, cosVal, sinVal;


  // N = fftLen;
  n2 = fftLen;

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  /* loop for groups */
  for (j = 0; j < n2; j++)
  {
    cosVal = pCoef[(ia * 2)];
    sinVal = pCoef[(ia * 2) + 1];
    ia = ia + twidCoefModifier;

    /* loop for butterfly */
    for (i = j; i < fftLen; i += n1)
    {
      l = i + n2;
      xt = (pSrc[2 * i] >> 1U) - (pSrc[2 * l] >> 1U);
      pSrc[2 * i] = ((pSrc[2 * i] >> 1U) + (pSrc[2 * l] >> 1U)) >> 1U;

      yt = (pSrc[2 * i + 1] >> 1U) - (pSrc[2 * l + 1] >> 1U);
      pSrc[2 * i + 1] = ((pSrc[2 * l + 1] >> 1U) +
                         (pSrc[2 * i + 1] >> 1U)  ) >> 1U;

      pSrc[2 * l] = (((int16_t) (((q31_t) xt * cosVal) >> 16)) +
                     ((int16_t) (((q31_t) yt * sinVal) >> 16)));

      pSrc[2U * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16)) -
                          ((int16_t) (((q31_t) xt * sinVal) >> 16)));

    } /* butterfly loop end */

  } /* groups loop end */

  twidCoefModifier = twidCoefModifier << 1U;

  /* loop for stage */
  for (k = fftLen / 2; k > 2; k = k >> 1)
  {
    n1 = n2;
    n2 = n2 >> 1;
    ia = 0;

    /* loop for groups */
    for (j = 0; j < n2; j++)
    {
      cosVal = pCoef[ia * 2];
      sinVal = pCoef[(ia * 2) + 1];
      ia = ia + twidCoefModifier;

      /* loop for butterfly */
      for (i = j; i < fftLen; i += n1)
      {
        l = i + n2;
        xt = pSrc[2 * i] - pSrc[2 * l];
        pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]) >> 1U;

        yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
        pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]) >> 1U;

        pSrc[2 * l] = (((int16_t) (((q31_t) xt * cosVal) >> 16)) +
                       ((int16_t) (((q31_t) yt * sinVal) >> 16)));

        pSrc[2U * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16)) -
                            ((int16_t) (((q31_t) xt * sinVal) >> 16)));

      } /* butterfly loop end */

    } /* groups loop end */

    twidCoefModifier = twidCoefModifier << 1U;
  } /* stages loop end */

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  /* loop for groups */
  for (j = 0; j < n2; j++)
  {
    cosVal = pCoef[ia * 2];
    sinVal = pCoef[(ia * 2) + 1];

    ia = ia + twidCoefModifier;

    /* loop for butterfly */
    for (i = j; i < fftLen; i += n1)
    {
      l = i + n2;
      xt = pSrc[2 * i] - pSrc[2 * l];
      pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]);

      yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
      pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]);

      pSrc[2 * l] = xt;

      pSrc[2 * l + 1] = yt;

    } /* butterfly loop end */

  } /* groups loop end */

  twidCoefModifier = twidCoefModifier << 1U;

#endif /* #if defined (RISCV_MATH_DSP) */

}


void riscv_radix2_butterfly_inverse_q15(
        q15_t * pSrc,
        uint32_t fftLen,
  const q15_t * pCoef,
        uint16_t twidCoefModifier)
{
#if defined (RISCV_MATH_DSP)

        uint32_t i, j, k, l;
        uint32_t n1, n2, ia;
        q15_t in;
        q31_t T, S, R;
        q31_t coeff, out1, out2;
#if __RISCV_XLEN == 64
  q63_t T64, S64, R64;
  q63_t coeff64, out164, out264;
#endif /* __RISCV_XLEN == 64 */
  // N = fftLen;
  n2 = fftLen;

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  /* loop for groups */
  for (i = 0; i < n2; i++)
  {
#if __RISCV_XLEN == 64
    coeff64 = (q63_t)(((uint64_t)(read_q15x2 ((q15_t *)pCoef + (ia * 2U)))) | (((uint64_t)(read_q15x2 ((q15_t *)pCoef + ((ia + twidCoefModifier) * 2U)))) << 32));

    ia = ia + twidCoefModifier;
    ia = ia + twidCoefModifier;

    l = i + n2;

    T64 = read_q15x4 (pSrc + (2 * i));
    T64 = __RV_RADD16(T64, 0);

    S64 = read_q15x4 (pSrc + (2 * l));
    S64 = __RV_RADD16(S64, 0);

    R64 = __RV_KSUB16(T64, S64);

    write_q15x4 (pSrc + (2 * i), __RV_RADD16(T64, S64));

    out164 = __RV_KMDA(coeff64, R64) >> 16;
    out264 = __RV_SMXDS(R64, coeff64);

    write_q15x4 (pSrc + (2U * l), (q63_t) ((out264) & 0xFFFF0000FFFF0000) | (out164 & 0x0000FFFF0000FFFF));
    i++;
#else
    coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

    ia = ia + twidCoefModifier;

    l = i + n2;

    T = read_q15x2 (pSrc + (2 * i));
    in = ((int16_t) (T & 0xFFFF)) >> 1;
    T = ((T >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    S = read_q15x2 (pSrc + (2 * l));
    in = ((int16_t) (S & 0xFFFF)) >> 1;
    S = ((S >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    R = __RV_KSUB16(T, S);

    write_q15x2 (pSrc + (2 * i), __RV_RADD16(T, S));

    out1 = __RV_SMDRS(coeff, R) >> 16;
    out2 = __RV_KMXDA(coeff, R);

    write_q15x2 (pSrc + (2 * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));

    coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

    ia = ia + twidCoefModifier;

    /* loop for butterfly */
    i++;
    l++;

    T = read_q15x2 (pSrc + (2 * i));
    in = ((int16_t) (T & 0xFFFF)) >> 1;
    T = ((T >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    S = read_q15x2 (pSrc + (2 * l));
    in = ((int16_t) (S & 0xFFFF)) >> 1;
    S = ((S >> 1) & 0xFFFF0000) | (in & 0xFFFF);

    R = __RV_KSUB16(T, S);

    write_q15x2 (pSrc + (2 * i), __RV_RADD16(T, S));

    out1 = __RV_SMDRS(coeff, R) >> 16;
    out2 = __RV_KMXDA(coeff, R);

    write_q15x2 (pSrc + (2 * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));
#endif /* __RISCV_XLEN == 64 */
  } /* groups loop end */

  twidCoefModifier = twidCoefModifier << 1U;

  /* loop for stage */
  for (k = fftLen / 2; k > 2; k = k >> 1)
  {
    n1 = n2;
    n2 = n2 >> 1;
    ia = 0;

    /* loop for groups */
    for (j = 0; j < n2; j++)
    {
      coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

      ia = ia + twidCoefModifier;

      /* loop for butterfly */
      for (i = j; i < fftLen; i += n1)
      {
        l = i + n2;

        T = read_q15x2 (pSrc + (2 * i));

        S = read_q15x2 (pSrc + (2 * l));

        R = __RV_KSUB16(T, S);

        write_q15x2 (pSrc + (2 * i), __RV_RADD16(T, S));

        out1 = __RV_SMDRS(coeff, R) >> 16;
        out2 = __RV_KMXDA(coeff, R);

        write_q15x2 (pSrc + (2 * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));

        i += n1;

        l = i + n2;

        T = read_q15x2 (pSrc + (2 * i));

        S = read_q15x2 (pSrc + (2 * l));

        R = __RV_KSUB16(T, S);

        write_q15x2 (pSrc + (2 * i), __RV_RADD16(T, S));

        out1 = __RV_SMDRS(coeff, R) >> 16;
        out2 = __RV_KMXDA(coeff, R);

        write_q15x2 (pSrc + (2 * l), (q31_t) ((out2) & 0xFFFF0000) | (out1 & 0x0000FFFF));

      } /* butterfly loop end */

    } /* groups loop end */

    twidCoefModifier = twidCoefModifier << 1U;
  } /* stages loop end */

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  /* loop for groups */
  for (j = 0; j < n2; j++)
  {
    coeff = read_q15x2 ((q15_t *)pCoef + (ia * 2U));

    ia = ia + twidCoefModifier;

    /* loop for butterfly */
    for (i = j; i < fftLen; i += n1)
    {
      l = i + n2;

      T = read_q15x2 (pSrc + (2 * i));

      S = read_q15x2 (pSrc + (2 * l));

      R = __RV_KSUB16(T, S);

      write_q15x2 (pSrc + (2 * i), __RV_KADD16(T, S));

      write_q15x2 (pSrc + (2 * l), R);

    } /* butterfly loop end */

  } /* groups loop end */

  twidCoefModifier = twidCoefModifier << 1U;

#else /* #if defined (RISCV_MATH_DSP) */

        uint32_t i, j, k, l;
        uint32_t n1, n2, ia;
        q15_t xt, yt, cosVal, sinVal;

  // N = fftLen;
  n2 = fftLen;

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  /* loop for groups */
  for (j = 0; j < n2; j++)
  {
    cosVal = pCoef[(ia * 2)];
    sinVal = pCoef[(ia * 2) + 1];
    ia = ia + twidCoefModifier;

    /* loop for butterfly */
    for (i = j; i < fftLen; i += n1)
    {
      l = i + n2;
      xt = (pSrc[2 * i] >> 1U) - (pSrc[2 * l] >> 1U);
      pSrc[2 * i] = ((pSrc[2 * i] >> 1U) + (pSrc[2 * l] >> 1U)) >> 1U;

      yt = (pSrc[2 * i + 1] >> 1U) - (pSrc[2 * l + 1] >> 1U);
      pSrc[2 * i + 1] = ((pSrc[2 * l + 1] >> 1U) +
                         (pSrc[2 * i + 1] >> 1U)  ) >> 1U;

      pSrc[2 * l] = (((int16_t) (((q31_t) xt * cosVal) >> 16)) -
                     ((int16_t) (((q31_t) yt * sinVal) >> 16)));

      pSrc[2 * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16)) +
                         ((int16_t) (((q31_t) xt * sinVal) >> 16)));

    } /* butterfly loop end */

  } /* groups loop end */

  twidCoefModifier = twidCoefModifier << 1U;

  /* loop for stage */
  for (k = fftLen / 2; k > 2; k = k >> 1)
  {
    n1 = n2;
    n2 = n2 >> 1;
    ia = 0;

    /* loop for groups */
    for (j = 0; j < n2; j++)
    {
      cosVal = pCoef[(ia * 2)];
      sinVal = pCoef[(ia * 2) + 1];
      ia = ia + twidCoefModifier;

      /* loop for butterfly */
      for (i = j; i < fftLen; i += n1)
      {
        l = i + n2;
        xt = pSrc[2 * i] - pSrc[2 * l];
        pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]) >> 1U;

        yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
        pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]) >> 1U;

        pSrc[2 * l] = (((int16_t) (((q31_t) xt * cosVal) >> 16)) -
                       ((int16_t) (((q31_t) yt * sinVal) >> 16))  );

        pSrc[2 * l + 1] = (((int16_t) (((q31_t) yt * cosVal) >> 16)) +
                           ((int16_t) (((q31_t) xt * sinVal) >> 16))  );

      } /* butterfly loop end */

    } /* groups loop end */

    twidCoefModifier = twidCoefModifier << 1U;
  } /* stages loop end */

  n1 = n2;
  n2 = n2 >> 1;
  ia = 0;

  cosVal = pCoef[(ia * 2)];
  sinVal = pCoef[(ia * 2) + 1];

  ia = ia + twidCoefModifier;

  /* loop for butterfly */
  for (i = 0; i < fftLen; i += n1)
  {
    l = i + n2;
    xt = pSrc[2 * i] - pSrc[2 * l];
    pSrc[2 * i] = (pSrc[2 * i] + pSrc[2 * l]);

    yt = pSrc[2 * i + 1] - pSrc[2 * l + 1];
    pSrc[2 * i + 1] = (pSrc[2 * l + 1] + pSrc[2 * i + 1]);

    pSrc[2 * l] = xt;

    pSrc[2 * l + 1] = yt;

  } /* groups loop end */


#endif /* #if defined (RISCV_MATH_DSP) */

}
