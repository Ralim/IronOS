#include "bl_irq.h"

extern pFunc __Interrupt_Handlers[IRQn_LAST];

void bl_irq_enable(unsigned int source) { *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 1; }

void bl_irq_disable(unsigned int source) { *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 0; }

void bl_irq_pending_set(unsigned int source) { *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 1; }

void bl_irq_pending_clear(unsigned int source) { *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 0; }

void bl_irq_register(int irqnum, void *handler) {
  if (irqnum < IRQn_LAST) {
    __Interrupt_Handlers[irqnum] = handler;
  }
}

void bl_irq_unregister(int irqnum, void *handler) {
  if (irqnum < IRQn_LAST) {
    __Interrupt_Handlers[irqnum] = NULL;
  }
}

void bl_irq_handler_get(int irqnum, void **handler) {
  if (irqnum < IRQn_LAST) {
    *handler = __Interrupt_Handlers[irqnum];
  }
}
