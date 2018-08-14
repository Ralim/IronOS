/*
 * OLED.cpp
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#include <OLED.hpp>
#include <string.h>
#include "Translation.h"
#include "cmsis_os.h"
/*Setup params for the OLED screen*/
/*http://www.displayfuture.com/Display/datasheet/controller/SSD1307.pdf*/
/*All commands are prefixed with 0x80*/
/*Data packets are prefixed with 0x40*/
uint8_t OLED_Setup_Array[] = { /**/
0x80, 0xAE,/*Display off*/
0x80, 0xD5,/*Set display clock divide ratio / osc freq*/
0x80, 0x52,/*Divide ratios*/
0x80, 0xA8,/*Set Multiplex Ratio*/
0x80, 0x0F,/*16 == max brightness,39==dimmest*/
0x80, 0xC0,/*Set COM Scan direction*/
0x80, 0xD3,/*Set vertical Display offset*/
0x80, 0x00,/*0 Offset*/
0x80, 0x40,/*Set Display start line to 0*/
0x80, 0xA0,/*Set Segment remap to normal*/
0x80, 0x8D,/*Charge Pump*/
0x80, 0x14,/*Charge Pump settings*/
0x80, 0xDA,/*Set VCOM Pins hardware config*/
0x80, 0x02,/*Combination 2*/
0x80, 0x81,/*Contrast*/
0x80, 0x33,/*^51*/
0x80, 0xD9,/*Set pre-charge period*/
0x80, 0xF1,/*Pre charge period*/
0x80, 0xDB,/*Adjust VCOMH regulator ouput*/
0x80, 0x30,/*VCOM level*/
0x80, 0xA4,/*Enable the display GDDR*/
0x80, 0XA6,/*Normal display*/
0x80, 0x20,/*Memory Mode*/
0x80, 0x00,/*Wrap memory*/
0x80, 0xAF /*Display on*/
};
//Setup based on the SSD1307 and modified for the SSD1306

const uint8_t REFRESH_COMMANDS[17] = { 0x80, 0xAF, 0x80, 0x21, 0x80, 0x20, 0x80,
		0x7F, 0x80, 0xC0, 0x80, 0x22, 0x80, 0x00, 0x80, 0x01, 0x40 };

OLED::OLED(FRToSI2C* i2cHandle) {
	i2c = i2cHandle;
	cursor_x = cursor_y = 0;
	currentFont = FONT_12;
	fontWidth = 12;
	inLeftHandedMode = false;
	firstStripPtr = &screenBuffer[FRAMEBUFFER_START];
	secondStripPtr = &screenBuffer[FRAMEBUFFER_START + OLED_WIDTH];
	fontHeight = 16;
	displayOffset = 0;
	displayOnOffState = true;
}

void OLED::initialize() {
	memcpy(&screenBuffer[0], &REFRESH_COMMANDS[0], sizeof(REFRESH_COMMANDS));

	HAL_Delay(50);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	//Send the setup settings
	i2c->Transmit( DEVICEADDR_OLED, (uint8_t*) OLED_Setup_Array,
			sizeof(OLED_Setup_Array));
	displayOnOff(true);
}

/*
 * Prints a char to the screen.
 * UTF font handling is done using the two input chars.
 * Precursor is the command char that is used to select the table.
 */
void OLED::drawChar(char c, char PrecursorCommand) {
	if (c == '\n' && cursor_y == 0) {
		cursor_x = 0;
		cursor_y = 8;
	}
	if (c < ' ') {
		return;
	}
	uint16_t index = 0;
	if (PrecursorCommand == 0) {
		//Fonts are offset to start at the space char
		index = (c - ' ');
	} else {
		//This is for extended range
		//We decode the precursor command to find the offset
		//Latin starts at 96
		c -= 0x80;

		switch (PrecursorCommand) {

		case 0xC2:
			index = (96 - 32) + (c);
			break;  //-32 compensate for chars excluded from font C2 section
		case 0xC3:
			index = (128) + (c);
			break;
#if defined(LANG_RU) || defined(LANG_UK) || defined(LANG_SR) || defined(LANG_BG) || defined(LANG_MK)
			case 0xD0:
			index = (192) + (c);
			break;
			case 0xD1:
			index = (256) + (c);
			break;
#else
		case 0xC4:
			index = (192) + (c);
			break;
		case 0xC5:
			index = (256) + (c);
			break;
#endif

		default:
			return;
		}
	}
	uint8_t* charPointer;
	charPointer = ((uint8_t*) currentFont)
			+ ((fontWidth * (fontHeight / 8)) * index);

	drawArea(cursor_x, cursor_y, fontWidth, fontHeight, charPointer);

	cursor_x += fontWidth;
}

void OLED::setRotation(bool leftHanded) {
	if (inLeftHandedMode == leftHanded) {
		return;
	}

	//send command struct again with changes
	if (leftHanded) {
		OLED_Setup_Array[11] = 0xC8;    //c1?
		OLED_Setup_Array[19] = 0xA1;
	} else {
		OLED_Setup_Array[11] = 0xC0;
		OLED_Setup_Array[19] = 0xA0;
	}
	i2c->Transmit( DEVICEADDR_OLED, (uint8_t*) OLED_Setup_Array,
			sizeof(OLED_Setup_Array));
	inLeftHandedMode = leftHanded;

	screenBuffer[5] = inLeftHandedMode ? 0 : 32; //display is shifted by 32 in left handed mode as driver ram is 128 wide
	screenBuffer[7] = inLeftHandedMode ? 95 : 0x7F; //End address of the ram segment we are writing to (96 wide)
	screenBuffer[9] = inLeftHandedMode ? 0xC8 : 0xC0;
}

//print a string to the current cursor location
void OLED::print(const char* str) {
	while (str[0]) {
		if (str[0] >= 0x80) {
			drawChar(str[1], str[0]);
			str++;    //skip this marker
		} else
			drawChar(str[0]);
		str++;
	}
}

void OLED::setFont(uint8_t fontNumber) {
	if (fontNumber == 1) {
		//small font
		currentFont = FONT_6x8;
		fontHeight = 8;
		fontWidth = 6;
	} else if (fontNumber == 2) {
		currentFont = ExtraFontChars;
		fontHeight = 16;
		fontWidth = 12;
	} else {
		currentFont = FONT_12;
		fontHeight = 16;
		fontWidth = 12;
	}
}

//maximum places is 5
void OLED::printNumber(uint16_t number, uint8_t places) {
	char buffer[7] = { 0 };

	if (places >= 5) {
		buffer[5] = '0' + number % 10;
		number /= 10;
	}
	if (places > 4) {
		buffer[4] = '0' + number % 10;
		number /= 10;
	}

	if (places > 3) {
		buffer[3] = '0' + number % 10;
		number /= 10;
	}

	if (places > 2) {
		buffer[2] = '0' + number % 10;
		number /= 10;
	}

	if (places > 1) {
		buffer[1] = '0' + number % 10;
		number /= 10;
	}

	buffer[0] = '0' + number % 10;
	number /= 10;
	print(buffer);
}

void OLED::drawSymbol(uint8_t symbolID) {
	//draw a symbol to the current cursor location
	setFont(2);
	drawChar(' ' + symbolID); // space offset is in all fonts, so we pad it here and remove it later
	setFont(0);
}

//Draw an area, but y must be aligned on 0/8 offset
void OLED::drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height,
		const uint8_t* ptr) {
	// Splat this from x->x+wide in two strides
	if (x <= -wide)
		return;    //cutoffleft
	if (x > 96)
		return;    //cutoff right

	uint8_t visibleStart = 0;
	uint8_t visibleEnd = wide;

	// trimming to draw partials
	if (x < 0) {
		visibleStart -= x;  //subtract negative value == add absolute value
	}
	if (x + wide > 96) {
		visibleEnd = 96 - x;
	}

	if (y == 0) {
		//Splat first line of data
		for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
			firstStripPtr[xx + x] = ptr[xx];
		}
	}
	if (y == 8 || height == 16) {
		// Splat the second line
		for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
			secondStripPtr[x + xx] = ptr[xx + (height == 16 ? wide : 0)];
		}
	}
}
void OLED::fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height,
		const uint8_t value) {
	// Splat this from x->x+wide in two strides
	if (x <= -wide)
		return;    //cutoffleft
	if (x > 96)
		return;    //cutoff right

	uint8_t visibleStart = 0;
	uint8_t visibleEnd = wide;

	// trimming to draw partials
	if (x < 0) {
		visibleStart -= x;  //subtract negative value == add absolute value
	}
	if (x + wide > 96) {
		visibleEnd = 96 - x;
	}

	if (y == 0) {
		//Splat first line of data
		for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
			firstStripPtr[xx + x] = value;
		}
	}
	if (y == 8 || height == 16) {
		// Splat the second line
		for (uint8_t xx = visibleStart; xx < visibleEnd; xx++) {
			secondStripPtr[x + xx] = value;
		}
	}
}

void OLED::drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		bool clear) {
	//Draw this in 3 sections
	//This is basically a N wide version of vertical line

	//Step 1 : Draw in the top few pixels that are not /8 aligned
	//LSB is at the top of the screen
	uint8_t mask = 0xFF;
	if (y0) {
		mask = mask << (y0 % 8);
		for (uint8_t col = x0; col < x1; col++)
			if (clear)
				firstStripPtr[(y0 / 8) * 96 + col] &= ~mask;
			else
				firstStripPtr[(y0 / 8) * 96 + col] |= mask;
	}
	//Next loop down the line the total number of solids
	if (y0 / 8 != y1 / 8)
		for (uint8_t col = x0; col < x1; col++)
			for (uint8_t r = (y0 / 8); r < (y1 / 8); r++) {
				//This gives us the row index r
				if (clear)
					firstStripPtr[(r * 96) + col] = 0;
				else
					firstStripPtr[(r * 96) + col] = 0xFF;
			}

	//Finally draw the tail
	mask = ~(mask << (y1 % 8));
	for (uint8_t col = x0; col < x1; col++)
		if (clear)
			firstStripPtr[(y1 / 8) * 96 + col] &= ~mask;
		else
			firstStripPtr[(y1 / 8) * 96 + col] |= mask;
}

void OLED::drawHeatSymbol(uint8_t state) {
	//Draw symbol 14
//Then draw over it botom 5 pixels always stay. 8 pixels above that are the levels
	state /= 12; // 0-> 8 range
	//Then we want to draw down (16-(5+state)
	uint8_t cursor_x_temp = cursor_x;
	drawSymbol(14);
	drawFilledRect(cursor_x_temp, 0, cursor_x_temp + 12, 2 + (8 - state), true);
}
