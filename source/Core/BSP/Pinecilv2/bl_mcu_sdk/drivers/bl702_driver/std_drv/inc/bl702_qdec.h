/**
  ******************************************************************************
  * @file    bl702_qdec.h
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
#ifndef __BL702_QDEC_H__
#define __BL702_QDEC_H__

#include "qdec_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  QDEC
 *  @{
 */

/** @defgroup  QDEC_Public_Types
 *  @{
 */

/**
 *  @brief QDEC port type definition
 */
typedef enum {
    QDEC0_ID,    /*!< QDEC0 port define */
    QDEC1_ID,    /*!< QDEC1 port define */
    QDEC2_ID,    /*!< QDEC2 port define */
    QDEC_ID_MAX, /*!< QDEC MAX ID define */
} QDEC_ID_Type;

/**
 *  @brief QDEC sample work mode type definition
 */
typedef enum {
    QDEC_SAMPLE_SINGLE_MOD,   /*!< Stop sample if rpt_rdy */
    QDEC_SAMPLE_CONTINUE_MOD, /*!< Continue sample */
} QDEC_SAMPLE_MODE_Type;

/**
 *  @brief QDEC sample period type definition
 */
typedef enum {
    QDEC_SAMPLE_PERIOD_32US,  /*!< 32  us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_64US,  /*!< 64  us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_128US, /*!< 128 us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_256US, /*!< 256 us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_512US, /*!< 512 us at clock 1MHz */
    QDEC_SAMPLE_PERIOD_1MS,   /*!< 1   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_2MS,   /*!< 2   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_4MS,   /*!< 4   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_8MS,   /*!< 8   ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_16MS,  /*!< 16  ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_32MS,  /*!< 32  ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_65MS,  /*!< 65  ms at clock 1MHz */
    QDEC_SAMPLE_PERIOD_131MS, /*!< 131 ms at clock 1MHz */
} QDEC_SAMPLE_PERIOD_Type;

/**
 *  @brief QDEC report work mode type definition
 */
typedef enum {
    QDEC_REPORT_SAMPLE_CHANGE_MOD, /*!< Count time only if sample change */
    QDEC_REPORT_TIME_MOD,          /*!< Continue time */
} QDEC_REPORT_MODE_Type;

/**
 *  @brief QDEC sample config structure type definition
 */
typedef struct
{
    QDEC_SAMPLE_MODE_Type sampleMod;      /*!< Sample work mode */
    QDEC_SAMPLE_PERIOD_Type samplePeriod; /*!< Sample period time */
} QDEC_SAMPLE_Type;

/**
 *  @brief QDEC report config structure type definition
 */
typedef struct
{
    QDEC_REPORT_MODE_Type reportMod; /*!< Report work mode */
    uint16_t reportPeriod;           /*!< RPT_US report period in [us/report] = SP * RP */
} QDEC_REPORT_Type;

/**
 *  @brief QDEC acc work mode type definition
 */
typedef enum {
    QDEC_ACC_STOP_SAMPLE_IF_OVERFLOW, /*!< Stop accumulate if overflow */
    QDEC_ACC_CONTINUE_ACCUMULATE,     /*!< Continue accumulate */
} QDEC_ACC_MODE_Type;

/**
 *  @brief QDEC led config structure type definition
 */
typedef struct
{
    BL_Fun_Type ledEn;   /*!< LED enable */
    BL_Fun_Type ledSwap; /*!< LED on/off polarity swap */
    uint16_t ledPeriod;  /*!< Period in us the LED is switched on prior to sampling */
} QDEC_LED_Type;

/**
 *  @brief QDEC deglitch config structure type definition
 */
typedef struct
{
    BL_Fun_Type deglitchEn;   /*!< deglitch enable */
    uint8_t deglitchStrength; /*!< deglitch strength */
} QDEC_DEGLITCH_Type;

/**
 *  @brief QDEC config structure type definition
 */
typedef struct
{
    QDEC_SAMPLE_Type sampleCfg;     /*!< QDEC sample config structure */
    QDEC_REPORT_Type reportCfg;     /*!< QDEC report config structure */
    QDEC_ACC_MODE_Type accMod;      /*!< QDEC acc mode config */
    QDEC_LED_Type ledCfg;           /*!< QDEC led config structure */
    QDEC_DEGLITCH_Type deglitchCfg; /*!< QDEC deglitch config structure */
} QDEC_CFG_Type;

/**
 *  @brief QDEC direction type definition
 */
typedef enum {
    QDEC_DIRECTION_NO_CHANGE,         /*!< Direction of last change no change */
    QDEC_DIRECTION_CLOCKWISE,         /*!< Direction of last change clockwise */
    QDEC_DIRECTION_COUNTER_CLOCKWISE, /*!< Direction of last change counter-clockwise */
    QDEC_DIRECTION_ERROR,             /*!< Direction of last change error */
} QDEC_DIRECTION_Type;

/**
 *  @brief QDEC interrupt type definition
 */
typedef enum {
    QDEC_INT_REPORT,   /*!< report interrupt */
    QDEC_INT_SAMPLE,   /*!< sample interrupt */
    QDEC_INT_ERROR,    /*!< error interrupt */
    QDEC_INT_OVERFLOW, /*!< ACC1 and ACC2 overflow interrupt */
    QDEC_INT_ALL,      /*!< interrupt max num */
} QDEC_INT_Type;

/*@} end of group QDEC_Public_Types */

/** @defgroup  QDEC_Public_Constants
 *  @{
 */

/** @defgroup  QDEC_ID_TYPE
 *  @{
 */
#define IS_QDEC_ID_TYPE(type) (((type) == QDEC0_ID) || \
                               ((type) == QDEC1_ID) || \
                               ((type) == QDEC2_ID) || \
                               ((type) == QDEC_ID_MAX))

/** @defgroup  QDEC_SAMPLE_MODE_TYPE
 *  @{
 */
#define IS_QDEC_SAMPLE_MODE_TYPE(type) (((type) == QDEC_SAMPLE_SINGLE_MOD) || \
                                        ((type) == QDEC_SAMPLE_CONTINUE_MOD))

/** @defgroup  QDEC_SAMPLE_PERIOD_TYPE
 *  @{
 */
#define IS_QDEC_SAMPLE_PERIOD_TYPE(type) (((type) == QDEC_SAMPLE_PERIOD_32US) ||  \
                                          ((type) == QDEC_SAMPLE_PERIOD_64US) ||  \
                                          ((type) == QDEC_SAMPLE_PERIOD_128US) || \
                                          ((type) == QDEC_SAMPLE_PERIOD_256US) || \
                                          ((type) == QDEC_SAMPLE_PERIOD_512US) || \
                                          ((type) == QDEC_SAMPLE_PERIOD_1MS) ||   \
                                          ((type) == QDEC_SAMPLE_PERIOD_2MS) ||   \
                                          ((type) == QDEC_SAMPLE_PERIOD_4MS) ||   \
                                          ((type) == QDEC_SAMPLE_PERIOD_8MS) ||   \
                                          ((type) == QDEC_SAMPLE_PERIOD_16MS) ||  \
                                          ((type) == QDEC_SAMPLE_PERIOD_32MS) ||  \
                                          ((type) == QDEC_SAMPLE_PERIOD_65MS) ||  \
                                          ((type) == QDEC_SAMPLE_PERIOD_131MS))

/** @defgroup  QDEC_REPORT_MODE_TYPE
 *  @{
 */
#define IS_QDEC_REPORT_MODE_TYPE(type) (((type) == QDEC_REPORT_SAMPLE_CHANGE_MOD) || \
                                        ((type) == QDEC_REPORT_TIME_MOD))

/** @defgroup  QDEC_ACC_MODE_TYPE
 *  @{
 */
#define IS_QDEC_ACC_MODE_TYPE(type) (((type) == QDEC_ACC_STOP_SAMPLE_IF_OVERFLOW) || \
                                     ((type) == QDEC_ACC_CONTINUE_ACCUMULATE))

/** @defgroup  QDEC_DIRECTION_TYPE
 *  @{
 */
#define IS_QDEC_DIRECTION_TYPE(type) (((type) == QDEC_DIRECTION_NO_CHANGE) ||         \
                                      ((type) == QDEC_DIRECTION_CLOCKWISE) ||         \
                                      ((type) == QDEC_DIRECTION_COUNTER_CLOCKWISE) || \
                                      ((type) == QDEC_DIRECTION_ERROR))

/** @defgroup  QDEC_INT_TYPE
 *  @{
 */
#define IS_QDEC_INT_TYPE(type) (((type) == QDEC_INT_REPORT) ||   \
                                ((type) == QDEC_INT_SAMPLE) ||   \
                                ((type) == QDEC_INT_ERROR) ||    \
                                ((type) == QDEC_INT_OVERFLOW) || \
                                ((type) == QDEC_INT_ALL))

/*@} end of group QDEC_Public_Constants */

/** @defgroup  QDEC_Public_Macros
 *  @{
 */

/*@} end of group QDEC_Public_Macros */

/** @defgroup  QDEC_Public_Functions
 *  @{
 */

/**
 *  @brief QDEC Functions
 */
#ifndef BFLB_USE_HAL_DRIVER
void QDEC0_IRQHandler(void);
void QDEC1_IRQHandler(void);
void QDEC2_IRQHandler(void);
#endif
void QDEC_Init(QDEC_ID_Type qdecId, QDEC_CFG_Type *qdecCfg);
void QDEC_DeInit(QDEC_ID_Type qdecId);
void QDEC_Enable(QDEC_ID_Type qdecId);
void QDEC_Disable(QDEC_ID_Type qdecId);
void QDEC_SetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType, BL_Mask_Type intMask);
BL_Mask_Type QDEC_GetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType);
void QDEC_Int_Callback_Install(QDEC_ID_Type qdecId, QDEC_INT_Type intType, intCallback_Type *cbFun);
BL_Sts_Type QDEC_Get_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType);
void QDEC_Clr_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType);
QDEC_DIRECTION_Type QDEC_Get_Sample_Direction(QDEC_ID_Type qdecId);
uint8_t QDEC_Get_Err_Cnt(QDEC_ID_Type qdecId);
uint16_t QDEC_Get_Sample_Val(QDEC_ID_Type qdecId);
void QDEC_IntHandler(QDEC_ID_Type qdecId, QDEC_INT_Type intType);

/*@} end of group QDEC_Public_Functions */

/*@} end of group QDEC */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_QDEC_H__ */
