# Middlewares CMakeLists.txt
# Middleware components source files

# Find all C and C++ source files in this directory and subdirectories
file(GLOB_RECURSE MIDDLEWARES_SOURCES
    "*.c"
    "*.cpp"
)

# Create object library for middleware sources
add_library(ironos_middlewares OBJECT ${MIDDLEWARES_SOURCES})

# Set compilation properties
target_compile_options(ironos_middlewares PRIVATE
    ${CPU_FLAGS}
    ${OPTIMIZATION_FLAGS}
    ${GLOBAL_DEFINES}
    ${DEV_CFLAGS}
    ${DEV_CXXFLAGS}
    -MMD
    -g3
)

target_compile_options(ironos_middlewares PRIVATE
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
target_include_directories(ironos_middlewares PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Core/BSP
    ${DEVICE_BSP_DIR}
    ${IRONOS_INCLUDE_DIRS}
)

# Set position independent code if needed
set_target_properties(ironos_middlewares PROPERTIES POSITION_INDEPENDENT_CODE OFF)

# Find all middleware include directories
file(GLOB_RECURSE MIDDLEWARES_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/*"
)

# Add middleware include directories to the global list
foreach(DIR ${MIDDLEWARES_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Make include directories available to parent scope
set(IRONOS_INCLUDE_DIRS ${IRONOS_INCLUDE_DIRS} PARENT_SCOPE)
