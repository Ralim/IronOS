/**
  ******************************************************************************
  * @file    bl702_i2c_gpio_sim.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
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
#ifndef __BL702_I2C_GPIO_SIM_H__
#define __BL702_I2C_GPIO_SIM_H__

#include "bl702_glb.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  I2C_GPIO_SIM
 *  @{
 */

/** @defgroup  I2C_GPIO_SIM_Public_Types
 *  @{
 */

/*@} end of group I2C_GPIO_SIM_Public_Types */

/** @defgroup  I2C_GPIO_SIM_Public_Functions
 *  @{
 */

/**
 *  @brief I2C Functions
 */
void I2C_GPIO_Sim_Init(GLB_GPIO_Type sclGPIOPin, GLB_GPIO_Type sdaGPIOPin);
int I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_GetAck(void);
void I2C_SendByte(uint8_t Data);
uint8_t I2C_ReadByte(uint8_t ack);
int SCCB_Init(GLB_GPIO_Type sclGPIOPin, GLB_GPIO_Type sdaGPIOPin);
int SCCB_Write(uint8_t slave_addr, uint8_t *data, uint32_t wrsize);
int SCCB_Read(uint8_t slave_addr, uint8_t *data, uint32_t rdsize);

/*@} end of group I2C_GPIO_SIM_Public_Functions */

/*@} end of group I2C_GPIO_SIM */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_I2C_GPIO_SIM_H__ */
