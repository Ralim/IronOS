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
 * @file main.h
 * @author Nsing
 * @version V1.2.2
 *
 * @copyright Copyright (c) 2022, Nsingechnologies Pte. Ltd. All rights reserved.
 */
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "n32l40x.h"

/*Led1-PB0,Led2-PB1,Led3-PE0,Led4-PE1,Led5-PE5*/
#define PORT_GROUP1 GPIOA
#define PORT_GROUP2 GPIOB
#define LED1_PORT PORT_GROUP1
#define LED2_PORT PORT_GROUP2
#define LED3_PORT PORT_GROUP2
#define LED4_PORT PORT_GROUP2
#define LED5_PORT PORT_GROUP2
#define LED1_PIN GPIO_PIN_8
#define LED2_PIN GPIO_PIN_4
#define LED3_PIN GPIO_PIN_5
#define LED4_PIN GPIO_PIN_6
#define LED5_PIN GPIO_PIN_7

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
