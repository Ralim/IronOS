/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_scale_q31.c
 * Description:  Multiplies a Q31 vector by a scalar
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

#include "dsp/basic_math_functions.h"

/**
  @ingroup groupMath
 */

/**
  @addtogroup BasicScale
  @{
 */

/**
  @brief         Multiplies a Q31 vector by a scalar.
  @param[in]     pSrc       points to the input vector
  @param[in]     scaleFract fractional portion of the scale value
  @param[in]     shift      number of bits to shift the result by
  @param[out]    pDst       points to the output vector
  @param[in]     blockSize  number of samples in each vector
  @return        none

  @par           Scaling and Overflow Behavior
                   The input data <code>*pSrc</code> and <code>scaleFract</code> are in 1.31 format.
                   These are multiplied to yield a 2.62 intermediate result and this is shifted with saturation to 1.31 format.
 */

void riscv_scale_q31(
  const q31_t *pSrc,
        q31_t scaleFract,
        int8_t shift,
        q31_t *pDst,
        uint32_t blockSize)
{
#if defined(RISCV_VECTOR)&&((__XLEN!=32)||(__FLEN!=32))
  uint32_t blkCnt = blockSize;                               /* Loop counter */
  q31_t in, out;                                             /* Temporary variables */
  int8_t kShift = shift + 1;                                 /* Shift to apply after scaling */
  int8_t sign = (kShift & 0x80);
  size_t l;
  vint32m4_t v_in;
  vint32m4_t v_out;

  /* C = A * scale *
  /* Scale input and store result in destination buffer. */     
  for (; (l = vsetvl_e32m4(blkCnt)) > 0; blkCnt -= l) {
    v_in = vle32_v_i32m4(pSrc, l);
    pSrc += l;
    /* If the shift value is positive then do right shift else left shiftvmin_vx_i64m8(vmax_vx_i64m8( , 0xffffffff80000000),0x7fffffff)vmerge_vvm_i32m4(vmsne_vv_i32m4_b8(v_in, v_out), vxor_vx_i32m4(vsra_vx_i32m4(v_in, 31U), 0x7FFFFFFF), */
    if (sign == 0U)
    {
      v_out = vnclip_wx_i32m4(vsll_vx_i64m8(vsra_vx_i64m8(vwmul_vx_i64m8(v_in, scaleFract, l),32U, l), (uint8_t)kShift, l),0, l);
      //if (v_in != (vsll_vx_i32m4(v_out, kShift)))
      vse32_v_i32m4 (pDst, v_out, l);
     /*  {
         v_out = vxor_vx_i32m4(vsra_vx_i32m4(v_in, 31U), 0x7FFFFFFF);
         vse32_v_i32m4 (pDst, v_out);
      } */
      pDst += l;
    }
    else
    {
      /* C = A * scale */
      /* Scale input and store result in destination buffer. */
      v_in = vnsra_wx_i32m4(vwmul_vx_i64m8(v_in, scaleFract, l), 32U, l);
      v_out = vsra_vx_i32m4(v_in, -kShift, l);
      vse32_v_i32m4 (pDst, v_out, l);
      pDst += l;
    }
    
  }
#else
        uint32_t blkCnt;                               /* Loop counter */
        q31_t in, out;                                 /* Temporary variables */
        int8_t kShift = shift + 1;                     /* Shift to apply after scaling */
        int8_t sign = (kShift & 0x80);

#if defined (RISCV_MATH_LOOPUNROLL)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = blockSize >> 2U;

  if (sign == 0U)
  {
    while (blkCnt > 0U)
    {
      /* C = A * scale */

      /* Scale input and store result in destination buffer. */
      in = *pSrc++;                                /* read input from source */
      in = ((q63_t) in * scaleFract) >> 32;        /* multiply input with scaler value */
      out = in << kShift;                          /* apply shifting */
      if (in != (out >> kShift))                   /* saturate the result */
        out = 0x7FFFFFFF ^ (in >> 31);
      *pDst++ = out;                               /* Store result destination */

      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
        out = 0x7FFFFFFF ^ (in >> 31);
      *pDst++ = out;

      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
        out = 0x7FFFFFFF ^ (in >> 31);
      *pDst++ = out;

      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
        out = 0x7FFFFFFF ^ (in >> 31);
      *pDst++ = out;

      /* Decrement loop counter */
      blkCnt--;
    }
  }
  else
  {
    while (blkCnt > 0U)
    {
      /* C = A * scale */

      /* Scale input and store result in destination buffer. */
      in = *pSrc++;                                /* read four inputs from source */
      in = ((q63_t) in * scaleFract) >> 32;        /* multiply input with scaler value */
      out = in >> -kShift;                         /* apply shifting */
      *pDst++ = out;                               /* Store result destination */

      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in >> -kShift;
      *pDst++ = out;

      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in >> -kShift;
      *pDst++ = out;

      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in >> -kShift;
      *pDst++ = out;

      /* Decrement loop counter */
      blkCnt--;
    }
  }

  /* Loop unrolling: Compute remaining outputs */
  blkCnt = blockSize % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = blockSize;

#endif /* #if defined (RISCV_MATH_LOOPUNROLL) */

  if (sign == 0U)
  {
    while (blkCnt > 0U)
    {
      /* C = A * scale */

      /* Scale input and store result in destination buffer. */
      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in << kShift;
      if (in != (out >> kShift))
          out = 0x7FFFFFFF ^ (in >> 31);
      *pDst++ = out;

      /* Decrement loop counter */
      blkCnt--;
    }
  }
  else
  {
    while (blkCnt > 0U)
    {
      /* C = A * scale */

      /* Scale input and store result in destination buffer. */
      in = *pSrc++;
      in = ((q63_t) in * scaleFract) >> 32;
      out = in >> -kShift;
      *pDst++ = out;

      /* Decrement loop counter */
      blkCnt--;
    }
  }
#endif /* defined(RISCV_VECTOR) */
}

/**
  @} end of BasicScale group
 */
