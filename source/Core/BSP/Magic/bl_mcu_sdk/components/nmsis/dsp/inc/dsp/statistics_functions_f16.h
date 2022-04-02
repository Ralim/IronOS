/******************************************************************************
 * @file     statistics_functions_f16.h
 * @brief    Public header file for NMSIS DSP Library
 * @version  V1.9.0
 * @date     23 April 2021
 * Target Processor: RISC-V Cores
 ******************************************************************************/
/*
 * Copyright (c) 2010-2020 Arm Limited or its affiliates. All rights reserved.
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

 
#ifndef _STATISTICS_FUNCTIONS_F16_H_
#define _STATISTICS_FUNCTIONS_F16_H_

#include "riscv_math_types_f16.h"
#include "riscv_math_memory.h"

#include "dsp/none.h"
#include "dsp/utils.h"

#include "dsp/basic_math_functions_f16.h"
#include "dsp/fast_math_functions_f16.h"

#ifdef   __cplusplus
extern "C"
{
#endif

#if defined(RISCV_FLOAT16_SUPPORTED)

 /**
   * @brief  Sum of the squares of the elements of a floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[in]  blockSize  is the number of samples to process
   * @param[out] pResult    is output value.
   */
  void riscv_power_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult);

 /**
   * @brief  Mean value of a floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[in]  blockSize  is the number of samples to process
   * @param[out] pResult    is output value.
   */
  void riscv_mean_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult);

  /**
   * @brief  Variance of the elements of a floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[in]  blockSize  is the number of samples to process
   * @param[out] pResult    is output value.
   */
  void riscv_var_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult);

 /**
   * @brief  Root Mean Square of the elements of a floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[in]  blockSize  is the number of samples to process
   * @param[out] pResult    is output value.
   */
  void riscv_rms_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult);

 /**
   * @brief  Standard deviation of the elements of a floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[in]  blockSize  is the number of samples to process
   * @param[out] pResult    is output value.
   */
  void riscv_std_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult);

 /**
   * @brief  Minimum value of a floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[in]  blockSize  is the number of samples to process
   * @param[out] pResult    is output pointer
   * @param[out] pIndex     is the array index of the minimum value in the input buffer.
   */
  void riscv_min_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult,
        uint32_t * pIndex);

 /**
   * @brief  Minimum value of absolute values of a floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[in]  blockSize  is the number of samples to process
   * @param[out] pResult    is output pointer
   * @param[out] pIndex     is the array index of the minimum value in the input buffer.
   */
  void riscv_absmin_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult,
        uint32_t * pIndex);

/**
 * @brief Maximum value of a floating-point vector.
 * @param[in]  pSrc       points to the input buffer
 * @param[in]  blockSize  length of the input vector
 * @param[out] pResult    maximum value returned here
 * @param[out] pIndex     index of maximum value returned here
 */
  void riscv_max_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult,
        uint32_t * pIndex);

/**
 * @brief Maximum value of absolute values of a floating-point vector.
 * @param[in]  pSrc       points to the input buffer
 * @param[in]  blockSize  length of the input vector
 * @param[out] pResult    maximum value returned here
 * @param[out] pIndex     index of maximum value returned here
 */
  void riscv_absmax_f16(
  const float16_t * pSrc,
        uint32_t blockSize,
        float16_t * pResult,
        uint32_t * pIndex);

/**
 * @brief Entropy
 *
 * @param[in]  pSrcA        Array of input values.
 * @param[in]  blockSize    Number of samples in the input array.
 * @return     Entropy      -Sum(p ln p)
 *
 */


float16_t riscv_entropy_f16(const float16_t * pSrcA,uint32_t blockSize);

float16_t riscv_logsumexp_f16(const float16_t *in, uint32_t blockSize);

/**
 * @brief Dot product with log arithmetic
 *
 * Vectors are containing the log of the samples
 *
 * @param[in]       pSrcA points to the first input vector
 * @param[in]       pSrcB points to the second input vector
 * @param[in]       blockSize number of samples in each vector
 * @param[in]       pTmpBuffer temporary buffer of length blockSize
 * @return The log of the dot product .
 *
 */


float16_t riscv_logsumexp_dot_prod_f16(const float16_t * pSrcA,
  const float16_t * pSrcB,
  uint32_t blockSize,
  float16_t *pTmpBuffer);

/**
 * @brief Kullback-Leibler
 *
 * @param[in]  pSrcA         Pointer to an array of input values for probability distribution A.
 * @param[in]  pSrcB         Pointer to an array of input values for probability distribution B.
 * @param[in]  blockSize     Number of samples in the input array.
 * @return Kullback-Leibler  Divergence D(A || B)
 *
 */
float16_t riscv_kullback_leibler_f16(const float16_t * pSrcA
  ,const float16_t * pSrcB
  ,uint32_t blockSize);

/**
    @brief         Maximum value of a floating-point vector.
    @param[in]     pSrc       points to the input vector
    @param[in]     blockSize  number of samples in input vector
    @param[out]    pResult    maximum value returned here
    @return        none
   */
  void riscv_max_no_idx_f16(
      const float16_t *pSrc,
      uint32_t   blockSize,
      float16_t *pResult);



#endif /*defined(RISCV_FLOAT16_SUPPORTED)*/
#ifdef   __cplusplus
}
#endif

#endif /* ifndef _STATISTICS_FUNCTIONS_F16_H_ */
