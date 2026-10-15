#ifndef TS21_INTERRUPTS_H
#define TS21_INTERRUPTS_H

#include "ts21.h"

enum { SysTicK_IRQn = 12, Software_IRQn = 14 };

typedef struct {
  volatile uint32_t CTLR;
  volatile uint32_t SR;
  volatile uint64_t CNT;
  volatile uint64_t CMP;
} TS21SysTick;

#define SysTick ((TS21SysTick *)0xE000F000)

static inline void NVIC_EnableIRQ(unsigned irq) { ((volatile uint32_t *)0xE000E100)[irq / 32] = 1U << (irq % 32); }
static inline void NVIC_SetPendingIRQ(unsigned irq) { ((volatile uint32_t *)0xE000E200)[irq / 32] = 1U << (irq % 32); }
static inline void NVIC_SetPriority(unsigned irq, uint8_t priority) { ((volatile uint8_t *)0xE000E400)[irq] = priority; }

#ifdef __cplusplus
extern "C" {
#endif
void ts21_tick_reload(void);
#ifdef __cplusplus
}
#endif

#endif
