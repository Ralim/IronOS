/**
 * *****************************************************************************
 * @file hal_gpio.h
 * @version 0.1
 * @date 2021-03-01
 * @brief
 * *****************************************************************************
 * @attention
 *
 *  <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *    3. Neither the name of Bouffalo Lab nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 */
#ifndef __HAL_GPIO__H__
#define __HAL_GPIO__H__

#include "hal_common.h"
#include "drv_device.h"
#include "bl602_config.h"

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_16,
    GPIO_PIN_17,
    GPIO_PIN_18,
    GPIO_PIN_19,
    GPIO_PIN_20,
    GPIO_PIN_21,
    GPIO_PIN_22,
    GPIO_PIN_23,
    GPIO_PIN_24,
    GPIO_PIN_25,
    GPIO_PIN_26,
    GPIO_PIN_27,
    GPIO_PIN_28,
    GPIO_PIN_MAX,
} gpio_pin_type;

#define GPIO_OUTPUT_MODE                   0
#define GPIO_OUTPUT_PP_MODE                1
#define GPIO_OUTPUT_PD_MODE                2
#define GPIO_INPUT_MODE                    3
#define GPIO_INPUT_PP_MODE                 4
#define GPIO_INPUT_PD_MODE                 5
#define GPIO_ASYNC_RISING_TRIGER_INT_MODE  6
#define GPIO_ASYNC_FALLING_TRIGER_INT_MODE 7
#define GPIO_ASYNC_HIGH_LEVEL_INT_MODE     8
#define GPIO_ASYNC_LOW_LEVEL_INT_MODE      9
#define GPIO_SYNC_RISING_TRIGER_INT_MODE   10
#define GPIO_SYNC_FALLING_TRIGER_INT_MODE  11
#define GPIO_SYNC_HIGH_LEVEL_INT_MODE      12
#define GPIO_SYNC_LOW_LEVEL_INT_MODE       13

void gpio_set_mode(uint32_t pin, uint32_t mode);
void gpio_write(uint32_t pin, uint32_t value);
void gpio_toggle(uint32_t pin);
int gpio_read(uint32_t pin);
void gpio_attach_irq(uint32_t pin, void (*cbFun)(void));
void gpio_irq_enable(uint32_t pin, uint8_t enabled);
#endif