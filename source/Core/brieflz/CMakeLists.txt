# Core/brieflz CMakeLists.txt
# BriefLZ compression library source files

# Find all C source files in this directory
file(GLOB BRIEFLZ_SOURCES
    "depack.c"
)

# Create object library for brieflz sources
add_library(ironos_brieflz OBJECT ${BRIEFLZ_SOURCES})

# Set compilation properties
target_compile_options(ironos_brieflz PRIVATE
    ${CPU_FLAGS}
    ${OPTIMIZATION_FLAGS}
    ${GLOBAL_DEFINES}
    ${DEV_CFLAGS}
    -MMD
    -g3
)

target_compile_options(ironos_brieflz PRIVATE
    ${WARNING_FLAGS_C}
    -std=gnu11
)

# Include directories - make sure we have all the paths we need
target_include_directories(ironos_brieflz PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Core/BSP
    ${DEVICE_BSP_DIR}
    ${IRONOS_INCLUDE_DIRS}
)

# Set position independent code if needed
set_target_properties(ironos_brieflz PROPERTIES POSITION_INDEPENDENT_CODE OFF)
