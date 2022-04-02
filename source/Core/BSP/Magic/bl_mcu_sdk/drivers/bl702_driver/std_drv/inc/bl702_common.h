#ifndef __BL702_COMMON_H__
#define __BL702_COMMON_H__

#include "bl702.h"
#include "misc.h"

#ifndef __NOP
#define __NOP() __ASM volatile("nop") /* This implementation generates debug information */
#endif
#ifndef __WFI
#define __WFI() __ASM volatile("wfi") /* This implementation generates debug information */
#endif
#ifndef __WFE
#define __WFE() __ASM volatile("wfe") /* This implementation generates debug information */
#endif
#ifndef __SEV
#define __SEV() __ASM volatile("sev") /* This implementation generates debug information */
#endif
#ifndef __set_MSP
#define __set_MSP(msp) __ASM volatile("add sp, x0, %0" ::"r"(msp))
#endif

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value)
{
    //return __builtin_bswap32(value);
    uint32_t res = 0;

    res = (value << 24) | (value >> 24);
    res &= 0xFF0000FF; /* only for sure */
    res |= ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000);

    return res;
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value)
{
    return __builtin_bswap16(value);
}

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by setting the IE-bit in the PSR.
           Can only be executed in Privileged modes.
 */
__ALWAYS_STATIC_INLINE void __enable_irq(void)
{
    __ASM volatile("csrs mstatus, 8");
}

/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by clearing the IE-bit in the PSR.
  Can only be executed in Privileged modes.
 */
__ALWAYS_STATIC_INLINE void __disable_irq(void)
{
    __ASM volatile("csrc mstatus, 8");
}

/** @defgroup  COMMON_Public_Constants
 *  @{
 */

/** @defgroup DRIVER_INT_PERIPH
 *  @{
 */
#define IS_INT_PERIPH(INT_PERIPH) ((INT_PERIPH) < IRQn_LAST)

/*@} end of group DRIVER_INT_PERIPH */

/** @defgroup DRIVER_INT_MASK
 *  @{
 */
#define IS_BL_MASK_TYPE(type) (((type) == MASK) || ((type) == UNMASK))

/*@} end of group COMMON_Public_Constants */

/*@} end of group DRIVER_Public_Macro */
#define BL702_MemCpy      arch_memcpy
#define BL702_MemSet      arch_memset
#define BL702_MemCmp      arch_memcmp
#define BL702_MemCpy4     arch_memcpy4
#define BL702_MemCpy_Fast arch_memcpy_fast
#define BL702_MemSet4     arch_memset4

#define arch_delay_us BL702_Delay_US
#define arch_delay_ms BL702_Delay_MS

void BL702_Delay_US(uint32_t cnt);
void BL702_Delay_MS(uint32_t cnt);
#endif
