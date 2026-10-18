/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_adc.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_ADC_H__
#define __CH57x_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_CFG_TMP_25C 0x7F014

/**
 * @brief  adc single channel define
 */
typedef enum {
  CH_EXTIN_0 = 0,   // ADC 外部模拟通道 0
  CH_EXTIN_1,       // ADC 外部模拟通道 1
  CH_EXTIN_2,       // ADC 外部模拟通道 2
  CH_EXTIN_3,       // ADC 外部模拟通道 3
  CH_EXTIN_4,       // ADC 外部模拟通道 4
  CH_EXTIN_5,       // ADC 外部模拟通道 5
  CH_EXTIN_8 = 8,   // ADC 外部模拟通道 8
  CH_EXTIN_9,       // ADC 外部模拟通道 9
  CH_EXTIN_12 = 12, // ADC 外部模拟通道 12
  CH_EXTIN_13,      // ADC 外部模拟通道 13

  CH_INTE_VBAT  = 14, // ADC 内部电池检测通道
  CH_INTE_VTEMP = 15, // ADC 内部温度传感器检测通道

} ADC_SingleChannelTypeDef;

/**
 * @brief  adc differential channel define
 */
typedef enum {
  CH_DIFF_0_2 = 0, // ADC 差分通道 #0-#2
  CH_DIFF_1_3,     // ADC 差分通道 #1-#3

} ADC_DiffChannelTypeDef;

/**
 * @brief  adc sampling clock
 */
typedef enum {
  SampleFreq_3_2 = 0, // 3.2M 采样频率
  SampleFreq_8,       // 8M 采样频率
  SampleFreq_5_33,    // 5.33M 采样频率
  SampleFreq_4,       // 4M 采样频率
} ADC_SampClkTypeDef;

/**
 * @brief  adc signal PGA
 */
typedef enum {
  ADC_PGA_1_4 = 0, // -12dB, 1/4倍
  ADC_PGA_1_2,     // -6dB, 1/2倍
  ADC_PGA_0,       // 0dB, 1倍，无增益
  ADC_PGA_2,       // 6dB, 2倍
} ADC_SignalPGATypeDef;

/**
 * @brief  Configuration DMA mode
 */
typedef enum {
  ADC_Mode_Single = 0, // 单次模式
  ADC_Mode_LOOP,       // 循环模式
} ADC_DMAModeTypeDef;

/**
 * @brief   设置 ADC 采样通道
 *
 * @param   d   - refer to ADC_SingleChannelTypeDef
 */
#define ADC_ChannelCfg(d) (R8_ADC_CHANNEL = d)

/**
 * @brief   设置 ADC 采样时钟
 *
 * @param   d   - refer to ADC_SampClkTypeDef
 */
#define ADC_SampClkCfg(d) (R8_ADC_CFG = R8_ADC_CFG & (~RB_ADC_CLK_DIV) | (d << 6))

/**
 * @brief   设置 ADC 信号增益
 *
 * @param   d   - refer to ADC_SignalPGATypeDef
 */
#define ADC_PGACfg(d) (R8_ADC_CFG = R8_ADC_CFG & (~RB_ADC_PGA_GAIN) | (d << 4))

/**
 * @brief   设置内部温度传感器校准值
 *
 * @param   d   - 校准值
 */
#define ADC_TempCalibCfg(d) (R8_TEM_SENSOR = R8_TEM_SENSOR & (~RB_TEM_SEN_CALIB) | d)

/**
 * @brief   外部信号单通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 */
void ADC_ExtSingleChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

/**
 * @brief   外部信号差分通道采样初始化
 *
 * @param   sp  - refer to ADC_SampClkTypeDef
 * @param   ga  - refer to ADC_SignalPGATypeDef
 */
void ADC_ExtDiffChSampInit(ADC_SampClkTypeDef sp, ADC_SignalPGATypeDef ga);

/**
 * @brief   触摸按键通道采样初始化
 */
void TouchKey_ChSampInit(void);

/**
 * @brief   关闭TouchKey电源
 */
#define TouchKey_DisableTSPower() (R8_TKEY_CFG &= ~RB_TKEY_PWR_ON)

/**
 * @brief   内置温度传感器采样初始化
 */
void ADC_InterTSSampInit(void);

/**
 * @brief   关闭温度传感器电源
 */
#define ADC_DisableTSPower() (R8_TEM_SENSOR = 0)

/**
 * @brief   内置电池电压采样初始化
 */
void ADC_InterBATSampInit(void);

/**
 * @brief   ADC执行单次转换
 *
 * @return  ADC转换后的数据
 */
uint16_t ADC_ExcutSingleConver(void);

/**
 * @brief   采样数据粗调,获取偏差值,必须先配置ADC后调用此函数获取校准值
 *
 * @return  偏差
 */
signed short ADC_DataCalib_Rough(void);

/**
 * @brief   TouchKey转换后数据
 *
 * @param   charg   - Touchkey充电时间,5bits有效, t=charg*Tadc
 * @param   disch   - Touchkey放电时间,3bits有效, t=disch*Tadc
 *
 * @return  当前TouchKey等效数据
 */
uint16_t TouchKey_ExcutSingleConver(uint8_t charg, uint8_t disch);

/**
 * @brief   设置连续 ADC的周期
 *
 * @param   cycle   - 单位为 16个系统时钟
 */
void ADC_AutoConverCycle(uint8_t cycle);

/**
 * @brief   配置DMA功能
 *
 * @param   s           - 是否打开DMA功能
 * @param   startAddr   - DMA 起始地址
 * @param   endAddr     - DMA 结束地址
 * @param   m           - 配置DMA模式
 */
void ADC_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, ADC_DMAModeTypeDef m);

/**
 * @brief   获取当前采样的温度值（℃）
 *
 * @param   ts_v    - 当前温度传感器采样输出
 *
 * @return  转换后的温度值（℃）
 */
int ADC_GetCurrentTS(uint16_t ts_v);

/**
 * @brief   获取ADC转换值
 *
 * @return  ADC转换值
 */
#define ADC_ReadConverValue() (R16_ADC_DATA)

/**
 * @brief   ADC执行单次转换
 */
#define ADC_StartUp() (R8_ADC_CONVERT = RB_ADC_START)

/**
 * @brief   获取ADC中断状态
 */
#define ADC_GetITStatus() (R8_ADC_INT_FLAG & RB_ADC_IF_EOC)

/**
 * @brief   清除ADC中断标志
 */
#define ADC_ClearITFlag() (R8_ADC_CONVERT = 0)

/**
 * @brief   获取ADC DMA完成状态
 */
#define ADC_GetDMAStatus() (R8_ADC_DMA_IF & RB_ADC_IF_DMA_END)

/**
 * @brief   清除ADC DMA完成标志
 */
#define ADC_ClearDMAFlag() (R8_ADC_DMA_IF |= RB_ADC_IF_DMA_END)

/**
 * @brief   开启自动连续 ADC
 */
#define ADC_StartDMA() (R8_ADC_CTRL_DMA |= RB_ADC_AUTO_EN)

/**
 * @brief   停止自动连续 ADC
 */
#define ADC_StopDMA() (R8_ADC_CTRL_DMA &= ~RB_ADC_AUTO_EN)

/**
 * @brief   获取TouchKey中断状态
 */
#define TouchKey_GetITStatus() (R8_ADC_INT_FLAG & RB_ADC_IF_EOC)

/**
 * @brief   清除TouchKey中断标志
 */
#define TouchKey_ClearITFlag() (R8_TKEY_CTRL |= RB_TKEY_PWR_ON)

#ifdef __cplusplus
}
#endif

#endif // __CH57x_ADC_H__
