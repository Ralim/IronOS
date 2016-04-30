/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. *******************/
/* Brief : Interrupt Service Routines                           Author : bure */
/******************************************************************************/
#include "Interrupt.h"
#include "usb_istr.h"
#include "Bios.h"
#include "I2C.h"

/******************************************************************************/
/*                      Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void)
{
    ;
}

void HardFault_Handler(void)
{
    while (1);
}

void MemManage_Handler(void)
{
    while (1);
}

void BusFault_Handler(void)
{
    while (1);
}

void UsageFault_Handler(void)
{
    while (1);
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void) {}

/******************************************************************************/
/*                     Peripherals Interrupt Handlers                         */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                               */
/******************************************************************************/

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    USB_Istr();
}
/*
void I2C1_EV_IRQHandler(void)
{
  I2C1_EV_ISR();
}

void I2C1_ER_IRQHandler(void)
{
  I2C1_ER_ISR();
}
*/
void TIM2_IRQHandler(void)
{
    TIM2_ISR();
}

void TIM3_IRQHandler(void)
{
    TIM3_ISR();
}

/*********************************  END OF FILE  ******************************/

