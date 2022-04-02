/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_spline_interp_f32.c
 * Description:  Floating-point cubic spline interpolation
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

#include "dsp/interpolation_functions.h"

/**
  @ingroup groupInterpolation
 */

/**
  @defgroup SplineInterpolate Cubic Spline Interpolation
 
  Spline interpolation is a method of interpolation where the interpolant
  is a piecewise-defined polynomial called "spline". 
 
  @par Introduction

  Given a function f defined on the interval [a,b], a set of n nodes x(i) 
  where a=x(1)<x(2)<...<x(n)=b and a set of n values y(i) = f(x(i)), 
  a cubic spline interpolant S(x) is defined as: 

  <pre>
          S1(x)       x(1) < x < x(2)
  S(x) =   ...         
          Sn-1(x)   x(n-1) < x < x(n)
  </pre>

  where

  <pre> 
  Si(x) = a_i+b_i(x-xi)+c_i(x-xi)^2+d_i(x-xi)^3    i=1, ..., n-1
  </pre>
 
  @par Algorithm

  Having defined h(i) = x(i+1) - x(i)

  <pre>
  h(i-1)c(i-1)+2[h(i-1)+h(i)]c(i)+h(i)c(i+1) = 3/h(i)*[a(i+1)-a(i)]-3/h(i-1)*[a(i)-a(i-1)]    i=2, ..., n-1
  </pre>

  It is possible to write the previous conditions in matrix form (Ax=B).
  In order to solve the system two boundary conidtions are needed.
  - Natural spline: S1''(x1)=2*c(1)=0 ; Sn''(xn)=2*c(n)=0
  In matrix form:

  <pre>
  |  1        0         0  ...    0         0           0     ||  c(1)  | |                        0                        |
  | h(0) 2[h(0)+h(1)] h(1) ...    0         0           0     ||  c(2)  | |      3/h(2)*[a(3)-a(2)]-3/h(1)*[a(2)-a(1)]      |
  | ...      ...       ... ...   ...       ...         ...    ||  ...   |=|                       ...                       |
  |  0        0         0  ... h(n-2) 2[h(n-2)+h(n-1)] h(n-1) || c(n-1) | | 3/h(n-1)*[a(n)-a(n-1)]-3/h(n-2)*[a(n-1)-a(n-2)] |
  |  0        0         0  ...    0         0           1     ||  c(n)  | |                        0                        |
  </pre>

  - Parabolic runout spline: S1''(x1)=2*c(1)=S2''(x2)=2*c(2) ; Sn-1''(xn-1)=2*c(n-1)=Sn''(xn)=2*c(n)
  In matrix form:

  <pre>
  |  1       -1         0  ...    0         0           0     ||  c(1)  | |                        0                        |
  | h(0) 2[h(0)+h(1)] h(1) ...    0         0           0     ||  c(2)  | |      3/h(2)*[a(3)-a(2)]-3/h(1)*[a(2)-a(1)]      |
  | ...      ...       ... ...   ...       ...         ...    ||  ...   |=|                       ...                       |
  |  0        0         0  ... h(n-2) 2[h(n-2)+h(n-1)] h(n-1) || c(n-1) | | 3/h(n-1)*[a(n)-a(n-1)]-3/h(n-2)*[a(n-1)-a(n-2)] |
  |  0        0         0  ...    0        -1           1     ||  c(n)  | |                        0                        |
  </pre>

  A is a tridiagonal matrix (a band matrix of bandwidth 3) of size N=n+1. The factorization
  algorithms (A=LU) can be simplified considerably because a large number of zeros appear
  in regular patterns. The Crout method has been used:
  1) Solve LZ=B

  <pre>
  u(1,2) = A(1,2)/A(1,1)
  z(1)   = B(1)/l(11)
 
  FOR i=2, ..., N-1
    l(i,i)   = A(i,i)-A(i,i-1)u(i-1,i)
    u(i,i+1) = a(i,i+1)/l(i,i)
    z(i)     = [B(i)-A(i,i-1)z(i-1)]/l(i,i)
  
  l(N,N) = A(N,N)-A(N,N-1)u(N-1,N)
  z(N)   = [B(N)-A(N,N-1)z(N-1)]/l(N,N)
  </pre>

  2) Solve UX=Z

  <pre>
  c(N)=z(N)
  
  FOR i=N-1, ..., 1
    c(i)=z(i)-u(i,i+1)c(i+1) 
  </pre>

  c(i) for i=1, ..., n-1 are needed to compute the n-1 polynomials. 
  b(i) and d(i) are computed as:
  - b(i) = [y(i+1)-y(i)]/h(i)-h(i)*[c(i+1)+2*c(i)]/3 
  - d(i) = [c(i+1)-c(i)]/[3*h(i)] 
  Moreover, a(i)=y(i).

 @par Behaviour outside the given intervals

  It is possible to compute the interpolated vector for x values outside the 
  input range (xq<x(1); xq>x(n)). The coefficients used to compute the y values for
  xq<x(1) are going to be the ones used for the first interval, while for xq>x(n) the 
  coefficients used for the last interval.
 
 */

/**
  @addtogroup SplineInterpolate
  @{
 */

/**
 * @brief Processing function for the floating-point cubic spline interpolation.
 * @param[in]  S          points to an instance of the floating-point spline structure.
 * @param[in]  xq         points to the x values of the interpolated data points.
 * @param[out] pDst       points to the block of output data.
 * @param[in]  blockSize  number of samples of output data.
 */

void riscv_spline_f32(
        riscv_spline_instance_f32 * S, 
  const float32_t * xq,
        float32_t * pDst,
        uint32_t blockSize)
{
    const float32_t * x = S->x;
    const float32_t * y = S->y;
    int32_t n = S->n_x;

    /* Coefficients (a==y for i<=n-1) */
    float32_t * b = (S->coeffs);
    float32_t * c = (S->coeffs)+(n-1);
    float32_t * d = (S->coeffs)+(2*(n-1));    

    const float32_t * pXq = xq;
    int32_t blkCnt = (int32_t)blockSize;
    int32_t blkCnt2;
    int32_t i;
    float32_t x_sc;

#if defined(RISCV_VECTOR)
    uint32_t blkCnt_v;                               /* Loop counter */
    size_t l;
    float32_t temp_max;
    vfloat32m8_t v_x, v_y;
    vfloat32m8_t v_a, v_b;
    vfloat32m1_t v_temp;
    l = vsetvl_e32m1(1);
    v_temp = vfsub_vv_f32m1(v_temp, v_temp, l);
#endif


    /* Create output for x(i)<x<x(i+1) */
    for (i=0; i<n-1; i++)
    {

        while( *pXq <= x[i+1] && blkCnt > 0 )
        {
            x_sc = *pXq++;

            *pDst = y[i]+b[i]*(x_sc-x[i])+c[i]*(x_sc-x[i])*(x_sc-x[i])+d[i]*(x_sc-x[i])*(x_sc-x[i])*(x_sc-x[i]);

            pDst++;
            blkCnt--;
        }
    }

    /* Create output for remaining samples (x>=x(n)) */
    blkCnt2 = blkCnt;                                          
#if defined(RISCV_VECTOR)
    blkCnt_v = blkCnt;
    l = vsetvl_e32m8(blkCnt_v);
    v_a = vfsub_vv_f32m8(v_a,v_a, l);
    for (; (l = vsetvl_e32m8(blkCnt_v)) > 0; blkCnt_v -= l) {
      v_x = vle32_v_f32m8(pXq, l);
      v_x = vfsub_vf_f32m8(v_x,x[i-1], l);
      v_y = vfmul_vv_f32m8(v_x,v_x, l);
      v_b = vfmul_vv_f32m8(v_y,v_x, l);
      v_x = vfadd_vf_f32m8(vfmul_vf_f32m8(v_x,b[i-1], l),y[i-1], l);
      v_y = vfadd_vv_f32m8(vfmul_vf_f32m8(v_y,c[i-1], l),vfmul_vf_f32m8(v_b,d[i-1], l), l);
      v_x = vfadd_vv_f32m8(v_x,v_y, l);
      vse32_v_f32m8(pDst,v_x, l);
      pXq += l;
      pDst += l;
    }
#else
    while(blkCnt2 > 0)                                       
    { 
        x_sc = *pXq++; 
  
        *pDst = y[i-1]+b[i-1]*(x_sc-x[i-1])+c[i-1]*(x_sc-x[i-1])*(x_sc-x[i-1])+d[i-1]*(x_sc-x[i-1])*(x_sc-x[i-1])*(x_sc-x[i-1]);
 
        pDst++; 
        blkCnt2--;   
    }   
#endif
}
/**
  @} end of SplineInterpolate group
 */
