/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_vinverse_f16.c
 * Description:  Fast vectorized inverse
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

#include "dsp/fast_math_functions_f16.h"

#if defined(RISCV_FLOAT16_SUPPORTED)

#include "riscv_common_tables.h"

#include "riscv_vec_math_f16.h"

void riscv_vinverse_f16(
  const float16_t * pSrc,
        float16_t * pDst,
        uint32_t blockSize)
{
   uint32_t blkCnt; 

   blkCnt = blockSize;

   while (blkCnt > 0U)
   {
      
      *pDst++ = 1.0 / *pSrc++;
  
      /* Decrement loop counter */
      blkCnt--;
   }
}

#endif /* #if defined(RISCV_FLOAT16_SUPPORTED) */ 

