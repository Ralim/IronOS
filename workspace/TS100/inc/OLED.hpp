/*
 * OLED.hpp
 *
 *  Created on: 20Jan.,2017
 *      Author: Ben V. Brown <Ralim>
 *      Designed for the SSD1307
 *      Cleared for release for TS100 2017/08/20
 */

#ifndef OLED_HPP_
#define OLED_HPP_
#include <hardware.h>
#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <string.h>
#include "FRToSI2C.hpp"
#include "Font.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "FreeRTOS.h"
#ifdef __cplusplus
}
#endif
#define DEVICEADDR_OLED   (0x3c<<1)
#define OLED_WIDTH        96
#define FRAMEBUFFER_START 17


class OLED {
public:
	OLED(FRToSI2C* i2cHandle); // Initialize Driver and store I2C pointer
	void initialize(); 	// Startup the I2C coms (brings screen out of reset etc)
	
	// Draw the buffer out to the LCD using the DMA Channel
	void refresh() {
		i2c->Transmit( DEVICEADDR_OLED, screenBuffer, FRAMEBUFFER_START + (OLED_WIDTH * 2));
	}

	void drawChar(char c, char preCursorCommand = '\0'); // Draw a character to a specific location
	// Turn the screen on or not
	void displayOnOff(bool on) {
		displayOnOffState = on;
		screenBuffer[1] = on ? 0xAF : 0xAE;
	}			
	void setRotation(bool leftHanded);  	// Set the rotation for the screen
	// Get the current rotation of the LCD
	bool getRotation() const {
		return inLeftHandedMode;
	}					
	void print(const char* string);	// Draw a string to the current location, with current font
	// Set the cursor location by pixels
	void setCursor(int16_t x, int16_t y) {
		cursor_x = x;
		cursor_y = y;		
	}   
	//Set cursor location by chars in current font
	void setCharCursor(int16_t x, int16_t y) {
		cursor_x = x * fontWidth;
		cursor_y = y * fontHeight;
	}
	void setFont(uint8_t fontNumber); // (Future) Set the font that is being used
	void drawImage(const uint8_t* buffer, uint8_t x, uint8_t width) {
		drawArea(x, 0, width, 16, buffer);
	}
	// Draws an image to the buffer, at x offset from top to bottom (fixed height renders)
	void printNumber(uint16_t number, uint8_t places);
	// Draws a number at the current cursor location
	// Clears the buffer
	void clearScreen() {
		memset(&screenBuffer[FRAMEBUFFER_START], 0, OLED_WIDTH * 2);
	}					
	// Draws the battery level symbol
	void drawBattery(uint8_t state) {
		drawSymbol(3 + (state > 10 ? 10 : state));
	}		
	// Draws a checkbox
	void drawCheckbox(bool state) {
		drawSymbol((state) ? 16 : 17);
	}
	void drawSymbol(uint8_t symbolID);//Used for drawing symbols of a predictable width
	void drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t* ptr); //Draw an area, but y must be aligned on 0/8 offset
	void fillArea(int16_t x, int8_t y, uint8_t wide, uint8_t height, const uint8_t value); //Fill an area, but y must be aligned on 0/8 offset
	void drawFilledRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,bool clear);
	void drawHeatSymbol(uint8_t state);
private:

	//Draw a buffer to the screen buffer

	FRToSI2C* i2c;					//i2c Pointer
	const uint8_t* currentFont;	// Pointer to the current font used for rendering to the buffer
	uint8_t* firstStripPtr; // Pointers to the strips to allow for buffer having extra content
	uint8_t* secondStripPtr;	//Pointers to the strips
	bool inLeftHandedMode; // Whether the screen is in left or not (used for offsets in GRAM)
	bool displayOnOffState;					// If the display is on or not
	uint8_t fontWidth, fontHeight;
	int16_t cursor_x, cursor_y;
	uint8_t displayOffset;
	uint8_t screenBuffer[16 + (OLED_WIDTH * 2) + 10];    // The data buffer
};

#endif /* OLED_HPP_ */
