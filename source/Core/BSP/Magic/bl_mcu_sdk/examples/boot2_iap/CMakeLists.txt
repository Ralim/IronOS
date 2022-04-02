set(TARGET_REQUIRED_LIBS xz)

if((${CHIP} STREQUAL "bl702"))
    list(APPEND TARGET_REQUIRED_LIBS usb_stack )
    set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
    set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb)
endif()

list(APPEND TARGET_REQUIRED_SRCS blsp_common.c blsp_media_boot.c )
list(APPEND TARGET_REQUIRED_SRCS blsp_boot_parser.c blsp_boot_decompress.c blsp_port.c )
list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_uart.c  ) #bflb_eflash_loader_gpio.c
list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_cmds.c )
list(APPEND TARGET_REQUIRED_SRCS blsp_ram_image_boot.c )

list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_interface.c )
if((${CHIP} STREQUAL "bl702"))
    list(APPEND TARGET_REQUIRED_SRCS bflb_eflash_loader_usb.c )
    list(APPEND TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/usb/usb_dc.c ${BSP_COMMON_DIR}/usb/uart_interface.c )
endif()

SET(LINKER_SCRIPT ${BOOT2_LINKER_SCRIPT})

list(APPEND GLOBAL_C_FLAGS -DCONFIG_ERR_NOT_DUMP_MCAUSE_INFO)
list(APPEND GLOBAL_C_FLAGS -DCONFIG_ERR_NOT_DUMP_ALL_REGS)
#list(APPEND GLOBAL_C_FLAGS -D_vsnprintf=vsnprintf)
list(APPEND GLOBAL_C_FLAGS -DCONFIG_SUPPORT_UART_CLOCK_GET_ONLY)
list(APPEND GLOBAL_C_FLAGS -D__NO_HBN_RECV_IN_STARTUP)
list(APPEND GLOBAL_C_FLAGS -DCONFIG_SUPPORT_U32_DIV_ONLY)
list(REMOVE_ITEM GLOBAL_C_FLAGS -O2)
list(APPEND GLOBAL_C_FLAGS -Os)


set(mains main.c)
generate_bin()



