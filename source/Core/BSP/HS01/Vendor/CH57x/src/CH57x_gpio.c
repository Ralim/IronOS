/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_gpio.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH57x_common.h"

/*********************************************************************
 * @fn      GPIOA_ModeCfg
 *
 * @brief   GPIOA端口引脚模式配置
 *
 * @param   pin     - PA0-PA15
 * @param   mode    - 输入输出类型
 *
 * @return  none
 */
void GPIOA_ModeCfg(uint32_t pin, GPIOModeTypeDef mode) {
  switch (mode) {
  case GPIO_ModeIN_Floating:
    R32_PA_PD_DRV &= ~pin;
    R32_PA_PU &= ~pin;
    R32_PA_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PU:
    R32_PA_PD_DRV &= ~pin;
    R32_PA_PU |= pin;
    R32_PA_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PD:
    R32_PA_PD_DRV |= pin;
    R32_PA_PU &= ~pin;
    R32_PA_DIR &= ~pin;
    break;

  case GPIO_ModeOut_PP_5mA:
    R32_PA_PD_DRV &= ~pin;
    R32_PA_DIR |= pin;
    break;

  case GPIO_ModeOut_PP_20mA:
    R32_PA_PD_DRV |= pin;
    R32_PA_DIR |= pin;
    break;

  default:
    break;
  }
}

/*********************************************************************
 * @fn      GPIOB_ModeCfg
 *
 * @brief   GPIOB端口引脚模式配置
 *
 * @param   pin     - PB0-PB23
 * @param   mode    - 输入输出类型
 *
 * @return  none
 */
void GPIOB_ModeCfg(uint32_t pin, GPIOModeTypeDef mode) {
  switch (mode) {
  case GPIO_ModeIN_Floating:
    R32_PB_PD_DRV &= ~pin;
    R32_PB_PU &= ~pin;
    R32_PB_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PU:
    R32_PB_PD_DRV &= ~pin;
    R32_PB_PU |= pin;
    R32_PB_DIR &= ~pin;
    break;

  case GPIO_ModeIN_PD:
    R32_PB_PD_DRV |= pin;
    R32_PB_PU &= ~pin;
    R32_PB_DIR &= ~pin;
    break;

  case GPIO_ModeOut_PP_5mA:
    R32_PB_PD_DRV &= ~pin;
    R32_PB_DIR |= pin;
    break;

  case GPIO_ModeOut_PP_20mA:
    R32_PB_PD_DRV |= pin;
    R32_PB_DIR |= pin;
    break;

  default:
    break;
  }
}

/*********************************************************************
 * @fn      GPIOA_ITModeCfg
 *
 * @brief   GPIOA引脚中断模式配置
 *
 * @param   pin     - PA0-PA15
 * @param   mode    - 触发类型
 *
 * @return  none
 */
void GPIOA_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode) {
  switch (mode) {
  case GPIO_ITMode_LowLevel: // 低电平触发
    R16_PA_INT_MODE &= ~pin;
    R32_PA_CLR |= pin;
    break;

  case GPIO_ITMode_HighLevel: // 高电平触发
    R16_PA_INT_MODE &= ~pin;
    R32_PA_OUT |= pin;
    break;

  case GPIO_ITMode_FallEdge: // 下降沿触发
    R16_PA_INT_MODE |= pin;
    R32_PA_CLR |= pin;
    break;

  case GPIO_ITMode_RiseEdge: // 上升沿触发
    R16_PA_INT_MODE |= pin;
    R32_PA_OUT |= pin;
    break;

  default:
    break;
  }
  R16_PA_INT_IF = pin;
  R16_PA_INT_EN |= pin;
}

/*********************************************************************
 * @fn      GPIOB_ITModeCfg
 *
 * @brief   GPIOB引脚中断模式配置
 *
 * @param   pin     - PB0-PB23
 * @param   mode    - 触发类型
 *
 * @return  none
 */
void GPIOB_ITModeCfg(uint32_t pin, GPIOITModeTpDef mode) {
  uint32_t Pin = pin | ((pin & (GPIO_Pin_22 | GPIO_Pin_23)) >> 14);
  switch (mode) {
  case GPIO_ITMode_LowLevel: // 低电平触发
    R16_PB_INT_MODE &= ~Pin;
    R32_PB_CLR |= pin;
    break;

  case GPIO_ITMode_HighLevel: // 高电平触发
    R16_PB_INT_MODE &= ~Pin;
    R32_PB_OUT |= pin;
    break;

  case GPIO_ITMode_FallEdge: // 下降沿触发
    R16_PB_INT_MODE |= Pin;
    R32_PB_CLR |= pin;
    break;

  case GPIO_ITMode_RiseEdge: // 上升沿触发
    R16_PB_INT_MODE |= Pin;
    R32_PB_OUT |= pin;
    break;

  default:
    break;
  }
  R16_PB_INT_IF = Pin;
  R16_PB_INT_EN |= Pin;
}

/*********************************************************************
 * @fn      GPIOPinRemap
 *
 * @brief   外设功能引脚映射
 *
 * @param   s       - 是否使能映射
 * @param   perph   - RB_PIN_SPI0   -  SPI0:  PA12/PA13/PA14/PA15 -> PB12/PB13/PB14/PB15
 *                    RB_PIN_UART1  -  UART1: PA8/PA9 ->  PB12/PB13
 *                    RB_PIN_UART0  -  UART0: PB4/PB7 ->  PA15/PA14
 *                    RB_PIN_TMR2   -  TMR2:  PA11 ->  PB11
 *                    RB_PIN_TMR1   -  TMR1:  PA10 ->  PB10
 *                    RB_PIN_TMR0   -  TMR0:  PA9 ->  PB23
 *
 * @return  none
 */
void GPIOPinRemap(FunctionalState s, uint16_t perph) {
  if (s) {
    R16_PIN_ALTERNATE |= perph;
  } else {
    R16_PIN_ALTERNATE &= ~perph;
  }
}

/*********************************************************************
 * @fn      GPIOAGPPCfg
 *
 * @brief   模拟外设GPIO引脚功能控制
 *
 * @param   s       -   ENABLE  - 打开模拟外设功能，关闭数字功能
 *                      DISABLE - 启用数字功能，关闭模拟外设功能
 * @param   perph   -   RB_PIN_ADC0_1_IE    -  ADC0-1通道
 *                      RB_PIN_ADC2_3_IE    -  ADC2-3通道
 *                      RB_PIN_ADC4_5_IE    -  ADC4-5通道
 *                      RB_PIN_ADC6_7_IE    -  ADC6-7通道
 *                      RB_PIN_ADC8_9_IE    -  ADC8-9通道
 *                      RB_PIN_ADC10_11_IE  -  ADC10-11通道
 *                      RB_PIN_ADC12_13_IE  -  ADC12-13通道
 *                      RB_PIN_XT32K_IE     -  外部32K引脚
 *                      RB_PIN_USB_IE       -  USB功能信号引脚
 *                      RB_PIN_ETH_IE       -  以太网功能信号引脚
 *                      RB_PIN_SEG0_3_IE    -  LCD控制器SEG0-3驱动引脚
 *                      RB_PIN_SEG4_7_IE    -  LCD控制器SEG4-7驱动引脚
 *                      RB_PIN_SEG8_11_IE   -  LCD控制器SEG8-11驱动引脚
 *                      RB_PIN_SEG12_15_IE  -  LCD控制器SEG12-15驱动引脚
 *                      RB_PIN_SEG16_19_IE  -  LCD控制器SEG16-19驱动引脚
 *                      RB_PIN_SEG20_23_IE  -  LCD控制器SEG20-23驱动引脚
 *
 * @return  none
 */
void GPIOAGPPCfg(FunctionalState s, uint16_t perph) {
  if (s) {
    R16_PIN_ANALOG_IE |= perph;
  } else {
    R16_PIN_ANALOG_IE &= ~perph;
  }
}
