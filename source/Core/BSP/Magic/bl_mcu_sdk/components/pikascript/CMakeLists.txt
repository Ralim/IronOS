################# Add global include #################
list(APPEND ADD_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}/pikascript-core"
"${CMAKE_CURRENT_SOURCE_DIR}/pikascript-api"
)
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################

############## Add current dir source files ###########
file(GLOB_RECURSE sources
"${CMAKE_CURRENT_SOURCE_DIR}/pikascript-core/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/pikascript-api/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/pikascript-lib/*.c"
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
# list(APPEND ADD_DEFINITIONS -D${CHIPNAME})
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_PRIVATE_DEFINITIONS -Dxxx)
#######################################################

generate_library()
