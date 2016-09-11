/**
  ******************************************************************************
  * @file    system_stm32f10x.c
  * @author  MCD Application Team
  * @version V3.6.1
  * @date    09-March-2012
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Source File.
  * 
  * 1.  This file provides two functions and one global variable to be called from 
  *     user application:
  *      - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
  *                      factors, AHB/APBx prescalers and Flash settings). 
  *                      This function is called at startup just after reset and 
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32f10x_xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick 
  *                                  timer or configure other parameters.
  *                                     
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  * 2. After each device reset the HSI (8 MHz) is used as system clock source.
  *    Then SystemInit() function is called, in "startup_stm32f10x_xx.s" file, to
  *    configure the system clock before to branch to main program.
  *
  * 3. If the system clock source selected by user fails to startup, the SystemInit()
  *    function will do nothing and HSI still used as system clock source. User can 
  *    add some code to deal with this issue inside the SetSysClock() function.
  *
  * 4. The default value of HSE crystal is set to 8 MHz (or 25 MHz, depedning on
  *    the product used), refer to "HSE_VALUE" define in "stm32f10x.h" file. 
  *    When HSE is used as system clock source, directly or through PLL, and you
  *    are using different crystal you have to adapt the HSE value to your own
  *    configuration.
  *        
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


#include "stm32f10x.h"

    
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field. 
                                  This value must be a multiple of 0x200. */

static void SetSysClockTo72MHz(void);

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
u32 SystemCoreClock   = 72000000; // 72MHz System Clock Frequency
u8  AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

void SystemInit (void)
{
  RCC->CR   |= 0x00000001;// Set HSION bit 
  RCC->CFGR &= 0xF8FF0000;// Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits 
  RCC->CR   &= 0xFEF6FFFF;// Reset HSEON, CSSON and PLLON bits 
  RCC->CR   &= 0xFFFBFFFF;// Reset HSEBYP bit 
  RCC->CFGR &= 0xFF80FFFF;// Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits 
  RCC->CIR   = 0x009F0000;// Disable all interrupts and clear pending bits 
    
  /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
  /* Configure the Flash Latency cycles and enable prefetch buffer */
  SetSysClockTo72MHz();
  SCB->VTOR  = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
}

void SystemCoreClockUpdate (void)
{
  u32 tmp = 0, pllmull = 0, pllsource = 0;

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;
  switch(tmp){
    case 0x00:  // HSI used as system clock 
      SystemCoreClock = 8000000; // HSI_VALUE;
      break;
    case 0x04:  // HSE used as system clock 
      SystemCoreClock = 8000000; // HSE_VALUE;
      break;
    case 0x08:  // PLL used as system clock 
      // Get PLL clock source and multiplication factor 
      pllmull   = RCC->CFGR & RCC_CFGR_PLLMULL;
      pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
      pllmull   = ( pllmull >> 18) + 2;
      // HSI oscillator clock divided by 2 selected as PLL clock entry 
      if (pllsource == 0x00) SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      break;
    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }
  
  // Compute HCLK clock frequency 
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];// Get HCLK prescaler 
  SystemCoreClock >>= tmp; // HCLK clock frequency  
}

static void SetSysClockTo72MHz(void)
{
  u32 StartUpCounter = 0, HSEStatus = 0;
  
  // SYSCLK, HCLK, PCLK2 and PCLK1 configuration    
  RCC->CR |= ((u32)RCC_CR_HSEON);/* Enable HSE */
  do { // Wait till HSE is ready and if Time out is reached exit 
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET) HSEStatus = 0x01;
  else                                    HSEStatus = 0x00;

  if (HSEStatus == 0x01){
    FLASH->ACR |= FLASH_ACR_PRFTBE;/* Enable Prefetch Buffer */
    /* Flash 2 wait state */
    FLASH->ACR &= (u32)((u32)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (u32)FLASH_ACR_LATENCY_2;    
    RCC->CFGR  |= RCC_CFGR_HPRE_DIV1;/* HCLK = SYSCLK */
    RCC->CFGR  |= RCC_CFGR_PPRE2_DIV1;/* PCLK2 = HCLK */
    RCC->CFGR  |= RCC_CFGR_PPRE1_DIV2;/* PCLK1 = HCLK */
    /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
    RCC->CFGR  &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR  |= (u32)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
    RCC->CR    |= RCC_CR_PLLON;/* Enable PLL */
    while((RCC->CR & RCC_CR_PLLRDY) == 0){}/* Wait till PLL is ready */
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {}/* Wait till PLL is used as system clock source */
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
