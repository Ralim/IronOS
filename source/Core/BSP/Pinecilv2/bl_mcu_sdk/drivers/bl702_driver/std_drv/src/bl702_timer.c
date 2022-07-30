/**
  ******************************************************************************
  * @file    bl702_timer.c
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

#include "bl702_timer.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  TIMER
 *  @{
 */

/** @defgroup  TIMER_Private_Macros
 *  @{
 */
#define TIMER_MAX_MATCH 3

/*@} end of group TIMER_Private_Macros */

/** @defgroup  TIMER_Private_Types
 *  @{
 */

/*@} end of group TIMER_Private_Types */

/** @defgroup  TIMER_Private_Variables
 *  @{
 */
intCallback_Type *timerIntCbfArra[3][TIMER_INT_ALL] = {
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL }
};

/*@} end of group TIMER_Private_Variables */

/** @defgroup  TIMER_Global_Variables
 *  @{
 */

/*@} end of group TIMER_Global_Variables */

/** @defgroup  TIMER_Private_Fun_Declaration
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
static void TIMER_IntHandler(IRQn_Type irqNo, TIMER_Chan_Type timerCh);
#endif

/*@} end of group TIMER_Private_Fun_Declaration */

/** @defgroup  TIMER_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  TIMER interrupt common handler function
 *
 * @param  irqNo: Interrupt ID type
 * @param  timerCh: TIMER channel type
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void TIMER_IntHandler(IRQn_Type irqNo, TIMER_Chan_Type timerCh)
{
    uint32_t intId;
    uint32_t tmpVal;
    uint32_t tmpAddr;

    intId = BL_RD_WORD(TIMER_BASE + TIMER_TMSR2_OFFSET + 4 * timerCh);
    tmpAddr = TIMER_BASE + TIMER_TICR2_OFFSET + 4 * timerCh;
    tmpVal = BL_RD_WORD(tmpAddr);

    /* Comparator 0 match interrupt */
    if (BL_IS_REG_BIT_SET(intId, TIMER_TMSR_0)) {
        BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TCLR_0));

        if (timerIntCbfArra[irqNo - TIMER_CH0_IRQn][TIMER_INT_COMP_0] != NULL) {
            /* Call the callback function */
            timerIntCbfArra[irqNo - TIMER_CH0_IRQn][TIMER_INT_COMP_0]();
        }
    }

    /* Comparator 1 match interrupt */
    if (BL_IS_REG_BIT_SET(intId, TIMER_TMSR_1)) {
        BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TCLR_1));

        if (timerIntCbfArra[irqNo - TIMER_CH0_IRQn][TIMER_INT_COMP_1] != NULL) {
            /* Call the callback function */
            timerIntCbfArra[irqNo - TIMER_CH0_IRQn][TIMER_INT_COMP_1]();
        }
    }

    /* Comparator 2 match interrupt */
    if (BL_IS_REG_BIT_SET(intId, TIMER_TMSR_2)) {
        BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TCLR_2));

        if (timerIntCbfArra[irqNo - TIMER_CH0_IRQn][TIMER_INT_COMP_2] != NULL) {
            /* Call the callback function */
            timerIntCbfArra[irqNo - TIMER_CH0_IRQn][TIMER_INT_COMP_2]();
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Get the specified channel and match comparator value
 *
 * @param  timerCh: TIMER channel type
 * @param  cmpNo: TIMER comparator ID type
 *
 * @return Match comapre register value
 *
*******************************************************************************/
uint32_t TIMER_GetCompValue(TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));
    CHECK_PARAM(IS_TIMER_COMP_ID_TYPE(cmpNo));

    tmpVal = BL_RD_WORD(TIMER_BASE + TIMER_TMR2_0_OFFSET + 4 * (TIMER_MAX_MATCH * timerCh + cmpNo));
    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  TIMER set specified channel and comparator compare value
 *
 * @param  timerCh: TIMER channel type
 * @param  cmpNo: TIMER comparator ID type
 * @param  val: TIMER match comapre register value
 *
 * @return None
 *
*******************************************************************************/
void TIMER_SetCompValue(TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo, uint32_t val)
{
    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));
    CHECK_PARAM(IS_TIMER_COMP_ID_TYPE(cmpNo));

    BL_WR_WORD(TIMER_BASE + TIMER_TMR2_0_OFFSET + 4 * (TIMER_MAX_MATCH * timerCh + cmpNo), val);
}

/****************************************************************************/ /**
 * @brief  TIMER get the specified channel count value
 *
 * @param  timerCh: TIMER channel type
 *
 * @return TIMER count register value
 *
*******************************************************************************/
uint32_t TIMER_GetCounterValue(TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;
    uint32_t tmpAddr;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));

    /* TO avoid risk of reading, don't read TCVWR directly*/
    /* request for read*/
    tmpAddr = TIMER_BASE + TIMER_TCVWR2_OFFSET + 4 * timerCh;
    BL_WR_WORD(tmpAddr, 1);

    /* Need wait */
    tmpVal = BL_RD_WORD(tmpAddr);
    tmpVal = BL_RD_WORD(tmpAddr);
    tmpVal = BL_RD_WORD(tmpAddr);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  TIMER get specified channel and comparator match status
 *
 * @param  timerCh: TIMER channel type
 * @param  cmpNo: TIMER comparator ID type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type TIMER_GetMatchStatus(TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpVal;
    BL_Sts_Type bitStatus = RESET;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));
    CHECK_PARAM(IS_TIMER_COMP_ID_TYPE(cmpNo));

    tmpVal = BL_RD_WORD(TIMER_BASE + TIMER_TMSR2_OFFSET + 4 * timerCh);

    switch (cmpNo) {
        case TIMER_COMP_ID_0:
            bitStatus = BL_IS_REG_BIT_SET(tmpVal, TIMER_TMSR_0) ? SET : RESET;
            break;

        case TIMER_COMP_ID_1:
            bitStatus = BL_IS_REG_BIT_SET(tmpVal, TIMER_TMSR_1) ? SET : RESET;
            break;

        case TIMER_COMP_ID_2:
            bitStatus = BL_IS_REG_BIT_SET(tmpVal, TIMER_TMSR_2) ? SET : RESET;
            break;

        default:
            break;
    }

    return bitStatus;
}

/****************************************************************************/ /**
 * @brief  TIMER get specified channel preload value
 *
 * @param  timerCh: TIMER channel type
 *
 * @return Preload register value
 *
*******************************************************************************/
uint32_t TIMER_GetPreloadValue(TIMER_Chan_Type timerCh)
{
    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));

    return BL_RD_WORD(TIMER_BASE + TIMER_TPLVR2_OFFSET + 4 * timerCh);
}

/****************************************************************************/ /**
 * @brief  TIMER set preload register low 32bits value
 *
 * @param  timerCh: TIMER channel type
 * @param  val: Preload register low 32bits value
 *
 * @return None
 *
*******************************************************************************/
void TIMER_SetPreloadValue(TIMER_Chan_Type timerCh, uint32_t val)
{
    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));

    BL_WR_WORD(TIMER_BASE + TIMER_TPLVR2_OFFSET + 4 * timerCh, val);
}

/****************************************************************************/ /**
 * @brief  TIMER set preload trigger source,COMP0,COMP1,COMP2 or None
 *
 * @param  timerCh: TIMER channel type
 * @param  plSrc: TIMER preload source type
 *
 * @return None
 *
*******************************************************************************/
void TIMER_SetPreloadTrigSrc(TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc)
{
    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));
    CHECK_PARAM(IS_TIMER_PRELOAD_TRIG_TYPE(plSrc));

    BL_WR_WORD(TIMER_BASE + TIMER_TPLCR2_OFFSET + 4 * timerCh, plSrc);
}

/****************************************************************************/ /**
 * @brief  TIMER set count mode:preload or free run
 *
 * @param  timerCh: TIMER channel type
 * @param  countMode: TIMER count mode: TIMER_COUNT_PRELOAD or TIMER_COUNT_FREERUN
 *
 * @return None
 *
*******************************************************************************/
void TIMER_SetCountMode(TIMER_Chan_Type timerCh, TIMER_CountMode_Type countMode)
{
    uint32_t tmpval;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));
    CHECK_PARAM(IS_TIMER_COUNTMODE_TYPE(countMode));

    tmpval = BL_RD_WORD(TIMER_BASE + TIMER_TCMR_OFFSET);
    tmpval &= (~(1 << (timerCh + 1)));
    tmpval |= (countMode << (timerCh + 1));

    BL_WR_WORD(TIMER_BASE + TIMER_TCMR_OFFSET, tmpval);
}

/****************************************************************************/ /**
 * @brief  TIMER clear interrupt status
 *
 * @param  timerCh: TIMER channel type
 * @param  cmpNo: TIMER macth comparator ID type
 *
 * @return None
 *
*******************************************************************************/
void TIMER_ClearIntStatus(TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpAddr;
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));
    CHECK_PARAM(IS_TIMER_COMP_ID_TYPE(cmpNo));

    tmpAddr = TIMER_BASE + TIMER_TICR2_OFFSET + 4 * timerCh;

    tmpVal = BL_RD_WORD(tmpAddr);
    tmpVal |= (1 << cmpNo);

    BL_WR_WORD(tmpAddr, tmpVal);
}

/****************************************************************************/ /**
 * @brief  TIMER initialization function
 *
 * @param  timerCfg: TIMER configuration structure pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type TIMER_Init(TIMER_CFG_Type *timerCfg)
{
    TIMER_Chan_Type timerCh = timerCfg->timerCh;
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CLKSRC_TYPE(timerCfg->clkSrc));
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCfg->timerCh));
    CHECK_PARAM(IS_TIMER_PRELOAD_TRIG_TYPE(timerCfg->plTrigSrc));
    CHECK_PARAM(IS_TIMER_COUNTMODE_TYPE(timerCfg->countMode));

    /* Configure timer clock source */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);

    if (timerCh == TIMER_CH0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_CS_1, timerCfg->clkSrc);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_CS_2, timerCfg->clkSrc);
    }

    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpVal);

    /* Configure timer clock division */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);

    if (timerCh == TIMER_CH0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR2, timerCfg->clockDivision);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_TCDR3, timerCfg->clockDivision);
    }

    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);

    /* Configure timer count mode: preload or free run */
    TIMER_SetCountMode(timerCh, timerCfg->countMode);

    /* Configure timer preload trigger src */
    TIMER_SetPreloadTrigSrc(timerCh, timerCfg->plTrigSrc);

    if (timerCfg->countMode == TIMER_COUNT_PRELOAD) {
        /* Configure timer preload value */
        TIMER_SetPreloadValue(timerCh, timerCfg->preLoadVal);

        /* Configure match compare values */
        if (timerCfg->matchVal0 > 1 + timerCfg->preLoadVal) {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0 - 2);
        } else {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0);
        }

        if (timerCfg->matchVal1 > 1 + timerCfg->preLoadVal) {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1 - 2);
        } else {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1);
        }

        if (timerCfg->matchVal2 > 1 + timerCfg->preLoadVal) {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2 - 2);
        } else {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2);
        }
    } else {
        /* Configure match compare values */
        if (timerCfg->matchVal0 > 1) {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0 - 2);
        } else {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0);
        }

        if (timerCfg->matchVal1 > 1) {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1 - 2);
        } else {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1);
        }

        if (timerCfg->matchVal2 > 1) {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2 - 2);
        } else {
            TIMER_SetCompValue(timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2);
        }
    }

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(TIMER_CH0_IRQn, TIMER_CH0_IRQHandler);
    Interrupt_Handler_Register(TIMER_CH1_IRQn, TIMER_CH1_IRQHandler);
#endif

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  TIMER enable one channel function
 *
 * @param  timerCh: TIMER channel type
 *
 * @return None
 *
*******************************************************************************/
void TIMER_Enable(TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCER);
    tmpVal |= (1 << (timerCh + 1));

    BL_WR_REG(TIMER_BASE, TIMER_TCER, tmpVal);
}

/****************************************************************************/ /**
 * @brief  TIMER disable one channel function
 *
 * @param  timerCh: TIMER channel type
 *
 * @return None
 *
*******************************************************************************/
void TIMER_Disable(TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCER);
    tmpVal &= (~(1 << (timerCh + 1)));

    BL_WR_REG(TIMER_BASE, TIMER_TCER, tmpVal);
}

/****************************************************************************/ /**
 * @brief  TIMER mask or unmask certain or all interrupt
 *
 * @param  timerCh: TIMER channel type
 * @param  intType: TIMER interrupt type
 * @param  intMask: TIMER interrupt mask value:MASK:disbale interrupt.UNMASK:enable interrupt
 *
 * @return None
 *
*******************************************************************************/
void TIMER_IntMask(TIMER_Chan_Type timerCh, TIMER_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpAddr;
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerCh));
    CHECK_PARAM(IS_TIMER_INT_TYPE(intType));
    CHECK_PARAM(IS_BL_MASK_TYPE(intMask));

    tmpAddr = TIMER_BASE + TIMER_TIER2_OFFSET + 4 * timerCh;
    tmpVal = BL_RD_WORD(tmpAddr);

    switch (intType) {
        case TIMER_INT_COMP_0:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TIER_0));
            } else {
                /* Disable this interrupt */
                BL_WR_WORD(tmpAddr, BL_CLR_REG_BIT(tmpVal, TIMER_TIER_0));
            }

            break;

        case TIMER_INT_COMP_1:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TIER_1));
            } else {
                /* Disable this interrupt */
                BL_WR_WORD(tmpAddr, BL_CLR_REG_BIT(tmpVal, TIMER_TIER_1));
            }

            break;

        case TIMER_INT_COMP_2:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TIER_2));
            } else {
                /* Disable this interrupt */
                BL_WR_WORD(tmpAddr, BL_CLR_REG_BIT(tmpVal, TIMER_TIER_2));
            }

            break;

        case TIMER_INT_ALL:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TIER_0));
                BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TIER_1));
                BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TIER_2));
            } else {
                /* Disable this interrupt */
                BL_WR_WORD(tmpAddr, BL_CLR_REG_BIT(tmpVal, TIMER_TIER_0));
                BL_WR_WORD(tmpAddr, BL_CLR_REG_BIT(tmpVal, TIMER_TIER_1));
                BL_WR_WORD(tmpAddr, BL_CLR_REG_BIT(tmpVal, TIMER_TIER_2));
            }

            break;

        default:
            break;
    }
}

/****************************************************************************/ /**
 * @brief  TIMER set watchdog clock source and clock division
 *
 * @param  clkSrc: Watchdog timer clock source type
 * @param  div: Watchdog timer clock division value
 *
 * @return None
 *
*******************************************************************************/
void WDT_Set_Clock(TIMER_ClkSrc_Type clkSrc, uint8_t div)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CLKSRC_TYPE(clkSrc));

    /* Configure watchdog timer clock source */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_CS_WDT, clkSrc);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpVal);

    /* Configure watchdog timer clock divison */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TIMER_WCDR, div);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpVal);
}

/****************************************************************************/ /**
 * @brief  TIMER get watchdog match compare value
 *
 * @param  None
 *
 * @return Watchdog match comapre register value
 *
*******************************************************************************/
uint16_t WDT_GetMatchValue(void)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    /* Get watchdog timer match register value */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WMR);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  TIMER set watchdog match compare value
 *
 * @param  val: Watchdog match compare value
 *
 * @return None
 *
*******************************************************************************/
void WDT_SetCompValue(uint16_t val)
{
    WDT_ENABLE_ACCESS();

    /* Set watchdog timer match register value */
    BL_WR_REG(TIMER_BASE, TIMER_WMR, val);
}

/****************************************************************************/ /**
 * @brief  TIMER get watchdog count register value
 *
 * @param  None
 *
 * @return Watchdog count register value
 *
*******************************************************************************/
uint16_t WDT_GetCounterValue(void)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    /* Get watchdog timer count register value */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WVR);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  TIMER reset watchdog count register value
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void WDT_ResetCounterValue(void)
{
    uint32_t tmpVal;

    /* Reset watchdog timer count register value */
    WDT_ENABLE_ACCESS();

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WCR);

    /* Set watchdog counter reset register bit0 to 1 */
    BL_WR_REG(TIMER_BASE, TIMER_WCR, BL_SET_REG_BIT(tmpVal, TIMER_WCR));
}

/****************************************************************************/ /**
 * @brief  TIMER get watchdog reset status
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type WDT_GetResetStatus(void)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    /* Get watchdog status register */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WSR);

    return (BL_IS_REG_BIT_SET(tmpVal, TIMER_WTS)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  TIMER clear watchdog reset status
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void WDT_ClearResetStatus(void)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WSR);

    /* Set watchdog status register */
    BL_WR_REG(TIMER_BASE, TIMER_WSR, BL_CLR_REG_BIT(tmpVal, TIMER_WTS));
}

/****************************************************************************/ /**
 * @brief  TIMER enable watchdog function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void WDT_Enable(void)
{
    uint32_t tmpVal;

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(TIMER_WDT_IRQn, TIMER_WDT_IRQHandler);
#endif

    WDT_ENABLE_ACCESS();

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WMER);

    BL_WR_REG(TIMER_BASE, TIMER_WMER, BL_SET_REG_BIT(tmpVal, TIMER_WE));
}

/****************************************************************************/ /**
 * @brief  Watchdog timer disable function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void WDT_Disable(void)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WMER);

    BL_WR_REG(TIMER_BASE, TIMER_WMER, BL_CLR_REG_BIT(tmpVal, TIMER_WE));
}

/****************************************************************************/ /**
 * @brief  Watchdog timer mask or unmask certain or all interrupt
 *
 * @param  intType: Watchdog interrupt type
 * @param  intMask: Watchdog interrupt mask value:MASK:disbale interrupt.UNMASK:enable interrupt
 *
 * @return None
 *
*******************************************************************************/
void WDT_IntMask(WDT_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_WDT_INT_TYPE(intType));
    CHECK_PARAM(IS_BL_MASK_TYPE(intMask));

    WDT_ENABLE_ACCESS();

    /* Deal with watchdog match/interrupt enable register,
      WRIE:watchdog reset/interrupt enable */
    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WMER);

    switch (intType) {
        case WDT_INT:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                /* 0 means generates a watchdog interrupt,
                   a watchdog timer reset is not generated*/
                BL_WR_REG(TIMER_BASE, TIMER_WMER, BL_CLR_REG_BIT(tmpVal, TIMER_WRIE));
            } else {
                /* Disable this interrupt */
                /* 1 means generates a watchdog timer reset,
                   a watchdog  interrupt is not generated*/
                BL_WR_REG(TIMER_BASE, TIMER_WMER, BL_SET_REG_BIT(tmpVal, TIMER_WRIE));
            }

            break;

        default:
            break;
    }
}

/****************************************************************************/ /**
 * @brief  TIMER channel 0 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void TIMER_CH0_IRQHandler(void)
{
    TIMER_IntHandler(TIMER_CH0_IRQn, TIMER_CH0);
}
#endif

/****************************************************************************/ /**
 * @brief  TIMER channel 1 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void TIMER_CH1_IRQHandler(void)
{
    TIMER_IntHandler(TIMER_CH1_IRQn, TIMER_CH1);
}
#endif

/****************************************************************************/ /**
 * @brief  TIMER watchdog interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void TIMER_WDT_IRQHandler(void)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = BL_RD_REG(TIMER_BASE, TIMER_WICR);
    BL_WR_REG(TIMER_BASE, TIMER_WICR, BL_SET_REG_BIT(tmpVal, TIMER_WICLR));

    if (timerIntCbfArra[TIMER_WDT_IRQn - TIMER_CH0_IRQn][WDT_INT] != NULL) {
        /* Call the callback function */
        timerIntCbfArra[TIMER_WDT_IRQn - TIMER_CH0_IRQn][WDT_INT]();
    }
}
#endif

/****************************************************************************/ /**
 * @brief  TIMER install interrupt callback
 *
 * @param  timerChan: TIMER channel type
 * @param  intType: TIMER interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return None
 *
*******************************************************************************/
void Timer_Int_Callback_Install(TIMER_Chan_Type timerChan, TIMER_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_TIMER_CHAN_TYPE(timerChan));
    CHECK_PARAM(IS_TIMER_INT_TYPE(intType));

    timerIntCbfArra[timerChan][intType] = cbFun;
}

/****************************************************************************/ /**
 * @brief  Watchdog install interrupt callback
 *
 * @param  wdtInt: Watchdog interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return None
 *
*******************************************************************************/
void WDT_Int_Callback_Install(WDT_INT_Type wdtInt, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_WDT_INT_TYPE(wdtInt));

    timerIntCbfArra[2][wdtInt] = cbFun;
}

/*@} end of group TIMER_Private_Functions */

/*@} end of group TIMER */

/*@} end of group BL702_Peripheral_Driver */
