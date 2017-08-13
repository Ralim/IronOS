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
int8_t displayOffset = 32;
uint8_t currentOrientation = 0;

uint8_t displayBuffer[2 * 96]; //This is used to allow us to draw locally before sync'ing to the screen.

/*Setup params for the OLED screen*/
/*http://www.displayfuture.com/Display/datasheet/controller/SSD1307.pdf*/
/*All commands are prefixed with 0x80*/
u8 OLED_Setup_Array[46] = { /**/
0x80, 0xAE,/*Display off*/
0x80, 0xD5,/*Set display clock divide ratio / osc freq*/
0x80, 0x52,/*Unknown*/
0x80, 0xA8,/*Set Multiplex Ratio*/
0x80, 0x0F, /*16 == max brightness,39==dimmest*/
0x80, 0xC0,/*Set COM Scan direction*/
0x80, 0xD3,/*Set Display offset*/
0x80, 0x00,/*0 Offset*/
0x80, 0x40,/*Set Display start line to 0*/
0x80, 0xA0,/*Set Segment remap to normal*/
0x80, 0x8D,/*Unknown*/
0x80, 0x14,/*Unknown*/
0x80, 0xDA,/*Set VCOM Pins hardware config*/
0x80, 0x02,/*Combination 2*/
0x80, 0x81,/*Contrast*/
0x80, 0x33,/*51*/
0x80, 0xD9,/*Set pre-charge period*/
0x80, 0xF1,/**/
0x80, 0xDB,/*Adjust VCOMH regulator ouput*/
0x80, 0x30,/*Unknown*/
0x80, 0xA4,/*Enable the display GDDR*/
0x80, 0XA6,/*Normal display*/
0x80, 0xAF /*Dispaly on*/
};
uint8_t OLEDOnOffState = 0; //Used to lock out so we dont send it too often
/*
 Function: Oled_DisplayOn
 Description:Turn on the Oled display
 */
void Oled_DisplayOn(void) {
	if (OLEDOnOffState != 1) {
		u8 data[6] = { 0x80, 0X8D, 0x80, 0X14, 0x80, 0XAF };

		I2C_PageWrite(data, 6, DEVICEADDR_OLED);
		OLEDOnOffState = 1;
	}
}
/*
 Function: Oled_DisplayOff
 Description:Turn off the Oled display
 */
void Oled_DisplayOff(void) {
	if (OLEDOnOffState != 2) {
		u8 data[6] = { 0x80, 0X8D, 0x80, 0X10, 0x80, 0XAE };

		I2C_PageWrite(data, 6, DEVICEADDR_OLED);
		OLEDOnOffState = 2;
	}
}

/*
 Description: write a command to the Oled display
 Input: number of bytes to write, array to write
 Output:
 */
const u8* Data_Command(u8 length, const u8* data) {
	int i;
	u8 tx_data[129];
	//here are are inserting the data write command at the beginning
	tx_data[0] = 0x40;
	length++;
	for (i = 1; i <= length; i++) //Loop through the array of data
			{
		if (data == 0)
			tx_data[i] = 0;
		else
			tx_data[i] = *data++;
	}
	I2C_PageWrite(tx_data, length, DEVICEADDR_OLED); //write out the buffer
	return data;
}
//This causes us to write out the buffered screen data to the display
void OLED_Sync() {
	Set_ShowPos(0, 0);
	Data_Command(96, displayBuffer);
	Set_ShowPos(0, 1);
	Data_Command(96, displayBuffer + 96);

}
/*******************************************************************************
 Function:Set_ShowPos
 Description:Set the current position in GRAM that we are drawing to
 Input:x,y co-ordinates
 *******************************************************************************/
void Set_ShowPos(u8 x, u8 y) {
	u8 pos_param[8] = { 0x80, 0xB0, 0x80, 0x21, 0x80, 0x00, 0x80, 0x7F };
	//page 0, start add = x(below) through to 0x7F (aka 127)
	pos_param[5] = x + displayOffset;/*Display offset ==0 for Lefty, == 32 for righty*/
	pos_param[1] += y;
	I2C_PageWrite(pos_param, 8, DEVICEADDR_OLED);
}

/*******************************************************************************
 Function:Oled_DrawArea
 Description:
 Inputs:(x,y) start point, (width,height) of enclosing rect, pointer to data
 Output: pointer to the last byte written out
 *******************************************************************************/
void Oled_DrawArea(u8 x, u8 y, u8 wide, u8 height, const u8* ptr) {
	//We want to blat the given data over the buffer
	//X is the left right position (index's through the display buffer)
	//Y is the height value (affects the bits)
	//Y is either 0 or 8, we dont do smaller bit blatting
	u8 lines = height / 8;
	//We draw the 1 or two stripes seperately
	for (u8 i = 0; i < (wide * lines); i++) {
		u8 xp = x + (i % wide);
		u8 yoffset = i < wide ? 0 : 96;
		if (y == 8)
			yoffset = 96;
		displayBuffer[xp + yoffset] = ptr[i];
	}

}
void OLED_InvertBuffer() {
	for (uint16_t i = 0; i < (2 * 96); i++)
		displayBuffer[i] = ~displayBuffer[i];
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
 Description: Initializes the Oled screen
 *******************************************************************************/
void Init_Oled(uint8_t leftHanded) {

	currentOrientation = leftHanded;
	u8 param_len;
	OLED_RST();
	delayMs(5);
	OLED_ACT(); //Toggling reset to reset the oled
	delayMs(5);
	param_len = 46;
	if (leftHanded == 1) {
		OLED_Setup_Array[11] = 0xC8;
		OLED_Setup_Array[19] = 0xA1;
		displayOffset = 0;
	} else if (leftHanded == 0) {
		OLED_Setup_Array[11] = 0xC0;
		OLED_Setup_Array[19] = 0x40;
		displayOffset = 32;
	}
	I2C_PageWrite((u8 *) OLED_Setup_Array, param_len, DEVICEADDR_OLED);
	for (uint8_t i = 0; i < 2 * 96; i++)
		displayBuffer[i] = 0; //turn off screen
}

/*******************************************************************************
 Function:Clear_Screen
 Description:Clear the entire screen to off (black)
 *******************************************************************************/
void Clear_Screen(void) {
	memset(displayBuffer, 0, 96 * 2);

}
/*
 * Draws a string onto the screen starting at the left
 */
void OLED_DrawString(const char* string, const uint8_t length) {
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
	u8* ptr;
	u16 offset = 0;
	if (c < 0x80) {
		ptr = (u8*) FONT;
		offset = c - ' ';
	} else if (c >= 0xA0) {
		ptr = (u8*) FontLatin2;
		offset = c - 0xA0; //this table starts at 0xA0
	} else
		return; //not in font

	offset *= (2 * FONT_WIDTH);
	ptr += offset;

	Oled_DrawArea(x, 0, FONT_WIDTH, 16, (u8*) ptr);
}
void OLED_DrawExtraFontChars(uint8_t id, uint8_t x) {
	u8* ptr = (u8*) ExtraFontChars;
	ptr += (id) * (FONT_WIDTH * 2);
	x *= FONT_WIDTH; //convert to a x coordinate
	Oled_DrawArea(x, 0, FONT_WIDTH, 16, (u8*) ptr);
}
void OLED_DrawSymbolChar(uint8_t id, uint8_t x) {
	u8* ptr = (u8*) FontSymbols;
	ptr += (id) * (FONT_WIDTH * 2);
	x *= FONT_WIDTH; //convert to a x coordinate

	Oled_DrawArea(x, 0, FONT_WIDTH, 16, (u8*) ptr);
}

void OLED_DrawWideChar(uint8_t id, uint8_t x) {
	u8* ptr = (u8*) DoubleWidthChars;
	ptr += (id) * (FONT_WIDTH * 4);
	x *= FONT_WIDTH; //convert to a x coordinate

	Oled_DrawArea(x, 0, FONT_WIDTH * 2, 16, (u8*) ptr);
}
void OLED_BlankSlot(uint8_t xStart, uint8_t width) {
	u8* ptr = (u8*) FONT;
	ptr += (36) * (FONT_WIDTH * 2);

	Oled_DrawArea(xStart, 0, width, 16, (u8*) ptr);
}

/*
 * Draw a 2 digit number to the display at letter slot x
 */
void OLED_DrawTwoNumber(uint8_t in, uint8_t x) {

	OLED_DrawChar(48 + (in / 10) % 10, x);
	OLED_DrawChar(48 + in % 10, x + 1);
}
/*
 * Draw a 3 digit number to the display at letter slot x
 */
void OLED_DrawThreeNumber(uint16_t in, uint8_t x) {

	OLED_DrawChar(48 + (in / 100) % 10, x);
	OLED_DrawChar(48 + (in / 10) % 10, x + 1);
	OLED_DrawChar(48 + in % 10, x + 2);
}
/*
 * Draw a 4 digit number to the display at letter slot x
 */
void OLED_DrawFourNumber(uint16_t in, uint8_t x) {

	OLED_DrawChar(48 + (in / 1000) % 10, x);
	OLED_DrawChar(48 + (in / 100) % 10, x + 1);
	OLED_DrawChar(48 + (in / 10) % 10, x + 2);
	OLED_DrawChar(48 + (in % 10), x + 3);
}

void OLED_DrawIDLELogo() {
	static uint8_t drawAttempt = 0;
	drawAttempt++;
	if (drawAttempt & 0x80) {
		if (drawAttempt & 0x08)
			Oled_DrawArea(0, 0, 96, 8, (u8*) Iron_RightArrow_UP);
		else
			Oled_DrawArea(0, 0, 96, 8, (u8*) Iron_RightArrow_DOWN);

		Oled_DrawArea(0, 8, 96, 8, (u8*) Iron_Base);
	} else {
		if (drawAttempt & 0x08)
			Oled_DrawArea(0, 0, 96, 8, (u8*) Iron_LeftArrow_UP);
		else
			Oled_DrawArea(0, 0, 96, 8, (u8*) Iron_LeftArrow_DOWN);
		Oled_DrawArea(0, 8, 96, 8, (u8*) Iron_Base);
	}

}

void OLED_DrawSymbol(uint8_t x, uint8_t symbol) {
	Oled_DrawArea(x * FONT_WIDTH, 0, 16, 16, SymbolTable + (symbol * 32));
}

void OLED_SetOrientation(uint8_t ori) {
	if (ori > 1)
		return;
	if (ori != currentOrientation) {
		Init_Oled(ori);
	}
}

uint8_t OLED_GetOrientation() {
	return currentOrientation;
}
