/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      Oled.c
Version :        S100 APP Ver 2.11
Description:
Author :         Celery
Data:            2015/07/07
History:
2015/07/07   统一命名；
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "APP_Version.h"
#include "Oled.h"
#include "Bios.h"
#include "I2C.h"
#include "HARDWARE.h"
#include "DISK.h"
#include "UI.h"


const u8 Mini[] = {/*12*16*/
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFE,
    0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0xC0,0xE0,0xF0,0xF8,0xFC,0xFE,0xFE,0xFE,0xFC,0x78,
    0x30,0x80,0xC0,0xC0,0x00,0x18,0x9C,0xCE,0xE6,0xF0,0xF8,0xFC,0xFE,0xFE,0xFC,0xF8,
    0xF0,0xE0,0xC0,0xE0,0xF0,0xF8,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x0F,0x07,0x03,0x01,0x03,0x07,0x0F,0x1F,
    0x3F,0x7F,0x7F,0x3F,0x1F,0x0F,0x07,0x03,0x01,0x03,0x07,0x07,0x03,0x19,0x1C,0x0E,
    0x67,0x73,0x39,0x1C,0x0E,0x07,0x03,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0x7F,0x3F,
    0x1F,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"D:\yinyongqin\烙铁界面\LOGOIN.BMP.bmp",0*/
};
const u8 Seeed[] = {/*12*16*/
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x80,0x00,0x00,0xC0,0xF8,0x38,0x04,0x00,
    0x04,0x38,0xF8,0xC0,0x00,0x00,0x80,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x30,
    0x10,0x10,0x10,0x10,0x10,0x20,0x00,0x00,0x80,0xC0,0x60,0x30,0x10,0x10,0x10,0x30,
    0x60,0xC0,0x80,0x00,0x00,0x80,0xC0,0x60,0x30,0x10,0x10,0x10,0x30,0x60,0xC0,0x80,
    0x00,0x80,0xC0,0x60,0x30,0x10,0x10,0x10,0x30,0x60,0xC0,0x80,0x00,0x00,0x80,0xC0,
    0x40,0x20,0x20,0x20,0x20,0x40,0xC0,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0F,0x1C,0x38,0x23,0x6F,0x5C,0x00,0x00,
    0x00,0x5C,0x6F,0x23,0x38,0x1C,0x0F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x23,
    0x42,0x42,0x42,0x42,0x42,0x22,0x1C,0x00,0x07,0x1E,0x32,0x22,0x42,0x42,0x42,0x42,
    0x62,0x32,0x03,0x00,0x00,0x07,0x1E,0x32,0x22,0x42,0x42,0x42,0x42,0x22,0x32,0x03,
    0x00,0x07,0x1E,0x32,0x22,0x42,0x42,0x42,0x42,0x62,0x32,0x03,0x00,0x00,0x0F,0x18,
    0x10,0x20,0x20,0x20,0x20,0x10,0x18,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"D:\yinyongqin\烙铁界面\LOGOIN.ddMP.bmp",0*/
};
#ifdef SSD1316
u8 gOled_param[50] = {  0x80,0xAE,0x80,0x00,0x80,0x10,0x80,0x40,0x80,0xB0,0x80,
                        0x81,0x80,0xFF,0x80,0xA0,0x80,0xA6,0x80,0xA8,0x80,0x1F,
                        0x80,0xC8,0x80,0xD3,0x80,0x00,0x80,0xD5,0x80,0x80,0x80,
                        0xD9,0x80,0x22,0x80,0xDA,0x80,0x12,0x80,0xDB,0x80,0x40,
                        0x80,0x8D,0x80,0x14,0x80,0xAF,
                     };
#else
u8 gOled_param[46] = {  0x80,0xAE,0x80,0xD5,0x80,0x52,0x80,0xA8,0x80,0x0f,0x80,
                        0xC0,0x80,0xD3,0x80,0x00,0x80,0x40,0x80,0xA0,0x80,0x8D,
                        0x80,0x14,0x80,0xDA,0x80,0x02,0x80,0x81,0x80,0x33,0x80,
                        0xD9,0x80,0xF1,0x80,0xDB,0x80,0x30,0x80,0xA4,0x80,0XA6,
                        0x80,0xAF
                     };
#endif
/*******************************************************************************
函数名: Sc_Pt
函数作用:屏幕保护改变屏幕对比度
输入参数:Co屏幕对比度参数
返回参数:NULL
*******************************************************************************/
void Sc_Pt(u8 Co) //屏幕保护
{
    u8 pt[4] = {0x80,0x81,0x80,Co};

    I2C_PageWrite(pt,4,DEVICEADDR_OLED);
}
/*******************************************************************************
函数名: Oled_DisplayOn
函数作用:打开OLED显示
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Oled_DisplayOn(void)
{
    u8 data[6] = {0x80,0X8D,0x80,0X14,0x80,0XAF};

    I2C_PageWrite(data,6,DEVICEADDR_OLED);
}
/*******************************************************************************
函数名: Oled_DisplayOff
函数作用:关闭OLED显示
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Oled_DisplayOff(void)
{
    u8 data[6] = {0x80,0X8D,0x80,0X10,0x80,0XAE};

    I2C_PageWrite(data,6,DEVICEADDR_OLED);
}

/*******************************************************************************
函数名: Data_Command
函数作用:将显示的宽度以及显示的输入传入OLED
输入参数:wide 宽,ptr 库指针
返回参数:下一库指针
*******************************************************************************/
u8* Data_Command(u8 wide,u8* ptr)
{
    int i;
    u8 tx_data[128];

    tx_data[0] = 0x40;
    wide += 1;
    for(i = 1; i < wide; i++)  tx_data[i] = *ptr++;
    I2C_PageWrite(tx_data,wide,DEVICEADDR_OLED);
    return ptr;
}
/*******************************************************************************
函数名: Set_ShowPos
函数作用:要显示内容的位置
输入参数:x:横坐标,y:纵坐标(0,8,16,24)
返回参数:NULL
*******************************************************************************/
void Set_ShowPos(u8 x,u8 y)
{
    u8 pos_param[8] = { 0x80,0xB0,0x80,0x21,0x80,0x20,0x80,0x7F };

    pos_param[5] = x + 32;
    pos_param[1] += y;

    I2C_PageWrite(pos_param,8,DEVICEADDR_OLED);
}

/*******************************************************************************
函数名: Oled_DrawArea
函数作用:显示一个区域
输入参数: x0:起始横坐标
          y0:起始纵坐标(0,8,16,24)
          wide:显示内容宽度
          high:显示内容高度
          ptr:显示的内容库指针
返回参数:下一库指针
*******************************************************************************/
u8* Oled_DrawArea(u8 x0,u8 y0,u8 wide, u8 high,u8* ptr)
{
    u8 m,n,y;

    n = y0 + high;
    if(y0 % 8 == 0) m = y0 / 8;
    else            m = y0 / 8 + 1;

    if(n % 8 == 0)  y = n / 8;
    else            y = n / 8 + 1;

    for(; m < y; m++) {
        Set_ShowPos(x0,m);
        ptr = Data_Command(wide,ptr);
    }
    return ptr;
}

/*******************************************************************************
函数名: Clean_Char
函数作用:清除长度wide位置为k处的屏幕
输入参数:    清除位置wide为清除宽度
返回参数:NULL
*******************************************************************************/
void Clean_Char(int k,u8 wide)
{
    int i;
    u8 tx_data[128];

    memset(&tx_data[0],0,wide);
    for(i = 0; i < 2; i++) {
        Oled_DrawArea(k,i * 8,wide,8,tx_data);
    }
}
/*******************************************************************************
函数名: GPIO_Init_OLED
函数作用:初始化LED端口
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void GPIO_Init_OLED(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = OLED_RST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
函数名: Init_Oled
函数作用:初始化LED设置
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Init_Oled(void)
{
    u8 param_len;

    OLED_RST();
    Delay_Ms(2);
    OLED_ACT();
    Delay_Ms(2);

#ifdef SSD1316
    param_len = 50;
#else
    param_len = 46;
#endif

    I2C_PageWrite((u8 *)gOled_param,param_len,DEVICEADDR_OLED);
}

/*******************************************************************************
函数名: Clear_Screen
函数作用:清屏
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Clear_Screen(void)
{
    u8 tx_data[128];
    u8 i,wd;

#ifdef SSD1316
    wd = 32;
#else
    wd = 16;
#endif

    memset(&tx_data[0],0,128);
    for(i = 0; i < wd / 8; i++) {
        Oled_DrawArea(0,i * 8,128,8,tx_data);
    }
}
/*******************************************************************************
函数名: Display_BG
函数作用:搜索并显示名为LOGOIN.BMP的图片
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void  Display_BG(void)
{
    u8 i,j,k,m,n,p,ch,Palette = 1;
    u8* bmpfile;
    u16 filelen;
    u16* Root_addr = 0;
    u8 *ptr = 0;
    u8 g_au8TxData[128];

    if(bmpfile = SearchFile("LOGOIN  BMP",&filelen,Root_addr)) {
        if(bmpfile[0] == 'B' && bmpfile[1] == 'M' ) {
            if((bmpfile[0x36] == 0xFF) && (bmpfile[0x37] == 0xFF) && (bmpfile[0x38] == 0xFF)) {
                Palette = 0;
            }
            memset(&g_au8TxData[1],0,127);
            p = 0x1;
            for(i = 15; i >= 8; i--) {
                m = 0;
                for(j = 0; j < 12; j++) {
                    ch = bmpfile[0x3E + i * 12 + j];
                    n = 0x80;
                    for(k = 0; k < 8; k++) {
                        if(Palette) {
                            if(!(ch & n)) g_au8TxData[m + 1] |= p;
                        } else {
                            if((ch & n))  g_au8TxData[m + 1] |= p;
                        }
                        m++;
                        n >>= 1;
                    }
                }
                p <<= 1;
            }
            Oled_DrawArea(0,0,96,8,g_au8TxData);
            Clear_Watchdog();

            memset(&g_au8TxData[1],0,127);
            p = 0x1;
            for(i = 0; i < 8; i++) {
                m = 0;
                for(j = 0; j < 12; j++) {
                    ch = bmpfile[0x3E + (7 - i) * 12 + j];
                    n = 0x80;
                    for(k = 0; k < 8; k++) {
                        if(Palette) {
                            if(!(ch & n)) g_au8TxData[m + 1] |= p;
                        } else {
                            if((ch & n))  g_au8TxData[m + 1] |= p;
                        }
                        m++;
                        n >>= 1;
                    }
                }
                p <<= 1;
            }
            Oled_DrawArea(0,8,96,8,g_au8TxData);
            Delay_Ms(1000);
            Clear_Watchdog();
            return;
        }
    } else {
#ifdef MFTSEEED
        ptr = (u8*)Seeed;
#else
        ptr = (u8*)Mini;
#endif
        Oled_DrawArea(0,0,96,16,ptr);
        Delay_Ms(1000);
        Clear_Watchdog();
    }
}

/******************************** END OF FILE *********************************/

