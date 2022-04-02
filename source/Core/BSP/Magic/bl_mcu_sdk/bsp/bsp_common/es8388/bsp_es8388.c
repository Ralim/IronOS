/**
 * @file bsp_es8388.c
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
#include "hal_i2c.h"
#include "bsp_es8388.h"

/** @addtogroup  BL702_STD_PERIPH_DRIVER
 *  @{
 */

/** @addtogroup  ES8388
 *  @{
 */

/** @defgroup  ES8388_Private_Macros
 *  @{
 */

#define ES8388_I2C_SLAVE_ADDR 0x10

/*@} end of group ES8388_Private_Macros */

/** @defgroup  ES8388_Private_Types
 *  @{
 */

/*@} end of group ES8388_Private_Types */

/** @defgroup  ES8388_Private_Variables
 *  @{
 */

/*@} end of group ES8388_Private_Variables */

/** @defgroup  ES8388_Global_Variables
 *  @{
 */

/*@} end of group ES8388_Global_Variables */

/** @defgroup  ES8388_Private_Fun_Declaration
 *  @{
 */

/*@} end of group ES8388_Private_Fun_Declaration */

/** @defgroup  ES8388_Private_Functions_User_Define
 *  @{
 */

/*@} end of group ES8388_Private_Functions_User_Define */

/** @defgroup  ES8388_Private_Functions
 *  @{
 */

struct device *es8388_i2c = NULL;

/****************************************************************************/ /**
 * @brief  ES8388 write register
 *
 * @param  addr: Register address
 * @param  data: data
 *
 * @return None
 *
*******************************************************************************/
BL_Err_Type ES8388_Write_Reg(uint8_t addr, uint8_t data)
{
    i2c_msg_t msg1;

    msg1.slaveaddr = ES8388_I2C_SLAVE_ADDR,
    msg1.len = 1,
    msg1.buf = &data;
    msg1.flags = SUB_ADDR_1BYTE | I2C_WR;
    msg1.subaddr = addr;
    bflb_platform_delay_ms(10);
    return i2c_transfer(es8388_i2c, &msg1, 1);
}

/****************************************************************************/ /**
 * @brief  ES8388_Read_Reg
 *
 * @param  addr: Register address
 * @param  rdata: data
 *
 * @return None
 *
*******************************************************************************/
BL_Err_Type ES8388_Read_Reg(uint8_t addr, uint8_t *rdata)
{
    i2c_msg_t msg1;
    msg1.len = 1,
    msg1.buf = rdata;
    msg1.subaddr = addr;
    msg1.slaveaddr = ES8388_I2C_SLAVE_ADDR,
    msg1.flags = SUB_ADDR_1BYTE | I2C_RD;
    return i2c_transfer(es8388_i2c, &msg1, 1);
}

/****************************************************************************/ /**
 * @brief  ES8388 codec mode
 *
 * @param  cfg: None
 *
 * @return None
 *
*******************************************************************************/
void ES8388_Codec_Mode(ES8388_Cfg_Type *cfg)
{
    uint8_t tempVal = 0;

    ES8388_Write_Reg(0x01, 0x58); //power down whole chip analog
    ES8388_Write_Reg(0x01, 0x50); //power up whole chip analog

    ES8388_Write_Reg(0x02, 0xf3); //power down ADC/DAC and reset ADC/DAC state machine
    ES8388_Write_Reg(0x02, 0xf0); //power on   ADC/DAC

    ES8388_Write_Reg(0x2B, 0x80); //set internal ADC and DAC use sanme LRCK clock

    ES8388_Write_Reg(0x00, 0x36); //ADC clock is same as DAC . DACMCLK is the chip master clock source

    if (cfg->role == ES8388_MASTER) {
        ES8388_Write_Reg(0x08, 0x8D);
    } else {
        ES8388_Write_Reg(0x08, 0x00);
    }

    ES8388_Write_Reg(0x04, 0x00); //power up dac

    ES8388_Write_Reg(0x05, 0x00); //stop lowpower mode
    ES8388_Write_Reg(0x06, 0xc3); //stop lowpower mode

    if (cfg->mic_input_mode == ES8388_SINGLE_ENDED_MIC) {
        /*
         * default set LINPUT1 and RINPUT1 as single ended mic input
         * if user want to use LINPUT2 and RINPUT2 as input
         * please set 0x0a register as 0x50
         */
        ES8388_Write_Reg(0x0A, 0xf8);
        ES8388_Write_Reg(0x0B, 0x88); //analog mono mix to left ADC
    } else {
        /*
         * defualt select LIN1 and RIN1 as Mic diff input
         * if user want to use LIN2 and RIN2 as input
         * please set 0x0b register as 0x82
         */
        ES8388_Write_Reg(0x0A, 0xf8); // Fixed stereo problems
        ES8388_Write_Reg(0x0B, 0x82); // stereo
    }

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;

    ES8388_Write_Reg(0x0C, tempVal); //ADC I2S Format sel as i2s_frame  , data len as data_width

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;
    tempVal <<= 1;

    ES8388_Write_Reg(0x17, tempVal); //DAC I2S Format sel as i2s_frame  , data len as data_width

    /* when work in master mode , BCLK is devided form MCLK
     * default divider is 256 , see datasheet reigster 13
     */
    if (cfg->role == ES8388_MASTER) {
        ES8388_Write_Reg(0x0d, 0x06); //ADCLRCK = MCLK/256
        ES8388_Write_Reg(0x18, 0x06); //DACLRCK = MCLK/256
    }

    /*set ADC/DAC default volume as 0 db */
    ES8388_Write_Reg(0x10, 0x00); //LADC volume as 0db
    ES8388_Write_Reg(0x11, 0x00); //RADC volume as 0db
    ES8388_Write_Reg(0x1A, 0x00); //LDAC volume as 0db
    ES8388_Write_Reg(0x1B, 0x00); //RDAC volume as 0db

    tempVal = cfg->mic_pga;
    tempVal <<= 4;
    tempVal |= cfg->mic_pga;

    ES8388_Write_Reg(0x09, tempVal); //set ADC PGA as mic pga

    /*ADC ALC default Setting */
    ES8388_Write_Reg(0x12, 0xE2);
    ES8388_Write_Reg(0x13, 0xC0);
    ES8388_Write_Reg(0x14, 0x12);
    ES8388_Write_Reg(0x15, 0x06);
    ES8388_Write_Reg(0x16, 0xC3);

    /*Mixer setting for LDAC to LOUT and RDAC TO ROUT
     * default close mixer
     */
    ES8388_Write_Reg(0x27, 0xB8);
    ES8388_Write_Reg(0x2A, 0xB8);

    ES8388_Write_Reg(0x02, 0x00); //startup FSM and DLL
    bflb_platform_delay_ms(500);

    /* set LOUT1 ROUT1 LOUT2 ROUT2 volume */
    ES8388_Write_Reg(0x2e, 0x1E);
    ES8388_Write_Reg(0x2f, 0x1E);
    ES8388_Write_Reg(0x30, 0x1E);
    ES8388_Write_Reg(0x31, 0x1E);

    //ES8388_Write_Reg(0x04,0x3C);//enable LOUT & ROUT
    ES8388_Write_Reg(0x04, 0x24);
    ES8388_Write_Reg(0x26, 0x01);
    ES8388_Write_Reg(0x03, 0x09); //power up ADC Enable LIN &RIN.
}

/****************************************************************************/ /**
 * @brief  ES8388 recording mode
 *
 * @param  cfg: None
 *
 * @return None
 *
*******************************************************************************/
void ES8388_Recording_Mode(ES8388_Cfg_Type *cfg)
{
    uint8_t tempVal = 0;
    MSG("RECORDING\r\n");
    ES8388_Write_Reg(0x01, 0x58); //power down whole chip analog
    ES8388_Write_Reg(0x01, 0x50); //power up whole chip analog

    ES8388_Write_Reg(0x02, 0xf3); //power down ADC/DAC and reset ADC/DAC state machine
    ES8388_Write_Reg(0x02, 0xf0); //power on   ADC/DAC
    ES8388_Write_Reg(0x2B, 0x80); //set internal ADC and DAC use the same LRCK clock
    ES8388_Write_Reg(0x00, 0x16); //ADC clock is same as DAC. use ADC MCLK as internal MCLK

    if (cfg->role == ES8388_MASTER) {
        ES8388_Write_Reg(0x08, 0x80);
    } else {
        ES8388_Write_Reg(0x08, 0x00);
    }

    ES8388_Write_Reg(0x04, 0xc0);
    ES8388_Write_Reg(0x05, 0x00);
    ES8388_Write_Reg(0x06, 0xc3);

    if (cfg->mic_input_mode == ES8388_SINGLE_ENDED_MIC) {
        /*
         * default set LINPUT1 and RINPUT1 as single ended mic input
         * if user want to use LINPUT2 and RINPUT2 as input
         * please set 0x0a register as 0x50
         */
        //ES8388_Write_Reg(0x0A,0x00);//select lin1 and rin1 as micphone input
        ES8388_Write_Reg(0x0A, 0xf8);
        ES8388_Write_Reg(0x0B, 0x88); //analog mono mix to left ADC
        //ES8388_Write_Reg(0x0B,0x90); //analog mono mix to right ADC
    } else {
        /*
         * defualt select LIN1 and RIN1 as Mic diff input
         * if user want to use LIN2 and RIN2 as input
         * please set 0x0b register as 0x82
         */
        //ES8388_Write_Reg(0x0A,0xf0);//set micphone input as difference mode
        //ES8388_Write_Reg(0x0B,0x02);//set LIN1 and RIN1 as micphone different input
        ES8388_Write_Reg(0x0A, 0xf8); // Fixed stereo problems
        ES8388_Write_Reg(0x0B, 0x82); // stereo
    }

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;

    ES8388_Write_Reg(0x0C, tempVal); //ADC I2S Format sel as i2s_frame  , data len as data_width

    ES8388_Write_Reg(0x0d, 0x02); //ADC LRCK = MCLK/256

    ES8388_Write_Reg(0x10, 0x00); //ADC VOLUME = 0 DB
    ES8388_Write_Reg(0x11, 0x00); //set ADC VOLUME as 0 DB

    tempVal = cfg->mic_pga;
    tempVal <<= 4;
    tempVal |= cfg->mic_pga;

    ES8388_Write_Reg(0x09, tempVal); //MIC PGA SEL

    ES8388_Write_Reg(0x12, 0xe2); //MIC ALC SETTING

    ES8388_Write_Reg(0x13, 0xc0);
    ES8388_Write_Reg(0x14, 0x12);
    ES8388_Write_Reg(0x15, 0x06);
    ES8388_Write_Reg(0x16, 0xc3);

    ES8388_Write_Reg(0x02, 0x55); //startup FSM and DLL
    ES8388_Write_Reg(0x03, 0x09); //power up adc , enable LIN and RIN
}

/****************************************************************************/ /**
 * @brief  ES8388 Playback mode
 *
 * @param  cfg: None
 *
 * @return None
 *
*******************************************************************************/
void ES8388_Playback_Mode(ES8388_Cfg_Type *cfg)
{
    uint8_t tempVal = 0;

    ES8388_Write_Reg(0x00, 0x80); //Reset control port register to default
    ES8388_Write_Reg(0x00, 0x06); //Close reset
    bflb_platform_delay_ms(10);
    ES8388_Write_Reg(0x02, 0xF3); //Stop STM, DLL and digital block

    if (cfg->role == ES8388_MASTER) {
        ES8388_Write_Reg(0x08, 0x80);
    } else {
        ES8388_Write_Reg(0x08, 0x00);
    }

    ES8388_Write_Reg(0x2B, 0x80); //Set ADC and DAC have the same LRCK
    ES8388_Write_Reg(0x00, 0x05); //Start up reference
    ES8388_Write_Reg(0x01, 0x40); //Start up reference
    bflb_platform_delay_ms(30);
    ES8388_Write_Reg(0x03, 0x00); //Power on ADC and LIN/RIN input
    ES8388_Write_Reg(0x04, 0x3F); //Power on DAC and LOUT/ROUT input

    /* Set ADC */
    ES8388_Write_Reg(0x09, 0x77); //MicBoost PGA = +21dB
    ES8388_Write_Reg(0x0A, 0xF0); //Differential input
    ES8388_Write_Reg(0x0B, 0x02); //Select LIN1 and RIN1 as differential input pairs

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;

    ES8388_Write_Reg(0x0C, tempVal); //ADC I2S Format sel as i2s_frame  , data len as data_width

    ES8388_Write_Reg(0x0D, 0x04); //MCLK / LRCK = 512
    ES8388_Write_Reg(0x10, 0x00); //LADC volume = 0dB
    ES8388_Write_Reg(0x11, 0x00); //RADC volume = 0dB
    ES8388_Write_Reg(0x12, 0xE2); //ALC enable, PGA Max.Gain = 23.5dB, Min.Gain = 0dB
    ES8388_Write_Reg(0x13, 0xC0); //ALC target = -4.5dB, ALC hold time = 0 ms
    ES8388_Write_Reg(0x14, 0x12); //Decay time = 820us, attack time = 416us
    ES8388_Write_Reg(0x15, 0x06); //ALC mode
    ES8388_Write_Reg(0x16, 0xC3); //Nose gate =-40.5dB, NGG = 0x01(mute ADC)

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;
    tempVal <<= 1;

    ES8388_Write_Reg(0x18, 0x04); //MCLK / LRCK = 512
    ES8388_Write_Reg(0x1A, 0x00); //LDAC volume = 0dB
    ES8388_Write_Reg(0x1B, 0x00); //RDAC volume = 0dB
    ES8388_Write_Reg(0x26, 0x00); //Setup mixer
    ES8388_Write_Reg(0x27, 0xB8); //Setup mixer
    ES8388_Write_Reg(0x28, 0x38); //Setup mixer
    ES8388_Write_Reg(0x29, 0x38); //Setup mixer
    ES8388_Write_Reg(0x2A, 0xB8); //Setup mixer
    ES8388_Write_Reg(0x2E, 0x1E); //Set Lout/Rout volume:0dB
    ES8388_Write_Reg(0x2F, 0x1E); //Set Lout/Rout volume:0dB
    ES8388_Write_Reg(0x30, 0x1E); //Set Lout/Rout volume:0dB
    ES8388_Write_Reg(0x31, 0x1E); //Set Lout/Rout volume:0dB
    ES8388_Write_Reg(0x02, 0x00); //Power up DEM and STM
}

/****************************************************************************/ /**
 * @brief  ES8388_I2C_Init
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
int ES8388_Set_Voice_Volume(int volume)
{
    int res;

    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }

    volume /= 3;
    res = ES8388_Write_Reg(0x2e, volume);
    res |= ES8388_Write_Reg(0x2f, volume);
    res |= ES8388_Write_Reg(0x30, volume);
    res |= ES8388_Write_Reg(0x31, volume);
    return res;
}

/****************************************************************************/ /**
 * @brief  ES8388_I2C_Init
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ES8388_I2C_Init(void)
{
    i2c_register(I2C0_INDEX, "i2c");
    es8388_i2c = device_find("i2c");

    if (es8388_i2c) {
        device_open(es8388_i2c, 0);
    }
}

/****************************************************************************/ /**
 * @brief  ES8388_Reg_Dump
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ES8388_Reg_Dump(void)
{
    int i;
    uint8_t tmp;

    for (i = 0; i < 0X3F; i++) {
        if (ES8388_Read_Reg(i, &tmp) != SUCCESS) {
            bflb_platform_printf("iic read err\r\n");
        }

        bflb_platform_printf("Reg[%02d]=0x%02x \n", i, tmp);
    }

}

/*@} end of group ES8388_Private_Functions */

/** @defgroup  ES8388_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  ES8388 Init function
 *
 * @param  cfg: None
 *
 * @return None
 *
*******************************************************************************/
void ES8388_Init(ES8388_Cfg_Type *cfg)
{
    ES8388_I2C_Init();

    switch (cfg->work_mode) {
        case ES8388_CODEC_MDOE:
            ES8388_Codec_Mode(cfg);
            break;

        case ES8388_RECORDING_MODE:
            ES8388_Recording_Mode(cfg);
            break;

        case ES8388_PLAY_BACK_MODE:
            ES8388_Playback_Mode(cfg);
            break;

        default:
            break;
    }
}

/*@} end of group ES8388_Public_Functions */

/*@} end of group ES8388 */

/*@} end of group BL702_STD_PERIPH_DRIVER */
