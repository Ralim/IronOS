/******************************************************************************
 * @file     riscv_math_memory.h
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

#ifndef _RISCV_MATH_MEMORY_H_

#define _RISCV_MATH_MEMORY_H_

#include "riscv_math_types.h"


#ifdef   __cplusplus
extern "C"
{
#endif

/**
  @brief definition to read/write two 16 bit values.
  @deprecated
 */
#if   defined ( __GNUC__ )
  #define __SIMD32_TYPE int32_t
#elif defined ( __TI_RISCV__ )
  #define __SIMD32_TYPE int32_t
#elif defined ( __CSMC__ )
  #define __SIMD32_TYPE int32_t
#elif defined ( __TASKING__ )
  #define __SIMD32_TYPE __un(aligned) int32_t
#elif defined(_MSC_VER )
  #define __SIMD32_TYPE int32_t
#else
  #error Unknown compiler
#endif

#define __SIMD32(addr)        (*(__SIMD32_TYPE **) & (addr))
#define __SIMD32_CONST(addr)  ( (__SIMD32_TYPE * )   (addr))
#define _SIMD32_OFFSET(addr)  (*(__SIMD32_TYPE * )   (addr))
#define __SIMD64(addr)        (*(      int64_t **) & (addr))


/* SIMD replacement */

/**
  @brief         Read 2 Q31 from Q31 pointer and increment pointer afterwards.
  @param[in]     pQ31      points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t read_q31x2_ia (
  q31_t ** pQ31)
{
  q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(*pQ31);
#else
  val = *((q63_t *)*pQ31);
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(&val), (void *)(*pQ31), 8);
#endif
  *pQ31 += 2;
  return (val);
}

/**
  @brief         Read 2 Q31 from Q31 pointer and decrement pointer afterwards.
  @param[in]     pQ31      points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t read_q31x2_da (
  q31_t ** pQ31)
{
  q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(*pQ31);
#else
  val = *((q63_t *)*pQ31);
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(&val), (void *)(*pQ31), 8);
#endif
  *pQ31 -= 2;
  return (val);
}

/**
  @brief         Read 2 Q31 from Q31 pointer.
  @param[in]     pQ31      points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t read_q31x2 (
  q31_t * pQ31)
{
  q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(pQ31);
#else
  val = *((q63_t *)pQ31);
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(&val), (void *)(pQ31), 8);
#endif
  return (val);
}

/**
  @brief         Write 2 Q31 to Q31 pointer and increment pointer afterwards.
  @param[in]     pQ31      points to input value
  @param[in]     value     Q63 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q31x2_ia (
		q31_t ** pQ31,
		q63_t    value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(*pQ31, value);
#else
  *((q63_t *)*pQ31) = value;
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(*pQ31), (void *)(&value), 8);
#endif
  *pQ31 += 2;
}

/**
  @brief         Write 2 Q31 to Q31 pointer.
  @param[in]     pQ31      points to input value
  @param[in]     value     Q63 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q31x2 (
		q31_t * pQ31,
		q63_t value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(pQ31, value);
#else
  *((q63_t *)pQ31) = value;
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(pQ31), (void *)(&value), 8);
#endif
}

/**
  @brief         Read 2 Q15 from Q15 pointer.
  @param[in]     pQ15      points to input value
  @return        Q31 value
 */
__STATIC_FORCEINLINE q31_t read_q15x2 (
  q15_t * pQ15)
{
  q31_t val;

#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (&val, pQ15, 4);
#else
  val = __LW(pQ15);
#endif

  return (val);
}

/**
  @brief         Read 2 Q15 from Q15 pointer and increment pointer afterwards.
  @param[in]     pQ15      points to input value
  @return        Q31 value
 */
__STATIC_FORCEINLINE q31_t read_q15x2_ia (
  q15_t ** pQ15)
{
  q31_t val;

#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (&val, *pQ15, 4);
#else
  val = __LW(*pQ15);
#endif

 *pQ15 += 2;
 return (val);
}

/**
  @brief         Read 2 Q15 from Q15 pointer and decrement pointer afterwards.
  @param[in]     pQ15      points to input value
  @return        Q31 value
 */
__STATIC_FORCEINLINE q31_t read_q15x2_da (
  q15_t ** pQ15)
{
  q31_t val;

#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (&val, *pQ15, 4);
#else
  val = __LW(*pQ15);
#endif

  *pQ15 -= 2;
  return (val);
}

/**
  @brief         Write 2 Q15 to Q15 pointer and increment pointer afterwards.
  @param[in]     pQ15      points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q15x2_ia (
  q15_t ** pQ15,
  q31_t    value)
{
#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (*pQ15, &value, 4);
#else
  __SW(*pQ15, value);
#endif

 *pQ15 += 2;
}

/**
  @brief         Write 2 Q15 to Q15 pointer.
  @param[in]     pQ15      points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q15x2 (
  q15_t * pQ15,
  q31_t   value)
{
#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (pQ15, &value, 4);
#else
  __SW(pQ15, value);
#endif
}

/**
  @brief         Write 4 Q15 to Q15 pointer and increment pointer afterwards.
  @param[in]     pQ15      points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q15x4_ia (
		q15_t ** pQ15,
		q63_t    value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(*pQ15, value);
#else
  *((q63_t *)*pQ15) = value;
#endif
#else
  memcpy((void *)(*pQ15), (void *)(&value), 8);
#endif
  *pQ15 += 4;
}

/**
  @brief         Write 4 Q15 to Q15 pointer and decrement pointer afterwards.
  @param[in]     pQ15      points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q15x4_da (
		q15_t ** pQ15,
		q63_t    value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(*pQ15, value);
#else
  *((q63_t *)*pQ15) = value;
#endif
#else
  memcpy((void *)(*pQ15), (void *)(&value), 8);
#endif
  *pQ15 -= 4;
}

/**
  @brief         Write 4 Q15 to Q15 pointer.
  @param[in]     pQ15      points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q15x4 (
		q15_t * pQ15,
		q63_t   value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(pQ15, value);
#else
  *((q63_t *)pQ15) = value;
#endif
#else
  memcpy((void *)(pQ15), (void *)(&value), 8);
#endif
}

/**
  @brief         Read 4 Q15 from Q15 pointer and increment pointer afterwards.
  @param[in]     pQ15      points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t read_q15x4_ia (
		q15_t ** pQ15)
{
  q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(*pQ15);
#else
  val = *((q63_t *)*pQ15);
#endif
#else
  memcpy((void *)(&val), (void *)(*pQ15), 8);
#endif
  *pQ15 += 4;

  return (val);
}

/**
  @brief         Read 4 Q15 from Q15 pointer and decrement pointer afterwards.
  @param[in]     pQ15      points to input value
  @return        Q31 value
 */
__STATIC_FORCEINLINE q63_t read_q15x4_da (
		q15_t ** pQ15)
{
	q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(*pQ15);
#else
  val = *((q63_t *)*pQ15);
#endif
#else
  memcpy((void *)(&val), (void *)(*pQ15), 8);
#endif
  *pQ15 -= 4;

  return (val);
}

/**
  @brief         Read 4 Q15 from Q15 pointer.
  @param[in]     pQ15      points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t read_q15x4 (
		q15_t * pQ15)
{
  q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(pQ15);
#else
  val = *((q63_t *)pQ15);
#endif /* __RISCV_XLEN == 64 */
#else
  memcpy((void *)(&val), (void *)(pQ15), 8);
#endif
  return (val);
}

/**
  @brief         Read 4 Q7 from Q7 pointer and increment pointer afterwards.
  @param[in]     pQ7       points to input value
  @return        Q31 value
 */
__STATIC_FORCEINLINE q31_t read_q7x4_ia (
  q7_t ** pQ7)
{
  q31_t val;


#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (&val, *pQ7, 4);
#else
  val = __LW(*pQ7);
#endif

  *pQ7 += 4;

  return (val);
}

/**
  @brief         Read 4 Q7 from Q7 pointer and decrement pointer afterwards.
  @param[in]     pQ7       points to input value
  @return        Q31 value
 */
__STATIC_FORCEINLINE q31_t read_q7x4_da (
  q7_t ** pQ7)
{
  q31_t val;
#ifdef __RISCV_FEATURE_UNALIGNED
  memcpy (&val, *pQ7, 4);
#else
  val = __LW(*pQ7);
#endif
  *pQ7 -= 4;

  return (val);
}

/**
  @brief         Write 4 Q7 to Q7 pointer and increment pointer afterwards.
  @param[in]     pQ7       points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q7x4_ia (
  q7_t ** pQ7,
  q31_t   value)
{
#ifdef __RISCV_FEATURE_UNALIGNED
  q31_t val = value;
  memcpy (*pQ7, &val, 4);
#else
  __SW(*pQ7, value);
#endif
  *pQ7 += 4;
}

/**
  @brief         Write 4 Q7 to Q7 pointer.
  @param[in]     pQ7       points to input value
  @param[in]     value     Q31 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q7x4 (
  q7_t * pQ7,
  q31_t   value)
{
#ifdef __RISCV_FEATURE_UNALIGNED
  q31_t val = value;
  memcpy (pQ7, &val, 4);
#else
  __SW(pQ7, value);
#endif
}

/**
  @brief         Read 8 Q7 from Q7 pointer and increment pointer afterwards.
  @param[in]     pQ7       points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t read_q7x8_ia (
		q7_t ** pQ7)
{
	q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(*pQ7);
#else
  val = *((q63_t *)*pQ7);
#endif
#else
  memcpy((void *)(&val), (void *)(*pQ7), 8);
#endif
  *pQ7 += 8;

  return val;
}

/**
  @brief         Read 8 Q7 from Q7 pointer and decrement pointer afterwards.
  @param[in]     pQ7       points to input value
  @return        Q63 value
 */
__STATIC_FORCEINLINE q63_t read_q7x8_da (
		q7_t ** pQ7)
{
	q63_t val;
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  val = __LD(*pQ7);
#else
  val = *((q63_t *)*pQ7);
#endif
#else
  memcpy((void *)(&val), (void *)(*pQ7), 8);
#endif
  *pQ7 -= 8;
  return val;
}

/**
  @brief         Write 8 Q7 to Q7 pointer and increment pointer afterwards.
  @param[in]     pQ7       points to input value
  @param[in]     value     Q63 value
  @return        none
 */
__STATIC_FORCEINLINE void write_q7x8_ia (
		q7_t ** pQ7,
		q63_t   value)
{
#ifndef __RISCV_FEATURE_UNALIGNED
#if __RISCV_XLEN == 64
  __SD(*pQ7,value);
#else
  *((q63_t *)*pQ7) = value;
#endif
#else
  memcpy((void *)(*pQ7), (void *)(&value), 8);
#endif
  *pQ7 += 8;
}


#ifdef   __cplusplus
}
#endif

#endif /*ifndef _RISCV_MATH_MEMORY_H_ */
