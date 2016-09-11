/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      I2C.c
Version :        S100 APP Ver 2.11   
Description:
Author :         Celery
Data:            2015/07/20
History:
2015/07/07   ͳһ������
2015/07/21   I2C_DELAYTIME = 2;
*******************************************************************************/

#include "stm32f10x.h"
#include "I2C.h"
#include "Bios.h"
#include "Oled.h"
#include "S100V0_1.h"

// --------- ����I2C�ӿ���ض���-------- //
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
������: Delay_uS
��������: �����ʱ
�������:us 
���ز���:NULL
*******************************************************************************/
void Delay_uS(u32 us)
{
    while(us) us--;
}
/*******************************************************************************
������: I2C_Configuration
��������: ����I2C
�������:NULL
���ز���:NULL
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
������: I2C_Configuration
��������: ����I2C
�������:NULL
���ز���:NULL
*******************************************************************************/
void Sim_I2C_Set(u8 pin, u8 status)
{
    if(status == HIGH) GPIO_SetBits  (GPIOB, pin);
    if(status == LOW)  GPIO_ResetBits(GPIOB, pin);
}
/*******************************************************************************
������: Sim_I2C_Start
��������: ��ʼ
�������:NULL
���ز���:NULL
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
������: Sim_I2C_Stop
��������: ֹͣ
�������:NULL
���ز���:NULL
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
������: Sim_I2C_WR_Byte
��������:��I2Cд��λ����
�������:dataҪд�������
���ز���:NULL
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
������: Sim_I2C_RD_Byte
��������:��I2C����λ����
�������:more
���ز���:�����İ�λ����
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
������: I2C_PageWrite
��������:�� ��ַ deviceaddr д��numbyte���ֽڵ����ݣ�д���������pbuf
�������:pbuf д������ݣ�numbyte Ϊд����ֽ�����deviceaddrΪд��ĵ�ַ
���ز���:NULL
*******************************************************************************/
void I2C_PageWrite(u8* pbuf, u8 numbyte,u8 deviceaddr )
{
    Sim_I2C_Start();
    Sim_I2C_WR_Byte(deviceaddr<<1);
    while(numbyte--) Sim_I2C_WR_Byte(*pbuf++);
    Sim_I2C_Stop();
}
/*******************************************************************************
������: I2C_PageRead
��������:��I2C����λ����
�������: pbuf �������Ĵ�ŵ�ַ numbyteΪ���������ֽ��� 
          deviceaddr�豸��ַ readaddr��ȡ�����ݵ�ַ
���ز���:�����İ�λ����
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
