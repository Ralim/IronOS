/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_helium_utils.h
 * Description:  Utility functions for Helium development
 *
 * @version  V1.9.0
 * @date     23 April 2021
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

#ifndef _RISCV_UTILS_HELIUM_H_
#define _RISCV_UTILS_HELIUM_H_


#ifdef   __cplusplus
extern "C"
{
#endif
/***************************************

Definitions available for MVEF and MVEI

***************************************/

/***************************************

Definitions available for MVEF only

***************************************/


/***************************************

Definitions available for f16 datatype with HW acceleration only

***************************************/
#if defined(RISCV_FLOAT16_SUPPORTED)
#endif 

/***************************************

Definitions available for MVEI and MVEF only

***************************************/

/***************************************

Definitions available for MVEI only

***************************************/

#ifdef   __cplusplus
}
#endif

#endif
