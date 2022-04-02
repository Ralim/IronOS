################# Add global include #################
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/bl702_rf/include")
list(APPEND ADD_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/lmac154/include")
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################

############## Add current dir source files ###########
add_library(lmac154 STATIC IMPORTED)
set_target_properties(lmac154 PROPERTIES
IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/components/mac154/lmac154/lib/liblmac154.a"
INTERFACE_LINK_LIBRARIES "${CMAKE_SOURCE_DIR}/components/mac154/bl702_rf/lib/libbl702_rf.a")
#######################################################

########### Add required/dependent components #########
list(APPEND ADD_REQUIREMENTS lmac154)
#######################################################

############ Add static libs ##########################
# list(APPEND ADD_STATIC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/bl702_rf/lib/libbl702_rf.a")
# list(APPEND ADD_STATIC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lmac154/lib/liblmac154.a")
#######################################################

############ Add dynamic libs #########################
# list(APPEND ADD_DYNAMIC_LIB "libxxx.so"
# )
#######################################################

############ Add global compile option ################
#add components denpend on this component
# list(APPEND ADD_DEFINITIONS -Dxxx)
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_PRIVATE_DEFINITIONS -Dxxx)
#######################################################

generate_library()
