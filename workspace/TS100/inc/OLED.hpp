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
#include "Font.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "FreeRTOS.h"
#ifdef __cplusplus
}
#endif
#define DEVICEADDR_OLED  (0x3c<<1)

class OLED {
public:
	OLED(I2C_HandleTypeDef* i2cHandle);	// Initialize Driver and store I2C pointer
	void initialize(); 	// Startup the I2C coms (brings screen out of reset etc)
	void refresh(); 	// Draw the buffer out to the LCD using the DMA Channel
	void drawChar(char c, char preCursorCommand = '\0');// Draw a character to a specific location
	void displayOnOff(bool on);				// Turn the screen on or not
	void setRotation(bool leftHanded);  	// Set the rotation for the screen
	bool getRotation();					// Get the current rotation of the LCD
	void print(const char* string);	// Draw a string to the current location, with current font
	void setCursor(int16_t x, int16_t y);   // Set the cursor location by pixels
	void setCharCursor(int16_t x, int16_t y); //Set cursor location by chars in current font
	void setFont(uint8_t fontNumber);// (Future) Set the font that is being used
	void drawImage(const uint8_t* buffer, uint8_t x, uint8_t width);
	// Draws an image to the buffer, at x offset from top to bottom (fixed height renders)
	void printNumber(uint16_t number, uint8_t places);
	// Draws a number at the current cursor location
	void clearScreen();						// Clears the buffer
	void drawBattery(uint8_t state);		// Draws the battery level symbol
	void drawCheckbox(bool state);		// Draws a checkbox
	void drawTemperatureSymbol(bool inFahrenheit); // Draw Temperature symbol
	void drawSymbol(uint8_t symbolID);//Used for drawing symbols of a predictable width
	void drawArea(int16_t x, int8_t y, uint8_t wide, uint8_t height,
			const uint8_t* ptr);
private:

	//Draw a buffer to the screen buffer

	I2C_HandleTypeDef* i2c;					//i2c Pointer
	const uint8_t* currentFont;	// Pointer to the current font used for rendering to the buffer
	uint8_t screenBuffer[12 + 96 + 96 + 10];    // The data buffer
	uint8_t* firstStripPtr; // Pointers to the strips to allow for buffer having extra content
	uint8_t* secondStripPtr;	//Pointers to the strips
	bool inLeftHandedMode; // Whether the screen is in left or not (used for offsets in GRAM)
	bool displayOnOffState;					// If the display is on or not
	uint8_t fontWidth, fontHeight;
	int16_t cursor_x, cursor_y;
	uint8_t displayOffset;
};

#endif /* OLED_HPP_ */
