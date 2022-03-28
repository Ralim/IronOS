#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_CLINT               0
#define portasmADDITIONAL_CONTEXT_SIZE 0 /* Must be even number on 32-bit cores. */

.macro portasmSAVE_ADDITIONAL_REGISTERS
    /* No additional registers to save, so this macro does nothing. */
    .endm

    /* Restore the additional registers found on the Pulpino. */
    .macro portasmRESTORE_ADDITIONAL_REGISTERS
    /* No additional registers to restore, so this macro does nothing. */
    .endm

/* hkim temporary workaround */
#define portasmMCAUSE_MASK      0xfff
#define portasmHANDLE_INTERRUPT irq_entry
#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */