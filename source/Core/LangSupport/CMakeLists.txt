# Core/LangSupport CMakeLists.txt
# Language support source files

# Find all C and C++ source files in this directory
file(GLOB LANGSUPPORT_SOURCES
    "*.c"
    "*.cpp"
)

# Create object library for language support sources
add_library(ironos_langsupport OBJECT ${LANGSUPPORT_SOURCES})



# Set compilation properties
target_compile_options(ironos_langsupport PRIVATE
    ${CPU_FLAGS}
    ${OPTIMIZATION_FLAGS}
    ${GLOBAL_DEFINES}
    ${DEV_CFLAGS}
    ${DEV_CXXFLAGS}
    -MMD
    -g3
)

target_compile_options(ironos_langsupport PRIVATE
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
target_include_directories(ironos_langsupport PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Core/BSP
    ${DEVICE_BSP_DIR}
    ${IRONOS_INCLUDE_DIRS}
)

# Set position independent code if needed
set_target_properties(ironos_langsupport PROPERTIES POSITION_INDEPENDENT_CODE OFF)

# Create specific targets for single and multi-language support
# These will be individual object files needed by different firmware variants

# Generate lang_single.o target
add_custom_command(
    OUTPUT "${OUTPUT_DIR}/Core/LangSupport/lang_single.o"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Core/LangSupport"
    COMMAND ${CMAKE_CXX_COMPILER} -c
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${GLOBAL_DEFINES}
        ${DEV_CXXFLAGS}
        -I"${CMAKE_SOURCE_DIR}/Core/Inc"
        -I"${CMAKE_SOURCE_DIR}/Core/BSP"
        -I"${DEVICE_BSP_DIR}"
        ${WARNING_FLAGS_COMMON}
        -std=c++17
        -fno-rtti
        -fno-exceptions
        -fno-non-call-exceptions
        -fno-use-cxa-atexit
        -fno-strict-aliasing
        -fno-threadsafe-statics
        -MMD
        -g3
        -DLANG_SUPPORT_SINGLE
        "${CMAKE_CURRENT_SOURCE_DIR}/lang_single.cpp"
        -o "${OUTPUT_DIR}/Core/LangSupport/lang_single.o"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lang_single.cpp"
    COMMENT "Compiling single language support"
)

add_custom_target(generate_lang_single
    DEPENDS "${OUTPUT_DIR}/Core/LangSupport/lang_single.o"
)

# Generate lang_multi.o target
add_custom_command(
    OUTPUT "${OUTPUT_DIR}/Core/LangSupport/lang_multi.o"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Core/LangSupport"
    COMMAND ${CMAKE_CXX_COMPILER} -c
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${GLOBAL_DEFINES}
        ${DEV_CXXFLAGS}
        -I"${CMAKE_SOURCE_DIR}/Core/Inc"
        -I"${CMAKE_SOURCE_DIR}/Core/BSP"
        -I"${DEVICE_BSP_DIR}"
        ${WARNING_FLAGS_COMMON}
        -std=c++17
        -fno-rtti
        -fno-exceptions
        -fno-non-call-exceptions
        -fno-use-cxa-atexit
        -fno-strict-aliasing
        -fno-threadsafe-statics
        -MMD
        -g3
        -DLANG_SUPPORT_MULTI
        "${CMAKE_CURRENT_SOURCE_DIR}/lang_multi.cpp"
        -o "${OUTPUT_DIR}/Core/LangSupport/lang_multi.o"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lang_multi.cpp"
    COMMENT "Compiling multi-language support"
)

add_custom_target(generate_lang_multi
    DEPENDS "${OUTPUT_DIR}/Core/LangSupport/lang_multi.o"
)
