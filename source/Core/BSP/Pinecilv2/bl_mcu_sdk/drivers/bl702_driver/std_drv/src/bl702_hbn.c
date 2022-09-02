/**
  ******************************************************************************
  * @file    bl702_hbn.c
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

#include "bl702_hbn.h"
#include "bl702_glb.h"
#include "bl702_xip_sflash.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  HBN
 *  @{
 */

/** @defgroup  HBN_Private_Macros
 *  @{
 */
#define HBN_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }

/*@} end of group HBN_Private_Macros */

/** @defgroup  HBN_Private_Types
 *  @{
 */

/*@} end of group HBN_Private_Types */

/** @defgroup  HBN_Private_Variables
 *  @{
 */
static intCallback_Type *hbnInt0CbfArra[HBN_OUT0_MAX] = { NULL, NULL, NULL, NULL, NULL, NULL };
static intCallback_Type *hbnInt1CbfArra[HBN_OUT1_MAX] = { NULL, NULL, NULL, NULL };

/*@} end of group HBN_Private_Variables */

/** @defgroup  HBN_Global_Variables
 *  @{
 */

/*@} end of group HBN_Global_Variables */

/** @defgroup  HBN_Private_Fun_Declaration
 *  @{
 */

/*@} end of group HBN_Private_Fun_Declaration */

/** @defgroup  HBN_Private_Functions
 *  @{
 */

/*@} end of group HBN_Private_Functions */

/** @defgroup  HBN_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Enter HBN
 *
 * @param  cfg: HBN APP Config
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Mode_Enter(HBN_APP_CFG_Type *cfg)
{
    uint32_t valLow = 0, valHigh = 0;
    uint64_t val;

    /* work clock select */
    if (cfg->useXtal32k) {
        HBN_32K_Sel(HBN_32K_XTAL);
    } else {
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }

    /* turn off RC32K during HBN */
    if ((cfg->hbnLevel) >= HBN_LEVEL_2) {
        HBN_Power_Off_RC32K();
    } else {
        HBN_Power_On_RC32K();
    }

    /* clear aon pad interrupt before config them */
    HBN_Clear_IRQ(HBN_INT_GPIO9);
    HBN_Clear_IRQ(HBN_INT_GPIO10);
    HBN_Clear_IRQ(HBN_INT_GPIO11);
    HBN_Clear_IRQ(HBN_INT_GPIO12);
    HBN_Clear_IRQ(HBN_INT_GPIO13);

    /* always disable HBN pin pull up/down to reduce PDS/HBN current, 0x4000F014[16]=0 */
    HBN_Hw_Pu_Pd_Cfg(DISABLE);

    HBN_Pin_WakeUp_Mask(~(cfg->gpioWakeupSrc));

    if (cfg->gpioWakeupSrc != 0) {
        HBN_Aon_Pad_IeSmt_Cfg(cfg->gpioWakeupSrc);
        HBN_GPIO_INT_Enable(cfg->gpioTrigType);
    } else {
        HBN_Aon_Pad_IeSmt_Cfg(0);
    }

    /* HBN RTC config and enable */
    if (cfg->sleepTime != 0) {
        // set rtc enable flag
        BL_WR_WORD(0x40010FFC, 0x1);

        HBN_Clear_RTC_Counter();
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        val = valLow + ((uint64_t)valHigh << 32);
        val += cfg->sleepTime;
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, val & 0xffffffff, val >> 32, HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
    }

    HBN_Power_Down_Flash(cfg->flashCfg);

    switch (cfg->flashPinCfg) {
        case 0:
            HBN_Set_Pad_23_28_Pullup();
            break;

        case 1:
            /* need do nothing */
            break;

        case 2:
            /* need do nothing */
            break;

        case 3:
            /* can do nothing */
            break;

        default:
            break;
    }

    GLB_Set_System_CLK(GLB_DLL_XTAL_NONE, GLB_SYS_CLK_RC32M);

    /* power off xtal */
    AON_Power_Off_XTAL();

    HBN_Enable_Ext(cfg->gpioWakeupSrc, cfg->ldoLevel, cfg->hbnLevel);
}

/****************************************************************************/ /**
 * @brief  power down and switch clock
 *
 * @param  flashCfg: None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Power_Down_Flash(SPI_Flash_Cfg_Type *flashCfg)
{
    SPI_Flash_Cfg_Type bhFlashCfg;

    if (flashCfg == NULL) {
        /* fix this some time */
        /* SFlash_Cache_Flush(); */
        XIP_SFlash_Read_Via_Cache_Need_Lock(BL702_FLASH_XIP_BASE + 8 + 4, (uint8_t *)(&bhFlashCfg), sizeof(SPI_Flash_Cfg_Type));
        /* fix this some time */
        /* SFlash_Cache_Flush(); */

        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(&bhFlashCfg);
    } else {
        SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);
        SFlash_Reset_Continue_Read(flashCfg);
    }

    SFlash_Powerdown();
}

/****************************************************************************/ /**
 * @brief  Enable HBN mode
 *
 * @param  aGPIOIeCfg: AON GPIO IE config,Bit0->GPIO18. Bit(s) of Wakeup GPIO(s) must not be set to
 *                     0(s),say when use GPIO7 as wake up pin,aGPIOIeCfg should be 0x01.
 * @param  ldoLevel: LDO volatge level
 * @param  hbnLevel: HBN work level
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Enable_Ext(uint8_t aGPIOIeCfg, HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));
    CHECK_PARAM(IS_HBN_LEVEL_TYPE(hbnLevel));

    /* Setting from guide */
    /* RAM Retion, no longer use */
    /* BL_WR_REG(HBN_BASE,HBN_SRAM,0x24); */

    /* AON GPIO IE */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_PAD_IE_SMT, aGPIOIeCfg);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    /* HBN mode LDO level */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_RT_VOUT_SEL, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Select RC32M */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);

    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbnLevel) {
        case HBN_LEVEL_0:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_2:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_3:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        default:
            break;
    }

    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Enable HBN mode */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    while (1) {
        BL702_Delay_MS(1000);
    }
}

/****************************************************************************/ /**
 * @brief  Reset HBN mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION HBN_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Reset HBN mode */
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  reset HBN by software
 *
 * @param  npXtalType: NP clock type
 * @param  bclkDiv: NP clock div
 * @param  apXtalType: AP clock type
 * @param  fclkDiv: AP clock div
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_App_Reset(uint8_t npXtalType, uint8_t bclkDiv, uint8_t apXtalType, uint8_t fclkDiv)
{
    uint32_t tmp[12];

    tmp[0] = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmp[1] = BL_RD_REG(HBN_BASE, HBN_TIME_L);
    tmp[2] = BL_RD_REG(HBN_BASE, HBN_TIME_H);
    tmp[3] = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmp[4] = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmp[5] = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmp[6] = BL_RD_REG(HBN_BASE, HBN_PIR_VTH);
    tmp[7] = BL_RD_REG(HBN_BASE, HBN_PIR_INTERVAL);
    tmp[8] = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmp[9] = BL_RD_REG(HBN_BASE, HBN_RSV0);
    tmp[10] = BL_RD_REG(HBN_BASE, HBN_RSV1);
    tmp[11] = BL_RD_REG(HBN_BASE, HBN_RSV2);
    /* DO HBN reset */
    HBN_Reset();
    /* HBN need 3 32k cyclce to recovery */
    BL702_Delay_US(100);
    /* Recover HBN value */
    BL_WR_REG(HBN_BASE, HBN_TIME_L, tmp[1]);
    BL_WR_REG(HBN_BASE, HBN_TIME_H, tmp[2]);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmp[0]);

    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmp[3]);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmp[4]);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmp[5]);
    BL_WR_REG(HBN_BASE, HBN_PIR_VTH, tmp[6]);
    BL_WR_REG(HBN_BASE, HBN_PIR_INTERVAL, tmp[7]);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmp[8]);
    BL_WR_REG(HBN_BASE, HBN_RSV0, tmp[9]);
    BL_WR_REG(HBN_BASE, HBN_RSV1, tmp[10]);
    BL_WR_REG(HBN_BASE, HBN_RSV2, tmp[11]);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable HBN mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Disable HBN mode */
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable HBN PIR
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PIR_EN);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable HBN PIR
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PIR_EN);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Config HBN PIR interrupt
 *
 * @param  pirIntCfg: HBN PIR interrupt configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg)
{
    uint32_t tmpVal;
    uint32_t bit4 = 0;
    uint32_t bit5 = 0;
    uint32_t bitVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);

    /* low trigger interrupt */
    if (pirIntCfg->lowIntEn == ENABLE) {
        bit5 = 0;
    } else {
        bit5 = 1;
    }

    /* high trigger interrupt */
    if (pirIntCfg->highIntEn == ENABLE) {
        bit4 = 0;
    } else {
        bit4 = 1;
    }

    bitVal = bit4 | (bit5 << 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_DIS, bitVal);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select HBN PIR low pass filter
 *
 * @param  lpf: HBN PIR low pass filter selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_LPF_Sel(HBN_PIR_LPF_Type lpf)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_PIR_LPF_TYPE(lpf));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_LPF_SEL, lpf);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select HBN PIR high pass filter
 *
 * @param  hpf: HBN PIR high pass filter selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_PIR_HPF_Sel(HBN_PIR_HPF_Type hpf)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_PIR_HPF_TYPE(hpf));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_HPF_SEL, hpf);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set HBN PIR threshold value
 *
 * @param  threshold: HBN PIR threshold value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_PIR_Threshold(uint16_t threshold)
{
    uint32_t tmpVal;

    CHECK_PARAM((threshold <= 0x3FFF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_VTH);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_VTH, threshold);
    BL_WR_REG(HBN_BASE, HBN_PIR_VTH, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN PIR threshold value
 *
 * @param  None
 *
 * @return HBN PIR threshold value
 *
*******************************************************************************/
uint16_t HBN_Get_PIR_Threshold(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_VTH);

    return BL_GET_REG_BITS_VAL(tmpVal, HBN_PIR_VTH);
}

/****************************************************************************/ /**
 * @brief  Set HBN PIR interval value
 *
 * @param  interval: HBN PIR interval value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_PIR_Interval(uint16_t interval)
{
    uint32_t tmpVal;

    CHECK_PARAM((interval <= 0xFFF));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_INTERVAL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIR_INTERVAL, interval);
    BL_WR_REG(HBN_BASE, HBN_PIR_INTERVAL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN PIR interval value
 *
 * @param  None
 *
 * @return HBN PIR interval value
 *
*******************************************************************************/
uint16_t HBN_Get_PIR_Interval(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PIR_INTERVAL);

    return BL_GET_REG_BITS_VAL(tmpVal, HBN_PIR_INTERVAL);
}

/****************************************************************************/ /**
 * @brief  get HBN bor out state
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_BOR_OUT_State(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_MISC), HBN_R_BOR_OUT) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  set HBN bor config
 *
 * @param  enable: ENABLE or DISABLE, if enable, Power up Brown Out Reset
 * @param  threshold: bor threshold
 * @param  mode: bor work mode with por
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOR_Config(uint8_t enable, HBN_BOR_THRES_Type threshold, HBN_BOR_MODE_Type mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_BOR_THRES_TYPE(threshold));
    CHECK_PARAM(IS_HBN_BOR_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);

    if (enable) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOR, 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOR, 0);
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOR_VTH, threshold);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOR_SEL, mode);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11aon voltage out
 *
 * @param  ldoLevel: LDO volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11_Aon_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11rt voltage out
 *
 * @param  ldoLevel: LDO volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11_Rt_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11_RT_VOUT_SEL, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11soc voltage out
 *
 * @param  ldoLevel: LDO volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11_Soc_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11 all voltage out
 *
 * @param  ldoLevel: LDO volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11_RT_VOUT_SEL, ldoLevel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11rt drive strength
 *
 * @param  strength: ldo11rt drive strength
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11rt_Drive_Strength(HBN_LDO11RT_DRIVE_STRENGTH_Type strength)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO11RT_DRIVE_STRENGTH_TYPE(strength));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_RT_ILOAD_SEL, strength);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN select 32K
 *
 * @param  clkType: HBN 32k clock type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_32K_Sel(HBN_32K_CLK_Type clkType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_32K_CLK_TYPE(clkType));

    HBN_Trim_RC32K();

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_F32K_SEL, clkType);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select uart clock source
 *
 * @param  clkSel: uart clock type selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_UART_CLK_Sel(HBN_UART_CLK_Type clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_UART_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL, clkSel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select xclk clock source
 *
 * @param  xClk: xclk clock type selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_Type xClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_XCLK_CLK_TYPE(xClk));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);

    switch (xClk) {
        case HBN_XCLK_CLK_RC32M:
            tmpVal2 &= (~(1 << 0));
            break;

        case HBN_XCLK_CLK_XTAL:
            tmpVal2 |= (1 << 0);
            break;

        default:
            break;
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select root clk source
 *
 * @param  rootClk: root clock type selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_Type rootClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_ROOT_CLK_TYPE(rootClk));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);

    switch (rootClk) {
        case HBN_ROOT_CLK_RC32M:
            tmpVal2 = 0x0;
            break;

        case HBN_ROOT_CLK_XTAL:
            tmpVal2 = 0x1;
            break;

        case HBN_ROOT_CLK_DLL:
            tmpVal2 |= (1 << 1);
            break;

        default:
            break;
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  set HBN_RAM sleep mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_HRAM_slp(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RETRAM_SLP);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RETRAM_RET);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set HBN_RAM retension mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_HRAM_Ret(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RETRAM_SLP);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RETRAM_RET);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_On_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_XTAL32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_XTAL32K_BUF);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    /* Delay >1s */
    BL702_Delay_US(1100);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_Off_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_XTAL32K_BUF);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on RC32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_On_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_RC32K);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    /* Delay >800us */
    BL702_Delay_US(880);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off RC3K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_Off_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_RC32K);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Trim RC32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION HBN_Trim_RC32K(void)
{
    Efuse_Ana_RC32K_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_RC32K_Trim(&trim);

    if (trim.trimRc32kExtCodeEn) {
        if (trim.trimRc32kCodeFrExtParity == EF_Ctrl_Get_Trim_Parity(trim.trimRc32kCodeFrExt, 10)) {
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_CTRL0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32K_CODE_FR_EXT, trim.trimRc32kCodeFrExt);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RC32K_EXT_CODE_EN);
            BL_WR_REG(HBN_BASE, HBN_RC32K_CTRL0, tmpVal);
            BL702_Delay_US(2);
            return SUCCESS;
        }
    }

    return ERROR;
}
#endif

/****************************************************************************/ /**
 * @brief  Get HBN status flag
 *
 * @param  None
 *
 * @return HBN status flag value
 *
*******************************************************************************/
uint32_t HBN_Get_Status_Flag(void)
{
    return BL_RD_REG(HBN_BASE, HBN_RSV0);
}

/****************************************************************************/ /**
 * @brief  Set HBN status flag
 *
 * @param  flag: Status Flag
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_Status_Flag(uint32_t flag)
{
    BL_WR_REG(HBN_BASE, HBN_RSV0, flag);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN wakeup address
 *
 * @param  None
 *
 * @return HBN wakeup address
 *
*******************************************************************************/
uint32_t HBN_Get_Wakeup_Addr(void)
{
    return BL_RD_REG(HBN_BASE, HBN_RSV1);
}

/****************************************************************************/ /**
 * @brief  Set HBN wakeup address
 *
 * @param  addr: HBN wakeup address
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_Wakeup_Addr(uint32_t addr)
{
    BL_WR_REG(HBN_BASE, HBN_RSV1, addr);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer counter
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Clear RTC control bit0 */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal & 0xfffffffe);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer counter
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Set RTC control bit0 */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal | 0x01);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set RTC timer configuration
 *
 * @param  delay: RTC interrupt delay 32 clocks
 * @param  compValLow: RTC interrupt commpare value low 32 bits
 * @param  compValHigh: RTC interrupt commpare value high 32 bits
 * @param  compMode: RTC interrupt commpare
 *                   mode:HBN_RTC_COMP_BIT0_39,HBN_RTC_COMP_BIT0_23,HBN_RTC_COMP_BIT13_39
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_RTC_Timer(HBN_RTC_INT_Delay_Type delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_RTC_INT_DELAY_TYPE(delay));

    BL_WR_REG(HBN_BASE, HBN_TIME_L, compValLow);
    BL_WR_REG(HBN_BASE, HBN_TIME_H, compValHigh & 0xff);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Set interrupt delay option */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RTC_DLY_OPTION, delay);
    /* Set RTC compare mode */
    tmpVal |= (compMode << 1);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get RTC async timer count value
 *
 * @param  valLow: RTC count value pointer for low 32 bits
 * @param  valHigh: RTC count value pointer for high 8 bits
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type HBN_Get_RTC_Timer_Async_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpVal;

    /* Tigger RTC val read */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
    BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
    BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);

    /* Read RTC val */
    *valLow = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_L);
    *valHigh = (BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H) & 0xff);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get RTC timer count value
 *
 * @param  valLow: RTC count value pointer for low 32 bits
 * @param  valHigh: RTC count value pointer for high 8 bits
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint64_t val, val1;

    do {
        HBN_Get_RTC_Timer_Async_Val(&tmpValLow, &tmpValHigh);
        val = ((uint64_t)tmpValHigh << 32) | ((uint64_t)tmpValLow);
        HBN_Get_RTC_Timer_Async_Val(&tmpValLow1, &tmpValHigh1);
        val1 = ((uint64_t)tmpValHigh1 << 32) | ((uint64_t)tmpValLow1);
    } while (val1 < val);

    *valLow = tmpValLow1;
    *valHigh = tmpValHigh1;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer interrupt,this function must be called to clear delayed rtc IRQ
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_RTC_INT(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Clear RTC commpare:bit1-3 for clearing Delayed RTC IRQ */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal & 0xfffffff1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable GPIO interrupt
 *
 * @param  gpioIntTrigType: HBN GPIO interrupt trigger type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_GPIO_INT_Enable(HBN_GPIO_INT_Trigger_Type gpioIntTrigType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(gpioIntTrigType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable GPIO interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_GPIO_INT_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, 0);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get interrupt status
 *
 * @param  irqType: HBN interrupt type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_INT_State(HBN_INT_Type irqType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_STAT);

    if (tmpVal & (1 << irqType)) {
        return SET;
    } else {
        return RESET;
    }
}

/****************************************************************************/ /**
 * @brief  HBN get pin wakeup mode value
 *
 * @param  None
 *
 * @return HBN pin wakeup mode value
 *
*******************************************************************************/
uint8_t HBN_Get_Pin_Wakeup_Mode(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_IRQ_MODE), HBN_PIN_WAKEUP_MODE);
}

/****************************************************************************/ /**
 * @brief  HBN clear interrupt status
 *
 * @param  irqType: HBN interrupt type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_IRQ(HBN_INT_Type irqType)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_INT_TYPE(irqType));

    /* set clear bit */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmpVal |= (1 << irqType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

    /* unset clear bit */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmpVal &= (~(1 << irqType));
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN hardware pullup or pulldown configuration
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Hw_Pu_Pd_Cfg(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);

    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    }

    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Config AON pad input and SMT
 *
 * @param  padCfg: AON pad config
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Aon_Pad_IeSmt_Cfg(uint8_t padCfg)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_PAD_IE_SMT, padCfg);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN wakeup pin mask configuration
 *
 * @param  maskVal: mask value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Pin_WakeUp_Mask(uint8_t maskVal)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable ACOMP0 interrupt
 *
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN);
    tmpVal2 = tmpVal2 | (1 << edge);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable ACOMP0 interrupt
 *
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_AComp0_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN);
    tmpVal2 = tmpVal2 & (~(1 << edge));
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable ACOMP1 interrupt
 *
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN);
    tmpVal2 = tmpVal2 | (1 << edge);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable ACOMP1 interrupt
 *
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_AComp1_IRQ(HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN);
    tmpVal2 = tmpVal2 & (~(1 << edge));
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable BOR interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_BOR_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable BOR interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_BOR_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_IRQ_BOR_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get HBN reset event status
 *
 * @param  event: HBN reset event type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_Reset_Event(HBN_RST_EVENT_Type event)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_RESET_EVENT);

    return (tmpVal & (1 << event)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  clear HBN reset event status
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_Reset_Event(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_CLEAR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_CLEAR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_CLEAR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN out0 install interrupt callback
 *
 * @param  intType: HBN out0 interrupt type
 * @param  cbFun: HBN out0 interrupt callback
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Out0_Callback_Install(HBN_OUT0_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_HBN_OUT0_INT_TYPE(intType));

    hbnInt0CbfArra[intType] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN out1 install interrupt callback
 *
 * @param  intType: HBN out1 interrupt type
 * @param  cbFun: HBN out1 interrupt callback
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Out1_Callback_Install(HBN_OUT1_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_HBN_OUT1_INT_TYPE(intType));

    hbnInt1CbfArra[intType] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN GPIO debug pull config
 *
 * @param  pupdEn: Enable or disable GPIO pull down and pull up
 * @param  dlyEn: Enable or disable GPIO wakeup delay function
 * @param  dlySec: GPIO wakeup delay sec 1 to 7
 * @param  gpioIrq: HBN GPIO num
 * @param  gpioMask: HBN GPIO MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION HBN_GPIO_Dbg_Pull_Cfg(BL_Fun_Type pupdEn, BL_Fun_Type dlyEn, uint8_t dlySec, HBN_INT_Type gpioIrq, BL_Mask_Type gpioMask)
{
    uint32_t tmpVal;

    CHECK_PARAM(((dlySec >= 1) && (dlySec <= 7)));
    CHECK_PARAM((gpioIrq >= HBN_INT_GPIO9) && (gpioIrq <= HBN_INT_GPIO13));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_EN, dlyEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_SEL, dlySec);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_EN_HW_PU_PD, pupdEn);

    if (gpioMask != UNMASK) {
        tmpVal = tmpVal | (1 << (gpioIrq + 8));
    } else {
        tmpVal = tmpVal & ~(1 << (gpioIrq + 8));
    }

    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Set pad 23-28 pull none
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Pad_23_28_Pullnone(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLDOWN_AON, 0x00);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLUP_AON, 0x00);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set pad 23-28 pull up
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Pad_23_28_Pullup(void)
{
    uint32_t tmpVal = 0;

    /********************************************/
    /* GPIO28 is bootpin, so leave it pull none */
    /********************************************/

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLDOWN_AON, 0x00);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLUP_AON, 0x1F);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set pad 23-28 pull down
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Pad_23_28_Pulldown(void)
{
    uint32_t tmpVal = 0;

    /********************************************/
    /* GPIO28 is bootpin, so leave it pull none */
    /********************************************/

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLDOWN_AON, 0x1F);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLUP_AON, 0x00);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set pad 23-28 active ie
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Pad_23_28_ActiveIE(void)
{
    uint32_t tmpVal = 0;

    /********************************************/
    /* GPIO28 is bootpin, so leave it pull none */
    /********************************************/

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLDOWN_AON, 0x1F);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_FLASH_PULLUP_AON, 0x1F);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set BOR config
 *
 * @param  cfg: Enable or disable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOR_Cfg(HBN_BOR_CFG_Type *cfg)
{
    uint32_t tmpVal = 0;

    if (cfg->enableBorInt) {
        HBN_Enable_BOR_IRQ();
    } else {
        HBN_Disable_BOR_IRQ();
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOR_VTH, cfg->borThreshold);

    if (cfg->enablePorInBor) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_BOR_SEL);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_BOR_SEL);
    }

    if (cfg->enableBor) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_BOR);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_BOR);
    }

    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN OUT0 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void HBN_OUT0_IRQHandler(void)
{
    if (SET == HBN_Get_INT_State(HBN_INT_GPIO9)) {
        HBN_Clear_IRQ(HBN_INT_GPIO9);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO9] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO9]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO10)) {
        HBN_Clear_IRQ(HBN_INT_GPIO10);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO10] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO10]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO11)) {
        HBN_Clear_IRQ(HBN_INT_GPIO11);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO11] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO11]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO12)) {
        HBN_Clear_IRQ(HBN_INT_GPIO12);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO12] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO12]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO13)) {
        HBN_Clear_IRQ(HBN_INT_GPIO13);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO13] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO13]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_RTC)) {
        HBN_Clear_IRQ(HBN_INT_RTC);
        HBN_Clear_RTC_INT();

        if (hbnInt0CbfArra[HBN_OUT0_INT_RTC] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_RTC]();
        }
    }
}

/****************************************************************************/ /**
 * @brief  HBN OUT1 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void HBN_OUT1_IRQHandler(void)
{
    /* PIR */
    if (SET == HBN_Get_INT_State(HBN_INT_PIR)) {
        HBN_Clear_IRQ(HBN_INT_PIR);

        if (hbnInt1CbfArra[HBN_OUT1_INT_PIR] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_PIR]();
        }
    }

    /* BOR */
    if (SET == HBN_Get_INT_State(HBN_INT_BOR)) {
        HBN_Clear_IRQ(HBN_INT_BOR);

        if (hbnInt1CbfArra[HBN_OUT1_INT_BOR] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_BOR]();
        }
    }

    /* ACOMP0 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP0)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP0);

        if (hbnInt1CbfArra[HBN_OUT1_INT_ACOMP0] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_ACOMP0]();
        }
    }

    /* ACOMP1 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP1)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP1);

        if (hbnInt1CbfArra[HBN_OUT1_INT_ACOMP1] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_ACOMP1]();
        }
    }
}

/****************************************************************************/ /**
 * @brief  Enable HBN mode
 *
 * @param  aGPIOIeCfg: AON GPIO IE config,Bit0->GPIO18. Bit(s) of Wakeup GPIO(s) must not be set to
 *                     0(s),say when use GPIO7 as wake up pin,aGPIOIeCfg should be 0x01.
 * @param  ldoLevel: LDO volatge level
 * @param  hbnLevel: HBN work level
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Enable(uint8_t aGPIOIeCfg, HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));
    CHECK_PARAM(IS_HBN_LEVEL_TYPE(hbnLevel));

    /* Setting from guide */
    /* RAM Retion */
    BL_WR_REG(HBN_BASE, HBN_SRAM, 0x24);
    /* AON GPIO IE */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_PAD_IE_SMT, aGPIOIeCfg);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    ///* Power off 1.8V */
    //tmpVal=BL_RD_REG(AON_BASE,AON_PMIP);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_PU_TOPLDO11_SOC);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_PU_TOPLDO18_RF);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_PU_TOPLDO18_IO);
    ///* SOC11 enum is not the same as VDD11*/
    //tmpVal=BL_SET_REG_BITS_VAL(tmpVal,AON_TOPLDO11_SOC_VOUT_SEL,ldoLevel-1);
    //BL_WR_REG(AON_BASE,AON_PMIP,tmpVal);
    //
    ///* Set RT voltage */
    //tmpVal=BL_RD_REG(AON_BASE,AON);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_TOPLDO18_IO_SW3);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_TOPLDO18_IO_SW2);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_TOPLDO18_IO_SW1);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_TOPLDO18_IO_BYPASS);
    //tmpVal=BL_CLR_REG_BIT(tmpVal,AON_PU_LDO18_AON);
    ///* RT11 enum is not the same as VDD11*/
    //tmpVal=BL_SET_REG_BITS_VAL(tmpVal,AON_TOPLDO11_RT_VOUT_SEL,ldoLevel-1);
    //tmpVal=BL_SET_REG_BITS_VAL(tmpVal,AON_VDD11_SEL,ldoLevel);
    //BL_WR_REG(AON_BASE,AON,tmpVal);

    /* Select RC32M */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);

    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbnLevel) {
        case HBN_LEVEL_0:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_2:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_3:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        default:
            break;
    }

    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Enable HBN mode */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    while (1) {
        BL702_Delay_MS(1000);
    }
}

/****************************************************************************/ /**
 * @brief  HBN out0 IRQHandler install
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Out0_IRQHandler_Install(void)
{
    Interrupt_Handler_Register(HBN_OUT0_IRQn, HBN_OUT0_IRQHandler);
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN out1 IRQHandler install
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Out1_IRQHandler_Install(void)
{
    Interrupt_Handler_Register(HBN_OUT1_IRQn, HBN_OUT1_IRQHandler);
    return SUCCESS;
}

/*@} end of group HBN_Public_Functions */

/*@} end of group HBN */

/*@} end of group BL702_Peripheral_Driver */
