
# set additional for compiler and linker: optimization and generate map file
set(additional_linker_flags -Wl,-Map=${map_file},--cref,--no-warn-mismatch)
target_link_libraries(${elf_file} PRIVATE ${additional_linker_flags})

# remove unused sections
target_link_libraries(${elf_file} PUBLIC "-Wl,--gc-sections -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--undefined=vTaskSwitchContext -Wl,--undefined=pxCurrentTCB --specs=nosys.specs  -Wl,--print-memory-usage -flto")

set(TARGET_FLAGS "-DMODEL_${MODEL}")

set(OPTIM_FLAGS "-Os -flto -finline-small-functions -findirect-inlining -fdiagnostics-color -ffunction-sections -fdata-sections -fshort-enums -fsingle-precision-constant -ffreestanding -fno-common")

message(ERROR "flags >${CORE_FLAGS}<")
# compiler: language specific flags
set(CMAKE_C_FLAGS " ${CORE_FLAGS} ${OPTIM_FLAGS} ${TARGET_FLAGS} -fno-builtin -Wall -std=gnu11 -fdata-sections -ffunction-sections -g3 " CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS " ${CORE_FLAGS} ${OPTIM_FLAGS} ${TARGET_FLAGS} -fno-rtti -fno-exceptions -fno-builtin -Wall -std=gnu++11 -fdata-sections -ffunction-sections -g -ggdb3" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_ASM_FLAGS " ${CORE_FLAGS} ${OPTIM_FLAGS} ${TARGET_FLAGS} -g -ggdb3 -D__USES_CXX" CACHE INTERNAL "asm compiler flags")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

