/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_adc.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH57x_common.h"

/*********************************************************************
 * @fn      ADC_DataCalib_Rough
 *
 * @brief   采样数据粗调,获取偏差值,必须先配置ADC后调用此函数获取校准值
 *
 * @param   none
 *
 * @return  偏差
 */
signed short ADC_DataCalib_Rough(void) // 采样数据粗调,获取偏差值
{
  uint16_t i;
  uint32_t sum  = 0;
  uint8_t  ch   = 0; // 备份通道
  uint8_t  ctrl = 0; // 备份控制寄存器

  ch   = R8_ADC_CHANNEL;
  ctrl = R8_ADC_CFG;

  R8_ADC_CFG = 0;
  ADC_ChannelCfg(6);                                          // 6/7/10/11 可选
  R8_ADC_CFG |= RB_ADC_OFS_TEST | RB_ADC_POWER_ON | (2 << 4); // 进入测试模式
  R8_ADC_CONVERT = RB_ADC_START;
  while (R8_ADC_CONVERT & RB_ADC_START)
    ;
  for (i = 0; i < 16; i++) {
    R8_ADC_CONVERT = RB_ADC_START;
    while (R8_ADC_CONVERT & RB_ADC_START)
      ;
    sum += (~R16_ADC_DATA) & RB_ADC_DATA;
  }
  sum = (sum + 8) >> 4;
  R8_ADC_CFG &= ~RB_ADC_OFS_TEST; // 关闭测试模式

  R8_ADC_CHANNEL = ch;
  R8_ADC_CFG     = ctrl;
  return (2048 - sum);
}

/*********************************************************************
 * @fn      ADC_ExtSingleChSampInit
 *
 * @brief   外部信号单通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 *
 * @return  none
 */
void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (sp << 6) | (ga << 4);
}

/*********************************************************************
 * @fn      ADC_ExtDiffChSampInit
 *
 * @brief   外部信号差分通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 *
 * @return  none
 */
void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_DIFF_EN | (sp << 6) | (ga << 4);
}

/*********************************************************************
 * @fn      ADC_InterTSSampInit
 *
 * @brief   内置温度传感器采样初始化
 *
 * @param   none
 *
 * @return  none
 */
void ADC_InterTSSampInit(void) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_TEM_SENSOR  = RB_TEM_SEN_PWR_ON;
  R8_ADC_CHANNEL = CH_INTE_VTEMP;
  R8_ADC_CFG     = RB_ADC_BUF_EN | RB_ADC_POWER_ON | RB_ADC_DIFF_EN | (3 << 4);
}

/*********************************************************************
 * @fn      ADC_InterBATSampInit
 *
 * @brief   内置电池电压采样初始化
 *
 * @param   none
 *
 * @return  none
 */
void ADC_InterBATSampInit(void) {
  R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
  R8_ADC_CHANNEL = CH_INTE_VBAT;
  R8_ADC_CFG     = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (0 << 4); // 使用-12dB模式，
}

/*********************************************************************
 * @fn      TouchKey_ChSampInit
 *
 * @brief   触摸按键通道采样初始化
 *
 * @param   none
 *
 * @return  none
 */
void TouchKey_ChSampInit(void) {
  R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (2 << 4);
  R8_TKEY_CFG |= RB_TKEY_PWR_ON;
}

/*********************************************************************
 * @fn      ADC_ExcutSingleConver
 *
 * @brief   ADC执行单次转换
 *
 * @param   none
 *
 * @return  ADC转换后的数据
 */
uint16_t ADC_ExcutSingleConver(void) {
  R8_ADC_CONVERT = RB_ADC_START;
  while (R8_ADC_CONVERT & RB_ADC_START)
    ;

  return (R16_ADC_DATA & RB_ADC_DATA);
}

/*********************************************************************
 * @fn      TouchKey_ExcutSingleConver
 *
 * @brief   TouchKey转换后数据
 *
 * @param   charg   - Touchkey充电时间,5bits有效, t=charg*Tadc
 * @param   disch   - Touchkey放电时间,3bits有效, t=disch*Tadc
 *
 * @return  当前TouchKey等效数据
 */
uint16_t TouchKey_ExcutSingleConver(uint8_t charg, uint8_t disch) {
  R8_TKEY_COUNT   = (disch << 5) | (charg & 0x1f);
  R8_TKEY_CONVERT = RB_TKEY_START;
  while (R8_TKEY_CONVERT & RB_TKEY_START)
    ;
  return (R16_ADC_DATA & RB_ADC_DATA);
}

/*********************************************************************
 * @fn      ADC_AutoConverCycle
 *
 * @brief   设置连续 ADC的周期
 *
 * @param   cycle   - 采样周期计算方法为(256-cycle)*16*Tsys
 *
 * @return  none
 */
void ADC_AutoConverCycle(uint8_t cycle) { R8_ADC_AUTO_CYCLE = cycle; }

/*********************************************************************
 * @fn      ADC_DMACfg
 *
 * @brief   配置DMA功能
 *
 * @param   s           - 是否打开DMA功能
 * @param   startAddr   - DMA 起始地址
 * @param   endAddr     - DMA 结束地址
 * @param   m           - 配置DMA模式
 *
 * @return  none
 */
void ADC_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, ADC_DMAModeTypeDef m) {
  if (s == DISABLE) {
    R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_ENABLE;
  } else {
    R16_ADC_DMA_BEG = startAddr;
    R16_ADC_DMA_END = endAddr;
    if (m) {
      R8_ADC_CTRL_DMA |= RB_ADC_DMA_LOOP | RB_ADC_IE_DMA_END | RB_ADC_DMA_ENABLE;
    } else {
      R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_LOOP;
      R8_ADC_CTRL_DMA |= RB_ADC_IE_DMA_END | RB_ADC_DMA_ENABLE;
    }
  }
}

/*********************************************************************
 * @fn      ADC_GetCurrentTS
 *
 * @brief   获取当前采样的温度值（℃）
 *
 * @param   ts_v    - 当前温度传感器采样输出
 *
 * @return  转换后的温度值（℃）
 */
int ADC_GetCurrentTS(uint16_t ts_v) {
  uint32_t C25_Data[2];
  int      cal;

  FLASH_EEPROM_CMD(CMD_GET_ROM_INFO, ROM_CFG_TMP_25C, C25_Data, 0);
  cal = (ts_v * 2100) >> 12;
  cal = (((C25_Data[0] >> 16) & 0xFFFF) ? ((C25_Data[0] >> 16) & 0xFFFF) : 25) + ((cal - ((int)(C25_Data[0] & 0xFFFF) - 1050 / 2) * 2) * 10 / 14);
  return (cal);
}
