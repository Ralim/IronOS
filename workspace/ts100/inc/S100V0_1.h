/*
 * Defines for the device
 * These houses the settings for the device hardware
 */
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
//--------------------------- key Definitions ------------------------------//
#define KEY1_PIN          GPIO_Pin_9    //PA8
#define KEY2_PIN          GPIO_Pin_6    //PA6

#define KEY_A			  KEY1_PIN
#define KEY_B			  KEY2_PIN
#define BUT_A			  0x01
#define BUT_B			  0x02


//--------------------------- OLED  ------------------------------//
#define OLED_RST_PIN        GPIO_Pin_8      //PA9
#define OLED_RST()          GPIO_ResetBits(GPIOA, OLED_RST_PIN)
#define OLED_ACT()          GPIO_SetBits  (GPIOA, OLED_RST_PIN)

//------------------------------ Iron Heater --------------------------------//
#define HEAT_PIN            GPIO_Pin_4
#define HEAT_OFF()          GPIOB->BRR = HEAT_PIN
#define HEAT_ON()           GPIOB->BSRR = HEAT_PIN

//-----------------------Battery voltage measurement---------------------------------------//
#define   VB_PIN            GPIO_Pin_1      //PB1(Ai9)

//----------------------------Accelerometer INT1,INT2--------------------------------//
#define   INT1_PIN            GPIO_Pin_5    //PB5  
#define   INT2_PIN            GPIO_Pin_3    //PB3

//--------------------------------- RCC Clock Config ---------------------------------//

#define RCC_PLL_EN()        RCC->CR   |= 0x01000000;// PLL En

#define RCC_CFGR_CFG()      RCC->CFGR |= 0x0068840A;/*RCC peripheral clock config
                                           |||||||+--Bits3~0 = 1010 PLL used as sys clock
                                           ||||||+---Bits7~4 = 0000 AHB clock = SYSCLK
                                           |||||+----Bits10~8  = 100 PCLK1=HCLK divided by 2
                                           ||||++----Bits13~11 = 000 PCLK2=HCLK
                                           ||||+-----Bits15~14 = 10 ADC prescaler PCLK2 divided by 6
                                           |||+------Bit17~16 = 00 HSI/2 clock selected as PLL input clock
                                           ||++------Bits21~18 = 1010 PLL input clock x12
                                           ||+-------Bit22 = 1 USB prescaler is PLL clock
                                           ++--------Bits31~27 Reserved*/

