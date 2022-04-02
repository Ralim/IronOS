
/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_braycurtis_distance_f32.c
 * Description:  Bray-Curtis distance between two vectors
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

#include "dsp/distance_functions.h"
#include <limits.h>
#include <math.h>



/**
  @addtogroup braycurtis
  @{
 */


/**
 * @brief        Bray-Curtis distance between two vectors
 * @param[in]    pA         First vector
 * @param[in]    pB         Second vector
 * @param[in]    blockSize  vector length
 * @return distance
 *
 */
float32_t riscv_braycurtis_distance_f32(const float32_t *pA,const float32_t *pB, uint32_t blockSize)
{
   float32_t accumDiff=0.0f, accumSum=0.0f, tmpA, tmpB;

#if defined(RISCV_VECTOR)
   uint32_t blkCnt = blockSize;                               /* Loop counter */
   size_t l;
   vfloat32m8_t v_x, v_y;
   vfloat32m8_t v_a, v_b;
   vfloat32m8_t v_at, v_bt;
   vfloat32m1_t v_temp;
   l = vsetvl_e32m1(1);
   v_temp = vfsub_vv_f32m1(v_temp, v_temp, l);
   l = vsetvl_e32m8(blkCnt);
   v_a = vfsub_vv_f32m8(v_a,v_a, l);
   v_b = vfsub_vv_f32m8(v_b,v_b, l);
   for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
      v_x = vle32_v_f32m8(pA, l);
      v_y = vle32_v_f32m8(pB, l);
      v_at = vfsub_vv_f32m8(v_x,v_y, l);
      v_bt = vfadd_vv_f32m8(v_x,v_y, l);
      v_a = vfadd_vv_f32m8 (v_a, vfsgnjx_vv_f32m8(v_at, v_at, l), l);
      v_b = vfadd_vv_f32m8 (v_b, vfsgnjx_vv_f32m8(v_bt, v_bt, l), l);
      pA += l;
      pB += l;
   }
   l = vsetvl_e32m8(blockSize);
   accumDiff = vfmv_f_s_f32m1_f32 (vfredsum_vs_f32m8_f32m1(v_temp,v_a,v_temp, l));
   l = vsetvl_e32m8(blockSize);
   accumSum = vfmv_f_s_f32m1_f32 (vfredsum_vs_f32m8_f32m1(v_temp,v_b,v_temp, l));
#else
   while(blockSize > 0)
   {
      tmpA = *pA++;
      tmpB = *pB++;
      accumDiff += fabsf(tmpA - tmpB);
      accumSum += fabsf(tmpA + tmpB);
      blockSize --;
   }
#endif
   /*

   It is assumed that accumSum is not zero. Since it is the sum of several absolute
   values it would imply that all of them are zero. It is very unlikely for long vectors.

   */
   return(accumDiff / accumSum);
}


/**
 * @} end of braycurtis group
 */

