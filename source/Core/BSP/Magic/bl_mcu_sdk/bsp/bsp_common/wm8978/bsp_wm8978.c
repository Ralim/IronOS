/**
 * @file wm8978.h
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
#include "bsp_wm8978.h"

struct device *wm8978_i2c = NULL;

#define GET_REG_VAL(reg, val, off, bits)           \
    {                                              \
        reg &= ~(((1 << bits) - 1) << off);        \
        reg |= ((val & ((1 << bits) - 1)) << off); \
    }

//WM8978寄存器值缓存区(总共58个寄存器,0~57),占用116字节内存
//因为WM8978的IIC操作不支持读操作,所以在本地保存所有寄存器值
//写WM8978寄存器时,同步更新到本地寄存器值,读寄存器时,直接返回本地保存的寄存器值.
//注意:WM8978的寄存器值是9位的,所以要用u16来存储.
volatile static uint16_t WM8978_RegCash[] = {
    0X0000, 0X0000, 0X0000, 0X0000, 0X0050, 0X0000, 0X0140, 0X0000,
    0X0000, 0X0000, 0X0000, 0X00FF, 0X00FF, 0X0000, 0X0100, 0X00FF,
    0X00FF, 0X0000, 0X012C, 0X002C, 0X002C, 0X002C, 0X002C, 0X0000,
    0X0032, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000, 0X0000,
    0X0038, 0X000B, 0X0032, 0X0000, 0X0008, 0X000C, 0X0093, 0X00E9,
    0X0000, 0X0000, 0X0000, 0X0000, 0X0003, 0X0010, 0X0010, 0X0100,
    0X0100, 0X0002, 0X0001, 0X0001, 0X0039, 0X0039, 0X0039, 0X0039,
    0X0001, 0X0001
};

void wm8978_i2c_init(void)
{
    i2c_register(I2C0_INDEX, "i2c");
    wm8978_i2c = device_find("i2c");

    if (wm8978_i2c) {
        device_open(wm8978_i2c, 0);
    }
}

uint16_t WM8978_ReadReg(uint8_t RegAddr)
{
    return WM8978_RegCash[RegAddr];
}

/**
 * @brief 写寄存器，寄存器是9bit的,最高一位是地址的最低一位
 *
 * @param RegAddr
 * @param Value
 * @return uint8_t
 */
uint8_t WM8978_WriteReg(uint8_t RegAddr, uint16_t Value)
{
    i2c_msg_t msg1;
    int flag = 0;
    uint8_t val = Value & 0XFF;

    msg1.slaveaddr = WM8978_ADDR,
    msg1.len = 1,
    msg1.buf = &val;
    msg1.flags = SUB_ADDR_1BYTE | I2C_WR;
    msg1.subaddr = (RegAddr << 1) | ((Value >> 8) & 0X01);
    bflb_platform_delay_ms(10);
    flag = i2c_transfer(wm8978_i2c, &msg1, 1);
    if (flag) {
        MSG("[E]TYPE[%d] | REG[%d] | %04x\r\n", flag, RegAddr, Value);
        return 1;
    }
    WM8978_RegCash[RegAddr] = Value; //保存寄存器值到本地
    return 0;
}

/**
 * @brief WM8978_SetVolume,(0-63)
 *
 * @param Volume
 */
void WM8978_SetVolume(uint8_t Volume)
{
    uint16_t regV;
    Volume = 0x3F;
    regV = Volume;
    //耳机左右声道音量设置
    WM8978_WriteReg(52, regV);
    WM8978_WriteReg(53, regV | (1 << 8)); //同步更新(SPKVU=1)
    //喇叭左右声道音量设置
    WM8978_WriteReg(54, regV);
    WM8978_WriteReg(55, regV | (1 << 8));
}

uint8_t WM8978_ReadVolume(void)
{
    return (uint8_t)(WM8978_ReadReg(52) & 0x3F);
}

void WM8978_MIC_Gain(uint8_t Gain)
{
    Gain &= 0x3F;
    WM8978_WriteReg(45, Gain);
    WM8978_WriteReg(46, Gain | (1 << 8));
}

/**
 *设置I2S工作模式
 *fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;
 *len:0,16位;1,20位;2,24位;3,32位;
*/
void WM8978_I2S_Cfg(uint8_t fmt, uint8_t len)
{
    //限定范围
    fmt &= 0x03;
    len &= 0x03;
    //R4,WM8978工作模式设置
    WM8978_WriteReg(4, (fmt << 3) | (len << 5));
}

/**
 *WM8978 DAC/ADC配置
 *adcen:adc使能(1)/关闭(0)
 *dacen:dac使能(1)/关闭(0)
*/
void WM8978_ADDA_Cfg(uint8_t dacen, uint8_t adcen)
{
    uint16_t regval;

    regval = WM8978_ReadReg(3);
    if (dacen) {
        //R3最低2个位设置为1,开启DACR&DACL
        regval |= 3 << 0;
    } else {
        //R3最低2个位清零,关闭DACR&DACL.
        regval &= ~(3 << 0);
    }
    WM8978_WriteReg(3, regval);
    regval = WM8978_ReadReg(2);
    if (adcen) {
        //R2最低2个位设置为1,开启ADCR&ADCL
        regval |= 3 << 0;
    } else {
        //R2最低2个位清零,关闭ADCR&ADCL.
        regval &= ~(3 << 0);
    }
    WM8978_WriteReg(2, regval);
}

/**
 *WM8978 L2/R2(也就是Line In)增益设置(L2/R2-->ADC输入部分的增益)
 *gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
*/
void WM8978_LINEIN_Gain(uint8_t gain)
{
    uint16_t regval;

    gain &= 0X07;
    regval = WM8978_ReadReg(47);
    //清除原来的设置
    regval &= ~(7 << 4);
    WM8978_WriteReg(47, regval | gain << 4);
    regval = WM8978_ReadReg(48);
    //清除原来的设置
    regval &= ~(7 << 4);
    WM8978_WriteReg(48, regval | gain << 4);
}

/**
 *WM8978 AUXR,AUXL(PWM音频部分)增益设置(AUXR/L-->ADC输入部分的增益)
 *gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
*/
void WM8978_AUX_Gain(uint8_t gain)
{
    uint16_t regval;

    gain &= 0X07;
    regval = WM8978_ReadReg(47);
    //清除原来的设置
    regval &= ~(7 << 0);
    WM8978_WriteReg(47, regval | gain << 0);
    regval = WM8978_ReadReg(48);
    //清除原来的设置
    regval &= ~(7 << 0);
    WM8978_WriteReg(48, regval | gain << 0);
}

/*
 *WM8978 输入通道配置
 *micen:MIC开启(1)/关闭(0)
 *lineinen:Line In开启(1)/关闭(0)
 *auxen:aux开启(1)/关闭(0)
*/
void WM8978_Input_Cfg(uint8_t micen, uint8_t lineinen, uint8_t auxen)
{
    uint16_t regval;

    regval = WM8978_ReadReg(2);
    if (micen) {
        //开启INPPGAENR,INPPGAENL(MIC的PGA放大)
        regval |= 3 << 2;
    } else {
        //关闭INPPGAENR,INPPGAENL.
        regval &= ~(3 << 2);
    }
    WM8978_WriteReg(2, regval);
    regval = WM8978_ReadReg(44);
    if (micen) {
        //开启LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
        regval |= 3 << 4 | 3 << 0;
    } else {
        //关闭LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
        regval &= ~(3 << 4 | 3 << 0);
    }
    WM8978_WriteReg(44, regval);
    if (lineinen) {
        //LINE IN 0dB增益
        WM8978_LINEIN_Gain(5);
    } else {
        //关闭LINE IN
        WM8978_LINEIN_Gain(0);
    }
    if (auxen) {
        //AUX 6dB增益
        WM8978_AUX_Gain(7);
    } else {
        //关闭AUX输入
        WM8978_AUX_Gain(0);
    }
}

/**
 *WM8978 输出配置
 *dacen:DAC输出(放音)开启(1)/关闭(0)
 *bpsen:Bypass输出(录音,包括MIC,LINE IN,AUX等)开启(1)/关闭(0)
*/
void WM8978_Output_Cfg(uint8_t dacen, uint8_t bpsen)
{
    uint16_t regval = 0;
    if (dacen) {
        //DAC输出使能
        regval |= 1 << 0;
    }
    if (bpsen) {
        //BYPASS使能
        regval |= 1 << 1;
        //0dB增益
        regval |= 5 << 2;
    }
    WM8978_WriteReg(50, regval);
    WM8978_WriteReg(51, regval);
}

/**
 *设置耳机左右声道音量
 *voll:左声道音量(0~63)
 *volr:右声道音量(0~63)
*/
void WM8978_HPvol_Set(uint8_t voll, uint8_t volr)
{
    //限定范围
    voll &= 0X3F;
    volr &= 0X3F;
    //音量为0时,直接mute
    if (voll == 0) {
        voll |= 1 << 6;
    }
    //音量为0时,直接mute
    if (volr == 0) {
        volr |= 1 << 6;
    }
    //R52,耳机左声道音量设置
    WM8978_WriteReg(52, voll);
    //R53,耳机右声道音量设置,同步更新(HPVU=1)
    WM8978_WriteReg(53, volr | (1 << 8));
}

/**
 *设置喇叭音量
 *voll:左声道音量(0~63)
*/
void WM8978_SPKvol_Set(uint8_t volx)
{
    //限定范围
    volx &= 0x3F;

    //音量为0时,直接mute
    if (volx == 0)
        volx |= 1 << 6;
    //R54,喇叭左声道音量设置
    WM8978_WriteReg(54, volx);
    //R55,喇叭右声道音量设置,同步更新(SPKVU=1)
    WM8978_WriteReg(55, volx | (1 << 8));
}

uint8_t WM8978_Init(void)
{
    uint8_t ucFun_res = 0;

    wm8978_i2c_init();
    ucFun_res |= WM8978_WriteReg(0, 0x0000); //向0寄存器写入任意值，就可以软件复位
    //先把声音关到最小，防止噪音
    WM8978_HPvol_Set(0, 0);
    WM8978_SPKvol_Set(0);
    ucFun_res |= WM8978_WriteReg(1, 0x001B); //R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)
    ucFun_res |= WM8978_WriteReg(2, 0x01B0); //R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
    ucFun_res |= WM8978_WriteReg(3, 0x006C); //R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能
    ucFun_res |= WM8978_WriteReg(6, 0x0000); //R6,MCLK由外部提供

    ucFun_res |= WM8978_WriteReg(43, 1 << 4); //R43,INVROUT2反向,驱动喇叭
    ucFun_res |= WM8978_WriteReg(47, 1 << 8); //R47设置,PGABOOSTL,左通道MIC获得20倍增益
    ucFun_res |= WM8978_WriteReg(48, 1 << 8); //R48设置,PGABOOSTR,右通道MIC获得20倍增益
    ucFun_res |= WM8978_WriteReg(49, 1 << 1); //R49,TSDEN,开启过热保护
    ucFun_res |= WM8978_WriteReg(10, 1 << 3); //R10,SOFTMUTE关闭,128x采样,最佳SNR
    ucFun_res |= WM8978_WriteReg(14, 1 << 3); //R14,ADC 128x采样率

    return ucFun_res;
}

/**
 * @brief 仅播放模式
 *
 */
void WM8978_PlayMode(void)
{
    WM8978_ADDA_Cfg(1, 0);     //开启DAC
    WM8978_Input_Cfg(0, 0, 0); //关闭输入通道
    WM8978_Output_Cfg(1, 0);   //开启DAC输出
    WM8978_HPvol_Set(20, 20);
    WM8978_SPKvol_Set(0x3F);
    WM8978_MIC_Gain(0);
    WM8978_I2S_Cfg(1, 0); //设置I2S接口模式,左对齐16位
}

/**
 * @brief 仅录音模式
 *
 */
void WM8978_RecoMode(void)
{
    WM8978_ADDA_Cfg(0, 1);     //开启ADC
    WM8978_Input_Cfg(1, 0, 0); //开启输入通道(MIC)
    WM8978_Output_Cfg(0, 1);   //开启BYPASS输出
    WM8978_HPvol_Set(0, 0);
    WM8978_SPKvol_Set(0);
    WM8978_MIC_Gain(50);  //MIC增益设置
    WM8978_I2S_Cfg(1, 0); //设置I2S接口模式,左对齐16位
}

//设置3D环绕声
//depth:0~15(3D强度,0最弱,15最强)
void WM8978_3D_Set(uint8_t depth)
{
    depth &= 0XF;                //限定范围
    WM8978_Write_Reg(41, depth); //R41,3D环绕设置
}
//设置EQ/3D作用方向
//dir:0,在ADC起作用
//    1,在DAC起作用(默认)
void WM8978_EQ_3D_Dir(uint8_t dir)
{
    uint16_t regval;
    regval = WM8978_Read_Reg(0X12);
    if (dir)
        regval |= 1 << 8;
    else
        regval &= ~(1 << 8);
    WM8978_Write_Reg(18, regval); //R18,EQ1的第9位控制EQ/3D方向
}

//设置EQ1
//cfreq:截止频率,0~3,分别对应:80/105/135/175Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ1_Set(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval = WM8978_Read_Reg(18);
    regval &= 0X100;
    regval |= cfreq << 5;         //设置截止频率
    regval |= gain;               //设置增益
    WM8978_Write_Reg(18, regval); //R18,EQ1设置
}
//设置EQ2
//cfreq:中心频率,0~3,分别对应:230/300/385/500Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ2_Set(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;         //设置截止频率
    regval |= gain;               //设置增益
    WM8978_Write_Reg(19, regval); //R19,EQ2设置
}
//设置EQ3
//cfreq:中心频率,0~3,分别对应:650/850/1100/1400Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ3_Set(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;         //设置截止频率
    regval |= gain;               //设置增益
    WM8978_Write_Reg(20, regval); //R20,EQ3设置
}
//设置EQ4
//cfreq:中心频率,0~3,分别对应:1800/2400/3200/4100Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ4_Set(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;         //设置截止频率
    regval |= gain;               //设置增益
    WM8978_Write_Reg(21, regval); //R21,EQ4设置
}
//设置EQ5
//cfreq:中心频率,0~3,分别对应:5300/6900/9000/11700Hz
//gain:增益,0~24,对应-12~+12dB
void WM8978_EQ5_Set(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;         //设置截止频率
    regval |= gain;               //设置增益
    WM8978_Write_Reg(22, regval); //R22,EQ5设置
}

/**
* @brief config wm8978 master or slaver mode
*
* @param master 0: slaver mode
*               1: master mode
*
* @return 0: false
*         1: true
*/
bool wm8978_ms_set(bool master)
{
    uint16_t regval = 0;

    if (master > 2) {
        return false;
    }
    regval = WM8978_ReadReg(6);

    GET_REG_VAL(regval, master, 0, 1);

    if (!WM8978_WriteReg(6, regval)) {
        return true;
    } else {
        return false;
    }

    return true;
}

/**
* @brief
*
* @param div
*
* @return 0: false
*         1: true
*/
bool wm8978_bclkdiv_set(uint8_t div)
{
    uint16_t regval = 0;

    if (div > 7) {
        return false;
    }

    regval = WM8978_ReadReg(6);

    GET_REG_VAL(regval, div, 2, 3);

    if (!WM8978_WriteReg(6, regval)) {
        return true;
    } else {
        return false;
    }
}

/**
* @brief
*
* @param div
*
* @return 0: false
*         1: true
*/
bool wm8978_mclkdiv_set(uint8_t div)
{
    uint16_t regval = 0;

    if (div > 7) {
        return false;
    }

    regval = WM8978_ReadReg(6);

    GET_REG_VAL(regval, div, 5, 3);

    if (!WM8978_WriteReg(6, regval)) {
        return true;
    } else {
        return false;
    }
}

/**
* @brief select clk source for all internal operation
*
* @param clksel 0: mclk
*				1: pll out
*
* @return 0: false
*         1: true
*/
bool wm8978_clkset(uint8_t clksel)
{
    uint16_t regval = 0;

    if (clksel > 2) {
        return false;
    }

    regval = WM8978_ReadReg(6);

    GET_REG_VAL(regval, clksel, 8, 1);

    if (!WM8978_WriteReg(6, regval)) {
        return true;
    } else {
        return false;
    }
}

bool wm8978_master_cfg(void)
{
    bool flag = true;

    /* master mode */
    flag &= wm8978_ms_set(1);

    /* bclk=mclk/2 */
    flag &= wm8978_bclkdiv_set(3);

    /* mclk/12 */
    flag &= wm8978_mclkdiv_set(3);

    /* clk source is mclk */
    flag &= wm8978_clkset(0);

    return flag;
}
