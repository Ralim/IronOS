# Firmware targets configuration for IronOS
# This file creates the main firmware executable targets and their variants

# Function to create firmware targets for a specific language
function(create_firmware_target LANGUAGE)
    set(TARGET_NAME "${MODEL}_${LANGUAGE}")
    set(ELF_FILE "${HEXFILE_DIR}/${TARGET_NAME}.elf")
    set(HEX_FILE "${HEXFILE_DIR}/${TARGET_NAME}.hex")
    set(BIN_FILE "${HEXFILE_DIR}/${TARGET_NAME}.bin")
    set(DFU_FILE "${HEXFILE_DIR}/${TARGET_NAME}.dfu")

    # Create a dummy source file for the executable (CMake requirement)
    set(DUMMY_SOURCE "${CMAKE_BINARY_DIR}/dummy_${TARGET_NAME}.c")
    file(WRITE ${DUMMY_SOURCE} "// Dummy source file for ${TARGET_NAME}\n")

    # Create the main executable target
    add_executable(${TARGET_NAME} ${DUMMY_SOURCE})

    # Set target properties
    set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${TARGET_NAME}.elf"
        RUNTIME_OUTPUT_DIRECTORY "${HEXFILE_DIR}"
        LINK_DEPENDS "${LDSCRIPT}"
    )

    # Link against the object libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        ironos_core
        ironos_threads
        ironos_drivers
        ironos_bsp
        ironos_middlewares
        ironos_brieflz
    )

    # Link Pinecilv2 binary blob libraries if building for Pinecilv2
    if(MODEL IN_LIST ALL_PINECIL_V2_MODELS)
        target_link_libraries(${TARGET_NAME} PRIVATE ${PINECILV2_LIBRARIES})
    endif()

    # Apply compiler and linker flags
    target_compile_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${GLOBAL_DEFINES}
        ${DEV_CFLAGS}
        ${DEV_CXXFLAGS}
        -MMD
        -g3
    )

    target_compile_options(${TARGET_NAME} PRIVATE
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

    target_link_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${LINKER_FLAGS}
        -T${LDSCRIPT}
        -Wl,-Map=${ELF_FILE}.map
    )

    # Include directories
    target_include_directories(${TARGET_NAME} PRIVATE ${IRONOS_INCLUDE_DIRS})

    # Add translation object file to link
    target_link_options(${TARGET_NAME} PRIVATE
        "${OUTPUT_DIR}/Core/Gen/Translation.${LANGUAGE}.o"
        "${OUTPUT_DIR}/Core/LangSupport/lang_single.o"
    )

    # Dependencies
    add_dependencies(${TARGET_NAME}
        generate_translation_${LANGUAGE}
        compile_main_translation_${LANGUAGE}
        generate_lang_single
    )

    # Create HEX file
    add_custom_command(
        OUTPUT ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${ELF_FILE} ${HEX_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating HEX file for ${TARGET_NAME}"
    )

    # Create BIN file
    add_custom_command(
        OUTPUT ${BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF_FILE} ${BIN_FILE}
        COMMAND ${CMAKE_SIZE} ${ELF_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating BIN file for ${TARGET_NAME}"
    )

    # Create DFU file
    add_custom_command(
        OUTPUT ${DFU_FILE}
        COMMAND ${HOST_PYTHON} ${CMAKE_SOURCE_DIR}/dfuse-pack.py
            -b ${DEVICE_DFU_ADDRESS}@0:${BIN_FILE}
            -D ${DEVICE_DFU_VID_PID}
            ${DFU_FILE}
        DEPENDS ${BIN_FILE}
        COMMENT "Creating DFU file for ${TARGET_NAME}"
    )

    # Create custom targets for the output files
    add_custom_target(${TARGET_NAME}.hex DEPENDS ${HEX_FILE})
    add_custom_target(${TARGET_NAME}.bin DEPENDS ${BIN_FILE})
    add_custom_target(${TARGET_NAME}.dfu DEPENDS ${DFU_FILE})

    # Create a combined target for all formats
    add_custom_target(firmware-${LANGUAGE}
        DEPENDS ${HEX_FILE} ${BIN_FILE} ${DFU_FILE}
    )
endfunction()

# Function to create compressed string firmware targets
function(create_compressed_string_target LANGUAGE)
    set(TARGET_NAME "${MODEL}_string_compressed_${LANGUAGE}")
    set(ELF_FILE "${HEXFILE_DIR}/${TARGET_NAME}.elf")
    set(HEX_FILE "${HEXFILE_DIR}/${TARGET_NAME}.hex")
    set(BIN_FILE "${HEXFILE_DIR}/${TARGET_NAME}.bin")
    set(DFU_FILE "${HEXFILE_DIR}/${TARGET_NAME}.dfu")

    # Create a dummy source file for the executable (CMake requirement)
    set(DUMMY_SOURCE "${CMAKE_BINARY_DIR}/dummy_${TARGET_NAME}.c")
    file(WRITE ${DUMMY_SOURCE} "// Dummy source file for ${TARGET_NAME}\n")

    # Create the executable target
    add_executable(${TARGET_NAME} ${DUMMY_SOURCE})

    # Set target properties
    set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${TARGET_NAME}.elf"
        RUNTIME_OUTPUT_DIRECTORY "${HEXFILE_DIR}"
        LINK_DEPENDS "${LDSCRIPT}"
    )

    # Link against the object libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        ironos_core
        ironos_threads
        ironos_drivers
        ironos_bsp
        ironos_middlewares
        ironos_brieflz
    )

    # Link Pinecilv2 binary blob libraries if building for Pinecilv2
    if(MODEL IN_LIST ALL_PINECIL_V2_MODELS)
        target_link_libraries(${TARGET_NAME} PRIVATE ${PINECILV2_LIBRARIES})
    endif()

    # Apply same compilation settings as regular firmware
    target_compile_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${GLOBAL_DEFINES}
        ${DEV_CFLAGS}
        ${DEV_CXXFLAGS}
        -MMD
        -g3
    )

    target_compile_options(${TARGET_NAME} PRIVATE
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

    target_link_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${LINKER_FLAGS}
        -T${LDSCRIPT}
        -Wl,-Map=${ELF_FILE}.map
    )

    # Include directories
    target_include_directories(${TARGET_NAME} PRIVATE ${IRONOS_INCLUDE_DIRS})

    # Add translation object file to link
    target_link_options(${TARGET_NAME} PRIVATE
        "${OUTPUT_DIR}/Core/Gen/Translation_brieflz.${LANGUAGE}.o"
        "${OUTPUT_DIR}/Core/LangSupport/lang_single.o"
    )

    # Dependencies
    add_dependencies(${TARGET_NAME}
        generate_translation_brieflz_${LANGUAGE}
        generate_lang_single
    )

    # Create output files (same as regular firmware)
    add_custom_command(
        OUTPUT ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${ELF_FILE} ${HEX_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating compressed string HEX file for ${TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF_FILE} ${BIN_FILE}
        COMMAND ${CMAKE_SIZE} ${ELF_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating compressed string BIN file for ${TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${DFU_FILE}
        COMMAND ${HOST_PYTHON} ${CMAKE_SOURCE_DIR}/dfuse-pack.py
            -b ${DEVICE_DFU_ADDRESS}@0:${BIN_FILE}
            -D ${DEVICE_DFU_VID_PID}
            ${DFU_FILE}
        DEPENDS ${BIN_FILE}
        COMMENT "Creating compressed string DFU file for ${TARGET_NAME}"
    )

    # Create custom targets
    add_custom_target(${TARGET_NAME}.hex DEPENDS ${HEX_FILE})
    add_custom_target(${TARGET_NAME}.bin DEPENDS ${BIN_FILE})
    add_custom_target(${TARGET_NAME}.dfu DEPENDS ${DFU_FILE})
    add_custom_target(firmware-string-compressed-${LANGUAGE}
        DEPENDS ${HEX_FILE} ${BIN_FILE} ${DFU_FILE}
    )
endfunction()

# Function to create compressed font firmware targets
function(create_compressed_font_target LANGUAGE)
    set(TARGET_NAME "${MODEL}_font_compressed_${LANGUAGE}")
    set(ELF_FILE "${HEXFILE_DIR}/${TARGET_NAME}.elf")
    set(HEX_FILE "${HEXFILE_DIR}/${TARGET_NAME}.hex")
    set(BIN_FILE "${HEXFILE_DIR}/${TARGET_NAME}.bin")
    set(DFU_FILE "${HEXFILE_DIR}/${TARGET_NAME}.dfu")

    # Create a dummy source file for the executable (CMake requirement)
    set(DUMMY_SOURCE "${CMAKE_BINARY_DIR}/dummy_${TARGET_NAME}.c")
    file(WRITE ${DUMMY_SOURCE} "// Dummy source file for ${TARGET_NAME}\n")

    # Create the executable target
    add_executable(${TARGET_NAME} ${DUMMY_SOURCE})

    # Set target properties
    set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${TARGET_NAME}.elf"
        RUNTIME_OUTPUT_DIRECTORY "${HEXFILE_DIR}"
        LINK_DEPENDS "${LDSCRIPT}"
    )

    # Link against the object libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        ironos_core
        ironos_threads
        ironos_drivers
        ironos_bsp
        ironos_middlewares
        ironos_brieflz
    )

    # Link Pinecilv2 binary blob libraries if building for Pinecilv2
    if(MODEL IN_LIST ALL_PINECIL_V2_MODELS)
        target_link_libraries(${TARGET_NAME} PRIVATE ${PINECILV2_LIBRARIES})
    endif()

    # Apply same compilation settings as regular firmware
    # Apply same compilation settings
    target_compile_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${GLOBAL_DEFINES}
        ${DEV_CFLAGS}
        ${DEV_CXXFLAGS}
        -MMD
        -g3
    )

    target_compile_options(${TARGET_NAME} PRIVATE
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

    target_link_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${LINKER_FLAGS}
        -T${LDSCRIPT}
        -Wl,-Map=${ELF_FILE}.map
    )

    # Include directories
    target_include_directories(${TARGET_NAME} PRIVATE ${IRONOS_INCLUDE_DIRS})

    # Add translation object file to link
    target_link_options(${TARGET_NAME} PRIVATE
        "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_font.${LANGUAGE}.o"
        "${OUTPUT_DIR}/Core/LangSupport/lang_single.o"
    )

    # Dependencies
    add_dependencies(${TARGET_NAME}
        generate_translation_brieflz_font_${LANGUAGE}
        generate_lang_single
    )

    # Create output files
    add_custom_command(
        OUTPUT ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${ELF_FILE} ${HEX_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating compressed font HEX file for ${TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF_FILE} ${BIN_FILE}
        COMMAND ${CMAKE_SIZE} ${ELF_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating compressed font BIN file for ${TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${DFU_FILE}
        COMMAND ${HOST_PYTHON} ${CMAKE_SOURCE_DIR}/dfuse-pack.py
            -b ${DEVICE_DFU_ADDRESS}@0:${BIN_FILE}
            -D ${DEVICE_DFU_VID_PID}
            ${DFU_FILE}
        DEPENDS ${BIN_FILE}
        COMMENT "Creating compressed font DFU file for ${TARGET_NAME}"
    )

    # Create custom targets
    add_custom_target(${TARGET_NAME}.hex DEPENDS ${HEX_FILE})
    add_custom_target(${TARGET_NAME}.bin DEPENDS ${BIN_FILE})
    add_custom_target(${TARGET_NAME}.dfu DEPENDS ${DFU_FILE})
    add_custom_target(firmware-font-compressed-${LANGUAGE}
        DEPENDS ${HEX_FILE} ${BIN_FILE} ${DFU_FILE}
    )
endfunction()

# Function to create multi-language firmware targets
function(create_multi_language_target GROUP_CODE GROUP_NAME LANGUAGES)
    set(TARGET_NAME "${MODEL}_multi_${GROUP_NAME}")
    set(ELF_FILE "${HEXFILE_DIR}/${TARGET_NAME}.elf")
    set(HEX_FILE "${HEXFILE_DIR}/${TARGET_NAME}.hex")
    set(BIN_FILE "${HEXFILE_DIR}/${TARGET_NAME}.bin")
    set(DFU_FILE "${HEXFILE_DIR}/${TARGET_NAME}.dfu")

    # Create a dummy source file for the executable (CMake requirement)
    set(DUMMY_SOURCE "${CMAKE_BINARY_DIR}/dummy_${TARGET_NAME}.c")
    file(WRITE ${DUMMY_SOURCE} "// Dummy source file for ${TARGET_NAME}\n")

    # Create the executable target
    add_executable(${TARGET_NAME} ${DUMMY_SOURCE})

    # Set target properties and compilation settings (same as others)
    set_target_properties(${TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${TARGET_NAME}.elf"
        RUNTIME_OUTPUT_DIRECTORY "${HEXFILE_DIR}"
        LINK_DEPENDS "${LDSCRIPT}"
    )

    # Link against the object libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        ironos_core
        ironos_threads
        ironos_drivers
        ironos_bsp
        ironos_middlewares
        ironos_brieflz
    )

    # Link Pinecilv2 binary blob libraries if building for Pinecilv2
    if(MODEL IN_LIST ALL_PINECIL_V2_MODELS)
        target_link_libraries(${TARGET_NAME} PRIVATE ${PINECILV2_LIBRARIES})
    endif()

    # Apply same compilation settings as regular firmware
    target_compile_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${GLOBAL_DEFINES}
        ${DEV_CFLAGS}
        ${DEV_CXXFLAGS}
        -MMD
        -g3
    )

    target_compile_options(${TARGET_NAME} PRIVATE
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

    target_link_options(${TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${LINKER_FLAGS}
        -T${LDSCRIPT}
        -Wl,-Map=${ELF_FILE}.map
    )

    target_include_directories(${TARGET_NAME} PRIVATE ${IRONOS_INCLUDE_DIRS})

    # Add translation object file to link
    target_link_options(${TARGET_NAME} PRIVATE
        "${OUTPUT_DIR}/Core/Gen/Translation_multi.${GROUP_CODE}.o"
        "${OUTPUT_DIR}/Core/LangSupport/lang_multi.o"
    )

    add_dependencies(${TARGET_NAME}
        generate_translation_multi_${GROUP_CODE}
        generate_lang_multi
    )

    # Create output files
    add_custom_command(
        OUTPUT ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${ELF_FILE} ${HEX_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating multi-language HEX file for ${TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${ELF_FILE} ${BIN_FILE}
        COMMAND ${CMAKE_SIZE} ${ELF_FILE}
        DEPENDS ${TARGET_NAME}
        COMMENT "Creating multi-language BIN file for ${TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${DFU_FILE}
        COMMAND ${HOST_PYTHON} ${CMAKE_SOURCE_DIR}/dfuse-pack.py
            -b ${DEVICE_DFU_ADDRESS}@0:${BIN_FILE}
            -D ${DEVICE_DFU_VID_PID}
            ${DFU_FILE}
        DEPENDS ${BIN_FILE}
        COMMENT "Creating multi-language DFU file for ${TARGET_NAME}"
    )

    add_custom_target(${TARGET_NAME}.hex DEPENDS ${HEX_FILE})
    add_custom_target(${TARGET_NAME}.bin DEPENDS ${BIN_FILE})
    add_custom_target(${TARGET_NAME}.dfu DEPENDS ${DFU_FILE})
    add_custom_target(firmware-multi-${GROUP_NAME}
        DEPENDS ${HEX_FILE} ${BIN_FILE} ${DFU_FILE}
    )

    # Also create compressed multi-language target
    set(COMPRESSED_TARGET_NAME "${MODEL}_multi_compressed_${GROUP_NAME}")
    set(COMPRESSED_ELF_FILE "${HEXFILE_DIR}/${COMPRESSED_TARGET_NAME}.elf")
    set(COMPRESSED_HEX_FILE "${HEXFILE_DIR}/${COMPRESSED_TARGET_NAME}.hex")
    set(COMPRESSED_BIN_FILE "${HEXFILE_DIR}/${COMPRESSED_TARGET_NAME}.bin")
    set(COMPRESSED_DFU_FILE "${HEXFILE_DIR}/${COMPRESSED_TARGET_NAME}.dfu")

    # Create a dummy source file for the compressed executable
    set(COMPRESSED_DUMMY_SOURCE "${CMAKE_BINARY_DIR}/dummy_${COMPRESSED_TARGET_NAME}.c")
    file(WRITE ${COMPRESSED_DUMMY_SOURCE} "// Dummy source file for ${COMPRESSED_TARGET_NAME}\n")

    add_executable(${COMPRESSED_TARGET_NAME} ${COMPRESSED_DUMMY_SOURCE})

    # Same settings as regular compressed target
    set_target_properties(${COMPRESSED_TARGET_NAME} PROPERTIES
        OUTPUT_NAME "${COMPRESSED_TARGET_NAME}.elf"
        RUNTIME_OUTPUT_DIRECTORY "${HEXFILE_DIR}"
        LINK_DEPENDS "${LDSCRIPT}"
    )

    # Link against the object libraries
    target_link_libraries(${COMPRESSED_TARGET_NAME} PRIVATE
        ironos_core
        ironos_threads
        ironos_drivers
        ironos_bsp
        ironos_middlewares
        ironos_brieflz
    )

    # Link Pinecilv2 binary blob libraries if building for Pinecilv2
    if(MODEL IN_LIST ALL_PINECIL_V2_MODELS)
        target_link_libraries(${COMPRESSED_TARGET_NAME} PRIVATE ${PINECILV2_LIBRARIES})
    endif()

    # Apply compressed multi-language compilation settings
    target_compile_options(${COMPRESSED_TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${GLOBAL_DEFINES}
        ${DEV_CFLAGS}
        ${DEV_CXXFLAGS}
        -MMD
        -g3
    )

    target_compile_options(${COMPRESSED_TARGET_NAME} PRIVATE
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

    target_link_options(${COMPRESSED_TARGET_NAME} PRIVATE
        ${CPU_FLAGS}
        ${OPTIMIZATION_FLAGS}
        ${LINKER_FLAGS}
        -T${LDSCRIPT}
        -Wl,-Map=${COMPRESSED_ELF_FILE}.map
    )

    target_include_directories(${COMPRESSED_TARGET_NAME} PRIVATE ${IRONOS_INCLUDE_DIRS})

    # Add translation object file to link
    target_link_options(${COMPRESSED_TARGET_NAME} PRIVATE
        "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_multi.${GROUP_CODE}.o"
        "${OUTPUT_DIR}/Core/LangSupport/lang_multi.o"
    )

    add_dependencies(${COMPRESSED_TARGET_NAME}
        generate_translation_brieflz_multi_${GROUP_CODE}
        compile_translation_brieflz_multi_${GROUP_CODE}
        generate_lang_multi
    )

    # Create compressed output files
    add_custom_command(
        OUTPUT ${COMPRESSED_HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${COMPRESSED_ELF_FILE} ${COMPRESSED_HEX_FILE}
        DEPENDS ${COMPRESSED_TARGET_NAME}
        COMMENT "Creating compressed multi-language HEX file for ${COMPRESSED_TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${COMPRESSED_BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${COMPRESSED_ELF_FILE} ${COMPRESSED_BIN_FILE}
        COMMAND ${CMAKE_SIZE} ${COMPRESSED_ELF_FILE}
        DEPENDS ${COMPRESSED_TARGET_NAME}
        COMMENT "Creating compressed multi-language BIN file for ${COMPRESSED_TARGET_NAME}"
    )

    add_custom_command(
        OUTPUT ${COMPRESSED_DFU_FILE}
        COMMAND ${HOST_PYTHON} ${CMAKE_SOURCE_DIR}/dfuse-pack.py
            -b ${DEVICE_DFU_ADDRESS}@0:${COMPRESSED_BIN_FILE}
            -D ${DEVICE_DFU_VID_PID}
            ${COMPRESSED_DFU_FILE}
        DEPENDS ${COMPRESSED_BIN_FILE}
        COMMENT "Creating compressed multi-language DFU file for ${COMPRESSED_TARGET_NAME}"
    )

    add_custom_target(${COMPRESSED_TARGET_NAME}.hex DEPENDS ${COMPRESSED_HEX_FILE})
    add_custom_target(${COMPRESSED_TARGET_NAME}.bin DEPENDS ${COMPRESSED_BIN_FILE})
    add_custom_target(${COMPRESSED_TARGET_NAME}.dfu DEPENDS ${COMPRESSED_DFU_FILE})
    add_custom_target(firmware-multi-compressed-${GROUP_NAME}
        DEPENDS ${COMPRESSED_HEX_FILE} ${COMPRESSED_BIN_FILE} ${COMPRESSED_DFU_FILE}
    )
endfunction()

# Create firmware targets for common languages
create_firmware_target("EN")

# Create compressed variants for EN
create_compressed_string_target("EN")
create_compressed_font_target("EN")

# Create targets for all other languages
foreach(LANG ${ALL_LANGUAGES})
    if(NOT LANG STREQUAL "EN")
        create_firmware_target(${LANG})
        create_compressed_string_target(${LANG})
        create_compressed_font_target(${LANG})
    endif()
endforeach()

# Create multi-language targets
foreach(GROUP ${LANGUAGE_GROUPS})
    set(GROUP_NAME_VAR "LANGUAGE_GROUP_${GROUP}_NAME")
    set(GROUP_LANGS_VAR "LANGUAGE_GROUP_${GROUP}_LANGS")
    if(DEFINED ${GROUP_NAME_VAR} AND DEFINED ${GROUP_LANGS_VAR})
        create_multi_language_target(${GROUP} ${${GROUP_NAME_VAR}} "${${GROUP_LANGS_VAR}}")
    endif()
endforeach()
