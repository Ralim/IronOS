cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/tools/cmake/riscv64-unknown-elf-gcc.cmake)
include(${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver/cpu_flags.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/compiler_flags.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/tools.cmake)

PROJECT(${BOARD} C CXX ASM)
# set(CMAKE_VERBOSE_MAKEFILE ON)
ENABLE_LANGUAGE(ASM)

include_directories(${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/bsp/board/${CHIP})
include_directories(${CMAKE_SOURCE_DIR}/bsp/board/${CHIP})
else()
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/bsp/board/${CHIP} is not exist")
endif()

search_application(${CMAKE_SOURCE_DIR}/${APP_DIR})