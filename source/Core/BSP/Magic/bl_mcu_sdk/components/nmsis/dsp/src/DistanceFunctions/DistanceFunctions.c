/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        BayesFunctions.c
 * Description:  Combination of all distance function source files.
 *
 * $Date:        16. March 2020
 * $Revision:    V1.0.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2020 ARM Limited or its affiliates. All rights reserved.
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

#include "riscv_boolean_distance.c"
#include "riscv_braycurtis_distance_f32.c"
#include "riscv_canberra_distance_f32.c"
#include "riscv_chebyshev_distance_f32.c"
#include "riscv_cityblock_distance_f32.c"
#include "riscv_correlation_distance_f32.c"
#include "riscv_cosine_distance_f32.c"
#include "riscv_dice_distance.c"
#include "riscv_euclidean_distance_f32.c"
#include "riscv_hamming_distance.c"
#include "riscv_jaccard_distance.c"
#include "riscv_jensenshannon_distance_f32.c"
#include "riscv_kulsinski_distance.c"
#include "riscv_minkowski_distance_f32.c"
#include "riscv_rogerstanimoto_distance.c"
#include "riscv_russellrao_distance.c"
#include "riscv_sokalmichener_distance.c"
#include "riscv_sokalsneath_distance.c"
#include "riscv_yule_distance.c"
