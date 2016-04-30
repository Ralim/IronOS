/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      MMA8652FC.h
Version :        S100 APP Ver 2.11   
Description:
Author :         Celery
Data:            2015/07/07
History:
2015/07/07   统一命名；
*******************************************************************************/
#ifndef __MMA8652FC__H
#define __MMA8652FC__H

//--------------MMA8652 定义部分----------------------------------------------//

#define DEVICE_ADDR     0X1D        
//--------------MMA8652 寄存器定义-------------------------------------------//
#define STATUS_REG    0x00
#define X_MSB_REG     0X01
#define X_LSB_REG     0X02
#define Y_MSB_REG     0X03
#define Y_LSB_REG     0X04
#define Z_MSB_REG     0X05
#define Z_LSB_REG     0X06

#define TRIG_CFG      0X0A
#define SYSMOD        0X0B
#define INT_SOURCE    0X0C
#define DEVICE_ID     0X0D

#define XYZ_DATA_CFG_REG      0X0E


#define CTRL_REG1     0X2A      //
#define CTRL_REG2     0X2B      //System Control 2 register
#define CTRL_REG3     0X2C      //
#define CTRL_REG4     0X2D      //Interrupt Enable register
#define CTRL_REG5     0X2E      //

//-----STATUS_REG(0X00)-----Bit Define----------------------------------------//
#define ZYXDR_BIT     0X08
//----XYZ_DATA_CFG_REG(0xE)-Bit Define----------------------------------------//
#define FS_MASK       0x03    
#define FULL_SCALE_2G 0x00      //2g=0x0,4g=0x1,8g=0x2
#define FULL_SCALE_4G 0x01
#define FULL_SCALE_8G 0x02
//---------CTRL_REG1(0X2A)Bit Define------------------------------------------//
#define ACTIVE_MASK   1<<0       //bit0 
#define DR_MASK       0x38      //bit D5,D4,D3
#define FHZ800        0x0       //800hz
#define FHZ400        0x1       //400hz
#define FHZ200        0x2       //200hz
#define FHZ100        0x3       //100hz
#define FHZ50         0x4       //50hz
#define FHZ2          0x5       //12.5hz
#define FHZ1          0x6       //6.25hz
#define FHZ0          0x7       //1.563hz
#define DataRateValue FHZ100
//---------CTRL_REG2(0X2B)Bit Define------------------------------------------// 
#define MODS_MASK     0x03     //Oversampling Mode 4 
#define Normal_Mode   0x0      //Normal=0,Low Noise Low Power MODS=1,
                               //HI RESOLUTION=2,LOW POWER MODS = 11 
//----CTRL_REG4---Interrupt Enable BIT ---------------------------------------//   
//0 interrupt is disabled (default) 
//1 interrupt is enabled     
#define INT_EN_ASLP     1<<7    //Auto-SLEEP/WAKE Interrupt Enable 
#define INT_EN_FIFO     1<<6    //FIFO Interrupt Enable
#define INT_EN_TRANS    1<<5    //Transient Interrupt Enable
#define INT_EN_LNDPRT   1<<4    //Orientation(Landscape/Portrait)Interrupt Enable
#define INT_EN_PULSE    1<<3    //Pulse Detection Interrupt Enable
#define INT_EN_FF_MT    1<<2    //Freefall/Motion Interrupt Enable
#define INT_EN_DRDY     1<<0    //Data Ready Interrupt Enable

u16 Get_MmaShift(void);
void Set_MmaShift(u16 shift);
u16 Get_MmaActive(void);
void MMA865x_Standby(void);
void MMA865x_Active(void);
u16 Cheak_XYData(u16 x0,u16 y0,u16 x1,u16 y1);
u16 Update_X(void);
u16 Update_Y(void);
u16 Update_Z(void);
void Check_Accelerated(void);
void StartUp_Accelerated(void);
#endif
/******************************** END OF FILE *********************************/
