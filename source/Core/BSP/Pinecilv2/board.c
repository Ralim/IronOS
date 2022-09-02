/**
 * @file board.c
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

#include "bflb_platform.h"
#include "bl702_config.h"
#include "bl702_glb.h"
#include "hal_gpio.h"

struct pin_mux_cfg {
  uint8_t  pin;
  uint16_t func;
};

static const struct pin_mux_cfg af_pin_table[] = {
#ifdef CONFIG_GPIO0_FUNC
    {.pin = GPIO_PIN_0, .func = CONFIG_GPIO0_FUNC},
#endif
#ifdef CONFIG_GPIO1_FUNC
    {.pin = GPIO_PIN_1, .func = CONFIG_GPIO1_FUNC},
#endif
#ifdef CONFIG_GPIO2_FUNC
    {.pin = GPIO_PIN_2, .func = CONFIG_GPIO2_FUNC},
#endif
#ifdef CONFIG_GPIO3_FUNC
    {.pin = GPIO_PIN_3, .func = CONFIG_GPIO3_FUNC},
#endif
#ifdef CONFIG_GPIO4_FUNC
    {.pin = GPIO_PIN_4, .func = CONFIG_GPIO4_FUNC},
#endif
#ifdef CONFIG_GPIO5_FUNC
    {.pin = GPIO_PIN_5, .func = CONFIG_GPIO5_FUNC},
#endif
#ifdef CONFIG_GPIO6_FUNC
    {.pin = GPIO_PIN_6, .func = CONFIG_GPIO6_FUNC},
#endif
#ifdef CONFIG_GPIO7_FUNC
    {.pin = GPIO_PIN_7, .func = CONFIG_GPIO7_FUNC},
#endif
#ifdef CONFIG_GPIO8_FUNC
    {.pin = GPIO_PIN_8, .func = CONFIG_GPIO8_FUNC},
#endif
#ifdef CONFIG_GPIO9_FUNC
    {.pin = GPIO_PIN_9, .func = CONFIG_GPIO9_FUNC},
#endif
#ifdef CONFIG_GPIO10_FUNC
    {.pin = GPIO_PIN_10, .func = CONFIG_GPIO10_FUNC},
#endif
#ifdef CONFIG_GPIO11_FUNC
    {.pin = GPIO_PIN_11, .func = CONFIG_GPIO11_FUNC},
#endif
#ifdef CONFIG_GPIO12_FUNC
    {.pin = GPIO_PIN_12, .func = CONFIG_GPIO12_FUNC},
#endif
#ifdef CONFIG_GPIO13_FUNC
    {.pin = GPIO_PIN_13, .func = CONFIG_GPIO13_FUNC},
#endif
#ifdef CONFIG_GPIO14_FUNC
    {.pin = GPIO_PIN_14, .func = CONFIG_GPIO14_FUNC},
#endif
#ifdef CONFIG_GPIO15_FUNC
    {.pin = GPIO_PIN_15, .func = CONFIG_GPIO15_FUNC},
#endif
#ifdef CONFIG_GPIO16_FUNC
    {.pin = GPIO_PIN_16, .func = CONFIG_GPIO16_FUNC},
#endif
#ifdef CONFIG_GPIO17_FUNC
    {.pin = GPIO_PIN_17, .func = CONFIG_GPIO17_FUNC},
#endif
#ifdef CONFIG_GPIO18_FUNC
    {.pin = GPIO_PIN_18, .func = CONFIG_GPIO18_FUNC},
#endif
#ifdef CONFIG_GPIO19_FUNC
    {.pin = GPIO_PIN_19, .func = CONFIG_GPIO19_FUNC},
#endif
#ifdef CONFIG_GPIO20_FUNC
    {.pin = GPIO_PIN_20, .func = CONFIG_GPIO20_FUNC},
#endif
#ifdef CONFIG_GPIO21_FUNC
    {.pin = GPIO_PIN_21, .func = CONFIG_GPIO21_FUNC},
#endif
#ifdef CONFIG_GPIO22_FUNC
    {.pin = GPIO_PIN_22, .func = CONFIG_GPIO22_FUNC},
#endif
#ifdef CONFIG_GPIO23_FUNC
    {.pin = GPIO_PIN_23, .func = CONFIG_GPIO23_FUNC},
#endif
#ifdef CONFIG_GPIO24_FUNC
    {.pin = GPIO_PIN_24, .func = CONFIG_GPIO24_FUNC},
#endif
#ifdef CONFIG_GPIO25_FUNC
    {.pin = GPIO_PIN_25, .func = CONFIG_GPIO25_FUNC},
#endif
#ifdef CONFIG_GPIO26_FUNC
    {.pin = GPIO_PIN_26, .func = CONFIG_GPIO26_FUNC},
#endif
#ifdef CONFIG_GPIO27_FUNC
    {.pin = GPIO_PIN_27, .func = CONFIG_GPIO27_FUNC},
#endif
#ifdef CONFIG_GPIO28_FUNC
    {.pin = GPIO_PIN_28, .func = CONFIG_GPIO28_FUNC},
#endif
#ifdef CONFIG_GPIO29_FUNC
    {.pin = GPIO_PIN_29, .func = CONFIG_GPIO29_FUNC},
#endif
#ifdef CONFIG_GPIO30_FUNC
    {.pin = GPIO_PIN_30, .func = CONFIG_GPIO30_FUNC},
#endif
#ifdef CONFIG_GPIO31_FUNC
    {.pin = GPIO_PIN_31, .func = CONFIG_GPIO31_FUNC},
#endif
};

static void board_pin_mux_init(void) {
  GLB_GPIO_Cfg_Type gpio_cfg;
  uint32_t          tmpVal;
  gpio_cfg.drive        = 3;
  gpio_cfg.smtCtrl      = 1;
  uint8_t hbn_gpio_mask = 0x1f;
  uint8_t hbn_aon_ie    = 0;

  for (uint32_t i = 0; i < sizeof(af_pin_table) / sizeof(af_pin_table[0]); i++) {
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioPin  = af_pin_table[i].pin;
    gpio_cfg.gpioFun  = af_pin_table[i].func;

    /*if using gpio9-gpio12 and func is not analog and output ,should set reg_aon_pad_ie_smt corresponding bit = 1*/
    if ((af_pin_table[i].pin > GPIO_PIN_8) && (af_pin_table[i].pin < GPIO_PIN_13)) {
      if ((af_pin_table[i].func != 10) && ((af_pin_table[i].func < GPIO_FUN_GPIO_OUTPUT_UP) || (af_pin_table[i].func > GPIO_FUN_GPIO_OUTPUT_NONE)))
        hbn_aon_ie |= (1 << (af_pin_table[i].pin - 9));
    }

    /*if reset state*/
    if (af_pin_table[i].func == GPIO_FUN_UNUSED) {
      continue;
    } else if (af_pin_table[i].func == GPIO_FUN_WAKEUP) {
      /*if hbn or pds gpio wakeup func*/
      if (af_pin_table[i].pin < GPIO_PIN_8) {
        /*enable pds gpio wakeup and irq unmask*/
        tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_INT);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_INT_SELECT, af_pin_table[i].pin);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_GPIO_INT_MODE, PDS_AON_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE);
        tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_GPIO_INT_MASK);
        BL_WR_REG(PDS_BASE, PDS_GPIO_INT, tmpVal);
      } else if ((af_pin_table[i].pin > GPIO_PIN_8) && (af_pin_table[i].pin < GPIO_PIN_13)) {
        hbn_gpio_mask &= ~(1 << (af_pin_table[i].pin - 9));
      }
      continue;
    } else if ((af_pin_table[i].func == GPIO_FUN_USB) || (af_pin_table[i].func == GPIO_FUN_DAC) || (af_pin_table[i].func == GPIO_FUN_ADC)) {
      /*if analog func , for usb、adc、dac*/
      gpio_cfg.gpioFun  = GPIO_FUN_ANALOG;
      gpio_cfg.gpioMode = GPIO_MODE_ANALOG;
      gpio_cfg.pullType = GPIO_PULL_NONE;
    } else if ((af_pin_table[i].func & 0xF0) == 0xF0) {
      /*if uart func*/
      gpio_cfg.gpioFun  = GPIO_FUN_UART;
      uint8_t uart_func = af_pin_table[i].func & 0x07;
      uint8_t uart_sig  = gpio_cfg.gpioPin % 8;
      /*link to one uart sig*/
      GLB_UART_Fun_Sel((GLB_UART_SIG_Type)uart_sig, (GLB_UART_SIG_FUN_Type)uart_func);
      GLB_UART_Fun_Sel((GLB_UART_SIG_Type)uart_func, (GLB_UART_SIG_FUN_Type)uart_sig);
    } else if (af_pin_table[i].func == GPIO_FUN_PWM) {
      /*if pwm func*/
      gpio_cfg.pullType = GPIO_PULL_DOWN;
    } else if (af_pin_table[i].func == GPIO_FUN_QDEC) {
      /* if qdec a/b */
      gpio_cfg.pullType = GPIO_PULL_NONE;
      gpio_cfg.gpioMode = GPIO_MODE_INPUT;
      gpio_cfg.gpioFun  = GPIO_FUN_QDEC;
    } else if (af_pin_table[i].func == GPIO_FUN_QDEC_LED) {
      /* if qdec led */
      gpio_cfg.pullType = GPIO_PULL_NONE;
      gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
      gpio_cfg.gpioFun  = GPIO_FUN_QDEC;
    } else if (af_pin_table[i].func == GPIO_FUN_CLK_OUT) {
      if (af_pin_table[i].pin % 2) {
        /*odd gpio output clock*/
        GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
      } else {
        /*even gpio output clock*/
        GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
      }
    } else if ((af_pin_table[i].func == GPIO_FUN_GPIO_INPUT_UP) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_FALLING_EDGE) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_LOW_LEVEL)) {
      /*if common gpio func,include input、output and exti*/
      gpio_cfg.gpioFun  = GPIO_FUN_GPIO;
      gpio_cfg.gpioMode = GPIO_MODE_INPUT;
      gpio_cfg.pullType = GPIO_PULL_UP;

      if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_FALLING_EDGE) {
        GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_PULSE);
      } else if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_LOW_LEVEL) {
        GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_LEVEL);
      }
    } else if ((af_pin_table[i].func == GPIO_FUN_GPIO_INPUT_DOWN) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_RISING_EDGE) || (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_HIGH_LEVEL)) {
      gpio_cfg.gpioFun  = GPIO_FUN_GPIO;
      gpio_cfg.gpioMode = GPIO_MODE_INPUT;
      gpio_cfg.pullType = GPIO_PULL_DOWN;

      if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_RISING_EDGE) {
        GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_PULSE);
      } else if (af_pin_table[i].func == GPIO_FUN_GPIO_EXTI_HIGH_LEVEL) {
        GLB_Set_GPIO_IntMod(af_pin_table[i].pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_LEVEL);
      }
    } else if (af_pin_table[i].func == GPIO_FUN_GPIO_INPUT_NONE) {
      gpio_cfg.gpioFun  = GPIO_FUN_GPIO;
      gpio_cfg.gpioMode = GPIO_MODE_INPUT;
      gpio_cfg.pullType = GPIO_PULL_NONE;
    } else if (af_pin_table[i].func == GPIO_FUN_GPIO_OUTPUT_UP) {
      gpio_cfg.gpioFun  = GPIO_FUN_GPIO;
      gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
      gpio_cfg.pullType = GPIO_PULL_UP;
    } else if (af_pin_table[i].func == GPIO_FUN_GPIO_OUTPUT_DOWN) {
      gpio_cfg.gpioFun  = GPIO_FUN_GPIO;
      gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
      gpio_cfg.pullType = GPIO_PULL_DOWN;
    } else if (af_pin_table[i].func == GPIO_FUN_GPIO_OUTPUT_NONE) {
      gpio_cfg.gpioFun  = GPIO_FUN_GPIO;
      gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
      gpio_cfg.pullType = GPIO_PULL_NONE;
    }
    GLB_GPIO_Init(&gpio_cfg);
  }

  /*disable unused reg_aon_pad_ie_smt bits and hbn_pin_wakeup_mask bits*/
  tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, hbn_gpio_mask);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_PAD_IE_SMT, hbn_aon_ie);
  tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE);
  BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
}

static void board_clock_init(void) {
  system_clock_init();
  peripheral_clock_init();
}

void board_init(void) {
  board_clock_init();
  board_pin_mux_init();
}