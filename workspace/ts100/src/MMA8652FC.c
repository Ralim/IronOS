/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      MMA8652FC.c
 Version :        S100 APP Ver 2.11
 Description:
 Author :         Celery
 Data:            2015/07/07
 History:
 2016/09/13	Ben V. Brown - English comments and fixing a few errors
 2015/07/07   ͳһ������
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "APP_Version.h"
#include "Bios.h"
#include "Oled.h"
#include "MMA8652FC.h"
#include "I2C.h"

//------------------------------------------------------------------//

static int IIC_RegWrite(u8 reg, u8 data);
static int IIC_RegRead(u8 reg);
static int Read_ZYXDr(void);

u16 gactive = 0, gShift = 0;
u8 gMmatxdata;

typedef struct {
	u8 hi;
	u8 lo;

} DR_Value;

DR_Value gX_value, gY_value, gZ_value;

/*******************************************************************************
 Function:
 Description:Returns if the unit is actively being moved
 Output: if the unit is active or not.
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
 Function: Set_MmaShift
 Description: Set the Shift Value
 Input: shift value
 *******************************************************************************/
void Set_MmaShift(u16 shift) {
	gShift = shift;
}

/*******************************************************************************
 Function:IIC_RegWrite
 Description:Writes a value to a register
 Input:the register, the data
 Output: 1 if the write succeeded
 *******************************************************************************/
int IIC_RegWrite(u8 reg, u8 data) {
	u8 tx_data[20];

	tx_data[0] = reg;
	tx_data[1] = data;
	I2C_PageWrite(tx_data, 2, DEVICE_ADDR);
	return 1;
}
/*******************************************************************************
 Function:IIC_RegRead
 Description: Reads a register from I2C, using a single byte addressing scheme
 Inputs: uint8_t register to read
 Output: 1 if the read worked.
 *******************************************************************************/
int IIC_RegRead(u8 reg) {
	u8 tx_data[20];
	tx_data[0] = reg;

	I2C_PageRead(tx_data, 1, DEVICE_ADDR, reg);
	gMmatxdata = tx_data[0];
	return 1;
}
/*******************************************************************************
 Function: MMA865x_Standby
 Description: Put the MMA865 into standby mode
 *******************************************************************************/
void MMA865x_Standby(void) {
	//Put the sensor into Standby Mode by clearing
	// the Active bit of the System Control 1 Register
	IIC_RegWrite(CTRL_REG1, 0); //(IIC_RegRead(CTRL_REG1) & ~ ACTIVE_MASK)
}
/*******************************************************************************
 Function: MMA865x_Active
 Description: Put the MMA865 into active mode
 *******************************************************************************/
void MMA865x_Active(void) {
	// Put the sensor into Active Mode by setting the
	// Active bit of the System Control 1 Register
	IIC_RegWrite(CTRL_REG1, ACTIVE_MASK); //(IIC_RegRead(CTRL_REG1) | ACTIVE_MASK)
}
/*******************************************************************************
 Function: IIC_RegRead
 Description:Setup the MMA865x IC settings
 *******************************************************************************/
void StartUp_Accelerated(void) {
	//Put the unit into standby state so we can edit its configuration registers
	MMA865x_Standby();
	//Set the unit to full scale measurement
	IIC_RegWrite(XYZ_DATA_CFG_REG, FULL_SCALE_8G); //(IIC_RegRead(XYZ_DATA_CFG_REG) & ~FS_MASK)
	//Set the unit to the required update rate (eg 100Hz)
	IIC_RegWrite(CTRL_REG1, DataRateValue); //IIC_RegRead(CTRL_REG1)|

	IIC_RegWrite(CTRL_REG2, 0); //Normal mode

	//Change the unit back to active mode to exit setup and start the readings
	MMA865x_Active();
}

/*******************************************************************************
 Function: Read_ZYXDr
 Description:
 Output: 1 if new data, 0 if not
 *******************************************************************************/
int Read_ZYXDr(void) {
	u8 reg_flag;
	u8 ptr, i;
	u8 value[6] = { 0, 0, 0, 0, 0, 0 };
	//Poll the ZYXDR status bit and wait for it to set
	if (IIC_RegRead(STATUS_REG)) {	//check we can read the status
		reg_flag = gMmatxdata;
		if ((reg_flag & ZYXDR_BIT) != 0) {	//if new measurement
			//Read 12/10-bit XYZ results using a 6 byte IIC access
			ptr = X_MSB_REG;
			for (i = 0; i < 6; i++) {
				if (IIC_RegRead(ptr++) == 0)
					break;

				value[i] = gMmatxdata;
				//Copy and save each result as a 16-bit left-justified value
				gX_value.hi = value[0];
				gX_value.lo = value[1];
				gY_value.hi = value[2];
				gY_value.lo = value[3];
				gZ_value.hi = value[4];
				gZ_value.lo = value[5];
				return 1;
			}
		} else
			return 0;
	}
	return 0;
}
/*******************************************************************************
 Function: Cheak_XYData
 Description: Check the input X,Y for a large enough acceleration to wake the unit
 Inputs:x0,y0,x1,y1 to check
 Output: if the unit is active
 *******************************************************************************/
u16 Cheak_XYData(u16 x0, u16 y0, u16 x1, u16 y1) {
	u16 active = 0;
	gShift = 0;

	if ((x1 > (x0 + 32)) || (x1 < (x0 - 32)))
		gShift = 1;
	if ((y1 > (y0 + 32)) || (y1 < (y0 - 32)))
		gShift = 1;

	if ((x1 > (x0 + 16)) || (x1 < (x0 - 16)))
		active = 1;
	if ((y1 > (y0 + 16)) || (y1 < (y0 - 16)))
		active = 1;

	return active;
}
/*******************************************************************************
 Function: Update_X
 Description: Converts the read value for x into an actual properly located value
 Output: X
 *******************************************************************************/
u16 Update_X(void) {
	u16 value, x;

	value = ((gX_value.hi << 8) | (gX_value.lo & 0xf0)) >> 4;
	if (gX_value.hi > 0x7f)
		x = (~value + 1) & 0xfff;
	else
		x = value & 0xfff;

	return x;
}
/*******************************************************************************
 Function: Update_Y
 Description: Converts the read value for y into an actual properly located value
 Output: Y
 *******************************************************************************/
u16 Update_Y(void) {
	u16 value, y;

	value = ((gY_value.hi << 8) | (gY_value.lo & 0xf0)) >> 4;
	if (gY_value.hi > 0x7f)
		y = (~value + 1) & 0xfff;
	else
		y = value & 0xfff;

	return y;
}
/*******************************************************************************
 Function: Update_Z
 Description: Converts the read value for z into an actual properly located value
 Output: Z
 *******************************************************************************/
u16 Update_Z(void) {
	u16 value, z;

	value = ((gZ_value.hi << 8) | (gZ_value.lo & 0xf0)) >> 4;
	if (gZ_value.hi > 0x7f)
		z = (~value + 1) & 0xfff;
	else
		z = value & 0xfff;

	return z;
}
/*******************************************************************************
 Function: Check_Accelerated
 Description:Check if the unit has moved
 *******************************************************************************/
void Check_Accelerated(void) {
	static u16 x0 = 0, y0 = 0;
	u16 x1, y1;

	if (Read_ZYXDr()) { //Read the new values from the accelerometer
		x1 = Update_X(); //convert the values into usable form
		y1 = Update_Y();
	} else {
		x1 = x0;
		y1 = y0; //use old values
		gactive = 0;
		return;
	}

	gactive = Cheak_XYData(x0, y0, x1, y1); //gactive == If the unit is moving or not

	x0 = x1;
	y0 = y1;
}
/******************************** END OF FILE *********************************/
