# Translation targets configuration for IronOS
# This file creates custom commands for generating translation files

# Create the Core/Gen directory in Objects folder, not source tree
file(MAKE_DIRECTORY "${OUTPUT_DIR}/Core/Gen")
file(MAKE_DIRECTORY "${OUTPUT_DIR}/Core/Gen/translation.files")

# Host tools for building brieflz library
set(HOST_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Objects/host")
file(MAKE_DIRECTORY "${HOST_OUTPUT_DIR}/brieflz")

# Build host brieflz shared library
add_custom_command(
    OUTPUT "${HOST_OUTPUT_DIR}/brieflz/libbrieflz.so"
    COMMAND ${HOST_CC} -fPIC -shared -DBLZ_DLL -DBLZ_DLL_EXPORTS -O
        "${CMAKE_SOURCE_DIR}/Core/brieflz/brieflz.c"
        "${CMAKE_SOURCE_DIR}/Core/brieflz/depack.c"
        -o "${HOST_OUTPUT_DIR}/brieflz/libbrieflz.so"
    DEPENDS
        "${CMAKE_SOURCE_DIR}/Core/brieflz/brieflz.c"
        "${CMAKE_SOURCE_DIR}/Core/brieflz/depack.c"
    COMMENT "Building host brieflz shared library"
)

add_custom_target(build_host_brieflz
    DEPENDS "${HOST_OUTPUT_DIR}/brieflz/libbrieflz.so"
)

# Generate macros.txt file using shell script
add_custom_command(
    OUTPUT "${OUTPUT_DIR}/Core/Gen/macros.txt"
    COMMAND bash "${CMAKE_SOURCE_DIR}/cmake/generate_macros.sh"
        "${CMAKE_C_COMPILER}"
        "${OUTPUT_DIR}/Core/Gen"
        "${CMAKE_SOURCE_DIR}"
        "${DEVICE_BSP_DIR}"
        ${GLOBAL_DEFINES}
        "--"
        ${CPU_FLAGS}
    COMMENT "Generating macros.txt"
)

add_custom_target(generate_macros
    DEPENDS "${OUTPUT_DIR}/Core/Gen/macros.txt"
)

# Function to create translation generation targets for a single language
function(create_translation_target LANGUAGE)
    set(TRANSLATION_JSON "../Translations/translation_${LANGUAGE}.json")
    set(TRANSLATION_CPP "${OUTPUT_DIR}/Core/Gen/Translation.${LANGUAGE}.cpp")
    set(PICKLE_FILE "${OUTPUT_DIR}/Core/Gen/translation.files/${LANGUAGE}.pickle")

    # Generate translation C++ file and pickle
    add_custom_command(
        OUTPUT "${TRANSLATION_CPP}" "${PICKLE_FILE}"
        COMMAND ${HOST_PYTHON} "${CMAKE_SOURCE_DIR}/../Translations/make_translation.py"
            --macros "${OUTPUT_DIR}/Core/Gen/macros.txt"
            -o "${TRANSLATION_CPP}"
            --output-pickled "${PICKLE_FILE}"
            ${LANGUAGE}
        DEPENDS
            "${CMAKE_SOURCE_DIR}/${TRANSLATION_JSON}"
            "${CMAKE_SOURCE_DIR}/../Translations/make_translation.py"
            "${CMAKE_SOURCE_DIR}/../Translations/translations_definitions.json"
            "${CMAKE_SOURCE_DIR}/../Translations/font_tables.py"
            "${CMAKE_SOURCE_DIR}/../Translations/wqy-bitmapsong/wenquanyi_9pt.bdf"
            "${OUTPUT_DIR}/Core/Gen/macros.txt"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating translations for language ${LANGUAGE}"
    )

    add_custom_target(generate_translation_${LANGUAGE}
        DEPENDS "${TRANSLATION_CPP}" "${PICKLE_FILE}"
    )
    add_dependencies(generate_translation_${LANGUAGE} generate_macros)

    # Compile translation object file (without LTO for compression)
    add_custom_command(
        OUTPUT "${OUTPUT_DIR}/Core/Gen/translation.files/${LANGUAGE}.o"
        COMMAND ${CMAKE_CXX_COMPILER} -c
            ${CPU_FLAGS}
            ${GLOBAL_DEFINES}
            ${DEV_CXXFLAGS}
            -I"${CMAKE_SOURCE_DIR}/Core/Inc"
            -I"${CMAKE_SOURCE_DIR}/Core/BSP"
            -I"${CMAKE_SOURCE_DIR}/${DEVICE_BSP_DIR}"
            ${WARNING_FLAGS_COMMON}
            -std=c++17
            -fno-rtti
            -fno-exceptions
            -fno-non-call-exceptions
            -fno-use-cxa-atexit
            -fno-strict-aliasing
            -fno-threadsafe-statics
            -Os
            -fno-jump-tables
            -foptimize-strlen
            -faggressive-loop-optimizations
            -fdevirtualize-at-ltrans
            -fmerge-all-constants
            -fshort-wchar
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
            -fsingle-precision-constant
            "${TRANSLATION_CPP}"
            -o "${OUTPUT_DIR}/Core/Gen/translation.files/${LANGUAGE}.o"
        DEPENDS "${TRANSLATION_CPP}"
        COMMENT "Compiling translation object for ${LANGUAGE}"
    )

    add_custom_target(compile_translation_${LANGUAGE}
        DEPENDS "${OUTPUT_DIR}/Core/Gen/translation.files/${LANGUAGE}.o"
    )
    add_dependencies(compile_translation_${LANGUAGE} generate_translation_${LANGUAGE})

    # Compile main translation object file that the firmware target expects
    add_custom_command(
        OUTPUT "${OUTPUT_DIR}/Core/Gen/Translation.${LANGUAGE}.o"
        COMMAND ${CMAKE_CXX_COMPILER} -c
            ${CPU_FLAGS}
            ${OPTIMIZATION_FLAGS}
            ${GLOBAL_DEFINES}
            ${DEV_CXXFLAGS}
            -I"${CMAKE_SOURCE_DIR}/Core/Inc"
            -I"${CMAKE_SOURCE_DIR}/Core/BSP"
            -I"${CMAKE_SOURCE_DIR}/${DEVICE_BSP_DIR}"
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
            "${TRANSLATION_CPP}"
            -o "${OUTPUT_DIR}/Core/Gen/Translation.${LANGUAGE}.o"
        DEPENDS "${TRANSLATION_CPP}"
        COMMENT "Compiling main translation object for ${LANGUAGE}"
    )

    add_custom_target(compile_main_translation_${LANGUAGE}
        DEPENDS "${OUTPUT_DIR}/Core/Gen/Translation.${LANGUAGE}.o"
    )
    add_dependencies(compile_main_translation_${LANGUAGE} generate_translation_${LANGUAGE})

    # Generate compressed translation (BriefLZ string compression)
    set(BRIEFLZ_CPP "${OUTPUT_DIR}/Core/Gen/Translation_brieflz.${LANGUAGE}.cpp")
    add_custom_command(
        OUTPUT "${BRIEFLZ_CPP}"
        COMMAND ${CMAKE_COMMAND} -E env "OBJCOPY=${CMAKE_OBJCOPY}"
            ${HOST_PYTHON} "${CMAKE_SOURCE_DIR}/../Translations/make_translation.py"
            --macros "${OUTPUT_DIR}/Core/Gen/macros.txt"
            -o "${BRIEFLZ_CPP}"
            --input-pickled "${PICKLE_FILE}"
            --strings-obj "${OUTPUT_DIR}/Core/Gen/translation.files/${LANGUAGE}.o"
            ${LANGUAGE}
        DEPENDS
            "${OUTPUT_DIR}/Core/Gen/translation.files/${LANGUAGE}.o"
            "${PICKLE_FILE}"
            "${HOST_OUTPUT_DIR}/brieflz/libbrieflz.so"
            "${OUTPUT_DIR}/Core/Gen/macros.txt"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating BriefLZ compressed translation for ${LANGUAGE}"
    )

    add_custom_target(generate_translation_brieflz_${LANGUAGE}
        DEPENDS "${BRIEFLZ_CPP}"
    )
    add_dependencies(generate_translation_brieflz_${LANGUAGE}
        compile_translation_${LANGUAGE}
        build_host_brieflz
    )

    # Compile compressed translation
    add_custom_command(
        OUTPUT "${OUTPUT_DIR}/Core/Gen/Translation_brieflz.${LANGUAGE}.o"
        COMMAND ${CMAKE_CXX_COMPILER} -c
            ${CPU_FLAGS}
            ${OPTIMIZATION_FLAGS}
            ${GLOBAL_DEFINES}
            ${DEV_CXXFLAGS}
            -I"${CMAKE_SOURCE_DIR}/Core/Inc"
            -I"${CMAKE_SOURCE_DIR}/Core/BSP"
            -I"${CMAKE_SOURCE_DIR}/${DEVICE_BSP_DIR}"
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
            "${BRIEFLZ_CPP}"
            -o "${OUTPUT_DIR}/Core/Gen/Translation_brieflz.${LANGUAGE}.o"
        DEPENDS "${BRIEFLZ_CPP}"
        COMMENT "Compiling compressed translation for ${LANGUAGE}"
    )

    # Generate compressed font translation
    set(BRIEFLZ_FONT_CPP "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_font.${LANGUAGE}.cpp")
    add_custom_command(
        OUTPUT "${BRIEFLZ_FONT_CPP}"
        COMMAND ${HOST_PYTHON} "${CMAKE_SOURCE_DIR}/../Translations/make_translation.py"
            --macros "${OUTPUT_DIR}/Core/Gen/macros.txt"
            -o "${BRIEFLZ_FONT_CPP}"
            --input-pickled "${PICKLE_FILE}"
            --compress-font
            ${LANGUAGE}
        DEPENDS
            "${PICKLE_FILE}"
            "${HOST_OUTPUT_DIR}/brieflz/libbrieflz.so"
            "${OUTPUT_DIR}/Core/Gen/macros.txt"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating BriefLZ compressed translation font for ${LANGUAGE}"
    )

    add_custom_target(generate_translation_brieflz_font_${LANGUAGE}
        DEPENDS "${BRIEFLZ_FONT_CPP}"
    )
    add_dependencies(generate_translation_brieflz_font_${LANGUAGE}
        generate_translation_${LANGUAGE}
        build_host_brieflz
    )

    # Compile compressed font translation
    add_custom_command(
        OUTPUT "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_font.${LANGUAGE}.o"
        COMMAND ${CMAKE_CXX_COMPILER} -c
            ${CPU_FLAGS}
            ${OPTIMIZATION_FLAGS}
            ${GLOBAL_DEFINES}
            ${DEV_CXXFLAGS}
            -I"${CMAKE_SOURCE_DIR}/Core/Inc"
            -I"${CMAKE_SOURCE_DIR}/Core/BSP"
            -I"${CMAKE_SOURCE_DIR}/${DEVICE_BSP_DIR}"
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
            "${BRIEFLZ_FONT_CPP}"
            -o "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_font.${LANGUAGE}.o"
        DEPENDS "${BRIEFLZ_FONT_CPP}"
        COMMENT "Compiling compressed font translation for ${LANGUAGE}"
    )
endfunction()

# Function to create multi-language translation targets
function(create_multi_translation_target GROUP_CODE GROUP_NAME LANGUAGES)
    # Convert language list to space-separated string for command line
    string(REPLACE ";" " " LANGUAGES_STR "${LANGUAGES}")

    set(MULTI_CPP "${OUTPUT_DIR}/Core/Gen/Translation_multi.${GROUP_CODE}.cpp")
    set(MULTI_PICKLE "${OUTPUT_DIR}/Core/Gen/translation.files/multi.${GROUP_CODE}.pickle")

    # Prepare dependency list for translation JSON files
    set(TRANSLATION_DEPS "")
    foreach(LANG ${LANGUAGES})
        list(APPEND TRANSLATION_DEPS "${CMAKE_SOURCE_DIR}/../Translations/translation_${LANG}.json")
    endforeach()

    # Generate multi-language translation
    add_custom_command(
        OUTPUT "${MULTI_CPP}" "${MULTI_PICKLE}"
        COMMAND ${HOST_PYTHON} "${CMAKE_SOURCE_DIR}/../Translations/make_translation.py"
            --macros "${OUTPUT_DIR}/Core/Gen/macros.txt"
            -o "${MULTI_CPP}"
            --output-pickled "${MULTI_PICKLE}"
            ${LANGUAGES_STR}
        DEPENDS
            ${TRANSLATION_DEPS}
            "${CMAKE_SOURCE_DIR}/../Translations/make_translation.py"
            "${CMAKE_SOURCE_DIR}/../Translations/translations_definitions.json"
            "${CMAKE_SOURCE_DIR}/../Translations/font_tables.py"
            "${CMAKE_SOURCE_DIR}/../Translations/wqy-bitmapsong/wenquanyi_9pt.bdf"
            "${OUTPUT_DIR}/Core/Gen/macros.txt"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating translations for multi-language ${GROUP_NAME}"
    )

    add_custom_target(generate_translation_multi_${GROUP_CODE}
        DEPENDS "${MULTI_CPP}" "${MULTI_PICKLE}"
    )
    add_dependencies(generate_translation_multi_${GROUP_CODE} generate_macros)

    # Compile multi-language translation object
    add_custom_command(
        OUTPUT "${OUTPUT_DIR}/Core/Gen/translation.files/multi.${GROUP_CODE}.o"
        COMMAND ${CMAKE_CXX_COMPILER} -c
            ${CPU_FLAGS}
            ${GLOBAL_DEFINES}
            ${DEV_CXXFLAGS}
            -I"${CMAKE_SOURCE_DIR}/Core/Inc"
            -I"${CMAKE_SOURCE_DIR}/Core/BSP"
            -I"${CMAKE_SOURCE_DIR}/${DEVICE_BSP_DIR}"
            ${WARNING_FLAGS_COMMON}
            -std=c++17
            -fno-rtti
            -fno-exceptions
            -fno-non-call-exceptions
            -fno-use-cxa-atexit
            -fno-strict-aliasing
            -fno-threadsafe-statics
            -Os
            -fno-jump-tables
            -foptimize-strlen
            -faggressive-loop-optimizations
            -fdevirtualize-at-ltrans
            -fmerge-all-constants
            -fshort-wchar
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
            -fsingle-precision-constant
            "${MULTI_CPP}"
            -o "${OUTPUT_DIR}/Core/Gen/translation.files/multi.${GROUP_CODE}.o"
        DEPENDS "${MULTI_CPP}"
        COMMENT "Compiling multi-language translation for ${GROUP_NAME}"
    )

    add_custom_target(compile_translation_multi_${GROUP_CODE}
        DEPENDS "${OUTPUT_DIR}/Core/Gen/translation.files/multi.${GROUP_CODE}.o"
    )
    add_dependencies(compile_translation_multi_${GROUP_CODE} generate_translation_multi_${GROUP_CODE})

    # Generate compressed multi-language translation
    set(MULTI_BRIEFLZ_CPP "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_multi.${GROUP_CODE}.cpp")
    add_custom_command(
        OUTPUT "${MULTI_BRIEFLZ_CPP}"
        COMMAND ${CMAKE_COMMAND} -E env "OBJCOPY=${CMAKE_OBJCOPY}"
            ${HOST_PYTHON} "${CMAKE_SOURCE_DIR}/../Translations/make_translation.py"
            --macros "${OUTPUT_DIR}/Core/Gen/macros.txt"
            -o "${MULTI_BRIEFLZ_CPP}"
            --input-pickled "${MULTI_PICKLE}"
            --strings-obj "${OUTPUT_DIR}/Core/Gen/translation.files/multi.${GROUP_CODE}.o"
            --compress-font
            ${LANGUAGES_STR}
        DEPENDS
            "${OUTPUT_DIR}/Core/Gen/translation.files/multi.${GROUP_CODE}.o"
            "${MULTI_PICKLE}"
            "${HOST_OUTPUT_DIR}/brieflz/libbrieflz.so"
            "${OUTPUT_DIR}/Core/Gen/macros.txt"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Generating BriefLZ compressed translation for multi-language ${GROUP_NAME}"
    )

    add_custom_target(generate_translation_brieflz_multi_${GROUP_CODE}
        DEPENDS "${MULTI_BRIEFLZ_CPP}"
    )
    add_dependencies(generate_translation_brieflz_multi_${GROUP_CODE}
        compile_translation_multi_${GROUP_CODE}
        build_host_brieflz
    )

    # Compile compressed multi-language translation
    add_custom_command(
        OUTPUT "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_multi.${GROUP_CODE}.o"
        COMMAND ${CMAKE_CXX_COMPILER} -c
            ${CPU_FLAGS}
            ${OPTIMIZATION_FLAGS}
            ${GLOBAL_DEFINES}
            ${DEV_CXXFLAGS}
            -I"${CMAKE_SOURCE_DIR}/Core/Inc"
            -I"${CMAKE_SOURCE_DIR}/Core/BSP"
            -I"${CMAKE_SOURCE_DIR}/${DEVICE_BSP_DIR}"
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
            "${MULTI_BRIEFLZ_CPP}"
            -o "${OUTPUT_DIR}/Core/Gen/Translation_brieflz_multi.${GROUP_CODE}.o"
        DEPENDS "${MULTI_BRIEFLZ_CPP}"
        COMMENT "Compiling compressed multi-language translation for ${GROUP_NAME}"
    )
endfunction()

# Create translation targets for all languages
foreach(LANG ${ALL_LANGUAGES})
    create_translation_target(${LANG})
endforeach()

# Create multi-language targets
foreach(GROUP ${LANGUAGE_GROUPS})
    set(GROUP_NAME_VAR "LANGUAGE_GROUP_${GROUP}_NAME")
    set(GROUP_LANGS_VAR "LANGUAGE_GROUP_${GROUP}_LANGS")
    if(DEFINED ${GROUP_NAME_VAR} AND DEFINED ${GROUP_LANGS_VAR})
        create_multi_translation_target(${GROUP} ${${GROUP_NAME_VAR}} "${${GROUP_LANGS_VAR}}")
    endif()
endforeach()
