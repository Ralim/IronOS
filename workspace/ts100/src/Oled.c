/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      Oled.c
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
#include "Oled.h"
#include "Bios.h"
#include "I2C.h"

#include "Font.h"

u8 gOled_param[46] = { 0x80, 0xAE, 0x80, 0xD5, 0x80, 0x52, 0x80, 0xA8, 0x80,
		0x0f, 0x80, 0xC0, 0x80, 0xD3, 0x80, 0x00, 0x80, 0x40, 0x80, 0xA0, 0x80,
		0x8D, 0x80, 0x14, 0x80, 0xDA, 0x80, 0x02, 0x80, 0x81, 0x80, 0x33, 0x80,
		0xD9, 0x80, 0xF1, 0x80, 0xDB, 0x80, 0x30, 0x80, 0xA4, 0x80, 0XA6, 0x80,
		0xAF };

/*******************************************************************************

 *******************************************************************************/
void Sc_Pt(u8 Co) {
	u8 pt[4] = { 0x80, 0x81, 0x80, Co };

	I2C_PageWrite(pt, 4, DEVICEADDR_OLED);
}
/*******************************************************************************
 Function: Oled_DisplayOn
 Description:Turn on the Oled display
 *******************************************************************************/
void Oled_DisplayOn(void) {
	u8 data[6] = { 0x80, 0X8D, 0x80, 0X14, 0x80, 0XAF };

	I2C_PageWrite(data, 6, DEVICEADDR_OLED);
}
/*******************************************************************************
 Function: Oled_DisplayOff
 Description:Turn off the Oled display
 *******************************************************************************/
void Oled_DisplayOff(void) {
	u8 data[6] = { 0x80, 0X8D, 0x80, 0X10, 0x80, 0XAE };

	I2C_PageWrite(data, 6, DEVICEADDR_OLED);
}

/*******************************************************************************
 Function: Data_Command
 Description: write a command to the Oled display
 Input: number of bytes to write, array to write
 Output:
 *******************************************************************************/
u8* Data_Command(u8 wide, u8* ptr) {
	int i;
	u8 tx_data[128];
	//here are are inserting the data write command at the beginning
	tx_data[0] = 0x40;
	wide += 1;
	for (i = 1; i < wide; i++) //Loop through the array of data
		tx_data[i] = *ptr++;
	I2C_PageWrite(tx_data, wide, DEVICEADDR_OLED); //write out the buffer
	return ptr;
}
/*******************************************************************************
 Function:Set_ShowPos
 Description:Set the current position in GRAM that we are drawing to
 Input:x,y co-ordinates
 *******************************************************************************/
void Set_ShowPos(u8 x, u8 y) {
	u8 pos_param[8] = { 0x80, 0xB0, 0x80, 0x21, 0x80, 0x20, 0x80, 0x7F };

	pos_param[5] = x + 32;
	pos_param[1] += y;

	I2C_PageWrite(pos_param, 8, DEVICEADDR_OLED);
}

/*******************************************************************************
 Function:Oled_DrawArea
 Description:
 Inputs:(x,y) start point, (width,height) of enclosing rect, pointer to data
 Output: last byte written out
 *******************************************************************************/
u8* Oled_DrawArea(u8 x0, u8 y0, u8 wide, u8 high, u8* ptr) {
	u8 m, n, y;

	n = y0 + high;
	if (y0 % 8 == 0)
		m = y0 / 8;
	else
		m = y0 / 8 + 1;

	if (n % 8 == 0)
		y = n / 8;
	else
		y = n / 8 + 1;

	for (; m < y; m++) {
		Set_ShowPos(x0, m);
		ptr = Data_Command(wide, ptr);
	}
	return ptr;
}

/*******************************************************************************
 Function:Clean_Char
 Description:Overwries a square to off, used to overwrite a char
 Inputs:(k) input X position char starts at, (wide) how many pixels wide the char is
 *******************************************************************************/
void Clean_Char(int k, u8 wide) {
	int i;
	u8 tx_data[128];

	memset(&tx_data[0], 0, wide);
	for (i = 0; i < 2; i++) {
		Oled_DrawArea(k, i * 8, wide, 8, tx_data);
	}
}
/*******************************************************************************
 Function:GPIO_Init_OLED
 Description:Init the outputs as needed for the OLED (in this case the RST line)
 *******************************************************************************/
void GPIO_Init_OLED(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
 Function: Init_Oled
 Description: Initalizes the Oled screen
 *******************************************************************************/
void Init_Oled(void) {
	u8 param_len;

	OLED_RST();
	delayMs(2); //reset the oled
	OLED_ACT();
	delayMs(2);
	param_len = 46;
	I2C_PageWrite((u8 *) gOled_param, param_len, DEVICEADDR_OLED);
}

/*******************************************************************************
 Function:Clear_Screen
 Description:Clear the entire screen to off (black)
 *******************************************************************************/
void Clear_Screen(void) {
	u8 tx_data[128];
	memset(&tx_data[0], 0, 128);
	for (u8 i = 0; i < 2; i++) {
		Oled_DrawArea(0, i * 8, 128, 8, tx_data);
	}
}
void OLED_DrawString(char* string, uint8_t length) {
	for (uint8_t i = 0; i < length; i++) {
		OLED_DrawChar(string[i], i * 14);
	}
}
void OLED_DrawChar(char c, uint8_t x) {
	if ((x) > (128 - 14))
		return; //Rudimentary clipping to not draw off screen
	u8* ptr;
	ptr = (u8*) FONT;
	if (c >= 'A' && c <= 'Z') {
		ptr += (c - 'A' + 10) * (14 * 2); //alpha is ofset 10 chars into the array
	} else if (c >= '0' && c <= '9')
		ptr += (c - '0') * (14 * 2);
	else if (c < 10)
		ptr += (c) * (14 * 2);
	else if (c == ' ') {
		//blank on space bar
		ptr += (36) * (14 * 2);
	}

	Oled_DrawArea(x, 0, 14, 16, (u8*) ptr);
}
/*
 * Draw a 2 digit number to the display
 * */
void OLED_DrawTwoNumber(uint8_t in, uint8_t x) {
	OLED_DrawChar(in / 10, x);
	OLED_DrawChar(in % 10, x + 14);
}
void OLED_DrawThreeNumber(uint16_t in, uint8_t x) {

	OLED_DrawChar((in / 100)%10, x);
	OLED_DrawChar((in / 10)%10, x + 14);
	OLED_DrawChar(in % 10, x + 28);
}
void OLED_DrawFourNumber(uint16_t in, uint8_t x) {

	OLED_DrawChar((in / 1000)%10, x);
	OLED_DrawChar((in / 100)%10, x+14);
	OLED_DrawChar((in / 10)%10, x + 28);
	OLED_DrawChar(in % 10, x + 32);
}

/******************************** END OF FILE *********************************/

