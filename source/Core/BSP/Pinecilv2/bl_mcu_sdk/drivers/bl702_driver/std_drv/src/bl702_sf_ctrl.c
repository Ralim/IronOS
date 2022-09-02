/**
  ******************************************************************************
  * @file    bl702_sf_ctrl.c
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

#include "bl702_sf_ctrl.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SF_CTRL
 *  @{
 */

/** @defgroup  SF_CTRL_Private_Macros
 *  @{
 */

/*@} end of group SF_CTRL_Private_Macros */

/** @defgroup  SF_CTRL_Private_Types
 *  @{
 */

/*@} end of group SF_CTRL_Private_Types */

/** @defgroup  SF_CTRL_Private_Variables
 *  @{
 */
#define SF_CTRL_BUSY_STATE_TIMEOUT      (5 * 160 * 1000)
#define SF_Ctrl_Get_AES_Region(addr, r) (addr + SF_CTRL_AES_REGION_OFFSET + (r)*0x100)

/*@} end of group SF_CTRL_Private_Variables */

/** @defgroup  SF_CTRL_Global_Variables
 *  @{
 */

/*@} end of group SF_CTRL_Global_Variables */

/** @defgroup  SF_CTRL_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SF_CTRL_Private_Fun_Declaration */

/** @defgroup  SF_CTRL_Private_Functions
 *  @{
 */

/*@} end of group SF_CTRL_Private_Functions */

/** @defgroup  SF_CTRL_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Enable serail flash controller
 *
 * @param  cfg: serial flash controller config
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    if (cfg == NULL) {
        return;
    }

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_OWNER_TYPE(cfg->owner));

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_0);

    if (cfg->clkDelay > 0) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_READ_DLY_N, cfg->clkDelay - 1);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
    }

    /* Serail out inverted, so sf ctrl send on negative edge */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_CLK_OUT_INV_SEL, cfg->clkInvert);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_CLK_SF_RX_INV_SEL, cfg->rxClkInvert);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_0, tmpVal);

    /* Set do di and oe delay */
    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_0_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_0_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_0_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_1, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_1_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_1_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_1_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_2, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_2_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_2_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_2_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_3, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_3_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_3_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IO_3_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IO_DLY_4, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_0_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_0_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_0_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_1, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_1_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_1_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_1_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_2, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_2_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_2_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_2_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_3, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_3_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_3_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IO_3_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF2_IF_IO_DLY_4, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_0_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_0_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_0_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_1, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_1_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_1_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_1_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_2, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_3);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_2_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_2_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_2_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_3, tmpVal);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_4);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_3_DO_DLY_SEL, cfg->doDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_3_DI_DLY_SEL, cfg->diDelay);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF3_IO_3_OE_DLY_SEL, cfg->oeDelay);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF3_IF_IO_DLY_4, tmpVal);

    /* Enable AHB access sram buffer and enable sf interface */
    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_AHB2SRAM_EN);
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_EN);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_1, tmpVal);

    SF_Ctrl_Set_Owner(cfg->owner);
}
#endif

/****************************************************************************/ /**
 * @brief  Enable serail psram controller
 *
 * @param  sfCtrlPsramCfg: serial psram controller config
 *
 * @return None
 *
*******************************************************************************/
//#ifndef BFLB_USE_ROM_DRIVER
//__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Psram_Init(SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg)
{
    uint32_t tmpVal = 0;

    SF_Ctrl_Select_Pad(sfCtrlPsramCfg->padSel);
    SF_Ctrl_Select_Bank(sfCtrlPsramCfg->bankSel);

    /* Select psram clock delay */
    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_12);

    if (sfCtrlPsramCfg->psramRxClkInvertSrc) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SRC);

        if (sfCtrlPsramCfg->psramRxClkInvertSel) {
            tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SEL);
        } else {
            tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SEL);
        }
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF2_CLK_SF_RX_INV_SRC);
    }

    if (sfCtrlPsramCfg->psramDelaySrc) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF2_IF_READ_DLY_SRC);

        if (sfCtrlPsramCfg->psramClkDelay > 0) {
            tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF2_IF_READ_DLY_EN);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF2_IF_READ_DLY_N, sfCtrlPsramCfg->psramClkDelay - 1);
        } else {
            tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF2_IF_READ_DLY_EN);
        }
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF2_IF_READ_DLY_SRC);
    }

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_12, tmpVal);

    /* Enable AHB access sram buffer and enable sf interface */
    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_AHB2SRAM_EN);
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_EN);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_1, tmpVal);

    SF_Ctrl_Set_Owner(sfCtrlPsramCfg->owner);
}
//#endif

/****************************************************************************/ /**
 * @brief  Get flash controller clock delay value
 *
 * @param  None
 *
 * @return Clock delay value
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_TCM_SECTION SF_Ctrl_Get_Clock_Delay(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_0);

    if (BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN) == 0) {
        return 0;
    } else {
        return BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_READ_DLY_N) + 1;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Set flash controller clock delay value
 *
 * @param  delay: Clock delay value
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Clock_Delay(uint8_t delay)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_0);

    if (delay > 0) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_READ_DLY_N, delay - 1);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_READ_DLY_EN);
    }

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_0, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  SF Ctrl set cmds config
 *
 * @param  cmdsCfg: SF Ctrl cmds config
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_WRAP_LEN_TYPE(cmdsCfg->wrapLen));

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_3);

    if (cmdsCfg->cmdsEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_CMDS_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_CMDS_EN);
    }

    if (cmdsCfg->burstToggleEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_CMDS_BT_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_CMDS_BT_EN);
    }

    if (cmdsCfg->wrapModeEn) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_CMDS_WRAP_MODE);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_CMDS_WRAP_MODE);
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_CMDS_WRAP_LEN, cmdsCfg->wrapLen);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_3, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  SF Ctrl pad select
 *
 * @param  sel: pad select type
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Select_Pad(SF_Ctrl_Pad_Select sel)
{
    /* TODO: sf_if_bk_swap */
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_PAD_SELECT(sel));

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_2);

    if (sel <= SF_CTRL_PAD_SEL_SF3) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_BK2_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_PAD_SEL, sel);
    } else if (sel >= SF_CTRL_PAD_SEL_DUAL_BANK_SF1_SF2 && sel <= SF_CTRL_PAD_SEL_DUAL_BANK_SF3_SF1) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_BK2_EN);
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_BK2_MODE);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_PAD_SEL, sel - SF_CTRL_PAD_SEL_DUAL_BANK_SF1_SF2);
    } else if (sel == SF_CTRL_PAD_SEL_DUAL_CS_SF2) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_BK2_EN);
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_BK2_MODE);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_PAD_SEL, 1);
    } else if (sel == SF_CTRL_PAD_SEL_DUAL_CS_SF3) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_BK2_EN);
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_BK2_MODE);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_PAD_SEL, 2);
    }

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_2, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  SF Ctrl bank select
 *
 * @param  sel: bank select type
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Select_Bank(SF_Ctrl_Select sel)
{
    /* TODO: sf_if_bk_swap */
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_SELECT(sel));

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_2);

    if (sel == SF_CTRL_SEL_FLASH) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_BK_SEL);
    } else {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_BK_SEL);
    }

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_2, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Set flash controller owner:I/D AHB or system AHB
 *
 * @param  owner: owner type
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Owner(SF_Ctrl_Owner_Type owner)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_OWNER_TYPE(owner));

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);

    /* Set owner */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_FN_SEL, owner);

    /* Set iahb to flash interface */
    if (owner == SF_CTRL_OWNER_IAHB) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_AHB2SIF_EN);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_AHB2SIF_EN);
    }

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_1, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Disable flash controller
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);

    tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_EN);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_1, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Enable flash controller AES with big indian
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable_BE(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_0);

    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_AES_KEY_ENDIAN);
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_AES_IV_ENDIAN);
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_AES_DOUT_ENDIAN);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_0, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Enable flash controller AES with little indian
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable_LE(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_0);

    tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_AES_KEY_ENDIAN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_AES_IV_ENDIAN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_AES_DOUT_ENDIAN);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_0, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES region
 *
 * @param  region: region number
 * @param  enable: enable or not
 * @param  hwKey: hardware key or software key
 * @param  startAddr: region start address
 * @param  endAddr: region end address
 * @param  locked: lock this region or not
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Region(uint8_t region, uint8_t enable,
                                             uint8_t hwKey, uint32_t startAddr, uint32_t endAddr, uint8_t locked)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal;

    if (!hwKey) {
        regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    }

    tmpVal = BL_RD_REG(regionRegBase, SF_CTRL_SF_AES_CFG);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_HW_KEY_EN, hwKey);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_START, startAddr / 1024);
    /* sf_aes_end =1 means 1,11,1111,1111 */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_END, endAddr / 1024);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_EN, enable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_LOCK, locked);

    BL_WR_REG(regionRegBase, SF_CTRL_SF_AES_CFG, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES key
 *
 * @param  region: region number
 * @param  key: key data pointer
 * @param  keyType: flash controller AES key type:128 bits,192 bits or 256 bits
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_AES_KEY_TYPE(keyType));

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_AES);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_AES, tmpVal);

    if (NULL != key) {
        if (keyType == SF_CTRL_AES_128BITS) {
            i = 4;
            /*
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_7,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_6,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_5,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_4,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            */
        } else if (keyType == SF_CTRL_AES_256BITS) {
            i = 8;
            /*
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_7,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_6,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_5,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_4,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_3,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_2,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_1,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_0,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            */
        } else if (keyType == SF_CTRL_AES_192BITS) {
            i = 6;
            /*
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_7,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_6,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_5,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_4,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_3,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_2,__REV(BL_RDWD_FRM_BYTEP(key)));
            key+=4;
            */
        }

        tmpVal = SF_CTRL_SF_AES_KEY_7_OFFSET;

        while (i--) {
            BL_WR_WORD(regionRegBase + tmpVal, __REV(BL_RDWD_FRM_BYTEP(key)));
            key += 4;
            tmpVal -= 4;
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES key with big endian
 *
 * @param  region: region number
 * @param  key: key data pointer
 * @param  keyType: flash controller AES key type:128 bits,192 bits or 256 bits
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_AES_KEY_TYPE(keyType));

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_AES);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_MODE, keyType);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_AES, tmpVal);

    if (NULL != key) {
        if (keyType == SF_CTRL_AES_128BITS) {
            i = 4;
            /*
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_0,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_1,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_2,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_3,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            */
        } else if (keyType == SF_CTRL_AES_256BITS) {
            i = 8;
            /*
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_0,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_1,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_2,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_3,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_4,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_5,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_6,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_7,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            */
        } else if (keyType == SF_CTRL_AES_192BITS) {
            i = 6;
            /*
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_0,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_1,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_2,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_3,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_4,BL_RDWD_FRM_BYTEP(key));
            key+=4;
            BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_KEY_5,BL_RDWD_FRM_BYTEP(key));
            */
        }

        tmpVal = SF_CTRL_SF_AES_KEY_0_OFFSET;

        while (i--) {
            BL_WR_WORD(regionRegBase + tmpVal, BL_RDWD_FRM_BYTEP(key));
            key += 4;
            tmpVal += 4;
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES iv
 *
 * @param  region: region number
 * @param  iv: iv data pointer
 * @param  addrOffset: flash address offset
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 3;

    if (iv != NULL) {
        tmpVal = SF_CTRL_SF_AES_IV_W3_OFFSET;

        while (i--) {
            BL_WR_WORD(regionRegBase + tmpVal, __REV(BL_RDWD_FRM_BYTEP(iv)));
            iv += 4;
            tmpVal -= 4;
        }

        /*
        BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_IV_W3,__REV(BL_RDWD_FRM_BYTEP(iv)));
        iv+=4;
        BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_IV_W2,__REV(BL_RDWD_FRM_BYTEP(iv)));
        iv+=4;
        BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_IV_W1,__REV(BL_RDWD_FRM_BYTEP(iv)));
        iv+=4;
        */
        BL_WR_REG(regionRegBase, SF_CTRL_SF_AES_IV_W0, addrOffset);
        iv += 4;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Serial flash controller set AES iv with big endian
 *
 * @param  region: region number
 * @param  iv: iv data pointer
 * @param  addrOffset: flash address offset
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    /* Do flash key eco*/
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, region);
    uint32_t tmpVal, i = 3;

    if (iv != NULL) {
        tmpVal = SF_CTRL_SF_AES_IV_W0_OFFSET;

        while (i--) {
            BL_WR_WORD(regionRegBase + tmpVal, BL_RDWD_FRM_BYTEP(iv));
            iv += 4;
            tmpVal += 4;
        }

        /*
        BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_IV_W0,BL_RDWD_FRM_BYTEP(iv));
        iv+=4;
        BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_IV_W1,BL_RDWD_FRM_BYTEP(iv));
        iv+=4;
        BL_WR_REG(regionRegBase,SF_CTRL_SF_AES_IV_W2,BL_RDWD_FRM_BYTEP(iv));
        iv+=4;
        */
        BL_WR_REG(regionRegBase, SF_CTRL_SF_AES_IV_W3, __REV(addrOffset));
        iv += 4;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  Enable serial flash controller AES
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_AES);
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_AES_EN);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_AES, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Disable serial flash controller AES
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_AES_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_AES);
    tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_AES_EN);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_AES, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Check is serial flash controller AES enable
 *
 * @param  None
 *
 * @return Wether AES is enable
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_TCM_SECTION SF_Ctrl_Is_AES_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_AES);
    return BL_IS_REG_BIT_SET(tmpVal, SF_CTRL_SF_AES_EN);
}
#endif

/****************************************************************************/ /**
 * @brief  Set flash image offset
 *
 * @param  addrOffset: Address offset value
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset)
{
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_ID0_OFFSET, addrOffset);
}
#endif

/****************************************************************************/ /**
 * @brief  Get flash image offset
 *
 * @param  None
 *
 * @return :Address offset value
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
uint32_t ATTR_TCM_SECTION SF_Ctrl_Get_Flash_Image_Offset(void)
{
    return BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_ID0_OFFSET);
}
#endif

/****************************************************************************/ /**
 * @brief  SF controller send one command
 *
 * @param  sahbType: Serial flash controller clock sahb sram select
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Select_Clock(SF_Ctrl_Sahb_Type sahbType)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_0);

    if (sahbType == SF_CTRL_SAHB_CLOCK) {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_CLK_SAHB_SRAM_SEL);
    } else if (sahbType == SF_CTRL_FLASH_CLOCK) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_CLK_SAHB_SRAM_SEL);
    }

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_0, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  SF controller send one command
 *
 * @param  cfg: Serial flash controller command configuration pointer
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_CMD_MODE_TYPE(cfg->cmdMode));
    CHECK_PARAM(IS_SF_CTRL_ADDR_MODE_TYPE(cfg->addrMode));
    CHECK_PARAM(IS_SF_CTRL_DMY_MODE_TYPE(cfg->dummyMode));
    CHECK_PARAM(IS_SF_CTRL_DATA_MODE_TYPE(cfg->dataMode));

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);

    if (BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_SAHB) {
        return;
    }

    /* Clear trigger */
    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_SAHB_0);
    tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_TRIG);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_SAHB_0, tmpVal);

    /* Copy command buffer */
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_SAHB_1, cfg->cmdBuf[0]);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_SAHB_2, cfg->cmdBuf[1]);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    /* Configure cmd */
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_CMD_EN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_CMD_BYTE, 0);

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_ADR_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_ADR_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_ADR_BYTE, 0);
    }

    /* Configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_DMY_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_DMY_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_DAT_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_DAT_BYTE, cfg->nbData - 1);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_DAT_EN);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_DAT_BYTE, 0);
    }

    /* Set read write flag */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_0_DAT_RW, cfg->rwFlag);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_SAHB_0, tmpVal);

    //switch sf_clk_sahb_sram_sel = 1
    SF_Ctrl_Select_Clock(SF_CTRL_FLASH_CLOCK);
    /* Trigger */
    tmpVal = BL_SET_REG_BIT(tmpVal, SF_CTRL_SF_IF_0_TRIG);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_SAHB_0, tmpVal);

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            SF_Ctrl_Select_Clock(SF_CTRL_SAHB_CLOCK);
            return;
        }
    }

    //switch sf_clk_sahb_sram_sel = 0
    SF_Ctrl_Select_Clock(SF_CTRL_SAHB_CLOCK);
}
#endif

/****************************************************************************/ /**
 * @brief  Config SF controller for flash I/D cache read
 *
 * @param  cfg: Serial flash controller command configuration pointer
 * @param  cmdValid: command valid or not, for continous read, cache may need no command
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_CMD_MODE_TYPE(cfg->cmdMode));
    CHECK_PARAM(IS_SF_CTRL_ADDR_MODE_TYPE(cfg->addrMode));
    CHECK_PARAM(IS_SF_CTRL_DMY_MODE_TYPE(cfg->dummyMode));
    CHECK_PARAM(IS_SF_CTRL_DATA_MODE_TYPE(cfg->dataMode));

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);

    if (BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_IAHB) {
        return;
    }

    /* Copy command buffer */
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_1, cfg->cmdBuf[0]);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_2, cfg->cmdBuf[1]);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_0);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    if (cmdValid) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    }

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, 0);
    }

    /* configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 0);
    }

    /* Set read write flag */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DAT_RW, cfg->rwFlag);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_0, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Config psram controller for psram I/D cache write
 *
 * @param  cfg: Serial flash controller command configuration pointer
 * @param  cmdValid: command valid or not, cache may need no command
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Psram_Write_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_CMD_MODE_TYPE(cfg->cmdMode));
    CHECK_PARAM(IS_SF_CTRL_ADDR_MODE_TYPE(cfg->addrMode));
    CHECK_PARAM(IS_SF_CTRL_DMY_MODE_TYPE(cfg->dummyMode));
    CHECK_PARAM(IS_SF_CTRL_DATA_MODE_TYPE(cfg->dataMode));

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);

    if (BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_IAHB) {
        return;
    }

    /* Copy command buffer */
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_4, cfg->cmdBuf[0]);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_5, cfg->cmdBuf[1]);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_3);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    if (cmdValid) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_CMD_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_CMD_BYTE, 0);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_CMD_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_CMD_BYTE, 0);
    }

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_ADR_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_ADR_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_ADR_BYTE, 0);
    }

    /* configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_DMY_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_DMY_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_DAT_EN, 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_DAT_EN, 0);
    }

    /* Set read write flag */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_2_DAT_RW, cfg->rwFlag);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_3, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Config psram controller for psram I/D cache read
 *
 * @param  cfg: Serial flash controller command configuration pointer
 * @param  cmdValid: command valid or not, for continous read, cache may need no command
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION SF_Ctrl_Psram_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_SF_CTRL_CMD_MODE_TYPE(cfg->cmdMode));
    CHECK_PARAM(IS_SF_CTRL_ADDR_MODE_TYPE(cfg->addrMode));
    CHECK_PARAM(IS_SF_CTRL_DMY_MODE_TYPE(cfg->dummyMode));
    CHECK_PARAM(IS_SF_CTRL_DATA_MODE_TYPE(cfg->dataMode));

    timeOut = SF_CTRL_BUSY_STATE_TIMEOUT;

    while (SET == SF_Ctrl_GetBusyState()) {
        timeOut--;

        if (timeOut == 0) {
            return;
        }
    }

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_1);

    if (BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_FN_SEL) != SF_CTRL_OWNER_IAHB) {
        return;
    }

    /* Copy command buffer */
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_10, cfg->cmdBuf[0]);
    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_11, cfg->cmdBuf[1]);

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_9);

    /* Configure SPI and IO mode*/
    if (SF_CTRL_CMD_1_LINE == cfg->cmdMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_SPI_MODE);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_QPI_MODE_EN, SF_CTRL_QPI_MODE);
    }

    if (SF_CTRL_ADDR_1_LINE == cfg->addrMode) {
        if (SF_CTRL_DATA_1_LINE == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_NIO_MODE);
        } else if (SF_CTRL_DATA_2_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DO_MODE);
        } else if (SF_CTRL_DATA_4_LINES == cfg->dataMode) {
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QO_MODE);
        }
    } else if (SF_CTRL_ADDR_2_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_DIO_MODE);
    } else if (SF_CTRL_ADDR_4_LINES == cfg->addrMode) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_SPI_MODE, SF_CTRL_QIO_MODE);
    }

    if (cmdValid) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_CMD_BYTE, 0);
    }

    /* Configure address */
    if (cfg->addrSize != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, cfg->addrSize - 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_ADR_BYTE, 0);
    }

    /* configure dummy */
    if (cfg->dummyClks != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, cfg->dummyClks - 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_EN, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DMY_BYTE, 0);
    }

    /* Configure data */
    if (cfg->nbData != 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DAT_EN, 0);
    }

    /* Set read write flag */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_IF_1_DAT_RW, cfg->rwFlag);

    BL_WR_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_IAHB_9, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Get SF Ctrl busy state
 *
 * @param  None
 *
 * @return SET  for SF ctrl busy or RESET for SF ctrl not busy
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Sts_Type ATTR_TCM_SECTION SF_Ctrl_GetBusyState(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(SF_CTRL_BASE, SF_CTRL_SF_IF_SAHB_0);

    if (BL_IS_REG_BIT_SET(tmpVal, SF_CTRL_SF_IF_BUSY)) {
        return SET;
    } else {
        return RESET;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  SF Controller interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void SF_Ctrl_IRQHandler(void)
{
    /* TODO: Not implemented */
}
#endif

/*@} end of group SF_CTRL_Public_Functions */

/*@} end of group SF_CTRL */

/*@} end of group BL702_Peripheral_Driver */
