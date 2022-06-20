include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/NMSIS/Core/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OS/FreeRTOS/Source/portable/GCC)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/)


set(sources 
# Vendor Code
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OS/FreeRTOS/Source/portable/GCC/portasm.S
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OS/FreeRTOS/Source/portable/GCC/port.c

${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/gd32vf103_soc.c     
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/system_gd32vf103.c

${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_adc.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_bkp.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_crc.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_dac.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_dbg.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_dma.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_eclic.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_exmc.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_exti.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_fmc.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_fwdgt.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_gpio.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_i2c.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_pmu.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_rcu.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_rtc.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_spi.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_timer.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_usart.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/gd32vf103_wwdgt.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Drivers/n200_func.c

${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/GCC/intexc_gd32vf103.S
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/GCC/startup_gd32vf103.S

${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/close.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/fstat.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/gettimeofday.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/isatty.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/lseek.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/read.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/sbrk.c
    BSP.cpp   
    Debug.cpp 
    I2C_Wrapper.cpp   
    postRTOS.cpp
    preRTOS.cpp
    flash.c   
    fusb_user.cpp
    ThermoModel.cpp   
    IRQ.cpp   
    Setup.cpp 
    Power.cpp 
    QC_GPIO.cpp   

)


add_library(BSPImplementation ${sources})
target_include_directories(BSPImplementation PUBLIC .  ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/NMSIS/Core/Include ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/OS/FreeRTOS/Source/portable/GCC ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Include ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/ ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/SoC/gd32vf103/Common/Source/Stubs/)
target_compile_definitions (BSPImplementation PUBLIC -DRTOS_FREERTOS -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP -DUSE_RTOS_SYSTICK  -DVECT_TAB_OFFSET=0U)
target_link_libraries(BSPImplementation PRIVATE brieflz BSP threads drivers mainSource)


# target_link_libraries(${elf_file} PUBLIC -Wl,--defsym=__FLASH_SIZE__=128k -Wl,--defsym=__BOOTLDR_SIZE__=0)

set(DEVICE_DFU_ADDRESS 0x08000000)
set(DEVICE_DFU_VID_PID 0x28E9:0x0189)