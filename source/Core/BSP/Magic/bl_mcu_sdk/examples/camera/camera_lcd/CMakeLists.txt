set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/bl706_camera_psram.ld)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/psram ${BSP_COMMON_DIR}/image_sensor
# ${BSP_COMMON_DIR}/il9341
${BSP_COMMON_DIR}/mcu_lcd)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/psram/bsp_sf_psram.c
${BSP_COMMON_DIR}/image_sensor/bsp_image_sensor.c
# ${BSP_COMMON_DIR}/il9341/bsp_il9341.c
${BSP_COMMON_DIR}/mcu_lcd/mcu_lcd.c
${BSP_COMMON_DIR}/mcu_lcd/ili9341.c
${BSP_COMMON_DIR}/mcu_lcd/font.c)
set(mains main.c)
generate_bin()