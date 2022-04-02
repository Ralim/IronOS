
/* ----------------------------------------------------------------------
 * Project:      NMSIS DSP Library
 * Title:        riscv_boolean_distance.c
 * Description:  Templates for boolean distances
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




/**
 * @defgroup DISTANCEF Distance Functions
 *
 * Computes Distances between vectors. 
 *
 * Distance functions are useful in a lot of algorithms.
 *
 */


/**
 * @addtogroup DISTANCEF
 * @{
 */




#define _FUNC(A,B) A##B 

#define FUNC(EXT) _FUNC(riscv_boolean_distance, EXT)

/**
 * @brief        Elements of boolean distances
 *
 * Different values which are used to compute boolean distances
 *
 * @param[in]    pA              First vector of packed booleans
 * @param[in]    pB              Second vector of packed booleans
 * @param[in]    numberOfBools   Number of booleans
 * @return None
 *
 */


void FUNC(EXT)(const uint32_t *pA
       , const uint32_t *pB
       , uint32_t numberOfBools
#ifdef TT
       , uint32_t *cTT
#endif
#ifdef FF
       , uint32_t *cFF
#endif
#ifdef TF
       , uint32_t *cTF
#endif
#ifdef FT
       , uint32_t *cFT
#endif
       )
{
  
#ifdef TT
    uint32_t _ctt=0;
#endif
#ifdef FF
    uint32_t _cff=0;
#endif
#ifdef TF
    uint32_t _ctf=0;
#endif
#ifdef FT
    uint32_t _cft=0;
#endif
    uint32_t a,b,ba,bb;
    int shift;

    while(numberOfBools >= 32)
    {
       a = *pA++;
       b = *pB++;
       shift = 0;
       while(shift < 32)
       {
          ba = a & 1;
          bb = b & 1;
          a = a >> 1;
          b = b >> 1;
#ifdef TT
          _ctt += (ba && bb);
#endif
#ifdef FF
          _cff += ((1 ^ ba) && (1 ^ bb));
#endif
#ifdef TF
          _ctf += (ba && (1 ^ bb));
#endif
#ifdef FT
          _cft += ((1 ^ ba) && bb);
#endif
          shift ++;
       }

       numberOfBools -= 32;
    }

    a = *pA++;
    b = *pB++;

    a = a >> (32 - numberOfBools);
    b = b >> (32 - numberOfBools);

    while(numberOfBools > 0)
    {
          ba = a & 1;
          bb = b & 1;
          a = a >> 1;
          b = b >> 1;

#ifdef TT
          _ctt += (ba && bb);
#endif
#ifdef FF
          _cff += ((1 ^ ba) && (1 ^ bb));
#endif
#ifdef TF
          _ctf += (ba && (1 ^ bb));
#endif
#ifdef FT
          _cft += ((1 ^ ba) && bb);
#endif
          numberOfBools --;
    }

#ifdef TT
    *cTT = _ctt;
#endif
#ifdef FF
    *cFF = _cff;
#endif
#ifdef TF 
    *cTF = _ctf;
#endif
#ifdef FT
    *cFT = _cft;
#endif
}


/**
 * @} end of DISTANCEF group
 */
