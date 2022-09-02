/**
  ******************************************************************************
  * @file    bl702_kys.c
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

#include "bl702_kys.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  KYS
 *  @{
 */

/** @defgroup  KYS_Private_Macros
 *  @{
 */

/*@} end of group KYS_Private_Macros */

/** @defgroup  KYS_Private_Types
 *  @{
 */

/*@} end of group KYS_Private_Types */

/** @defgroup  KYS_Private_Variables
 *  @{
 */
static intCallback_Type *KYSIntCbfArra[1] = { NULL };

/*@} end of group KYS_Private_Variables */

/** @defgroup  KYS_Global_Variables
 *  @{
 */

/*@} end of group KYS_Global_Variables */

/** @defgroup  KYS_Private_Fun_Declaration
 *  @{
 */

/*@} end of group KYS_Private_Fun_Declaration */

/** @defgroup  KYS_Private_Functions
 *  @{
 */

/*@} end of group KYS_Private_Functions */

/** @defgroup  KYS_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  KYS interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void KYS_IRQHandler(void)
{
    if (KYSIntCbfArra[0] != NULL) {
        KYSIntCbfArra[0]();
    }
}
#endif

/****************************************************************************/ /**
 * @brief  KYS initialization function
 *
 * @param  kysCfg: KYS configuration structure pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Init(KYS_CFG_Type *kysCfg)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    /* Set col and row */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_COL_NUM, kysCfg->col - 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_ROW_NUM, kysCfg->row - 1);

    /* Set idle duration between column scans */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_RC_EXT, kysCfg->idleDuration);

    /* Enable or disable ghost key event detection */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_GHOST_EN, kysCfg->ghostEn);

    /* Enable or disable deglitch function */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_EN, kysCfg->deglitchEn);

    /* Set deglitch count */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_CNT, kysCfg->deglitchCnt);

    /* Write back */
    BL_WR_REG(KYS_BASE, KYS_KS_CTRL, tmpVal);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(KYS_IRQn, KYS_IRQHandler);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable KYS
 *
 * @param  None
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    BL_WR_REG(KYS_BASE, KYS_KS_CTRL, BL_SET_REG_BIT(tmpVal, KYS_KS_EN));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable KYS
 *
 * @param  None
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    BL_WR_REG(KYS_BASE, KYS_KS_CTRL, BL_CLR_REG_BIT(tmpVal, KYS_KS_EN));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  KYS mask or unmask interrupt
 *
 * @param  intMask: KYS interrupt mask value( MASK:disbale interrupt,UNMASK:enable interrupt )
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_IntMask(BL_Mask_Type intMask)
{
    if (MASK == intMask) {
        BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, 0);
    } else {
        BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, 1);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  KYS clear interrupt
 *
 * @param  None
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_IntClear(void)
{
    BL_WR_REG(KYS_BASE, KYS_KEYCODE_CLR, 0xf);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Install KYS interrupt callback function
 *
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type KYS_Int_Callback_Install(intCallback_Type *cbFun)
{
    KYSIntCbfArra[0] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  KYS get interrupt status
 *
 * @param  None
 *
 * @return Status of interrupt
 *
*******************************************************************************/
uint8_t KYS_GetIntStatus(void)
{
    return BL_RD_REG(KYS_BASE, KYS_KS_INT_STS) & 0xf;
}

/****************************************************************************/ /**
 * @brief  KYS get keycode value
 *
 * @param  keycode: KYS keycode type
 * @param  col: Col of key
 * @param  row: Row of key
 *
 * @return Keycode value
 *
*******************************************************************************/
uint8_t KYS_GetKeycode(KYS_Keycode_Type keycode, uint8_t *col, uint8_t *row)
{
    uint32_t tmpVal;
    uint8_t keyValue;

    /* Get keycode value */
    keyValue = BL_RD_REG(KYS_BASE, KYS_KEYCODE_VALUE) >> (8 * keycode) & 0xff;

    /* Get total row number of keyboard */
    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, KYS_ROW_NUM);

    /* Calculate col and row of the key */
    *col = keyValue / (tmpVal + 1);
    *row = keyValue % (tmpVal + 1);

    return keyValue;
}

/*@} end of group KYS_Public_Functions */

/*@} end of group KYS */

/*@} end of group BL702_Peripheral_Driver */
