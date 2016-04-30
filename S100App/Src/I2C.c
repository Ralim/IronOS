/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      I2C.c
Version :        S100 APP Ver 2.11   
Description:
Author :         Celery
Data:            2015/07/20
History:
2015/07/07   统一命名；
2015/07/21   I2C_DELAYTIME = 2;
*******************************************************************************/

#include "stm32f10x.h"
#include "I2C.h"
#include "Bios.h"
#include "oled.h"
#include "S100V0_1.h"

// --------- 仿真I2C接口相关定义-------- //
#define SDA          GPIO_Pin_7
#define	SCL		       GPIO_Pin_6

#define HIGH         1
#define LOW          0

#define SDA_VAL      GPIO_ReadInputDataBit(GPIOB, SDA)
#define SCL_VAL      GPIO_ReadInputDataBit(GPIOB, SCL)

#define I2C_MORE     1
#define I2C_LAST     0
#define I2C_TIMEOUT  255

#define FAILURE      0
#define SUCCEED      1
#define I2C_DELAYTIME 2

static void Sim_I2C_Set(u8 pin, u8 status);
static void Sim_I2C_Stop(void);
static void Sim_I2C_Start(void);
static u8   Sim_I2C_RD_Byte(u8 more);
static u8   Sim_I2C_WR_Byte(u8 data);

/*******************************************************************************
函数名: Delay_uS
函数作用: 软件延时
输入参数:us 
返回参数:NULL
*******************************************************************************/
void Delay_uS(u32 us)
{
    while(us) us--;
}
/*******************************************************************************
函数名: I2C_Configuration
函数作用: 配置I2C
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void I2C_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    GPIO_Init_OLED();
    /* PB6,7 SCL and SDA */
    GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* I2C1 configuration ------------------------------------------------------*/
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1         = DEVICEADDR_OLED;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed          = 100000;//100k
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

/*******************************************************************************
函数名: I2C_Configuration
函数作用: 配置I2C
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Sim_I2C_Set(u8 pin, u8 status)
{
    if(status == HIGH) GPIO_SetBits  (GPIOB, pin);
    if(status == LOW)  GPIO_ResetBits(GPIOB, pin);
}
/*******************************************************************************
函数名: Sim_I2C_Start
函数作用: 开始
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Sim_I2C_Start(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // I2C_PIN_EN();
    GPIO_InitStructure.GPIO_Pin   = SCL | SDA;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init       (GPIOB, &GPIO_InitStructure);

    Sim_I2C_Set(SCL, LOW);  // SCL low
    Sim_I2C_Set(SDA, HIGH); // SDA float, set as output high
    Sim_I2C_Set(SCL, HIGH); // SCL high
    Delay_uS(I2C_DELAYTIME);
    Sim_I2C_Set(SDA, LOW);  // SDA high->low while sclk high, S state occur...
    Delay_uS(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, LOW);  // SCL low
}
/*******************************************************************************
函数名: Sim_I2C_Stop
函数作用: 停止
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Sim_I2C_Stop(void)
{
    Sim_I2C_Set(SCL, LOW);  // SCL low
    Sim_I2C_Set(SDA, LOW);  // SDA low
    Delay_uS(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, HIGH); // SCL high
    Delay_uS(I2C_DELAYTIME);
    Sim_I2C_Set(SDA, HIGH); // SDA low->high while sclk high, P state occur
    Delay_uS(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, LOW);  //  SCL low
    Delay_uS(I2C_DELAYTIME);
}
/*******************************************************************************
函数名: Sim_I2C_WR_Byte
函数作用:向I2C写八位数据
输入参数:data要写入的数据
返回参数:NULL
*******************************************************************************/
u8 Sim_I2C_WR_Byte(u8 data)
{
    u8 i = 8;

    while(i--) { 								//send out a bit by sda line.
        Sim_I2C_Set(SCL, LOW);                  // sclk low
        if(data & 0x80) Sim_I2C_Set(SDA, HIGH); // send bit is 1
        else            Sim_I2C_Set(SDA, LOW);  // send bit is 0
        Delay_uS(I2C_DELAYTIME);
        Sim_I2C_Set(SCL, HIGH);                 // SCL high
        Delay_uS(I2C_DELAYTIME);
        data <<=1;								// left shift 1 bit, MSB send first.
    }
    Sim_I2C_Set(SCL, LOW);                    // SCL low
    Sim_I2C_Set(SDA, HIGH);                   // SDA set as input
    for(i=I2C_TIMEOUT; i!=0; i--) {			// wait for sda low	to receive ack
        Delay_uS(I2C_DELAYTIME);
        if (!SDA_VAL) {
            Sim_I2C_Set(SCL, HIGH);               // SCL high
            Delay_uS(I2C_DELAYTIME);
            Sim_I2C_Set(SCL, LOW);                // SCL_LOW();
            Delay_uS(I2C_DELAYTIME);
            return SUCCEED;
        }
    }
    return FAILURE;
}
/*******************************************************************************
函数名: Sim_I2C_RD_Byte
函数作用:向I2C读八位数据
输入参数:more
返回参数:读出的八位数据
*******************************************************************************/
u8 Sim_I2C_RD_Byte(u8 more)
{
    u8 i = 8, byte = 0;

    Sim_I2C_Set(SDA, HIGH);             // SDA set as input
    while(i--) {
        Sim_I2C_Set(SCL, LOW);            // SCL low
        Delay_uS(I2C_DELAYTIME);
        Sim_I2C_Set(SCL, HIGH);           // SCL high
        Delay_uS(I2C_DELAYTIME);
        byte <<=1;				          //recv a bit
        if (SDA_VAL) byte |= 0x01;
    }
    Sim_I2C_Set(SCL, LOW);
    if(!more)	Sim_I2C_Set(SDA, HIGH);   //last byte, send nack.
    else		Sim_I2C_Set(SDA, LOW);    //send ack
    Delay_uS(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, HIGH);             // SCL_HIGH();
    Delay_uS(I2C_DELAYTIME);
    Sim_I2C_Set(SCL, LOW);
    return byte;
}
/*******************************************************************************
函数名: I2C_PageWrite
函数作用:向 地址 deviceaddr 写入numbyte个字节的数据，写入的内容在pbuf
输入参数:pbuf 写入的内容，numbyte 为写入的字节数，deviceaddr为写入的地址
返回参数:NULL
*******************************************************************************/
void I2C_PageWrite(u8* pbuf, u8 numbyte,u8 deviceaddr )
{
    Sim_I2C_Start();
    Sim_I2C_WR_Byte(deviceaddr<<1);
    while(numbyte--) Sim_I2C_WR_Byte(*pbuf++);
    Sim_I2C_Stop();
}
/*******************************************************************************
函数名: I2C_PageRead
函数作用:向I2C读八位数据
输入参数: pbuf 读出来的存放地址 numbyte为读出来的字节数 
          deviceaddr设备地址 readaddr读取的内容地址
返回参数:读出的八位数据
*******************************************************************************/
void I2C_PageRead(u8* pbuf,  u8 numbyte,u8 deviceaddr, u8 readaddr)
{
    Sim_I2C_Start();
    Sim_I2C_WR_Byte(deviceaddr<<1);
    Sim_I2C_WR_Byte(readaddr);
    Sim_I2C_Start();
    Sim_I2C_WR_Byte((deviceaddr<<1)|1);

    while(numbyte--) {
        if(numbyte) *pbuf++ = Sim_I2C_RD_Byte(I2C_MORE);
        else	  	  *pbuf++ = Sim_I2C_RD_Byte(I2C_LAST);
    }
    Sim_I2C_Stop();
}
/******************************** END OF FILE *********************************/
