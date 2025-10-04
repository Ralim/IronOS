# Core/BSP CMakeLists.txt
# Board Support Package source files

# Find all C, C++, and Assembly source files in the model-specific BSP directory
file(GLOB_RECURSE BSP_SOURCES
    "${DEVICE_BSP_DIR}/*.c"
    "${DEVICE_BSP_DIR}/*.cpp"
    "${DEVICE_BSP_DIR}/*.S"
)

# Also include common BSP files
file(GLOB BSP_COMMON_SOURCES
    "BSP_Common.c"
)

# Combine all BSP sources
set(ALL_BSP_SOURCES ${BSP_SOURCES} ${BSP_COMMON_SOURCES})

# Create object library for BSP sources
add_library(ironos_bsp OBJECT ${ALL_BSP_SOURCES})


# Set compilation properties
target_compile_options(ironos_bsp PRIVATE
    ${CPU_FLAGS}
    ${OPTIMIZATION_FLAGS}
    ${GLOBAL_DEFINES}
    ${DEV_CFLAGS}
    ${DEV_CXXFLAGS}
    ${DEV_AFLAGS}
    -MMD
    -g3
)

target_compile_options(ironos_bsp PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${WARNING_FLAGS_C}>
    $<$<COMPILE_LANGUAGE:CXX>:${WARNING_FLAGS_COMMON}>
    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-strict-aliasing>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
)

# Assembly-specific compilation options
target_compile_options(ironos_bsp PRIVATE
    $<$<COMPILE_LANGUAGE:ASM>:${CPU_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${DEV_AFLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${GLOBAL_DEFINES}>
    $<$<COMPILE_LANGUAGE:ASM>:${OPTIMIZATION_FLAGS}>
)

# Include directories - make sure we have all the paths we need
target_include_directories(ironos_bsp PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Core/BSP
    ${DEVICE_BSP_DIR}
    ${IRONOS_INCLUDE_DIRS}
)

# Set position independent code if needed
set_target_properties(ironos_bsp PROPERTIES POSITION_INDEPENDENT_CODE OFF)

# Print information about BSP being built
message(STATUS "Building BSP for: ${DEVICE_BSP_DIR}")
message(STATUS "BSP sources found: ${ALL_BSP_SOURCES}")
