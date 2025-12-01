/*****************************************************************************
 * Copyright (c) 2022, Nsing Technologies Pte. Ltd.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nsing' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY Nsing "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL Nsing BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file main.c
 * @author Nsing
 * @version V1.2.2
 *
 * @copyright Copyright (c) 2022, Nsingechnologies Pte. Ltd. All rights reserved.
 */
#include "main.h"
#include <stdio.h>
#include <stdint.h>

/**
 * @brief  Inserts a delay time.
 * @param count specifies the delay time length.
 */
void Delay(uint32_t count)
{
    for (; count > 0; count--)
        ;
}
/**
 * @brief  Configures LED GPIO.
 * @param GPIOx x can be A to G to select the GPIO port.
 * @param Pin This parameter can be GPIO_PIN_0~GPIO_PIN_15.
 */
void LedInit(GPIO_Module *GPIOx, uint16_t Pin)
{
    GPIO_InitType GPIO_InitStructure;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
    }
    else
    {
        if (GPIOx == GPIOD)
        {
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
        }
    }

    /* Configure the GPIO pin */
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin = Pin;
        GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
        GPIO_InitStructure.GPIO_Pull = GPIO_No_Pull;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }
}
/**
 * @brief  Turns selected Led on as output low level.
 * @param GPIOx x can be A to G to select the GPIO port.
 * @param Pin This parameter can be GPIO_PIN_0~GPIO_PIN_15.
 */
void LedOn(GPIO_Module *GPIOx, uint16_t Pin)
{
    GPIOx->PBSC = Pin;
}
/**
 * @brief  Turns selected Led Off as output high level.
 * @param GPIOx x can be A to G to select the GPIO port.
 * @param Pin This parameter can be GPIO_PIN_0~GPIO_PIN_15.
 */
void LedOff(GPIO_Module *GPIOx, uint16_t Pin)
{
    GPIOx->PBC = Pin;
}
/**
 * @brief  Turns selected Led on or off.
 * @param GPIOx x can be A to G to select the GPIO port.
 * @param Pin This parameter can be one of the following values:
 *  @arg GPIO_PIN_0~GPIO_PIN_15: set related pin on
 *      @arg (GPIO_PIN_0<<16)~(GPIO_PIN_15<<16): clear related pin off
 */
void LedOnOff(GPIO_Module *GPIOx, uint32_t Pin)
{
    GPIOx->PBSC = Pin;
}
/**
 * @brief  Toggles the selected Led.
 * @param GPIOx x can be A to G to select the GPIO port.
 * @param Pin This parameter can be GPIO_PIN_0~GPIO_PIN_15.
 */
void LedBlink(GPIO_Module *GPIOx, uint16_t Pin)
{
    GPIOx->POD ^= Pin;
}

/**
 * @brief Assert failed function by user.
 * @param file The name of the call that failed.
 * @param line The source line number of the call that failed.
 */
#ifdef USE_FULL_ASSERT
void assert_failed(const uint8_t *expr, const uint8_t *file, uint32_t line)
{
    while (1)
    {
    }
}
#endif // USE_FULL_ASSERT

/**
 * @brief  Main program.
 */
int main(void)
{
    /*SystemInit() function has been called by startup file startup_n32l40x.s*/

    /* Initialize Led1~Led5 as output pushpull mode*/
    LedInit(PORT_GROUP1, LED1_PIN);
    LedInit(PORT_GROUP2, LED2_PIN | LED3_PIN | LED4_PIN | LED5_PIN);

    /*Turn on Led1*/
    LedOn(PORT_GROUP1, LED1_PIN);

    while (1)
    {
        /*LED1_PORT and LED2_PORT are the same port group.Enable Led2 blink and not effect Led1 by Exclusive-OR
         * operation.*/
        LedBlink(PORT_GROUP2, LED2_PIN);

        /*LED3_PORT, LED4_PORT and LED5_PORT are the same port group.*/
        /*Turn Led4 and Led5 off and not effect other ports by PBC register,correspond to
         * PORT_GROUP2->POD&=~(LED4_PIN|LED5_PIN);*/
        LedOff(PORT_GROUP2, LED4_PIN | LED5_PIN);
        /* Insert delay */
        Delay(0x28FFFF);

        /*Turn Led4 and Led5 on,turn Led3 off and not effect other ports by PBSC register,correspond to
         * PORT_GROUP2->POD&=~(LED3_PIN),then PORT_GROUP2->POD|=(LED4_PIN|LED5_PIN);*/
        LedOnOff(PORT_GROUP2, LED3_PIN << 16);
        LedOnOff(PORT_GROUP2, (LED4_PIN | LED5_PIN) << 16);
        /* Insert delay */
        Delay(0x28FFFF);

        /*Turn on Led3*/
        LedOn(PORT_GROUP2, LED3_PIN);
        /* Insert delay */
        Delay(0x28FFFF);
    }
}
/**
 * @}
 */
