set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS fatfs)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/il9341)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/il9341/bsp_il9341.c)
set(mains main.c)
generate_bin()


