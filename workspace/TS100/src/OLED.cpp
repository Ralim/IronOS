/*
 * OLED.cpp
 *
 *  Created on: 29Aug.,2017
 *      Author: Ben V. Brown
 */

#include <OLED.hpp>
#include <string.h>
/*Setup params for the OLED screen*/
/*http://www.displayfuture.com/Display/datasheet/controller/SSD1307.pdf*/
/*All commands are prefixed with 0x80*/
/*Data packets are prefixed with 0x40*/
const uint8_t configLength = 50;
uint8_t OLED_Setup_Array[] = { /**/
0x80, 0xAE,/*Display off*/
0x80, 0xD5,/*Set display clock divide ratio / osc freq*/
0x80, 0x52,/*Divide ratios*/
0x80, 0xA8,/*Set Multiplex Ratio*/
0x80, 0x0E,/*16 == max brightness,39==dimmest*/
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

OLED::OLED(I2C_HandleTypeDef* i2cHandle) {
	i2c = i2cHandle;
	cursor_x = cursor_y = 0;
	currentFont = FONT_12;
	fontWidth = 12;
	inLeftHandedMode = false;
	firstStripPtr = &screenBuffer[13];
	secondStripPtr = &screenBuffer[13 + 96];
	fontHeight = 16;
	fontWidth = 12;
	displayOffset = 0;
	displayOnOffState = true;

}

void OLED::initialize() {
	HAL_Delay(5);
	HAL_GPIO_WritePin(OLED_RESET_GPIO_Port, OLED_RESET_Pin, GPIO_PIN_SET);
	HAL_Delay(5);
	//Send the setup settings
	HAL_I2C_Master_Transmit(i2c, DEVICEADDR_OLED, (uint8_t*) OLED_Setup_Array, configLength, 0xFFFF);
	//displayOnOff(true);

}

//Write out the buffer to the OLEd & call any rendering objects
void OLED::refresh() {
	screenBuffer[0] = 0x80;
	screenBuffer[1] = 0x21;
	screenBuffer[2] = 0x80;
	screenBuffer[3] = inLeftHandedMode ? 0 : 32;
	screenBuffer[4] = 0x80;
	screenBuffer[5] = inLeftHandedMode ? 95 : 0x7F;

	screenBuffer[6] = 0x80;    //Set pages to rollover after 2
	screenBuffer[7] = 0x22;
	screenBuffer[8] = 0x80;
	screenBuffer[9] = 0x00;
	screenBuffer[10] = 0x80;
	screenBuffer[11] = 0x01;

	screenBuffer[12] = 0x40;    //start of data marker

	HAL_I2C_Master_Transmit(i2c, DEVICEADDR_OLED, screenBuffer, 12 + 96 * 2 + 1, 0xFFFF);

}

void OLED::drawChar(char c, char PrecursorCommand) {
//prints a char to the screen
	if (c < ' ')
		return;
	//We are left with
	uint8_t* charPointer;
	//Fonts are offset to start at the space char.
	/*
	 * UTF font handling is done using the two input chars
	 * Precursor is the command char that is used to select the table
	 *
	 */
	uint16_t index = 0;
	if (PrecursorCommand == 0)
		index = (c - ' ');
	else {

		//This is for extended range
		//We decode the precursor command to find the offset
		//Latin stats at 96
		c -= 0x80;
		if (PrecursorCommand == 0xC3)
			index = (128) + (c);
		else if (PrecursorCommand == 0xC2)
			index = (96) + (c);
		else if (PrecursorCommand == 0xD0)
			index = (192) + (c);
		else if (PrecursorCommand == 0xD1)
			index = (256) + (c);
		else
			return;
	}
	charPointer = ((uint8_t*) currentFont) + ((fontWidth * (fontHeight / 8)) * index);
	if ((charPointer - currentFont) > fontTableLength)
		return;
	if (cursor_x >= 0 && cursor_x < 96)
		drawArea(cursor_x, cursor_y, fontWidth, fontHeight, charPointer);
	cursor_x += fontWidth;
}

void OLED::displayOnOff(bool on) {

	if (on != displayOnOffState) {
		uint8_t data[6] = { 0x80, 0X8D, 0x80, 0X14, 0x80, 0XAF };    //on
		if (!on) {
			data[3] = 0x10;
			data[5] = 0xAE;
		}
		HAL_I2C_Master_Transmit(i2c, DEVICEADDR_OLED, data, 6, 0xFFFF);
		displayOnOffState = on;
	}
}

void OLED::setRotation(bool leftHanded) {
	if (inLeftHandedMode != leftHanded) {
		//send command struct again with changes
		if (leftHanded == 1) {
			OLED_Setup_Array[11] = 0xC8;    //c1?
			OLED_Setup_Array[19] = 0xA1;
		} else if (leftHanded == 0) {
			OLED_Setup_Array[11] = 0xC0;
			OLED_Setup_Array[19] = 0xA0;
		}
		HAL_I2C_Master_Transmit(i2c, DEVICEADDR_OLED, (uint8_t*) OLED_Setup_Array, 50, 0xFFFF);
		inLeftHandedMode = leftHanded;
	}
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

void OLED::setCursor(int16_t x, int16_t y) {
	cursor_x = x;
	cursor_y = y;
}

void OLED::setFont(uint8_t fontNumber) {
	if (fontNumber == 1) {
		//small font
		currentFont = ASCII6x8;
		fontHeight = 8;
		fontWidth = 6;
		fontTableLength = sizeof(ASCII6x8);
	} else if (fontNumber == 2) {
		currentFont = ExtraFontChars;
		fontHeight = 16;
		fontWidth = 12;
		fontTableLength = sizeof(ExtraFontChars);
	} else {
		currentFont = FONT_12;
		fontHeight = 16;
		fontWidth = 12;
		fontTableLength = sizeof(FONT_12);
	}
}

void OLED::drawImage(const uint8_t* buffer, uint8_t x, uint8_t width) {
	drawArea(x, 0, width, 16, buffer);
}

//maximum places is 5
void OLED::printNumber(uint16_t number, uint8_t places) {
	char buffer[6];
	buffer[5] = 0;    //null
	if (places == 5) {
		buffer[4] = '0' + number % 10;
		number /= 10;
	} else
		buffer[4] = 0;

	if (places > 3) {
		buffer[3] = '0' + number % 10;
		number /= 10;
	} else
		buffer[3] = 0;

	if (places > 2) {
		buffer[2] = '0' + number % 10;
		number /= 10;
	} else
		buffer[2] = 0;

	if (places > 1) {
		buffer[1] = '0' + number % 10;
		number /= 10;
	} else
		buffer[1] = 0;
	buffer[0] = '0' + number % 10;
	number /= 10;
	print(buffer);
}

void OLED::clearScreen() {
	memset(firstStripPtr, 0, 96);
	memset(secondStripPtr, 0, 96);
}

bool OLED::getRotation() {
	return inLeftHandedMode;
}
void OLED::drawBattery(uint8_t state) {
	if (state > 10)
		state = 10;
	drawSymbol(3 + state);
}
void OLED::drawSymbol(uint8_t symbolID) {
	//draw a symbol to the current cursor location
	setFont(2);
	drawChar(' ' + symbolID);    // space offset is in all fonts, so we pad it here and remove it later
	setFont(0);
}

//Draw an area, but y must be aligned on 0/8 offset
void OLED::drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t* ptr) {
	// Splat this from x->x+wide in two strides
	if (x < 0)
		return;    //cutoffleft
	if ((x + wide) > 96)
		return;    //cutoff right
	if (y == 0) {
		//Splat first line of data
		for (uint8_t xx = 0; xx < (wide); xx++) {
			firstStripPtr[xx + x] = ptr[xx];
		}
	}
	if (y == 8 || height == 16) {
		// Splat the second line
		for (uint8_t xx = 0; xx < wide; xx++) {
			secondStripPtr[x + xx] = ptr[xx + (height == 16 ? wide : 0)];

		}
	}
}
