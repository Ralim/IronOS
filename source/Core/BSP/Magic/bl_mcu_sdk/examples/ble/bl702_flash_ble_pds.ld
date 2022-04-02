/****************************************************************************************
* @file bl702_flash_pds.ld
*
* @brief This file is the map file (gnuarm or armgcc).
*
* Copyright (C) BouffaloLab 2021
*
****************************************************************************************
*/

/* configure the CPU type */
OUTPUT_ARCH( "riscv" )
/* link with the standard c library */
/* INPUT(-lc) */
/* link with the standard GCC library */
/* INPUT(-lgcc) */
/* configure the entry point */
ENTRY(_enter)

StackSize = 0x1000; /*  4KB */
HeapSize  = 0x800; /*  2KB */
__EM_SIZE = DEFINED(ble_controller_init) ? 8K : 0K;

MEMORY
{
    xip_memory  (rx)  : ORIGIN = 0x23000000, LENGTH = 1024K
    itcm_memory (rx)  : ORIGIN = 0x22014000, LENGTH = 16K
    dtcm_memory (rx)  : ORIGIN = 0x42018000, LENGTH = 32K
    ram_memory  (!rx) : ORIGIN = 0x42020000, LENGTH = 20K /* OCRAM_1*/
    ram1_memory (!rx) : ORIGIN = 0x42025000, LENGTH = 4K
    rsvd_memory (!rx) : ORIGIN = 0x42026000, LENGTH = 16K /*OCRAM_2*/
    ram2_memory (!rx) : ORIGIN = 0x4202A000, LENGTH = (24K - __EM_SIZE) /*OCRAM_3*/
    hbn_memory  (rx)  : ORIGIN = 0x40010000, LENGTH = 0xE00
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
        /* section information for shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;

        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;

        /* section information for usb desc */
        . = ALIGN(4);
        _usb_desc_start = .;
        KEEP(*(usb_desc))
        . = ALIGN(4);
        _usb_desc_end = .;

        *(.text)
        *(.text.*)

        /*put .rodata**/
        *(EXCLUDE_FILE( *bl702_glb*.o* \
                        *bl702_pds*.o* \
                        *bl702_common*.o* \
                        *bl702_sf_cfg*.o* \
                        *bl702_sf_cfg_ext*.o* \
                        *bl702_sf_ctrl*.o* \
                        *bl702_sflash*.o* \
                        *bl702_sflash_ext*.o* \
                        *bl702_xip_sflash*.o* \
                        *bl702_xip_sflash_ext*.o* \
                        *bl702_ef_ctrl*.o*) .rodata*)

        *(.rodata)
        *(.rodata.*)

        *(.srodata)
        *(.srodata.*)

		_bt_gatt_service_static_list_start = .;
        KEEP(*(SORT_BY_NAME("._bt_gatt_service_static.static.*")))
        _bt_gatt_service_static_list_end = .;
        _bt_l2cap_fixed_chan_list_start = .;
        KEEP(*(SORT_BY_NAME("._bt_l2cap_fixed_chan.static.*")))
        _bt_l2cap_fixed_chan_list_end = .;

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

        *bl702_glb*.o*(.rodata*)
        *bl702_pds*.o*(.rodata*)
        *bl702_common*.o*(.rodata*)
        *bl702_sf_cfg*.o*(.rodata*)
        *bl702_sf_cfg_ext*.o*(.rodata*)
        *bl702_sf_ctrl*.o*(.rodata*)
        *bl702_sflash*.o*(.rodata*)
        *bl702_sflash_ext*.o*(.rodata*)
        *bl702_xip_sflash*.o*(.rodata*)
        *bl702_xip_sflash_ext*.o*(.rodata*)
        *bl702_ef_ctrl*.o*(.rodata*)

        . = ALIGN(4);
        __tcm_code_end__ = .;
    } > itcm_memory

    __hbn_load_addr = __itcm_load_addr + SIZEOF(.itcm_region);

    .hbn_ram_region : AT (__hbn_load_addr)
    {
        . = ALIGN(4);
        __hbn_ram_start__ = .;
        *bl702_hbn_wakeup*.o*(.rodata*)
        *(.hbn_ram_code*)
        *(.hbn_ram_data)
        . = ALIGN(4);
        __hbn_ram_end__ = .;
    } > hbn_memory

    __dtcm_load_addr = __hbn_load_addr + SIZEOF(.hbn_ram_region);

    .dtcm_region : AT (__dtcm_load_addr)
    {
        . = ALIGN(4);
        __tcm_data_start__ = .;

        *(.tcm_data)
        /* *finger_print.o(.data*) */

        . = ALIGN(4);
        __tcm_data_end__ = .;
    } > dtcm_memory



    __system_ram_load_addr = __dtcm_load_addr + SIZEOF(.dtcm_region);

    .system_ram_data_region : AT (__system_ram_load_addr)
    {
        . = ALIGN(4);
        __system_ram_data_start__ = .;

        *(.system_ram)
        *hal_pm_util*.o*(.rodata*)
        *(.pds_ram_code*)
        *(.pds_ram_data)
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

    /*************************************************************************/
    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (NOLOAD):
    {
        . = ALIGN(0x4);
        . = . + StackSize;
        . = ALIGN(0x4);
    } > ram_memory

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(ram_memory) + LENGTH(ram_memory);
    PROVIDE( __freertos_irq_stack_top = __StackTop);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __noinit_data_end__, "region RAM overflowed with stack")
	
	
	
    /*************************************************************************/
    .heap (NOLOAD):
    {
        . = ALIGN(8);
        __HeapBase = .;

        /*__end__ = .;*/
        /*end = __end__;*/
        KEEP(*(.heap*))

        . = ALIGN(8);
        . = . + HeapSize;
        . = ALIGN(8);
        __HeapLimit = .;

    } > ram_memory

    /*__HeapBase = __noinit_data_end__;
    __HeapLimit = ORIGIN(ram_memory) + LENGTH(ram_memory) - StackSize;
    */
    /*
    PROVIDE( _heap_start = . );
    PROVIDE( _heap_size = ORIGIN(ram_memory) + LENGTH(ram_memory) - _heap_start );
    */
    PROVIDE( _heap_start = ORIGIN(ram1_memory) );
    PROVIDE( _heap_size = LENGTH(ram1_memory) );

    PROVIDE( _heap2_start = ORIGIN(ram2_memory) );
    PROVIDE( _heap2_size = LENGTH(ram2_memory) );


    /* PDS backup address */
    PROVIDE ( __ld_pds_bak_addr = 0x40010E00 );
}

