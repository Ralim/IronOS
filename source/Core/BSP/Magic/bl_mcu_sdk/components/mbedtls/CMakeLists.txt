################# Add global include #################
list(APPEND ADD_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}/library"
"${CMAKE_CURRENT_SOURCE_DIR}/include/psa"
"${CMAKE_CURRENT_SOURCE_DIR}/include"
"${CMAKE_CURRENT_SOURCE_DIR}/bflb_port/inc"
)
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################

############## Add current dir source files ###########
list(APPEND ADD_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/bflb_port/src/bflb_aes.c"
"${CMAKE_CURRENT_SOURCE_DIR}/bflb_port/src/bflb_ccm.c"
"${CMAKE_CURRENT_SOURCE_DIR}/library/sha256.c"
"${CMAKE_CURRENT_SOURCE_DIR}/library/platform_util.c"
)


# aux_source_directory(src ADD_SRCS)
# list(REMOVE_ITEM ADD_SRCS "${CMAKE_CURRENT_SOURCE_DIR}")
#######################################################

########### Add required/dependent components #########
list(APPEND ADD_REQUIREMENTS ${CHIP}_driver)
#######################################################

############ Add static libs ##########################
# if(CONFIG_COMPONENT1_INCLUDE_STATIC_LIB)
#     list(APPEND ADD_STATIC_LIB "libxxx.a")
# endif()
#######################################################

############ Add dynamic libs #########################
# list(APPEND ADD_DYNAMIC_LIB "libxxx.so"
# )
#######################################################

############ Add global compile option ################
#add components denpend on this component
#list(APPEND ADD_DEFINITIONS -DMBEDTLS_CONFIG_FILE='<mbedtls_bflb_config.h>')
list(APPEND ADD_DEFINITIONS -DMBEDTLS_CONFIG_FILE="mbedtls_bflb_config.h")
#list(APPEND ADD_DEFINITIONS -DMBEDTLS_CONFIG_FILE="mbedtls_bflb_config.h")
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_DEFINITIONS_PRIVATE -Dxxx)
#######################################################

generate_library()
