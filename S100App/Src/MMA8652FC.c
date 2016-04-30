/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      MMA8652FC.c
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
#include "Bios.h"
#include "Oled.h"
#include "MMA8652FC.h"
#include "I2C.h"
#include "CTRL.h"
#include "UI.h"
//------------------------------------------------------------------//

static int IIC_RegWrite(u8 reg,u8 data);
static int IIC_RegRead(u8 reg);
static int Read_ZYXDr(void);

u16 gactive = 0,gShift = 0;
u8 gMmatxdata;

typedef struct {
    u8  hi;
    u8  lo;
} DRByte;
typedef struct {
    DRByte  Byte;

} DR_Value;

DR_Value gX_value,gY_value,gZ_value;

/*******************************************************************************
函数名: Get_MmaActive
函数作用:获取加速度传感器静动状态
输入参数:NULL
返回参数:加速度传感器状态
*******************************************************************************/
u16 Get_MmaActive(void)
{
    return gactive;
}
/*******************************************************************************
函数名: Get_MmaActive
函数作用:XXXXXXXXXXXXXXXXXXXXXX
输入参数:XXXXXXXXXXXXXXXXXXXXXX
返回参数:XXXXXXXXXXXXXXXXXXXXXX
*******************************************************************************/
u16 Get_MmaShift(void)
{
    return gShift;
}
/*******************************************************************************
函数名: Get_MmaActive
函数作用:XXXXXXXXXXXXXXXXXXXXXX
输入参数:XXXXXXXXXXXXXXXXXXXXXX
返回参数:XXXXXXXXXXXXXXXXXXXXXX
*******************************************************************************/
void Set_MmaShift(u16 shift)
{
    gShift = shift;
}

/*******************************************************************************
函数名: IIC_RegWrite
函数作用:向Reg地址写入Data
输入参数:Reg 设置中的地址，Data数据
返回参数: 成功；
*******************************************************************************/
int IIC_RegWrite(u8 reg,u8 data)
{
    u8 tx_data[20];
    
    tx_data[0]=reg;
    tx_data[1]=data;
    I2C_PageWrite(tx_data,2,DEVICE_ADDR);
    return 1;
}
/*******************************************************************************
函数名: IIC_RegRead
函数作用:从设置Reg处读出数据
输入参数:Reg 设置中的地址，存入gMmatxdata中
返回参数: 成功；
*******************************************************************************/
int IIC_RegRead(u8 reg)
{
    u8 tx_data[20];
    tx_data[0] = reg;

    I2C_PageRead(tx_data, 1, DEVICE_ADDR, reg);
    gMmatxdata = tx_data[0];
    return 1;
}
/*******************************************************************************
函数名: MMA865x_Standby
函数作用:进入待机状态
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void MMA865x_Standby(void)
{
    //Put the sensor into Standby Mode by clearing
    // the Active bit of the System Control 1 Register
    IIC_RegWrite(CTRL_REG1, 0); //(IIC_RegRead(CTRL_REG1) & ~ ACTIVE_MASK)
}
/*******************************************************************************
函数名: MMA865x_Active
函数作用:启动测量模式
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void MMA865x_Active(void)
{
    // Put the sensor into Active Mode by setting the
    // Active bit of the System Control 1 Register
    IIC_RegWrite(CTRL_REG1,ACTIVE_MASK );//(IIC_RegRead(CTRL_REG1) | ACTIVE_MASK)
}
/*******************************************************************************
函数名: IIC_RegRead
函数作用:从设置Reg处读出数据
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void StartUp_Accelerated(void)
{
    //------进入待机状态-----------------------//
    MMA865x_Standby();
    //---- 设置测量范围4g----------------------//
    IIC_RegWrite(XYZ_DATA_CFG_REG,FULL_SCALE_8G );//(IIC_RegRead(XYZ_DATA_CFG_REG) & ~FS_MASK)
    //--- 设置正常操作时数据速率100HZ------------------------------------//
    IIC_RegWrite(CTRL_REG1,  DataRateValue); //IIC_RegRead(CTRL_REG1)|
    //----设置采样工作模式------------------------------------------------------//
    IIC_RegWrite(CTRL_REG2, 0);//(IIC_RegRead(CTRL_REG2) & ~MODS_MASK)
    //---------启动测量模式------------------------------------//
    MMA865x_Active();
}

/*******************************************************************************
函数名: Read_ZYXDr
函数作用:读取XYZ方向
输入参数:NULL
返回参数:x,y,z的方向
*******************************************************************************/
int Read_ZYXDr(void)
{
    u8 reg_flag;
    u8 ptr,i;
    u8 value[6];

    memset((u8*)&gX_value,0,6);
    //Poll the ZYXDR status bit and wait for it to set
    if(IIC_RegRead(STATUS_REG)) {
        reg_flag = gMmatxdata ;
        if((reg_flag & ZYXDR_BIT) !=0) {
            //Read 12/10-bit XYZ results using a 6 byte IIC access
            ptr=X_MSB_REG;
            for(i=0; i<6; i++) {
                if(IIC_RegRead(ptr++)==0)break;
                value[i]=gMmatxdata ;
                //Copy and save each result as a 16-bit left-justified value
                gX_value.Byte.hi = value[0];
                gX_value.Byte.lo = value[1];
                gY_value.Byte.hi = value[2];
                gY_value.Byte.lo = value[3];
                gZ_value.Byte.hi = value[4];
                gZ_value.Byte.lo = value[5];
                return 1;
            }
        } else
            return 0;
    }
    return 0;
}
/*******************************************************************************
函数名: Cheak_XYData
函数作用:检查x，y的方向变化
输入参数:前一xy，跟现在xy坐标对比
返回参数:是否移动
*******************************************************************************/
u16 Cheak_XYData(u16 x0,u16 y0,u16 x1,u16 y1)
{
    u16 active = 0;
    gShift = 0;

    if((x1 > (x0 + 16)) || (x1 < (x0 - 16)))  active = 1;
    if((y1 > (y0 + 16)) || (y1 < (y0 - 16)))  active = 1;

    if((x1 > (x0 + 32)) || (x1 < (x0 - 32)))  gShift = 1;
    if((y1 > (y0 + 32)) || (y1 < (y0 - 32)))  gShift = 1;

    return active;
}
/*******************************************************************************
函数名: Update_X
函数作用:更新数据
输入参数:返回x
返回参数:NULL
*******************************************************************************/
u16 Update_X(void)
{
    u16 value,x;

    value = ((gX_value.Byte.hi<<8) | (gX_value.Byte.lo & 0xf0 ))>>4;
    if(gX_value.Byte.hi>0x7f)    x = (~value+1) & 0xfff;
    else                        x = value & 0xfff;

    return x;
}
/*******************************************************************************
函数名: Update_Y
函数作用:更新数据
输入参数:NULL
返回参数:返回y
*******************************************************************************/
u16 Update_Y(void)
{
    u16 value,y;

    value = ((gY_value.Byte.hi<<8) | (gY_value.Byte.lo & 0xf0 ))>>4;
    if(gY_value.Byte.hi>0x7f)    y = (~value+1) & 0xfff;
    else                          y = value & 0xfff;

    return y;
}
/*******************************************************************************
函数名: Update_z
函数作用:更新数据
输入参数:NULL
返回参数:返回z
*******************************************************************************/
u16 Update_Z(void)
{
    u16 value,z;

    value = ((gZ_value.Byte.hi<<8) | (gZ_value.Byte.lo & 0xf0 ))>>4;
    if(gZ_value.Byte.hi>0x7f)     z = (~value+1) & 0xfff;
    else                          z = value & 0xfff;

    return z;
}
/*******************************************************************************
函数名: Check_Accelerated
函数作用:检查加速度传感器是否移动
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Check_Accelerated(void)
{
    static u16 x0 = 0,y0 = 0;
    u16 x1,y1;

    if(Read_ZYXDr()) { /*读数据，更新数据*/
        x1 = Update_X();
        y1 = Update_Y();
    } else  x1 = x0;
    y1 = y0;
    gactive = Cheak_XYData(x0,y0,x1,y1);/*检查是否移动*/

    x0 = x1;
    y0 = y1;
}
/******************************** END OF FILE *********************************/
