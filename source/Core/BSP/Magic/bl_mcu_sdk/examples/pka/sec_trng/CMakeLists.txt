set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_PRIVATE_INCLUDE
${BSP_COMMON_DIR}/il9341
${BSP_COMMON_DIR}/touch )
set(TARGET_REQUIRED_SRCS
${BSP_COMMON_DIR}/il9341/bsp_il9341.c
${BSP_COMMON_DIR}/touch/touch.c
${BSP_COMMON_DIR}/touch/xpt2046.c)
set(mains main.c)
generate_bin()



