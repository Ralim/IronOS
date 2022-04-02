set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS lvgl)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/mcu_lcd  ${BSP_COMMON_DIR}/lvgl ${BSP_COMMON_DIR}/touch)
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.c")
set(TARGET_REQUIRED_SRCS  ${sources} ${BSP_COMMON_DIR}/mcu_lcd/mcu_lcd.c ${BSP_COMMON_DIR}/mcu_lcd/ili9341.c ${BSP_COMMON_DIR}/mcu_lcd/st7735s.c ${BSP_COMMON_DIR}/mcu_lcd/st7789v.c ${BSP_COMMON_DIR}/mcu_lcd/font.c
${BSP_COMMON_DIR}/lvgl/lv_port_disp.c ${BSP_COMMON_DIR}/lvgl/lv_port_indev.c
${BSP_COMMON_DIR}/touch/xpt2046.c ${BSP_COMMON_DIR}/touch/touch.c )
list(APPEND GLOBAL_C_FLAGS -DLV_USING_FATFS=0 -DLV_USING_ROMFS=0)
set(mains main.c)
generate_bin()


