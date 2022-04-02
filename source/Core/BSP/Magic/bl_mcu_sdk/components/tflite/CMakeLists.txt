################# Add global include #################
list(APPEND ADD_INCLUDE
"${CMAKE_CURRENT_SOURCE_DIR}"
"${CMAKE_CURRENT_SOURCE_DIR}/third_party/flatbuffers/include"
"${CMAKE_CURRENT_SOURCE_DIR}/third_party/gemmlowp"
"${CMAKE_CURRENT_SOURCE_DIR}/third_party/ruy"
)
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE
# )
#######################################################


############## Add current dir source files ###########
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/c sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/core/api sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/kernels sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/kernels/internal sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/kernels sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/memory_planner sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/schema sources)

list(APPEND ADD_SRCS  ${sources})
#######################################################

########### Add required/dependent components #########
# list(APPEND ADD_REQUIREMENTS xxx)
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
list(APPEND ADD_DEFINITIONS -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS)
list(APPEND ADD_DEFINITIONS -DTF_LITE_USE_GLOBAL_MIN)
list(APPEND ADD_DEFINITIONS -DTF_LITE_USE_GLOBAL_MAX)
list(APPEND ADD_DEFINITIONS -DTF_LITE_STATIC_MEMORY)
list(APPEND ADD_DEFINITIONS $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>)

#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules
# list(APPEND ADD_PRIVATE_DEFINITIONS)
#######################################################

generate_library()
