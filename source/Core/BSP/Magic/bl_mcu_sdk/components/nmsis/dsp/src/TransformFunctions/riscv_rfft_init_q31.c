/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_rfft_init_q31.c
 * Description:  RFFT & RIFFT Q31 initialisation function
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
#include "riscv_common_tables.h"
#include "riscv_const_structs.h"



/**
  @addtogroup RealFFT
  @{
 */

/**
  @brief         Initialization function for the Q31 RFFT/RIFFT.
  @param[in,out] S              points to an instance of the Q31 RFFT/RIFFT structure
  @param[in]     fftLenReal     length of the FFT
  @param[in]     ifftFlagR      flag that selects transform direction
                   - value = 0: forward transform
                   - value = 1: inverse transform
  @param[in]     bitReverseFlag flag that enables / disables bit reversal of output
                   - value = 0: disables bit reversal of output
                   - value = 1: enables bit reversal of output
  @return        execution status
                   - \ref RISCV_MATH_SUCCESS        : Operation successful
                   - \ref RISCV_MATH_ARGUMENT_ERROR : <code>fftLenReal</code> is not a supported length

  @par           Details
                   The parameter <code>fftLenReal</code> specifies length of RFFT/RIFFT Process.
                   Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192.
  @par
                   The parameter <code>ifftFlagR</code> controls whether a forward or inverse transform is computed.
                   Set(=1) ifftFlagR to calculate RIFFT, otherwise RFFT is calculated.
  @par
                   The parameter <code>bitReverseFlag</code> controls whether output is in normal order or bit reversed order.
                   Set(=1) bitReverseFlag for output to be in normal order otherwise output is in bit reversed order.
  @par
                   This function also initializes Twiddle factor table.
*/

riscv_status riscv_rfft_init_q31(
    riscv_rfft_instance_q31 * S,
    uint32_t fftLenReal,
    uint32_t ifftFlagR,
    uint32_t bitReverseFlag)
{
     /*  Initialise the default riscv status */
    riscv_status status = RISCV_MATH_ARGUMENT_ERROR;

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_FFT_ALLOW_TABLES)

#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || defined(RISCV_TABLE_REALCOEF_Q31)

    /*  Initialise the default riscv status */
    status = RISCV_MATH_SUCCESS;

    /*  Initialize the Real FFT length */
    S->fftLenReal = (uint16_t) fftLenReal;

    /*  Initialize the Twiddle coefficientA pointer */
    S->pTwiddleAReal = (q31_t *) realCoefAQ31;

    /*  Initialize the Twiddle coefficientB pointer */
    S->pTwiddleBReal = (q31_t *) realCoefBQ31;

    /*  Initialize the Flag for selection of RFFT or RIFFT */
    S->ifftFlagR = (uint8_t) ifftFlagR;

    /*  Initialize the Flag for calculation Bit reversal or not */
    S->bitReverseFlagR = (uint8_t) bitReverseFlag;

    /*  Initialization of coef modifier depending on the FFT length */
    switch (S->fftLenReal)
    {
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_4096) && defined(RISCV_TABLE_BITREVIDX_FXT_4096))
    case 8192U:


        S->twidCoefRModifier = 1U;

           S->pCfft = &riscv_cfft_sR_q31_len4096;
        break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_2048) && defined(RISCV_TABLE_BITREVIDX_FXT_2048))
    case 4096U:
        S->twidCoefRModifier = 2U;

           S->pCfft = &riscv_cfft_sR_q31_len2048;
        break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_1024) && defined(RISCV_TABLE_BITREVIDX_FXT_1024))
    case 2048U:
        S->twidCoefRModifier = 4U;

          S->pCfft = &riscv_cfft_sR_q31_len1024;
        break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_512) && defined(RISCV_TABLE_BITREVIDX_FXT_512))
    case 1024U:
        S->twidCoefRModifier = 8U;
           S->pCfft = &riscv_cfft_sR_q31_len512;
        break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_256) && defined(RISCV_TABLE_BITREVIDX_FXT_256))
    case 512U:
        S->twidCoefRModifier = 16U;
           S->pCfft = &riscv_cfft_sR_q31_len256;
        break;
#endif 
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_128) && defined(RISCV_TABLE_BITREVIDX_FXT_128))
    case 256U:
        S->twidCoefRModifier = 32U;
           S->pCfft = &riscv_cfft_sR_q31_len128;
        break;
#endif 
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_64) && defined(RISCV_TABLE_BITREVIDX_FXT_64))
    case 128U:
        S->twidCoefRModifier = 64U;
           S->pCfft = &riscv_cfft_sR_q31_len64;
        break;
#endif
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_32) && defined(RISCV_TABLE_BITREVIDX_FXT_32))
    case 64U:
        S->twidCoefRModifier = 128U;
           S->pCfft = &riscv_cfft_sR_q31_len32;
        break;
#endif 
#if !defined(RISCV_DSP_CONFIG_TABLES) || defined(RISCV_ALL_FFT_TABLES) || (defined(RISCV_TABLE_TWIDDLECOEF_Q31_16) && defined(RISCV_TABLE_BITREVIDX_FXT_16))
    case 32U:
        S->twidCoefRModifier = 256U;
           S->pCfft = &riscv_cfft_sR_q31_len16;
        break;
#endif
    default:
        /*  Reporting argument error if rfftSize is not valid value */
        status = RISCV_MATH_ARGUMENT_ERROR;
        break;
    }

#endif
#endif
    /* return the status of RFFT Init function */
    return (status);
}

/**
  @} end of RealFFT group
 */
