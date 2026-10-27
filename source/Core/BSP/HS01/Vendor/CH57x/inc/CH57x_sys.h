/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_SYS.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_SYS_H__
#define __CH57x_SYS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  RST_STATUS_SW = 0, // 软件复位
  RST_STATUS_RPOR,   // 上电复位
  RST_STATUS_WTR,    // 看门狗超时复位
  RST_STATUS_MR,     // 外部手动复位
  RST_STATUS_LRM0,   // 唤醒复位-软复位引起
  RST_STATUS_GPWSM,  // 下电模式唤醒复位
  RST_STATUS_LRM1,   //	唤醒复位-看门狗引起
  RST_STATUS_LRM2,   //	唤醒复位-手动复位引起

} SYS_ResetStaTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  INFO_ROM_READ = 0, // FlashROM 代码和数据区 是否可读
  INFO_RESET_EN = 2, // RST#外部手动复位输入功能是否开启
  INFO_BOOT_EN,      // 系统引导程序 BootLoader 是否开启
  INFO_DEBUG_EN,     // 系统仿真调试接口是否开启
  INFO_LOADER,       // 当前系统是否处于Bootloader 区
  STA_SAFEACC_ACT,   // 当前系统是否处于安全访问状态，否则RWA属性区域不可访问

} SYS_InfoStaTypeDef;

/**
 * @brief  获取芯片ID类，一般为固定值
 */
#define SYS_GetChipID() R8_CHIP_ID

/**
 * @brief  获取安全访问ID，一般为固定值
 */
#define SYS_GetAccessID() R8_SAFE_ACCESS_ID

/**
 * @brief   配置系统运行时钟
 *
 * @param   sc      - 系统时钟源选择 refer to SYS_CLKTypeDef
 */
void SetSysClock(SYS_CLKTypeDef sc);

/**
 * @brief   获取当前系统时钟
 *
 * @return  Hz
 */
uint32_t GetSysClock(void);

/**
 * @brief   获取当前系统信息状态
 *
 * @param   i       - refer to SYS_InfoStaTypeDef
 *
 * @return  是否开启
 */
uint8_t SYS_GetInfoSta(SYS_InfoStaTypeDef i);

/**
 * @brief   获取系统上次复位状态
 *
 * @return  refer to SYS_ResetStaTypeDef
 */
#define SYS_GetLastResetSta() (R8_RESET_STATUS & RB_RESET_FLAG)

/**
 * @brief   执行系统软件复位
 */
void SYS_ResetExecute(void);

/**
 * @brief   设置复位保存寄存器的值，不受手动复位、 软件复位、 看门狗复位或者普通唤醒复位的影响
 *
 * @param   i       - refer to SYS_InfoStaTypeDef
 */
#define SYS_ResetKeepBuf(d) (R8_GLOB_RESET_KEEP = d)

/**
 * @brief   关闭所有中断，并保留当前中断值
 *
 * @param   pirqv   - 当前保留中断值
 */
void SYS_DisableAllIrq(uint32_t *pirqv);

/**
 * @brief   恢复之前关闭的中断值
 *
 * @param   irq_status  - 当前保留中断值
 */
void SYS_RecoverIrq(uint32_t irq_status);

/**
 * @brief   获取当前系统(SYSTICK)计数值
 *
 * @return  当前计数值
 */
uint32_t SYS_GetSysTickCnt(void);

/**
 * @brief   加载看门狗计数初值，递增型
 *
 * @param   c       - 看门狗计数初值
 */
#define WWDG_SetCounter(c) (R8_WDOG_COUNT = c)

/**
 * @brief   看门狗定时器溢出中断使能
 *
 * @param   s       - 溢出是否中断
 */
void WWDG_ITCfg(FunctionalState s);

/**
 * @brief   看门狗定时器复位功能
 *
 * @param   s       - 溢出是否复位
 */
void WWDG_ResetCfg(FunctionalState s);

/**
 * @brief   获取当前看门狗定时器溢出标志
 *
 * @return  看门狗定时器溢出标志
 */
#define WWDG_GetFlowFlag() (R8_RST_WDOG_CTRL & RB_WDOG_INT_FLAG)

/**
 * @brief   清除看门狗中断标志，重新加载计数值也可清除
 */
void WWDG_ClearFlag(void);

/**
 * @brief   uS 延时
 *
 * @param   t       - 时间参数
 */
void mDelayuS(uint16_t t);

/**
 * @brief   mS 延时
 *
 * @param   t       - 时间参数
 */
void mDelaymS(uint16_t t);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_SYS_H__
