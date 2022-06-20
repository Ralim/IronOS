include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(sources 
BSP_Common.c
)


add_library(BSP ${sources})
target_include_directories(BSP PUBLIC .)
target_link_libraries(BSP PUBLIC FreeRTOS)

if (MODEL STREQUAL TS100 OR MODEL STREQUAL TS80 OR MODEL STREQUAL TS80P)
    add_subdirectory(Miniware)  
elseif(MODEL STREQUAL Pinecil)
    add_subdirectory(Pine64)  
elseif(MODEL STREQUAL MHP30)
    add_subdirectory(MHP30)  
else()
 message(FATAL_ERROR "You must specify a model")
endif()
