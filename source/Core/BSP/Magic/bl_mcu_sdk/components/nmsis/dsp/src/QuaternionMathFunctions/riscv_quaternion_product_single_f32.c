/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_quaternion_product_single_f32.c
 * Description:  Floating-point quaternion product
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
  @ingroup QuatProd
 */

/**
  @defgroup QuatProdSingle Quaternion Product

  Compute the  product of two quaternions.
 */

/**
  @addtogroup QuatProdSingle
  @{
 */

/**
  @brief         Floating-point product of two quaternions.
  @param[in]     qa       first quaternion
  @param[in]     qb       second quaternion
  @param[out]    qr       product of two quaternions
  @return        none
 */

void riscv_quaternion_product_single_f32(const float32_t *qa, 
    const float32_t *qb, 
    float32_t *qr)
{
    qr[0] = qa[0] * qb[0] - qa[1] * qb[1] - qa[2] * qb[2] - qa[3] * qb[3];
    qr[1] = qa[0] * qb[1] + qa[1] * qb[0] + qa[2] * qb[3] - qa[3] * qb[2];
    qr[2] = qa[0] * qb[2] + qa[2] * qb[0] + qa[3] * qb[1] - qa[1] * qb[3];
    qr[3] = qa[0] * qb[3] + qa[3] * qb[0] + qa[1] * qb[2] - qa[2] * qb[1];
}

/**
  @} end of QuatProdSingle group
 */
