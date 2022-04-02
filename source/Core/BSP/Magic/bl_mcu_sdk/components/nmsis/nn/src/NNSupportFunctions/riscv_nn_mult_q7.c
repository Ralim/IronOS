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
 * Title:        riscv_nn_mult_q7.c
 * Description:  Q7 vector multiplication with variable output shifts
 *
 * $Date:        09. October 2020
 * $Revision:    V.1.0.2
 *
 * Target Processor: RISC-V Cores
 *
 * -------------------------------------------------------------------- */

#include "riscv_nnsupportfunctions.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup NNBasicMath
 * @{
 */

/**
 * @brief           Q7 vector multiplication with variable output shifts
 * @param[in]       *pSrcA        pointer to the first input vector
 * @param[in]       *pSrcB        pointer to the second input vector
 * @param[out]      *pDst         pointer to the output vector
 * @param[in]       out_shift     amount of right-shift for output
 * @param[in]       blockSize     number of samples in each vector
 *
 * <b>Scaling and Overflow Behavior:</b>
 * \par
 * The function uses saturating arithmetic.
 * Results outside of the allowable Q7 range [0x80 0x7F] will be saturated.
 */

void riscv_nn_mult_q7(q7_t *pSrcA, q7_t *pSrcB, q7_t *pDst, const uint16_t out_shift, uint32_t blockSize)
{
#if defined (RISCV_VECTOR)
    uint32_t blkCnt = blockSize;                              /* Loop counter */
    size_t l;
    q7_t *pCnt = pDst;
    q7_t *pA = pSrcA;
    q7_t *pB = pSrcB;
    vint8m2_t pA_v8m2,pB_v8m2;
    vint16m4_t mulRes_i16m4;
    vint32m8_t addRes_i32m8;
         
    for (; (l = vsetvl_e8m2(blkCnt)) > 0; blkCnt -= l) {
      pA_v8m2 = vle8_v_i8m2(pA, l);
      pB_v8m2 = vle8_v_i8m2(pB, l);
      pA += l;
      pB += l;
      mulRes_i16m4 = vwmul_vv_i16m4(pA_v8m2,pB_v8m2, l);
      addRes_i32m8 = vwadd_vx_i32m8(mulRes_i16m4,NN_ROUND(out_shift), l);
      vse8_v_i8m2 (pCnt, vnclip_wx_i8m2(vnsra_wx_i16m4(addRes_i32m8,out_shift, l),0, l), l);
      pCnt += l;
    }
#else
  uint32_t blkCnt;                               /* loop counters */

#if defined(RISCV_MATH_DSP)

/* Run the below code for RISC-V Core with DSP enabled */
  q7_t out1, out2, out3, out4;                   /* Temporary variables to store the product */
  q63_t mul1;
  q31_t inA1, inB1; 
  /* loop Unrolling */
  blkCnt = blockSize >> 2U;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
   ** a second loop below computes the remaining 1 to 3 samples. */
  while (blkCnt > 0U)
  {
    /* C = A * B */
    /* Multiply the inputs and store the results in temporary variables */
    // out1 = (q7_t) __SSAT(((q15_t) ((q15_t) (*pSrcA++) * (*pSrcB++) + NN_ROUND(out_shift)) >> out_shift), 8);
    // out2 = (q7_t) __SSAT(((q15_t) ((q15_t) (*pSrcA++) * (*pSrcB++) + NN_ROUND(out_shift)) >> out_shift), 8);
    // out3 = (q7_t) __SSAT(((q15_t) ((q15_t) (*pSrcA++) * (*pSrcB++) + NN_ROUND(out_shift)) >> out_shift), 8);
    // out4 = (q7_t) __SSAT(((q15_t) ((q15_t) (*pSrcA++) * (*pSrcB++) + NN_ROUND(out_shift)) >> out_shift), 8);

    inA1 = riscv_nn_read_q7x4_ia((const q7_t **)&pSrcA);
    inB1 = riscv_nn_read_q7x4_ia((const q7_t **)&pSrcB);

    mul1 = __RV_SMUL8(inA1,inB1);

    out1 = (q7_t) __SSAT((q15_t) ((mul1 & 0xffff) + NN_ROUND(out_shift)) >> out_shift, 8);
    out2 = (q7_t) __SSAT((q15_t) (((mul1 & 0x00000000ffff0000UL) >> 16) + NN_ROUND(out_shift)) >> out_shift, 8);
    out3 = (q7_t) __SSAT((q15_t) (((mul1 & 0x0000ffff00000000UL) >> 32) + NN_ROUND(out_shift)) >> out_shift, 8);
    out4 = (q7_t) __SSAT((q15_t) (((mul1 & 0xffff000000000000UL) >> 48) + NN_ROUND(out_shift)) >> out_shift, 8);

        /* Store the results of 4 inputs in the destination buffer in single cycle by packing */
        *__SIMD32(pDst)++ = __PACKq7(out1, out2, out3, out4);

        /* Decrement the blockSize loop counter */
        blkCnt--;
    }

    /* If the blockSize is not a multiple of 4, compute any remaining output samples here.
     ** No loop unrolling is used. */
    blkCnt = blockSize % 0x4U;

#else

    /* Run the below code for RISC-V Core without DSP */

    /* Initialize blkCnt with number of samples */
    blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_DSP) */

    while (blkCnt > 0U)
    {
        /* C = A * B */
        /* Multiply the inputs and store the result in the destination buffer */
        *pDst++ = (q7_t)__SSAT(((q15_t)((q15_t)(*pSrcA++) * (*pSrcB++) + NN_ROUND(out_shift)) >> out_shift), 8);

        /* Decrement the blockSize loop counter */
        blkCnt--;
    }
#endif /*defined (RISCV_VECTOR)*/
}

/**
 * @} end of NNBasicMath group
 */
