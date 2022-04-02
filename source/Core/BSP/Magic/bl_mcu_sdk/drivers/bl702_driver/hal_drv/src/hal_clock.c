/**
 * @file hal_clock.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bl702_glb.h"
#include "bl702_pwm.h"
#include "bl702_timer.h"
#include "hal_clock.h"

#if XTAL_TYPE != EXTERNAL_XTAL_32M
static void internal_rc32m_init(void)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL_CAPCODE_EXTRA_AON);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_SEL_AON, 0);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    tmpVal = BL_RD_REG(AON_BASE, AON_TSEN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_RDY_INT_SEL_AON, 0);
    BL_WR_REG(AON_BASE, AON_TSEN, tmpVal);

    for (uint32_t i = 0; i < 20000; i++) {
        tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_XTAL_EXT_SEL_AON);
        BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
        tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_XTAL_EXT_SEL_AON);
        BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);
        if (BL_IS_REG_BIT_SET(BL_RD_REG(GLB_BASE, GLB_CLK_CFG0), GLB_CHIP_RDY))
            break;
    }
}
#endif

static uint32_t mtimer_get_clk_src_div(void)
{
    return (system_clock_get(SYSTEM_CLOCK_BCLK) / 1000 / 1000 - 1);
}

static void peripheral_clock_gate_all()
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    // tmpVal &= (~(1 << BL_AHB_SLAVE1_GPIP));
    // tmpVal &= (~(1 << BL_AHB_SLAVE1_SEC_DBG));
    // tmpVal &= (~(1 << BL_AHB_SLAVE1_SEC));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TZ1));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TZ2));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_DMA));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_EMAC));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_UART0));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_UART1));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_SPI));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_I2C));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_PWM));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TMR));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_IRR));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_CKS));
    tmpVal &= (~(1 << 24)); //QDEC0
    tmpVal &= (~(1 << 25)); //QDEC1
    tmpVal &= (~(1 << 26)); //QDEC2/I2S
    tmpVal &= (~(1 << 27)); //KYS
    tmpVal &= (~(1 << BL_AHB_SLAVE1_USB));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_CAM));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_MJPEG));
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);
}

void system_clock_init(void)
{
#if XTAL_TYPE != EXTERNAL_XTAL_32M
    internal_rc32m_init();
    AON_Power_Off_XTAL();
#endif
    /*select root clock*/
    GLB_Set_System_CLK(XTAL_TYPE, BSP_ROOT_CLOCK_SOURCE);
#if BSP_ROOT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_PLL_57P6M
    /* fix 57.6M */
    SystemCoreClockSet(57.6 * 1000 * 1000);
#endif
    /*set fclk/hclk and bclk clock*/
    GLB_Set_System_CLK_Div(BSP_FCLK_DIV, BSP_BCLK_DIV);
    /* Set MTimer the same frequency as SystemCoreClock */
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, mtimer_get_clk_src_div());
#ifndef FAST_WAKEUP
#ifdef BSP_AUDIO_PLL_CLOCK_SOURCE
    PDS_Set_Audio_PLL_Freq(BSP_AUDIO_PLL_CLOCK_SOURCE - ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ);
#endif
#endif
#if XTAL_32K_TYPE == INTERNAL_RC_32K
    HBN_32K_Sel(HBN_32K_RC);
    HBN_Power_Off_Xtal_32K();
#else
    HBN_Power_On_Xtal_32K();
    HBN_32K_Sel(HBN_32K_XTAL);
#endif
#if XTAL_TYPE == EXTERNAL_XTAL_32M
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
#else
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_RC32M);
#endif
}

void peripheral_clock_init(void)
{
    uint32_t tmpVal = 0;

    peripheral_clock_gate_all();

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1)
#if defined(BSP_USING_UART0)
    tmpVal |= (1 << BL_AHB_SLAVE1_UART0);
#endif
#if defined(BSP_USING_UART1)
    tmpVal |= (1 << BL_AHB_SLAVE1_UART1);
#endif
#if BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_PLL_96M
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_96M, BSP_UART_CLOCK_DIV);
#elif BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_FCLK, BSP_UART_CLOCK_DIV);
#else
#error "please select correct uart clock source"
#endif
#endif

#if defined(BSP_USING_I2C0)
#if BSP_I2C_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_BCLK
    tmpVal |= (1 << BL_AHB_SLAVE1_I2C);
    GLB_Set_I2C_CLK(ENABLE, BSP_I2C_CLOCK_DIV);
#else
#error "please select correct i2c clock source"
#endif
#endif

#if defined(BSP_USING_SPI0)
#if BSP_SPI_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_BCLK
    tmpVal |= (1 << BL_AHB_SLAVE1_SPI);
    GLB_Set_SPI_CLK(ENABLE, BSP_SPI_CLOCK_DIV);
#else
#error "please select correct spi clock source"
#endif
#endif

#if defined(BSP_USING_TIMER0)
    tmpVal |= (1 << BL_AHB_SLAVE1_TMR);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

#if BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_FCLK);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#elif BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_XTAL);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#elif BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_32K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#elif BSP_TIMER0_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_1K_CLK
    /* Configure timer clock source */
    uint32_t tmp = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_CS_1, TIMER_CLKSRC_1K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp);

    /* Configure timer clock division */
    tmp = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR2, BSP_TIMER0_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp);
#else
#error "please select correct timer0 clock source"
#endif
#endif

#if defined(BSP_USING_TIMER1)
    tmpVal |= (1 << BL_AHB_SLAVE1_TMR);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

#if BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_FCLK);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#elif BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_XTAL);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#elif BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_32K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#elif BSP_TIMER1_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_1K_CLK
    /* Configure timer clock source */
    uint32_t tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp1, TIMER_CS_2, TIMER_CLKSRC_1K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmp1);

    /* Configure timer clock division */
    tmp1 = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmp1 = BL_SET_REG_BITS_VAL(tmp, TIMER_TCDR3, BSP_TIMER1_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmp1);
#else
#error "please select correct timer1 clock source"
#endif
#endif

#if defined(BSP_USING_WDT)
    tmpVal |= (1 << BL_AHB_SLAVE1_TMR);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

#if BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_FCLK);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#elif BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_XTAL);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#elif BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_32K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#elif BSP_WDT_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_1K_CLK
    /* Configure watchdog timer clock source */
    uint32_t tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_CS_WDT, TIMER_CLKSRC_1K);
    BL_WR_REG(TIMER_BASE, TIMER_TCCR, tmpwdt);

    /* Configure watchdog timer clock division */
    tmpwdt = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
    tmpwdt = BL_SET_REG_BITS_VAL(tmpwdt, TIMER_WCDR, BSP_WDT_CLOCK_DIV);
    BL_WR_REG(TIMER_BASE, TIMER_TCDR, tmpwdt);
#else
#error "please select correct watchdog timer clock source"
#endif
#endif

#if defined(BSP_USING_PWM_CH0) || defined(BSP_USING_PWM_CH1) || defined(BSP_USING_PWM_CH2) || defined(BSP_USING_PWM_CH3) || defined(BSP_USING_PWM_CH4) || defined(BSP_USING_PWM_CH5)
    tmpVal |= (1 << BL_AHB_SLAVE1_PWM);
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

    uint32_t timeoutCnt = 160 * 1000;
    uint32_t tmp_pwm;
    uint32_t PWMx;
#if BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK

    for (int i = 0; i < 5; i++) {
        PWMx = PWM_BASE + PWM_CHANNEL_OFFSET + (i)*0x20;
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmp_pwm, PWM_STOP_EN));

        while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return;
            }
        }
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        tmp_pwm = BL_SET_REG_BITS_VAL(tmp_pwm, PWM_REG_CLK_SEL, PWM_CLK_32K);
        BL_WR_REG(PWMx, PWM_CONFIG, tmp_pwm);
        /* Config pwm division */
        BL_WR_REG(PWMx, PWM_CLKDIV, BSP_PWM_CLOCK_DIV + 1);
    }
#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_BCLK

    for (int i = 0; i < 5; i++) {
        PWMx = PWM_BASE + PWM_CHANNEL_OFFSET + (i)*0x20;
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmp_pwm, PWM_STOP_EN));

        while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return;
            }
        }
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        tmp_pwm = BL_SET_REG_BITS_VAL(tmp_pwm, PWM_REG_CLK_SEL, PWM_CLK_BCLK);
        BL_WR_REG(PWMx, PWM_CONFIG, tmp_pwm);
        /* Config pwm division */
        BL_WR_REG(PWMx, PWM_CLKDIV, BSP_PWM_CLOCK_DIV + 1);
    }
#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK

    for (int i = 0; i < 5; i++) {
        PWMx = PWM_BASE + PWM_CHANNEL_OFFSET + (i)*0x20;
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        BL_WR_REG(PWMx, PWM_CONFIG, BL_SET_REG_BIT(tmp_pwm, PWM_STOP_EN));

        while (!BL_IS_REG_BIT_SET(BL_RD_REG(PWMx, PWM_CONFIG), PWM_STS_TOP)) {
            timeoutCnt--;

            if (timeoutCnt == 0) {
                return;
            }
        }
        tmp_pwm = BL_RD_REG(PWMx, PWM_CONFIG);
        tmp_pwm = BL_SET_REG_BITS_VAL(tmp_pwm, PWM_REG_CLK_SEL, PWM_CLK_XCLK);
        BL_WR_REG(PWMx, PWM_CONFIG, tmp_pwm);
        /* Config pwm division */
        BL_WR_REG(PWMx, PWM_CLKDIV, BSP_PWM_CLOCK_DIV + 1);
    }
#else
#error "please select correct pwm clock source"
#endif
#endif

#if defined(BSP_USING_IR)
    tmpVal |= (1 << BL_AHB_SLAVE1_IRR);
    GLB_Set_IR_CLK(ENABLE, 0, BSP_IR_CLOCK_DIV);
#endif

#if defined(BSP_USING_I2S0)
    tmpVal |= (1 << BL_AHB_SLAVE1_I2S);
    GLB_Set_I2S_CLK(ENABLE, GLB_I2S_OUT_REF_CLK_NONE);
#endif

#if defined(BSP_USING_ADC0)
    tmpVal |= (1 << BL_AHB_SLAVE1_GPIP);
#if BSP_ADC_CLOCK_SOURCE >= ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_AUDIO_PLL, BSP_ADC_CLOCK_DIV);
#elif BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, BSP_ADC_CLOCK_DIV);
#else
#error "please select correct adc clock source"
#endif
#endif

#if defined(BSP_USING_DAC0)
    tmpVal |= (1 << BL_AHB_SLAVE1_GPIP);
#if BSP_DAC_CLOCK_SOURCE >= ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_AUDIO_PLL, BSP_DAC_CLOCK_DIV + 1);
#elif BSP_DAC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_XCLK, BSP_DAC_CLOCK_DIV + 1);
#else
#error "please select correct dac clock source"
#endif
#endif

#if defined(BSP_USING_CAM0)
    tmpVal |= (1 << BL_AHB_SLAVE1_CAM);
#if BSP_CAM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_PLL_96M
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_DLL96M, BSP_CAM_CLOCK_DIV);
    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);
#elif BSP_CAM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_XCLK, BSP_CAM_CLOCK_DIV);
    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);
#else
#error "please select correct camera clock source"
#endif
#endif

#if defined(BSP_USING_QDEC0) || defined(BSP_USING_QDEC1) || defined(BSP_USING_QDEC2) || defined(BSP_USING_KEYSCAN)
#ifdef BSP_USING_KEYSCAN
    tmpVal |= (1 << 27);
#endif
#if defined(BSP_USING_QDEC0)
    tmpVal |= (1 << 24);
#endif
#if defined(BSP_USING_QDEC1)
    tmpVal |= (1 << 25);
#endif
#if defined(BSP_USING_QDEC2)
    tmpVal |= (1 << 26);
#endif
#if BSP_QDEC_KEYSCAN_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_32K_CLK
    GLB_Set_QDEC_CLK(GLB_QDEC_CLK_F32K, BSP_QDEC_KEYSCAN_CLOCK_DIV);
#elif BSP_QDEC_KEYSCAN_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_QDEC_CLK(GLB_QDEC_CLK_XCLK, BSP_QDEC_KEYSCAN_CLOCK_DIV);
#else
#error "please select correct qdec or keyscan clock source"
#endif
#endif

#if defined(BSP_USING_USB)
    tmpVal |= (1 << BL_AHB_SLAVE1_USB);
    GLB_Set_USB_CLK(1);
#endif
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);
}

uint32_t system_clock_get(enum system_clock_type type)
{
    switch (type) {
        case SYSTEM_CLOCK_ROOT_CLOCK:
            if (GLB_Get_Root_CLK_Sel() == 0) {
                return 32 * 1000 * 1000;
            } else if (GLB_Get_Root_CLK_Sel() == 1)
                return 32 * 1000 * 1000;
            else {
                uint32_t tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_PLL_SEL);
                if (tmpVal == 0) {
                    return 57.6 * 1000 * 1000;
                } else if (tmpVal == 1) {
                    return 96 * 1000 * 1000;
                } else if (tmpVal == 2) {
                    return 144 * 1000 * 1000;
                } else {
                    return 0;
                }
            }
        case SYSTEM_CLOCK_FCLK:
            return system_clock_get(SYSTEM_CLOCK_ROOT_CLOCK) / (GLB_Get_HCLK_Div() + 1);

        case SYSTEM_CLOCK_BCLK:
            return system_clock_get(SYSTEM_CLOCK_ROOT_CLOCK) / (GLB_Get_HCLK_Div() + 1) / (GLB_Get_BCLK_Div() + 1);

        case SYSTEM_CLOCK_XCLK:
            return 32000000;
        case SYSTEM_CLOCK_32K_CLK:
            return 32000;
        case SYSTEM_CLOCK_AUPLL:
#ifdef BSP_AUDIO_PLL_CLOCK_SOURCE
            if (BSP_AUDIO_PLL_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ) {
                return 12288000;
            } else if (BSP_AUDIO_PLL_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL_11289600_HZ) {
                return 11289600;
            } else if (BSP_AUDIO_PLL_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL_5644800_HZ) {
                return 5644800;
            } else if (BSP_AUDIO_PLL_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL_24576000_HZ) {
                return 24576000;
            } else if (BSP_AUDIO_PLL_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL_24000000_HZ) {
                return 24000000;
            }
#endif
        default:
            break;
    }

    return 0;
}

uint32_t peripheral_clock_get(enum peripheral_clock_type type)
{
    uint32_t tmpVal;
    uint32_t div;

    switch (type) {
#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1)
        case PERIPHERAL_CLOCK_UART:
            tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL);

            div = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
            div = BL_GET_REG_BITS_VAL(div, GLB_UART_CLK_DIV);
            if (tmpVal == HBN_UART_CLK_FCLK) {
                return system_clock_get(SYSTEM_CLOCK_FCLK) / (div + 1);
            } else if (tmpVal == HBN_UART_CLK_96M) {
                return 96000000 / (div + 1);
            }
#endif
#if defined(BSP_USING_SPI0)
        case PERIPHERAL_CLOCK_SPI:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
            div = BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV);
            return system_clock_get(SYSTEM_CLOCK_BCLK) / (div + 1);
#endif
#if defined(BSP_USING_I2C0)
        case PERIPHERAL_CLOCK_I2C:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
            div = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV);
            return system_clock_get(SYSTEM_CLOCK_BCLK) / (div + 1);
#endif
#if defined(BSP_USING_I2S0)
        case PERIPHERAL_CLOCK_I2S:
            return system_clock_get(SYSTEM_CLOCK_AUPLL);
#endif
#if defined(BSP_USING_ADC0)
        case PERIPHERAL_CLOCK_ADC:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_GPADC_32M_SRC_CTRL);
            div = BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_DIV);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_GPADC_32M_CLK_SEL);
            if (tmpVal == GLB_ADC_CLK_AUDIO_PLL) {
                return system_clock_get(SYSTEM_CLOCK_AUPLL) / (div + 1);
            } else if (tmpVal == GLB_ADC_CLK_XCLK) {
                return system_clock_get(SYSTEM_CLOCK_XCLK) / (div + 1);
            }
#endif
#if defined(BSP_USING_DAC0)
        case PERIPHERAL_CLOCK_DAC:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_DIG32K_WAKEUP_CTRL);
            div = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_512K_DIV);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_DIG_CLK_SRC_SEL);
            if (tmpVal == GLB_DAC_CLK_AUDIO_PLL) {
                return system_clock_get(SYSTEM_CLOCK_AUPLL) / div;
            } else if (tmpVal == GLB_DAC_CLK_XCLK) {
                return system_clock_get(SYSTEM_CLOCK_XCLK) / div;
            }
#endif
#if defined(BSP_USING_TIMER0)
        case PERIPHERAL_CLOCK_TIMER0:
            tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_CS_1);

            div = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
            div = BL_GET_REG_BITS_VAL(div, TIMER_TCDR2);
            if (tmpVal == TIMER_CLKSRC_FCLK) {
                return system_clock_get(SYSTEM_CLOCK_FCLK) / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_32K) {
                return system_clock_get(SYSTEM_CLOCK_32K_CLK) / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_1K) {
                return 1000 / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_XTAL) {
                return system_clock_get(SYSTEM_CLOCK_XCLK) / (div + 1);
            }
#endif
#if defined(BSP_USING_TIMER1)
        case PERIPHERAL_CLOCK_TIMER1:
            tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_CS_2);

            div = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
            div = BL_GET_REG_BITS_VAL(div, TIMER_TCDR3);
            if (tmpVal == TIMER_CLKSRC_FCLK) {
                return system_clock_get(SYSTEM_CLOCK_FCLK) / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_32K) {
                return system_clock_get(SYSTEM_CLOCK_32K_CLK) / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_1K) {
                return 1000 / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_XTAL) {
                return system_clock_get(SYSTEM_CLOCK_XCLK) / (div + 1);
            }
#endif
#if defined(BSP_USING_WDT)
        case PERIPHERAL_CLOCK_WDT:
            tmpVal = BL_RD_REG(TIMER_BASE, TIMER_TCCR);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, TIMER_CS_WDT);

            div = BL_RD_REG(TIMER_BASE, TIMER_TCDR);
            div = BL_GET_REG_BITS_VAL(div, TIMER_WCDR);
            if (tmpVal == TIMER_CLKSRC_FCLK) {
                return system_clock_get(SYSTEM_CLOCK_FCLK) / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_32K) {
                return system_clock_get(SYSTEM_CLOCK_32K_CLK) / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_1K) {
                return 1000 / (div + 1);
            } else if (tmpVal == TIMER_CLKSRC_XTAL) {
                return system_clock_get(SYSTEM_CLOCK_XCLK) / (div + 1);
            }
#endif
#if defined(BSP_USING_PWM_CH0) || defined(BSP_USING_PWM_CH1) || defined(BSP_USING_PWM_CH2) || defined(BSP_USING_PWM_CH3) || defined(BSP_USING_PWM_CH4)
        case PERIPHERAL_CLOCK_PWM:
            tmpVal = BL_RD_REG(PWM_BASE + PWM_CHANNEL_OFFSET, PWM_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, PWM_REG_CLK_SEL);
            div = BL_RD_REG(PWM_BASE + PWM_CHANNEL_OFFSET, PWM_CLKDIV);
            if (tmpVal == PWM_CLK_XCLK) {
                return system_clock_get(SYSTEM_CLOCK_XCLK) / div;
            } else if (tmpVal == PWM_CLK_BCLK) {
                return system_clock_get(SYSTEM_CLOCK_BCLK) / div;
            } else if (tmpVal == PWM_CLK_32K) {
                return system_clock_get(SYSTEM_CLOCK_32K_CLK) / div;
            }
#endif
#if defined(BSP_USING_CAM)
        case PERIPHERAL_CLOCK_CAM:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG1);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_SRC_SEL);
            div = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_CAM_REF_CLK_DIV);

            if (tmpVal == GLB_CAM_CLK_XCLK) {
                return system_clock_get(SYSTEM_CLOCK_XCLK) / (div + 1);
            } else if (tmpVal == GLB_CAM_CLK_DLL96M) {
                return 96000000 / (div + 1);
            }
#endif
        default:

            break;
    }

    (void)(tmpVal);
    (void)(div);
    return 0;
}

void system_mtimer_clock_init(void)
{
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, mtimer_get_clk_src_div());
}

void system_mtimer_clock_reinit(void)
{
    /* reinit clock to 10M */
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 7);
}
