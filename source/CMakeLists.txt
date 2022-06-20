cmake_minimum_required(VERSION 3.13)
set(application_name "${MODEL}_${LANG}")
set(elf_file ${application_name}.elf)
set(bin_file ${application_name}.bin)
set(hex_file ${application_name}.hex)
set(map_file ${application_name}.map)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMake")



if (MODEL STREQUAL TS100 OR MODEL STREQUAL TS80 OR MODEL STREQUAL TS80P)
    set(CMAKE_TOOLCHAIN_FILE "CMake/arm-gcc.cmake" CACHE PATH "toolchain file")
    include(${CMAKE_TOOLCHAIN_FILE})
elseif(MODEL STREQUAL Pinecil)
    set(CMAKE_TOOLCHAIN_FILE "CMake/riscv-gcc.cmake" CACHE PATH "toolchain file")
    include(${CMAKE_TOOLCHAIN_FILE})
elseif(MODEL STREQUAL MHP30)
    set(CMAKE_TOOLCHAIN_FILE "CMake/arm-gcc.cmake" CACHE PATH "toolchain file")
    include(${CMAKE_TOOLCHAIN_FILE})
else()
 message(FATAL_ERROR "You must specify a model")
endif()


# Export compile commands for IDE's
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#add_compile_options( -Wall -Werror)


project(IronOS
  LANGUAGES C CXX ASM
)
#Setup outputs

project (${application_name} C CXX ASM)


# add sources to elf file

file(WRITE null.cpp "")

add_executable(${elf_file}
    null.cpp
)




include_directories(
  Core
  Middlewares
)
add_subdirectory(Core)
add_subdirectory(Middlewares)



target_link_libraries(${elf_file} PRIVATE  BSP mainSource threads FreeRTOS languages  BSPImplementation drivers USBPDLib)

if (MODEL STREQUAL TS100 OR MODEL STREQUAL TS80 OR MODEL STREQUAL TS80P)
    include(stm32f1)
elseif(MODEL STREQUAL Pinecil)
    include(gd32vf1)
elseif(MODEL STREQUAL MHP30)
    include(stm32f1)
else()
    message(FATAL_ERROR "You must specify a model")
endif()
include(shared-compiler-settings)


# set additional for compiler and linker: optimization and generate map file
set(additional_compiler_flags ${opt_level})
set(additional_linker_flags -Wl,-Map=${map_file},--cref,--no-warn-mismatch)
target_compile_options(${elf_file} PRIVATE ${additional_compiler_flags})
target_link_libraries(${elf_file} PRIVATE ${additional_linker_flags})

# remove unused sections



# create binary and hex files
add_custom_target(${hex_file} DEPENDS ${elf_file} COMMAND ${CMAKE_OBJCOPY} -O ihex ${elf_file} ${hex_file})
add_custom_target(${bin_file} DEPENDS ${elf_file} COMMAND ${CMAKE_OBJCOPY} -O binary ${elf_file} ${bin_file})
