/**
 * *****************************************************************************
 * @file hal_gpio.c
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
#include "bl602_glb.h"
#include "bl602_gpio.h"
#include "hal_gpio.h"

/**
 * @brief
 *
 * @param pin
 * @param mode
 */
void gpio_set_mode(uint32_t pin, uint32_t mode)
{
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.gpioFun = GPIO_FUN_GPIO;
    gpio_cfg.gpioPin = pin;
    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;

    switch (mode) {
        case GPIO_OUTPUT_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_NONE;
            break;

        case GPIO_OUTPUT_PP_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_UP;
            break;

        case GPIO_OUTPUT_PD_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_DOWN;
            break;

        case GPIO_INPUT_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_NONE;
            break;

        case GPIO_INPUT_PP_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_UP;
            break;

        case GPIO_INPUT_PD_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_DOWN;
            break;

        default:
            CPU_Interrupt_Disable(GPIO_INT0_IRQn);
            GLB_GPIO_IntMask(pin, MASK);

            gpio_cfg.gpioMode = GPIO_MODE_INPUT;

            GLB_GPIO_INT0_IRQHandler_Install();

            if (mode == GPIO_ASYNC_RISING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_PULSE);
            }

            else if (mode == GPIO_ASYNC_FALLING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_PULSE);
            }

            else if (mode == GPIO_ASYNC_HIGH_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_LEVEL);
            }

            else if (mode == GPIO_ASYNC_LOW_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_LEVEL);
            }

            else if (mode == GPIO_SYNC_RISING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_POS_PULSE);
            }

            else if (mode == GPIO_SYNC_FALLING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_NEG_PULSE);
            }

            else if (mode == GPIO_SYNC_HIGH_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_POS_LEVEL);
            }

            else if (mode == GPIO_SYNC_LOW_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_NEG_LEVEL);
            }

            else {
                return;
            }

            CPU_Interrupt_Enable(GPIO_INT0_IRQn);
            break;
    }

    GLB_GPIO_Init(&gpio_cfg);
}
/**
 * @brief
 *
 * @param pin
 * @param value
 */
void gpio_write(uint32_t pin, uint32_t value)
{
    GLB_GPIO_Write(pin, value);
}
/**
 * @brief
 *
 * @param pin
 */
void gpio_toggle(uint32_t pin)
{
}
/**
 * @brief
 *
 * @param pin
 * @return int
 */
int gpio_read(uint32_t pin)
{
    return GLB_GPIO_Read(pin);
}
/**
 * @brief
 *
 * @param pin
 * @param cbFun
 */
void gpio_attach_irq(uint32_t pin, void (*cbFun)(void))
{
    GLB_GPIO_INT0_Callback_Install(pin, cbFun);
}
/**
 * @brief
 *
 * @param pin
 * @param enabled
 */
void gpio_irq_enable(uint32_t pin, uint8_t enabled)
{
    if (enabled) {
        GLB_GPIO_IntMask(pin, UNMASK);
    } else {
        GLB_GPIO_IntMask(pin, MASK);
    }
}