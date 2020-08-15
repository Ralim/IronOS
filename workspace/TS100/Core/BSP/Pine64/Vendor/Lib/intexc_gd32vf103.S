/*
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
/******************************************************************************
 * \file     intexc_gd32vf103.S
 * \brief    NMSIS Interrupt and Exception Handling Template File
 *           for Device gd32vf103
 * \version  V1.00
 * \date     7 Jan 2020
 *
 ******************************************************************************/

#include "riscv_encoding.h"

/* Default Handler for Exceptions / Interrupts */
.global default_intexc_handler
.weak default_intexc_handler
Undef_Handler:
default_intexc_handler:
1:
	j 1b
