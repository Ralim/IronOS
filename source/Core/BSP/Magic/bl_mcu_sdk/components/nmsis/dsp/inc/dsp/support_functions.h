/******************************************************************************
 * @file     support_functions.h
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

 
#ifndef _SUPPORT_FUNCTIONS_H_
#define _SUPPORT_FUNCTIONS_H_

#include "riscv_math_types.h"
#include "riscv_math_memory.h"

#include "dsp/none.h"
#include "dsp/utils.h"

#ifdef   __cplusplus
extern "C"
{
#endif

/**
 * @defgroup groupSupport Support Functions
 */


/**
   * @brief Converts the elements of the floating-point vector to Q31 vector.
   * @param[in]  pSrc       points to the floating-point input vector
   * @param[out] pDst       points to the Q31 output vector
   * @param[in]  blockSize  length of the input vector
   */
  void riscv_float_to_q31(
  const float32_t * pSrc,
        q31_t * pDst,
        uint32_t blockSize);


  /**
   * @brief Converts the elements of the floating-point vector to Q15 vector.
   * @param[in]  pSrc       points to the floating-point input vector
   * @param[out] pDst       points to the Q15 output vector
   * @param[in]  blockSize  length of the input vector
   */
  void riscv_float_to_q15(
  const float32_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize);


  /**
   * @brief Converts the elements of the floating-point vector to Q7 vector.
   * @param[in]  pSrc       points to the floating-point input vector
   * @param[out] pDst       points to the Q7 output vector
   * @param[in]  blockSize  length of the input vector
   */
  void riscv_float_to_q7(
  const float32_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q31 vector to floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[out] pDst       is output pointer
   * @param[in]  blockSize  is the number of samples to process
   */
  void riscv_q31_to_float(
  const q31_t * pSrc,
        float32_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q31 vector to Q15 vector.
   * @param[in]  pSrc       is input pointer
   * @param[out] pDst       is output pointer
   * @param[in]  blockSize  is the number of samples to process
   */
  void riscv_q31_to_q15(
  const q31_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q31 vector to Q7 vector.
   * @param[in]  pSrc       is input pointer
   * @param[out] pDst       is output pointer
   * @param[in]  blockSize  is the number of samples to process
   */
  void riscv_q31_to_q7(
  const q31_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q15 vector to floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[out] pDst       is output pointer
   * @param[in]  blockSize  is the number of samples to process
   */
  void riscv_q15_to_float(
  const q15_t * pSrc,
        float32_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q15 vector to Q31 vector.
   * @param[in]  pSrc       is input pointer
   * @param[out] pDst       is output pointer
   * @param[in]  blockSize  is the number of samples to process
   */
  void riscv_q15_to_q31(
  const q15_t * pSrc,
        q31_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q15 vector to Q7 vector.
   * @param[in]  pSrc       is input pointer
   * @param[out] pDst       is output pointer
   * @param[in]  blockSize  is the number of samples to process
   */
  void riscv_q15_to_q7(
  const q15_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q7 vector to floating-point vector.
   * @param[in]  pSrc       is input pointer
   * @param[out] pDst       is output pointer
   * @param[in]  blockSize  is the number of samples to process
   */
  void riscv_q7_to_float(
  const q7_t * pSrc,
        float32_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q7 vector to Q31 vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_q7_to_q31(
  const q7_t * pSrc,
        q31_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Converts the elements of the Q7 vector to Q15 vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_q7_to_q15(
  const q7_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize);




  
  /**
   * @brief Struct for specifying sorting algorithm
   */
  typedef enum
  {
    RISCV_SORT_BITONIC   = 0,
             /**< Bitonic sort   */
    RISCV_SORT_BUBBLE    = 1,
             /**< Bubble sort    */
    RISCV_SORT_HEAP      = 2,
             /**< Heap sort      */
    RISCV_SORT_INSERTION = 3,
             /**< Insertion sort */
    RISCV_SORT_QUICK     = 4,
             /**< Quick sort     */
    RISCV_SORT_SELECTION = 5
             /**< Selection sort */
  } riscv_sort_alg;

  /**
   * @brief Struct for specifying sorting algorithm
   */
  typedef enum
  {
    RISCV_SORT_DESCENDING = 0,
             /**< Descending order (9 to 0) */
    RISCV_SORT_ASCENDING = 1
             /**< Ascending order (0 to 9) */
  } riscv_sort_dir;

  /**
   * @brief Instance structure for the sorting algorithms.
   */
  typedef struct            
  {
    riscv_sort_alg alg;        /**< Sorting algorithm selected */
    riscv_sort_dir dir;        /**< Sorting order (direction)  */
  } riscv_sort_instance_f32;  

  /**
   * @param[in]  S          points to an instance of the sorting structure.
   * @param[in]  pSrc       points to the block of input data.
   * @param[out] pDst       points to the block of output data.
   * @param[in]  blockSize  number of samples to process.
   */
  void riscv_sort_f32(
    const riscv_sort_instance_f32 * S, 
          float32_t * pSrc, 
          float32_t * pDst, 
          uint32_t blockSize);

  /**
   * @param[in,out]  S            points to an instance of the sorting structure.
   * @param[in]      alg          Selected algorithm.
   * @param[in]      dir          Sorting order.
   */
  void riscv_sort_init_f32(
    riscv_sort_instance_f32 * S, 
    riscv_sort_alg alg, 
    riscv_sort_dir dir); 

  /**
   * @brief Instance structure for the sorting algorithms.
   */
  typedef struct            
  {
    riscv_sort_dir dir;        /**< Sorting order (direction)  */
    float32_t * buffer;      /**< Working buffer */
  } riscv_merge_sort_instance_f32;  

  /**
   * @param[in]      S          points to an instance of the sorting structure.
   * @param[in,out]  pSrc       points to the block of input data.
   * @param[out]     pDst       points to the block of output data
   * @param[in]      blockSize  number of samples to process.
   */
  void riscv_merge_sort_f32(
    const riscv_merge_sort_instance_f32 * S,
          float32_t *pSrc,
          float32_t *pDst,
          uint32_t blockSize);

  /**
   * @param[in,out]  S            points to an instance of the sorting structure.
   * @param[in]      dir          Sorting order.
   * @param[in]      buffer       Working buffer.
   */
  void riscv_merge_sort_init_f32(
    riscv_merge_sort_instance_f32 * S,
    riscv_sort_dir dir,
    float32_t * buffer);

 
 
  /**
   * @brief  Copies the elements of a floating-point vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_copy_f32(
  const float32_t * pSrc,
        float32_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Copies the elements of a Q7 vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_copy_q7(
  const q7_t * pSrc,
        q7_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Copies the elements of a Q15 vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_copy_q15(
  const q15_t * pSrc,
        q15_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Copies the elements of a Q31 vector.
   * @param[in]  pSrc       input pointer
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_copy_q31(
  const q31_t * pSrc,
        q31_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Fills a constant value into a floating-point vector.
   * @param[in]  value      input value to be filled
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_fill_f32(
        float32_t value,
        float32_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Fills a constant value into a Q7 vector.
   * @param[in]  value      input value to be filled
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_fill_q7(
        q7_t value,
        q7_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Fills a constant value into a Q15 vector.
   * @param[in]  value      input value to be filled
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_fill_q15(
        q15_t value,
        q15_t * pDst,
        uint32_t blockSize);


  /**
   * @brief  Fills a constant value into a Q31 vector.
   * @param[in]  value      input value to be filled
   * @param[out] pDst       output pointer
   * @param[in]  blockSize  number of samples to process
   */
  void riscv_fill_q31(
        q31_t value,
        q31_t * pDst,
        uint32_t blockSize);







/**
 * @brief Weighted sum
 *
 *
 * @param[in]    *in           Array of input values.
 * @param[in]    *weigths      Weights
 * @param[in]    blockSize     Number of samples in the input array.
 * @return Weighted sum
 *
 */
float32_t riscv_weighted_sum_f32(const float32_t *in
  , const float32_t *weigths
  , uint32_t blockSize);


/**
 * @brief Barycenter
 *
 *
 * @param[in]    in         List of vectors
 * @param[in]    weights    Weights of the vectors
 * @param[out]   out        Barycenter
 * @param[in]    nbVectors  Number of vectors
 * @param[in]    vecDim     Dimension of space (vector dimension)
 * @return       None
 *
 */
void riscv_barycenter_f32(const float32_t *in
  , const float32_t *weights
  , float32_t *out
  , uint32_t nbVectors
  , uint32_t vecDim);



#ifdef   __cplusplus
}
#endif

#endif /* ifndef _SUPPORT_FUNCTIONS_H_ */
