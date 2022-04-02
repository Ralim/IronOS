set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/ethernet)
set(TARGET_REQUIRED_LIBS freertos lwip)
set(TARGET_REQUIRED_SRCS http_server.c
http_led.c
# fsdata_custom.c
${BSP_COMMON_DIR}/ethernet/ethernetif.c
${BSP_COMMON_DIR}/ethernet/emac_phy.c)

set(mains main.c )

generate_bin()
