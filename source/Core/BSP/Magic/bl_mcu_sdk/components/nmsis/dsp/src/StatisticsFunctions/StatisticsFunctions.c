/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        StatisticsFunctions.c
 * Description:  Combination of all statistics function source files.
 *
 * $Date:        16. March 2020
 * $Revision:    V1.1.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2019-2020 ARM Limited or its affiliates. All rights reserved.
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

#include "riscv_entropy_f32.c"
#include "riscv_entropy_f64.c"
#include "riscv_kullback_leibler_f32.c"
#include "riscv_kullback_leibler_f64.c"
#include "riscv_logsumexp_dot_prod_f32.c"
#include "riscv_logsumexp_f32.c"
#include "riscv_max_f32.c"
#include "riscv_max_q15.c"
#include "riscv_max_q31.c"
#include "riscv_max_q7.c"
#include "riscv_max_no_idx_f32.c"
#include "riscv_mean_f32.c"
#include "riscv_mean_q15.c"
#include "riscv_mean_q31.c"
#include "riscv_mean_q7.c"
#include "riscv_min_f32.c"
#include "riscv_min_q15.c"
#include "riscv_min_q31.c"
#include "riscv_min_q7.c"
#include "riscv_power_f32.c"
#include "riscv_power_q15.c"
#include "riscv_power_q31.c"
#include "riscv_power_q7.c"
#include "riscv_rms_f32.c"
#include "riscv_rms_q15.c"
#include "riscv_rms_q31.c"
#include "riscv_std_f32.c"
#include "riscv_std_q15.c"
#include "riscv_std_q31.c"
#include "riscv_var_f32.c"
#include "riscv_var_q15.c"
#include "riscv_var_q31.c"
#include "riscv_absmax_f32.c"
#include "riscv_absmax_q15.c"
#include "riscv_absmax_q31.c"
#include "riscv_absmax_q7.c"
#include "riscv_absmin_f32.c"
#include "riscv_absmin_q15.c"
#include "riscv_absmin_q31.c"
#include "riscv_absmin_q7.c"