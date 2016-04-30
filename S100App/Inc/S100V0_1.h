/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************
 Brief   : 底层硬件配置                                          Author : bure
*******************************************************************************/
#include "STM32F10x.h"

#define PRODUCT_INFO       "TS100"
#define MCU_TYPE           "STM32F103T8"
#define ADC_TYPE           "MCU's ADC"
#define FPGA_TYPE          "None"
#define SCH_VER            "2.46"
//  #define SSD1316             1
//  #define MFTSEEED           "Manufacturer"
//  #define MFTMINI            "Manufacturer"

#define SPIx                SPI1
#define FLASH_PAGE          0x0400
//--------------------------- key 相关控制信号 ------------------------------//
#define KEY1_PIN          GPIO_Pin_9    //PA8
#define KEY2_PIN          GPIO_Pin_6    //PA6
#define KEY_1             !(GPIOA->IDR & KEY1_PIN)
#define KEY_2             !(GPIOA->IDR & KEY2_PIN)
#define KEY_DFU           KEY_1
#define KEY_ON            0

#define I2C1_DMA_CHANNEL_TX           DMA1_Channel6
#define I2C1_DMA_CHANNEL_RX           DMA1_Channel7

#define I2C2_DMA_CHANNEL_TX           DMA1_Channel4
#define I2C2_DMA_CHANNEL_RX           DMA1_Channel5

#define I2C1_DR_Address               0x40005410
#define I2C2_DR_Address               0x40005810


#define SERIAL_NO1         (*(u32*)0x1FFFF7E8)
#define SERIAL_NO2         (*(u32*)0x1FFFF7EC)
#define SERIAL_NO3         (*(u32*)0x1FFFF7F0)

//--------------------------- OLED 相关控制信号 ------------------------------//
#define OLED_RST_PIN        GPIO_Pin_8      //PA9
#define OLED_RST()          GPIO_ResetBits(GPIOA, OLED_RST_PIN)
#define OLED_ACT()          GPIO_SetBits  (GPIOA, OLED_RST_PIN)

//------------------------------ 加热控制信号 --------------------------------//
#define HEAT_PIN            GPIO_Pin_4    //PA15(JTDI)

#define HEAT_OFF()          GPIOB->BRR = HEAT_PIN//GPIO_ResetBits(GPIOB, HEAT_PIN)
#define HEAT_ON()           GPIOB->BSRR = HEAT_PIN//GPIO_SetBits  (GPIOB, HEAT_PIN)

//---------------------输入电压检测--VB---------------------------------------//
#define   VB_PIN            GPIO_Pin_1      //PB1(Ai9)
//----------------------------ADX345 INT1,INT2--------------------------------//
#define   INT1_PIN            GPIO_Pin_5    //PB5  
#define   INT2_PIN            GPIO_Pin_3    //PB3
//------------------------------ GPIO 端口设置 -------------------------------//
  
#define GPIOA_OUTPUT()      GPIOA->ODR  = 0xFFFF;
#define GPIOA_L_DEF()       GPIOA->CRL  = 0x08888888; /* Ai7 K2 xxx xxx xxx xxx xxx xxx */
#define GPIOA_H_DEF()       GPIOA->CRH  = 0x8BBBB883; /* xxx SWC SWD D+  D-  xxx K1 nCR  */
    
#define GPIOB_OUTPUT()      GPIOB->ODR  = 0xFFFF;
#define GPIOB_L_DEF()       GPIOB->CRL  = 0x44838800; /* SDA SCL It1 Po It2 xxx Ai9 Ai8 */
#define GPIOB_H_DEF()       GPIOB->CRH  = 0x88888888; /* xxx xxx xxx xxx xxx xxx xxx xxx */


//--------------------------------- RCC 设置 ---------------------------------//

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

/******************************** END OF FILE *********************************/
