/**
 * @file pinmux_config.h
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
#ifndef _PINMUX_CONFIG_H
#define _PINMUX_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <q> GPIO0 <2> [GPIO_FUN_UNUSED//GPIO_FUN_I2S//GPIO_FUN_SPI//GPIO_FUN_I2C//GPIO_FUN_PWM//GPIO_FUN_CAM//GPIO_FUN_UART0_RTS//GPIO_FUN_UART1_RTS//GPIO_FUN_ETHER_MAC//GPIO_FUN_QDEC]
// <i> config gpio0 function
#define CONFIG_GPIO0_FUNC GPIO_FUN_E21_JTAG // jtms

// <q> GPIO1 <2> [GPIO_FUN_UNUSED//GPIO_FUN_I2S//GPIO_FUN_SPI//GPIO_FUN_I2C//GPIO_FUN_PWM//GPIO_FUN_CAM//GPIO_FUN_UART0_CTS//GPIO_FUN_UART1_CTS//GPIO_FUN_ETHER_MAC//GPIO_FUN_QDEC]
// <i> config gpio1 function
#define CONFIG_GPIO1_FUNC GPIO_FUN_E21_JTAG // jtdi

// <q> GPIO2 <2> [GPIO_FUN_UNUSED//GPIO_FUN_I2S//GPIO_FUN_SPI//GPIO_FUN_I2C//GPIO_FUN_PWM//GPIO_FUN_CAM//GPIO_FUN_UART0_TX//GPIO_FUN_UART1_TX//GPIO_FUN_ETHER_MAC//GPIO_FUN_QDEC]
// <i> config gpio2 function
#define CONFIG_GPIO2_FUNC  GPIO_FUN_E21_JTAG         // jtck
#define CONFIG_GPIO3_FUNC  GPIO_FUN_GPIO_OUTPUT_NONE // Oled reset
#define CONFIG_GPIO4_FUNC  GPIO_FUN_UNUSED           // QC D- 1
#define CONFIG_GPIO5_FUNC  GPIO_FUN_UNUSED           // QC D+
#define CONFIG_GPIO6_FUNC  GPIO_FUN_UNUSED           // QC D- 0
#define CONFIG_GPIO7_FUNC  GPIO_FUN_USB
#define CONFIG_GPIO8_FUNC  GPIO_FUN_USB
#define CONFIG_GPIO9_FUNC  GPIO_FUN_E21_JTAG               // JTDO
#define CONFIG_GPIO10_FUNC GPIO_FUN_I2C                    // SDA
#define CONFIG_GPIO11_FUNC GPIO_FUN_I2C                    // SCL
#define CONFIG_GPIO12_FUNC GPIO_FUN_UNUSED                 // cc1
#define CONFIG_GPIO14_FUNC GPIO_FUN_UNUSED                 // cc2
#define CONFIG_GPIO15_FUNC GPIO_FUN_SPI                    // spi sck
#define CONFIG_GPIO16_FUNC GPIO_FUN_GPIO_EXTI_FALLING_EDGE // FUSB302 IRQ
#define CONFIG_GPIO17_FUNC GPIO_FUN_UNUSED
#define CONFIG_GPIO18_FUNC GPIO_FUN_ADC             // Vin
#define CONFIG_GPIO19_FUNC GPIO_FUN_ADC             // Tip Sense
#define CONFIG_GPIO20_FUNC GPIO_FUN_ADC             // Temp Sensor
#define CONFIG_GPIO21_FUNC GPIO_FUN_PWM             // Tip Control PWM output
#define CONFIG_GPIO22_FUNC GPIO_FUN_UART1_TX        //
#define CONFIG_GPIO23_FUNC GPIO_FUN_UART1_RX        //
#define CONFIG_GPIO24_FUNC GPIO_FUN_UNUSED          //
#define CONFIG_GPIO25_FUNC GPIO_FUN_GPIO_INPUT_DOWN // Button B
#define CONFIG_GPIO26_FUNC GPIO_FUN_UNUSED          // CC1
#define CONFIG_GPIO27_FUNC GPIO_FUN_UNUSED          // CC2
#define CONFIG_GPIO28_FUNC GPIO_FUN_GPIO_INPUT_DOWN // Butt A
#define CONFIG_GPIO29_FUNC GPIO_FUN_UNUSED          // Mag alert
#define CONFIG_GPIO30_FUNC GPIO_FUN_UNUSED          // G int 1
#define CONFIG_GPIO31_FUNC GPIO_FUN_UNUSED          // G int 2

#endif
