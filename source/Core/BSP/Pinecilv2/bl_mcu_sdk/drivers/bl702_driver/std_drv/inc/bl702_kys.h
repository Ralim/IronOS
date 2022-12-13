/**
  ******************************************************************************
  * @file    bl702_kys.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __BL702_KYS_H__
#define __BL702_KYS_H__

#include "kys_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  KYS
 *  @{
 */

/** @defgroup  KYS_Public_Types
 *  @{
 */

/**
 *  @brief KYS keycode type definition
 */
typedef enum {
    KYS_KEYCODE_0, /*!< KYS keycode 0 */
    KYS_KEYCODE_1, /*!< KYS keycode 1 */
    KYS_KEYCODE_2, /*!< KYS keycode 2 */
    KYS_KEYCODE_3, /*!< KYS keycode 3 */
} KYS_Keycode_Type;

/**
 *  @brief KYS configuration structure type definition
 */
typedef struct
{
    uint8_t col;            /*!< Col of keyboard,max:20 */
    uint8_t row;            /*!< Row of keyboard,max:8 */
    uint8_t idleDuration;   /*!< Idle duration between column scans */
    BL_Fun_Type ghostEn;    /*!< Enable or disable ghost key event detection */
    BL_Fun_Type deglitchEn; /*!< Enable or disable deglitch function */
    uint8_t deglitchCnt;    /*!< Deglitch count */
} KYS_CFG_Type;

/*@} end of group KYS_Public_Types */

/** @defgroup  KYS_Public_Constants
 *  @{
 */

/** @defgroup  KYS_KEYCODE_TYPE
 *  @{
 */
#define IS_KYS_KEYCODE_TYPE(type) (((type) == KYS_KEYCODE_0) || \
                                   ((type) == KYS_KEYCODE_1) || \
                                   ((type) == KYS_KEYCODE_2) || \
                                   ((type) == KYS_KEYCODE_3))

/*@} end of group KYS_Public_Constants */

/** @defgroup  KYS_Public_Macros
 *  @{
 */

/*@} end of group KYS_Public_Macros */

/** @defgroup  KYS_Public_Functions
 *  @{
 */

/**
 *  @brief UART Functions
 */
#ifndef BFLB_USE_HAL_DRIVER
void KYS_IRQHandler(void);
#endif
BL_Err_Type KYS_Init(KYS_CFG_Type *kysCfg);
BL_Err_Type KYS_Enable(void);
BL_Err_Type KYS_Disable(void);
BL_Err_Type KYS_IntMask(BL_Mask_Type intMask);
BL_Err_Type KYS_IntClear(void);
BL_Err_Type KYS_Int_Callback_Install(intCallback_Type *cbFun);
uint8_t KYS_GetIntStatus(void);
uint8_t KYS_GetKeycode(KYS_Keycode_Type keycode, uint8_t *col, uint8_t *row);

/*@} end of group KYS_Public_Functions */

/*@} end of group KYS */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_KYS_H__ */
