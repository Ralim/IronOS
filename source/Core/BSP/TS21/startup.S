    .section .init, "ax", @progbits
    .global _start
    .option push
    .option norvc
_start:
    j reset_handler
    .option pop

    .section .vectors, "a", @progbits
    .balign 4
    .global vector_base
vector_base:
    .word _start
#ifdef TS21_IRONOS
    .rept 11
    .word fault_handler
    .endr
    .word SysTick_Handler
    .word fault_handler
    .word SW_Handler
    .rept 49
    .word fault_handler
    .endr
#else
    .rept 63
    .word fault_handler
    .endr
#endif

    .section .text.startup, "ax", @progbits
    .balign 4
reset_handler:
    csrw mstatus, zero
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, __stack_top
    la t0, __data_load
    la t1, __data_start
    la t2, __data_end
1:
    bgeu t1, t2, 2f
    lw t3, 0(t0)
    sw t3, 0(t1)
    addi t0, t0, 4
    addi t1, t1, 4
    j 1b
2:
    la t1, __bss_start
    la t2, __bss_end
3:
    bgeu t1, t2, 4f
    sw zero, 0(t1)
    addi t1, t1, 4
    j 3b
4:
#ifdef TS21_IRONOS
    li t0, 0x1f
    csrw 0xbc0, t0
    li t0, 2
    csrw 0x804, t0
    li t0, 0x1800
    csrw mstatus, t0
#else
    csrw 0x804, zero
#endif
    la t0, vector_base
    ori t0, t0, 3
    csrw mtvec, t0
#ifdef TS21_IRONOS
    call ts21_run_constructors
    // Leave any bootloader interrupt context before starting FreeRTOS.
    la ra, fault_handler
    la t0, main
    csrw mepc, t0
    mret
#else
    call main
#endif
fault_handler:
    csrw mstatus, zero
    li a0, 64
    tail ts21_fault
