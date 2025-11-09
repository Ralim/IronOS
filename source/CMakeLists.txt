cmake_minimum_required(VERSION 3.20)

# Set the model if not specified
if(NOT DEFINED MODEL)
    set(MODEL "Pinecilv2" CACHE STRING "Device model to build for")
endif()

# Define all valid models
set(ALL_MINIWARE_MODELS "TS100" "TS80" "TS80P" "TS101")
set(ALL_PINECIL_MODELS "Pinecil")
set(ALL_PINECIL_V2_MODELS "Pinecilv2")
set(ALL_MHP30_MODELS "MHP30")
set(ALL_SEQURE_MODELS "S60" "S60P" "T55")
set(ALL_MODELS ${ALL_MINIWARE_MODELS} ${ALL_PINECIL_MODELS} ${ALL_MHP30_MODELS} ${ALL_PINECIL_V2_MODELS} ${ALL_SEQURE_MODELS})

# Validate model
if(NOT MODEL IN_LIST ALL_MODELS)
    message(FATAL_ERROR "Invalid model '${MODEL}', valid options are: ${ALL_MODELS}")
endif()

# Model-specific configuration (must be before project() to set cross-compiler)
include(cmake/ModelConfig.cmake)

# Set project name and languages
project(IronOS
    LANGUAGES C CXX ASM
    VERSION 2.23
)

# Create imported library targets for Pinecilv2 binary blobs (after project initialization)
if(MODEL IN_LIST ALL_PINECIL_V2_MODELS)
    add_library(pinecilv2_ble_controller STATIC IMPORTED)
    set_target_properties(pinecilv2_ble_controller PROPERTIES
        IMPORTED_LOCATION "${PINECILV2_BLE_LIB_PATH}"
    )

    add_library(pinecilv2_rf STATIC IMPORTED)
    set_target_properties(pinecilv2_rf PROPERTIES
        IMPORTED_LOCATION "${PINECILV2_RF_LIB_PATH}"
    )

    # Store library targets for linking
    set(PINECILV2_LIBRARIES pinecilv2_ble_controller pinecilv2_rf)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable export of compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Define output directories matching Makefile structure
set(HEXFILE_DIR "${CMAKE_BINARY_DIR}/Hexfile")
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Objects/${MODEL}")

# Set global object file output directory
set(CMAKE_OBJECT_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

# Create output directories
file(MAKE_DIRECTORY ${HEXFILE_DIR})
file(MAKE_DIRECTORY ${OUTPUT_DIR})

# Host tools configuration
if(NOT DEFINED HOST_PYTHON)
    set(HOST_PYTHON "python3")
endif()

if(NOT DEFINED HOST_CC)
    set(HOST_CC "gcc")
endif()

# DFU configuration
set(DEVICE_DFU_VID_PID "0x28E9:0x0189")

# Language configuration
set(ALL_LANGUAGES "BE" "BG" "CS" "DA" "DE" "EL" "EN" "ES" "ET" "FI" "FR" "HR" "HU" "IT" "JA_JP" "LT" "NB" "NL_BE" "NL" "PL" "PT" "RO" "RU" "SK" "SL" "SR_CYRL" "SR_LATN" "SV" "TR" "UK" "UZ" "VI" "YUE_HK" "ZH_CN" "ZH_TW")

# Language groups
set(LANGUAGE_GROUP_CJK_LANGS "EN" "JA_JP" "YUE_HK" "ZH_TW" "ZH_CN")
set(LANGUAGE_GROUP_CJK_NAME "Chinese+Japanese")

set(LANGUAGE_GROUP_CYRILLIC_LANGS "EN" "BE" "BG" "RU" "SR_CYRL" "SR_LATN" "UK")
set(LANGUAGE_GROUP_CYRILLIC_NAME "Belorussian+Bulgarian+Russian+Serbian+Ukrainian")

# European languages (exclude CJK and Cyrillic)
set(LANGUAGE_GROUP_EUR_LANGS "EN" "CS" "DA" "DE" "EL" "ES" "ET" "FI" "FR" "HR" "HU" "IT" "LT" "NB" "NL_BE" "NL" "PL" "PT" "RO" "SK" "SL" "SV" "TR" "UZ" "VI")
set(LANGUAGE_GROUP_EUR_NAME "European")

set(LANGUAGE_GROUPS "CJK" "CYRILLIC" "EUR")

# Custom multi-language support
if(DEFINED CUSTOM_MULTI_LANGS)
    set(LANGUAGE_GROUP_CUSTOM_LANGS ${CUSTOM_MULTI_LANGS})
    set(LANGUAGE_GROUP_CUSTOM_NAME "Custom")
    list(APPEND LANGUAGE_GROUPS "CUSTOM")
    # Clean up generated files for custom builds
    file(REMOVE_RECURSE "${CMAKE_SOURCE_DIR}/Core/Gen")
    file(REMOVE_RECURSE "${OUTPUT_DIR}/Core/Gen")
    file(GLOB CUSTOM_FILES "${HEXFILE_DIR}/*_Custom.*")
    if(CUSTOM_FILES)
        file(REMOVE ${CUSTOM_FILES})
    endif()
endif()

# Include directories (will be populated by subdirectories)
set(IRONOS_INCLUDE_DIRS "")

# Common optimization flags
# Override CMake's default Release optimization flags to match make build
# CMake automatically adds -O3 -DNDEBUG for Release builds, but we want to control this exactly
# Make build doesn't use NDEBUG, so we remove it here
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_CXX_FLAGS_RELEASE "")
set(CMAKE_ASM_FLAGS_RELEASE "")

set(OPTIMIZATION_FLAGS
    -Os
    -fno-jump-tables
    -foptimize-strlen
    -faggressive-loop-optimizations
    -fdevirtualize-at-ltrans
    -fmerge-all-constants
    -fshort-wchar
    -flto=auto
    -finline-small-functions
    -finline-functions
    -findirect-inlining
    -fdiagnostics-color
    -ffunction-sections
    -fdata-sections
    -fshort-enums
    -fsingle-precision-constant
    -fno-common
    -fno-math-errno
    -ffast-math
    -ffinite-math-only
    -fno-signed-zeros
)

# Common warning flags
set(WARNING_FLAGS_COMMON
    -Wtrigraphs
    -Wuninitialized
    -Wmissing-braces
    -Wfloat-equal
    -Wunreachable-code
    -Wswitch-default
    -Wreturn-type
    -Wundef
    -Wparentheses
    -Wnonnull
    -Winit-self
    -Wmissing-include-dirs
    -Wsequence-point
    -Wswitch
    -Wformat
    -Wsign-compare
    -Waddress
    -Waggregate-return
    -Wmissing-field-initializers
    -Wshadow
    -Wno-unused-parameter
    -Wno-undef
    -Wdouble-promotion
)

# C-specific warning flags
set(WARNING_FLAGS_C
    ${WARNING_FLAGS_COMMON}
    -Wbad-function-cast
)

# Pre-populate include directories before processing subdirectories
list(APPEND IRONOS_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}/Core/Inc"
    "${CMAKE_SOURCE_DIR}/Core/BSP"
    "${DEVICE_BSP_DIR}"
)

# Find all BSP include directories for the current model
file(GLOB_RECURSE BSP_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${DEVICE_BSP_DIR}/*"
)
foreach(DIR ${BSP_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Find all Core include directories
file(GLOB_RECURSE CORE_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_SOURCE_DIR}/Core/*"
)
foreach(DIR ${CORE_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Find all Middleware include directories
file(GLOB_RECURSE MIDDLEWARE_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_SOURCE_DIR}/Middlewares/*"
)
foreach(DIR ${MIDDLEWARE_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Set up global include directories before processing subdirectories
include_directories(${IRONOS_INCLUDE_DIRS})

# Add subdirectories with custom binary directories
add_subdirectory(Core ${CMAKE_BINARY_DIR}/Objects/${MODEL}/Core)
add_subdirectory(Middlewares ${CMAKE_BINARY_DIR}/Objects/${MODEL}/Middlewares)

# Create the main firmware targets
include(cmake/FirmwareTargets.cmake)

# Custom target for cleaning generated files
add_custom_target(clean-generated
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/Core/Gen"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/Objects"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${HEXFILE_DIR}"
    COMMENT "Cleaning generated files"
)

# Add phony targets for compatibility
add_custom_target(clean-all DEPENDS clean-generated)

# Print configuration summary
message(STATUS "Building for model: ${MODEL}")
message(STATUS "Compiler prefix: ${COMPILER_PREFIX}")
message(STATUS "CPU flags: ${CPU_FLAGS}")
message(STATUS "Flash size: ${FLASH_SIZE}")
message(STATUS "Bootloader size: ${BOOTLDR_SIZE}")
message(STATUS "DFU address: ${DEVICE_DFU_ADDRESS}")
