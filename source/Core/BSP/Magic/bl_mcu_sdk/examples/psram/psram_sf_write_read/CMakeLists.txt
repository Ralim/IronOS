set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/psram)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/psram/bsp_sf_psram.c)
set(mains main.c)
generate_bin()



