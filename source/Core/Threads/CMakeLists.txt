# Core/Threads CMakeLists.txt
# Thread implementation source files

# Find all C and C++ source files in this directory and subdirectories
file(GLOB_RECURSE THREADS_SOURCES
    "*.c"
    "*.cpp"
)

# Create object library for thread sources
add_library(ironos_threads OBJECT ${THREADS_SOURCES})

# Set compilation properties
target_compile_options(ironos_threads PRIVATE
    ${CPU_FLAGS}
    ${OPTIMIZATION_FLAGS}
    ${GLOBAL_DEFINES}
    ${DEV_CFLAGS}
    ${DEV_CXXFLAGS}
    -MMD
    -g3
)

target_compile_options(ironos_threads PRIVATE
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

# Include directories - make sure we have all the paths we need
target_include_directories(ironos_threads PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Core/BSP
    ${DEVICE_BSP_DIR}
    ${IRONOS_INCLUDE_DIRS}
)

# Set position independent code if needed
set_target_properties(ironos_threads PROPERTIES POSITION_INDEPENDENT_CODE OFF)
