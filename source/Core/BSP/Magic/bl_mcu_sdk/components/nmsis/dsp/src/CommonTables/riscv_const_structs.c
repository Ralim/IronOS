/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_const_structs.c
 * Description:  Constant structs that are initialized for user convenience.
 *               For example, some can be given as arguments to the riscv_cfft_f32() or riscv_rfft_f32() functions.
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

#include "riscv_math_types.h"
#include "riscv_const_structs.h"

/*
ALLOW TABLE is true when config table is enabled and the Tramsform folder is included 
for compilation.
*/
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_FFT_ALLOW_TABLES)

/* Floating-point structs */
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_16) && defined(RISCV_TABLE_BITREVIDX_FLT64_16))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len16 = {
  16, (const float64_t *)twiddleCoefF64_16, riscvBitRevIndexTableF64_16, RISCVBITREVINDEXTABLEF64_16_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_32) && defined(RISCV_TABLE_BITREVIDX_FLT64_32))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len32 = {
  32, (const float64_t *)twiddleCoefF64_32, riscvBitRevIndexTableF64_32, RISCVBITREVINDEXTABLEF64_32_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_64) && defined(RISCV_TABLE_BITREVIDX_FLT64_64))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len64 = {
  64, (const float64_t *)twiddleCoefF64_64, riscvBitRevIndexTableF64_64, RISCVBITREVINDEXTABLEF64_64_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_128) && defined(RISCV_TABLE_BITREVIDX_FLT64_128))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len128 = {
  128, (const float64_t *)twiddleCoefF64_128, riscvBitRevIndexTableF64_128, RISCVBITREVINDEXTABLEF64_128_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_256) && defined(RISCV_TABLE_BITREVIDX_FLT64_256))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len256 = {
  256, (const float64_t *)twiddleCoefF64_256, riscvBitRevIndexTableF64_256, RISCVBITREVINDEXTABLEF64_256_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_512) && defined(RISCV_TABLE_BITREVIDX_FLT64_512))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len512 = {
  512, (const float64_t *)twiddleCoefF64_512, riscvBitRevIndexTableF64_512, RISCVBITREVINDEXTABLEF64_512_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_1024) && defined(RISCV_TABLE_BITREVIDX_FLT64_1024))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len1024 = {
  1024, (const float64_t *)twiddleCoefF64_1024, riscvBitRevIndexTableF64_1024, RISCVBITREVINDEXTABLEF64_1024_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_2048) && defined(RISCV_TABLE_BITREVIDX_FLT64_2048))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len2048 = {
  2048, (const float64_t *)twiddleCoefF64_2048, riscvBitRevIndexTableF64_2048, RISCVBITREVINDEXTABLEF64_2048_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_4096) && defined(RISCV_TABLE_BITREVIDX_FLT64_4096))
const riscv_cfft_instance_f64 riscv_cfft_sR_f64_len4096 = {
  4096, (const float64_t *)twiddleCoefF64_4096, riscvBitRevIndexTableF64_4096, RISCVBITREVINDEXTABLEF64_4096_TABLE_LENGTH
};
#endif

/* Floating-point structs */


#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_16) && defined(RISCV_TABLE_BITREVIDX_FLT_16))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len16 = {
  16, twiddleCoef_16, riscvBitRevIndexTable16, RISCVBITREVINDEXTABLE_16_TABLE_LENGTH
};
#endif


#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_32) && defined(RISCV_TABLE_BITREVIDX_FLT_32))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len32 = {
  32, twiddleCoef_32, riscvBitRevIndexTable32, RISCVBITREVINDEXTABLE_32_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_64) && defined(RISCV_TABLE_BITREVIDX_FLT_64))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len64 = {
  64, twiddleCoef_64, riscvBitRevIndexTable64, RISCVBITREVINDEXTABLE_64_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_128) && defined(RISCV_TABLE_BITREVIDX_FLT_128))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len128 = {
  128, twiddleCoef_128, riscvBitRevIndexTable128, RISCVBITREVINDEXTABLE_128_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_256) && defined(RISCV_TABLE_BITREVIDX_FLT_256))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len256 = {
  256, twiddleCoef_256, riscvBitRevIndexTable256, RISCVBITREVINDEXTABLE_256_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_512) && defined(RISCV_TABLE_BITREVIDX_FLT_512))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len512 = {
  512, twiddleCoef_512, riscvBitRevIndexTable512, RISCVBITREVINDEXTABLE_512_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_1024) && defined(RISCV_TABLE_BITREVIDX_FLT_1024))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len1024 = {
  1024, twiddleCoef_1024, riscvBitRevIndexTable1024, RISCVBITREVINDEXTABLE_1024_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_2048) && defined(RISCV_TABLE_BITREVIDX_FLT_2048))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len2048 = {
  2048, twiddleCoef_2048, riscvBitRevIndexTable2048, RISCVBITREVINDEXTABLE_2048_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_4096) && defined(RISCV_TABLE_BITREVIDX_FLT_4096))
const riscv_cfft_instance_f32 riscv_cfft_sR_f32_len4096 = {
  4096, twiddleCoef_4096, riscvBitRevIndexTable4096, RISCVBITREVINDEXTABLE_4096_TABLE_LENGTH
};
#endif


/* Fixed-point structs */


/* 

Those structures cannot be used to initialize the MVE version of the FFT Q31 instances.
So they are not compiled when MVE is defined.

For the MVE version, the new riscv_cfft_init_f32 must be used.


*/

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_16) && defined(RISCV_TABLE_BITREVIDX_FXT_16))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len16 = {
  16, twiddleCoef_16_q31, riscvBitRevIndexTable_fixed_16, RISCVBITREVINDEXTABLE_FIXED_16_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_32) && defined(RISCV_TABLE_BITREVIDX_FXT_32))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len32 = {
  32, twiddleCoef_32_q31, riscvBitRevIndexTable_fixed_32, RISCVBITREVINDEXTABLE_FIXED_32_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_64) && defined(RISCV_TABLE_BITREVIDX_FXT_64))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len64 = {
  64, twiddleCoef_64_q31, riscvBitRevIndexTable_fixed_64, RISCVBITREVINDEXTABLE_FIXED_64_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_128) && defined(RISCV_TABLE_BITREVIDX_FXT_128))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len128 = {
  128, twiddleCoef_128_q31, riscvBitRevIndexTable_fixed_128, RISCVBITREVINDEXTABLE_FIXED_128_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_256) && defined(RISCV_TABLE_BITREVIDX_FXT_256))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len256 = {
  256, twiddleCoef_256_q31, riscvBitRevIndexTable_fixed_256, RISCVBITREVINDEXTABLE_FIXED_256_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_512) && defined(RISCV_TABLE_BITREVIDX_FXT_512))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len512 = {
  512, twiddleCoef_512_q31, riscvBitRevIndexTable_fixed_512, RISCVBITREVINDEXTABLE_FIXED_512_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_1024) && defined(RISCV_TABLE_BITREVIDX_FXT_1024))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len1024 = {
  1024, twiddleCoef_1024_q31, riscvBitRevIndexTable_fixed_1024, RISCVBITREVINDEXTABLE_FIXED_1024_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_2048) && defined(RISCV_TABLE_BITREVIDX_FXT_2048))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len2048 = {
  2048, twiddleCoef_2048_q31, riscvBitRevIndexTable_fixed_2048, RISCVBITREVINDEXTABLE_FIXED_2048_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_4096) && defined(RISCV_TABLE_BITREVIDX_FXT_4096))
const riscv_cfft_instance_q31 riscv_cfft_sR_q31_len4096 = {
  4096, twiddleCoef_4096_q31, riscvBitRevIndexTable_fixed_4096, RISCVBITREVINDEXTABLE_FIXED_4096_TABLE_LENGTH
};
#endif


#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_16) && defined(RISCV_TABLE_BITREVIDX_FXT_16))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len16 = {
  16, twiddleCoef_16_q15, riscvBitRevIndexTable_fixed_16, RISCVBITREVINDEXTABLE_FIXED_16_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_32) && defined(RISCV_TABLE_BITREVIDX_FXT_32))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len32 = {
  32, twiddleCoef_32_q15, riscvBitRevIndexTable_fixed_32, RISCVBITREVINDEXTABLE_FIXED_32_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_64) && defined(RISCV_TABLE_BITREVIDX_FXT_64))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len64 = {
  64, twiddleCoef_64_q15, riscvBitRevIndexTable_fixed_64, RISCVBITREVINDEXTABLE_FIXED_64_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_128) && defined(RISCV_TABLE_BITREVIDX_FXT_128))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len128 = {
  128, twiddleCoef_128_q15, riscvBitRevIndexTable_fixed_128, RISCVBITREVINDEXTABLE_FIXED_128_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_256) && defined(RISCV_TABLE_BITREVIDX_FXT_256))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len256 = {
  256, twiddleCoef_256_q15, riscvBitRevIndexTable_fixed_256, RISCVBITREVINDEXTABLE_FIXED_256_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_512) && defined(RISCV_TABLE_BITREVIDX_FXT_512))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len512 = {
  512, twiddleCoef_512_q15, riscvBitRevIndexTable_fixed_512, RISCVBITREVINDEXTABLE_FIXED_512_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_1024) && defined(RISCV_TABLE_BITREVIDX_FXT_1024))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len1024 = {
  1024, twiddleCoef_1024_q15, riscvBitRevIndexTable_fixed_1024, RISCVBITREVINDEXTABLE_FIXED_1024_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_2048) && defined(RISCV_TABLE_BITREVIDX_FXT_2048))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len2048 = {
  2048, twiddleCoef_2048_q15, riscvBitRevIndexTable_fixed_2048, RISCVBITREVINDEXTABLE_FIXED_2048_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q15_4096) && defined(RISCV_TABLE_BITREVIDX_FXT_4096))
const riscv_cfft_instance_q15 riscv_cfft_sR_q15_len4096 = {
  4096, twiddleCoef_4096_q15, riscvBitRevIndexTable_fixed_4096, RISCVBITREVINDEXTABLE_FIXED_4096_TABLE_LENGTH
};
#endif


/* Structure for real-value inputs */
/* Double precision strucs */

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_32) && defined(RISCV_TABLE_BITREVIDX_FLT64_32) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_32))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len32 = {
  { 16, (const float64_t *)twiddleCoefF64_16, riscvBitRevIndexTableF64_16, RISCVBITREVINDEXTABLEF64_16_TABLE_LENGTH },
  32U,
  (float64_t *)twiddleCoefF64_rfft_32
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_64) && defined(RISCV_TABLE_BITREVIDX_FLT64_64) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_64))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len64 = {
   { 32, (const float64_t *)twiddleCoefF64_32, riscvBitRevIndexTableF64_32, RISCVBITREVINDEXTABLEF64_32_TABLE_LENGTH },
  64U,
  (float64_t *)twiddleCoefF64_rfft_64
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_128) && defined(RISCV_TABLE_BITREVIDX_FLT64_128) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_128))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len128 = {
  { 64, (const float64_t *)twiddleCoefF64_64, riscvBitRevIndexTableF64_64, RISCVBITREVINDEXTABLEF64_64_TABLE_LENGTH },
  128U,
  (float64_t *)twiddleCoefF64_rfft_128
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_256) && defined(RISCV_TABLE_BITREVIDX_FLT64_256) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_256))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len256 = {
  { 128, (const float64_t *)twiddleCoefF64_128, riscvBitRevIndexTableF64_128, RISCVBITREVINDEXTABLEF64_128_TABLE_LENGTH },
  256U,
  (float64_t *)twiddleCoefF64_rfft_256
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_512) && defined(RISCV_TABLE_BITREVIDX_FLT64_512) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_512))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len512 = {
  { 256, (const float64_t *)twiddleCoefF64_256, riscvBitRevIndexTableF64_256, RISCVBITREVINDEXTABLEF64_256_TABLE_LENGTH },
  512U,
  (float64_t *)twiddleCoefF64_rfft_512
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_1024) && defined(RISCV_TABLE_BITREVIDX_FLT64_1024) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_1024))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len1024 = {
  { 512, (const float64_t *)twiddleCoefF64_512, riscvBitRevIndexTableF64_512, RISCVBITREVINDEXTABLEF64_512_TABLE_LENGTH },
  1024U,
  (float64_t *)twiddleCoefF64_rfft_1024
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_2048) && defined(RISCV_TABLE_BITREVIDX_FLT64_2048) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_2048))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len2048 = {
  { 1024, (const float64_t *)twiddleCoefF64_1024, riscvBitRevIndexTableF64_1024, RISCVBITREVINDEXTABLEF64_1024_TABLE_LENGTH },
  2048U,
  (float64_t *)twiddleCoefF64_rfft_2048
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F64_4096) && defined(RISCV_TABLE_BITREVIDX_FLT64_4096) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F64_4096))
const riscv_rfft_fast_instance_f64 riscv_rfft_fast_sR_f64_len4096 = {
  { 2048, (const float64_t *)twiddleCoefF64_2048, riscvBitRevIndexTableF64_2048, RISCVBITREVINDEXTABLEF64_2048_TABLE_LENGTH },
  4096U,
  (float64_t *)twiddleCoefF64_rfft_4096
};
#endif

/* Floating-point structs */


#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_16) && defined(RISCV_TABLE_BITREVIDX_FLT_16) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_32))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len32 = {
  { 16, twiddleCoef_16, riscvBitRevIndexTable16, RISCVBITREVINDEXTABLE_16_TABLE_LENGTH },
  32U,
  (float32_t *)twiddleCoef_rfft_32
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_32) && defined(RISCV_TABLE_BITREVIDX_FLT_32) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_64))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len64 = {
   { 32, twiddleCoef_32, riscvBitRevIndexTable32, RISCVBITREVINDEXTABLE_32_TABLE_LENGTH },
  64U,
  (float32_t *)twiddleCoef_rfft_64
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_64) && defined(RISCV_TABLE_BITREVIDX_FLT_64) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_128))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len128 = {
  { 64, twiddleCoef_64, riscvBitRevIndexTable64, RISCVBITREVINDEXTABLE_64_TABLE_LENGTH },
  128U,
  (float32_t *)twiddleCoef_rfft_128
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_128) && defined(RISCV_TABLE_BITREVIDX_FLT_128) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_256))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len256 = {
  { 128, twiddleCoef_128, riscvBitRevIndexTable128, RISCVBITREVINDEXTABLE_128_TABLE_LENGTH },
  256U,
  (float32_t *)twiddleCoef_rfft_256
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_256) && defined(RISCV_TABLE_BITREVIDX_FLT_256) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_512))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len512 = {
  { 256, twiddleCoef_256, riscvBitRevIndexTable256, RISCVBITREVINDEXTABLE_256_TABLE_LENGTH },
  512U,
  (float32_t *)twiddleCoef_rfft_512
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_512) && defined(RISCV_TABLE_BITREVIDX_FLT_512) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_1024))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len1024 = {
  { 512, twiddleCoef_512, riscvBitRevIndexTable512, RISCVBITREVINDEXTABLE_512_TABLE_LENGTH },
  1024U,
  (float32_t *)twiddleCoef_rfft_1024
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_1024) && defined(RISCV_TABLE_BITREVIDX_FLT_1024) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_2048))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len2048 = {
  { 1024, twiddleCoef_1024, riscvBitRevIndexTable1024, RISCVBITREVINDEXTABLE_1024_TABLE_LENGTH },
  2048U,
  (float32_t *)twiddleCoef_rfft_2048
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_2048) && defined(RISCV_TABLE_BITREVIDX_FLT_2048) && defined(RISCV_TABLE_TWIDDLECOEF_RFFT_F32_4096))
const riscv_rfft_fast_instance_f32 riscv_rfft_fast_sR_f32_len4096 = {
  { 2048, twiddleCoef_2048, riscvBitRevIndexTable2048, RISCVBITREVINDEXTABLE_2048_TABLE_LENGTH },
  4096U,
  (float32_t *)twiddleCoef_rfft_4096
};
#endif


/* Fixed-point structs */
/* q31_t */


/* 

Those structures cannot be used to initialize the MVE version of the FFT Q31 instances.
So they are not compiled when MVE is defined.

For the MVE version, the new riscv_cfft_init_f32 must be used.


*/

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_16) && defined(RISCV_TABLE_BITREVIDX_FXT_16))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len32 = {
  32U,
  0,
  1,
  256U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len16
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_32) && defined(RISCV_TABLE_BITREVIDX_FXT_32))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len64 = {
  64U,
  0,
  1,
  128U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len32
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_64) && defined(RISCV_TABLE_BITREVIDX_FXT_64))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len128 = {
  128U,
  0,
  1,
  64U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len64
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_128) && defined(RISCV_TABLE_BITREVIDX_FXT_128))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len256 = {
  256U,
  0,
  1,
  32U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len128
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_256) && defined(RISCV_TABLE_BITREVIDX_FXT_256))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len512 = {
  512U,
  0,
  1,
  16U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len256
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_512) && defined(RISCV_TABLE_BITREVIDX_FXT_512))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len1024 = {
  1024U,
  0,
  1,
  8U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len512
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_1024) && defined(RISCV_TABLE_BITREVIDX_FXT_1024))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len2048 = {
  2048U,
  0,
  1,
  4U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len1024
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_2048) && defined(RISCV_TABLE_BITREVIDX_FXT_2048))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len4096 = {
  4096U,
  0,
  1,
  2U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len2048
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q31) && defined(RISCV_TABLE_TWIDDLECOEF_Q31_4096) && defined(RISCV_TABLE_BITREVIDX_FXT_4096))
const riscv_rfft_instance_q31 riscv_rfft_sR_q31_len8192 = {
  8192U,
  0,
  1,
  1U,
  (q31_t*)realCoefAQ31,
  (q31_t*)realCoefBQ31,
  &riscv_cfft_sR_q31_len4096
};
#endif



/* q15_t */
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_16) && defined(RISCV_TABLE_BITREVIDX_FXT_16))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len32 = {
  32U,
  0,
  1,
  256U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len16
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_32) && defined(RISCV_TABLE_BITREVIDX_FXT_32))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len64 = {
  64U,
  0,
  1,
  128U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len32
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_64) && defined(RISCV_TABLE_BITREVIDX_FXT_64))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len128 = {
  128U,
  0,
  1,
  64U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len64
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_128) && defined(RISCV_TABLE_BITREVIDX_FXT_128))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len256 = {
  256U,
  0,
  1,
  32U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len128
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_256) && defined(RISCV_TABLE_BITREVIDX_FXT_256))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len512 = {
  512U,
  0,
  1,
  16U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len256
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_512) && defined(RISCV_TABLE_BITREVIDX_FXT_512))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len1024 = {
  1024U,
  0,
  1,
  8U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len512
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_1024) && defined(RISCV_TABLE_BITREVIDX_FXT_1024))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len2048 = {
  2048U,
  0,
  1,
  4U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len1024
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_2048) && defined(RISCV_TABLE_BITREVIDX_FXT_2048))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len4096 = {
  4096U,
  0,
  1,
  2U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len2048
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_REALCOEF_Q15) && defined(RISCV_TABLE_TWIDDLECOEF_Q15_4096) && defined(RISCV_TABLE_BITREVIDX_FXT_4096))
const riscv_rfft_instance_q15 riscv_rfft_sR_q15_len8192 = {
  8192U,
  0,
  1,
  1U,
  (q15_t*)realCoefAQ15,
  (q15_t*)realCoefBQ15,
  &riscv_cfft_sR_q15_len4096
};
#endif



#endif
