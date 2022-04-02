/****************************************************************************************
* @file map.txt
*
* @brief This file is the map file (gnuarm or armgcc).
*
* Copyright (C) BouffaloLab 2018
*
****************************************************************************************
*/

/* configure the CPU type */
OUTPUT_ARCH( "riscv" )
/* link with the standard c library */
INPUT(-lc)
/* link with the standard GCC library */
INPUT(-lgcc)
/* configure the entry point */
ENTRY(_enter)

StackSize = 0x1000; /*  4KB */
HeapSize  = 0x0;    /*  0KB */
PROVIDE(__boot2_pass_param_addr = 0x4202DC00);

MEMORY
{
    xip_memory (rx) : 	ORIGIN = 0x23000000, LENGTH = 64K
   	itcm_memory (rx)  : ORIGIN = 0x22014000, LENGTH = 16K
    dtcm_memory (rx)  : ORIGIN = 0x42018000, LENGTH = 4K
    ram_memory  (!rx) : ORIGIN = 0x42019000/*0x42020000*/, LENGTH = 88K
}

SECTIONS
{
    PROVIDE(__metal_chicken_bit = 0);

    .text :
    {
        . = ALIGN(4);
        __text_code_start__ = .;

        KEEP (*(.text.metal.init.enter))
        KEEP (*(SORT_NONE(.init)))

        /* section information for usb desc */
        . = ALIGN(4);
        _usb_desc_start = .;
        KEEP(*(usb_desc))
        . = ALIGN(4);
        _usb_desc_end = .;

        *(EXCLUDE_FILE ( *bl702_uart*.o* *hal_uart*.o* ) .text*)

        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)

        . = ALIGN(4);
        __text_code_end__ = .;
    } > xip_memory

    . = ALIGN(4);
    __itcm_load_addr = .;

    .itcm_region : AT (__itcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_code_start__ = .;
        *(.tcm_code.*)
        *(.tcm_const.*)
        *(.sclock_rlt_code.*)
        *(.sclock_rlt_const.*)
        *bl702_romapi*.o*(.text)
        *bl702_romapi*.o*(.text.*)
        *bl702_romapi*.o*(.rodata)
        *bl702_romapi*.o*(.rodata.*)
        *bl702_romapi*.o*(.srodata)
        *bl702_romapi*.o*(.srodata.*)
        *bl702_uart*.o* (.text*)
        *hal_uart*.o* (.text*)
        . = ALIGN(4);
        __tcm_code_end__ = .;

    } > itcm_memory

    __dtcm_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .dtcm_region : AT (__dtcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data)
        /* *finger_print.o(.data*) */

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > dtcm_memory

	/* .heap_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of heap sections, and assign
     * values to heap symbols later */
    .heap_dummy (NOLOAD):
    {
        . = ALIGN(0x4);
        . = . + HeapSize;
        . = ALIGN(0x4);
    } > dtcm_memory

    __HeapBase = ORIGIN(dtcm_memory) + LENGTH(dtcm_memory) - StackSize - HeapSize;
    __HeapSize = HeapSize;
    __HeapLimit = __HeapBase + __HeapSize;
    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__HeapBase >= __tcm_data_end__, "region RAM overflowed with stack")

    /*************************************************************************/
    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(0x4);
        . = . + StackSize;
        . = ALIGN(0x4);
    } > dtcm_memory

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(dtcm_memory) + LENGTH(dtcm_memory);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __tcm_data_end__, "region RAM overflowed with stack")
    /*************************************************************************/

    __system_ram_load_addr = __dtcm_load_addr + SIZEOF(.dtcm_region);

    .system_ram_data_region : AT (__system_ram_load_addr)
    {
        . = ALIGN(4);
        __system_ram_data_start__ = .;

        *(.system_ram)

        . = ALIGN(4);
        __system_ram_data_end__ = .;
    } > ram_memory

    __ram_load_addr = __system_ram_load_addr + SIZEOF(.system_ram_data_region);

    /* Data section */
    RAM_DATA : AT (__ram_load_addr)
    {
        . = ALIGN(4);
        __ram_data_start__ = .;

        PROVIDE( __global_pointer$ = . + 0x800 );

        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        *(.sdata2)
        *(.sdata2.*)

        . = ALIGN(4);
        __ram_data_end__ = .;
    } > ram_memory

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;

        *(.bss*)
        *(.sbss*)
        *(COMMON)

        . = ALIGN(4);
        __bss_end__ = .;
    } > ram_memory

    .noinit_data (NOLOAD) :
    {
        . = ALIGN(4);
        __noinit_data_start__ = .;

        *(.noinit_data*)

        . = ALIGN(4);
        __noinit_data_end__ = .;
    } > ram_memory


}

