/*
 *
 * OLED.c
 * Functions for working with the oled screen.
 * Writes to the screen using I2C
 */

#include <stdio.h>
#include <string.h>

#include "Oled.h"
#include "Bios.h"
#include "I2C.h"

#include "Font.h"
u8 displayOffset = 32;
/*Setup params for the OLED screen*/
/*http://www.displayfuture.com/Display/datasheet/controller/SSD1307.pdf*/
/*All commands are prefixed with 0x80*/
u8 OLED_Setup_Array[46] = { 0x80, 0xAE,/*Display off*/
0x80, 0xD5,/*Set display clock divide ratio / osc freq*/
0x80, 0b01010001,/**/
0x80, 0xA8,/*Set Multiplex Ratio*/
0x80, 16,  /*16 == max brightness,39==dimmest*/
0x80, 0xC0,/*Set COM Scan direction*/
0x80, 0xD3,/*Set Display offset*/
0x80, 0x00,/*0 Offset*/
0x80, 0x40,/*Set Display start line to 0*/
0x80, 0xA0,/*Set Segment remap to normal*/
0x80, 0x8D,/*Unknown*/
0x80, 0x14,/**/
0x80, 0xDA,/*Set VCOM Pins hardware config*/
0x80, 0x02,/*Combination 2*/
0x80, 0x81,/*Contrast*/
0x80, 0x33,/*51*/
0x80, 0xD9,/*Set pre-charge period*/
0x80, 0xF1,/**/
0x80, 0xDB,/*Adjust VCOMH regulator ouput*/
0x80, 0x30,/**/
0x80, 0xA4,/*Enable the display GDDR*/
0x80, 0XA6,/*Normal display*/
0x80, 0xAF /*Dispaly on*/
};

/*
 Function: Oled_DisplayOn
 Description:Turn on the Oled display
 */
void Oled_DisplayOn(void) {
	u8 data[6] = { 0x80, 0X8D, 0x80, 0X14, 0x80, 0XAF };

	I2C_PageWrite(data, 6, DEVICEADDR_OLED);
}
/*
 Function: Oled_DisplayOff
 Description:Turn off the Oled display
 */
void Oled_DisplayOff(void) {
	u8 data[6] = { 0x80, 0X8D, 0x80, 0X10, 0x80, 0XAE };

	I2C_PageWrite(data, 6, DEVICEADDR_OLED);
}
/*
 * This sets the OLED screen to invert the screen (flip it vertically)
 * This is used if the unit is set to left hand mode
 */
void Oled_DisplayFlip() {
	u8 data[2] = { 0x80, 0XC8 };
	I2C_PageWrite(data, 2, DEVICEADDR_OLED);
	data[1] = 0xA1;
	I2C_PageWrite(data, 2, DEVICEADDR_OLED);
	displayOffset=0;

}
/*
 Description: write a command to the Oled display
 Input: number of bytes to write, array to write
 Output:
 */
u8* Data_Command(u8 length, u8* data) {
	int i;
	u8 tx_data[128];
	//here are are inserting the data write command at the beginning
	tx_data[0] = 0x40;
	length += 1;
	for (i = 1; i < length; i++) //Loop through the array of data
		tx_data[i] = *data++;
	I2C_PageWrite(tx_data, length, DEVICEADDR_OLED); //write out the buffer
	return data;
}
/*******************************************************************************
 Function:Set_ShowPos
 Description:Set the current position in GRAM that we are drawing to
 Input:x,y co-ordinates
 *******************************************************************************/
void Set_ShowPos(u8 x, u8 y) {
	u8 pos_param[8] = { 0x80, 0xB0, 0x80, 0x21, 0x80, 0x00, 0x80, 0x7F };
	//page 0, start add = x(below) through to 0x7F (aka 127)
	pos_param[5] = x + displayOffset;/*Display offset ==0 for Lefty, == 32 fo righty*/
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
	delayMs(2);
	OLED_ACT(); //Toggling reset to reset the oled
	delayMs(2);
	param_len = 46;
	I2C_PageWrite((u8 *) OLED_Setup_Array, param_len, DEVICEADDR_OLED);
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
/*
 * Draws a string onto the screen starting at the left
 */
void OLED_DrawString(char* string, uint8_t length) {
	for (uint8_t i = 0; i < length; i++) {
		OLED_DrawChar(string[i], i);
	}
}
/*
 * Draw a char onscreen at letter index x
 */
void OLED_DrawChar(char c, uint8_t x) {
	if (x > 7)
		return; //clipping
	x *= FONT_WIDTH; //convert to a x coordinate

	u8* ptr = (u8*) FONT;
	if (c >= 'A' && c <= 'Z') {
		ptr += (c - 'A' + 10) * (FONT_WIDTH * 2); //alpha is ofset 10 chars into the array
	} else if (c >= '0' && c <= '9')
		ptr += (c - '0') * (FONT_WIDTH * 2);
	else if (c < 10)
		ptr += (c) * (FONT_WIDTH * 2);
	else if (c == ' ') {
		//blank on space bar
		ptr += (36) * (FONT_WIDTH * 2);
	} else if (c == '<') {
		ptr += (37) * (FONT_WIDTH * 2);
	} else if (c == '>') {
		ptr += (38) * (FONT_WIDTH * 2);
	}else if (c=='.')
	{
		ptr += (39) * (FONT_WIDTH * 2);
	}

	Oled_DrawArea(x, 0, FONT_WIDTH, 16, (u8*) ptr);
}
/*
 * Draw a 2 digit number to the display at letter slot x
 */
void OLED_DrawTwoNumber(uint8_t in, uint8_t x) {

	OLED_DrawChar((in / 10) % 10, x);
	OLED_DrawChar(in % 10, x + 1);
}
/*
 * Draw a 3 digit number to the display at letter slot x
 */
void OLED_DrawThreeNumber(uint16_t in, uint8_t x) {

	OLED_DrawChar((in / 100) % 10, x);
	OLED_DrawChar((in / 10) % 10, x + 1);
	OLED_DrawChar(in % 10, x + 2);
}
/*
 * Draw a 4 digit number to the display at letter slot x
 */
void OLED_DrawFourNumber(uint16_t in, uint8_t x) {

	OLED_DrawChar((in / 1000) % 10, x);
	OLED_DrawChar((in / 100) % 10, x + 1);
	OLED_DrawChar((in / 10) % 10, x + 2);
	OLED_DrawChar(in % 10, x + 3);
}
