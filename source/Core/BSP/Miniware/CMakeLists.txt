include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/CMSIS/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/CMSIS/Device)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Inc)
set(sources 
BSP.cpp
flash.c
fusb_user.cpp
I2C_Wrapper.cpp
IRQ.cpp
port.c
postRTOS.cpp
Power.cpp
preRTOS.cpp
QC_GPIO.cpp
Setup.cpp
startup_stm32f103t8ux.S
stm32f1xx_hal_msp.c
stm32f1xx_hal_timebase_TIM.c
stm32f1xx_it.c
system_stm32f1xx.c
ThermoModel.cpp
# ST code
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
)


add_library(BSPImplementation ${sources})
target_include_directories(BSPImplementation PUBLIC . ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/CMSIS/Include ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/CMSIS/Device ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/STM32F1xx_HAL_Driver/Inc)
target_compile_definitions (BSPImplementation PUBLIC "-D STM32F103T8Ux -D STM32F1 -D STM32 -D USE_HAL_DRIVER -D STM32F103xB -D USE_RTOS_SYSTICK -D GCC_ARMCM3 -D ARM_MATH_CM3 	-D STM32F10X_MD")
target_link_libraries(BSPImplementation PUBLIC brieflz BSP mainSource drivers)


target_link_libraries(${elf_file} PUBLIC -Wl,--defsym=__FLASH_SIZE__=64k -Wl,--defsym=__BOOTLDR_SIZE__=0x4000)

set(DEVICE_DFU_ADDRESS 0x08004000)
set(DEVICE_DFU_VID_PID 0x1209:0xDB42)