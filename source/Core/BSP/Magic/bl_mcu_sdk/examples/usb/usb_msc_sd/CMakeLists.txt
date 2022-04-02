set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS usb_stack)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb ${BSP_COMMON_DIR}/spi_sd)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/usb/usb_dc.c ${BSP_COMMON_DIR}/usb/uart_interface.c ${BSP_COMMON_DIR}/spi_sd/bsp_spi_sd.c)
set(mains main.c)
generate_bin()


