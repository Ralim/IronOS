/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_quaternion2rotation_f32.c
 * Description:  Floating-point quaternion 2 rotation conversion
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
  @defgroup QuatConv Quaternion conversions

  Conversions between quaternion and rotation representations.
 */

/**
  @ingroup QuatConv
 */

/**
  @defgroup QuatRot Quaternion to Rotation

  Conversions from quaternion to rotation.
 */

/**
  @addtogroup QuatRot
  @{
 */

/**
   @brief Conversion of quaternion to equivalent rotation matrix.
   @param[in]       pInputQuaternions points to an array of normalized quaternions
   @param[out]      pOutputRotations points to an array of 3x3 rotations (in row order)
   @param[in]       nbQuaternions number of quaternions in the array
   @return none.
  
   @par
   Format of rotation matrix
   
   
   The quaternion a + ib + jc + kd is converted into rotation matrix:
   <pre>
     a^2 + b^2 - c^2 - d^2                 2bc - 2ad                 2bd + 2ac
                 2bc + 2ad     a^2 - b^2 + c^2 - d^2                 2cd - 2ab
                 2bd - 2ac                 2cd + 2ab     a^2 - b^2 - c^2 + d^2
   </pre>
   Rotation matrix is saved in row order : R00 R01 R02 R10 R11 R12 R20 R21 R22
 */

void riscv_quaternion2rotation_f32(const float32_t *pInputQuaternions, 
    float32_t *pOutputRotations, 
    uint32_t nbQuaternions)
{
#if defined(RISCV_VECTOR)
    uint32_t blkCnt = nbQuaternions;                               /* Loop counter */
    size_t l;
    vfloat32m8_t v_QA0, v_QA1, v_QA2, v_QA3;
    vfloat32m8_t v_q00, v_q11, v_q22, v_q33, v_q01, v_q02, v_q03, v_q12, v_q13, v_q23;
    vfloat32m8_t v_xx, v_yy, v_zz, v_xy, v_xz, v_yx, v_yz, v_zx, v_zy;
    vfloat32m8_t v_temp;
    float32_t *pQA = pInputQuaternions;
    float32_t *pOUT = pOutputRotations;
    ptrdiff_t bstride = 16;
    ptrdiff_t bstride_out = 36;
    for (; (l = vsetvl_e32m8(blkCnt)) > 0; blkCnt -= l) {
        v_QA0 = vlse32_v_f32m8(pQA,bstride, l);
        v_QA1 = vlse32_v_f32m8(pQA+1,bstride, l);
        v_QA2 = vlse32_v_f32m8(pQA+2,bstride, l);
        v_QA3 = vlse32_v_f32m8(pQA+3,bstride, l);

        v_q00 = vfmul_vv_f32m8(v_QA0,v_QA0, l);
        v_q11 = vfmul_vv_f32m8(v_QA1,v_QA1, l);
        v_q22 = vfmul_vv_f32m8(v_QA2,v_QA2, l);
        v_q33 = vfmul_vv_f32m8(v_QA3,v_QA3, l);
        v_q01 = vfmul_vv_f32m8(v_QA0,v_QA1, l);
        v_q02 = vfmul_vv_f32m8(v_QA0,v_QA2, l);
        v_q03 = vfmul_vv_f32m8(v_QA0,v_QA3, l);
        v_q12 = vfmul_vv_f32m8(v_QA1,v_QA2, l);
        v_q13 = vfmul_vv_f32m8(v_QA1,v_QA3, l);
        v_q23 = vfmul_vv_f32m8(v_QA2,v_QA3, l);

        v_xx = vfsub_vv_f32m8(vfadd_vv_f32m8(v_q00,v_q11, l),vfadd_vv_f32m8(v_q22,v_q33, l), l);
        v_yy = vfadd_vv_f32m8(vfsub_vv_f32m8(v_q00,v_q11, l),vfsub_vv_f32m8(v_q22,v_q33, l), l);
        v_zz = vfsub_vv_f32m8(vfsub_vv_f32m8(v_q00,v_q11, l),vfsub_vv_f32m8(v_q22,v_q33, l), l);
        v_xy = vfmul_vf_f32m8(vfsub_vv_f32m8(v_q12,v_q03, l),2, l);
        v_xz = vfmul_vf_f32m8(vfadd_vv_f32m8(v_q13,v_q02, l),2, l);
        v_yx = vfmul_vf_f32m8(vfadd_vv_f32m8(v_q12,v_q03, l),2, l);
        v_yz = vfmul_vf_f32m8(vfsub_vv_f32m8(v_q23,v_q01, l),2, l);
        v_zx = vfmul_vf_f32m8(vfsub_vv_f32m8(v_q13,v_q02, l),2, l);
        v_zy = vfmul_vf_f32m8(vfadd_vv_f32m8(v_q23,v_q01, l),2, l);

        vsse32_v_f32m8(pOUT,bstride_out,v_xx, l);
        vsse32_v_f32m8(pOUT+1,bstride_out,v_xy, l);
        vsse32_v_f32m8(pOUT+2,bstride_out,v_xz, l);
        vsse32_v_f32m8(pOUT+3,bstride_out,v_yx, l);
        vsse32_v_f32m8(pOUT+4,bstride_out,v_yy, l);
        vsse32_v_f32m8(pOUT+5,bstride_out,v_yz, l);
        vsse32_v_f32m8(pOUT+6,bstride_out,v_zx, l);
        vsse32_v_f32m8(pOUT+7,bstride_out,v_zy, l);
        vsse32_v_f32m8(pOUT+8,bstride_out,v_zz, l);

        pQA += l*4;
        pOUT += l*9;
    }
#else
   for(uint32_t nb=0; nb < nbQuaternions; nb++)
   {
        float32_t q00 = SQ(pInputQuaternions[0 + nb * 4]);
        float32_t q11 = SQ(pInputQuaternions[1 + nb * 4]);
        float32_t q22 = SQ(pInputQuaternions[2 + nb * 4]);
        float32_t q33 = SQ(pInputQuaternions[3 + nb * 4]);
        float32_t q01 =  pInputQuaternions[0 + nb * 4]*pInputQuaternions[1 + nb * 4];
        float32_t q02 =  pInputQuaternions[0 + nb * 4]*pInputQuaternions[2 + nb * 4];
        float32_t q03 =  pInputQuaternions[0 + nb * 4]*pInputQuaternions[3 + nb * 4];
        float32_t q12 =  pInputQuaternions[1 + nb * 4]*pInputQuaternions[2 + nb * 4];
        float32_t q13 =  pInputQuaternions[1 + nb * 4]*pInputQuaternions[3 + nb * 4];
        float32_t q23 =  pInputQuaternions[2 + nb * 4]*pInputQuaternions[3 + nb * 4];

        float32_t xx = q00 + q11 - q22 - q33;
        float32_t yy = q00 - q11 + q22 - q33;
        float32_t zz = q00 - q11 - q22 + q33;
        float32_t xy = 2*(q12 - q03);
        float32_t xz = 2*(q13 + q02);
        float32_t yx = 2*(q12 + q03);
        float32_t yz = 2*(q23 - q01);
        float32_t zx = 2*(q13 - q02);
        float32_t zy = 2*(q23 + q01);

        pOutputRotations[0 + nb * 9] = xx; pOutputRotations[1 + nb * 9] = xy; pOutputRotations[2 + nb * 9] = xz;
        pOutputRotations[3 + nb * 9] = yx; pOutputRotations[4 + nb * 9] = yy; pOutputRotations[5 + nb * 9] = yz;
        pOutputRotations[6 + nb * 9] = zx; pOutputRotations[7 + nb * 9] = zy; pOutputRotations[8 + nb * 9] = zz;
   }
#endif
}


/**
  @} end of QuatRot group
 */
