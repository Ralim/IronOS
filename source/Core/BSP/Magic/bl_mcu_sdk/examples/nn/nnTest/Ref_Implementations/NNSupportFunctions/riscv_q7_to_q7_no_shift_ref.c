/*
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.
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
 * Title:        riscv_q7_to_q7_no_shift.c
 * Description:  Converts the elements of the Q7 vector to Q7 vector without left-shift
 *
 * $Date:        17. January 2018
 * $Revision:    V.1.0.0
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"
#include "ref_functions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup nndata_convert
 * @{
 */

/**
 * @brief Converts the elements of the Q7 vector to Q7 vector without left-shift
 * @param[in]       *pSrc points to the Q7 input vector
 * @param[out]      *pDst points to the Q7 output vector
 * @param[in]       blockSize length of the input vector
 * @return none.
 *
 * \par Description:
 *
 * The equation used for the conversion process is:
 *
 * <pre>
 * 	pDst[n] = (q15_t) pSrc[n];   0 <= n < blockSize.
 * </pre>
 *
 */

void riscv_q7_to_q7_no_shift_ref(const q7_t * pSrc, q7_t * pDst, uint32_t blockSize)
{
    const q7_t *pIn = pSrc;
    uint32_t  blkCnt;

    /* Run the below code for RISC-V Core without DSP */

    /* Loop over blockSize number of values */
    blkCnt = blockSize;

    while (blkCnt > 0u)
    {
        /* convert from q7 to q7 and then store the results in the destination buffer */

        *pDst++ = *pIn++;

        /* Decrement the loop counter */
        blkCnt--;
    }

}

/**
 * @} end of nndata_convert group
 */
