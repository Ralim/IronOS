/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      MMA8652FC.c
 Version :        S100 APP Ver 2.11
 Description:
 Author :         Celery
 Data:            2015/07/07
 History:
 2015/07/07   ͳһ������
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

static int IIC_RegWrite(u8 reg, u8 data);
static int IIC_RegRead(u8 reg);
static int Read_ZYXDr(void);

u16 gactive = 0, gShift = 0;
u8 gMmatxdata;

typedef struct {
	u8 hi;
	u8 lo;
} DRByte;
typedef struct {
	DRByte Byte;

} DR_Value;

DR_Value gX_value, gY_value, gZ_value;

/*******************************************************************************
 ������: Get_MmaActive
 ��������:��ȡ���ٶȴ���������״̬
 �������:NULL
 ���ز���:���ٶȴ�����״̬
 *******************************************************************************/
u16 Get_MmaActive(void) {
	return gactive;
}
/*******************************************************************************
 Function: Get_MmaActive
 Description:Returns if movement has occured (0==still,1==movement)
 *******************************************************************************/
u16 Get_MmaShift(void) {
	return gShift;
}
/*******************************************************************************
 ������: Get_MmaActive
 ��������:XXXXXXXXXXXXXXXXXXXXXX
 �������:XXXXXXXXXXXXXXXXXXXXXX
 ���ز���:XXXXXXXXXXXXXXXXXXXXXX
 *******************************************************************************/
void Set_MmaShift(u16 shift) {
	gShift = shift;
}

/*******************************************************************************
 ������: IIC_RegWrite
 ��������:��Reg��ַд��Data
 �������:Reg �����еĵ�ַ��Data����
 ���ز���: �ɹ���
 *******************************************************************************/
int IIC_RegWrite(u8 reg, u8 data) {
	u8 tx_data[20];

	tx_data[0] = reg;
	tx_data[1] = data;
	I2C_PageWrite(tx_data, 2, DEVICE_ADDR);
	return 1;
}
/*******************************************************************************
 ������: IIC_RegRead
 ��������:������Reg����������
 �������:Reg �����еĵ�ַ������gMmatxdata��
 ���ز���: �ɹ���
 *******************************************************************************/
int IIC_RegRead(u8 reg) {
	u8 tx_data[20];
	tx_data[0] = reg;

	I2C_PageRead(tx_data, 1, DEVICE_ADDR, reg);
	gMmatxdata = tx_data[0];
	return 1;
}
/*******************************************************************************
 ������: MMA865x_Standby
 ��������:�������״̬
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void MMA865x_Standby(void) {
	//Put the sensor into Standby Mode by clearing
	// the Active bit of the System Control 1 Register
	IIC_RegWrite(CTRL_REG1, 0); //(IIC_RegRead(CTRL_REG1) & ~ ACTIVE_MASK)
}
/*******************************************************************************
 ������: MMA865x_Active
 ��������:��������ģʽ
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void MMA865x_Active(void) {
	// Put the sensor into Active Mode by setting the
	// Active bit of the System Control 1 Register
	IIC_RegWrite(CTRL_REG1, ACTIVE_MASK); //(IIC_RegRead(CTRL_REG1) | ACTIVE_MASK)
}
/*******************************************************************************
 ������: IIC_RegRead
 ��������:������Reg����������
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void StartUp_Accelerated(void) {
	//------�������״̬-----------------------//
	MMA865x_Standby();
	//---- ���ò�����Χ4g----------------------//
	IIC_RegWrite(XYZ_DATA_CFG_REG, FULL_SCALE_8G); //(IIC_RegRead(XYZ_DATA_CFG_REG) & ~FS_MASK)
	//--- ������������ʱ��������100HZ------------------------------------//
	IIC_RegWrite(CTRL_REG1, DataRateValue); //IIC_RegRead(CTRL_REG1)|
	//----���ò�������ģʽ------------------------------------------------------//
	IIC_RegWrite(CTRL_REG2, 0);    //(IIC_RegRead(CTRL_REG2) & ~MODS_MASK)
	//---------��������ģʽ------------------------------------//
	MMA865x_Active();
}

/*******************************************************************************
 ������: Read_ZYXDr
 ��������:��ȡXYZ����
 �������:NULL
 ���ز���:x,y,z�ķ���
 *******************************************************************************/
int Read_ZYXDr(void) {
	u8 reg_flag;
	u8 ptr, i;
	u8 value[6];

	memset((u8*) &gX_value, 0, 6);
	//Poll the ZYXDR status bit and wait for it to set
	if (IIC_RegRead(STATUS_REG)) {
		reg_flag = gMmatxdata;
		if ((reg_flag & ZYXDR_BIT) != 0) {
			//Read 12/10-bit XYZ results using a 6 byte IIC access
			ptr = X_MSB_REG;
			for (i = 0; i < 6; i++) {
				if (IIC_RegRead(ptr++) == 0)
					break;
				value[i] = gMmatxdata;
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
 ������: Cheak_XYData
 ��������:���x��y�ķ���仯
 �������:ǰһxy��������xy����Ա�
 ���ز���:�Ƿ��ƶ�
 *******************************************************************************/
u16 Cheak_XYData(u16 x0, u16 y0, u16 x1, u16 y1) {
	u16 active = 0;
	gShift = 0;

	if ((x1 > (x0 + 16)) || (x1 < (x0 - 16)))
		active = 1;
	if ((y1 > (y0 + 16)) || (y1 < (y0 - 16)))
		active = 1;

	if ((x1 > (x0 + 32)) || (x1 < (x0 - 32)))
		gShift = 1;
	if ((y1 > (y0 + 32)) || (y1 < (y0 - 32)))
		gShift = 1;

	return active;
}
/*******************************************************************************
 ������: Update_X
 ��������:��������
 �������:����x
 ���ز���:NULL
 *******************************************************************************/
u16 Update_X(void) {
	u16 value, x;

	value = ((gX_value.Byte.hi << 8) | (gX_value.Byte.lo & 0xf0)) >> 4;
	if (gX_value.Byte.hi > 0x7f)
		x = (~value + 1) & 0xfff;
	else
		x = value & 0xfff;

	return x;
}
/*******************************************************************************
 ������: Update_Y
 ��������:��������
 �������:NULL
 ���ز���:����y
 *******************************************************************************/
u16 Update_Y(void) {
	u16 value, y;

	value = ((gY_value.Byte.hi << 8) | (gY_value.Byte.lo & 0xf0)) >> 4;
	if (gY_value.Byte.hi > 0x7f)
		y = (~value + 1) & 0xfff;
	else
		y = value & 0xfff;

	return y;
}
/*******************************************************************************
 ������: Update_z
 ��������:��������
 �������:NULL
 ���ز���:����z
 *******************************************************************************/
u16 Update_Z(void) {
	u16 value, z;

	value = ((gZ_value.Byte.hi << 8) | (gZ_value.Byte.lo & 0xf0)) >> 4;
	if (gZ_value.Byte.hi > 0x7f)
		z = (~value + 1) & 0xfff;
	else
		z = value & 0xfff;

	return z;
}
/*******************************************************************************
 ������: Check_Accelerated
 ��������:�����ٶȴ������Ƿ��ƶ�
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void Check_Accelerated(void) {
	static u16 x0 = 0, y0 = 0;
	u16 x1, y1;

	if (Read_ZYXDr()) { /*�����ݣ���������*/
		x1 = Update_X();
		y1 = Update_Y();
	} else
		x1 = x0;
	y1 = y0;
	gactive = Cheak_XYData(x0, y0, x1, y1);/*����Ƿ��ƶ�*/

	x0 = x1;
	y0 = y1;
}
/******************************** END OF FILE *********************************/
