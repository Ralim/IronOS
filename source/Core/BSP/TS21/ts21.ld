OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08007000, LENGTH = 0x8C00
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 0x5000
}

SECTIONS
{
    .init :
    {
        KEEP(*(.init))
    } > FLASH
    .vectors :
    {
        KEEP(*(.vectors))
    } > FLASH
    .text :
    {
        *(.text .text.*)
        *(.rodata .rodata.* .srodata .srodata.* .sdata2 .sdata2.*)
        . = ALIGN(4);
        __init_array_start = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    } > FLASH
    .data :
    {
        . = ALIGN(4);
        __data_start = .;
        __global_pointer$ = . + 0x800;
        *(.sdata .sdata.* .data .data.*)
        . = ALIGN(4);
        __data_end = .;
    } > RAM AT > FLASH
    __data_load = LOADADDR(.data);
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start = .;
        *(.sbss .sbss.* .bss .bss.* COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } > RAM
    __stack_top = ORIGIN(RAM) + LENGTH(RAM);
    __freertos_irq_stack_top = __stack_top;
    __stack_bottom = __stack_top - 0x800;
    /DISCARD/ :
    {
        *(.eh_frame .eh_frame.* .note .note.*)
    }
}

ASSERT(SIZEOF(.init) == 4, "The Miniware application entry must be one 32-bit jump")
ASSERT(ADDR(.vectors) == 0x08007004, "Unexpected TS21 vector table placement")
ASSERT(SIZEOF(.vectors) == 256, "Unexpected TS21 interrupt vector count")
ASSERT(__bss_end <= __stack_bottom, "TS21 RAM overlaps the reserved stack")
