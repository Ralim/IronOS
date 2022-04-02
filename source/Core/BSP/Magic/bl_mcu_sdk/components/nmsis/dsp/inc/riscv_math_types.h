/******************************************************************************
 * @file     riscv_math_types.h
 * @brief    Public header file for NMSIS DSP Library
 * @version  V1.9.0
 * @date     23 April 2021
 * Target Processor: RISC-V Cores
 ******************************************************************************/
/*
 * Copyright (c) 2010-2021 Arm Limited or its affiliates. All rights reserved.
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

#ifndef _RISCV_MATH_TYPES_H_

#define _RISCV_MATH_TYPES_H_

#ifdef   __cplusplus
extern "C"
{
#endif

/* Compiler specific diagnostic adjustment */
#if   defined ( __GNUC__ )
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsign-conversion"
  #pragma GCC diagnostic ignored "-Wconversion"
  #pragma GCC diagnostic ignored "-Wunused-parameter"

#elif defined ( __TI_RISCV__ )

#elif defined ( __CSMC__ )

#elif defined ( __TASKING__ )

#elif defined ( _MSC_VER )

#else
  #error Unknown compiler
#endif


/* Included for instrinsics definitions */
#if defined (_MSC_VER )
#include <stdint.h>
#define __STATIC_FORCEINLINE static __forceinline
#define __STATIC_INLINE static __inline
#define __ALIGNED(x) __declspec(align(x))

#elif defined (__GNUC_PYTHON__)
#include <stdint.h>
#define  __ALIGNED(x) __attribute__((aligned(x)))
#define __STATIC_FORCEINLINE static inline __attribute__((always_inline)) 
#define __STATIC_INLINE static inline

#else
#define __NMSIS_GENERIC
#if (defined (__RISCV_FEATURE_DSP) && (__RISCV_FEATURE_DSP == 1))
    #define __DSP_PRESENT   1
#endif
#include "nmsis_core.h"
#undef __NMSIS_GENERIC
#endif

#if (defined (__RISCV_FEATURE_VECTOR) && (__RISCV_FEATURE_VECTOR == 1))
#define RISCV_VECTOR                  1
#include <riscv_vector.h> 
#endif

#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>

/* evaluate RISCV DSP feature */
#if (defined (__RISCV_FEATURE_DSP) && (__RISCV_FEATURE_DSP == 1))
  #define RISCV_MATH_DSP                   1
#endif




#if (__RISCV_FEATURE_MVE & 2)
    #define RISCV_MATH_MVEF
       #define RISCV_MATH_MVE_FLOAT16
#endif






#if   defined ( __GNUC__ )
  #define LOW_OPTIMIZATION_ENTER \
       __attribute__(( optimize("-O1") ))
  #define LOW_OPTIMIZATION_EXIT
  #define IAR_ONLY_LOW_OPTIMIZATION_ENTER
  #define IAR_ONLY_LOW_OPTIMIZATION_EXIT

#elif defined ( __TI_RISCV__ )
  #define LOW_OPTIMIZATION_ENTER
  #define LOW_OPTIMIZATION_EXIT
  #define IAR_ONLY_LOW_OPTIMIZATION_ENTER
  #define IAR_ONLY_LOW_OPTIMIZATION_EXIT

#elif defined ( __CSMC__ )
  #define LOW_OPTIMIZATION_ENTER
  #define LOW_OPTIMIZATION_EXIT
  #define IAR_ONLY_LOW_OPTIMIZATION_ENTER
  #define IAR_ONLY_LOW_OPTIMIZATION_EXIT

#elif defined ( __TASKING__ )
  #define LOW_OPTIMIZATION_ENTER
  #define LOW_OPTIMIZATION_EXIT
  #define IAR_ONLY_LOW_OPTIMIZATION_ENTER
  #define IAR_ONLY_LOW_OPTIMIZATION_EXIT

#elif defined ( _MSC_VER ) || defined(__GNUC_PYTHON__)
      #define LOW_OPTIMIZATION_ENTER
      #define LOW_OPTIMIZATION_EXIT
      #define IAR_ONLY_LOW_OPTIMIZATION_ENTER
      #define IAR_ONLY_LOW_OPTIMIZATION_EXIT
#endif



/* Compiler specific diagnostic adjustment */
#if   defined ( __GNUC__ )
#pragma GCC diagnostic pop

#elif defined ( __TI_RISCV__ )

#elif defined ( __CSMC__ )

#elif defined ( __TASKING__ )

#elif defined ( _MSC_VER )

#else
  #error Unknown compiler
#endif

#ifdef   __cplusplus
}
#endif


#ifdef   __cplusplus
extern "C"
{
#endif

 /**
   * @brief 8-bit fractional data type in 1.7 format.
   */
  typedef int8_t q7_t;

  /**
   * @brief 16-bit fractional data type in 1.15 format.
   */
  typedef int16_t q15_t;

  /**
   * @brief 32-bit fractional data type in 1.31 format.
   */
  typedef int32_t q31_t;

  /**
   * @brief 64-bit fractional data type in 1.63 format.
   */
  typedef int64_t q63_t;

  /**
   * @brief 32-bit floating-point type definition.
   */
  typedef float float32_t;

  /**
   * @brief 64-bit floating-point type definition.
   */
  typedef double float64_t;

  /**
   * @brief vector types
   */







#define F64_MAX   ((float64_t)DBL_MAX)
#define F32_MAX   ((float32_t)FLT_MAX)



#define F64_MIN   (-DBL_MAX)
#define F32_MIN   (-FLT_MAX)



#define F64_ABSMAX   ((float64_t)DBL_MAX)
#define F32_ABSMAX   ((float32_t)FLT_MAX)



#define F64_ABSMIN   ((float64_t)0.0)
#define F32_ABSMIN   ((float32_t)0.0)


#define Q31_MAX   ((q31_t)(0x7FFFFFFFL))
#define Q15_MAX   ((q15_t)(0x7FFF))
#define Q7_MAX    ((q7_t)(0x7F))
#define Q31_MIN   ((q31_t)(0x80000000L))
#define Q15_MIN   ((q15_t)(0x8000))
#define Q7_MIN    ((q7_t)(0x80))

#define Q31_ABSMAX   ((q31_t)(0x7FFFFFFFL))
#define Q15_ABSMAX   ((q15_t)(0x7FFF))
#define Q7_ABSMAX    ((q7_t)(0x7F))
#define Q31_ABSMIN   ((q31_t)0)
#define Q15_ABSMIN   ((q15_t)0)
#define Q7_ABSMIN    ((q7_t)0)

  /* Dimension C vector space */
  #define CMPLX_DIM 2

  /**
   * @brief Error status returned by some functions in the library.
   */

  typedef enum
  {
    RISCV_MATH_SUCCESS                 =  0,        /**< No error */
    RISCV_MATH_ARGUMENT_ERROR          = -1,        /**< One or more arguments are incorrect */
    RISCV_MATH_LENGTH_ERROR            = -2,        /**< Length of data buffer is incorrect */
    RISCV_MATH_SIZE_MISMATCH           = -3,        /**< Size of matrices is not compatible with the operation */
    RISCV_MATH_NANINF                  = -4,        /**< Not-a-number (NaN) or infinity is generated */
    RISCV_MATH_SINGULAR                = -5,        /**< Input matrix is singular and cannot be inverted */
    RISCV_MATH_TEST_FAILURE            = -6,        /**< Test Failed */
    RISCV_MATH_DECOMPOSITION_FAILURE   = -7         /**< Decomposition Failed */
  } riscv_status;


#ifdef   __cplusplus
}
#endif

#endif /*ifndef _RISCV_MATH_TYPES_H_ */
