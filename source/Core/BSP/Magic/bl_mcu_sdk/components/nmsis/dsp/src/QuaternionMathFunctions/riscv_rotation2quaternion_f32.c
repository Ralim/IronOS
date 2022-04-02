/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_rotation2quaternion_f32.c
 * Description:  Floating-point rotation to quaternion conversion
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

#define RI(x,y) r[(3*(x) + (y))]


/**
  @ingroup QuatConv
 */

/**
  @defgroup RotQuat Rotation to Quaternion

  Conversions from rotation to quaternion.
 */

/**
  @addtogroup RotQuat
  @{
 */

/**
 * @brief Conversion of a rotation matrix to an equivalent quaternion.
 * @param[in]       pInputRotations points to an array 3x3 rotation matrix (in row order)
 * @param[out]      pOutputQuaternions points to an array quaternions
 * @param[in]       nbQuaternions number of quaternions in the array
 * @return none.
 *
 * q and -q are representing the same rotation. This ambiguity must be taken into
 * account when using the output of this function.
 * 
 */

void riscv_rotation2quaternion_f32(const float32_t *pInputRotations, 
    float32_t *pOutputQuaternions,  
    uint32_t nbQuaternions)
{
   for(uint32_t nb=0; nb < nbQuaternions; nb++)
   {
       const float32_t *r=&pInputRotations[nb*9];
       float32_t *q=&pOutputQuaternions[nb*4];

       float32_t trace = RI(0,0) + RI(1,1) + RI(2,2);

       float32_t doubler;
       float32_t s;



      if (trace > 0)
      {
        doubler = sqrtf(trace + 1.0) * 2; // invs=4*qw
        s = 1.0 / doubler;
        q[0] = 0.25 * doubler;
        q[1] = (RI(2,1) - RI(1,2)) * s;
        q[2] = (RI(0,2) - RI(2,0)) * s;
        q[3] = (RI(1,0) - RI(0,1)) * s;
      }
      else if ((RI(0,0) > RI(1,1)) && (RI(0,0) > RI(2,2)) )
      {
        doubler = sqrtf(1.0 + RI(0,0) - RI(1,1) - RI(2,2)) * 2; // invs=4*qx
        s = 1.0 / doubler;
        q[0] = (RI(2,1) - RI(1,2)) * s;
        q[1] = 0.25 * doubler;
        q[2] = (RI(0,1) + RI(1,0)) * s;
        q[3] = (RI(0,2) + RI(2,0)) * s;
      }
      else if (RI(1,1) > RI(2,2))
      {
        doubler = sqrtf(1.0 + RI(1,1) - RI(0,0) - RI(2,2)) * 2; // invs=4*qy
        s = 1.0 / doubler;
        q[0] = (RI(0,2) - RI(2,0)) * s;
        q[1] = (RI(0,1) + RI(1,0)) * s;
        q[2] = 0.25 * doubler;
        q[3] = (RI(1,2) + RI(2,1)) * s;
      }
      else
      {
        doubler = sqrtf(1.0 + RI(2,2) - RI(0,0) - RI(1,1)) * 2; // invs=4*qz
        s = 1.0 / doubler;
        q[0] = (RI(1,0) - RI(0,1)) * s;
        q[1] = (RI(0,2) + RI(2,0)) * s;
        q[2] = (RI(1,2) + RI(2,1)) * s;
        q[3] = 0.25 * doubler;
      }

    }
}

/**
  @} end of RotQuat group
 */
