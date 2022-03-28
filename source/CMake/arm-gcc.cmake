# which compilers to use for C and C++
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_C_COMPILER arm-none-eabi-gcc  CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER arm-none-eabi-g++ CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc -x assembler-with-cpp CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE INTERNAL "Object Copy")
set(CMAKE_OBJDUMP arm-none-eabi-objdump CACHE INTERNAL "Object Dump")


