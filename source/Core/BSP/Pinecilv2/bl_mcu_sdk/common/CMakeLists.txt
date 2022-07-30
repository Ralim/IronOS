################# Add global include #################
list(APPEND ADD_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer"
"${CMAKE_CURRENT_SOURCE_DIR}/soft_crc"
"${CMAKE_CURRENT_SOURCE_DIR}/memheap"
"${CMAKE_CURRENT_SOURCE_DIR}/misc"
"${CMAKE_CURRENT_SOURCE_DIR}/list"
"${CMAKE_CURRENT_SOURCE_DIR}/device"
"${CMAKE_CURRENT_SOURCE_DIR}/partition"
"${CMAKE_CURRENT_SOURCE_DIR}/bl_math"
"${CMAKE_CURRENT_SOURCE_DIR}/pid"
"${CMAKE_CURRENT_SOURCE_DIR}/timestamp"
)
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################

############## Add current dir source files ###########
file(GLOB_RECURSE sources
"${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/soft_crc/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/memheap/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/misc/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/device/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/partition/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/bl_math/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/pid/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/timestamp/*.c"
)

#aux_source_directory(. sources)
list(APPEND ADD_SRCS  ${sources})
#######################################################

########### Add required/dependent components #########
#list(APPEND ADD_REQUIREMENTS xxx)
#######################################################

############ Add static libs ##########################
#list(APPEND ADD_STATIC_LIB "libxxx.a")
#######################################################

############ Add dynamic libs #########################
# list(APPEND ADD_DYNAMIC_LIB "libxxx.so"
# )
#######################################################

############ Add global compile option ################
#add components denpend on this component
string(TOUPPER ${CHIP} CHIPNAME)
list(APPEND ADD_DEFINITIONS -D${CHIPNAME})
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_PRIVATE_DEFINITIONS -Dxxx)
#######################################################

generate_library()
