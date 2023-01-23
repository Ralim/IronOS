ifndef model
model:=Pinecil
endif

ALL_MINIWARE_MODELS=TS100 TS80 TS80P
ALL_PINECIL_MODELS=Pinecil
ALL_PINECIL_V2_MODELS=Pinecilv2
ALL_MHP30_MODELS=MHP30
ALL_MODELS=$(ALL_MINIWARE_MODELS) $(ALL_PINECIL_MODELS) $(ALL_MHP30_MODELS) $(ALL_PINECIL_V2_MODELS)
ifneq ($(model),$(filter $(model),$(ALL_MODELS)))
$(error Invalid model '$(model)', valid options are: $(ALL_MODELS))
endif

# output folder
HEXFILE_DIR=Hexfile
# temporary objects folder
OUTPUT_DIR_BASE=Objects
OUTPUT_DIR=Objects/$(model)

ALL_LANGUAGES=BG CS DA DE EN ES FI FR HR HU IT JA_JP LT NL NL_BE NB PL PT RU SK SL SR_CYRL SR_LATN SV TR UK VI YUE_HK ZH_CN ZH_TW

LANGUAGE_GROUP_CJK_LANGS=EN JA_JP YUE_HK ZH_TW ZH_CN
LANGUAGE_GROUP_CJK_NAME=Chinese+Japanese

ifdef custom_multi_langs
RUN_SHELL_CMD := $(shell rm -Rf {Core/Gen,$(OUTPUT_DIR)/Core/Gen,$(HEXFILE_DIR)/*_Custom.*})
LANGUAGE_GROUP_CUSTOM_LANGS=$(custom_multi_langs)
LANGUAGE_GROUP_CUSTOM_NAME=Custom
endif

LANGUAGE_GROUP_CYRILLIC_LANGS=EN BG RU SR_CYRL SR_LATN UK
LANGUAGE_GROUP_CYRILLIC_NAME=Bulgarian+Russian+Serbian+Ukrainian

LANGUAGE_GROUP_EUR_LANGS=EN $(filter-out $(LANGUAGE_GROUP_CJK_LANGS) $(LANGUAGE_GROUP_CYRILLIC_LANGS),$(ALL_LANGUAGES))
LANGUAGE_GROUP_EUR_NAME=European

LANGUAGE_GROUPS=CUSTOM CJK CYRILLIC EUR


# Defines for host tools
ifeq ($(HOST_CC),)
HOST_CC := gcc
endif
HOST_OUTPUT_DIR=Objects/host

# DFU packing address to use
DEVICE_DFU_ADDRESS=0x08000000
DEVICE_DFU_VID_PID=0x28E9:0x0189
# Enumerate all of the include directories
APP_INC_DIR = ./Core/Inc
BRIEFLZ_INC_DIR = ./Core/brieflz
MINIWARE_INC_CMSIS_DEVICE = ./Core/BSP/Miniware/Vendor/CMSIS/Device/ST/STM32F1xx/Include
MINIWARE_CMSIS_CORE_INC_DIR = ./Core/BSP/Miniware/Vendor/CMSIS/Include
MINIWARE_HAL_INC_DIR = ./Core/BSP/Miniware/Vendor/STM32F1xx_HAL_Driver/Inc
MINIWARE_HAL_LEGACY_INC_DIR = ./Core/BSP/Miniware/Vendor/STM32F1xx_HAL_Driver/Inc/Legacy
MINIWARE_STARTUP_DIR = ./Startup
MINIWARE_INC_DIR = ./Core/BSP/Miniware
MINIWARE_LD_FILE = ./Core/BSP/Miniware/stm32f103.ld

MHP30_INC_CMSIS_DEVICE = ./Core/BSP/MHP30/Vendor/CMSIS/Device/ST/STM32F1xx/Include
MHP30_CMSIS_CORE_INC_DIR = ./Core/BSP/MHP30/Vendor/CMSIS/Include
MHP30_HAL_INC_DIR = ./Core/BSP/MHP30/Vendor/STM32F1xx_HAL_Driver/Inc
MHP30_HAL_LEGACY_INC_DIR = ./Core/BSP/MHP30/Vendor/STM32F1xx_HAL_Driver/Inc/Legacy
MHP30_STARTUP_DIR = ./Startup
MHP30_INC_DIR = ./Core/BSP/MHP30
MHP30_LD_FILE = ./Core/BSP/MHP30/stm32f103.ld

PINE_INC_DIR = ./Core/BSP/Pinecil
PINE_VENDOR_INC_DIR = ./Core/BSP/Pinecil/Vendor/SoC/gd32vf103/Common/Include
PINE_VENDOR_USB_INC_DIR = ./Core/BSP/Pinecil/Vendor/SoC/gd32vf103/Common/Include/Usb
PINE_NMSIS_INC_DIR = ./Core/BSP/Pinecil/Vendor/NMSIS/Core/Include
PINE_FREERTOS_PORT_INC_DIR = ./Core/BSP/Pinecil/Vendor/OS/FreeRTOS/Source/portable/GCC

PINECILV2_DIR = ./Core/BSP/Pinecilv2
PINECILV2_SDK_DIR = $(PINECILV2_DIR)/bl_mcu_sdk

PINECILV2_VENDOR_BSP_DIR = $(PINECILV2_SDK_DIR)/bsp
PINECILV2_VENDOR_BSP_COMMON_DIR = $(PINECILV2_VENDOR_BSP_DIR)/bsp_common
PINECILV2_VENDOR_BSP_BOARD_DIR = $(PINECILV2_VENDOR_BSP_DIR)/board
PINECILV2_VENDOR_BSP_PLATFORM_DIR = $(PINECILV2_VENDOR_BSP_COMMON_DIR)/platform
PINECILV2_VENDOR_BSP_USB_DIR = $(PINECILV2_VENDOR_BSP_COMMON_DIR)/usb


PINECILV2_COMMON_DIR = $(PINECILV2_SDK_DIR)/common
PINECILV2_COMMON_BL_MATH_DIR = $(PINECILV2_COMMON_DIR)/bl_math
PINECILV2_COMMON_DEVICE_DIR = $(PINECILV2_COMMON_DIR)/device
PINECILV2_COMMON_LIST_DIR = $(PINECILV2_COMMON_DIR)/list
PINECILV2_COMMON_MISC_DIR = $(PINECILV2_COMMON_DIR)/misc
PINECILV2_COMMON_PARTITION_DIR = $(PINECILV2_COMMON_DIR)/partition
PINECILV2_COMMON_PID_DIR = $(PINECILV2_COMMON_DIR)/pid
PINECILV2_COMMON_RING_BUFFERDIR = $(PINECILV2_COMMON_DIR)/ring_buffer
PINECILV2_COMMON_SOFT_CRC_DIR = $(PINECILV2_COMMON_DIR)/soft_crc
PINECILV2_COMMON_TIMESTAMP_DIR = $(PINECILV2_COMMON_DIR)/timestamp
PINECILV2_COMPONENTS_DIR = $(PINECILV2_SDK_DIR)/components


PINECILV2_COMPONENTS_FREERTOS_DIR = $(PINECILV2_COMPONENTS_DIR)/freertos
PINECILV2_COMPONENTS_FREERTOS_BL602_DIR = $(PINECILV2_COMPONENTS_FREERTOS_DIR)/portable/gcc/risc-v/bl702

PINECILV2_COMPONENTS_BLE_STACK_PORT_INCLUDE_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/port/include
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/include
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_DRIVERS_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/include/drivers
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_DRIVERS_BLUETOOTH_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/include/drivers/bluetooth
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_BLUETOOTH_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/include/bluetooth
PINECILV2_COMPONENTS_BLE_STACK_COMMON_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common
PINECILV2_COMPONENTS_BLE_STACK_COMMON_INCLUDE_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common/include
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_MISC_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common/include/misc
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_ZEPHYR_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common/include/zephyr
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_NET_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common/include/net
PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_TOOLCHAIN_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common/include/toolchain
PINECILV2_COMPONENTS_BLE_STACK_TINYCRYPT_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common/tinycrypt/include
PINECILV2_COMPONENTS_BLE_STACK_TINYCRYPT_INCLUDE_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/common/tinycrypt/include/tinycrypt
PINECILV2_COMPONENTS_BLE_STACK_HOST_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/host
PINECILV2_COMPONENTS_BLE_STACK_BL_HCI_WRAPPER_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/bl_hci_wrapper
PINECILV2_COMPONENTS_BLE_CONTROLLER_BLE_INC_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/blecontroller/ble_inc
PINECILV2_COMPONENTS_BLE_STACK_BC_DEC_DIR = $(PINECILV2_COMPONENTS_DIR)/ble/ble_stack/sbc/dec


#Binary blobs suck and they should be ashamed
PINECILV2_BLE_CRAPWARE_BLOB_DIR=$(PINECILV2_COMPONENTS_DIR)/ble/blecontroller/lib
PINECILV2_RF_CRAPWARE_BLOB_DIR=$(PINECILV2_COMPONENTS_DIR)/ble/bl702_rf/lib


PINECILV2_COMPONENTS_NMSIS_DIR = $(PINECILV2_COMPONENTS_DIR)/nmsis
PINECILV2_COMPONENTS_NMSIS_CORE_INC_DIR = $(PINECILV2_COMPONENTS_NMSIS_DIR)/core/inc

PINECILV2_COMPONENTS_USB_STACK_DIR = $(PINECILV2_COMPONENTS_DIR)/usb_stack
PINECILV2_COMPONENTS_USB_STACK_COMMON_DIR = $(PINECILV2_COMPONENTS_USB_STACK_DIR)/common
PINECILV2_COMPONENTS_USB_STACK_CORE_DIR = $(PINECILV2_COMPONENTS_USB_STACK_DIR)/core
PINECILV2_COMPONENTS_USB_STACK_CDC_DIR = $(PINECILV2_COMPONENTS_USB_STACK_DIR)/class/cdc
PINECILV2_COMPONENTS_USB_STACK_WINUSB_DIR = $(PINECILV2_COMPONENTS_USB_STACK_DIR)/class/winusb

PINECILV2_DRIVERS_DIR = $(PINECILV2_SDK_DIR)/drivers/bl702_driver
PINECILV2_DRIVERS_HAL_DRV_INC_DIR = $(PINECILV2_DRIVERS_DIR)/hal_drv/inc
PINECILV2_DRIVERS_HAL_DRV_DEF_DIR = $(PINECILV2_DRIVERS_DIR)/hal_drv/default_config
PINECILV2_DRIVERS_REGS_DIR = $(PINECILV2_DRIVERS_DIR)/regs
PINECILV2_DRIVERS_RISCV_DIR = $(PINECILV2_DRIVERS_DIR)/risc-v
PINECILV2_DRIVERS_STARTUP_DIR = $(PINECILV2_DRIVERS_DIR)/startup
PINECILV2_DRIVERS_STD_DRV_DIR = $(PINECILV2_DRIVERS_DIR)/std_drv/inc




SOURCE_MIDDLEWARES_DIR = ./Middlewares
FRTOS_CMIS_INC_DIR = ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
FRTOS_INC_DIR = ./Middlewares/Third_Party/FreeRTOS/Source/include
DRIVER_INC_DIR =./Core/Drivers
BSP_INC_DIR = ./Core/BSP
THREADS_INC_DIR = ./Core/Threads
THREADS_OP_MODES_INC_DIR = ./Core/Threads/OperatingModes
THREADS_OP_MODES_TOOLS_INC_DIR = ./Core/Threads/OperatingModes/utils

SOURCE_THREADS_DIR = ./Core/Threads
SOURCE_CORE_DIR = ./Core/Src
SOURCE_BRIEFLZ_DIR = ./Core/brieflz
SOURCE_DRIVERS_DIR = ./Core/Drivers
INC_PD_DRIVERS_DIR = ./Core/Drivers/usb-pd/include
PD_DRIVER_TESTS_DIR = ./Core/Drivers/usb-pd/tests
PD_DRIVER_DIR = ./Core/Drivers/usb-pd


# Find-all's used for formatting; have to exclude external modules

ALL_INCLUDES = $(shell find ./Core -type d \( -path $(BRIEFLZ_INC_DIR) -o -path $(PD_DRIVER_DIR) -o -path $(PINECILV2_SDK_DIR) \)  -prune -false -o \( -type f \( -name '*.h' -o -name '*.hpp' \) \) )
ALL_SOURCE = $(shell find ./Core -type d  \( -path $(SOURCE_BRIEFLZ_DIR) -o -path $(PD_DRIVER_DIR) -o -path $(PINECILV2_SDK_DIR)  \) -prune -false -o \( -type f \( -name '*.c' -o -name '*.cpp' \) \) )

# Device dependent settings
ifeq ($(model),$(filter $(model),$(ALL_MINIWARE_MODELS)))
$(info Building for Miniware )
DEVICE_INCLUDES = -I$(MINIWARE_INC_DIR) \
	-I$(MINIWARE_INC_CMSIS_DEVICE)\
	-I$(MINIWARE_CMSIS_CORE_INC_DIR)	\
	-I$(MINIWARE_HAL_INC_DIR)	\
	-I$(MINIWARE_HAL_LEGACY_INC_DIR)
DEVICE_BSP_DIR = ./Core/BSP/Miniware
S_SRCS := $(shell find $(MINIWARE_STARTUP_DIR) -type f -name '*.S')
LDSCRIPT=$(MINIWARE_LD_FILE)
DEV_GLOBAL_DEFS= -D STM32F103T8Ux -D STM32F1 -D STM32 -D USE_HAL_DRIVER -D STM32F103xB -D USE_RTOS_SYSTICK -D GCC_ARMCM3		\
		-D ARM_MATH_CM3 	\
		-D STM32F10X_MD -finline-limit=9999999
DEV_LDFLAGS= -Wl,--wrap=printf  -Wl,--no-wchar-size-warning 
DEV_AFLAGS=
DEV_CFLAGS=  -D VECT_TAB_OFFSET=$(bootldr_size)U
DEV_CXXFLAGS= 
CPUFLAGS= -mcpu=cortex-m3     \
		-mthumb 			\
		-mfloat-abi=soft
flash_size=62k
bootldr_size=0x4000
DEVICE_DFU_ADDRESS=0x08004000
DEVICE_DFU_VID_PID=0x1209:0xDB42
endif

ifeq ($(model),$(filter $(model),$(ALL_MHP30_MODELS)))
$(info Building for MHP30 )
DEVICE_INCLUDES = -I$(MHP30_INC_DIR) \
	-I$(MHP30_INC_CMSIS_DEVICE)\
	-I$(MHP30_CMSIS_CORE_INC_DIR)	\
	-I$(MHP30_HAL_INC_DIR)	\
	-I$(MHP30_HAL_LEGACY_INC_DIR)
DEVICE_BSP_DIR = ./Core/BSP/MHP30
S_SRCS := $(shell find $(MHP30_STARTUP_DIR) -type f -name '*.S')
LDSCRIPT=$(MHP30_LD_FILE)
DEV_GLOBAL_DEFS= -D STM32F103T8Ux -D STM32F1 -D STM32 -D USE_HAL_DRIVER -D STM32F103xB -D USE_RTOS_SYSTICK -D GCC_ARMCM3		\
		-D ARM_MATH_CM3 	\
		-D STM32F10X_MD 
DEV_LDFLAGS= 
DEV_AFLAGS=
DEV_CFLAGS=  -D VECT_TAB_OFFSET=$(bootldr_size)U
DEV_CXXFLAGS= 
CPUFLAGS= -mcpu=cortex-m3     \
		-mthumb 			\
		-mfloat-abi=soft
flash_size=126k
bootldr_size=32k
DEVICE_DFU_ADDRESS=0x08008000
DEVICE_DFU_VID_PID=0x1209:0xDB42
endif


ifeq ($(model),$(ALL_PINECIL_MODELS))
$(info Building for Pine64 Pinecilv1)
DEVICE_INCLUDES = -I$(PINE_INC_DIR) \
-I$(PINE_VENDOR_INC_DIR) \
-I$(PINE_VENDOR_USB_INC_DIR) \
-I$(PINE_NMSIS_INC_DIR) \
-I$(PINE_FREERTOS_PORT_INC_DIR) 
	
DEVICE_BSP_DIR = ./Core/BSP/Pinecil
S_SRCS := $(shell find $(PINE_INC_DIR) -type f -name '*.S') $(info $(S_SRCS) )
ASM_INC = -I$(PINE_RISCV_INC_DIR)
LDSCRIPT=./Core/BSP/Pinecil/Vendor/SoC/gd32vf103/Board/pinecil/Source/GCC/gcc_gd32vf103_flashxip.ld
flash_size=128k
bootldr_size=0x0
# Flags
CPUFLAGS= -march=rv32imac \
			-mabi=ilp32 \
			-mcmodel=medany -fsigned-char -fno-builtin -nostartfiles
DEV_LDFLAGS=-nostartfiles 
DEV_AFLAGS=  
DEV_GLOBAL_DEFS= -DRTOS_FREERTOS -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP
DEV_CFLAGS= -D VECT_TAB_OFFSET=$(bootldr_size)U
DEV_CXXFLAGS=
endif




ifeq ($(model),$(ALL_PINECIL_V2_MODELS))
$(info Building for Pine64 Pinecilv2 )



DEVICE_INCLUDES = -I$(PINECILV2_DIR) \
	-I$(PINECILV2_SDK_DIR) \
	-I$(PINECILV2_VENDOR_BSP_COMMON_DIR) \
	-I$(PINECILV2_VENDOR_BSP_PLATFORM_DIR) \
	-I$(PINECILV2_COMMON_DIR) \
	-I$(PINECILV2_COMMON_BL_MATH_DIR) \
	-I$(PINECILV2_COMMON_DEVICE_DIR) \
	-I$(PINECILV2_COMMON_LIST_DIR) \
	-I$(PINECILV2_COMMON_MISC_DIR) \
	-I$(PINECILV2_COMMON_PARTITION_DIR) \
	-I$(PINECILV2_COMMON_PID_DIR) \
	-I$(PINECILV2_COMMON_RING_BUFFERDIR) \
	-I$(PINECILV2_COMMON_SOFT_CRC_DIR) \
	-I$(PINECILV2_COMMON_TIMESTAMP_DIR) \
	-I$(PINECILV2_COMPONENTS_DIR) \
	-I$(PINECILV2_COMPONENTS_FREERTOS_DIR) \
	-I$(PINECILV2_COMPONENTS_FREERTOS_BL602_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_DRIVERS_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_DRIVERS_BLUETOOTH_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_PORT_INCLUDE_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_BLUETOOTH_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_COMMON_INCLUDE_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_COMMON_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_MISC_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_ZEPHYR_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_NET_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_TOOLCHAIN_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_INCLUDE_TOOLCHAIN_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_TINYCRYPT_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_TINYCRYPT_INCLUDE_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_HOST_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_BL_HCI_WRAPPER_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_CONTROLLER_BLE_INC_DIR) \
	-I$(PINECILV2_COMPONENTS_BLE_STACK_BC_DEC_DIR) \
	-I$(PINECILV2_COMPONENTS_NMSIS_DIR) \
	-I$(PINECILV2_COMPONENTS_USB_STACK_DIR) \
	-I$(PINECILV2_DRIVERS_DIR) \
	-I$(PINECILV2_DRIVERS_HAL_DRV_INC_DIR) \
	-I$(PINECILV2_DRIVERS_HAL_DRV_DEF_DIR) \
	-I$(PINECILV2_DRIVERS_REGS_DIR) \
	-I$(PINECILV2_DRIVERS_RISCV_DIR) \
	-I$(PINECILV2_DRIVERS_STARTUP_DIR) \
	-I$(PINECILV2_DRIVERS_STD_DRV_DIR) \
	-I$(PINECILV2_VENDOR_BSP_PLATFORM_DIR) \
	-I$(PINECILV2_VENDOR_BSP_USB_DIR) \
	-I$(PINECILV2_COMPONENTS_USB_STACK_COMMON_DIR) \
	-I$(PINECILV2_COMPONENTS_USB_STACK_CORE_DIR) \
	-I$(PINECILV2_COMPONENTS_USB_STACK_CDC_DIR) \
	-I$(PINECILV2_COMPONENTS_USB_STACK_WINUSB_DIR) \
	-I$(PINECILV2_COMPONENTS_NMSIS_CORE_INC_DIR) 

DEVICE_BSP_DIR = ./Core/BSP/Pinecilv2
S_SRCS := $(shell find $(PINECILV2_DIR) -type d  \(  -path $(PINECILV2_VENDOR_BSP_COMMON_DIR) \) -prune -false -o  -type f -name '*.S') $(info $(S_SRCS) )
ASM_INC = $(DEVICE_INCLUDES)
LDSCRIPT=./Core/BSP/Pinecilv2/bl_mcu_sdk/drivers/bl702_driver/bl702_flash.ld

# Flags
CPUFLAGS= -march=rv32imafc \
			-mabi=ilp32f \
			-mcmodel=medany -fsigned-char -fno-builtin -nostartfiles \
			-DportasmHANDLE_INTERRUPT=FreeRTOS_Interrupt_Handler -DARCH_RISCV -D__RISCV_FEATURE_MVE=0 -DBL702 -DBFLB_USE_ROM_DRIVER=1
DEV_LDFLAGS=-nostartfiles  -L $(PINECILV2_BLE_CRAPWARE_BLOB_DIR) -L $(PINECILV2_RF_CRAPWARE_BLOB_DIR) -l blecontroller_702_m0s1s -l bl702_rf
DEV_AFLAGS=  
DEV_GLOBAL_DEFS= -DCFG_FREERTOS \
-DARCH_RISCV \
-DBL702 \
-DCFG_BLE_ENABLE \
-DBFLB_BLE \
-DCFG_BLE \
-DOPTIMIZE_DATA_EVT_FLOW_FROM_CONTROLLER \
-DBL_MCU_SDK \
-DCFG_CON=2 \
-DCFG_BLE_TX_BUFF_DATA=2 \
-DCONFIG_BT_PERIPHERAL \
-DCONFIG_BT_L2CAP_DYNAMIC_CHANNEL \
-DCONFIG_BT_GATT_CLIENT \
-DCONFIG_BT_CONN \
-DCONFIG_BT_GATT_DIS_PNP \
-DCONFIG_BT_GATT_DIS_SERIAL_NUMBER \
-DCONFIG_BT_GATT_DIS_FW_REV \
-DCONFIG_BT_GATT_DIS_HW_REV \
-DCONFIG_BT_GATT_DIS_SW_REV \
-DCONFIG_BT_ECC \
-DCONFIG_BT_GATT_DYNAMIC_DB \
-DCONFIG_BT_GATT_SERVICE_CHANGED \
-DCONFIG_BT_KEYS_OVERWRITE_OLDEST \
-DCONFIG_BT_KEYS_SAVE_AGING_COUNTER_ON_PAIRING \
-DCONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS \
-DCONFIG_BT_BONDABLE \
-DCONFIG_BT_HCI_VS_EVT_USER \
-DCONFIG_BT_ASSERT \
-DCONFIG_BT_SIGNING \
-DCONFIG_BT_SETTINGS_CCC_LAZY_LOADING \
-DCONFIG_BT_SETTINGS_USE_PRINTK \
-DCFG_SLEEP \
-DCONFIG_BT_ALLROLES \
-DCONFIG_BT_CENTRAL \
-DCONFIG_BT_OBSERVER \
-DCONFIG_BT_BROADCASTER \
-DCFG_BLE_STACK_DBG_PRINT \
-DportasmHANDLE_INTERRUPT=FreeRTOS_Interrupt_Handler  \
-DCONFIG_BT_DEVICE_NAME=\"Pinecil\" \
-DCONFIG_BT_DEVICE_APPEARANCE=0x06C1
# -DBFLB_USE_HAL_DRIVER
# -DCONFIG_BT_SMP \


#Required to be turned off due to their drivers tripping warnings
DEV_CFLAGS= -Wno-error=enum-conversion -Wno-type-limits -Wno-implicit-fallthrough
DEV_CXXFLAGS= $(DEV_CFLAGS)
flash_size=128k
bootldr_size=0x0

endif

INCLUDES = -I$(APP_INC_DIR)	\
	-I$(BRIEFLZ_INC_DIR) \
	-I$(FRTOS_CMIS_INC_DIR)	\
	-I$(FRTOS_INC_DIR)	\
	-I$(DRIVER_INC_DIR) \
	-I$(BSP_INC_DIR) \
	-I$(THREADS_INC_DIR) \
	-I$(THREADS_OP_MODES_INC_DIR) \
	-I$(THREADS_OP_MODES_TOOLS_INC_DIR) \
	-I$(INC_PD_DRIVERS_DIR) \
	$(DEVICE_INCLUDES) 

EXCLUDED_DIRS := -path  $(PINECILV2_VENDOR_BSP_ES8388_DIR) \
-o -path  $(PINECILV2_VENDOR_BSP_IMAGE_SENSOR_DIR) \
-o -path  $(PINECILV2_VENDOR_BSP_LVGL_DIR) \
-o -path  $(PINECILV2_VENDOR_BSP_MCU_LCD_DIR) \
-o -path  $(PINECILV2_VENDOR_BSP_BOARD_DIR) \
-o -path $(PINECILV2_VENDOR_BSP_USB_DIR) \

SOURCE := $(shell find $(SOURCE_THREADS_DIR) -type f -name '*.c') \
$(shell find $(SOURCE_CORE_DIR) -type f -name '*.c') \
$(shell find $(SOURCE_DRIVERS_DIR) -type f -name '*.c') \
$(shell find $(DEVICE_BSP_DIR) -type d  \( $(EXCLUDED_DIRS)  \) -prune -false -o -type f -name '*.c')\
$(shell find $(SOURCE_MIDDLEWARES_DIR) -type f -name '*.c') \
$(SOURCE_BRIEFLZ_DIR)/depack.c
# We exclude the USB-PD stack tests $(PD_DRIVER_TESTS_DIR)
SOURCE_CPP := $(shell find $(SOURCE_THREADS_DIR) -type f -name '*.cpp') \
$(shell find $(SOURCE_CORE_DIR) -type f -name '*.cpp') \
$(shell find $(SOURCE_DRIVERS_DIR) -path $(PD_DRIVER_TESTS_DIR) -prune -false -o -type f -name '*.cpp') \
$(shell find $(DEVICE_BSP_DIR) -type d  \( $(EXCLUDED_DIRS) \) -prune -false -o -type f -name '*.cpp')  \
$(shell find $(SOURCE_MIDDLEWARES_DIR) -type f -name '*.cpp')

# code optimisation ------------------------------------------------------------
OPTIM=-Os -fno-jump-tables -foptimize-strlen -faggressive-loop-optimizations -fdevirtualize-at-ltrans -fmerge-all-constants -fshort-wchar -flto -finline-small-functions -finline-functions -findirect-inlining -fdiagnostics-color -ffunction-sections -fdata-sections -fshort-enums -fsingle-precision-constant  -fno-common -fno-math-errno -ffast-math -ffinite-math-only -fno-signed-zeros -fsingle-precision-constant

# global defines ---------------------------------------------------------------
GLOBAL_DEFINES += $(DEV_GLOBAL_DEFS) -D USE_RTOS_SYSTICK -D MODEL_$(model) -D VECT_TAB_OFFSET=$(bootldr_size)U -fshort-wchar

ifdef swd_enable
	GLOBAL_DEFINES += -DSWD_ENABLE
endif



# libs -------------------------------------------------------------------------
LIBS=

# ------------------------------------------------------------------------------
COMPILER=gcc
# arm-none-eabi is the general ARM compiler, 
# riscv-none-embed is the riscv compiler
# riscv-nuclei-elf is the nuclei tuned one for their cores
ifeq ($(model),$(filter $(model),$(ALL_MINIWARE_MODELS) $(ALL_MHP30_MODELS)))
COMPILER_PREFIX=arm-none-eabi
endif
ifeq ($(model),$(filter $(model),$(ALL_PINECIL_MODELS) $(ALL_PINECIL_V2_MODELS)))
COMPILER_PREFIX=riscv-none-elf
endif
# programs ---------------------------------------------------------------------
CC=$(COMPILER_PREFIX)-gcc
CPP=$(COMPILER_PREFIX)-g++
OBJCOPY=$(COMPILER_PREFIX)-objcopy
SIZE=$(COMPILER_PREFIX)-size
OBJDUMP=$(COMPILER_PREFIX)-objdump
# use gcc in assembler mode so we can use defines etc in assembly
AS=$(COMPILER_PREFIX)-gcc -x assembler-with-cpp

# linker flags -----------------------------------------------------------------
LINKER_FLAGS=   -Wl,--gc-sections 		                      \
                -Wl,--wrap=malloc                             \
			    -Wl,--wrap=free                               \
			    -Wl,--undefined=vTaskSwitchContext            \
			    -Wl,--undefined=pxCurrentTCB                  \
			    -Wl,--defsym=__FLASH_SIZE__=$(flash_size)     \
			    -Wl,--defsym=__BOOTLDR_SIZE__=$(bootldr_size) \
				-Wl,--print-memory-usage 					  \
				--specs=nosys.specs 						  \
			    --specs=nano.specs						      \
				$(DEV_LDFLAGS)

# compiler flags ---------------------------------------------------------------


CHECKOPTIONS=-Wtrigraphs			\
			-Wuninitialized		\
			-Wmissing-braces	\
			-Wfloat-equal		\
			-Wunreachable-code	\
			-Wswitch-default	\
			-Wreturn-type		\
			-Wundef				\
			-Wparentheses		\
			-Wnonnull			\
			-Winit-self			\
			-Wmissing-include-dirs	\
			-Wsequence-point	\
			-Wswitch			\
			-Wformat			\
			-Wsign-compare		\
			-Waddress			\
			-Waggregate-return	\
			-Wmissing-field-initializers	\
			-Wshadow			\
			-Wno-unused-parameter \
			-Wdouble-promotion 
			
			


CHECKOPTIONS_C=	$(CHECKOPTIONS) -Wbad-function-cast


CXXFLAGS=$(DEV_CXXFLAGS)         \
        $(CPUFLAGS)              \
		$(INCLUDES)              \
		$(GLOBAL_DEFINES) 	     \
		-D${COMPILER}  		     \
		-MMD 				     \
		$(CHECKOPTIONS)		     \
		-std=c++17			     \
		$(OPTIM) 			     \
		-fno-rtti 			     \
		-fno-exceptions 	     \
		-fno-non-call-exceptions \
		-fno-use-cxa-atexit		 \
		-fno-strict-aliasing 	 \
		-fno-threadsafe-statics  \
		-T$(LDSCRIPT)


CFLAGS=$(DEV_CFLAGS) 			\
        $(CPUFLAGS) 			\
		$(INCLUDES) 			\
		$(CHECKOPTIONS_C) 		\
		$(GLOBAL_DEFINES) 		\
		-D${COMPILER}  			\
		-MMD 					\
		-std=gnu11				\
		$(OPTIM) 				\
		-T$(LDSCRIPT)			\
		-c                      

		
		
AFLAGS= $(CPUFLAGS)    			\
		$(DEV_AFLAGS)  			\
		$(GLOBAL_DEFINES) 		\
		$(OPTIM) 				\
		$(ASM_INC)    			\
		$(INCLUDES)




OBJS = $(SOURCE:.c=.o)
OBJS_CPP = $(SOURCE_CPP:.cpp=.o)
OBJS_S = $(S_SRCS:.S=.o)



OUT_OBJS=$(addprefix $(OUTPUT_DIR)/,$(OBJS))
OUT_OBJS_CPP=$(addprefix $(OUTPUT_DIR)/,$(OBJS_CPP))
OUT_OBJS_S=$(addprefix $(OUTPUT_DIR)/,$(OBJS_S))


default : firmware-EN

firmware-%: $(HEXFILE_DIR)/$(model)_%.hex $(HEXFILE_DIR)/$(model)_%.bin $(HEXFILE_DIR)/$(model)_%.dfu
	@true


#
# The rule to create the target directory
#

# Create hexfile
%.hex : %.elf Makefile
	$(OBJCOPY) $< -O ihex $@

%.bin : %.elf Makefile
	$(OBJCOPY) $< -O binary $@
	$(SIZE) $<

%.dfu : %.bin Makefile
	python3 dfuse-pack.py -b $(DEVICE_DFU_ADDRESS)@0:$< -D $(DEVICE_DFU_VID_PID) $@

$(HEXFILE_DIR)/$(model)_%.elf : \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation.%.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation.$*.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		$(LIBS) $(LINKER_FLAGS) -o$@ -Wl,-Map=$@.map

$(HEXFILE_DIR)/$(model)_string_compressed_%.elf : \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz.%.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz.$*.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		$(LIBS) $(LINKER_FLAGS) -o$@ -Wl,-Map=$@.map

$(HEXFILE_DIR)/$(model)_font_compressed_%.elf : \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_font.%.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_font.$*.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		$(LIBS) $(LINKER_FLAGS) -o$@ -Wl,-Map=$@.map

$(OUT_OBJS): $(OUTPUT_DIR)/%.o : %.c Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@echo Compiling ${<}
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/%.o : %.cpp Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@echo Compiling ${<}
	@$(CPP) -c $(CXXFLAGS) $< -o $@

$(OUT_OBJS_S): $(OUTPUT_DIR)/%.o: %.S Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@echo 'Building file: $<'
	@$(AS) -c $(AFLAGS) $< -o $@

Core/Gen/Translation.%.cpp $(OUTPUT_DIR)/Core/Gen/translation.files/%.pickle: ../Translations/translation_%.json \
		../Translations/make_translation.py \
		../Translations/translations_definitions.json \
		../Translations/font_tables.py \
		Makefile ../Translations/wqy-bitmapsong/wenquanyi_9pt.bdf
	@test -d Core/Gen || mkdir -p Core/Gen
	@test -d $(OUTPUT_DIR)/Core/Gen/translation.files || mkdir -p $(OUTPUT_DIR)/Core/Gen/translation.files
	@echo 'Generating translations for language $*'
	@python3 ../Translations/make_translation.py \
		-o $(PWD)/Core/Gen/Translation.$*.cpp \
		--output-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/$*.pickle \
		$*

#
# The recipes to produce compressed translation data:
#

$(OUTPUT_DIR)/Core/Gen/translation.files/%.o: Core/Gen/Translation.%.cpp
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating $@
	@$(CPP) -c $(filter-out -flto ,$(CXXFLAGS)) $< -o $@

$(OUTPUT_DIR)/Core/Gen/translation.files/multi.%.o: Core/Gen/Translation_multi.%.cpp
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating $@
	@$(CPP) -c $(filter-out -flto ,$(CXXFLAGS)) $< -o $@

$(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so: Core/brieflz/brieflz.c Core/brieflz/depack.c
	@test -d $(@D) || mkdir -p $(@D)
	@echo Building host brieflz shared library $@
	@$(HOST_CC) -fPIC -shared -DBLZ_DLL -DBLZ_DLL_EXPORTS -O $^ -o $@

Core/Gen/Translation_brieflz.%.cpp: $(OUTPUT_DIR)/Core/Gen/translation.files/%.o $(OUTPUT_DIR)/Core/Gen/translation.files/%.pickle $(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating BriefLZ compressed translation for $*
	@OBJCOPY=$(OBJCOPY) python3 ../Translations/make_translation.py \
		-o $(PWD)/Core/Gen/Translation_brieflz.$*.cpp \
		--input-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/$*.pickle \
		--strings-obj $(OUTPUT_DIR)/Core/Gen/translation.files/$*.o \
		$*

Core/Gen/Translation_brieflz_font.%.cpp: $(OUTPUT_DIR)/Core/Gen/translation.files/%.pickle $(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating BriefLZ compressed translation for $*
	@python3 ../Translations/make_translation.py \
		-o $(PWD)/Core/Gen/Translation_brieflz_font.$*.cpp \
		--input-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/$*.pickle \
		--compress-font \
		$*

#
# The recipes to produce multi-language firmwares:
#

# Usage: $(eval $(call multi_lang_rule,$(1)=group_code,$(2)=group_name,$(3)=lang_codes))
define multi_lang_rule

$(HEXFILE_DIR)/$(model)_multi_$(2).elf : \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_multi.$(1).o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o \
		Makefile $(LDSCRIPT)
	@test -d $$(@D) || mkdir -p $$(@D)
	@echo Linking $$@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_multi.$(1).o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o \
		$(LIBS) $(LINKER_FLAGS) -o$$@ -Wl,-Map=$$@.map

$(HEXFILE_DIR)/$(model)_multi_compressed_$(2).elf : \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_multi.$(1).o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o \
		Makefile $(LDSCRIPT)
	@test -d $$(@D) || mkdir -p $$(@D)
	@echo Linking $$@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_multi.$(1).o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o \
		$(LIBS) $(LINKER_FLAGS) -o$$@ -Wl,-Map=$$@.map

Core/Gen/Translation_multi.$(1).cpp: $(patsubst %,../Translations/translation_%.json,$(3)) \
		../Translations/make_translation.py \
		../Translations/translations_definitions.json \
		../Translations/font_tables.py \
		Makefile ../Translations/wqy-bitmapsong/wenquanyi_9pt.bdf
	@test -d Core/Gen || mkdir -p Core/Gen
	@test -d $(OUTPUT_DIR)/Core/Gen/translation.files || mkdir -p $(OUTPUT_DIR)/Core/Gen/translation.files
	@echo 'Generating translations for multi-language $(2)'
	@python3 ../Translations/make_translation.py \
		-o $(PWD)/Core/Gen/Translation_multi.$(1).cpp \
		--output-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle \
		$(3)

$(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle: Core/Gen/Translation_multi.$(1).cpp

Core/Gen/Translation_brieflz_multi.$(1).cpp: $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).o $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle $(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so
	@test -d $$(@D) || mkdir -p $$(@D)
	@echo Generating BriefLZ compressed translation for multi-language $(2)
	@OBJCOPY=$(OBJCOPY) python3 ../Translations/make_translation.py \
		-o $(PWD)/Core/Gen/Translation_brieflz_multi.$(1).cpp \
		--input-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle \
		--strings-obj $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).o \
		--compress-font \
		$(3)

endef # multi_lang_rule


# Add multi-language firmware rules:
$(foreach group_code,$(LANGUAGE_GROUPS),$(eval $(call multi_lang_rule,$(group_code),$(LANGUAGE_GROUP_$(group_code)_NAME),$(LANGUAGE_GROUP_$(group_code)_LANGS))))


clean :
	rm -Rf Core/Gen
	rm -Rf $(OUTPUT_DIR_BASE)
	rm -Rf $(HEXFILE_DIR)/*

style:
	@for src in $(ALL_SOURCE) $(ALL_INCLUDES); do \
		echo "Formatting $$src..." ; \
		clang-format -i "$$src" ; \
	done
	@echo "Done"

check-style:
	@for src in $(ALL_SOURCE) $(ALL_INCLUDES) ; do \
		var=`clang-format "$$src" | diff "$$src" - | wc -l` ; \
		if [ $$var -ne 0 ] ; then \
			echo "$$src does not respect the coding style (diff: $$var lines)" ; \
			clang-format "$$src" | diff "$$src" -; \
			exit 1 ; \
		fi ; \
	done
	@echo "Style check passed"
  
.PHONY: style all clean default
.SECONDARY:

# pull in dependency info for *existing* .o files
-include $(OUT_OBJS:.o=.d)
-include $(OUT_OBJS_CPP:.o=.d)
-include $(OUTPUT_DIR)/Core/Gen/Translation.*.d
-include $(OUTPUT_DIR)/Core/Gen/Translation_*.d
-include $(OUTPUT_DIR)/Core/Gen/translation.files/*.d
