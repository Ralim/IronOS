set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)

set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/es8388 )
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/es8388/bsp_es8388.c )
set(mains main.c)

generate_bin()



