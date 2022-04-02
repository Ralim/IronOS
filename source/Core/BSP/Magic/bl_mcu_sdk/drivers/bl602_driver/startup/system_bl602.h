#ifndef __SYSTEM_BL602_H__
#define __SYSTEM_BL602_H__

/**
 *  @brief PLL Clock type definition
 */

extern uint32_t SystemCoreClock;
typedef void (*pFunc)(void);

#define CPU_Interrupt_Enable        clic_enable_interrupt
#define CPU_Interrupt_Disable       clic_disable_interrupt
#define CPU_Interrupt_Pending_Clear clic_clear_pending

extern void SystemCoreClockUpdate(void);
extern void SystemInit(void);
void clic_enable_interrupt(uint32_t source);
void clic_disable_interrupt(uint32_t source);
void clic_clear_pending(uint32_t source);
void Interrupt_Handler_Register(IRQn_Type irq, pFunc interruptFun);
void System_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
#endif
