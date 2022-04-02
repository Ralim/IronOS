/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_cfft_init_f32.c
 * Description:  Initialization function for cfft f32 instance
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

#define FFTINIT(EXT,SIZE)                                           \
  S->bitRevLength = riscv_cfft_sR_##EXT##_len##SIZE.bitRevLength;        \
  S->pBitRevTable = riscv_cfft_sR_##EXT##_len##SIZE.pBitRevTable;         \
  S->pTwiddle = riscv_cfft_sR_##EXT##_len##SIZE.pTwiddle;

/**
  @addtogroup ComplexFFT
  @{
 */

/**
  @brief         Initialization function for the cfft f32 function
  @param[in,out] S              points to an instance of the floating-point CFFT structure
  @param[in]     fftLen         fft length (number of complex samples)
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : an error is detected

  @par          Use of this function is mandatory only for the MVE version of the FFT.
                Other versions can still initialize directly the data structure using 
                variables declared in riscv_const_structs.h
 */

#include "dsp/transform_functions.h"
#include "riscv_common_tables.h"
#include "riscv_const_structs.h"

riscv_status riscv_cfft_init_f32(
  riscv_cfft_instance_f32 * S,
  uint16_t fftLen)
{
        /*  Initialise the default riscv status */
        riscv_status status = RISCV_MATH_SUCCESS;

        /*  Initialise the FFT length */
        S->fftLen = fftLen;

        /*  Initialise the Twiddle coefficient pointer */
        S->pTwiddle = NULL;


        /*  Initializations of Instance structure depending on the FFT length */
        switch (S->fftLen) {
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_4096) && defined(RISCV_TABLE_BITREVIDX_FLT_4096))
            /*  Initializations of structure parameters for 4096 point FFT */
        case 4096U:
            /*  Initialise the bit reversal table modifier */
            FFTINIT(f32,4096);
            break;
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_2048) && defined(RISCV_TABLE_BITREVIDX_FLT_2048))
            /*  Initializations of structure parameters for 2048 point FFT */
        case 2048U:
            /*  Initialise the bit reversal table modifier */
            FFTINIT(f32,2048);

            break;
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_1024) && defined(RISCV_TABLE_BITREVIDX_FLT_1024))
            /*  Initializations of structure parameters for 1024 point FFT */
        case 1024U:
            /*  Initialise the bit reversal table modifier */
            FFTINIT(f32,1024);

            break;
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_512) && defined(RISCV_TABLE_BITREVIDX_FLT_512))
            /*  Initializations of structure parameters for 512 point FFT */
        case 512U:
            /*  Initialise the bit reversal table modifier */
            FFTINIT(f32,512);
            break;
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_256) && defined(RISCV_TABLE_BITREVIDX_FLT_256))
        case 256U:
            FFTINIT(f32,256);
            break;
#endif

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_128) && defined(RISCV_TABLE_BITREVIDX_FLT_128))
        case 128U:
            FFTINIT(f32,128);
            break;
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_64) && defined(RISCV_TABLE_BITREVIDX_FLT_64))
        case 64U:
            FFTINIT(f32,64);
            break;
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_32) && defined(RISCV_TABLE_BITREVIDX_FLT_32))
        case 32U:
            FFTINIT(f32,32);
            break;
#endif 

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_F32_16) && defined(RISCV_TABLE_BITREVIDX_FLT_16))
        case 16U:
            /*  Initializations of structure parameters for 16 point FFT */
            FFTINIT(f32,16);
            break;
#endif

        default:
            /*  Reporting argument error if fftSize is not valid value */
            status = RISCV_MATH_ARGUMENT_ERROR;
            break;
        }


        return (status);
}

/**
  @} end of ComplexFFT group
 */
