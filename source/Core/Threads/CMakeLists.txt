include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(sources 
    GUIThread.cpp
    MOVThread.cpp
    PIDThread.cpp
    POWThread.cpp
    shared.cpp
)


add_library(threads ${sources})
target_include_directories(threads PUBLIC .)
target_link_libraries(threads PRIVATE mainSource drivers BSP FreeRTOS  languages BSPImplementation )
