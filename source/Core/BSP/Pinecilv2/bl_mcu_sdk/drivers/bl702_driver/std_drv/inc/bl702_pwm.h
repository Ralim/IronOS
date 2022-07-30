/**
  ******************************************************************************
  * @file    bl702_pwm.h
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
#ifndef __BL702_PWM_H__
#define __BL702_PWM_H__

#include "pwm_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PWM
 *  @{
 */

/** @defgroup  PWM_Public_Types
 *  @{
 */

/**
 *  @brief PWM No. type definition
 */
typedef enum {
    PWM_CH0 = 0, /*!< PWM Channel 0 define */
    PWM_CH1,     /*!< PWM Channel 1 define */
    PWM_CH2,     /*!< PWM Channel 2 define */
    PWM_CH3,     /*!< PWM Channel 3 define */
    PWM_CH4,     /*!< PWM Channel 4 define */
    PWM_CH_MAX,  /*!<  */
} PWM_CH_ID_Type;

/**
 *  @brief PWM Clock definition
 */
typedef enum {
    PWM_CLK_XCLK = 0, /*!< PWM Clock source :XTAL CLK */
    PWM_CLK_BCLK,     /*!< PWM Clock source :Bus CLK */
    PWM_CLK_32K,      /*!< PWM Clock source :32K CLK */
} PWM_Clk_Type;

/**
 *  @brief PWM Stop Mode definition
 */
typedef enum {
    PWM_STOP_ABRUPT = 0, /*!< PWM stop abrupt select define */
    PWM_STOP_GRACEFUL,   /*!< PWM stop graceful select define */
} PWM_Stop_Mode_Type;

/**
 *  @brief PWM mode type def
 */
typedef enum {
    PWM_POL_NORMAL = 0, /*!< PWM normal polarity mode define */
    PWM_POL_INVERT,     /*!< PWM invert polarity mode define */
} PWM_Polarity_Type;

/**
 *  @brief PWM interrupt type def
 */
typedef enum {
    PWM_INT_PULSE_CNT = 0, /*!< PWM Pulse count interrupt define */
    PWM_INT_ALL,           /*!<  */
} PWM_INT_Type;

/**
 *  @brief PWM configuration structure type definition
 */
typedef struct
{
    PWM_CH_ID_Type ch;           /*!< PWM channel */
    PWM_Clk_Type clk;            /*!< PWM Clock */
    PWM_Stop_Mode_Type stopMode; /*!< PWM Stop Mode */
    PWM_Polarity_Type pol;       /*!< PWM mode type */
    uint16_t clkDiv;             /*!< PWM clkDiv num */
    uint16_t period;             /*!< PWM period set */
    uint16_t threshold1;         /*!< PWM threshold1 num */
    uint16_t threshold2;         /*!< PWM threshold2 num */
    uint16_t intPulseCnt;        /*!< PWM interrupt pulse count */
} PWM_CH_CFG_Type;

/*@} end of group PWM_Public_Types */

/** @defgroup  PWM_Public_Constants
 *  @{
 */

/** @defgroup  PWM_CH_ID_TYPE
 *  @{
 */
#define IS_PWM_CH_ID_TYPE(type) (((type) == PWM_CH0) || \
                                 ((type) == PWM_CH1) || \
                                 ((type) == PWM_CH2) || \
                                 ((type) == PWM_CH3) || \
                                 ((type) == PWM_CH4) || \
                                 ((type) == PWM_CH_MAX))

/** @defgroup  PWM_CLK_TYPE
 *  @{
 */
#define IS_PWM_CLK_TYPE(type) (((type) == PWM_CLK_XCLK) || \
                               ((type) == PWM_CLK_BCLK) || \
                               ((type) == PWM_CLK_32K))

/** @defgroup  PWM_STOP_MODE_TYPE
 *  @{
 */
#define IS_PWM_STOP_MODE_TYPE(type) (((type) == PWM_STOP_ABRUPT) || \
                                     ((type) == PWM_STOP_GRACEFUL))

/** @defgroup  PWM_POLARITY_TYPE
 *  @{
 */
#define IS_PWM_POLARITY_TYPE(type) (((type) == PWM_POL_NORMAL) || \
                                    ((type) == PWM_POL_INVERT))

/** @defgroup  PWM_INT_TYPE
 *  @{
 */
#define IS_PWM_INT_TYPE(type) (((type) == PWM_INT_PULSE_CNT) || \
                               ((type) == PWM_INT_ALL))

/*@} end of group PWM_Public_Constants */

/** @defgroup  PWM_Public_Macros
 *  @{
 */
#define IS_PWM_CH(CH) ((CH) < PWM_CH_MAX)

/*@} end of group PWM_Public_Macros */

/** @defgroup  PWM_Public_Functions
 *  @{
 */

/**
 *  @brief PWM Functions
 */
#ifndef BFLB_USE_HAL_DRIVER
void PWM_IRQHandler(void);
#endif
BL_Err_Type PWM_Channel_Init(PWM_CH_CFG_Type *chCfg);
void PWM_Channel_Update(PWM_CH_ID_Type ch, uint16_t period, uint16_t threshold1, uint16_t threshold2);
void PWM_Channel_Set_Div(PWM_CH_ID_Type ch, uint16_t div);
void PWM_Channel_Set_Threshold1(PWM_CH_ID_Type ch, uint16_t threshold1);
void PWM_Channel_Set_Threshold2(PWM_CH_ID_Type ch, uint16_t threshold2);
void PWM_Channel_Set_Period(PWM_CH_ID_Type ch, uint16_t period);
void PWM_Channel_Get(PWM_CH_ID_Type ch, uint16_t *period, uint16_t *threshold1, uint16_t *threshold2);
void PWM_IntMask(PWM_CH_ID_Type ch, PWM_INT_Type intType, BL_Mask_Type intMask);
void PWM_Channel_Enable(PWM_CH_ID_Type ch);
void PWM_Channel_Disable(PWM_CH_ID_Type ch);
void PWM_SW_Mode(PWM_CH_ID_Type ch, BL_Fun_Type enable);
void PWM_SW_Force_Value(PWM_CH_ID_Type ch, uint8_t value);
void PWM_Int_Callback_Install(PWM_CH_ID_Type ch, uint32_t intType, intCallback_Type *cbFun);
BL_Err_Type PWM_Smart_Configure(PWM_CH_ID_Type ch, uint32_t frequency, uint8_t dutyCycle);


/*@} end of group PWM_Public_Functions */

/*@} end of group PWM */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_PWM_H__ */
