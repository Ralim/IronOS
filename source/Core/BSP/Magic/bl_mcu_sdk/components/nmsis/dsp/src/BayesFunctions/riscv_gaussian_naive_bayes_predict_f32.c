/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_naive_gaussian_bayes_predict_f32
 * Description:  Naive Gaussian Bayesian Estimator
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

#include "dsp/bayes_functions.h"
#include <limits.h>
#include <math.h>

#define PI_F 3.1415926535897932384626433832795f
#define DPI_F (2.0f*3.1415926535897932384626433832795f)

/**
 * @addtogroup groupBayes
 * @{
 */

/**
 * @brief Naive Gaussian Bayesian Estimator
 *
 * @param[in]   *S                      points to a naive bayes instance structure
 * @param[in]   *in                     points to the elements of the input vector.
 * @param[out]  *pOutputProbabilities   points to a buffer of length numberOfClasses containing estimated probabilities
 * @param[out]  *pBufferB               points to a temporary buffer of length numberOfClasses
 * @return The predicted class
 *
 *
 */



uint32_t riscv_gaussian_naive_bayes_predict_f32(const riscv_gaussian_naive_bayes_instance_f32 *S, 
   const float32_t * in, 
   float32_t *pOutputProbabilities,
   float32_t *pBufferB)
{
    uint32_t nbClass;
    uint32_t nbDim;
    const float32_t *pPrior = S->classPriors;
    const float32_t *pTheta = S->theta;
    const float32_t *pSigma = S->sigma;
    float32_t *buffer = pOutputProbabilities;
    const float32_t *pIn=in;
    float32_t result;
    float32_t sigma;
    float32_t tmp;
    float32_t acc1,acc2;
    uint32_t index;

    (void)pBufferB;

    pTheta=S->theta;
    pSigma=S->sigma;

    for(nbClass = 0; nbClass < S->numberOfClasses; nbClass++)
    {

        
        pIn = in;

        tmp = 0.0;
        acc1 = 0.0f;
        acc2 = 0.0f;
        for(nbDim = 0; nbDim < S->vectorDimension; nbDim++)
        {
           sigma = *pSigma + S->epsilon;
           acc1 += logf(2.0f * PI_F * sigma);
           acc2 += (*pIn - *pTheta) * (*pIn - *pTheta) / sigma;

           pIn++;
           pTheta++;
           pSigma++;
        }

        tmp = -0.5f * acc1;
        tmp -= 0.5f * acc2;


        *buffer = tmp + logf(*pPrior++);
        buffer++;
    }

    riscv_max_f32(pOutputProbabilities,S->numberOfClasses,&result,&index);

    return(index);
}


/**
 * @} end of groupBayes group
 */
