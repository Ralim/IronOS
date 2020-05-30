
ifndef model
model:=TS100
endif
OUTPUT_EXE=$(model)_$(lang)
ifndef lang
lang:= EN
endif

# Discover the source files to build
SOURCE := $(shell find . -type f -name '*.c')
SOURCE_CPP := $(shell find . -type f -name '*.cpp')
SOURCES := $(shell find . -type f -name '*.c*')
S_SRCS := $(shell find . -type f -name '*.s') 

APP_INC_DIR = ./Core/Inc
CMSIS_DEVICE_INC_DIR = ./Core/BSP/Miniware/Vendor/CMSIS/Device/ST/STM32F1xx/Include
CMSIS_CORE_INC_DIR = ./Core/BSP/Miniware/Vendor/CMSIS/Include
HAL_INC_DIR = ./Core/BSP/Miniware/Vendor/STM32F1xx_HAL_Driver/Inc
HAL_LEGACY_INC_DIR = ./Core/BSP/Miniware/Vendor/STM32F1xx_HAL_Driver/Inc/Legacy
FRTOS_CMIS_INC_DIR = ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
FRTOS_INC_DIR = ./Middlewares/Third_Party/FreeRTOS/Source/include
FRTOS_GCC_INC_DIR = ./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3
DRIVER_INC_DIR =./Core/Drivers
BSP_INC_DIR = ./Core/BSP
MINIWARE_INC_DIR = ./Core/BSP/Miniware
THREADS_INC_DIR = ./Core/Threads

INCLUDES = -I$(APP_INC_DIR)	\
	-I$(CMSIS_DEVICE_INC_DIR)\
	-I$(CMSIS_CORE_INC_DIR)	\
	-I$(HAL_INC_DIR)	\
	-I$(HAL_LEGACY_INC_DIR)	\
	-I$(FRTOS_CMIS_INC_DIR)	\
	-I$(FRTOS_INC_DIR)	\
	-I$(FRTOS_GCC_INC_DIR) \
	-I$(DRIVER_INC_DIR) \
	-I$(BSP_INC_DIR) \
	-I$(MINIWARE_INC_DIR) \
	-I$(THREADS_INC_DIR)
# output folder
HEXFILE_DIR=Hexfile

# temporary objects folder
OUTPUT_DIR=Objects

# code optimisation ------------------------------------------------------------
OPTIM=-Os -flto -ffat-lto-objects -finline-small-functions -findirect-inlining -fdiagnostics-color -ffunction-sections -fdata-sections


# global defines ---------------------------------------------------------------
GLOBAL_DEFINES += -D STM32F103T8Ux -D STM32F1 -D STM32 -D USE_HAL_DRIVER -D STM32F103xB -D USE_RTOS_SYSTICK -D LANG_$(lang) -D LANG -D MODEL_$(model)

# Enable debug code generation
DEBUG=-g3
# Without debug code
#DEBUG=

	
# libs -------------------------------------------------------------------------
LIBS=

# linker script ----------------------------------------------------------------
LDSCRIPT=LinkerScript.ld

# ------------------------------------------------------------------------------
COMPILER=gcc
# arm-none is the general ARM compiler, 
COMPILER_PREFIX=arm-none
# programs ---------------------------------------------------------------------
CC=$(COMPILER_PREFIX)-eabi-gcc
CPP=$(COMPILER_PREFIX)-eabi-g++
AS=$(COMPILER_PREFIX)-eabi-as
GCOV=$(COMPILER_PREFIX)-eabi-gcov
OBJCOPY=$(COMPILER_PREFIX)-eabi-objcopy
OBJDUMP=$(COMPILER_PREFIX)-eabi-objdump
SIZE=$(COMPILER_PREFIX)-eabi-size
SREC=srec_cat
SREC_INFO=srec_info




# linker flags -----------------------------------------------------------------
LINKER_FLAGS=-Wl,--gc-sections 		\
            -Wl,--wrap=malloc \
			-Wl,--wrap=free \
			-o$(OUT_HEXFILE).elf 	\
			-Wl,-Map=$(OUT_HEXFILE).map \
			-mcpu=cortex-m3		\
			-mthumb			\
			-mfloat-abi=soft	\
			-lm -Os -flto -Wl,--undefined=vTaskSwitchContext \
			--specs=nano.specs

# compiler flags ---------------------------------------------------------------
CPUFLAGS=-D GCC_ARMCM3		\
		-D ARM_MATH_CM3 	\
		-D STM32F10X_MD		\
		-mthumb 			\
		-mcpu=cortex-m3 	\
		-mfloat-abi=soft



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


CXXFLAGS=$(CPUFLAGS) \
		$(DEBUG) 			\
		$(INCLUDES) \
		$(GLOBAL_DEFINES) 	\
		-D${COMPILER}  		\
		-MMD 				\
		$(CHECKOPTIONS)		\
		-std=c++11			\
		$(OPTIM) 			\
		-fno-common			\
		-ffreestanding		\
		-fno-rtti 			\
		-fno-exceptions 	\
		-fno-non-call-exceptions \
		-fno-use-cxa-atexit		\
		-fno-strict-aliasing 		\
		-fno-rtti 			\
		-fno-exceptions		\
		-fno-threadsafe-statics \
		-T$(LDSCRIPT)


CFLAGS=$(CPUFLAGS) \
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
		
		
AFLAGS=$(CPUFLAGS) \
		$(DEBUG) 			\
		$(INCLUDES)


ifeq (${COMPILER}, gcc)
AFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -ffunction-sections -fdata-sections
endif



OBJS = $(SOURCE:.c=.o)
OBJS_CPP = $(SOURCE_CPP:.cpp=.o)
OBJS_S = $(S_SRCS:.s=.o)



OUT_OBJS=$(addprefix $(OUTPUT_DIR)/,$(OBJS))
OUT_OBJS_CPP=$(addprefix $(OUTPUT_DIR)/,$(OBJS_CPP))
OUT_OBJS_S=$(addprefix $(OUTPUT_DIR)/,$(OBJS_S))
OUT_HEXFILE=$(addprefix $(HEXFILE_DIR)/,$(OUTPUT_EXE))

all: $(OUT_HEXFILE).hex $(OUT_HEXFILE).bin

#
# The rule to create the target directory
#

# Create hexfile
%.hex : %.elf
	$(SIZE) $^
	$(OBJCOPY) $^ -O ihex $@

%.bin : %.elf
	$(SIZE) $^
	$(OBJCOPY) $^ -O binary $@

$(OUT_HEXFILE).elf : $(OUT_OBJS) $(OUT_OBJS_CPP) $(OUT_OBJS_S) Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $(OUTPUT_EXE).elf
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) $(LIBS) $(LINKER_FLAGS)

$(OUT_OBJS): $(OUTPUT_DIR)/%.o : %.c Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@echo Compiling ${<}
#	@echo $(CFLAGS)
	@$(CC) -c $(CFLAGS) $< -o $@
	@$(OBJDUMP) -d -S $@ > $@.lst

$(OUT_OBJS_CPP): $(OUTPUT_DIR)/%.o : %.cpp Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@echo Compiling ${<}
	@$(CPP) -c $(CXXFLAGS) $< -o $@
	@$(OBJDUMP) -d -S $@ > $@.lst

$(OUT_OBJS_S): $(OUTPUT_DIR)/%.o: %.s Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@$(AS) -mcpu=cortex-m3 -mthumb -mfloat-abi=soft $(INCLUDES) -g $< -o $@
	@echo 'Finished building: $<'
	@echo ' '


clean :
	rm -Rf $(OUTPUT_DIR)
	rm -Rf $(HEXFILE_DIR)

# pull in dependency info for *existing* .o files
-include $(OUT_OBJS:.o=.d)
-include $(OUT_OBJS_CPP:.o=.d)

