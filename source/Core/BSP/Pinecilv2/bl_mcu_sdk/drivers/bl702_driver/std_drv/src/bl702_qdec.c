/**
  ******************************************************************************
  * @file    bl702_qdec.c
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

#include "bl702_qdec.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  QDEC
 *  @{
 */

/** @defgroup  QDEC_Private_Macros
 *  @{
 */

/*@} end of group QDEC_Private_Macros */

/** @defgroup  QDEC_Private_Types
 *  @{
 */

/*@} end of group QDEC_Private_Types */

/** @defgroup  QDEC_Private_Variables
 *  @{
 */
static const uint32_t qdecAddr[QDEC_ID_MAX] = { QDEC0_BASE, QDEC1_BASE, QDEC2_BASE };
static intCallback_Type *qdecIntCbfArra[QDEC_ID_MAX][QDEC_INT_ALL] = { { NULL, NULL, NULL, NULL },
                                                                       { NULL, NULL, NULL, NULL },
                                                                       { NULL, NULL, NULL, NULL } };

/*@} end of group QDEC_Private_Variables */

/** @defgroup  QDEC_Global_Variables
 *  @{
 */

/*@} end of group QDEC_Global_Variables */

/** @defgroup  QDEC_Private_Fun_Declaration
 *  @{
 */

/*@} end of group QDEC_Private_Fun_Declaration */

/** @defgroup  QDEC_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  QDEC init
 *
 * @param  qdecId: QDEC ID
 * @param  qdecCfg: QDEC config
 *
 * @return None
 *
*******************************************************************************/
void QDEC_Init(QDEC_ID_Type qdecId, QDEC_CFG_Type *qdecCfg)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    CHECK_PARAM(IS_QDEC_SAMPLE_MODE_TYPE(qdecCfg->sampleCfg.sampleMod));
    CHECK_PARAM(IS_QDEC_SAMPLE_PERIOD_TYPE(qdecCfg->sampleCfg.samplePeriod));
    CHECK_PARAM(IS_QDEC_REPORT_MODE_TYPE(qdecCfg->reportCfg.reportMod));
    CHECK_PARAM((qdecCfg->ledCfg.ledPeriod) <= 0x1FF);
    CHECK_PARAM((qdecCfg->deglitchCfg.deglitchStrength) <= 0xF);

    /* qdec_ctrl */
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_SPL_PERIOD, qdecCfg->sampleCfg.samplePeriod);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_RPT_PERIOD, qdecCfg->reportCfg.reportPeriod);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_LED_EN, qdecCfg->ledCfg.ledEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_LED_POL, qdecCfg->ledCfg.ledSwap);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_DEG_EN, qdecCfg->deglitchCfg.deglitchEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_DEG_CNT, qdecCfg->deglitchCfg.deglitchStrength);
    BL_WR_REG(QDECx, QDEC0_CTRL0, tmpVal);

    /* qdec_ctrl1 */
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_SPL_MODE, qdecCfg->sampleCfg.sampleMod);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_RPT_MODE, qdecCfg->reportCfg.reportMod);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_LED_PERIOD, qdecCfg->ledCfg.ledPeriod);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_ACC_MODE, qdecCfg->accMod);
    BL_WR_REG(QDECx, QDEC0_CTRL1, tmpVal);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(QDEC0_IRQn, QDEC0_IRQHandler);
    Interrupt_Handler_Register(QDEC1_IRQn, QDEC1_IRQHandler);
    Interrupt_Handler_Register(QDEC2_IRQn, QDEC2_IRQHandler);
#endif
}

/****************************************************************************/ /**
 * @brief  QDEC deinit
 *
 * @param  qdecId: QDEC ID
 *
 * @return None
 *
*******************************************************************************/
void QDEC_DeInit(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    /* deconfig qdec */
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_RPT_PERIOD, 10);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_SPL_PERIOD, 2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_DEG_CNT, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_DEG_EN, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_LED_POL, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_LED_EN, 0);
    BL_WR_REG(QDECx, QDEC0_CTRL0, tmpVal);
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_LED_PERIOD, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_INPUT_SWAP, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_RPT_MODE, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_SPL_MODE, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_ACC_MODE, 1);
    BL_WR_REG(QDECx, QDEC0_CTRL1, tmpVal);

    /* enable qdec */
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_EN, 1);
    BL_WR_REG(QDECx, QDEC0_CTRL0, tmpVal);

    /* deconfig interrupt */
    tmpVal = BL_RD_REG(QDECx, QDEC0_INT_EN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_OVERFLOW_EN, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_DBL_RDY_EN, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_SPL_RDY_EN, 0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_RPT_RDY_EN, 1);
    BL_WR_REG(QDECx, QDEC0_INT_EN, tmpVal);

    /* clear status */
    tmpVal = BL_RD_REG(QDECx, QDEC0_INT_CLR);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_OVERFLOW_CLR, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_DBL_RDY_CLR, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_SPL_RDY_CLR, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_RPT_RDY_CLR, 1);
    BL_WR_REG(QDECx, QDEC0_INT_STS, tmpVal);

    /* clear value */
    tmpVal = BL_RD_REG(QDECx, QDEC0_VALUE);

    /* disable qdec */
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_EN, 0);
    BL_WR_REG(QDECx, QDEC0_CTRL0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  QDEC enable
 *
 * @param  qdecId: QDEC ID
 *
 * @return None
 *
*******************************************************************************/
void QDEC_Enable(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    /* qdec_ctrl */
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL0);
    tmpVal = BL_SET_REG_BIT(tmpVal, QDEC_EN);
    BL_WR_REG(QDECx, QDEC0_CTRL0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  QDEC disable
 *
 * @param  qdecId: QDEC ID
 *
 * @return None
 *
*******************************************************************************/
void QDEC_Disable(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    /* qdec_ctrl */
    tmpVal = BL_RD_REG(QDECx, QDEC0_CTRL0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, QDEC_EN);
    BL_WR_REG(QDECx, QDEC0_CTRL0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  set QDEC interrupt mask
 *
 * @param  qdecId: QDEC ID
 * @param  intType: QDEC interrupt type
 * @param  intMask: MASK or UNMASK
 *
 * @return None
 *
*******************************************************************************/
void QDEC_SetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    CHECK_PARAM(IS_QDEC_INT_TYPE(intType));

    /* qdec_int_en */
    tmpVal = BL_RD_REG(QDECx, QDEC0_INT_EN);

    switch (intType) {
        case QDEC_INT_REPORT:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_RPT_RDY_EN, (intMask ? 0 : 1));
            break;

        case QDEC_INT_SAMPLE:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_SPL_RDY_EN, (intMask ? 0 : 1));
            break;

        case QDEC_INT_ERROR:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_DBL_RDY_EN, (intMask ? 0 : 1));
            break;

        case QDEC_INT_OVERFLOW:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, QDEC_OVERFLOW_EN, (intMask ? 0 : 1));
            break;

        default:
            break;
    }

    BL_WR_REG(QDECx, QDEC0_INT_EN, tmpVal);
}

/****************************************************************************/ /**
 * @brief  get QDEC interrupt mask
 *
 * @param  qdecId: QDEC ID
 * @param  intType: QDEC interrupt type
 *
 * @return MASK or UNMASK
 *
*******************************************************************************/
BL_Mask_Type QDEC_GetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    CHECK_PARAM(IS_QDEC_INT_TYPE(intType));

    /* qdec_int_en */
    tmpVal = BL_RD_REG(QDECx, QDEC0_INT_EN);

    switch (intType) {
        case QDEC_INT_REPORT:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_RPT_RDY_EN) ? UNMASK : MASK;

        case QDEC_INT_SAMPLE:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_SPL_RDY_EN) ? UNMASK : MASK;

        case QDEC_INT_ERROR:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_DBL_RDY_EN) ? UNMASK : MASK;

        case QDEC_INT_OVERFLOW:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_OVERFLOW_EN) ? UNMASK : MASK;

        default:
            return UNMASK;
    }
}

/****************************************************************************/ /**
 * @brief  QDEC interrupt callback install
 *
 * @param  qdecId: QDEC ID
 * @param  intType: QDEC interrupt type
 * @param  cbFun: interrupt callback
 *
 * @return None
 *
*******************************************************************************/
void QDEC_Int_Callback_Install(QDEC_ID_Type qdecId, QDEC_INT_Type intType, intCallback_Type *cbFun)
{
    qdecIntCbfArra[qdecId][intType] = cbFun;
}

/****************************************************************************/ /**
 * @brief  QDEC get interrupt status
 *
 * @param  qdecId: QDEC ID
 * @param  intType: QDEC interrupt type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type QDEC_Get_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    CHECK_PARAM(IS_QDEC_INT_TYPE(intType));

    /* qdec_int_sts */
    tmpVal = BL_RD_REG(QDECx, QDEC0_INT_STS);

    switch (intType) {
        case QDEC_INT_REPORT:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_RPT_RDY_STS) ? SET : RESET;

        case QDEC_INT_SAMPLE:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_SPL_RDY_STS) ? SET : RESET;

        case QDEC_INT_ERROR:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_DBL_RDY_STS) ? SET : RESET;

        case QDEC_INT_OVERFLOW:
            return BL_GET_REG_BITS_VAL(tmpVal, QDEC_OVERFLOW_STS) ? SET : RESET;

        default:
            return RESET;
    }
}

/****************************************************************************/ /**
 * @brief  QDEC clear interrupt status
 *
 * @param  qdecId: QDEC ID
 * @param  intType: QDEC interrupt type
 *
 * @return None
 *
*******************************************************************************/
void QDEC_Clr_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    CHECK_PARAM(IS_QDEC_INT_TYPE(intType));

    /* qdec_int_clr */
    tmpVal = BL_RD_REG(QDECx, QDEC0_INT_CLR);

    switch (intType) {
        case QDEC_INT_REPORT:
            tmpVal = BL_SET_REG_BIT(tmpVal, QDEC_RPT_RDY_CLR);
            break;

        case QDEC_INT_SAMPLE:
            tmpVal = BL_SET_REG_BIT(tmpVal, QDEC_SPL_RDY_CLR);
            break;

        case QDEC_INT_ERROR:
            tmpVal = BL_SET_REG_BIT(tmpVal, QDEC_DBL_RDY_CLR);
            break;

        case QDEC_INT_OVERFLOW:
            tmpVal = BL_SET_REG_BIT(tmpVal, QDEC_OVERFLOW_CLR);
            break;

        default:
            break;
    }

    BL_WR_REG(QDECx, QDEC0_INT_CLR, tmpVal);
}

/****************************************************************************/ /**
 * @brief  QDEC get sample direction
 *
 * @param  qdecId: QDEC ID
 *
 * @return None
 *
*******************************************************************************/
QDEC_DIRECTION_Type QDEC_Get_Sample_Direction(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    /* qdec_value */
    tmpVal = BL_RD_REG(QDECx, QDEC0_VALUE);

    return (QDEC_DIRECTION_Type)BL_GET_REG_BITS_VAL(tmpVal, QDEC_SPL_VAL);
}

/****************************************************************************/ /**
 * @brief  QDEC get error count
 *
 * @param  qdecId: QDEC ID
 *
 * @return None
 *
*******************************************************************************/
uint8_t QDEC_Get_Err_Cnt(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    /* qdec_value */
    tmpVal = BL_RD_REG(QDECx, QDEC0_VALUE);

    return BL_GET_REG_BITS_VAL(tmpVal, QDEC_ACC2_VAL);
}

/****************************************************************************/ /**
 * @brief  QDEC get sample value
 *
 * @param  qdecId: QDEC ID
 *
 * @return None
 *
*******************************************************************************/
uint16_t QDEC_Get_Sample_Val(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    /* qdec_value */
    tmpVal = BL_RD_REG(QDECx, QDEC0_VALUE);

    return BL_GET_REG_BITS_VAL(tmpVal, QDEC_ACC1_VAL);
}

/****************************************************************************/ /**
 * @brief  QDEC interrupt handler
 *
 * @param  qdecId: QDEC ID
 * @param  intType: QDEC interrupt type
 *
 * @return None
 *
*******************************************************************************/
void QDEC_IntHandler(QDEC_ID_Type qdecId, QDEC_INT_Type intType)
{
    if (SET == QDEC_Get_Int_Status(qdecId, intType)) {
        QDEC_Clr_Int_Status(qdecId, intType);

        if (qdecIntCbfArra[qdecId][intType] != NULL) {
            qdecIntCbfArra[qdecId][intType]();
        }
    }
}

/****************************************************************************/ /**
 * @brief  QDEC0 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void QDEC0_IRQHandler(void)
{
    QDEC_INT_Type intType;

    for (intType = QDEC_INT_REPORT; intType < QDEC_INT_ALL; intType++) {
        if (UNMASK == QDEC_GetIntMask(QDEC0_ID, intType)) {
            QDEC_IntHandler(QDEC0_ID, intType);
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  QDEC1 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void QDEC1_IRQHandler(void)
{
    QDEC_INT_Type intType;

    for (intType = QDEC_INT_REPORT; intType < QDEC_INT_ALL; intType++) {
        if (UNMASK == QDEC_GetIntMask(QDEC1_ID, intType)) {
            QDEC_IntHandler(QDEC1_ID, intType);
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  QDEC2 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void QDEC2_IRQHandler(void)
{
    QDEC_INT_Type intType;

    for (intType = QDEC_INT_REPORT; intType < QDEC_INT_ALL; intType++) {
        if (UNMASK == QDEC_GetIntMask(QDEC2_ID, intType)) {
            QDEC_IntHandler(QDEC2_ID, intType);
        }
    }
}
#endif

/*@} end of group QDEC_Private_Functions */

/*@} end of group QDEC */

/*@} end of group BL702_Peripheral_Driver */
