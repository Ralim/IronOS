/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_common.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_COMM_H__
#define __CH57x_COMM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif
#define ALL 0xFFFF

#define Debug_UART0 0
#define Debug_UART1 1
#define Debug_UART2 2
#define Debug_UART3 3

#ifdef DEBUG
#include <stdio.h>
#endif

/**
 * @brief  系统主频时钟（Hz）
 */
#ifndef FREQ_SYS
#define FREQ_SYS 60000000
#endif

/**
 * @brief  32K时钟（Hz）
 */
#ifdef CLK_OSC32K
#if (CLK_OSC32K == 1)
#define CAB_LSIFQ 32000
#else
#define CAB_LSIFQ 32768
#endif
#else
#define CAB_LSIFQ 32000
#endif

// clang-format off: CH573SFR.h must come before the peripheral headers
#include <string.h>
#include <stdint.h>
#include "CH573SFR.h"
#include "core_riscv.h"
#include "CH57x_clk.h"
#include "CH57x_uart.h"
#include "CH57x_gpio.h"
#include "CH57x_flash.h"
#include "CH57x_pwr.h"
#include "CH57x_pwm.h"
#include "CH57x_adc.h"
#include "CH57x_sys.h"
#include "CH57x_timer.h"
#include "CH57x_spi.h"
#include "CH57x_usbdev.h"
#include "CH57x_usbhost.h"
#include "ISP573.h"
// clang-format on

#define DelayMs(x) mDelaymS(x)
#define DelayUs(x) mDelayuS(x)

#ifdef __cplusplus
}
#endif

#endif // __CH57x_COMM_H__
