/*
 * Copyright (C) 2010-2019 Arm Limited or its affiliates. All rights reserved.
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

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_relu6_s8.c
 * Description:  Basic s8 version of ReLU6
 *
 * $Date:        09. October 2020
 * $Revision:    V.1.0.1
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnfunctions.h"
#include "riscv_nnsupportfunctions.h"

/**
 *  @ingroup groupNN
 */

/**
 * @addtogroup Acti
 * @{
 */

/*
 *  Basic ReLU6 function
 *
 * Refer to header file for details.
 *
 */

void riscv_relu6_s8(q7_t *data, uint16_t size)
{
#if defined(RISCV_VECTOR)
  uint16_t blkCnt = size;                               /* Loop counter */
  size_t l;
  vint8m8_t vx;
  //inital to zero
  int8_t vy1=0,vy2=6;
  for (; (l = vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l) {
    vx = vle8_v_i8m8(data, l);
    // if data >= zero, return data, else return zero
    vse8_v_i8m8 (data, vmax_vx_i8m8(vx, vy1, l), l);
    vse8_v_i8m8 (data, vmin_vx_i8m8(vx, vy2, l), l);
    data += l;
  }
#else
    int32_t i;

    for (i = 0; i < size; i++)
    {
        int32_t ip = data[i];

        ip = MAX(ip, 0);
        data[i] = MIN(ip, 6);
    }
#endif /* defined(RISCV_VECTOR) */
}

/**
 * @} end of Acti group
 */
