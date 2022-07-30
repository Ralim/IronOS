/**
  ******************************************************************************
  * @file    bl702_clock.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "bl702_clock.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  CLOCK
 *  @{
 */

/** @defgroup  CLOCK_Private_Macros
 *  @{
 */

/*@} end of group CLOCK_Private_Macros */

/** @defgroup  CLOCK_Private_Types
 *  @{
 */

/*@} end of group CLOCK_Private_Types */

/** @defgroup  CLOCK_Private_Variables
 *  @{
 */
static Clock_Cfg_Type clkCfg;

/*@} end of group CLOCK_Private_Variables */

/** @defgroup  CLOCK_Global_Variables
 *  @{
 */

/*@} end of group CLOCK_Global_Variables */

/** @defgroup  CLOCK_Private_Fun_Declaration
 *  @{
 */

/*@} end of group CLOCK_Private_Fun_Declaration */

/** @defgroup  CLOCK_Private_Functions
 *  @{
 */

/*@} end of group CLOCK_Private_Functions */

/** @defgroup  CLOCK_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Set System Clock
 *
 * @param  type: System clock type
 * @param  clock: System clock value
 *
 * @return None
 *
*******************************************************************************/
void Clock_System_Clock_Set(BL_System_Clock_Type type, uint32_t clock)
{
    if (type < BL_SYSTEM_CLOCK_MAX) {
        clkCfg.systemClock[type] = clock / 1000000;
    }
}

/****************************************************************************/ /**
 * @brief  Set Peripheral Clock
 *
 * @param  type: Peripheral clock type
 * @param  clock: Peripheral clock value
 *
 * @return None
 *
*******************************************************************************/
void Clock_Peripheral_Clock_Set(BL_AHB_Slave1_Type type, uint32_t clock)
{
    if (type < BL_AHB_SLAVE1_MAX) {
        if (type == BL_AHB_SLAVE1_I2S) {
            clkCfg.i2sClock = clock;
        } else {
            clkCfg.peripheralClock[type] = clock / 1000000;
        }
    }
}

/****************************************************************************/ /**
 * @brief  Get System Clock
 *
 * @param  type: System clock type
 *
 * @return System clock value
 *
*******************************************************************************/
uint32_t Clock_System_Clock_Get(BL_System_Clock_Type type)
{
    if (type < BL_SYSTEM_CLOCK_MAX) {
        return clkCfg.systemClock[type] * 1000000;
    }

    return 0;
}

/****************************************************************************/ /**
 * @brief  Get Peripheral Clock
 *
 * @param  type: Peripheral clock type
 *
 * @return Peripheral clock value
 *
*******************************************************************************/
uint32_t Clock_Peripheral_Clock_Get(BL_AHB_Slave1_Type type)
{
    if (type < BL_AHB_SLAVE1_MAX) {
        if (type == BL_AHB_SLAVE1_I2S) {
            return clkCfg.i2sClock;
        } else {
            return clkCfg.peripheralClock[type] * 1000000;
        }
    }

    return 0;
}

/*@} end of group CLOCK_Public_Functions */

/*@} end of group CLOCK */

/*@} end of group BL702_Peripheral_Driver */
