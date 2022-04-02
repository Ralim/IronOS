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
 * Title:        riscv_nn_mult_q15.c
 * Description:  Q15 vector multiplication with variable output shifts
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
 * Results outside of the allowable Q15 range [0x8000 0x7FFF] will be saturated.
 */

void riscv_nn_mult_q15(q15_t *pSrcA, q15_t *pSrcB, q15_t *pDst, const uint16_t out_shift, uint32_t blockSize)
{
#if defined (RISCV_VECTOR) && ((__XLEN != 32) || (__FLEN != 32))
    uint32_t blkCnt = blockSize;                              /* Loop counter */
    size_t l;
    q15_t *pCnt = pDst;
    q15_t *pA = pSrcA;
    q15_t *pB = pSrcB;
    vint16m2_t pA_v16m2,pB_v16m2;
    vint32m4_t mulRes_i32m4;
    vint64m8_t addRes_i64m8;
         
    for (; (l = vsetvl_e16m2(blkCnt)) > 0; blkCnt -= l) {
      pA_v16m2 = vle16_v_i16m2(pA, l);
      pB_v16m2 = vle16_v_i16m2(pB, l);
      pA += l;
      pB += l;
      mulRes_i32m4 = vwmul_vv_i32m4(pA_v16m2,pB_v16m2, l);
      addRes_i64m8 = vwadd_vx_i64m8(mulRes_i32m4,NN_ROUND(out_shift), l);
      vse16_v_i16m2 (pCnt, vnclip_wx_i16m2(vnsra_wx_i32m4(addRes_i64m8,out_shift, l),0, l), l);
      pCnt += l;
    }
#else
  uint32_t blkCnt;                               /* loop counters */

#if defined (RISCV_MATH_DSP)

/* Run the below code for RISC-V Core with DSP enabled */
  q31_t inA1, inA2, inB1, inB2;                  /* temporary input variables */
  q15_t out1, out2, out3, out4;                  /* temporary output variables */
  // q31_t mul1, mul2, mul3, mul4;                  /* temporary variables */
  q63_t mul1, mul2;

  /* loop Unrolling */
  blkCnt = blockSize >> 2U;

  /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
   ** a second loop below computes the remaining 1 to 3 samples. */
  while (blkCnt > 0U)
  {
    /* read two samples at a time from sourceA */
    inA1 = riscv_nn_read_q15x2_ia((const q15_t **)&pSrcA);
    /* read two samples at a time from sourceB */
    inB1 = riscv_nn_read_q15x2_ia((const q15_t **)&pSrcB);
    /* read two samples at a time from sourceA */
    inA2 = riscv_nn_read_q15x2_ia((const q15_t **)&pSrcA);
    /* read two samples at a time from sourceB */
    inB2 = riscv_nn_read_q15x2_ia((const q15_t **)&pSrcB);

    // /* multiply mul = sourceA * sourceB */
    // mul1 = (q31_t) ((q15_t) (inA1 >> 16) * (q15_t) (inB1 >> 16));
    // mul2 = (q31_t) ((q15_t) inA1 * (q15_t) inB1);
    // mul3 = (q31_t) ((q15_t) (inA2 >> 16) * (q15_t) (inB2 >> 16));
    // mul4 = (q31_t) ((q15_t) inA2 * (q15_t) inB2);
    mul1 = __RV_SMUL16(inA1,inB1);
    mul2 = __RV_SMUL16(inA2,inB2);

    // /* saturate result to 16 bit */
    // out1 = (q15_t) __SSAT((q31_t) (mul1 + NN_ROUND(out_shift)) >> out_shift, 16);
    // out2 = (q15_t) __SSAT((q31_t) (mul2 + NN_ROUND(out_shift)) >> out_shift, 16);
    // out3 = (q15_t) __SSAT((q31_t) (mul3 + NN_ROUND(out_shift)) >> out_shift, 16);
    // out4 = (q15_t) __SSAT((q31_t) (mul4 + NN_ROUND(out_shift)) >> out_shift, 16);
    out1 = (q15_t) __SSAT((q31_t) ((mul1 & 0xffffffff) + NN_ROUND(out_shift)) >> out_shift, 16);
    out2 = (q15_t) __SSAT((q31_t) (((mul1 & 0xffffffff00000000UL) >> 32) + NN_ROUND(out_shift)) >> out_shift, 16);
    out3 = (q15_t) __SSAT((q31_t) ((mul2 & 0xffffffff) + NN_ROUND(out_shift)) >> out_shift, 16);
    out4 = (q15_t) __SSAT((q31_t) (((mul2 & 0xffffffff00000000UL) >> 32) + NN_ROUND(out_shift)) >> out_shift, 16);
    /* store the result */

    *__SIMD32(pDst)++ = __RV_PKBB16(out2, out1);
    *__SIMD32(pDst)++ = __RV_PKBB16(out4, out3);


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
        *pDst++ = (q15_t)__SSAT(((q31_t)((q31_t)(*pSrcA++) * (*pSrcB++) + NN_ROUND(out_shift)) >> out_shift), 16);

        /* Decrement the blockSize loop counter */
        blkCnt--;
    }
#endif /*defined (RISCV_VECTOR)*/
}

/**
 * @} end of NNBasicMath group
 */
