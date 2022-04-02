function(generate_library)
    get_filename_component(library_name ${CMAKE_CURRENT_LIST_DIR} NAME)
    message(STATUS "[register library component: ${library_name}], path:${CMAKE_CURRENT_LIST_DIR}")

    # Add src to lib
    if(ADD_SRCS)
        set(library_scope PUBLIC)
        add_library(${library_name} STATIC)

        foreach(SRC ${ADD_SRCS})
        if(IS_DIRECTORY ${SRC})
            message(FATAL_ERROR "ADD_SRCS cannot be a directory")
        else()
            target_sources(${library_name} PRIVATE ${SRC})
        endif()
        endforeach()

    else()
        set(library_scope INTERFACE)
        add_library(${library_name} INTERFACE)
    endif()

    # Add global config include
    if(ADD_INCLUDE)
        foreach(include_dir ${ADD_INCLUDE})
            get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
            if(NOT IS_DIRECTORY ${abs_dir})
                message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: ${include_dir} not found!")
            endif()
            target_include_directories(${library_name} ${library_scope} ${abs_dir})
        endforeach()
    endif()

    # Add private include
    if(ADD_PRIVATE_INCLUDE)
        foreach(include_dir ${ADD_PRIVATE_INCLUDE})
            get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
            if(NOT IS_DIRECTORY ${abs_dir})
                message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: ${include_dir} not found!")
            endif()
            target_include_directories(${library_name} PRIVATE ${abs_dir})
        endforeach()
    endif()

    # Add definitions public
    if(ADD_DEFINITIONS)
        target_compile_options(${library_name} ${library_scope} ${ADD_DEFINITIONS})
    endif()

    # Add definitions private
    if(ADD_PRIVATE_DEFINITIONS)
        target_compile_options(${library_name} PRIVATE ${ADD_DEFINITIONS_PRIVATE})
    endif()

    # Add requirements
    if(ADD_REQUIREMENTS)
        target_link_libraries(${library_name} ${library_scope} ${ADD_REQUIREMENTS})
    endif()

    # Add static lib
    if(ADD_STATIC_LIB)
        foreach(lib ${ADD_STATIC_LIB})
            if(NOT EXISTS "${lib}")
                message(FATAL_ERROR "Can not find ${lib}")
            endif()
            get_filename_component(static_lib_relative_dir ${lib} DIRECTORY)
            get_filename_component(static_lib_name ${lib} NAME)
            target_link_directories(${library_name} ${library_scope} ${static_lib_relative_dir})
            target_link_libraries(${library_name} ${library_scope} ${static_lib_name})
        endforeach()
    endif()

    # Add dynamic lib
    if(ADD_DYNAMIC_LIB)
        foreach(lib ${ADD_DYNAMIC_LIB})
            if(NOT EXISTS "${lib}")
                message(FATAL_ERROR "Can not find ${lib}")
            endif()
            get_filename_component(dynamic_lib_relative_dir ${lib} DIRECTORY)
            get_filename_component(dynamic_lib_name ${lib} NAME)
            target_link_directories(${library_name} ${library_scope} ${dynamic_lib_relative_dir})
            target_link_libraries(${library_name} ${library_scope} ${dynamic_lib_name})
        endforeach()
    endif()
endfunction()

function(generate_bin)

    get_filename_component(current_dir_name ${CMAKE_CURRENT_LIST_DIR} NAME)
    string(REGEX REPLACE "(.*)/${current_dir_name}$" "\\1" above_absolute_dir ${CMAKE_CURRENT_LIST_DIR})
    get_filename_component(above_dir_name ${above_absolute_dir} NAME)

    # Add common options
    add_compile_options(${GLOBAL_C_FLAGS})
    add_compile_options(-D${BOARD})
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-std=c99>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-nostdlib>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)

    # add basic library which is built by add_library
    check_add_library(common ${CMAKE_SOURCE_DIR}/common)
    check_add_library(${CHIP}_driver ${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver)

    # add other libraries which are target or extern library
    if(TARGET_REQUIRED_LIBS)
        file(GLOB_RECURSE lib_cmakelists_files ${CMAKE_SOURCE_DIR}/components/CMakeLists.txt)
        foreach(lib ${TARGET_REQUIRED_LIBS})
            if(${lib} MATCHES "lib.*a$")
                get_filename_component(exlib_absolute_dir ${lib} DIRECTORY)
                get_filename_component(exlibrary_name ${lib} NAME_WE)
                message(STATUS "[register extern library component: ${exlibrary_name}], path:${exlib_absolute_dir}")
            elseif(lib_cmakelists_files)

                foreach(lib_cmakelists_file ${lib_cmakelists_files})
                get_filename_component(lib_absolute_dir ${lib_cmakelists_file} DIRECTORY)
                get_filename_component(lib_absolute_dir_name ${lib_absolute_dir} NAME)

                if(${lib_absolute_dir_name} STREQUAL "${lib}")
                    check_add_library(${lib} ${lib_absolute_dir})
                endif()
                endforeach()
            else()
                message(FATAL_ERROR "${lib} is not a target or a extern library")
            endif()
        endforeach()
    endif()

    # list mains to execute,now is executing one elf
    foreach(mainfile IN LISTS mains)
    # Get file name without directory
    get_filename_component(mainname ${mainfile} NAME_WE)

	if(DEFINED OUTPUT)
        set(OUTPUT_DIR ${OUTPUT})
        set(target_name firmware)
    else()
        string(REPLACE "." ":" dot2colon ${APP_DIR})
        if(${dot2colon} MATCHES "::/") #if demo is not in sdk path
            string(REPLACE "../" "" relative_dir ${APP_DIR})
            set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${relative_dir}/${current_dir_name})
        elseif(${dot2colon} MATCHES ":")#if demo is in sdk peer path
            set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${current_dir_name})
        else()  #if demo is in sdk path not in peer path
            if(${APP_DIR} MATCHES ${above_dir_name}) #if demo has one-Layer Catalog
                set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${APP_DIR}/${current_dir_name})
            else() #if demo has Two-Layer Catalog
                set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/out/${APP_DIR}/${above_dir_name}/${current_dir_name})
            endif()
        endif()
        if(${CPU_ID} STREQUAL "none")
            set(target_name ${current_dir_name}_${CHIP})
        else()
            set(target_name ${current_dir_name}_${CHIP}_${CPU_ID})
        endif()
    endif()

	file(MAKE_DIRECTORY ${OUTPUT_DIR})
	set(HEX_FILE ${OUTPUT_DIR}/${target_name}.hex)
    set(BIN_FILE ${OUTPUT_DIR}/${target_name}.bin)
    set(MAP_FILE ${OUTPUT_DIR}/${target_name}.map)
    set(ASM_FILE ${OUTPUT_DIR}/${target_name}.asm)

    if(TARGET_REQUIRED_SRCS)
        foreach(src ${TARGET_REQUIRED_SRCS})
            if((NOT EXISTS ${src}) AND (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${src}))
            message(FATAL_ERROR "${src} not exist,maybe you should autocomplete your path\r\n")
            endif()
            list(APPEND SRCS ${src})
        endforeach()
    endif()

    list(APPEND SRCS ${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform/bflb_platform.c)
    list(APPEND SRCS ${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform/syscalls.c)
    list(APPEND SRCS ${CMAKE_SOURCE_DIR}/bsp/board/${CHIP}/board.c)

    add_executable(${target_name}.elf ${mainfile} ${SRCS})
    target_link_options(${target_name}.elf PRIVATE ${GLOBAL_LD_FLAGS})
    set_target_properties(${target_name}.elf PROPERTIES LINK_FLAGS "-T${LINKER_SCRIPT} -Wl,-Map=${MAP_FILE}")
    set_target_properties(${target_name}.elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

    set_target_properties(${target_name}.elf PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

    # Add private include
    foreach(include_dir ${TARGET_REQUIRED_PRIVATE_INCLUDE})
        get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${CMAKE_SOURCE_DIR})
        if((NOT IS_DIRECTORY ${abs_dir}) AND (NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${abs_dir}))
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: ${include_dir} not found!")
        endif()
        target_include_directories(${target_name}.elf PRIVATE ${abs_dir})
    endforeach()

    # Add definitions private
    if(TARGET_REQUIRED_PRIVATE_OPTIONS)
    target_compile_options(${target_name}.elf PRIVATE ${TARGET_REQUIRED_PRIVATE_OPTIONS})
    endif()

    add_dependencies(${target_name}.elf ${CHIP}_driver)
    # Add libs
    target_link_libraries(${target_name}.elf ${CHIP}_driver c)

    if(TARGET_REQUIRED_LIBS)
    target_link_libraries(${target_name}.elf ${TARGET_REQUIRED_LIBS})
    endif()

    target_link_libraries(${target_name}.elf m)

    add_custom_command(TARGET ${target_name}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${target_name}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} -d -S $<TARGET_FILE:${target_name}.elf> >${ASM_FILE}
        # COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${mainname}.elf> ${HEX_FILE}
        COMMAND ${SIZE} $<TARGET_FILE:${target_name}.elf>
        COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} ${CMAKE_SOURCE_DIR}/tools/bflb_flash_tool/img/project.bin
        COMMENT "Generate ${BIN_FILE}\r\nCopy ${BIN_FILE} into download path")
    endforeach()

    save_information()
endfunction(generate_bin)

function(search_application component_path)

if(DEFINED APP)

    # find CMakeLists in ${component_path}/${APP}/,${APP} is the first directory
    file(GLOB_RECURSE first_dir_cmakelists ${component_path}/${APP}/CMakeLists.txt)

    if(first_dir_cmakelists)
    list(APPEND cmakelists_files ${first_dir_cmakelists})
    endif()

    # find CMakeLists in ${component_path}/*/${APP}*/,${APP} is the second directory
    file(GLOB_RECURSE second_dir_cmakelists ${component_path}/*/${APP}*/CMakeLists.txt)

    if(second_dir_cmakelists)
    list(APPEND cmakelists_files ${second_dir_cmakelists})
    endif()

    list(REMOVE_DUPLICATES cmakelists_files)

    if(cmakelists_files)
        #build app finding
        foreach(cmakelists_file IN LISTS cmakelists_files)
        get_filename_component(app_absolute_dir ${cmakelists_file} DIRECTORY)
        get_filename_component(app_absolute_dir_name ${app_absolute_dir} NAME)
        message(STATUS "[run app:${app_absolute_dir_name}], path:${app_absolute_dir}")
        add_subdirectory(${app_absolute_dir} ${PROJECT_BINARY_DIR}/samples/${app_absolute_dir_name})
        endforeach()
    else()
    message(FATAL_ERROR "can not find ${APP} in the first or second directory under the path:${component_path}")
    endif()

else()
add_subdirectory($ENV{PROJECT_DIR}/src src)
endif()

endfunction()

function(check_add_library target_name directory) # if library do not be built, add its subdirectory and build it
    if(NOT TARGET ${target_name})
    add_subdirectory(${directory} ${PROJECT_BINARY_DIR}/libraries/${target_name})
    endif()
endfunction()

function(save_information)

execute_process(
    COMMAND git submodule status
    OUTPUT_VARIABLE GIT_SUBMODULE_INFO
    OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
    COMMAND git log -1 --pretty=oneline
    OUTPUT_VARIABLE SDK_INFO
    OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
    COMMAND ${TOOLCHAIN_PREFIX}gcc --version
    OUTPUT_VARIABLE GCC_INFO
    OUTPUT_STRIP_TRAILING_WHITESPACE)

    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/submodule_commit_info.txt "*************submodule information*********************\r\n")
    file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/submodule_commit_info.txt ${GIT_SUBMODULE_INFO})
    file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/submodule_commit_info.txt "\r\n*****************sdk information************************\r\n")
    file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/submodule_commit_info.txt ${SDK_INFO})
    file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/submodule_commit_info.txt "\r\n*****************gcc information************************\r\n")
    file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/submodule_commit_info.txt ${GCC_INFO})

endfunction(save_information)
