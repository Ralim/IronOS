################# Add global include #################
list(APPEND ADD_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}/core"
"${CMAKE_CURRENT_SOURCE_DIR}/common"
"${CMAKE_CURRENT_SOURCE_DIR}/class/cdc"
"${CMAKE_CURRENT_SOURCE_DIR}/class/hid"
"${CMAKE_CURRENT_SOURCE_DIR}/class/msc"
"${CMAKE_CURRENT_SOURCE_DIR}/class/video"
"${CMAKE_CURRENT_SOURCE_DIR}/class/audio"
"${CMAKE_CURRENT_SOURCE_DIR}/class/winusb"
)
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################

############## Add current dir source files ###########
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/core/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/class/*.c"
)
list(APPEND ADD_SRCS  ${sources})
# aux_source_directory(src ADD_SRCS)
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
if(CONFIG_USB_HS)
list(APPEND ADD_DEFINITIONS -DCONFIG_USB_HS)
endif()
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_PRIVATE_DEFINITIONS -Dxxx)
#######################################################

generate_library()
