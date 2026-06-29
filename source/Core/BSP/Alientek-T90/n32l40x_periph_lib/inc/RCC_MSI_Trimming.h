/*****************************************************************************
 * Copyright (c) 2022, Nsing Technologies Pte. Ltd.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nsing' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY Nsing "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL Nsing BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file RCC_MSI_Trimming.h
 * @author Nsing
 * @version V1.2.1
 *
 * @copyright Copyright (c) 2022, Nsingechnologies Pte. Ltd. All rights reserved.
 */
 
 /**
 * @file RCC_MSI_Trimming.h
 * @author Nsing
 * @version V1.2.2
 *
 * @copyright Copyright (c) 2022, Nsingechnologies Pte. Ltd. All rights reserved.
 */
#ifndef __RCC_MSI_TRIMMING_H__
#define __RCC_MSI_TRIMMING_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32l40x.h"


typedef enum
{
    MSI_TRIM = 0,/*fine tuning*/
    MSI_OPT = 1,/*coarse tuning*/
}MSI_TRIM_MODE;

typedef enum
{
    MSI_INC = 0,/* frequency increase */
    MSI_DEC = 1,/* frequency decrease */
}MSI_TRIM_DIRECTION;

typedef enum
{
    MSI_TRIM_SUCCESS = 0,
    MSI_TRIM_ERROR_MODE = 1,
    MSI_TRIM_ERROR_DIR = 2,
    MSI_TRIM_ERROR_VALUE = 3,
}MSI_TRIM_STATE;



/**
 * @brief  Automatically calibrate MSI to eliminate frequency offset generated during packaging
 * @param none
 * @return none.
 */
void RCC_MSI_Trimming_Auto(void);


/**
 * @brief  get the MSI clock trim class.
 * @param opt and trim class point.
 *     @arg value[0] coarse tuning value opt range is (0x00~0x0F)
 *     @arg value[1] is fine tuning value trim range is (0x00~0x0F)
 * @return none.
 */
void RCC_MSI_Trimming_Value_Get_Manual(uint8_t* p_value);



/**
 * @brief  Manually calibrate MSI to eliminate frequency offset generated during reflow.
 * @param mode to choose between coarse or fine tuning MSI.
 *     @arg MSI_TRIM fine tuning MSI frequnce
 *     @arg MSI_OPT coarse tuning MSI frequnce
 * @param dir select the direction of trimming MSI
 *     @arg MSI_INC increase MSI frequency
 *     @arg MSI_DEC decrease MSI frequency
 * @param  value need to adjust the value of MSI frequency the range is 0x00~0x0F
 * @note the param value add the opt or trim value which get by RCC_MSI_Trimming_Value_Get_Manual function
 * @return MSI_TRIM_STATE the following value can be return
 *          - MSI_TRIM_SUCCESS  MSI Trimming success
 *          - MSI_TRIM_ERROR_MODE  param mode illegal
 *          - MSI_TRIM_ERROR_DIR   param dir illegal
 *          - MSI_TRIM_ERROR_VALUE  param trim illegal
 */
MSI_TRIM_STATE RCC_MSI_Trimming_Manual(MSI_TRIM_MODE mode, MSI_TRIM_DIRECTION dir, uint8_t value);



#ifdef __cplusplus
}
#endif

#endif

