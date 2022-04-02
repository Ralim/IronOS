/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_const_structs_f16.c
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

#include "riscv_math_types_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)

#include "riscv_const_structs_f16.h"


/*
ALLOW TABLE is true when config table is enabled and the Tramsform folder is included 
for compilation.
*/
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_FFT_ALLOW_TABLES)


/* Floating-point structs */


/* 

Those structures cannot be used to initialize the MVE version of the FFT F32 instances.
So they are not compiled when MVE is defined.

For the MVE version, the new riscv_cfft_init_f16 must be used.


*/

 
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_16) && defined(RISCV_TABLE_BITREVIDX_FLT_16))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len16 = {
  16, twiddleCoefF16_16, riscvBitRevIndexTable_fixed_16, RISCVBITREVINDEXTABLE_FIXED_16_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_32) && defined(RISCV_TABLE_BITREVIDX_FLT_32))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len32 = {
  32, twiddleCoefF16_32, riscvBitRevIndexTable_fixed_32, RISCVBITREVINDEXTABLE_FIXED_32_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_64) && defined(RISCV_TABLE_BITREVIDX_FLT_64))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len64 = {
  64, twiddleCoefF16_64, riscvBitRevIndexTable_fixed_64, RISCVBITREVINDEXTABLE_FIXED_64_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_128) && defined(RISCV_TABLE_BITREVIDX_FLT_128))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len128 = {
  128, twiddleCoefF16_128, riscvBitRevIndexTable_fixed_128, RISCVBITREVINDEXTABLE_FIXED_128_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_256) && defined(RISCV_TABLE_BITREVIDX_FLT_256))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len256 = {
  256, twiddleCoefF16_256, riscvBitRevIndexTable_fixed_256, RISCVBITREVINDEXTABLE_FIXED_256_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_512) && defined(RISCV_TABLE_BITREVIDX_FLT_512))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len512 = {
  512, twiddleCoefF16_512, riscvBitRevIndexTable_fixed_512, RISCVBITREVINDEXTABLE_FIXED_512_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_1024) && defined(RISCV_TABLE_BITREVIDX_FLT_1024))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len1024 = {
  1024, twiddleCoefF16_1024, riscvBitRevIndexTable_fixed_1024, RISCVBITREVINDEXTABLE_FIXED_1024_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_2048) && defined(RISCV_TABLE_BITREVIDX_FLT_2048))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len2048 = {
  2048, twiddleCoefF16_2048, riscvBitRevIndexTable_fixed_2048, RISCVBITREVINDEXTABLE_FIXED_2048_TABLE_LENGTH
};
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F16_4096) && defined(RISCV_TABLE_BITREVIDX_FLT_4096))
const riscv_cfft_instance_f16 riscv_cfft_sR_f16_len4096 = {
  4096, twiddleCoefF16_4096, riscvBitRevIndexTable_fixed_4096, RISCVBITREVINDEXTABLE_FIXED_4096_TABLE_LENGTH
};
#endif



#endif

#endif
