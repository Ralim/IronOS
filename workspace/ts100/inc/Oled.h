/*
 * Oled.h
 * Functions for writing to the OLED screen
 * Basically wraps drawing text and numbers to the OLED
 * Uses font.h -> 14 pixel wide fixed width
 */
#ifndef _OLED_SSD1306_H
#define _OLED_SSD1306_H

#define DEVICEADDR_OLED  0x3c
#include "stm32f10x.h"
#include "Interrupt.h"

void Oled_DisplayOn(void);
void Oled_DisplayOff(void);

u8* Oled_DrawArea(u8 x0, u8 y0, u8 wide, u8 high, u8* ptr);
void Set_ShowPos(u8 x, u8 y);
void Oled_DisplayFlip();
void GPIO_Init_OLED(void);
void Init_Oled(void);
u8* Data_Command(u8 len, u8* ptr);
void Clear_Screen(void);//Clear the screen
/*Functions for writing to the screen*/
void OLED_DrawString(char* string, uint8_t length);
void OLED_DrawChar(char c, uint8_t x);
void OLED_DrawTwoNumber(uint8_t in, uint8_t x);
void OLED_DrawThreeNumber(uint16_t in, uint8_t x);

#endif
/******************************** END OF FILE *********************************/
