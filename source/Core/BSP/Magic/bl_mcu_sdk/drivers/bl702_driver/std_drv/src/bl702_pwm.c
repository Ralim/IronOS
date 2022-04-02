/**
  ******************************************************************************
  * @file    bl702_pwm.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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

#include "bl702_pwm.h"
#include "bl702_glb.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  PWM
 *  @{
 */

/** @defgroup  PWM_Private_Macros
 *  @{
 */
#define PWM_Get_Channel_Reg(ch) (PWM_BASE + PWM_CHANNEL_OFFSET + (ch)*0x20)
#define PWM_INT_TIMEOUT_COUNT   (160 * 1000)
#define PWM_STOP_TIMEOUT_COUNT  (160 * 1000)

/*@} end of group PWM_Private_Macros */

/** @defgroup  PWM_Private_Types
 *  @{
 */

/*@} end of group PWM_Private_Types */

/** @defgroup  PWM_Private_Variables
 *  @{
 */

/**
 *  @brief PWM interrupt callback function address array
 */
static intCallback_Type *PWMIntCbfArra[PWM_CH_MAX][PWM_INT_ALL] = {
    { NULL }
};

/*@} end of group PWM_Private_Variables */

/** @defgroup  PWM_Global_Variables
 *  @{
 */

/*@} end of group PWM_Global_Variables */

/** @defgroup  PWM_Private_Fun_Declaration
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
static BL_Err_Type PWM_IntHandler(IRQn_Type intPeriph);
#endif

/*@} end of group PWM_Private_Fun_Declaration */

/** @defgroup  PWM_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  PWM interrupt handle
 *
 * @param  intPeriph: Select the peripheral, such as PWM0_IRQn
 *
 * @return SUCCESS
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
static BL_Err_Type PWM_IntHandler(IRQn_Type intPeriph)
{
    uint32_t i;
    uint32_t tmpVal;
    uint32_t timeoutCnt = PWM_INT_TIMEOUT_COUNT;
    /* Get channel register */
    uint32_t PWMx = PWM_BASE;

    for (i = 0; i < PWM_CH_MAX; i++) {
        tmpVal = BL_RD_REG(PWMx, PWM_INT_CONFIG);

        if ((BL_GET_REG_BITS_VAL(tmpVal, PWM_INTERRUPT_STS) & (1 << i)) != 0) {
            /* Clear interrupt */
            tmpVal |= (1 << (i + PWM_INT_CLEAR_POS));
            BL_WR_REG(PWMx, PWM_INT_CONFIG, tmpVal);

            /* FIXME: we need set pwm_int_clear to 0 by software and
               before this,we must make sure pwm_interrupt_sts is 0*/
            do {
                tmpVal = BL_RD_REG(PWMx, PWM_INT_CONFIG);
                timeoutCnt--;

                if (timeoutCnt == 0) {
                    break;
                }
            } while (BL_GET_REG_BITS_VAL(tmpVal, PWM_INTERRUPT_STS) & (1 << i));

            tmpVal &= (~(1 << (i + PWM_INT_CLEAR_POS)));
            BL_WR_REG(PWMx, PWM_INT_CONFIG, tmpVal);

            if (PWMIntCbfArra[i][PWM_INT_PULSE_CNT] != NULL) {
                /* Call the callback function */
                PWMIntCbfArra[i][PWM_INT_PULSE_CNT]();
            }
        }
    }

    return SUCCESS;
}
#endif

/*@} end of group PWM_Private_Functions */

/** @defgroup  PWM_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  PWM channel init
 *
 * @param  chCfg: PWM configuration
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type PWM_Channel_Init(PWM_CH_CFG_Type *chCfg)
{
    uint32_t tmpVal;
    uint32_t timeoutCnt = PWM_STOP_TIMEOUT_COUNT;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(chCfg->ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(chCfg->ch));
    CHECK_PARAM(IS_PWM_CLK_TYPE(chCfg->clk));
    CHECK_PARAM(IS_PWM_POLARITY_TYPE(chCfg->pol));
    CHECK_PARAM(IS_PWM_STOP_MODE_TYPE(chCfg->stopMode));

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_PWM);

    /* Config pwm clock and polarity */
    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmpVal, PWM_STOP_EN));

    while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    }

    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL, chCfg->clk);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_OUT_INV, chCfg->pol);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_STOP_MODE, chCfg->stopMode);
    BL_WR_REG(PWMx, PWM_CONFIG, tmpVal);

    /* Config pwm division */
    BL_WR_REG(PWMx, PWM_CLKDIV, chCfg->clkDiv);

    /* Config pwm period and duty */
    BL_WR_REG(PWMx, PWM_THRE1, chCfg->threshold1);
    BL_WR_REG(PWMx, PWM_THRE2, chCfg->threshold2);
    BL_WR_REG(PWMx, PWM_PERIOD, chCfg->period);

    /* Config interrupt pulse count */
    tmpVal = BL_RD_REG(PWMx, PWM_INTERRUPT);
    BL_WR_REG(PWMx, PWM_INTERRUPT, BL_SET_REG_BITS_VAL(tmpVal, PWM_INT_PERIOD_CNT, chCfg->intPulseCnt));
    PWM_IntMask(chCfg->ch, PWM_INT_PULSE_CNT, chCfg->intPulseCnt != 0 ? UNMASK : MASK);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(PWM_IRQn, PWM_IRQHandler);
#endif

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PWM channel update source memory address and len
 *
 * @param  ch: PWM channel
 * @param  period: period
 * @param  threshold1: threshold1
 * @param  threshold2: threshold2
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Update(PWM_CH_ID_Type ch, uint16_t period, uint16_t threshold1, uint16_t threshold2)
{
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    /* Config pwm period and duty */
    BL_WR_REG(PWMx, PWM_THRE1, threshold1);
    BL_WR_REG(PWMx, PWM_THRE2, threshold2);
    BL_WR_REG(PWMx, PWM_PERIOD, period);
}

/****************************************************************************/ /**
 * @brief  PWM channel update clock divider
 *
 * @param  ch: PWM channel
 * @param  div: Clock divider
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Set_Div(PWM_CH_ID_Type ch, uint16_t div)
{
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    BL_WR_REG(PWMx, PWM_CLKDIV, div);
}

/****************************************************************************/ /**
 * @brief  PWM channel update threshold1
 *
 * @param  ch: PWM channel
 * @param  threshold1: threshold1
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Set_Threshold1(PWM_CH_ID_Type ch, uint16_t threshold1)
{
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    /* Config pwm period and duty */
    BL_WR_REG(PWMx, PWM_THRE1, threshold1);
}

/****************************************************************************/ /**
 * @brief  PWM channel update threshold2
 *
 * @param  ch: PWM channel
 * @param  threshold2: threshold2
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Set_Threshold2(PWM_CH_ID_Type ch, uint16_t threshold2)
{
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    /* Config pwm period and duty */
    BL_WR_REG(PWMx, PWM_THRE2, threshold2);
}

/****************************************************************************/ /**
 * @brief  PWM channel update period
 *
 * @param  ch: PWM channel
 * @param  period: period
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Set_Period(PWM_CH_ID_Type ch, uint16_t period)
{
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    /* Config pwm period and duty */
    BL_WR_REG(PWMx, PWM_PERIOD, period);
}

/****************************************************************************/ /**
 * @brief  PWM get configuration
 *
 * @param  ch: PWM channel
 * @param  period: period pointer
 * @param  threshold1: threshold1 pointer
 * @param  threshold2: threshold2 pointer
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Get(PWM_CH_ID_Type ch, uint16_t *period, uint16_t *threshold1, uint16_t *threshold2)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    /* get pwm period and duty */
    tmpVal = BL_RD_REG(PWMx, PWM_THRE1);
    *threshold1 = BL_GET_REG_BITS_VAL(tmpVal, PWM_THRE1);
    tmpVal = BL_RD_REG(PWMx, PWM_THRE2);
    *threshold2 = BL_GET_REG_BITS_VAL(tmpVal, PWM_THRE2);
    tmpVal = BL_RD_REG(PWMx, PWM_PERIOD);
    *period = BL_GET_REG_BITS_VAL(tmpVal, PWM_PERIOD);
}

/****************************************************************************/ /**
 * @brief  PWM enable
 *
 * @param  ch: PWM channel number
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Enable(PWM_CH_ID_Type ch)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    /* Config pwm clock to enable pwm */
    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    BL_WR_REG(PWMx, PWM_CONFIG, BL_CLR_REG_BIT(tmpVal, PWM_STOP_EN));
}

/****************************************************************************/ /**
 * @brief  PWM disable
 *
 * @param  ch: PWM channel number
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Disable(PWM_CH_ID_Type ch)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    /* Config pwm clock to disable pwm */
    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmpVal, PWM_STOP_EN));
    PWM_IntMask(ch, PWM_INT_PULSE_CNT, MASK);
}

/****************************************************************************/ /**
 * @brief  PWM channel software mode enable or disable
 *
 * @param  ch: PWM channel number
 * @param  enable: Enable or disable
 *
 * @return None
 *
*******************************************************************************/
void PWM_SW_Mode(PWM_CH_ID_Type ch, BL_Fun_Type enable)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, PWM_SW_MODE, enable));
}

/****************************************************************************/ /**
 * @brief  PWM channel force output high or low
 *
 * @param  ch: PWM channel number
 * @param  value: Output value
 *
 * @return None
 *
*******************************************************************************/
void PWM_SW_Force_Value(PWM_CH_ID_Type ch, uint8_t value)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, PWM_SW_FORCE_VAL, value));
}

/****************************************************************************/ /**
 * @brief  PWM channel force output high
 *
 * @param  ch: PWM channel number
 *
 * @return None
 *
*******************************************************************************/
void PWM_Channel_Fource_Output(PWM_CH_ID_Type ch)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));

    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmpVal, PWM_SW_MODE));
}

/****************************************************************************/ /**
 * @brief  Mask/Unmask the PWM interrupt
 *
 * @param  ch: PWM channel number
 * @param  intType: Specifies the interrupt type
 * @param  intMask: Enable/Disable Specified interrupt type
 *
 * @return None
 *
*******************************************************************************/
void PWM_IntMask(PWM_CH_ID_Type ch, PWM_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_PWM_CH_ID_TYPE(ch));
    CHECK_PARAM(IS_PWM_INT_TYPE(intType));

    tmpVal = BL_RD_REG(PWMx, PWM_INTERRUPT);

    switch (intType) {
        case PWM_INT_PULSE_CNT:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(PWMx, PWM_INTERRUPT, BL_SET_REG_BIT(tmpVal, PWM_INT_ENABLE));
            } else {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(PWMx, PWM_INTERRUPT, BL_CLR_REG_BIT(tmpVal, PWM_INT_ENABLE));
            }

            break;

        case PWM_INT_ALL:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                BL_WR_REG(PWMx, PWM_INTERRUPT, BL_SET_REG_BIT(tmpVal, PWM_INT_ENABLE));
            } else {
                /* MASK(Disable) this interrupt */
                BL_WR_REG(PWMx, PWM_INTERRUPT, BL_CLR_REG_BIT(tmpVal, PWM_INT_ENABLE));
            }

            break;

        default:
            break;
    }
}

/****************************************************************************/ /**
 * @brief  Install PWM interrupt callback function
 *
 * @param  ch: PWM channel number
 * @param  intType: PWM interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return None
 *
*******************************************************************************/
void PWM_Int_Callback_Install(PWM_CH_ID_Type ch, uint32_t intType, intCallback_Type *cbFun)
{
    PWMIntCbfArra[ch][intType] = cbFun;
}

/****************************************************************************/ /**
 * @brief  PWM smart configure according to frequency and duty cycle function
 *
 * @param  ch: PWM channel number
 * @param  frequency: PWM frequency
 * @param  dutyCycle: PWM duty cycle
 *
 * @return SUCCESS or TIMEOUT
 *
*******************************************************************************/
BL_Err_Type PWM_Smart_Configure(PWM_CH_ID_Type ch, uint32_t frequency, uint8_t dutyCycle)
{
    uint32_t tmpVal;
    uint16_t clkDiv, period, threshold2;
    uint32_t timeoutCnt = PWM_STOP_TIMEOUT_COUNT;
    /* Get channel register */
    uint32_t PWMx = PWM_Get_Channel_Reg(ch);

    if (frequency <= 32) {
        clkDiv = 500;
        period = 64000 / frequency;
        threshold2 = 640 * dutyCycle / frequency;
    } else if (frequency <= 62) {
        clkDiv = 16;
        period = 2000000 / frequency;
        threshold2 = 20000 * dutyCycle / frequency;
    } else if (frequency <= 124) {
        clkDiv = 8;
        period = 4000000 / frequency;
        threshold2 = 40000 * dutyCycle / frequency;
    } else if (frequency <= 246) {
        clkDiv = 4;
        period = 8000000 / frequency;
        threshold2 = 80000 * dutyCycle / frequency;
    } else if (frequency <= 490) {
        clkDiv = 2;
        period = 16000000 / frequency;
        threshold2 = 160000 * dutyCycle / frequency;
    } else {
        clkDiv = 1;
        period = 32000000 / frequency;
        threshold2 = 320000 * dutyCycle / frequency;
    }

    tmpVal = BL_RD_REG(PWMx, PWM_CONFIG);
    if (BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL) != PWM_CLK_XCLK) {
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmpVal, PWM_STOP_EN));
        while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
            timeoutCnt--;
            if (timeoutCnt == 0) {
                return TIMEOUT;
            }
        }
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL, PWM_CLK_XCLK);
    }
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_OUT_INV, PWM_POL_NORMAL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PWM_STOP_MODE, PWM_STOP_GRACEFUL);
    BL_WR_REG(PWMx, PWM_CONFIG, tmpVal);

    /* Config pwm division */
    BL_WR_REG(PWMx, PWM_CLKDIV, clkDiv);

    /* Config pwm period and duty */
    BL_WR_REG(PWMx, PWM_PERIOD, period);
    BL_WR_REG(PWMx, PWM_THRE1, 0);
    BL_WR_REG(PWMx, PWM_THRE2, threshold2);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  PWM interrupt function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void PWM_IRQHandler(void)
{
    PWM_IntHandler(PWM_IRQn);
}
#endif

/*@} end of group PWM_Public_Functions */

/*@} end of group PWM */

/*@} end of group BL702_Peripheral_Driver */
