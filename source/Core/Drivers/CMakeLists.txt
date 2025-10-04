# Core/Drivers CMakeLists.txt
# Device drivers source files

# Find all C and C++ source files in this directory and subdirectories
# Exclude USB-PD tests directory
file(GLOB_RECURSE DRIVERS_SOURCES
    "*.c"
    "*.cpp"
)

# Filter out USB-PD test files
list(FILTER DRIVERS_SOURCES EXCLUDE REGEX ".*usb-pd/tests/.*")

# Create object library for driver sources
add_library(ironos_drivers OBJECT ${DRIVERS_SOURCES})



# Set compilation properties
target_compile_options(ironos_drivers PRIVATE
    ${CPU_FLAGS}
    ${OPTIMIZATION_FLAGS}
    ${GLOBAL_DEFINES}
    ${DEV_CFLAGS}
    ${DEV_CXXFLAGS}
    -MMD
    -g3
)

target_compile_options(ironos_drivers PRIVATE
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
target_include_directories(ironos_drivers PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Core/BSP
    ${DEVICE_BSP_DIR}
    ${IRONOS_INCLUDE_DIRS}
)

# Set position independent code if needed
set_target_properties(ironos_drivers PROPERTIES POSITION_INDEPENDENT_CODE OFF)
