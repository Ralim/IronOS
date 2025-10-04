# Model-specific configuration for IronOS
# This file configures compiler toolchain, CPU flags, memory layout, and device-specific settings

# Initialize model-specific variables
set(DEVICE_BSP_DIR "")
set(LDSCRIPT "")
set(FLASH_SIZE "")
set(BOOTLDR_SIZE "")
set(DEVICE_DFU_ADDRESS "0x08000000")
set(CPU_FLAGS "")
set(DEV_GLOBAL_DEFS "")
set(DEV_LDFLAGS "")
set(DEV_AFLAGS "")
set(DEV_CFLAGS "")
set(DEV_CXXFLAGS "")
set(COMPILER_PREFIX "")

# Miniware models (TS100, TS80, TS80P, TS101)
if(MODEL IN_LIST ALL_MINIWARE_MODELS)
    message(STATUS "Building for Miniware")

    set(DEVICE_BSP_DIR "${CMAKE_SOURCE_DIR}/Core/BSP/Miniware")
    set(LDSCRIPT "${CMAKE_SOURCE_DIR}/Core/BSP/Miniware/stm32f103.ld")
    set(COMPILER_PREFIX "arm-none-eabi")

    if(MODEL STREQUAL "TS101")
        # 128K, but logo must be at 99K so their broken ass DFU can flash it
        set(FLASH_SIZE "98k")
        set(BOOTLDR_SIZE "0x8000")
        set(DEVICE_DFU_ADDRESS "0x08008000")
    else()
        set(FLASH_SIZE "62k")
        set(BOOTLDR_SIZE "0x4000")
        set(DEVICE_DFU_ADDRESS "0x08004000")
    endif()

    set(DEV_GLOBAL_DEFS
        -DSTM32F103T8Ux
        -DSTM32F1
        -DSTM32
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
        -DUSE_RTOS_SYSTICK
        -DGCC_ARMCM3
        -DARM_MATH_CM3
        -DSTM32F10X_MD
        -finline-limit=9999999
    )

    set(DEV_LDFLAGS "-Wl,--wrap=printf" "-Wl,--no-wchar-size-warning")
    set(DEV_AFLAGS "")
    string(CONCAT VECT_TAB_DEFINE "-DVECT_TAB_OFFSET=" "${BOOTLDR_SIZE}" "U")
    set(DEV_CFLAGS "${VECT_TAB_DEFINE}")
    set(DEV_CXXFLAGS "")

    set(CPU_FLAGS
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=soft
    )

    set(DEVICE_DFU_VID_PID "0x1209:0xDB42")
endif()

# Sequre models (S60, S60P, T55)
if(MODEL IN_LIST ALL_SEQURE_MODELS)
    message(STATUS "Building for Sequre")

    set(DEVICE_BSP_DIR "${CMAKE_SOURCE_DIR}/Core/BSP/Sequre")
    set(LDSCRIPT "${CMAKE_SOURCE_DIR}/Core/BSP/Sequre/stm32f103.ld")
    set(COMPILER_PREFIX "arm-none-eabi")

    set(DEV_GLOBAL_DEFS
        -DSTM32F103T8Ux
        -DSTM32F1
        -DSTM32
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
        -DUSE_RTOS_SYSTICK
        -DGCC_ARMCM3
        -DARM_MATH_CM3
        -DSTM32F10X_MD
        -finline-limit=9999999
    )

    set(DEV_LDFLAGS "-Wl,--wrap=printf" "-Wl,--no-wchar-size-warning")
    set(DEV_AFLAGS "")
    set(DEV_CXXFLAGS "")

    set(CPU_FLAGS
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=soft
    )

    set(FLASH_SIZE "62k")
    if(MODEL STREQUAL "S60P")
        set(BOOTLDR_SIZE "0x5000")
        set(DEVICE_DFU_ADDRESS "0x08005000")
    else()
        # S60 or T55
        set(BOOTLDR_SIZE "0x4400")
        set(DEVICE_DFU_ADDRESS "0x08004400")
    endif()

    set(DEVICE_DFU_VID_PID "0x1209:0xDB42")

    # Set DEV_CFLAGS after BOOTLDR_SIZE is determined
    string(CONCAT VECT_TAB_DEFINE "-DVECT_TAB_OFFSET=" "${BOOTLDR_SIZE}" "U")
    set(DEV_CFLAGS "${VECT_TAB_DEFINE}")
endif()

# MHP30 models
if(MODEL IN_LIST ALL_MHP30_MODELS)
    message(STATUS "Building for MHP30")

    set(DEVICE_BSP_DIR "${CMAKE_SOURCE_DIR}/Core/BSP/MHP30")
    set(LDSCRIPT "${CMAKE_SOURCE_DIR}/Core/BSP/MHP30/stm32f103.ld")
    set(COMPILER_PREFIX "arm-none-eabi")

    set(DEV_GLOBAL_DEFS
        -DSTM32F103T8Ux
        -DSTM32F1
        -DSTM32
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
        -DUSE_RTOS_SYSTICK
        -DGCC_ARMCM3
        -DARM_MATH_CM3
        -DSTM32F10X_MD
    )

    set(DEV_LDFLAGS "")
    set(DEV_AFLAGS "")
    set(DEV_CXXFLAGS "")

    set(CPU_FLAGS
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=soft
    )

    set(FLASH_SIZE "126k")
    set(BOOTLDR_SIZE "32k")
    set(DEVICE_DFU_ADDRESS "0x08008000")
    set(DEVICE_DFU_VID_PID "0x1209:0xDB42")

    # Set DEV_CFLAGS after BOOTLDR_SIZE is determined
    string(CONCAT VECT_TAB_DEFINE "-DVECT_TAB_OFFSET=" "${BOOTLDR_SIZE}" "U")
    set(DEV_CFLAGS "${VECT_TAB_DEFINE}")
endif()

# Pinecil v1 models
if(MODEL IN_LIST ALL_PINECIL_MODELS)
    message(STATUS "Building for Pine64 Pinecilv1")

    set(DEVICE_BSP_DIR "${CMAKE_SOURCE_DIR}/Core/BSP/Pinecil")
    set(LDSCRIPT "${CMAKE_SOURCE_DIR}/Core/BSP/Pinecil/Vendor/SoC/gd32vf103/Board/pinecil/Source/GCC/gcc_gd32vf103_flashxip.ld")
    set(COMPILER_PREFIX "riscv-none-elf")

    set(FLASH_SIZE "128k")
    set(BOOTLDR_SIZE "0x0")

    set(CPU_FLAGS
        -march=rv32imaczicsr
        -mabi=ilp32
        -mcmodel=medany
        -fsigned-char
        -fno-builtin
        -nostartfiles
    )

    set(DEV_LDFLAGS "-nostartfiles")
    set(DEV_AFLAGS "")
    set(DEV_GLOBAL_DEFS "-DRTOS_FREERTOS" "-DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP")
    string(CONCAT VECT_TAB_DEFINE "-DVECT_TAB_OFFSET=" "${BOOTLDR_SIZE}" "U")
    set(DEV_CFLAGS "${VECT_TAB_DEFINE}")
    set(DEV_CXXFLAGS "")
endif()

# Pinecil v2 models
if(MODEL IN_LIST ALL_PINECIL_V2_MODELS)
    message(STATUS "Building for Pine64 Pinecilv2")

    set(DEVICE_BSP_DIR "${CMAKE_SOURCE_DIR}/Core/BSP/Pinecilv2")
    set(LDSCRIPT "${CMAKE_SOURCE_DIR}/Core/BSP/Pinecilv2/bl_mcu_sdk/drivers/bl702_driver/bl702_flash.ld")
    set(COMPILER_PREFIX "riscv-none-elf")
    set(DEVICE_DFU_ADDRESS "0x23000000")

    # Binary blob directories
    set(PINECILV2_SDK_DIR "${DEVICE_BSP_DIR}/bl_mcu_sdk")
    set(PINECILV2_COMPONENTS_DIR "${PINECILV2_SDK_DIR}/components")
    set(PINECILV2_BLE_CRAPWARE_BLOB_DIR "${PINECILV2_COMPONENTS_DIR}/ble/blecontroller/lib")
    set(PINECILV2_RF_CRAPWARE_BLOB_DIR "${PINECILV2_COMPONENTS_DIR}/ble/bl702_rf/lib")

    set(CPU_FLAGS
        -march=rv32imafczicsr
        -mabi=ilp32f
        -mcmodel=medany
        -fsigned-char
        -fno-builtin
        -nostartfiles
        -DportasmHANDLE_INTERRUPT=FreeRTOS_Interrupt_Handler
        -DARCH_RISCV
        -D__RISCV_FEATURE_MVE=0
        -DBL702
        -DBFLB_USE_ROM_DRIVER=0
    )

    set(DEV_LDFLAGS
        "-nostartfiles"
        "-Wl,--wrap=printf"
        "-Wl,--defsym=__wrap_printf=bflb_platform_printf"
    )

    # Store library paths for later creation of imported targets
    set(PINECILV2_BLE_LIB_PATH "${PINECILV2_BLE_CRAPWARE_BLOB_DIR}/libblecontroller_702_m0s1s.a")
    set(PINECILV2_RF_LIB_PATH "${PINECILV2_RF_CRAPWARE_BLOB_DIR}/libbl702_rf.a")

    set(DEV_AFLAGS "")

    set(DEV_GLOBAL_DEFS
        -DCFG_FREERTOS
        -DARCH_RISCV
        -DBL702
        -DCFG_BLE_ENABLE
        -DBFLB_BLE
        -DCFG_BLE
        -DOPTIMIZE_DATA_EVT_FLOW_FROM_CONTROLLER
        -DBL_MCU_SDK
        -DCFG_CON=1
        -DCFG_BLE_TX_BUFF_DATA=2
        -DCONFIG_BT_PERIPHERAL
        -DCONFIG_BT_L2CAP_DYNAMIC_CHANNEL
        -DCONFIG_BT_GATT_CLIENT
        -DCONFIG_BT_CONN
        -DCONFIG_BT_GATT_DIS_PNP
        -DCONFIG_BT_GATT_DIS_SERIAL_NUMBER
        -DCONFIG_BT_GATT_DIS_FW_REV
        -DCONFIG_BT_GATT_DIS_HW_REV
        -DCONFIG_BT_GATT_DIS_SW_REV
        -DCONFIG_BT_ECC
        -DCONFIG_BT_GATT_DYNAMIC_DB
        -DCONFIG_BT_GATT_SERVICE_CHANGED
        -DCONFIG_BT_KEYS_OVERWRITE_OLDEST
        -DCONFIG_BT_KEYS_SAVE_AGING_COUNTER_ON_PAIRING
        -DCONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS
        -DCONFIG_BT_BONDABLE
        -DCONFIG_BT_HCI_VS_EVT_USER
        -DCONFIG_BT_ASSERT
        -DCONFIG_BT_SIGNING
        -DCONFIG_BT_SETTINGS_CCC_LAZY_LOADING
        -DCONFIG_BT_SETTINGS_USE_PRINTK
        -DCFG_SLEEP
        -DCONFIG_BT_OBSERVER
        -DCONFIG_BT_BROADCASTER
        -DportasmHANDLE_INTERRUPT=FreeRTOS_Interrupt_Handler
        "-DCONFIG_BT_DEVICE_NAME=\"Pinecil\""
        -DCONFIG_BT_DEVICE_APPEARANCE=0x06C1
    )

    # Required to be turned off due to their drivers tripping warnings
    set(DEV_CFLAGS
        "-Wno-error=enum-conversion"
        "-Wno-type-limits"
        "-Wno-implicit-fallthrough"
        "-Wno-error=implicit-function-declaration"
        "-Wno-error=incompatible-pointer-types"
    )
    set(DEV_CXXFLAGS ${DEV_CFLAGS})

    set(FLASH_SIZE "128k")
    set(BOOTLDR_SIZE "0x0")

    # Optional WS2812B support
    if(DEFINED WS2812B_ENABLE)
        list(APPEND DEV_GLOBAL_DEFS "-DWS2812B_ENABLE")
    endif()
endif()

# Validate that model configuration was found
if(NOT DEVICE_BSP_DIR)
    message(FATAL_ERROR "Unknown model: ${MODEL}")
endif()

# Set up cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

if(COMPILER_PREFIX MATCHES "riscv")
    set(CMAKE_SYSTEM_PROCESSOR riscv)
endif()

# Set compiler paths
set(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${COMPILER_PREFIX}-gcc)
set(CMAKE_AR ${COMPILER_PREFIX}-ar)
set(CMAKE_OBJCOPY ${COMPILER_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${COMPILER_PREFIX}-objdump)
set(CMAKE_SIZE ${COMPILER_PREFIX}-size)
set(CMAKE_RANLIB ${COMPILER_PREFIX}-ranlib)

# Prevent CMake from trying to link during compiler testing
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Global defines including model-specific ones
set(GLOBAL_DEFINES
    ${DEV_GLOBAL_DEFS}
    -DUSE_RTOS_SYSTICK
    -DMODEL_${MODEL}
    -fshort-wchar
    -Dgcc
)

# Optional SWD enable
if(DEFINED SWD_ENABLE)
    list(APPEND GLOBAL_DEFINES "-DSWD_ENABLE")
endif()

# Common linker flags
set(LINKER_FLAGS
    -Wl,--gc-sections
    -Wl,--wrap=malloc
    -Wl,--wrap=free
    -Wl,--undefined=vTaskSwitchContext
    -Wl,--undefined=pxCurrentTCB
    -Wl,--defsym=__FLASH_SIZE__=${FLASH_SIZE}
    -Wl,--defsym=__BOOTLDR_SIZE__=${BOOTLDR_SIZE}
    -Wl,--print-memory-usage
    --specs=nosys.specs
    --specs=nano.specs
    ${DEV_LDFLAGS}
)
