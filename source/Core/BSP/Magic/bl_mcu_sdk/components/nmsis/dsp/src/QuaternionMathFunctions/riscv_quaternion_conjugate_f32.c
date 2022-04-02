/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_quaternion_conjugate_f32.c
 * Description:  Floating-point quaternion conjugate
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

#include "dsp/quaternion_math_functions.h"
#include <math.h>

/**
  @ingroup groupQuaternionMath
 */

/**
  @defgroup QuatConjugate Quaternion Conjugate

  Compute the conjugate of a quaternion.
 */

/**
  @addtogroup QuatConjugate
  @{
 */

/**
  @brief         Floating-point quaternion conjugates.
  @param[in]     pInputQuaternions            points to the input vector of quaternions
  @param[out]    pConjugateQuaternions        points to the output vector of conjugate quaternions
  @param[in]     nbQuaternions                number of quaternions in each vector
  @return        none
 */

void riscv_quaternion_conjugate_f32(const float32_t *pInputQuaternions, 
    float32_t *pConjugateQuaternions, 
    uint32_t nbQuaternions)
{
#if defined(RISCV_VECTOR)
    uint32_t blkCnt = nbQuaternions;                               /* Loop counter */
    size_t l;
    vfloat32m8_t v_x, v_y;
    float32_t *pIN = pInputQuaternions;
    float32_t *pOUT = pConjugateQuaternions;
    ptrdiff_t bstride = 16;
    for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
        v_x = vlse32_v_f32m8(pIN,bstride, l);
        vsse32_v_f32m8(pOUT,bstride,v_x, l);
        v_x = vlse32_v_f32m8(pIN+1,bstride, l);
        v_x = vfsgnjn_vv_f32m8(v_x,v_x, l);
        vsse32_v_f32m8(pOUT+1,bstride,v_x, l);
        v_x = vlse32_v_f32m8(pIN+2,bstride, l);
        v_x = vfsgnjn_vv_f32m8(v_x,v_x, l);
        vsse32_v_f32m8(pOUT+2,bstride,v_x, l);
        v_x = vlse32_v_f32m8(pIN+3,bstride, l);
        v_x = vfsgnjn_vv_f32m8(v_x,v_x, l);
        vsse32_v_f32m8(pOUT+3,bstride,v_x, l);
        pIN += l*4;
        pOUT += l*4;
    }
#else
    for(uint32_t i=0; i < nbQuaternions; i++)
    {

        pConjugateQuaternions[4 * i + 0] = pInputQuaternions[4 * i + 0];
        pConjugateQuaternions[4 * i + 1] = -pInputQuaternions[4 * i + 1];
        pConjugateQuaternions[4 * i + 2] = -pInputQuaternions[4 * i + 2];
        pConjugateQuaternions[4 * i + 3] = -pInputQuaternions[4 * i + 3];
    }
#endif
}

/**
  @} end of QuatConjugate group
 */
