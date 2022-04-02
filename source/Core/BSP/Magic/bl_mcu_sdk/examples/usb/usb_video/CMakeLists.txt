set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_LIBS usb_stack)
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/usb_cdc_video_psram.ld)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${BSP_COMMON_DIR}/usb ${BSP_COMMON_DIR}/psram ${BSP_COMMON_DIR}/image_sensor)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/usb/usb_dc.c ${BSP_COMMON_DIR}/psram/bsp_sf_psram.c ${BSP_COMMON_DIR}/image_sensor/bsp_image_sensor.c)
set(mains main.c)
generate_bin()


