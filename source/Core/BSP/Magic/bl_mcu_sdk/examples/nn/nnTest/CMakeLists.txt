set(mains main.cc)
set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)

file(GLOB_RECURSE ref
"${CMAKE_CURRENT_SOURCE_DIR}/Ref_Implementations/*.c"
)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/platform/cpp_new.cpp ${ref})
set(TARGET_REQUIRED_LIBS nmsis)
set(TARGET_REQUIRED_PRIVATE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Ref_Implementations)
generate_bin()

