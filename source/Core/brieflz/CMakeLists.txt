include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(sources 
brieflz.c
depack.c
)


add_library(brieflz ${sources})
target_include_directories(brieflz PUBLIC .)
