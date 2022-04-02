/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_rfft_fast_init_f16.c
 * Description:  Split Radix Decimation in Frequency CFFT Floating point processing function
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
#include "riscv_const_structs_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)

/**
  @ingroup groupTransforms
 */

/**
  @addtogroup RealFFT
  @{
 */

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_16) && defined(RISCV_TABLE_BITREVIDX_FLT_16) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_32))

/**
  @private
  @brief         Initialization function for the 32pt floating-point real FFT.
  @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
 */

static riscv_status riscv_rfft_32_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),16);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }

  S->fftLenRFFT = 32U;
  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_32;

  return RISCV_MATH_SUCCESS;
}
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_32) && defined(RISCV_TABLE_BITREVIDX_FLT_32) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_64))

/**
  @private
  @brief         Initialization function for the 64pt floating-point real FFT.
  @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
 */

static riscv_status riscv_rfft_64_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),32);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }
  S->fftLenRFFT = 64U;

  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_64;

  return RISCV_MATH_SUCCESS;
}
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_64) && defined(RISCV_TABLE_BITREVIDX_FLT_64) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_128))

/**
  @private
  @brief         Initialization function for the 128pt floating-point real FFT.
  @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
 */

static riscv_status riscv_rfft_128_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),64);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }
  S->fftLenRFFT = 128;

  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_128;

  return RISCV_MATH_SUCCESS;
}
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_128) && defined(RISCV_TABLE_BITREVIDX_FLT_128) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_256))

/**
  @private
  @brief         Initialization function for the 256pt floating-point real FFT.
  @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
*/

static riscv_status riscv_rfft_256_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),128);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }
  S->fftLenRFFT = 256U;

  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_256;

  return RISCV_MATH_SUCCESS;
}
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_256) && defined(RISCV_TABLE_BITREVIDX_FLT_256) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_512))

/**
  @private
  @brief         Initialization function for the 512pt floating-point real FFT.
  @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
 */

static riscv_status riscv_rfft_512_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),256);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }
  S->fftLenRFFT = 512U;

  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_512;

  return RISCV_MATH_SUCCESS;
}
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_512) && defined(RISCV_TABLE_BITREVIDX_FLT_512) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_1024))
/**
  @private
  @brief         Initialization function for the 1024pt floating-point real FFT.
  @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
 */

static riscv_status riscv_rfft_1024_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),512);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }
  S->fftLenRFFT = 1024U;

  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_1024;

  return RISCV_MATH_SUCCESS;
}
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_1024) && defined(RISCV_TABLE_BITREVIDX_FLT_1024) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_2048))
/**
  @private
  @brief         Initialization function for the 2048pt floating-point real FFT.
  @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
 */
static riscv_status riscv_rfft_2048_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),1024);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }
  S->fftLenRFFT = 2048U;

  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_2048;

  return RISCV_MATH_SUCCESS;
}
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_2048) && defined(RISCV_TABLE_BITREVIDX_FLT_2048) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_4096))
/**
  @private
* @brief         Initialization function for the 4096pt floating-point real FFT.
* @param[in,out] S  points to an riscv_rfft_fast_instance_f16 structure
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected
 */

static riscv_status riscv_rfft_4096_fast_init_f16( riscv_rfft_fast_instance_f16 * S ) {

  riscv_status status;

  if( !S ) return RISCV_MATH_ARGUMENT_ERROR;

  status=riscv_cfft_init_f16(&(S->Sint),2048);
  if (status != RISCV_MATH_SUCCESS)
  {
    return(status);
  }
  S->fftLenRFFT = 4096U;

  S->pTwiddleRFFT    = (float16_t *) twiddleCoefF16_rfft_4096;

  return RISCV_MATH_SUCCESS;
}
#endif 

/**
  @brief         Initialization function for the floating-point real FFT.
  @param[in,out] S       points to an riscv_rfft_fast_instance_f16 structure
  @param[in]     fftLen  length of the Real Sequence
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : <code>fftLen</code> is not a supported length

  @par           Description
                   The parameter <code>fftLen</code> specifies the length of RFFT/CIFFT process.
                   Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
  @par
                   This Function also initializes Twiddle factor table pointer and Bit reversal table pointer.
 */

riscv_status riscv_rfft_fast_init_f16(
  riscv_rfft_fast_instance_f16 * S,
  uint16_t fftLen)
{
  typedef riscv_status(*fft_init_ptr)( riscv_rfft_fast_instance_f16 *);
  fft_init_ptr fptr = 0x0;

  switch (fftLen)
  {
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_2048) && defined(RISCV_TABLE_BITREVIDX_FLT_2048) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_4096))
  case 4096U:
    fptr = riscv_rfft_4096_fast_init_f16;
    break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_1024) && defined(RISCV_TABLE_BITREVIDX_FLT_1024) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_2048))
  case 2048U:
    fptr = riscv_rfft_2048_fast_init_f16;
    break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_512) && defined(RISCV_TABLE_BITREVIDX_FLT_512) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_1024))
  case 1024U:
    fptr = riscv_rfft_1024_fast_init_f16;
    break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_256) && defined(RISCV_TABLE_BITREVIDX_FLT_256) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_512))
  case 512U:
    fptr = riscv_rfft_512_fast_init_f16;
    break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_128) && defined(RISCV_TABLE_BITREVIDX_FLT_128) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_256))
  case 256U:
    fptr = riscv_rfft_256_fast_init_f16;
    break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_64) && defined(RISCV_TABLE_BITREVIDX_FLT_64) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_128))
  case 128U:
    fptr = riscv_rfft_128_fast_init_f16;
    break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_32) && defined(RISCV_TABLE_BITREVIDX_FLT_32) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_64))
  case 64U:
    fptr = riscv_rfft_64_fast_init_f16;
    break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_16) && defined(RISCV_TABLE_BITREVIDX_FLT_16) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F16_32))
  case 32U:
    fptr = riscv_rfft_32_fast_init_f16;
    break;
#endif
  default:
    return RISCV_MATH_ARGUMENT_ERROR;
  }

  if( ! fptr ) return RISCV_MATH_ARGUMENT_ERROR;
  return fptr( S );

}

/**
  @} end of RealFFT group
 */

#endif /*  #if defined(RISCV_FLOAT16_SUPPORTED) */
