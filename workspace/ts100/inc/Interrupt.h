/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. *******************/
/* Brief : Interrupt Service Routines                           Author : bure */
/******************************************************************************/
#ifndef __INTERRUPT_H
#define __INTERRUPT_H
//See here for refernce to this block http://embedded.kleier.selfhost.me/vector.php
static void forever (void);
void __attribute__ ((weak, alias ("forever"))) WWDG_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) PVD_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TAMPER_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) RTC_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) FLASH_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) RCC_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) EXTI0_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) EXTI1_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) EXTI2_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) EXTI3_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) EXTI4_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA1_Channel1_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA1_Channel2_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA1_Channel3_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA1_Channel4_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA1_Channel5_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA1_Channel6_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA1_Channel7_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) ADC1_2_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) USB_HP_CAN1_TX_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) CAN1_RX1_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) CAN1_SCE_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) EXTI9_5_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM1_BRK_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM1_UP_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM1_TRG_COM_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM1_CC_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM4_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) I2C1_EV_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) I2C1_ER_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) I2C2_EV_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) I2C2_ER_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) SPI1_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) SPI2_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) USART1_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) USART2_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) USART3_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) EXTI15_10_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) RTCAlarm_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) USBWakeUp_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM8_BRK_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM8_UP_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM8_TRG_COM_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM8_CC_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) ADC3_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) FSMC_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) SDIO_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM5_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) SPI3_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) UART4_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) UART5_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM6_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) TIM7_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA2_Channel1_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA2_Channel2_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA2_Channel3_IRQHandler (void);
void __attribute__ ((weak, alias ("forever"))) DMA2_Channel4_5_IRQHandler (void);



void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void WWDG_IRQHandler(void);
void PVD_IRQHandler(void);
void TAMPER_STAMP_IRQHandler(void);
void RTC_WKUP_IRQHandler(void);
void FLASH_IRQHandler(void);

void USB_LP_CAN1_RX0_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);



#endif /* __INTERRUPT_H */

/*********************************  END OF FILE  ******************************/
