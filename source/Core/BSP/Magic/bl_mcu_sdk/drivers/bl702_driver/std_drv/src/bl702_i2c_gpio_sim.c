/**
  ******************************************************************************
  * @file    bl702_i2c_gpio_sim.c
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

#include "bl702_glb.h"
#include "bl702_gpio.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  I2C_GPIO_SIM
 *  @{
 */

/** @defgroup  I2C_GPIO_SIM_Private_Macros
 *  @{
 */
#define SCL_H           GLB_GPIO_Write(sclPin, 1)
#define SCL_L           GLB_GPIO_Write(sclPin, 0)
#define SDA_H           GLB_GPIO_Write(sdaPin, 1)
#define SDA_L           GLB_GPIO_Write(sdaPin, 0)
#define SDA_read        GLB_GPIO_Read(sdaPin)
#define I2C_Delay_US(a) BL702_Delay_US(a)
#define I2C_Delay_Const I2C_Delay_US(2)

/*@} end of group I2C_GPIO_SIM_Private_Macros */

/** @defgroup  I2C_GPIO_SIM_Private_Types
 *  @{
 */

/*@} end of group I2C_GPIO_SIM_Private_Types */

/** @defgroup  I2C_GPIO_SIM_Private_Variables
 *  @{
 */
static GLB_GPIO_Type sclPin;
static GLB_GPIO_Type sdaPin;
static uint8_t sda_out = 0;

/*@} end of group I2C_GPIO_SIM_Private_Variables */

/** @defgroup  I2C_GPIO_SIM_Global_Variables
 *  @{
 */

/*@} end of group I2C_GPIO_SIM_Global_Variables */

/** @defgroup  I2C_GPIO_SIM_Private_Fun_Declaration
 *  @{
 */

/*@} end of group I2C_GPIO_SIM_Private_Fun_Declaration */

/** @defgroup  I2C_GPIO_SIM_Private_Functions
 *  @{
 */

/*@} end of group I2C_GPIO_SIM_Private_Functions */

/** @defgroup  I2C_GPIO_SIM_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  I2C GPIO init function
 *
 * @param  sclGPIOPin: I2C SCL GPIO pin
 * @param  sdaGPIOPin: I2C SDA GPIO pin
 *
 * @return None
 *
*******************************************************************************/
void I2C_GPIO_Sim_Init(GLB_GPIO_Type sclGPIOPin, GLB_GPIO_Type sdaGPIOPin)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[2];
    uint8_t gpiofuns[2];
    int i;

    sclPin = sclGPIOPin;
    sdaPin = sdaGPIOPin;

    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioMode = GPIO_MODE_OUTPUT;

    gpiopins[0] = sclPin;
    gpiopins[1] = sdaPin;
    gpiofuns[0] = 11;
    gpiofuns[1] = 11;

    for (i = 0; i < sizeof(gpiopins) / sizeof(gpiopins[0]); i++) {
        cfg.gpioPin = gpiopins[i];
        cfg.gpioFun = gpiofuns[i];
        GLB_GPIO_Init(&cfg);
    }
}

/****************************************************************************/ /**
 * @brief  I2C SDA out function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void I2C_SDA_OUT(void)
{
    GLB_GPIO_Cfg_Type cfg;

    if (sda_out == 1) {
        return;
    }

    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.gpioPin = sdaPin;
    cfg.gpioFun = 11;

    GLB_GPIO_Init(&cfg);

    sda_out = 1;
}

/****************************************************************************/ /**
 * @brief  I2C SDA in function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void I2C_SDA_IN(void)
{
    GLB_GPIO_Cfg_Type cfg;

    if (sda_out == 0) {
        return;
    }

    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 1;
    cfg.smtCtrl = 1;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.gpioPin = sdaPin;
    cfg.gpioFun = 11;

    GLB_GPIO_Init(&cfg);

    sda_out = 0;
}

/****************************************************************************/ /**
 * @brief  I2C start function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void I2C_Start(void)
{
    I2C_SDA_OUT();
    SDA_H;
    I2C_Delay_Const;
    SCL_H;
    I2C_Delay_Const;
    SDA_L;
    I2C_Delay_Const;
    SCL_L;
}

/****************************************************************************/ /**
 * @brief  I2C stop function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void I2C_Stop(void)
{
    I2C_SDA_OUT();
    SCL_L;
    I2C_Delay_Const;
    SDA_L;
    I2C_Delay_Const;
    SCL_H;
    I2C_Delay_Const;
    SDA_H;
    I2C_Delay_Const;
}

/****************************************************************************/ /**
 * @brief  I2C ack function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void I2C_Ack(void)
{
    SCL_L;
    I2C_Delay_Const;
    I2C_SDA_OUT();
    SDA_L;
    I2C_Delay_Const;
    SCL_H;
    I2C_Delay_Const;
    SCL_L;
}

/****************************************************************************/ /**
 * @brief  I2C no ack function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
static void I2C_NoAck(void)
{
    SCL_L;
    I2C_Delay_Const;
    I2C_SDA_OUT();
    SDA_H;
    I2C_Delay_Const;
    SCL_H;
    I2C_Delay_Const;
    SCL_L;
}

/****************************************************************************/ /**
 * @brief  I2C get ack function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t I2C_GetAck(void)
{
    uint8_t time = 0;

    I2C_Delay_Const;
    I2C_SDA_IN();

    I2C_Delay_Const;
    SCL_H;
    I2C_Delay_Const;

    while (SDA_read) {
        time++;

        if (time > 25) {
            SCL_L;
            return 0;
        }
    }

    SCL_L;

    return 1;
}

/****************************************************************************/ /**
 * @brief  I2C send byte function
 *
 * @param  Data: send data
 *
 * @return None
 *
*******************************************************************************/
void I2C_SendByte(uint8_t Data)
{
    uint8_t cnt;

    I2C_SDA_OUT();

    for (cnt = 0; cnt < 8; cnt++) {
        SCL_L;
        I2C_Delay_Const;

        if (Data & 0x80) {
            SDA_H;
        } else {
            SDA_L;
        }

        Data <<= 1;
        I2C_Delay_Const;
        SCL_H;
        I2C_Delay_Const;
    }

    SCL_L;

    I2C_Delay_Const;
}

/****************************************************************************/ /**
 * @brief  I2C read byte function
 *
 * @param  ack: i2c ack byte
 *
 * @return None
 *
*******************************************************************************/
uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t cnt;
    uint8_t data = 0;

    I2C_SDA_IN();

    for (cnt = 0; cnt < 8; cnt++) {
        SCL_L;
        I2C_Delay_Const;

        SCL_H;
        I2C_Delay_Const;
        data <<= 1;

        if (SDA_read) {
            data |= 0x01;
        }
    }

    if (ack == 1) {
        I2C_Ack();
    } else {
        I2C_NoAck();
    }

    return data;
}

/****************************************************************************/ /**
 * @brief  SCCB init function
 *
 * @param  sclGPIOPin: I2C SCL GPIO pin
 * @param  sdaGPIOPin: I2C SDA GPIO pin
 *
 * @return None
 *
*******************************************************************************/
int SCCB_Init(GLB_GPIO_Type sclGPIOPin, GLB_GPIO_Type sdaGPIOPin)
{
    I2C_GPIO_Sim_Init(sclGPIOPin, sdaGPIOPin);
    return 0;
}

/****************************************************************************/ /**
 * @brief  SCCB write function
 *
 * @param  slave_addr: salve addr
 * @param  data: write data
 * @param  wrsize: write data len
 *
 * @return None
 *
*******************************************************************************/
int SCCB_Write(uint8_t slave_addr, uint8_t *data, uint32_t wrsize)
{
    uint32_t i = 0;

    I2C_Start();

    I2C_SendByte((slave_addr << 1) | 0);

    if (!I2C_GetAck()) {
        I2C_Stop();
        return -1;
    }

    for (i = 0; i < wrsize; i++) {
        I2C_SendByte(data[i]);

        if (!I2C_GetAck()) {
            I2C_Stop();
            return -1;
        }
    }

    I2C_Stop();

    return 0;
}

/****************************************************************************/ /**
 * @brief  SCCB read function
 *
 * @param  slave_addr: salve addr
 * @param  data: read data
 * @param  rdsize: read data len
 *
 * @return None
 *
*******************************************************************************/
int SCCB_Read(uint8_t slave_addr, uint8_t *data, uint32_t rdsize)
{
    uint32_t i = 0;

    if (0 == rdsize) {
        return -1;
    }

    I2C_Start();

    I2C_SendByte((slave_addr << 1) | 1);

    if (!I2C_GetAck()) {
        I2C_Stop();
        return -1;
    }

    for (i = 0; i < rdsize - 1; i++) {
        data[i] = I2C_ReadByte(1);
    }

    data[i] = I2C_ReadByte(0);

    I2C_Stop();

    return 0;
}

/*@} end of group I2C_GPIO_SIM_Public_Functions */

/*@} end of group I2C_GPIO_SIM */

/*@} end of group BL702_Peripheral_Driver */
