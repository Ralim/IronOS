.DEFAULT_GOAL := help
.DELETE_ON_ERROR:

ROOT := $(abspath ../../../..)
OUT ?= $(ROOT)/source/Objects/TS21-bringup
CROSS_COMPILE ?= riscv-none-embed-
HOST_CC ?= cc
HOST_CXX ?= c++
PYTHON ?= python3

ARCH_FLAGS := -march=rv32imac -mabi=ilp32
WARNINGS := -Wall -Wextra -Werror
CFLAGS := $(ARCH_FLAGS) $(WARNINGS) -std=c11 -Os -g3 -ffreestanding \
          -fno-builtin -fno-common -ffunction-sections -fdata-sections -fno-jump-tables \
          -I. -I$(OUT)
OBJECTS := $(OUT)/startup.o $(OUT)/main.o $(OUT)/hardware.o $(OUT)/protocol.o
ELF := $(OUT)/TS21_BRINGUP_NOHEAT.elf
HEX := $(OUT)/TS21_BRINGUP_NOHEAT.hex

.PHONY: help probe probe-hex test
help:
	@echo "Development-only TS21 no-heater diagnostic, NOT an IronOS soldering port."
	@echo "make probe EXPERIMENTAL=1 CROSS_COMPILE=<toolchain-prefix>"
	@echo "make probe-hex EXPERIMENTAL=1 CROSS_COMPILE=<toolchain-prefix>"
	@echo "make test"
	@echo "HEX export is for cartridge-removed bench testing; no automatic flashing."

ifeq ($(EXPERIMENTAL),1)
probe: $(ELF)
	$(PYTHON) tools/inspect_image.py $(ELF)
	@echo "UNVALIDATED HARDWARE: ELF is for bring-up, not normal soldering."

probe-hex: $(HEX)
	$(PYTHON) tools/inspect_image.py $(ELF) --hex $(HEX)
	@echo "UNVALIDATED HARDWARE: remove the cartridge before any diagnostic flash."
else
probe probe-hex:
	@echo "Refusing experimental build without EXPERIMENTAL=1. Read README.md."
	@exit 1
endif

$(OUT):
	mkdir -p $@

$(OUT)/font.h: tools/generate_font.py $(ROOT)/Translations/font_tables.py | $(OUT)
	$(PYTHON) $< $@

$(OBJECTS): Makefile

$(OUT)/main.o: main.c ts21.h $(OUT)/font.h
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $< -o $@

$(OUT)/%.o: %.c ts21.h | $(OUT)
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $< -o $@

$(OUT)/startup.o: startup.S | $(OUT)
	$(CROSS_COMPILE)gcc $(ARCH_FLAGS) -c $< -o $@

$(ELF): $(OBJECTS) ts21.ld Makefile
	$(CROSS_COMPILE)gcc $(ARCH_FLAGS) -nostartfiles -nostdlib \
		-Wl,--gc-sections,--build-id=none,-Map,$(OUT)/TS21_BRINGUP_NOHEAT.map \
		-T ts21.ld $(OBJECTS) -lc -lgcc -o $@
	$(CROSS_COMPILE)size $@

$(HEX): $(ELF) tools/inspect_image.py Makefile
	$(PYTHON) tools/inspect_image.py $(ELF) > /dev/null
	$(CROSS_COMPILE)objcopy -O ihex $(ELF) $@
	$(PYTHON) tools/inspect_image.py $(ELF) --hex $@ > /dev/null

$(OUT)/host-tests: tests/host_tests.c hardware.c protocol.c sensors.c ts21.h Makefile | $(OUT)
	$(HOST_CC) $(WARNINGS) -std=c11 -DTS21_HOST_TEST -I. tests/host_tests.c hardware.c protocol.c sensors.c -o $@

$(OUT)/ironos-hardware-tests: tests/host_tests.c hardware.c protocol.c sensors.c ts21.h Makefile | $(OUT)
	$(HOST_CC) $(WARNINGS) -std=c11 -DTS21_HOST_TEST -DTS21_IRONOS -I. tests/host_tests.c hardware.c protocol.c sensors.c -o $@

$(OUT)/heater-tests: tests/host_tests.c hardware.c protocol.c sensors.c ts21.h Makefile | $(OUT)
	$(HOST_CC) $(WARNINGS) -std=c11 -DTS21_HOST_TEST -DTS21_IRONOS -DTS21_HEATER_TEST -I. tests/host_tests.c hardware.c protocol.c sensors.c -o $@

HOST_INCLUDES := -I. -IIronOS -IIronOS/Port -I$(ROOT)/source/Core/BSP \
                 -I$(ROOT)/source/Core/Inc -I$(ROOT)/source/Core/Drivers \
                 -I$(ROOT)/source/Middlewares/Third_Party/FreeRTOS/Source/include \
                 $(addprefix -I,$(shell find $(ROOT)/source/Core/Threads -type d))
HOST_IRONOS_SOURCES := tests/ironos_tests.cpp $(ROOT)/source/Core/Src/power.cpp \
                       $(ROOT)/source/Core/Drivers/Buttons.cpp \
                       $(ROOT)/source/Core/Drivers/OLED.cpp \
                       $(ROOT)/source/Core/Threads/UI/logic/HomeScreen.cpp \
                       $(ROOT)/source/Core/Threads/UI/drawing/mono_128x32/pre_render_assets.cpp

$(OUT)/ironos-tests: $(HOST_IRONOS_SOURCES) Makefile IronOS/configuration.h | $(OUT)
	$(HOST_CXX) -std=c++17 -Wall -Werror -ffunction-sections -fdata-sections -Wl,--gc-sections \
		-D__riscv_xlen=32 -DMODEL_TS21 -DTS21_IRONOS $(HOST_INCLUDES) $(HOST_IRONOS_SOURCES) -o $@

$(OUT)/heater-power-tests: $(HOST_IRONOS_SOURCES) Makefile IronOS/configuration.h | $(OUT)
	$(HOST_CXX) -std=c++17 -Wall -Werror -ffunction-sections -fdata-sections -Wl,--gc-sections \
		-D__riscv_xlen=32 -DMODEL_TS21 -DTS21_IRONOS -DTS21_HEATER_TEST $(HOST_INCLUDES) $(HOST_IRONOS_SOURCES) -o $@

$(OUT)/pid-tests: tests/pid_tests.cpp $(ROOT)/source/Core/Threads/PIDThread.cpp IronOS/configuration.h ts21.h Makefile | $(OUT)
	$(HOST_CXX) -std=c++17 -Wall -Werror -ffunction-sections -fdata-sections -Wl,--gc-sections \
		-D__riscv_xlen=32 -DMODEL_TS21 -DTS21_IRONOS -DTS21_HEATER_TEST $(HOST_INCLUDES) tests/pid_tests.cpp -o $@

$(OUT)/settings_gen.cpp: $(ROOT)/source/Settings/settings.yaml $(ROOT)/source/Settings/generate_settings.py $(ROOT)/source/Settings/settings_gen.cpp.template | $(OUT)
	$(PYTHON) $(ROOT)/source/Settings/generate_settings.py TS21 $(ROOT)/source/Settings/settings.yaml $@

$(OUT)/settings-tests: tests/settings_tests.cpp $(ROOT)/source/Core/Src/Settings.cpp $(ROOT)/source/Core/Inc/Settings.h $(OUT)/settings_gen.cpp Makefile IronOS/configuration.h | $(OUT)
	$(HOST_CXX) -std=c++17 -Wall -Werror -ffunction-sections -fdata-sections -Wl,--gc-sections \
		-D__riscv_xlen=32 -DMODEL_TS21 -DTS21_IRONOS -DTS21_HEATER_TEST -DCANT_DIRECT_READ_SETTINGS \
		$(HOST_INCLUDES) tests/settings_tests.cpp $(ROOT)/source/Core/Src/Settings.cpp $(OUT)/settings_gen.cpp -o $@

test: $(OUT)/host-tests $(OUT)/ironos-hardware-tests $(OUT)/ironos-tests $(OUT)/heater-tests $(OUT)/heater-power-tests $(OUT)/pid-tests $(OUT)/settings-tests
	$(OUT)/host-tests
	$(OUT)/ironos-hardware-tests
	$(OUT)/ironos-tests
	$(OUT)/heater-tests
	$(OUT)/heater-power-tests
	$(OUT)/pid-tests
	$(OUT)/settings-tests
	$(PYTHON) -m unittest discover -s tests -p '*_test.py'
