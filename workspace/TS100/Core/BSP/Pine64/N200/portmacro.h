#ifndef PINE_PORTMACRO_H
#define PINE_PORTMACRO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "riscv_encoding.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR char
#define portFLOAT float
#define portDOUBLE double
#define portLONG long
#define portSHORT short
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE long

	typedef portSTACK_TYPE StackType_t;
	typedef long BaseType_t;
	typedef unsigned long UBaseType_t;

#if (configUSE_16_BIT_TICKS == 1)
	typedef uint16_t TickType_t;
#define portMAX_DELAY (TickType_t)0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY (TickType_t)0xffffffffUL

#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH (-1)
#define portTICK_PERIOD_MS ((TickType_t)1000 / configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT 8
	/*-----------------------------------------------------------*/

	/* Architecture specifics. */
	extern void vPortYield(unsigned long, unsigned long);
	extern void vPortYield_from_ulSynchTrap(unsigned long, unsigned long);
	extern int xPortSetInterruptMask(void);
	extern void vPortClearInterruptMask(int uxSavedStatusValue);

/*-----------------------------------------------------------*/
/*System Calls												 */
/*-----------------------------------------------------------*/
//ecall macro used to store argument in a3
#define ECALL(arg) ({                                   \
	register uintptr_t a2 asm("a2") = (uintptr_t)(arg); \
	asm volatile("ecall"                                \
				 : "+r"(a2)                             \
				 :                                      \
				 : "memory");                           \
	a2;                                                 \
})

	extern void vPortSetMSIPInt(void);
#define port_MSIPSET_BIT vPortSetMSIPInt()

#define IRQ_DISABLE 20
#define IRQ_ENABLE 30
#define PORT_YIELD 40
#define PORT_YIELD_TO_RA 50
/*-----------------------------------------------------------*/

/* Scheduler utilities. */
/* the return after the ECALL is VERY important */

//#define portYIELD() ECALL(PORT_YIELD);
#define portYIELD() port_MSIPSET_BIT;

#ifdef CONFIG_SYSTEMVIEW_EN
#define portEND_SWITCHING_ISR(xSwitchRequired) \
	{                                          \
		if (xSwitchRequired != pdFALSE)        \
		{                                      \
			traceISR_EXIT_TO_SCHEDULER();      \
			portYIELD();                       \
		}                                      \
		else                                   \
		{                                      \
			traceISR_EXIT();                   \
		}                                      \
	}
#else
#define portEND_SWITCHING_ISR(xSwitchRequired) \
	if (xSwitchRequired != pdFALSE)            \
	portYIELD()
#endif
#define portYIELD_FROM_ISR(x) portEND_SWITCHING_ISR(x)

	/* Critical section management. */
	extern void vPortEnterCritical(void);
	extern void vPortExitCritical(void);
	extern void eclic_set_mth(uint8_t mth);
#define portDISABLE_INTERRUPTS()                                      \
	{                                                                 \
		eclic_set_mth((configMAX_SYSCALL_INTERRUPT_PRIORITY) | 0x1f); \
		__asm volatile("fence");                                      \
		__asm volatile("fence.i");                                    \
	}
#define portENABLE_INTERRUPTS() eclic_set_mth(0)
#define portSET_INTERRUPT_MASK_FROM_ISR() xPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedStatusValue) vPortClearInterruptMask(uxSavedStatusValue)
#define portENTER_CRITICAL() vPortEnterCritical()
#define portEXIT_CRITICAL() vPortExitCritical()

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not necessary for to use this port.  They are defined so the common demo files
(which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) void vFunction(void *pvParameters)
/*-----------------------------------------------------------*/

/* Tickless idle/low power functionality. */
#ifndef portSUPPRESS_TICKS_AND_SLEEP
	extern void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime) vPortSuppressTicksAndSleep(xExpectedIdleTime)
#endif
	/*-----------------------------------------------------------*/

#define portINLINE __inline

#ifndef portFORCE_INLINE
#define portFORCE_INLINE inline __attribute__((always_inline))
#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
