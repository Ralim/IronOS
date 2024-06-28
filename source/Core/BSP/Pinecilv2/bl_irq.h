#ifndef __BL_IRQ_H__
#define __BL_IRQ_H__


#include "bl702_glb.h"
#include "risc-v/Core/Include/clic.h"
#include "risc-v/Core/Include/riscv_encoding.h"


void bl_irq_enable(unsigned int source);
void bl_irq_disable(unsigned int source);
void bl_irq_pending_set(unsigned int source);
void bl_irq_pending_clear(unsigned int source);
void bl_irq_register(int irqnum, void *handler);
void bl_irq_unregister(int irqnum, void *handler);
void bl_irq_handler_get(int irqnum, void **handler);


#endif
