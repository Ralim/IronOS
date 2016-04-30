/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. *******************/
/* Brief : Interrupt Service Routines                           Author : bure */
/******************************************************************************/
#ifndef __INTERRUPT_H
#define __INTERRUPT_H

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
