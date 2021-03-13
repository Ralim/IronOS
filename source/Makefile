
ifndef model
model:=TS100
endif

ALL_MINIWARE_MODELS=TS100 TS80 TS80P
ALL_PINE_MODELS=Pinecil
ALL_MODELS=$(ALL_MINIWARE_MODELS) $(ALL_PINE_MODELS)
ifneq ($(model),$(filter $(model),$(ALL_MODELS)))
$(error Invalid model '$(model)', valid options are: $(ALL_MODELS))
endif

ALL_LANGUAGES=BG CS DA DE EN ES FI FR HR HU IT LT NL NL_BE NO PL PT RU SK SL SR_CYRL SR_LATN SV TR UK YUE_HK ZH_TW


# Enumerate all of the include directories
APP_INC_DIR = ./Core/Inc
MINIWARE_INC_CMSIS_DEVICE = ./Core/BSP/Miniware/Vendor/CMSIS/Device/ST/STM32F1xx/Include
MINIWARE_CMSIS_CORE_INC_DIR = ./Core/BSP/Miniware/Vendor/CMSIS/Include
MINIWARE_HAL_INC_DIR = ./Core/BSP/Miniware/Vendor/STM32F1xx_HAL_Driver/Inc
MINIWARE_HAL_LEGACY_INC_DIR = ./Core/BSP/Miniware/Vendor/STM32F1xx_HAL_Driver/Inc/Legacy
FRTOS_CMIS_INC_DIR = ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
FRTOS_INC_DIR = ./Middlewares/Third_Party/FreeRTOS/Source/include
MINIWARE_STARTUP_DIR = ./Startup
DRIVER_INC_DIR =./Core/Drivers
BSP_INC_DIR = ./Core/BSP
THREADS_INC_DIR = ./Core/Threads
MINIWARE_INC_DIR = ./Core/BSP/Miniware
PINE_INC_DIR = ./Core/BSP/Pine64
PINE_VENDOR_INC_DIR = ./Core/BSP/Pine64/Vendor/SoC/gd32vf103/Common/Include
PINE_BOARD_INC_DIR = ./Core/BSP/Pine64/Vendor/SoC/gd32vf103/Board/pinecil/Include
PINE_VENDOR_USB_INC_DIR = ./Core/BSP/Pine64/Vendor/SoC/gd32vf103/Common/Include/Usb
PINE_NMSIS_INC_DIR = ./Core/BSP/Pine64/Vendor/NMSIS/Core/Include
PINE_FREERTOS_PORT_INC_DIR = ./Core/BSP/Pine64/Vendor/OS/FreeRTOS/Source/portable/GCC
SOURCE_THREADS_DIR = ./Core/Threads
SOURCE_CORE_DIR = ./Core/Src
SOURCE_DRIVERS_DIR = ./Core/Drivers
INC_PD_DRIVERS_DIR = ./Core/Drivers/FUSB302
SOURCE_MIDDLEWARES_DIR = ./Middlewares
# Find-all's used for formatting
ALL_INCLUDES = $(shell find ./Core -type f -name '*.h') \
			   $(shell find ./Core -type f -name '*.hpp') 

ALL_SOURCE = $(shell find ./Core -type f -name '*.c') \
			   $(shell find ./Core -type f -name '*.cpp') 
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
LDSCRIPT=stm32f103.ld
DEV_GLOBAL_DEFS= -D STM32F103T8Ux -D STM32F1 -D STM32 -D USE_HAL_DRIVER -D STM32F103xB -D USE_RTOS_SYSTICK -D GCC_ARMCM3		\
		-D ARM_MATH_CM3 	\
		-D STM32F10X_MD 
DEV_LDFLAGS=-lm -Wl,--gc-sections 
DEV_AFLAGS=
DEV_CFLAGS= -D GCC_ARMCM3		\
		-D ARM_MATH_CM3 	\
		-D STM32F10X_MD		
DEV_CXXFLAGS= -D GCC_ARMCM3		\
		-D ARM_MATH_CM3 	\
		-D STM32F10X_MD		
CPUFLAGS= -mcpu=cortex-m3     \
		  -mthumb 			\
		  -mfloat-abi=soft
flash_size=64k
bootldr_size=0x4000
endif

ifeq ($(model),$(ALL_PINE_MODELS))
$(info Building for Pine64 )
DEVICE_INCLUDES = -I$(PINE_INC_DIR) \
-I$(PINE_VENDOR_INC_DIR) \
-I$(PINE_BOARD_INC_DIR) \
-I$(PINE_VENDOR_USB_INC_DIR) \
-I$(PINE_NMSIS_INC_DIR) \
-I$(PINE_FREERTOS_PORT_INC_DIR) 
	
DEVICE_BSP_DIR = ./Core/BSP/Pine64
S_SRCS := $(shell find $(PINE_INC_DIR) -type f -name '*.S') $(info $(S_SRCS) )
ASM_INC = -I$(PINE_RISCV_INC_DIR)
LDSCRIPT=./Core/BSP/Pine64/Vendor/SoC/gd32vf103/Board/pinecil/Source/GCC/gcc_gd32vf103_flashxip.ld
flash_size=128k
bootldr_size=0x0
# Flags
CPUFLAGS= -march=rv32imac \
			-mabi=ilp32 \
			-mcmodel=medany -fsigned-char -fno-builtin -fsingle-precision-constant \
			-DRTOS_FREERTOS -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP

DEV_LDFLAGS= -lstdc++ -nostartfiles -Xlinker --gc-sections --specs=nosys.specs -u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek
DEV_AFLAGS= -nostartfiles -ffreestanding -fno-common -Os -flto 
DEV_GLOBAL_DEFS=
DEV_CFLAGS= -MMD -MP -MF "$(@:%.o=%.d)" -MT "$@"
DEV_CXXFLAGS= -MMD -MP -MF "$(@:%.o=%.d)" -MT "$@"
endif

INCLUDES = -I$(APP_INC_DIR)	\
	-I$(FRTOS_CMIS_INC_DIR)	\
	-I$(FRTOS_INC_DIR)	\
	-I$(DRIVER_INC_DIR) \
	-I$(BSP_INC_DIR) \
	-I$(THREADS_INC_DIR) \
	-I$(INC_PD_DRIVERS_DIR) \
	  $(DEVICE_INCLUDES) 
	  
TRANSLATION_FILES=$(wildcard ../../Translations/translation_*.json)
SOURCE := $(shell find $(SOURCE_THREADS_DIR) -type f -name '*.c') \
$(shell find $(SOURCE_CORE_DIR) -type f -name '*.c') \
$(shell find $(SOURCE_DRIVERS_DIR) -type f -name '*.c') \
$(shell find $(DEVICE_BSP_DIR) -type f -name '*.c') \
$(shell find $(SOURCE_MIDDLEWARES_DIR) -type f -name '*.c') 
SOURCE_CPP := $(shell find $(SOURCE_THREADS_DIR) -type f -name '*.cpp') \
$(shell find $(SOURCE_CORE_DIR) -type f -name '*.cpp') \
$(shell find $(SOURCE_DRIVERS_DIR) -type f -name '*.cpp') \
$(shell find $(DEVICE_BSP_DIR) -type f -name '*.cpp') \
$(shell find $(SOURCE_MIDDLEWARES_DIR) -type f -name '*.cpp')
# output folder
HEXFILE_DIR=Hexfile

# temporary objects folder
OUTPUT_DIR_BASE=Objects
OUTPUT_DIR=Objects/$(model)
# code optimisation ------------------------------------------------------------
OPTIM=-Os -flto -finline-small-functions -findirect-inlining -fdiagnostics-color -ffunction-sections -fdata-sections

# global defines ---------------------------------------------------------------
GLOBAL_DEFINES += $(DEV_GLOBAL_DEFS) -D USE_RTOS_SYSTICK -D MODEL_$(model) -D VECT_TAB_OFFSET=$(bootldr_size)U

# Without debug code
DEBUG=
ifdef swd_enable
	GLOBAL_DEFINES += -DSWD_ENABLE
	# Enable debug code generation
	DEBUG=-g
endif



# libs -------------------------------------------------------------------------
LIBS=

# ------------------------------------------------------------------------------
COMPILER=gcc
# arm-none-eabi is the general ARM compiler, 
# riscv-none-embed is the riscv compiler
# riscv-nuclei-elf is the nuclei tuned one for their cores
ifeq ($(model),$(filter $(model),$(ALL_MINIWARE_MODELS)))
COMPILER_PREFIX=arm-none-eabi
endif
ifeq ($(model),$(ALL_PINE_MODELS))
COMPILER_PREFIX=riscv-nuclei-elf
endif
# programs ---------------------------------------------------------------------
CC=$(COMPILER_PREFIX)-gcc
CPP=$(COMPILER_PREFIX)-g++
AS=$(COMPILER_PREFIX)-as
OBJCOPY=$(COMPILER_PREFIX)-objcopy
OBJDUMP=$(COMPILER_PREFIX)-objdump
SIZE=$(COMPILER_PREFIX)-size

# For the Pinecil, use gcc in assembler mode
ifeq ($(model),$(ALL_PINE_MODELS))
AS=$(COMPILER_PREFIX)-gcc -x assembler-with-cpp
endif

# linker flags -----------------------------------------------------------------
LINKER_FLAGS=   -Wl,--gc-sections 		                      \
                -Wl,--wrap=malloc                             \
			    -Wl,--wrap=free                               \
			    -lm                                           \
			    -Wl,--undefined=vTaskSwitchContext            \
			    -Wl,--undefined=pxCurrentTCB                  \
			    -Wl,--defsym=__FLASH_SIZE__=$(flash_size)     \
			    -Wl,--defsym=__BOOTLDR_SIZE__=$(bootldr_size) \
				$(DEV_LDFLAGS)                                \
				-flto 										  \
			    --specs=nano.specs

# compiler flags ---------------------------------------------------------------


CHECKOPTIONS=		-Wall 				\
			-Wextra				\
			-Wunused			\
			-Wcomment 			\
			-Wtrigraphs			\
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
			-Winline			\
			-Wshadow			\
			-Wno-unused-parameter \
			-Wdouble-promotion \
			-Werror 
			


CHECKOPTIONS_C=		-Wbad-function-cast


CXXFLAGS=$(DEV_CXXFLAGS)         \
        $(CPUFLAGS)              \
		$(DEBUG) 			     \
		$(INCLUDES)              \
		$(GLOBAL_DEFINES) 	     \
		-D${COMPILER}  		     \
		-MMD 				     \
		$(CHECKOPTIONS)		     \
		-std=c++11			     \
		$(OPTIM) 			     \
		-fno-common			     \
		-ffreestanding		     \
		-fno-rtti 			     \
		-fno-exceptions 	     \
		-fno-non-call-exceptions \
		-fno-use-cxa-atexit		 \
		-fno-strict-aliasing 	 \
		-fno-rtti 			     \
		-fno-exceptions		     \
		-fno-threadsafe-statics  \
		-T$(LDSCRIPT)


CFLAGS=$(DEV_CFLAGS) \
        $(CPUFLAGS) \
		$(DEBUG) 			\
		$(INCLUDES) 			\
		$(CHECKOPTIONS_C) 		\
		$(GLOBAL_DEFINES) 		\
		-D${COMPILER}  			\
		-MMD 				\
		-std=gnu99			\
		$(OPTIM) 			\
		-fno-common			\
		-ffreestanding			\
		-T$(LDSCRIPT)			\
		-c                      

		
		
AFLAGS= $(CPUFLAGS)   \
		$(DEV_AFLAGS) \
		$(DEBUG)      \
		$(ASM_INC)    \
		$(INCLUDES)




OBJS = $(SOURCE:.c=.o)
OBJS_CPP = $(SOURCE_CPP:.cpp=.o)
OBJS_S = $(S_SRCS:.S=.o)



OUT_OBJS=$(addprefix $(OUTPUT_DIR)/,$(OBJS))
OUT_OBJS_CPP=$(addprefix $(OUTPUT_DIR)/,$(OBJS_CPP))
OUT_OBJS_S=$(addprefix $(OUTPUT_DIR)/,$(OBJS_S))


default : firmware-EN

firmware-%: $(HEXFILE_DIR)/$(model)_%.hex $(HEXFILE_DIR)/$(model)_%.bin
	@true

ALL_FIRMWARE_TARGETS=$(addprefix firmware-,$(ALL_LANGUAGES))
all: $(ALL_FIRMWARE_TARGETS)

#
# The rule to create the target directory
#

# Create hexfile
%.hex : %.elf Makefile
	$(OBJCOPY) $< -O ihex $@

%.bin : %.elf Makefile
	$(SIZE)  --format=berkeley  $<
	$(OBJCOPY) $< -O binary $@

$(HEXFILE_DIR)/$(model)_%.elf : $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) $(OUTPUT_DIR)/Core/Gen/Translation.%.o Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) $(OUTPUT_DIR)/Core/Gen/Translation.$*.o $(LIBS) $(LINKER_FLAGS) -o$@ -Wl,-Map=$@.map

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

Core/Gen/Translation.%.cpp: ../Translations/translation_%.json Makefile ../Translations/make_translation.py ../Translations/translations_commons.js ../Translations/fontTables.py ../Translations/wqy-bitmapsong/wenquanyi_9pt.bdf
	@test -d $(@D) || mkdir -p $(@D)
	@echo 'Generating translations for language $*'
	@python3 ../Translations/make_translation.py -o $(PWD)/$@ $*

clean :
	rm -Rf $(SOURCE_CORE_DIR)/Translation.cpp Core/Gen
	rm -Rf $(OUTPUT_DIR_BASE)
	rm -Rf $(HEXFILE_DIR)

style:
	@for src in $(ALL_SOURCE) $(ALL_INCLUDES); do \
		echo "Formatting $$src..." ; \
		clang-format -i "$$src" ; \
	done
	@echo "Done"

.PHONY: style all clean default
.SECONDARY:

# pull in dependency info for *existing* .o files
-include $(OUT_OBJS:.o=.d)
-include $(OUT_OBJS_CPP:.o=.d)

