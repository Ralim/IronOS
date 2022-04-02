/******************************************************************************
 * @file     riscv_math.h
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

/**
   \mainpage NMSIS DSP Software Library
   *
   * \section intro Introduction
   *
   * This user manual describes the NMSIS DSP software library,
   * a suite of common signal processing functions for use on Nuclei N/NX processor based devices.
   *
   * The library is divided into a number of functions each covering a specific category:
   * - Basic math functions
   * - Fast math functions
   * - Complex math functions
   * - Filtering functions
   * - Matrix functions
   * - Transform functions
   * - Motor control functions
   * - Statistical functions
   * - Support functions
   * - Interpolation functions
   * - Support Vector Machine functions (SVM)
   * - Bayes classifier functions
   * - Distance functions
   * - Quaternion functions
   *
   * The library has generally separate functions for operating on 8-bit integers, 16-bit integers,
   * 32-bit integer and 32-bit floating-point values.
   *
   * The library functions are declared in the public file <code>riscv_math.h</code> which is placed in the <code>Include</code> folder.
   * Simply include this file and link the appropriate library in the application and begin calling the library functions.
   * The Library supports single public header file <code>riscv_math.h</code> for Nuclei N cores with little endian.
   * Same header file will be used for floating point unit(FPU) variants.
   *
   * \note Please refer to [NMSIS-DSP](../../../dsp/index.html)
   *
   * \section example Examples
   *
   * The library ships with a number of examples which demonstrate how to use the library functions.
   *
   * Toolchain Support
   * -----------------
   *
   * The library has been developed and tested with nuclei riscv gcc toolchain.
   *
   * Building the Library
   * --------------------
   *
   * In NMSIS repo, it contains a Makefile to rebuild libraries on nuclei riscv gcc toolchain in the <code>NMSIS/</code> folder.
   * * In *NMSIS* folder, you can run `make gen_dsp_lib` to build and install DSP library into **NMSIS/Library/DSP/GCC** folder.
   *
   * Preprocessor Macros
   * -------------------
   *
   * Each library project have different preprocessor macros.
   *
   * - RISCV_MATH_MATRIX_CHECK:
   *
   * Define macro RISCV_MATH_MATRIX_CHECK for checking on the input and output sizes of matrices
   *
   * - RISCV_MATH_ROUNDING:
   *
   * Define macro RISCV_MATH_ROUNDING for rounding on support functions
   *
   * - RISCV_MATH_LOOPUNROLL:
   *
   * Define macro RISCV_MATH_LOOPUNROLL to enable manual loop unrolling in DSP functions
   *
   */


/**
 * @defgroup groupExamples Examples
 */




#ifndef _RISCV_MATH_H
#define _RISCV_MATH_H


#include "riscv_math_types.h"
#include "riscv_math_memory.h"

#include "dsp/none.h"
#include "dsp/utils.h"

#include "dsp/basic_math_functions.h"
#include "dsp/interpolation_functions.h"
#include "dsp/bayes_functions.h"
#include "dsp/matrix_functions.h"
#include "dsp/complex_math_functions.h"
#include "dsp/statistics_functions.h"
#include "dsp/controller_functions.h"
#include "dsp/support_functions.h"
#include "dsp/distance_functions.h"
#include "dsp/svm_functions.h"
#include "dsp/fast_math_functions.h"
#include "dsp/transform_functions.h"
#include "dsp/filtering_functions.h"
#include "dsp/quaternion_math_functions.h"



#ifdef   __cplusplus
extern "C"
{
#endif




//#define TABLE_SPACING_Q31     0x400000
//#define TABLE_SPACING_Q15     0x80





#ifdef   __cplusplus
}
#endif


#endif /* _RISCV_MATH_H */

/**
 *
 * End of file.
 */
