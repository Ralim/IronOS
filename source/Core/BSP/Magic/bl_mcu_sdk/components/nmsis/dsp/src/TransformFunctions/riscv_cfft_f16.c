/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cfft_f32.c
 * Description:  Combined Radix Decimation in Frequency CFFT Floating point processing function
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

#include "dsp/transform_functions_f16.h"
#include "riscv_common_tables_f16.h"



#if defined(RISCV_FLOAT16_SUPPORTED)

extern void riscv_bitreversal_16(
        uint16_t * pSrc,
  const uint16_t bitRevLen,
  const uint16_t * pBitRevTable);


extern void riscv_cfft_radix4by2_f16(
    float16_t * pSrc,
    uint32_t fftLen,
    const float16_t * pCoef);

extern void riscv_radix4_butterfly_f16(
        float16_t * pSrc,
        uint16_t fftLen,
  const float16_t * pCoef,
        uint16_t twidCoefModifier);

/**
  @ingroup groupTransforms
 */

/**
  @defgroup ComplexFFT Complex FFT Functions

  @par
                   The Fast Fourier Transform (FFT) is an efficient algorithm for computing the
                   Discrete Fourier Transform (DFT).  The FFT can be orders of magnitude faster
                   than the DFT, especially for long lengths.
                   The algorithms described in this section
                   operate on complex data.  A separate set of functions is devoted to handling
                   of real sequences.
  @par
                   There are separate algorithms for handling floating-point, Q15, and Q31 data
                   types.  The algorithms available for each data type are described next.
  @par
                   The FFT functions operate in-place.  That is, the array holding the input data
                   will also be used to hold the corresponding result.  The input data is complex
                   and contains <code>2*fftLen</code> interleaved values as shown below.
                   <pre>{real[0], imag[0], real[1], imag[1], ...} </pre>
                   The FFT result will be contained in the same array and the frequency domain
                   values will have the same interleaving.

  @par Floating-point
                   The floating-point complex FFT uses a mixed-radix algorithm.  Multiple radix-8
                   stages are performed along with a single radix-2 or radix-4 stage, as needed.
                   The algorithm supports lengths of [16, 32, 64, ..., 4096] and each length uses
                   a different twiddle factor table.
  @par
                   The function uses the standard FFT definition and output values may grow by a
                   factor of <code>fftLen</code> when computing the forward transform.  The
                   inverse transform includes a scale of <code>1/fftLen</code> as part of the
                   calculation and this matches the textbook definition of the inverse FFT.
  @par
                   For the MVE version, the new riscv_cfft_init_f32 initialization function is
                   <b>mandatory</b>. <b>Compilation flags are available to include only the required tables for the
                   needed FFTs.</b> Other FFT versions can continue to be initialized as
                   explained below.
  @par
                   For not MVE versions, pre-initialized data structures containing twiddle factors
                   and bit reversal tables are provided and defined in <code>riscv_const_structs.h</code>.  Include
                   this header in your function and then pass one of the constant structures as
                   an argument to riscv_cfft_f32.  For example:
  @par
                   <code>riscv_cfft_f32(riscv_cfft_sR_f32_len64, pSrc, 1, 1)</code>
  @par
                   computes a 64-point inverse complex FFT including bit reversal.
                   The data structures are treated as constant data and not modified during the
                   calculation.  The same data structure can be reused for multiple transforms
                   including mixing forward and inverse transforms.
  @par
                   Earlier releases of the library provided separate radix-2 and radix-4
                   algorithms that operated on floating-point data.  These functions are still
                   provided but are deprecated.  The older functions are slower and less general
                   than the new functions.
  @par
                   An example of initialization of the constants for the riscv_cfft_f32 function follows:
  @code
                   const static riscv_cfft_instance_f32 *S;
                   ...
                     switch (length) {
                       case 16:
                         S = &riscv_cfft_sR_f32_len16;
                         break;
                       case 32:
                         S = &riscv_cfft_sR_f32_len32;
                         break;
                       case 64:
                         S = &riscv_cfft_sR_f32_len64;
                         break;
                       case 128:
                         S = &riscv_cfft_sR_f32_len128;
                         break;
                       case 256:
                         S = &riscv_cfft_sR_f32_len256;
                         break;
                       case 512:
                         S = &riscv_cfft_sR_f32_len512;
                         break;
                       case 1024:
                         S = &riscv_cfft_sR_f32_len1024;
                         break;
                       case 2048:
                         S = &riscv_cfft_sR_f32_len2048;
                         break;
                       case 4096:
                         S = &riscv_cfft_sR_f32_len4096;
                         break;
                     }
  @endcode
  @par
                   The new riscv_cfft_init_f32 can also be used.
  @par Q15 and Q31
                   The floating-point complex FFT uses a mixed-radix algorithm.  Multiple radix-4
                   stages are performed along with a single radix-2 stage, as needed.
                   The algorithm supports lengths of [16, 32, 64, ..., 4096] and each length uses
                   a different twiddle factor table.
  @par
                   The function uses the standard FFT definition and output values may grow by a
                   factor of <code>fftLen</code> when computing the forward transform.  The
                   inverse transform includes a scale of <code>1/fftLen</code> as part of the
                   calculation and this matches the textbook definition of the inverse FFT.
  @par
                   Pre-initialized data structures containing twiddle factors and bit reversal
                   tables are provided and defined in <code>riscv_const_structs.h</code>.  Include
                   this header in your function and then pass one of the constant structures as
                   an argument to riscv_cfft_q31. For example:
  @par
                   <code>riscv_cfft_q31(riscv_cfft_sR_q31_len64, pSrc, 1, 1)</code>
  @par
                   computes a 64-point inverse complex FFT including bit reversal.
                   The data structures are treated as constant data and not modified during the
                   calculation.  The same data structure can be reused for multiple transforms
                   including mixing forward and inverse transforms.
  @par
                   Earlier releases of the library provided separate radix-2 and radix-4
                   algorithms that operated on floating-point data.  These functions are still
                   provided but are deprecated.  The older functions are slower and less general
                   than the new functions.
  @par
                   An example of initialization of the constants for the riscv_cfft_q31 function follows:
  @code
                   const static riscv_cfft_instance_q31 *S;
                   ...
                     switch (length) {
                       case 16:
                         S = &riscv_cfft_sR_q31_len16;
                         break;
                       case 32:
                         S = &riscv_cfft_sR_q31_len32;
                         break;
                       case 64:
                         S = &riscv_cfft_sR_q31_len64;
                         break;
                       case 128:
                         S = &riscv_cfft_sR_q31_len128;
                         break;
                       case 256:
                         S = &riscv_cfft_sR_q31_len256;
                         break;
                       case 512:
                         S = &riscv_cfft_sR_q31_len512;
                         break;
                       case 1024:
                         S = &riscv_cfft_sR_q31_len1024;
                         break;
                       case 2048:
                         S = &riscv_cfft_sR_q31_len2048;
                         break;
                       case 4096:
                         S = &riscv_cfft_sR_q31_len4096;
                         break;
                     }
  @endcode

 */


/**
  @addtogroup ComplexFFT
  @{
 */

/**
  @brief         Processing function for the floating-point complex FFT.
  @param[in]     S              points to an instance of the floating-point CFFT structure
  @param[in,out] p1             points to the complex data buffer of size <code>2*fftLen</code>. Processing occurs in-place
  @param[in]     ifftFlag       flag that selects transform direction
                   - value = 0: forward transform
                   - value = 1: inverse transform
  @param[in]     bitReverseFlag flag that enables / disables bit reversal of output
                   - value = 0: disables bit reversal of output
                   - value = 1: enables bit reversal of output
  @return        none
 */

void riscv_cfft_f16(
    const riscv_cfft_instance_f16 * S,
    float16_t * p1,
    uint8_t ifftFlag,
    uint8_t bitReverseFlag)
{
    uint32_t  L = S->fftLen, l;
    float16_t invL, * pSrc;

    if (ifftFlag == 1U)
    {
        /*  Conjugate input data  */
        pSrc = p1 + 1;
        for(l=0; l<L; l++)
        {
            *pSrc = -*pSrc;
            pSrc += 2;
        }
    }

    switch (L)
    {

        case 16:
        case 64:
        case 256:
        case 1024:
        case 4096:
        riscv_radix4_butterfly_f16  (p1, L, (float16_t*)S->pTwiddle, 1U);
        break;

        case 32:
        case 128:
        case 512:
        case 2048:
        riscv_cfft_radix4by2_f16  ( p1, L, (float16_t*)S->pTwiddle);
        break;

    }

    if ( bitReverseFlag )
        riscv_bitreversal_16((uint16_t*)p1, S->bitRevLength,(uint16_t*)S->pBitRevTable);

    if (ifftFlag == 1U)
    {
        invL = 1.0f/(float16_t)L;
        /*  Conjugate and scale output data */
        pSrc = p1;
        for(l=0; l<L; l++)
        {
            *pSrc++ *=   invL ;
            *pSrc  = -(*pSrc) * invL;
            pSrc++;
        }
    }
}
#endif /* if defined(RISCV_FLOAT16_SUPPORTED) */

/**
  @} end of ComplexFFT group
 */
