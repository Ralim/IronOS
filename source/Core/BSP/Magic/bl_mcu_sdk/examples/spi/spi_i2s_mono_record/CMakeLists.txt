set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/es8388 ${BSP_COMMON_DIR}/wm8978 ${BSP_COMMON_DIR}/spi_sd)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/es8388/bsp_es8388.c ${BSP_COMMON_DIR}/wm8978/bsp_wm8978.c )
set(mains main.c)
generate_bin()


