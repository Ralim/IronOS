#ifndef HANDLERS_H_
#define HANDLERS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
uintptr_t handle_trap(uintptr_t mcause, uintptr_t sp);
unsigned long ulSynchTrap(unsigned long mcause, unsigned long sp, unsigned long arg1);

#ifdef __cplusplus
}
#endif
#endif