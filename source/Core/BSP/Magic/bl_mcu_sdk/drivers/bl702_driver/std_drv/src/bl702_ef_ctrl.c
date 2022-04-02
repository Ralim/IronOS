/**
  ******************************************************************************
  * @file    bl702_sec_ef_ctrl.c
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

#include "string.h"
#include "bl702_ef_ctrl.h"
#include "ef_data_0_reg.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  SEC_EF_CTRL
 *  @{
 */

/** @defgroup  SEC_EF_CTRL_Private_Macros
 *  @{
 */
#define EF_CTRL_EFUSE_CYCLE_PROTECT (0xbf << 24)
#define EF_CTRL_EFUSE_CTRL_PROTECT  (0xbf << 8)
#define EF_CTRL_DFT_TIMEOUT_VAL     (160 * 1000)
#ifndef BOOTROM
#define EF_CTRL_LOAD_BEFORE_READ_R0 EF_Ctrl_Load_Efuse_R0()
#else
#define EF_CTRL_LOAD_BEFORE_READ_R0
#endif
#define EF_CTRL_DATA0_CLEAR EF_Ctrl_Clear(0, EF_CTRL_EFUSE_R0_SIZE / 4)

/*@} end of group SEC_EF_CTRL_Private_Macros */

/** @defgroup  SEC_EF_CTRL_Private_Types
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Types */

/** @defgroup  SEC_EF_CTRL_Private_Variables
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Variables */

/** @defgroup  SEC_EF_CTRL_Global_Variables
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Global_Variables */

/** @defgroup  SEC_EF_CTRL_Private_Fun_Declaration
 *  @{
 */

/*@} end of group SEC_EF_CTRL_Private_Fun_Declaration */

/** @defgroup  SEC_EF_CTRL_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Switch efuse region 0 control to AHB clock
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
/* static */ void ATTR_TCM_SECTION EF_Ctrl_Sw_AHB_Clk_0(void)
{
    uint32_t tmpVal;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    while (EF_Ctrl_Busy() == SET) {
        timeout--;

        if (timeout == 0) {
            break;
        }
    }

    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (EF_CTRL_SAHB_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);

    BL_WR_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Program efuse region 0
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void EF_Ctrl_Program_Efuse_0(void)
{
    uint32_t tmpVal;

    /* Select auto mode and select ef clock */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    BL_WR_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0, tmpVal);

    /* Program */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (1 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (1 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    BL_WR_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0, tmpVal);

    /* Add delay for POR to be stable */
    BL702_Delay_US(4);

    /* Trigger */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (1 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (1 << EF_CTRL_EF_IF_0_RW_POS) |
             (1 << EF_CTRL_EF_IF_0_TRIG_POS);
    BL_WR_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0, tmpVal);
}

/*@} end of group SEC_EF_CTRL_Private_Functions */

/** @defgroup  SEC_EF_CTRL_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Load efuse region 0
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION EF_Ctrl_Load_Efuse_R0(void)
{
    uint32_t tmpVal;
    uint32_t timeout = EF_CTRL_DFT_TIMEOUT_VAL;

    EF_CTRL_DATA0_CLEAR;

    /* Trigger read */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    BL_WR_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0, tmpVal);

    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (EF_CTRL_EF_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (1 << EF_CTRL_EF_IF_0_TRIG_POS);
    BL_WR_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0, tmpVal);

    BL702_Delay_US(10);

    /* Wait for efuse control idle*/
    do {
        tmpVal = BL_RD_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0);
        timeout--;

        if (timeout == 0) {
            break;
        }
    } while (BL_IS_REG_BIT_SET(tmpVal, EF_CTRL_EF_IF_0_BUSY) ||

             (!BL_IS_REG_BIT_SET(tmpVal, EF_CTRL_EF_IF_0_AUTOLOAD_DONE)));

    /* Switch to AHB clock */
    tmpVal = (EF_CTRL_EFUSE_CTRL_PROTECT) |
             (EF_CTRL_OP_MODE_AUTO << EF_CTRL_EF_IF_0_MANUAL_EN_POS) |
             (EF_CTRL_PARA_DFT << EF_CTRL_EF_IF_0_CYC_MODIFY_POS) |
             (EF_CTRL_SAHB_CLK << EF_CTRL_EF_CLK_SAHB_DATA_SEL_POS) |
             (1 << EF_CTRL_EF_IF_AUTO_RD_EN_POS) |
             (0 << EF_CTRL_EF_IF_POR_DIG_POS) |
             (1 << EF_CTRL_EF_IF_0_INT_CLR_POS) |
             (0 << EF_CTRL_EF_IF_0_RW_POS) |
             (0 << EF_CTRL_EF_IF_0_TRIG_POS);
    BL_WR_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0, tmpVal);
}
#endif

/****************************************************************************/ /**
 * @brief  Check efuse busy status
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Sts_Type ATTR_TCM_SECTION EF_Ctrl_Busy(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0);

    if (BL_IS_REG_BIT_SET(tmpVal, EF_CTRL_EF_IF_0_BUSY)) {
        return SET;
    }

    return RESET;
}
#endif

/****************************************************************************/ /**
 * @brief  Check efuse whether finish loading
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type EF_Ctrl_AutoLoad_Done(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(EF_CTRL_BASE, EF_CTRL_EF_IF_CTRL_0);

    if (BL_IS_REG_BIT_SET(tmpVal, EF_CTRL_EF_IF_0_AUTOLOAD_DONE)) {
        return SET;
    } else {
        return RESET;
    }
}

/****************************************************************************/ /**
 * @brief  Efuse write debug password
 *
 * @param  passWdLow: password low 32 bits
 * @param  passWdHigh: password high 32 bits
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_Dbg_Pwd(uint32_t passWdLow, uint32_t passWdHigh, uint8_t program)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_LOW, passWdLow);
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_HIGH, passWdHigh);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read debug password
 *
 * @param  passWdLow: password low 32 bits pointer to store value
 * @param  passWdHigh: password high 32 bits pointer to store value
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Dbg_Pwd(uint32_t *passWdLow, uint32_t *passWdHigh)
{
    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    *passWdLow = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_LOW);
    *passWdHigh = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_HIGH);
}

/****************************************************************************/ /**
 * @brief  Efuse lock reading for passwd
 *
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Readlock_Dbg_Pwd(uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_LOCK);
    tmpVal = BL_SET_REG_BIT(tmpVal, EF_DATA_0_RD_LOCK_DBG_PWD);
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_LOCK, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse lock writing for passwd
 *
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Writelock_Dbg_Pwd(uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_LOCK);
    tmpVal = BL_SET_REG_BIT(tmpVal, EF_DATA_0_WR_LOCK_DBG_PWD);
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_LOCK, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read security configuration
 *
 * @param  cfg: security configuration pointer
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg, uint8_t program)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_CFG_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_DBG_MODE, cfg->ef_dbg_mode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_DBG_JTAG_0_DIS, cfg->ef_dbg_jtag_0_dis);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_SBOOT_EN, cfg->ef_sboot_en);
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_CFG_0, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read security configuration
 *
 * @param  cfg: security configuration pointer
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg)
{
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_CFG_0);

    cfg->ef_dbg_mode = (EF_Ctrl_Dbg_Mode_Type)BL_GET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_DBG_MODE);
    cfg->ef_dbg_jtag_0_dis = BL_GET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_DBG_JTAG_0_DIS);
    cfg->ef_sboot_en = BL_GET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_SBOOT_EN);
}

/****************************************************************************/ /**
 * @brief  Efuse write security boot configuration
 *
 * @param  sign[1]: Sign configuration pointer
 * @param  aes[1]: AES configuration pointer
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_Secure_Boot(EF_Ctrl_Sign_Type sign[1], EF_Ctrl_SF_AES_Type aes[1], uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_CFG_0);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_SBOOT_SIGN_MODE, sign[0]);

    if (aes[0] != EF_CTRL_SF_AES_NONE) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_SF_AES_MODE, aes[0]);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_CPU0_ENC_EN, 1);
    }

    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_CFG_0, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse write security boot configuration
 *
 * @param  sign[1]: Sign configuration pointer
 * @param  aes[1]: AES configuration pointer
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Secure_Boot(EF_Ctrl_Sign_Type sign[1], EF_Ctrl_SF_AES_Type aes[1])
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_CFG_0);

    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_SBOOT_SIGN_MODE);
    sign[0] = (EF_Ctrl_Sign_Type)(tmpVal2 & 0x01);

    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_CPU0_ENC_EN);

    if (tmpVal2) {
        aes[0] = (EF_Ctrl_SF_AES_Type)BL_GET_REG_BITS_VAL(tmpVal, EF_DATA_0_EF_SF_AES_MODE);
    } else {
        aes[0] = EF_CTRL_SF_AES_NONE;
    }
}

/****************************************************************************/ /**
 * @brief  Analog Trim parity calculate
 *
 * @param  val: Value of efuse trim data
 * @param  len: Length of bit to calculate
 *
 * @return Parity bit value
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
uint8_t ATTR_CLOCK_SECTION EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len)
{
    uint8_t cnt = 0;
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        if (val & (1 << i)) {
            cnt++;
        }
    }

    return cnt & 0x01;
}
#endif

/****************************************************************************/ /**
 * @brief  Efuse write analog trim
 *
 * @param  index: index of analog trim
 * @param  trim: trim value
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_Ana_Trim(uint32_t index, uint32_t trim, uint8_t program)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (index == 0) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_ANA_TRIM_0, trim);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read analog trim
 *
 * @param  index: index of analog trim
 * @param  trim: trim value
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Ana_Trim(uint32_t index, uint32_t *trim)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    if (index == 0) {
        *trim = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_ANA_TRIM_0);
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read RC32M trim
 *
 * @param  trim: Trim data pointer
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_RC32M_Trim(Efuse_Ana_RC32M_Trim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_ANA_TRIM_0);
    trim->trimRc32mCodeFrExt = (tmpVal >> 10) & 0xff;
    trim->trimRc32mCodeFrExtParity = (tmpVal >> 18) & 0x01;
    trim->trimRc32mExtCodeEn = (tmpVal >> 19) & 0x01;
}
#endif

/****************************************************************************/ /**
 * @brief  Efuse read RC32K trim
 *
 * @param  trim: Trim data pointer
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_CLOCK_SECTION EF_Ctrl_Read_RC32K_Trim(Efuse_Ana_RC32K_Trim_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_ANA_TRIM_0);
    trim->trimRc32kCodeFrExt = (tmpVal >> 20) & 0x3ff;
    trim->trimRc32kCodeFrExtParity = (tmpVal >> 30) & 0x01;
    trim->trimRc32kExtCodeEn = (tmpVal >> 31) & 0x01;
}
#endif

/****************************************************************************/ /**
 * @brief  Efuse read TSEN trim
 *
 * @param  trim: Trim data pointer
 *
 * @return None
 *
*******************************************************************************/
void ATTR_CLOCK_SECTION EF_Ctrl_Read_TSEN_Trim(Efuse_TSEN_Refcode_Corner_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
    trim->tsenRefcodeCornerEn = tmpVal & 0x01;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_LOCK);
    trim->tsenRefcodeCorner = tmpVal & 0xfff;
    trim->tsenRefcodeCornerParity = (tmpVal >> 12) & 0x01;

    trim->tsenRefcodeCornerVersion = 0;
}

/****************************************************************************/ /**
 * @brief  Efuse read ADC Gain trim
 *
 * @param  trim: Trim data pointer
 *
 * @return None
 *
*******************************************************************************/
void ATTR_CLOCK_SECTION EF_Ctrl_Read_ADC_Gain_Trim(Efuse_ADC_Gain_Coeff_Type *trim)
{
    uint32_t tmpVal = 0;
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
    trim->adcGainCoeff = (tmpVal >> 1) & 0xfff;
    trim->adcGainCoeffParity = (tmpVal >> 13) & 0x01;
    trim->adcGainCoeffEn = (tmpVal >> 14) & 0x01;
}

/****************************************************************************/ /**
 * @brief  Efuse write software usage
 *
 * @param  index: index of software usage
 * @param  usage: usage value
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_Sw_Usage(uint32_t index, uint32_t usage, uint8_t program)
{
    /* switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (index == 0) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_SW_USAGE_0, usage);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read software usage
 *
 * @param  index: index of software usage
 * @param  usage: usage value
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Sw_Usage(uint32_t index, uint32_t *usage)
{
    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    EF_CTRL_LOAD_BEFORE_READ_R0;

    if (index == 0) {
        *usage = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_SW_USAGE_0);
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read software usage
 *
 * @param  index: index of software usage
 * @param  program: usage value
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Writelock_Sw_Usage(uint32_t index, uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_LOCK);

    if (index == 0) {
        tmpVal = BL_SET_REG_BIT(tmpVal, EF_DATA_0_WR_LOCK_SW_USAGE_0);
    }

    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_LOCK, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse write MAC address
 *
 * @param  mac[6]: MAC address buffer
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_MAC_Address(uint8_t mac[6], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* The low 32 bits */
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_LOW, BL_RDWD_FRM_BYTEP(maclow));
    /* The high 16 bits */
    tmpVal = machigh[0] + (machigh[1] << 8);
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_HIGH, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse Ctrl get zero bit count
 *
 * @param  val: Value to count
 *
 * @return Zero bit count
 *
*******************************************************************************/
static uint32_t EF_Ctrl_Get_Byte_Zero_Cnt(uint8_t val)
{
    uint32_t cnt = 0;
    uint32_t i = 0;

    for (i = 0; i < 8; i++) {
        if ((val & (1 << i)) == 0) {
            cnt += 1;
        }
    }

    return cnt;
}

/****************************************************************************/ /**
 * @brief  Efuse read MAC address
 *
 * @param  mac[8]: MAC address buffer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Read_MAC_Address(uint8_t mac[8])
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;
    uint32_t i = 0;
    uint32_t cnt = 0;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_LOW);
    BL_WRWD_TO_BYTEP(maclow, tmpVal);

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_HIGH);
    BL_WRWD_TO_BYTEP(machigh, tmpVal);

    /* Get original parity */
    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W2);

    /* Check parity */
    for (i = 0; i < 8; i++) {
        cnt += EF_Ctrl_Get_Byte_Zero_Cnt(mac[i]);
    }

    if ((cnt & 0x3f) == (tmpVal & 0x3f)) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read MAC address
 *
 * @param  mac[7]: MAC address buffer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Read_MAC_Address_Raw(uint8_t mac[7])
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_LOW);
    BL_WRWD_TO_BYTEP(maclow, tmpVal);

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_HIGH);
    machigh[0] = tmpVal & 0xff;
    machigh[1] = (tmpVal >> 8) & 0xff;
    machigh[2] = (tmpVal >> 16) & 0xff;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse lock writing for MAC address
 *
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Writelock_MAC_Address(uint8_t program)
{
    uint32_t tmpVal;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_LOCK);
    tmpVal = BL_SET_REG_BIT(tmpVal, EF_DATA_0_WR_LOCK_WIFI_MAC);
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_LOCK, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Whether a value bits is all zero
 *
 * @param  val: value to check
 * @param  start: start bit
 * @param  len: total length of bits to check
 *
 * @return 1 for all bits zero 0 for others
 *
*******************************************************************************/
uint8_t EF_Ctrl_Is_All_Bits_Zero(uint32_t val, uint8_t start, uint8_t len)
{
    uint32_t mask = 0;

    val = (val >> start);

    if (len >= 32) {
        mask = 0xffffffff;
    } else {
        mask = (1 << len) - 1;
    }

    if ((val & mask) == 0) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  Whether MAC address slot is empty
 *
 * @param  slot: MAC address slot
 * @param  reload: whether  reload to check
 *
 * @return 0 for all slots full,1 for others
 *
*******************************************************************************/
uint8_t EF_Ctrl_Is_MAC_Address_Slot_Empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp1 = 0xffffffff, tmp2 = 0xffffffff;
    uint32_t part1Empty = 0, part2Empty = 0;

    if (slot == 0) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_LOW);
        tmp2 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_HIGH);
    } else if (slot == 1) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W0);
        tmp2 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W1);
    } else if (slot == 2) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();

        if (reload) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }

        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_LOW);
        tmp2 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_HIGH);
    }

    part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 0, 32));
    part2Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp2, 0, 22));

    return (part1Empty && part2Empty);
}

/****************************************************************************/ /**
 * @brief  Efuse write optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[8]: MAC address buffer
 * @param  program: Whether program
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Write_MAC_Address_Opt(uint8_t slot, uint8_t mac[8], uint8_t program)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;

    if (slot >= 3) {
        return ERROR;
    }

    if (slot == 2) {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();
    } else {
        /* Switch to AHB clock */
        EF_Ctrl_Sw_AHB_Clk_0();
    }

    /* The low 32 bits */
    if (slot == 0) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_LOW, BL_RDWD_FRM_BYTEP(maclow));
    } else if (slot == 1) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W0, BL_RDWD_FRM_BYTEP(maclow));
    } else if (slot == 2) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_LOW, BL_RDWD_FRM_BYTEP(maclow));
    }

    /* The high 32 bits */
    if (slot == 0) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_HIGH, BL_RDWD_FRM_BYTEP(machigh));
    } else if (slot == 1) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W1, BL_RDWD_FRM_BYTEP(machigh));
    } else if (slot == 2) {
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_HIGH, BL_RDWD_FRM_BYTEP(machigh));
    }

    cnt = 0;
    for (i = 0; i < 8; i++) {
        cnt += EF_Ctrl_Get_Byte_Zero_Cnt(mac[i]);
    }
    cnt &= 0x3f;
    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W2);
    if (slot == 0) {
        tmpVal |= (cnt << 0);
    } else if (slot == 1) {
        tmpVal |= (cnt << 6);
    } else if (slot == 2) {
        tmpVal |= (cnt << 12);
    }
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W2, tmpVal);

    if (program) {
        if (slot == 2) {
            EF_Ctrl_Program_Efuse_0();
        } else {
            EF_Ctrl_Program_Efuse_0();
        }
    }
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read optional MAC address
 *
 * @param  slot: MAC address slot
 * @param  mac[8]: MAC address buffer
 * @param  reload: Whether reload
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Read_MAC_Address_Opt(uint8_t slot, uint8_t mac[8], uint8_t reload)
{
    uint8_t *maclow = (uint8_t *)mac;
    uint8_t *machigh = (uint8_t *)(mac + 4);
    uint32_t tmpVal = 0;
    uint32_t i = 0;
    uint32_t cnt = 0;
    uint32_t crc = 0;

    if (slot >= 3) {
        return ERROR;
    }

    /* Trigger read data from efuse */
    if (reload) {
        if (slot == 2) {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        } else {
            EF_CTRL_LOAD_BEFORE_READ_R0;
        }
    }

    if (slot == 0) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_LOW);
    } else if (slot == 1) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W0);
    } else if (slot == 2) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_LOW);
    }
    BL_WRWD_TO_BYTEP(maclow, tmpVal);

    if (slot == 0) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_WIFI_MAC_HIGH);
    } else if (slot == 1) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W1);
    } else if (slot == 2) {
        tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_DBG_PWD_HIGH);
    }
    BL_WRWD_TO_BYTEP(machigh, tmpVal);

    /* Get original parity */
    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W2);
    if (slot == 0) {
        crc = ((tmpVal >> 0) & 0x3f);
    } else if (slot == 1) {
        crc = ((tmpVal >> 6) & 0x3f);
    } else if (slot == 2) {
        crc = ((tmpVal >> 12) & 0x3f);
    }

    /* Check parity */
    for (i = 0; i < 8; i++) {
        cnt += EF_Ctrl_Get_Byte_Zero_Cnt(mac[i]);
    }
    if ((cnt & 0x3f) == crc) {
        return SUCCESS;
    } else {
        return ERROR;
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read chip ID
 *
 * @param  chipID[8]: Chip ID buffer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Read_Chip_ID(uint8_t chipID[8])
{
    return EF_Ctrl_Read_MAC_Address(chipID);
}

/****************************************************************************/ /**
 * @brief  Efuse get chip PID&&VID
 *
 * @param  pid[1]: Chip PID
 * @param  vid[1]: Chip VID
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Get_Chip_PIDVID(uint16_t pid[1], uint16_t vid[1])
{
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_4_W3);
    pid[0] = (uint16_t)(tmpVal & 0xFFFF);
    vid[0] = (uint16_t)(tmpVal >> 16);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse get customer PID&&VID
 *
 * @param  pid[1]: Customer PID
 * @param  vid[1]: Customer VID
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Get_Customer_PIDVID(uint16_t pid[1], uint16_t vid[1])
{
    uint32_t tmpVal;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_4_W2);
    pid[0] = (uint16_t)(tmpVal & 0xFFFF);
    vid[0] = (uint16_t)(tmpVal >> 16);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read device info
 *
 * @param  deviceInfo: Device info pointer
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo)
{
    uint32_t tmpVal;
    uint32_t *p = (uint32_t *)deviceInfo;

    /* Trigger read data from efuse */
    EF_CTRL_LOAD_BEFORE_READ_R0;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W2);
    *p = tmpVal;
}

/****************************************************************************/ /**
 * @brief  Whether Capcode is empty
 *
 * @param  slot: Cap code slot
 * @param  reload: Whether reload
 *
 * @return 0 for all slots full,1 for others
 *
*******************************************************************************/
uint8_t EF_Ctrl_Is_CapCode_Empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp = 0xffffffff;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
        return (EF_Ctrl_Is_All_Bits_Zero(tmp, 25, 7));
    } else if (slot == 1) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        return (EF_Ctrl_Is_All_Bits_Zero(tmp, 9, 7));
    } else if (slot == 2) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        return (EF_Ctrl_Is_All_Bits_Zero(tmp, 25, 7));
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  Efuse write Cap code
 *
 * @param  slot: Cap code slot
 * @param  code: Cap code value
 * @param  program: Whether program
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Write_CapCode_Opt(uint8_t slot, uint8_t code, uint8_t program)
{
    uint32_t tmp;
    uint8_t trim;

    if (slot >= 3) {
        return ERROR;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();
    EF_CTRL_LOAD_BEFORE_READ_R0;

    trim = (code << 1);
    trim |= (1 << 0);

    if (slot == 0) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3, tmp | (trim << 25));
    } else if (slot == 1) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3, tmp | (trim << 9));
    } else if (slot == 2) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3, tmp | (trim << 25));
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
    while (SET == EF_Ctrl_Busy())
        ;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read Cap code
 *
 * @param  slot: Cap code slot
 * @param  code: Cap code pointer
 * @param  reload: Whether reload
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Read_CapCode_Opt(uint8_t slot, uint8_t *code, uint8_t reload)
{
    uint32_t tmp;
    uint8_t trim = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
        trim = (tmp >> 25) & 0x7f;
    } else if (slot == 1) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        trim = (tmp >> 9) & 0x7f;
    } else if (slot == 2) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        trim = (tmp >> 25) & 0x7f;
    }

    if (trim & 0x01) {
        *code = trim >> 1;
        return SUCCESS;
    }
    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Whether power offset slot is empty
 *
 * @param  slot: Power offset code slot
 * @param  reload: Whether reload
 *
 * @return 0 for all slots full,1 for others
 *
*******************************************************************************/
uint8_t EF_Ctrl_Is_PowerOffset_Slot_Empty(uint8_t slot, uint8_t reload)
{
    uint32_t tmp1 = 0xffffffff;
    uint32_t part1Empty = 0, part2Empty = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
        part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 16, 9));
        part2Empty = 1;
    } else if (slot == 1) {
        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 0, 9));
        part2Empty = 1;
    } else if (slot == 2) {
        tmp1 = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        part1Empty = (EF_Ctrl_Is_All_Bits_Zero(tmp1, 16, 9));
        part2Empty = 1;
    }

    return (part1Empty && part2Empty);
}

/****************************************************************************/ /**
 * @brief  Efuse write power offset
 *
 * @param  slot: Power offset slot
 * @param  pwrOffset[2]: Power offset value array
 * @param  program: Whether program
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Write_PowerOffset_Opt(uint8_t slot, int8_t pwrOffset[2], uint8_t program)
{
    uint32_t tmp = 0;
    uint32_t k = 0;
    uint32_t Value = 0;

    if (slot >= 3) {
        return ERROR;
    }

    for (k = 0; k < 2; k++) {
        /* Use 4 bits as signed value */
        if (pwrOffset[k] > 7) {
            pwrOffset[k] = 7;
        }
        if (pwrOffset[k] < -8) {
            pwrOffset[k] = -8;
        }
        Value += (uint32_t)(pwrOffset[k] & 0x0f) << (k * 4);
    }

    if (slot == 0) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
        tmp |= (Value << 17);
        tmp |= (1 << 16);
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3, tmp);
    } else if (slot == 1) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        tmp |= (Value << 1);
        tmp |= (1 << 0);
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3, tmp);
    } else if (slot == 2) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        tmp |= (Value << 17);
        tmp |= (1 << 16);
        BL_WR_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3, tmp);
    }

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }

    while (SET == EF_Ctrl_Busy())
        ;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Efuse read poweroffset value
 *
 * @param  slot: Power offset slot
 * @param  pwrOffset[2]: Power offset array
 * @param  reload: Whether reload
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Read_PowerOffset_Opt(uint8_t slot, int8_t pwrOffset[2], uint8_t reload)
{
    uint32_t pwrOffsetValue = 0;
    uint32_t tmp = 0, k;
    uint8_t en = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    if (reload) {
        EF_CTRL_LOAD_BEFORE_READ_R0;
    }

    if (slot == 0) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_5_W3);
        en = (tmp >> 16) & 0x01;
        pwrOffsetValue = (tmp >> 17) & 0xff;
    } else if (slot == 1) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        en = (tmp >> 0) & 0x01;
        pwrOffsetValue = (tmp >> 1) & 0xff;
    } else if (slot == 2) {
        tmp = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_EF_KEY_SLOT_3_W3);
        en = (tmp >> 16) & 0x01;
        pwrOffsetValue = (tmp >> 17) & 0xff;
    }

    if (en) {
        for (k = 0; k < 2; k++) {
            tmp = (pwrOffsetValue >> (k * 4)) & 0x0f;
            if (tmp >= 8) {
                pwrOffset[k] = tmp - 16;
            } else {
                pwrOffset[k] = tmp;
            }
        }
        return SUCCESS;
    }
    return ERROR;
}

/****************************************************************************/ /**
 * @brief  Efuse write AES key
 *
 * @param  index: index of key slot
 * @param  keyData: key data buffer
 * @param  len: key data length in words
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Write_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program)
{
    uint32_t *pAESKeyStart0 = (uint32_t *)(EF_DATA_BASE + 0x1C);

    if (index > 5) {
        return;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Every key is 4 words len*/
    BL702_MemCpy4(pAESKeyStart0 + index * 4, keyData, len);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse read AES key from specified region and index
 *
 * @param  index: index of key slot
 * @param  keyData: key data buffer
 * @param  len: key data length in words
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len)
{
    uint32_t *pAESKeyStart0 = (uint32_t *)(EF_DATA_BASE + 0x1C);

    if (index > 5) {
        return;
    }

    /* Trigger read data from efuse*/
    EF_CTRL_LOAD_BEFORE_READ_R0;

    /* Every key is 4 words len*/
    BL702_MemCpy4(keyData, pAESKeyStart0 + index * 4, len);
}

/****************************************************************************/ /**
 * @brief  Efuse lock writing for aes key
 *
 * @param  index: index of key slot
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Writelock_AES_Key(uint8_t index, uint8_t program)
{
    uint32_t tmpVal;

    if (index > 5) {
        return;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_LOCK);

    if (index <= 3) {
        tmpVal |= (1 << (index + 19));
    } else {
        tmpVal |= (1 << (index + 19));
        tmpVal |= (1 << (index - 4 + 13));
    }

    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_LOCK, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Efuse lock reading for aes key
 *
 * @param  index: index of key slot
 * @param  program: program to efuse entity or not
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Readlock_AES_Key(uint8_t index, uint8_t program)
{
    uint32_t tmpVal;

    if (index > 5) {
        return;
    }

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_DATA_0_LOCK);
    tmpVal |= (1 << (index + 26));
    BL_WR_REG(EF_DATA_BASE, EF_DATA_0_LOCK, tmpVal);

    if (program) {
        EF_Ctrl_Program_Efuse_0();
    }
}

/****************************************************************************/ /**
 * @brief  Program data to efuse region 0
 *
 * @param  index: index of efuse in word
 * @param  data: data buffer
 * @param  len: data length
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Add delay for CLK to be stable */
    BL702_Delay_US(4);

    BL702_MemCpy4(pEfuseStart0 + index, data, len);

    EF_Ctrl_Program_Efuse_0();
}

/****************************************************************************/ /**
 * @brief  Read data from efuse region 0
 *
 * @param  index: index of efuse in word
 * @param  data: data buffer
 * @param  len: data length
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);

    EF_CTRL_LOAD_BEFORE_READ_R0;

    BL702_MemCpy4(data, pEfuseStart0 + index, len);
}

/****************************************************************************/ /**
 * @brief  Clear efuse data register
 *
 * @param  index: index of efuse in word
 * @param  len: data length
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION EF_Ctrl_Clear(uint32_t index, uint32_t len)
{
    uint32_t *pEfuseStart0 = (uint32_t *)(EF_DATA_BASE + 0x00);
    uint32_t i = 0;

    /* Switch to AHB clock */
    EF_Ctrl_Sw_AHB_Clk_0();

    /* Clear data */
    for (i = 0; i < len; i++) {
        pEfuseStart0[index + i] = 0;
    }
}
#endif

/****************************************************************************/ /**
 * @brief  efuse ctrl crc enable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Crc_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_CTRL_EF_CRC_CTRL_0);
    tmpVal = BL_SET_REG_BIT(tmpVal, EF_CTRL_EF_CRC_TRIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EF_CTRL_EF_CRC_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, EF_CTRL_EF_CRC_DOUT_INV_EN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EF_CTRL_EF_CRC_DOUT_ENDIAN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EF_CTRL_EF_CRC_DIN_ENDIAN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EF_CTRL_EF_CRC_INT_CLR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EF_CTRL_EF_CRC_INT_SET);
    BL_WR_REG(EF_DATA_BASE, EF_CTRL_EF_CRC_CTRL_0, tmpVal);

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_CTRL_EF_CRC_CTRL_0);
    tmpVal = BL_SET_REG_BIT(tmpVal, EF_CTRL_EF_CRC_EN);
    BL_WR_REG(EF_DATA_BASE, EF_CTRL_EF_CRC_CTRL_0, tmpVal);
}

/****************************************************************************/ /**
 * @brief  efuse ctrl get crc busy status
 *
 * @param  None
 *
 * @return DISABLE or ENABLE
 *
*******************************************************************************/
BL_Sts_Type EF_Ctrl_Crc_Is_Busy(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_CTRL_EF_CRC_CTRL_0);
    return (BL_Sts_Type)BL_IS_REG_BIT_SET(tmpVal, EF_CTRL_EF_CRC_BUSY);
}

/****************************************************************************/ /**
 * @brief  efuse ctrl set golden value
 *
 * @param  goldenValue: Crc golden value
 *
 * @return None
 *
*******************************************************************************/
void EF_Ctrl_Crc_Set_Golden(uint32_t goldenValue)
{
    BL_WR_REG(EF_DATA_BASE, EF_CTRL_EF_CRC_CTRL_4, goldenValue);
}

/****************************************************************************/ /**
 * @brief  efuse ctrl get crc result
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EF_Ctrl_Crc_Result(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(EF_DATA_BASE, EF_CTRL_EF_CRC_CTRL_0);
    return (BL_Err_Type)BL_IS_REG_BIT_SET(tmpVal, EF_CTRL_EF_CRC_ERROR);
}

/*@} end of group SEC_EF_CTRL_Public_Functions */

/*@} end of group SEC_EF_CTRL */

/*@} end of group BL702_Peripheral_Driver */
