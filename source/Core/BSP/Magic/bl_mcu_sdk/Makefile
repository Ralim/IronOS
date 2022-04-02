#common config
BOARD?=bl706_iot
CHIP?=bl702
APP_DIR?=examples
APP?=helloworld
CPU_ID?=none

#bootrom config,users do not need it
BOOTROM?=n

#format config
FORMAT_DIR?=.

# The command to remove a file.
RM = cmake -E remove_directory

#flash tool config
INTERFACE?=uart
COMx?=
BAUDRATE ?=2000000
ifeq ($(INTERFACE),uart)
FLASH_DOWNLOAD_CONFIG:=--chipname=$(CHIP) --interface=uart --port=$(COMx) --baudrate=$(BAUDRATE)
else
FLASH_DOWNLOAD_CONFIG:=--chipname=$(CHIP) --interface=$(INTERFACE)
endif

#option config to use
SUPPORT_FLOAT?=n
SUPPORT_ROMAPI?=y
SUPPORT_HALAPI?=y
SUPPORT_USB_HS?=n
SUPPORT_HW_SEC_ENG_DISABLE?=n
SUPPORT_BLECONTROLLER_LIB?=

#cmake definition config
cmake_definition+= -DCHIP=$(CHIP)
cmake_definition+= -DCPU_ID=$(CPU_ID)
cmake_definition+= -DBOARD=$(BOARD)
cmake_definition+= -DAPP_DIR=$(APP_DIR)
cmake_definition+= -DAPP=$(APP)
cmake_definition+= -DBOOTROM=$(BOOTROM)
cmake_definition+= -DCONFIG_ROMAPI=$(SUPPORT_ROMAPI)
cmake_definition+= -DCONFIG_HALAPI=$(SUPPORT_HALAPI)
cmake_definition+= -DCONFIG_PRINT_FLOAT=$(SUPPORT_FLOAT)
cmake_definition+= -DCONFIG_USB_HS=$(SUPPORT_USB_HS)
cmake_definition+= -DCONFIG_HW_SEC_ENG_DISABLE=$(SUPPORT_HW_SEC_ENG_DISABLE)
cmake_definition+= -DCONFIG_BLECONTROLLER_LIB=$(SUPPORT_BLECONTROLLER_LIB)

build:Makefile
	cmake -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make -j4

help:
	@echo "Welcome to MCU SDK cmake build system,commands are as follows:"
	@echo ""
	@echo "make clean - Remove all cmake caches and output files"
	@echo "make SUPPORT_FLOAT=y - Enable float print"
	@echo "make SUPPORT_USB_HS=y - Enable usb high speed"
	@echo "make SUPPORT_BLECONTROLLER_LIB=value - Select blecontroller lib,value can be m0s1、m0s1s、std or empty"

download:
	./tools/bflb_flash_tool/bflb_mcu_tool $(FLASH_DOWNLOAD_CONFIG)

format:
	find $(FORMAT_DIR)/ -name "*.c" -o -name "*.h" -o -name "*.cc" -o -name "*.cpp"| xargs clang-format -style=file -i

clean:
	$(RM) out
	$(RM) build

.PHONY:build clean download format help

