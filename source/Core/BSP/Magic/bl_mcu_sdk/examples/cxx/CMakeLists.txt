set(BSP_COMMON_DIR ${CMAKE_SOURCE_DIR}/bsp/bsp_common)
set(TARGET_REQUIRED_SRCS ${BSP_COMMON_DIR}/platform/cpp_new.cpp)
set(mains main.cc)
generate_bin()



